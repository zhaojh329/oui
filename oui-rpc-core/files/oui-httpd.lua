#!/usr/bin/env eco

-- SPDX-License-Identifier: MIT
-- Author: Jianhui Zhao <zhaojh329@gmail.com>

local hex = require 'eco.encoding.hex'
local http = require 'eco.http.server'
local md5 = require 'eco.hash.md5'
local time = require 'eco.time'
local file = require 'eco.file'
local sys = require 'eco.sys'
local log = require 'eco.log'
local cjson = require 'cjson'
local uci = require 'uci'

local rpc = require 'oui.rpc'

eco.panic_hook = function(err)
    log.err('panic:', err)
end

local rpc_methods = {}

rpc_methods['challenge'] = function(con, req, params)
    if type(params.username) ~= 'string' then
        log.err('call challenge: username is required')
        return con:send_error(http.STATUS_UNAUTHORIZED)
    end

    local c = uci.cursor()
    local found = false

    c:foreach('oui', 'user', function(s)
        if s.username == params.username then
            found = true
            return false
        end
    end)

    if not found then
        return con:send_error(http.STATUS_UNAUTHORIZED)
    end

    local nonce = rpc.create_nonce()
    if not nonce then
        return con:send_error(http.STATUS_UNAUTHORIZED)
    end

    con:send(cjson.encode({ nonce = nonce }))
end

rpc_methods['login'] = function(con, req, params)
    local username = params.username
    local password = params.password

    if type(username) ~= 'string' then
        return con:send_error(http.STATUS_UNAUTHORIZED)
    end

    local acl = rpc.login(username, password)
    if not acl then
        return con:send_error(http.STATUS_UNAUTHORIZED)
    end

    local sid = rpc.create_session(username, acl, con:remote_addr().ipaddr)

    con:send(cjson.encode({ sid = sid }))
end

rpc_methods['logout'] = function(con, req, params)
    local sid = params.sid

    if type(sid) == 'string' then
        rpc.delete_session(sid)
    end
end

rpc_methods['alive'] = function(con, req, params)
    local sid = params.sid
    local alive = false

    if type(sid) == 'string' and rpc.get_session(sid) then
        alive = true
    end

    con:send(cjson.encode({ alive = alive }))
end

rpc_methods['call'] = function(con, req, params)
    local sid = params[1]
    local mod = params[2]
    local func = params[3]
    local args = params[4] or {}

    if type(sid) ~= 'string' or type(mod) ~= 'string' or type(func) ~= 'string' or type(args) ~= 'table' then
        return con:send_error(http.STATUS_BAD_REQUEST)
    end

    local session = rpc.get_session(sid) or { remote_addr = con:remote_addr().ipaddr }

    local result, err = rpc.call(mod, func, args, session)
    if type(err) == 'number' then
        if err == rpc.ERROR_CODE_INVALID_ARGUMENT then
            return con:send_error(http.STATUS_BAD_REQUEST)
        end

        if err == rpc.ERROR_CODE_NOT_FOUND then
            return con:send_error(http.STATUS_NOT_FOUND)
        end

        if err == rpc.ERROR_CODE_UNAUTHORIZED then
            return con:send_error(http.STATUS_UNAUTHORIZED)
        end

        if err == rpc.ERROR_CODE_PERMISSION_DENIED then
            return con:send_error(http.STATUS_FORBIDDEN)
        end

        return con:send_error(http.STATUS_INTERNAL_SERVER_ERROR)
    end

    if result then
        local resp = cjson.encode({ result = result }):gsub('{}','[]')
        con:send(resp)
    else
        con:send('{}')
    end
end

local function handle_rpc(con, req)
    if req.method ~= 'POST' then
        return con:send_error(http.STATUS_BAD_REQUEST)
    end

    local body, err = con:read_body()
    if not body then
        log.err('read body fail:', err)
        return con:send_error(http.STATUS_BAD_REQUEST)
    end

    local ok, msg = pcall(cjson.decode, body)
    if not ok or type(msg) ~= 'table' or type(msg.method) ~= 'string' then
        return con:send_error(http.STATUS_BAD_REQUEST)
    end

    if type(msg.params or {}) ~= 'table' then
        return con:send_error(http.STATUS_BAD_REQUEST)
    end

    if not rpc_methods[msg.method] then
        log.err('Oui: Not supported rpc method: ', msg.method)
        return con:send_error(http.STATUS_NOT_FOUND)
    end

    rpc_methods[msg.method](con, req, msg.params or {})
end

local function handle_upload(con, req)
    local part, sid, f, md5ctx
    local total = 0

    while true do
        local typ, data = con:read_formdata(req)
        if typ == 'header' then
            if data[1] == 'content-disposition' then
                part = data[2]:match('name="([%w_-]+)"')
            end
        elseif typ == 'body' then
            if part == 'sid' then
                sid = data[1]
                if not rpc.get_session(sid) then
                    return con:send_error(http.STATUS_UNAUTHORIZED)
                end
            elseif part == 'path' then
                f = io.open(data[1], 'w')
                if not f then
                    return con:send_error(http.STATUS_FORBIDDEN)
                end

                md5ctx = md5.new()
            elseif part == 'file' then
                if not f then
                    return con:send_error(http.STATUS_BAD_REQUEST)
                end

                if not sid then
                    return con:send_error(http.STATUS_UNAUTHORIZED)
                end

                f:write(data[1])

                md5ctx:update(data[1])
                total = total + #data[1]
            end
        elseif typ == 'end' then
            break
        else
            return con:send_error(http.STATUS_BAD_REQUEST)
        end
    end

    if f then f:close() end

    if not f then
        return con:send_error(http.STATUS_BAD_REQUEST)
    end

    con:send(cjson.encode({ size = total, md5 = hex.encode(md5ctx:final()) }))
end

local function handle_download(con, req)
    local path = req.query['path']
    if not path then
        return con:send_error(http.STATUS_BAD_REQUEST, 'no "path" in query')
    end

    local sid = req.query['sid']
    if not sid then
        return con:send_error(http.STATUS_BAD_REQUEST, 'no "sid" in query')
    end

    if not rpc.get_session(sid) then
        return con:send_error(http.STATUS_UNAUTHORIZED)
    end

    con:add_header('content-type', 'application/octet-stream')
    con:send_file(path)
end

local function http_handler(con, req)
    local path = req.path

    if path == '/oui-rpc' then
        handle_rpc(con, req)
    elseif path == '/oui-upload' then
        handle_upload(con, req)
    elseif path == '/oui-download' then
        handle_download(con, req)
    else
        con:serve_file(req)
    end
end

local function parse_listen_addr(str)
    local ip, port = str:match('^%[?([%d%.:]+)%]?:(%d+)$')
    if not ip then
        return nil
    end

    if ip:match('^%d+%.%d+%.%d+%.%d+$') then
        return ip, tonumber(port)
    elseif ip:match('^%[?[%x:]+%]?$') then
        return ip:gsub('^%[?([a-fA-F0-9:]+)%]?$', '%1'), tonumber(port), true
    else
        return nil
    end
end

local function listen_http(addr, global_options, ssl)
    local options = {}

    for k, v in pairs(global_options) do
        options[k] = v
    end

    local ipaddr, port, ipv6 = parse_listen_addr(addr)
    if not ipaddr then
        log.err('invalid addr:' .. addr)
        return
    end

    options.ipv6 = ipv6

    if ssl then
        log.info('listen https:', addr)
    else
        log.info('listen http:', addr)
    end

    eco.run(function()
        local srv, err = http.listen(ipaddr, port, options, http_handler)
        if not srv then
            log.err(err)
            return
        end
    end)
end

local function parse_config()
    local c = uci.cursor()

    local options = {
        docroot = '/www',
        index = 'oui.html',
        reuseaddr = true,
        gzip = true
    }

    c:foreach('oui', 'httpd', function(s)
        log.set_level(log[s.log_level or 'INFO'])

        if s.log_path then
            log.set_path(s.log_path)
        end

        options.http_keepalive = tonumber(s.http_keepalive or 0)
        options.tcp_keepalive = tonumber(s.tcp_keepalive or 0)

        for _, addr in ipairs(s.listen_http or {}) do
            listen_http(addr, options)
        end

        local cert = '/etc/oui-httpd.crt'
        local key = '/etc/oui-httpd.key'

        if s.listen_https and file.access(cert) and file.access(key) then
            options.cert = cert
            options.key = key

            for _, addr in ipairs(s.listen_https) do
                listen_http(addr, options, true)
            end
        end

        return false
    end)
end

rpc.init()
rpc.load_acl()

parse_config()

sys.signal(sys.SIGINT, function()
    log.info('\nGot SIGINT, now quit')
    eco.unloop()
end)

sys.signal(sys.SIGTERM, function()
    log.info('\nGot SIGTERM, now quit')
    eco.unloop()
end)

while true do
    time.sleep(1)
end

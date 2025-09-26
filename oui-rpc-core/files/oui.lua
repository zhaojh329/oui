#!/usr/bin/env eco

-- SPDX-License-Identifier: MIT
-- Author: Jianhui Zhao <zhaojh329@gmail.com>

local hex = require 'eco.encoding.hex'
local socket = require 'eco.socket'
local md5 = require 'eco.hash.md5'
local file = require 'eco.file'
local ubus = require 'eco.ubus'
local time = require 'eco.time'
local sys = require 'eco.sys'
local log = require 'eco.log'
local cjson = require 'cjson'
local uci = require 'eco.uci'

local rpc = require 'oui.rpc'

eco.panic_hook = function(err)
    os.remove('/var/run/oui-rpc.sock')
    log.err('panic:', err)
end

local function trace_hook(event)
    local info3 = debug.getinfo(3, 'Sl')

    if not info3 then
        return
    end

    local info2 = debug.getinfo(2, 'nf')

    local src = info3.short_src

    if info3.currentline ~= -1 then
        src = src .. ':' .. info3.currentline
    end

    log.info(event:upper(), info2.name or info2.func, src)
end

local function set_trace()
    for _, co in ipairs(eco.all()) do
        debug.sethook(co, trace_hook, 'rc')
    end
end

if arg[1] == '-trace' then
    set_trace()
end

local function init_signal()
    sys.signal(sys.SIGINT, function()
        log.info('\nGot SIGINT, now quit')
        eco.unloop()
    end)

    sys.signal(sys.SIGTERM, function()
        log.info('\nGot SIGTERM, now quit')
        eco.unloop()
    end)
end

local function parse_config()
    local c = uci.cursor()

    log.set_ident('oui')
    log.set_flags(log.FLAG_LF | log.FLAG_PATH)

    c:foreach('oui', 'global', function(s)
        log.set_level(log[s.log_level or 'INFO'])

        if s.log_path then
            log.set_path(s.log_path)
        end

        return false
    end)
end

local function get_lighttpd_username()
    for line in io.lines('/etc/lighttpd/lighttpd.conf') do
        local username = line:match('server.username[%s]+=%s+"(.+)"')
        if username then
            return username
        end
    end
end

local rpc_methods = {}

rpc_methods['challenge'] = function(req, params)
    if type(params.username) ~= 'string' then
        log.err('call challenge: username is required')
        return 401
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
        return 401
    end

    local nonce = rpc.create_nonce()
    if not nonce then
        return 401
    end

    return { nonce = nonce }
end

rpc_methods['login'] = function(req, params)
    local username = params.username
    local password = params.password

    if type(username) ~= 'string' then
        return 401
    end

    local acl = rpc.login(username, password)
    if not acl then
        return 401
    end

    local remote_addr = req.headers['REMOTE_ADDR']

    local sid = rpc.create_session(username, acl, remote_addr)

    return { sid = sid }
end

rpc_methods['logout'] = function(req, params)
    local sid = params.sid

    if type(sid) == 'string' then
        rpc.delete_session(sid)
    end
end

rpc_methods['alive'] = function(req, params)
    local sid = params.sid
    local alive = false

    if type(sid) == 'string' and rpc.get_session(sid) then
        alive = true
    end

    return { alive = alive }
end

rpc_methods['call'] = function(req, params)
    local sid = params[1]
    local mod = params[2]
    local func = params[3]
    local args = params[4] or {}

    if type(sid) ~= 'string' or type(mod) ~= 'string' or type(func) ~= 'string' or type(args) ~= 'table' then
        return 400
    end

    local remote_addr = req.headers['REMOTE_ADDR']
    local session = rpc.get_session(sid) or { remote_addr = remote_addr }

    local result, err = rpc.call(mod, func, args, session)
    if type(err) == 'number' then
        if err == rpc.ERROR_CODE_INVALID_ARGUMENT then
            return 400
        end

        if err == rpc.ERROR_CODE_NOT_FOUND then
            return 404
        end

        if err == rpc.ERROR_CODE_UNAUTHORIZED then
            return 401
        end

        if err == rpc.ERROR_CODE_PERMISSION_DENIED then
            return 403
        end

        return 500
    end

    if result then
        return { result = result }
    end
end


local function send_scgi_status(con, status)
    con:send('Status: ' .. status .. '\r\n\r\n')
end

local function read_scgi_body(con, req)
    local headers = req.headers

    if headers['REQUEST_METHOD'] ~= 'POST' then
        return nil, 400
    end

    local content_length = tonumber(headers['CONTENT_LENGTH'])
    if content_length then
        local data, err = con:readfull(content_length)
        if not data then
            log.err('read scgi:', err)
            return nil, 400
        end

        return data
    end

    return ''
end

local function handle_rpc(con, req)
    local body, err = read_scgi_body(con, req)
    if not body then
        return err
    end

    if body == '' then
        return 400
    end

    local ok, msg = pcall(cjson.decode, body)
    if not ok or type(msg) ~= 'table' or type(msg.method) ~= 'string' then
        return 400
    end

    if type(msg.params or {}) ~= 'table' then
        return 400
    end

    if not rpc_methods[msg.method] then
        log.err('Oui: Not supported rpc method: ', msg.method)
        return 404
    end

    local res = rpc_methods[msg.method](req, msg.params or {})
    if type(res) == 'number' then
        return res
    end

    send_scgi_status(con, 200)

    if res then
        local data = cjson.encode(res)
        con:send(data:gsub('{}','[]'))
    end
end

local function handle_download(con, req)
    local query = req.query
    local path = query.path or ''
    local sid = query.sid or ''

    if path == '' then
        return 400
    end

    local s = rpc.get_session(sid)
    if not s then
        return 401
    end

    local f = io.open(path)
    if not f then
        return 404
    end

    send_scgi_status(con, 200)

    while true do
        local data = f:read(4096)
        if not data then
            break
        end

        if not con:send(data) then
            break
        end
    end

    f:close()
end

local function read_formdata(con, req, state)
    if not state.boundary then
        local content_type = req.headers['CONTENT_TYPE'] or ''
        local boundary = content_type:match('multipart/form%-data; *boundary=(----[%w%p]+)')
        if not boundary then
            return nil, 'bad request'
        end

        state.boundary = '--' .. boundary
        state.state = 'init'
    end

    if state.state == 'init' then
        local line, err = con:recv('l')
        if not line then
            return nil, err
        end

        if line ~= state.boundary .. '\r' then
            return nil, 'bad request'
        end

        state.boundary = '\r\n' .. state.boundary

        state.state = 'header'
    end

    if state.state == 'header' then
        local line, err = con:recv('l')
        if not line then
            return nil, err
        end

        if line == '\r' then
            state.state = 'body'
        else
            local name, value = line:match('([%w%p]+) *: *(.+)\r?$')
            if not name or not value then
                return nil, 'invalid http header'
            end

            return 'header', { name:lower(), value }
        end
    end

    if state.state == 'body' then
        local data, found = con:readuntil(state.boundary)
        if not data then
            return nil, found
        end

        if found then
            local x, err = con:peek(2)
            if not x then
                return nil, err
            end

            if x == '--' then
                state.state = 'end'
            else
                if x ~= '\r\n' then
                    return nil, 'bad request'
                end

                con:recv(2)

                state.state = 'header'
            end
        end

        return 'body', { data, found }
    end

    return 'end'
end

local function handle_upload(con, req)
    local headers = req.headers

    if headers['REQUEST_METHOD'] ~= 'POST' then
        return 400
    end

    local part, sid, f, md5ctx
    local total = 0
    local state = {}

    while true do
        local typ, data = read_formdata(con, req, state)
        if typ == 'header' then
            if data[1] == 'content-disposition' then
                part = data[2]:match('name="([%w_-]+)"')
            end
        elseif typ == 'body' then
            if part == 'sid' then
                sid = data[1]
                if not rpc.get_session(sid) then
                    return 401
                end
            elseif part == 'path' then
                f = io.open(data[1], 'w')
                if not f then
                    return 403
                end

                md5ctx = md5.new()
            elseif part == 'file' then
                if not f then
                    return 400
                end

                if not sid then
                    return 401
                end

                f:write(data[1])

                md5ctx:update(data[1])
                total = total + #data[1]
            end
        elseif typ == 'end' then
            break
        else
            return 400
        end
    end

    if f then f:close() end

    if not f then
        return 400
    end

    send_scgi_status(con, 200)

    con:send(cjson.encode({ size = total, md5 = hex.encode(md5ctx:final()) }))
end

local handlers = {
    ['/oui-rpc'] = handle_rpc,
    ['/oui-download'] = handle_download,
    ['/oui-upload'] = handle_upload
}

local function url_unescape(s)
    return string.gsub(s, "%%(%x%x)", function(hex)
        return string.char(tonumber(hex, 16))
    end)
end

local function handle_scgi(c)
    local con<close> = c
    local data, err = con:recvuntil(':', 3)
    if not data then
        log.err('read scgi:', err)
        return send_scgi_status(con, 400)
    end

    local length = tonumber(data)
    if not length then
        return send_scgi_status(con, 400)
    end

    data, err = con:readfull(length)
    if not data then
        log.err('read scgi:', err)
        return send_scgi_status(con, 400)
    end

    local headers = {}

    repeat
        local pos = data:find('%z')
        if not pos or pos < 2 then
            return send_scgi_status(con, 400)
        end

        local name = data:sub(1, pos - 1)

        data = data:sub(pos + 1)

        pos = data:find('%z')
        if not pos then
            return send_scgi_status(con, 400)
        end

        headers[name] = data:sub(1, pos - 1)
        data = data:sub(pos + 1)
    until #data == 0

    data, err = con:recv(1)
    if not data then
        log.err('read scgi:', err)
        return send_scgi_status(con, 400)
    end

    if data ~= ',' then
        return send_scgi_status(con, 400)
    end

    if headers['SCGI'] ~= '1' then
        return send_scgi_status(con, 400)
    end

    local req = { headers = headers }

    local query_string = headers['QUERY_STRING']
    local query = {}

    for q in query_string:gmatch('[^&]+') do
        local name, value = q:match('(.+)=(.+)')
        if name then
            name = url_unescape(name)
            query[name] = url_unescape(value)
        end
    end

    req.query = query

    local script_name = headers['SCRIPT_NAME']
    local handler = handlers[script_name]
    if not handler then
        return send_scgi_status(con, 404)
    end

    err = handler(con, req)

    if type(err) == 'number' then
        send_scgi_status(con, err)
    end
end

local function run_scgi_server()
    local path = '/var/run/oui.sock'
    local s, err = socket.listen_unix(path)
    if not s then
        error(err)
    end

    local username = get_lighttpd_username()
    if username then
        local pw = sys.getpwnam(username)
        if pw then
            file.chown(path, pw.uid)
        end
    end

    while true do
        local c, peer = s:accept()
        if not c then
            log.err('accept:', peer)
            break
        end

        eco.run(handle_scgi, c)
    end
end

local function ubus_init()
    local con, err = ubus.connect()
    if not con then
        error(err)
    end

    con:add('oui', {
        get_session = {
            function(req, msg)
                local sid = msg.sid
                if type(sid) ~= 'string' then
                    return ubus.STATUS_INVALID_ARGUMENT
                end

                local s = rpc.get_session(sid)
                if not s then
                    return ubus.STATUS_NOT_FOUND
                end

                con:reply(req, {
                    remote_addr = s.remote_addr,
                    username = s.username,
                    acls = s.acls,
                    acl = s.acl
                })
            end, { sid = ubus.STRING }
        }
    })

    while true do
        time.sleep(1000)
    end
end

rpc.init()
rpc.load_acl()

parse_config()
init_signal()

eco.run(ubus_init)

run_scgi_server()

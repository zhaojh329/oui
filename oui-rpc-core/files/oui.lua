#!/usr/bin/env eco

-- SPDX-License-Identifier: MIT
-- Author: Jianhui Zhao <zhaojh329@gmail.com>

local hex = require 'eco.encoding.hex'
local socket = require 'eco.socket'
local md5 = require 'eco.hash.md5'
local file = require 'eco.file'
local sys = require 'eco.sys'
local log = require 'eco.log'
local cjson = require 'cjson'
local uci = require 'eco.uci'
local cli = require 'eco.cli'
local eco = require 'eco'

local rpc = require 'oui.rpc'

local sock_path = '/var/run/oui.sock'

local active_requests = 0
local gc_pending = false

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

    c:foreach('oui', 'oui', function(s)
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
    if err ~= nil then
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

    return { result = result == nil and cjson.null or result }
end


local scgi_headers = {}
local scgi_fields = {}

local function send_scgi_resp(con, status, body)
    local headers = scgi_headers[status]

    if body then
        con:send(headers .. body)
    else
        con:send(headers)
    end
end

local function read_scgi_body(con, req)
    local headers = req.headers

    if headers['REQUEST_METHOD'] ~= 'POST' then
        return nil, 400
    end

    local content_length = tonumber(headers['CONTENT_LENGTH'])
    if content_length then
        if content_length < 0 or content_length > 1024 * 1024 or content_length % 1 ~= 0 then
            return nil, 413
        end

        local data, err = con:readfull(content_length, 10)
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

    send_scgi_resp(con, 200, res and cjson.encode(res) or nil)
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

    send_scgi_resp(con, 200)

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

    send_scgi_resp(con, 200, cjson.encode({ size = total, md5 = hex.encode(md5ctx:final()) }))
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
    local digits = ''

    while true do
        local chunk, found = con:readuntil(':', 3)
        if not chunk or #digits + #chunk > 5 or chunk:find('%D') then
            return send_scgi_resp(con, 400)
        end

        digits = digits .. chunk

        if found then
            break
        end
    end

    local length = tonumber(digits)
    if not length or length < 1 or length > 16384 then
        return send_scgi_resp(con, 400)
    end

    local data, err = con:readfull(length, 10)
    if not data then
        log.err('read scgi:', err)
        return send_scgi_resp(con, 400)
    end

    local headers = {}
    local offset = 1

    while offset <= #data do
        local name_end = data:find('\0', offset, true)
        if not name_end or name_end == offset then
            return send_scgi_resp(con, 400)
        end

        local value_end = data:find('\0', name_end + 1, true)
        if not value_end then
            return send_scgi_resp(con, 400)
        end

        local name = data:sub(offset, name_end - 1)
        if scgi_fields[name] then
            headers[name] = data:sub(name_end + 1, value_end - 1)
        end

        offset = value_end + 1
    end

    data, err = con:recv(1, 10)
    if not data then
        log.err('read scgi:', err)
        return send_scgi_resp(con, 400)
    end

    if data ~= ',' then
        return send_scgi_resp(con, 400)
    end

    if headers['SCGI'] ~= '1' then
        return send_scgi_resp(con, 400)
    end

    local req = { headers = headers }

    local query_string = headers['QUERY_STRING'] or ''
    local query = {}

    for q in query_string:gmatch('[^&]+') do
        local name, value = q:match('^([^=]+)=(.*)$')
        if name then
            name = url_unescape(name)
            query[name] = url_unescape(value)
        end
    end

    req.query = query

    local script_name = headers['SCRIPT_NAME']
    local handler = handlers[script_name]
    if not handler then
        return send_scgi_resp(con, 404)
    end

    err = handler(con, req)

    if type(err) == 'number' then
        send_scgi_resp(con, err)
    end
end

local function run_scgi_server()
    local s, err = socket.listen_unix(sock_path)
    if not s then
        error(err)
    end

    local username = get_lighttpd_username()
    if username then
        local pw = sys.getpwnam(username)
        if pw then
            file.chown(sock_path, pw.uid)
        end
    end

    log.info('SCGI server listening on', sock_path)

    while true do
        local c, peer = s:accept()
        if not c then
            log.err('accept:', peer)
            break
        end

        active_requests = active_requests + 1

        eco.run(function()
            handle_scgi(c)
            active_requests = active_requests - 1
            gc_pending = true
        end)
    end
end

local function main()
    local opts, err = cli.parse_args({
        { name = 'trace' },
        { name = 'help', short = 'h' }
    })
    if not opts then
        io.stderr:write(err, '\n')
        os.exit(1)
    end

    if #opts.args > 0 then
        io.stderr:write('Unexpected positional argument: ', opts.args[1], '\n')
        os.exit(1)
    end

    if opts.help then
        print([[Usage: oui [--trace] [--help]
  --trace     Enable Lua call tracing
  -h, --help  Show this help]])
        return
    end

    eco.set_panic_hook(function(traceback1, traceback2)
        os.remove(sock_path)
        log.err(traceback1)
        log.err(traceback2)
    end)

    if opts.trace then
        set_trace()
    end

    -- Start collection earlier and take smaller steps during sustained traffic.
    collectgarbage('incremental', 120, 200, 10)

    -- Reclaim completed requests within one idle polling interval.
    eco.run(function()
        while true do
            eco.sleep(5)

            if active_requests == 0 and gc_pending then
                collectgarbage('collect')
                -- Reclaim objects retained until their finalizers ran.
                collectgarbage('collect')
                gc_pending = false
            elseif active_requests > 0 then
                collectgarbage('step', 64)
            end
        end
    end)

    for _, status in ipairs({ 200, 400, 401, 403, 404, 413, 500 }) do
        scgi_headers[status] = 'Status: ' .. status .. '\r\nCache-Control: no-store\r\nX-Content-Type-Options: nosniff\r\n\r\n'
    end

    scgi_fields = {
        SCGI = true,
        REQUEST_METHOD = true,
        CONTENT_LENGTH = true,
        CONTENT_TYPE = true,
        REMOTE_ADDR = true,
        SCRIPT_NAME = true,
        QUERY_STRING = true
    }

    rpc.init()
    rpc.load_acl()

    parse_config()
    init_signal()
    run_scgi_server()
end

main()

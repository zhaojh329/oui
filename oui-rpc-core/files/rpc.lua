local hex = require 'eco.encoding.hex'
local md5 = require 'eco.hash.md5'
local time = require 'eco.time'
local file = require 'eco.file'
local log = require 'eco.log'
local cjson = require 'cjson'
local uci = require 'uci'

local concat = table.concat
local random = math.random

local M = {
    ERROR_CODE_NOT_FOUND         = -1,
    ERROR_CODE_INVALID_ARGUMENT  = -2,
    ERROR_CODE_UNAUTHORIZED      = -3,
    ERROR_CODE_PERMISSION_DENIED = -4,
    ERROR_CODE_LOAD_SCRIPT       = -5,
    ERROR_CODE_UNKNOWN           = -6
}

local SESSION_TIMEOUT = 300
local MAX_NONCE_CNT = 5

local no_auth_funcs = {}
local funcs = {}
local acls = {}
local nonces = {}
local sessions = {}
local lua_code_cache = true

local function random_string(n)
    local t = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
    }
    local s = {}

    for _ = 1, n do
        s[#s + 1] = t[random(#t)]
    end

    return concat(s)
end

function M.init()
    local c = uci.cursor()

    lua_code_cache = c:get('oui', 'global', 'lua_code_cache') ~= '0'

    c:foreach('oui', 'no-auth', function(s)
        no_auth_funcs[s.module] = {}

        for _, func in ipairs(s.func or {}) do
            no_auth_funcs[s.module][func] = true
        end
    end)
end

function M.create_nonce()
    local cnt = #nonces

    if cnt > MAX_NONCE_CNT then
        log.err('The number of nonce too more than ' .. MAX_NONCE_CNT)
        return nil
    end

    local nonce = random_string(32)

    -- expires in 2s
    nonces[nonce] = time.at(2.0, function() nonces[nonce] = nil end)

    return nonce
end

function M.create_session(username, acl, remote_addr)
    local sid = random_string(32)
    local session = {
        tmr = time.at(SESSION_TIMEOUT, function() sessions[sid] = nil end),
        remote_addr = remote_addr,
        username = username,
        acls = acls[acl],
        acl = acl
    }

    sessions[sid] = session

    return sid
end

function M.login(username, password)
    local c = uci.cursor()
    local valid = false
    local acl

    c:foreach('oui', 'user', function(s)
        if s.username == username then
            if not s.password then
                acl = s.acl
                valid = true
                return false
            end

            for nonce in pairs(nonces) do
                if hex.encode(md5.sum(table.concat({s.password, nonce}, ':'))) == password then
                    acl = s.acl
                    valid = true
                    return false
                end
            end
            return false
        end
    end)

    if not valid then
        return nil
    end

    return acl
end

function M.get_session(sid)
    local s = sessions[sid]
    if s then s.tmr:set(SESSION_TIMEOUT) end
    return s
end

function M.delete_session(sid)
    sessions[sid] = nil
end

function M.get_acls()
    return acls
end

function M.load_acl()
    acls = {}

    for name in file.dir('/usr/share/oui/acl') do
        local acl = name:match('(.*).json')
        if acl then
            local data = file.readfile('/usr/share/oui/acl/' .. name)
            acls[acl] = cjson.decode(data)
        end
    end
end

local function is_local_session(session)
    return session.remote_addr == '127.0.0.1' or session.remote_addr == '::1'
end

local function need_auth(session, mod, func)
    if is_local_session(session) then
        return false
    end

    return not no_auth_funcs[mod] or not no_auth_funcs[mod][func]
end

function M.acl_match(session, content, class)
    if is_local_session(session) then
        return true
    end

    if not session.acls then
        return false
    end

    if not session.acls[class] then return false end

    local matchs = session.acls[class].matchs
    if not matchs then
        return false
    end

    for _, pattern in ipairs(matchs) do
        if content:match(pattern) then
            return not session.acls[class].reverse
        end
    end

    return session.acls[class].reverse
end

function M.call(mod, func, args, session)
    if not lua_code_cache or not funcs[mod] then
        local script = '/usr/share/oui/rpc/' .. mod .. '.lua'

        if not file.access(script) then
            log.err('module "' .. mod .. '" not found')
            return nil, M.ERROR_CODE_NOT_FOUND
        end

        local ok, tb = pcall(dofile, script)
        if not ok then
            log.err('load module "' .. mod .. '":', tb)
            return nil, M.ERROR_CODE_LOAD_SCRIPT
        end

        if type(tb) == 'table' then
            funcs[mod] = tb
        end
    end

    if not funcs[mod] or not funcs[mod][func] then
        log.err('module "' .. mod .. '.' .. func .. '" not found')
        return nil, M.ERROR_CODE_NOT_FOUND
    end

    if need_auth(session, mod, func) then
        if not session.username then
            return nil, M.ERROR_CODE_UNAUTHORIZED
        end

        if not M.acl_match(session, mod .. '.' .. func, 'rpc') then
            return nil, M.ERROR_CODE_PERMISSION_DENIED
        end
    end

    return funcs[mod][func](args, session)
end

return M

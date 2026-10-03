local hex = require 'eco.encoding.hex'
local md5 = require 'eco.hash.md5'
local uci = require 'eco.uci'
local rpc = require 'oui.rpc'

local M = {}

function M.get_users()
    local c = uci.cursor()
    local users = {}

    c:foreach('oui', 'user', function(s)
        users[#users + 1] = {
            username = s.username,
            acl = s.acl,
            id = s['.name']
        }
    end)

    return { users = users }
end

function M.del_user(params)
    local c = uci.cursor()
    local id = params.id

    if type(id) ~= 'string' or c:get('oui', id) ~= 'user' then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if c:get('oui', id, 'acl') == 'admin' then
        return nil, rpc.ERROR_CODE_PERMISSION_DENIED
    end

    c:delete('oui', id)
    c:commit('oui')
end

function M.change(params)
    local c = uci.cursor()
    local password = params.password
    local acl = params.acl
    local id = params.id

    if type(id) ~= 'string' or c:get('oui', id) ~= 'user' then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if type(password) ~= 'string' or password == '' then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if type(acl) ~= 'string' or not rpc.get_acls()[acl] then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    local username = c:get('oui', id, 'username')
    if username then
        c:set('oui', id, 'password', hex.encode(md5.sum(username .. ':' .. password)))
        c:set('oui', id, 'acl', acl or '')
        c:commit('oui')
    end
end

function M.add_user(params)
    local c = uci.cursor()
    local username = params.username
    local password = params.password
    local acl = params.acl

    if type(username) ~= 'string' or username == '' or #username > 64 then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if type(password) ~= 'string' or password == '' or #password > 1024 then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if type(acl) ~= 'string' or not rpc.get_acls()[acl] then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    local exist = false

    c:foreach('oui', 'user', function(s)
        if s.username == username then
            exist = true
            return false
        end
    end)

    if exist then
        return { code = 1, errors = 'already exist' }
    end

    local sid = c:add('oui', 'user')
    c:set('oui', sid, 'username', username)
    c:set('oui', sid, 'password', hex.encode(md5.sum(username .. ':' .. password)))
    c:set('oui', sid, 'acl', acl or '')
    c:commit('oui')

    return { code = 0 }
end

return M

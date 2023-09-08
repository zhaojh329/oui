local hex = require 'eco.encoding.hex'
local md5 = require 'eco.hash.md5'
local uci = require 'uci'

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

    c:delete('oui', id)
    c:commit('oui')
end

function M.change(params)
    local c = uci.cursor()
    local password = params.password
    local acl = params.acl
    local id = params.id

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

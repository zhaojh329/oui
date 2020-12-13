local utils = require "oui.utils"
local sqlite3 = require "lsqlite3"
local rpc = require "oui.rpc"

local M = {}

function M.get()
    local users = {}
    local db = sqlite3.open("/etc/oui-httpd/oh.db")

    db:exec(string.format("SELECT * FROM account"), function(udata, cols, values, names)
        users[#users + 1] = {
            username = values[1],
            acl = values[3]
        }
        return sqlite3.OK
    end)

    db:close()

    return {users = users}
end

function M.add(params)
    local username = params.username
    local password = params.password
    local acl = params.acl

    if type(username) ~= "string" or type(password) ~= "string" or type(acl) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    local db = sqlite3.open("/etc/oui-httpd/oh.db")
    local hash = utils.md5(username, password)

    db:exec(string.format("INSERT INTO account values('%s', '%s', '%s')", username, hash, acl))
    db:close()
end

function M.del(params)
    local username = params.username

    if type(username) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    local db = sqlite3.open("/etc/oui-httpd/oh.db")

    db:exec(string.format("DELETE FROM account WHERE username = '%s'", username))
    db:close()
end

function M.modify(params)
    local username = params.username
    local password = params.password
    local acl = params.acl

    if type(username) ~= "string" or type(password) ~= "string" or type(acl) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    local db = sqlite3.open("/etc/oui-httpd/oh.db")
    local hash = utils.md5(username, password)

    db:exec(string.format("UPDATE account SET password = '%s', acl = '%s' WHERE username = '%s'", hash, acl, username))
    db:close()
end

return M

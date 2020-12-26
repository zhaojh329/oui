local sqlite3 = require "lsqlite3"
local rpc = require "oui.rpc"

local M = {}

function M.groups(params)
    local db = sqlite3.open("/etc/oui-httpd/oh.db")

    local groups = {}

    db:exec("SELECT name FROM sqlite_master WHERE type = 'table' AND name LIKE 'acl_%'", function(udata, cols, values, names)
        local group = values[1]:sub(5)
        groups[#groups + 1] = group
        return sqlite3.OK
    end)

    db:close()

    return { groups = groups }
end

function M.add_group(params)
    local name = params.name

    if type(name) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    local db = sqlite3.open("/etc/oui-httpd/oh.db")

    db:exec(string.format("CREATE TABLE IF NOT EXISTS acl_%s(scope TEXT NOT NULL, entry TEXT NOT NULL, permissions TEXT NOT NULL)", name))

    db:close()
end

function M.del_group(params)
    local name = params.name

    if type(name) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    local db = sqlite3.open("/etc/oui-httpd/oh.db")

    db:exec(string.format("DROP TABLE IF EXISTS acl_%s", name))

    db:close()
end

function M.list(params)
    local group = params.group

    if type(group) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    local db = sqlite3.open("/etc/oui-httpd/oh.db")

    local acls = {}

    db:exec(string.format("SELECT * FROM acl_%s", group), function(udata, cols, values, names)
        acls[#acls + 1] = {
            scope = values[1],
            entry = values[2],
            perm = values[3]
        }

        return sqlite3.OK
    end)

    db:close()

    return { acls = acls }
end

function M.add(params)
    local group = params.group
    local scope = params.scope
    local entry = params.entry
    local perm = params.perm

    if type(group) ~= "string" or type(scope) ~= "string" or type(entry) ~= "string" or type(perm) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    local db = sqlite3.open("/etc/oui-httpd/oh.db")

    db:exec(string.format("INSERT INTO acl_%s values('%s', '%s', '%s')", group, scope, entry, perm))

    db:close()
end

function M.del(params)
    local group = params.group
    local scope = params.scope
    local entry = params.entry

    if type(group) ~= "string" or type(scope) ~= "string" or type(entry) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    local db = sqlite3.open("/etc/oui-httpd/oh.db")

    db:exec(string.format("DELETE FROM acl_%s WHERE scope = '%s' AND entry = '%s'", group, scope, entry))

    db:close()
end

return M

local sqlite3 = require "lsqlite3"

local M = {
    ERROR_CODE_INVALID_PARAMS   = -32602,
    ERROR_CODE_INTERNAL_ERROR   = -32603,
    ERROR_CODE_ACCESS           = -32000
}

M.session = function()
    return __oui_session or {}
end

M.access = function(scope, entry, need)
    local s = M.session()
    local is_local = s.is_local
    local aclgroup = s.aclgroup

    if is_local then return true end

    -- The admin acl group is always allowed
    if aclgroup == "admin" then return true end

    if not aclgroup or aclgroup == "" then return false end

    local db = sqlite3.open("/etc/oui-httpd/oh.db")

    local sql = string.format("SELECT permissions FROM acl_%s WHERE scope = '%s' AND entry = '%s'", aclgroup, scope, entry)
    local perm = ""

    db:exec(sql, function(udata, cols, values, names)
        perm = values[1]
        return 1
    end)

    db:close()

    if not need then return false end

    if need == "r" then
        return perm:find("[r,w]") ~= nil
    else
        return perm:find(need) ~= nil
    end
end

return M

local ubus = require "ubus"
local uci = require "uci"
local sqlite3 = require "lsqlite3"

local M = {}

local function uci_access(config, rw)
    local s = __oui_session

    -- The admin acl group is always allowed
    if s.aclgroup == "admin" then return true end

    local db = sqlite3.open("/etc/oui-httpd/oh.db")

    local sql = string.format("SELECT permissions FROM acl_%s WHERE scope = 'uci' AND entry = '%s'", s.aclgroup, config)
    local perm = ""

    db:exec(sql, function(udata, cols, values, names)
        perm = values[1]
        return 1
    end)

    db:close()

    if rw == "r" then
        return perm:find("[r,w]") ~= nil
    else
        return perm:find("w") ~= nil
    end
end

function M.load(params)
    local config = params.config

    if not uci_access(config, "r") then
        return nil, __rpc.RPC_ERROR_ACCESS
    end

    local c = uci.cursor()
    return c:get_all(params.config)
end

function M.set(params)
    local c = uci.cursor()
    local config = params.config
    local section = params.section

    if not uci_access(config, "w") then
        return nil, __rpc.RPC_ERROR_ACCESS
    end

    for option, value in pairs(params.values) do
        c:set(config, section, option, value)
    end

    c:save(config)
end

function M.delete(params)
    local c = uci.cursor()
    local config = params.config
    local section = params.section
    local options = params.options

    if not uci_access(config, "w") then
        return nil, __rpc.RPC_ERROR_ACCESS
    end

    if options then
        for _, option in ipairs(options) do
            c:delete(config, section, option)
        end
    else
        c:delete(config, section)
    end

    c:save(config)
end

function M.add(params)
    local config = params.config
    local typ = params.type
    local name = params.name
    local values = params.values

    if type(config) ~= "string" or type(typ) ~= "string" then
        return nil, __rpc.RPC_ERROR_PARAMS
    end

    if values and type(values) ~= "table" then
       return nil, __rpc.RPC_ERROR_PARAMS
    end

    if not uci_access(config, "w") then
        return nil, __rpc.RPC_ERROR_ACCESS
    end

    local c = uci.cursor()

    if name then
        c:set(config, name, typ)
    else
        name = c:add(config, typ)
    end

    for option, value in pairs(values) do
        c:set(config, name, option, value)
    end

    c:save(config)
end

function M.reorder(params)
    local c = uci.cursor()
    local config = params.config

    if not uci_access(config, "w") then
        return nil, __rpc.RPC_ERROR_ACCESS
    end

    for i, section in ipairs(params.sections) do
        c:reorder(config, section, i - 1)
    end

    c:save(config)
end

function M.apply()
    local c = uci.cursor()
    local changes = c:changes()

    local conn = ubus.connect()
    if not conn then
        error("Failed to connect to ubus")
    end

    for config in pairs(changes) do
        conn:call("service", "event", { type = "config.change", data = { package = config } })
        c:commit(config)
    end

    conn:close()
end

return M

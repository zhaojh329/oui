local ubus = require "ubus"
local uci = require "uci"
local sqlite3 = require "lsqlite3"

local M = {}

local function get_permissions(config)
    local db = sqlite3.open("/etc/oui-httpd/oh.db")
    local s = __oui_session
    local sql = string.format("SELECT permissions FROM acl_%s WHERE scope = 'uci' AND entry = '%s'", s.acl, config)
    local perm
    local neg = false

    db:exec(sql, function(udata, cols, values, names)
        perm = values[1]
        return 1
    end)

    db:close()

    if perm then
        neg = perm:sub(1, 1) == "!"
        if neg then
            perm = perm:sub(2)
        end
    end

    if perm == "" then perm = nil end

    return perm, neg
end

local function allow_read(config)
    local perm, neg = get_permissions(config)

    if perm then
        local allow = perm:find("[r,w]")
        if neg then allow = not allow end

        if not allow then
            error("forbidden")
        end
    end
end

local function allow_write(config)
    local perm, neg = get_permissions(config)

    if perm then
        local allow = perm:find("w")
        if neg then allow = not allow end

        if not allow then
            error("forbidden")
        end
    end
end

function M.load(params)
    local config = params.config

    allow_read(config)

    local c = uci.cursor()
    return c:get_all(params.config)
end

function M.set(params)
    local c = uci.cursor()
    local config = params.config
    local section = params.section

    allow_write(config)

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

    allow_write(config)

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
    local c = uci.cursor()
    local config = params.config
    local section = c:add(config, params.type)

    allow_write(config)

    for option, value in pairs(params.values) do
        c:set(config, section, option, value)
    end

    c:save(config)
end

function M.reorder(params)
    local c = uci.cursor()
    local config = params.config

    allow_write(config)

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

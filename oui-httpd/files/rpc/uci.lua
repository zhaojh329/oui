local ubus = require "ubus"
local uci = require "uci"

local M = {}

function M.load(params)
    local c = uci.cursor()
    return c:get_all(params.config)
end

function M.set(params)
    local c = uci.cursor()
    local config = params.config
    local section = params.section

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

    for option, value in pairs(params.values) do
        c:set(config, section, option, value)
    end

    c:save(config)
end

function M.reorder(params)
    local c = uci.cursor()
    local config = params.config

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

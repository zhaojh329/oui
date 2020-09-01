local ubus = require "ubus"

local M = {}

function M.call(params)
    local conn = ubus.connect()
    if not conn then
        error("Failed to connect to ubus")
    end

    local object = params.object
    local method = params.method

    if type(object) ~= "string" or type(method) ~= "string" then
        error("Invalid argument")
    end

    local res = conn:call(object, method, params.params or {})
    conn:close()

    return res
end

return M

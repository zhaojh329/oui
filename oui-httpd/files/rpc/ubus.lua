local ubus = require "ubus"
local rpc = require "oui.rpc"

local M = {}

function M.call(params)
    local conn = ubus.connect()
    if not conn then
        error("Failed to connect to ubus")
    end

    local object = params.object
    local method = params.method

    if type(object) ~= "string" or type(method) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    if not rpc.access("ubus", string.format("%s.%s", object, method), "x") then
        return rpc.ERROR_CODE_ACCESS
    end

    local res = conn:call(object, method, params.params or {})
    conn:close()

    return res
end

return M

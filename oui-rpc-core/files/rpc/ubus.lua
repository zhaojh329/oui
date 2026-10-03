local ubus = require 'eco.ubus'
local rpc = require 'oui.rpc'

local M = {}

function M.call(params, session)
    local object = params.object
    local method = params.method

    if type(object) ~= 'string' or type(method) ~= 'string' or
       (params.params ~= nil and type(params.params) ~= 'table') then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if not rpc.acl_match(session, params.object .. '.' .. params.method, 'ubus') then
        return nil, rpc.ERROR_CODE_PERMISSION_DENIED
    end

    local result, err = ubus.call(object, method, params.params or {})
    if err then
        return nil, rpc.ERROR_CODE_UNKNOWN
    end

    return result
end

return M

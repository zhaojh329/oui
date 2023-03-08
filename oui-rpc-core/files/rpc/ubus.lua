local ubus = require 'eco.ubus'
local rpc = require 'oui.rpc'
local log = require 'eco.log'

local M = {}

function M.call(params, session)
    local object = params.object
    local method = params.method

    if not rpc.acl_match(session, params.object .. '.' .. params.method, 'ubus') then
        return nil, M.ERROR_CODE_PERMISSION_DENIED
    end

    return ubus.call(object, method, params.params or {})
end

return M

local rpc = require 'oui.rpc'
local uci = require 'eco.uci'

local M = {}

local function valid_values(values)
    if type(values) ~= 'table' then
        return false
    end

    for option, value in pairs(values) do
        if type(option) ~= 'string' then
            return false
        end

        if type(value) == 'table' then
            for key, item in pairs(value) do
                if math.type(key) ~= 'integer' or key < 1 or type(item) ~= 'string' then
                    return false
                end
            end
        elseif type(value) ~= 'string' and type(value) ~= 'number' then
            return false
        end
    end

    return true
end

function M.load(params, session)
    if type(params.config) ~= 'string' or not params.config:match('^[%w_-]+$') then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    local config = params.config
    local c = uci.cursor()

    if not rpc.acl_match(session, config, 'uci') then
        return nil, rpc.ERROR_CODE_PERMISSION_DENIED
    end

    return c:get_all(params.config)
end

function M.get(params, session)
    if type(params.config) ~= 'string' or not params.config:match('^[%w_-]+$') then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if type(params.section) ~= 'string' or
       (params.option ~= nil and type(params.option) ~= 'string') then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    local c = uci.cursor()
    local config = params.config
    local section = params.section
    local option = params.option

    if not rpc.acl_match(session, config, 'uci') then
        return nil, rpc.ERROR_CODE_PERMISSION_DENIED
    end

    return c:get(config, section, option)
end

function M.set(params, session)
    if type(params.config) ~= 'string' or not params.config:match('^[%w_-]+$') then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if type(params.section) ~= 'string' or
       (params.option ~= nil and type(params.option) ~= 'string') then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if not valid_values(params.values) then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    local c = uci.cursor()
    local config = params.config
    local section = params.section

    if not rpc.acl_match(session, config, 'uci') then
        return nil, rpc.ERROR_CODE_PERMISSION_DENIED
    end

    for option, value in pairs(params.values) do
        if not c:set(config, section, option, value) then
            return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
        end
    end

    if not c:commit(config) then
        return nil, rpc.ERROR_CODE_UNKNOWN
    end
end

function M.delete(params, session)
    if type(params.config) ~= 'string' or not params.config:match('^[%w_-]+$') then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if type(params.section) ~= 'string' or
       (params.option ~= nil and type(params.option) ~= 'string') then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if params.options ~= nil then
        if type(params.options) ~= 'table' then
            return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
        end
        for _, option in pairs(params.options) do
            if type(option) ~= 'string' then
                return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
            end
        end
    end

    local c = uci.cursor()
    local config = params.config
    local section = params.section
    local options = params.options

    if not rpc.acl_match(session, config, 'uci') then
        return nil, rpc.ERROR_CODE_PERMISSION_DENIED
    end

    if options then
        for _, option in ipairs(options) do
            c:delete(config, section, option)
        end
    else
        c:delete(config, section)
    end

    if not c:commit(config) then
        return nil, rpc.ERROR_CODE_UNKNOWN
    end
end

function M.add(params, session)
    if type(params.config) ~= 'string' or not params.config:match('^[%w_-]+$') then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if not valid_values(params.values) then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if type(params.type) ~= 'string' or (params.name ~= nil and type(params.name) ~= 'string') then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    local c = uci.cursor()
    local config = params.config
    local typ = params.type
    local name = params.name
    local values = params.values

    if not rpc.acl_match(session, config, 'uci') then
        return nil, rpc.ERROR_CODE_PERMISSION_DENIED
    end

    if name then
        if not c:set(config, name, typ) then
            return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
        end
    else
        name = c:add(config, typ)
        if not name then
            return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
        end
    end

    for option, value in pairs(values) do
        if not c:set(config, name, option, value) then
            return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
        end
    end

    if not c:commit(config) then
        return nil, rpc.ERROR_CODE_UNKNOWN
    end
end

return M

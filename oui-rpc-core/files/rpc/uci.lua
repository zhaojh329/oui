local rpc = require 'oui.rpc'
local uci = require 'uci'

local M = {}

function M.load(params, session)
    local config = params.config
    local c = uci.cursor()

    if not rpc.acl_match(session, config, 'uci') then
        return nil, M.ERROR_CODE_PERMISSION_DENIED
    end

    return c:get_all(params.config)
end

function M.get(params, session)
    local c = uci.cursor()
    local config = params.config
    local section = params.section
    local option = params.option

    if not rpc.acl_match(session, config, 'uci') then
        return nil, M.ERROR_CODE_PERMISSION_DENIED
    end

    return c:get(config, section, option)
end

function M.set(params, session)
    local c = uci.cursor()
    local config = params.config
    local section = params.section

    if not rpc.acl_match(session, config, 'uci') then
        return nil, M.ERROR_CODE_PERMISSION_DENIED
    end

    for option, value in pairs(params.values) do
        c:set(config, section, option, value)
    end

    c:commit(config)
end

function M.delete(params, session)
    local c = uci.cursor()
    local config = params.config
    local section = params.section
    local options = params.options

    if not rpc.acl_match(session, config, 'uci') then
        return nil, M.ERROR_CODE_PERMISSION_DENIED
    end

    if options then
        for _, option in ipairs(options) do
            c:delete(config, section, option)
        end
    else
        c:delete(config, section)
    end

    c:commit(config)
end

function M.add(params, session)
    local c = uci.cursor()
    local config = params.config
    local typ = params.type
    local name = params.name
    local values = params.values

    if not rpc.acl_match(session, config, 'uci') then
        return nil, M.ERROR_CODE_PERMISSION_DENIED
    end

    if name then
        c:set(config, name, typ)
    else
        name = c:add(config, typ)
    end

    for option, value in pairs(values) do
        c:set(config, name, option, value)
    end

    c:commit(config)
end

return M

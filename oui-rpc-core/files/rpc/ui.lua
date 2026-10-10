local file = require 'eco.file'
local rpc = require 'oui.rpc'
local cjson = require 'cjson'
local uci = require 'eco.uci'

local M = {}

function M.get_locale()
    local c<close> = uci.cursor()
    local locale = c:get('oui', 'global', 'locale')

    return { locale = locale }
end

function M.get_theme()
    local c<close> = uci.cursor()
    local theme = c:get('oui', 'global', 'theme')

    return { theme = theme }
end

function M.set_theme(params)
    if params.theme ~= 'dark' and params.theme ~= 'light' then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    local c<close> = uci.cursor()

    c:set('oui', 'global', 'theme', params.theme)
    c:commit('oui')
end

function M.set_locale(params)
    local locale = params.locale

    if type(locale) ~= 'string' or #locale > 63 or
       not locale:match('^%a[%w%-]*$') or locale:find('%-%-') or locale:sub(-1) == '-' then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if locale ~= 'auto' then
        local language = locale:match('^(%a+)')
        if not language or #language < 2 or #language > 8 or
           (#language < #locale and locale:sub(#language + 1, #language + 1) ~= '-') then
            return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
        end

        for part in locale:gmatch('[^%-]+') do
            if #part > 8 then
                return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
            end
        end
    end

    local c<close> = uci.cursor()

    c:set('oui', 'global', 'locale', locale)
    c:commit('oui')
end

function M.get_menus(params, session)
    local menus = {}

    for name in file.dir('/usr/share/oui/menu.d') do
        if name:match('^%w.*%.json$') then
            local data = file.readfile('/usr/share/oui/menu.d/' .. name)
            local menu = cjson.decode(data)
            for m, info in pairs(menu) do
                if rpc.acl_match(session, m, 'menu') then
                    menus[m] = info
                end
            end
        end
    end

    return { menus = menus }
end

return M

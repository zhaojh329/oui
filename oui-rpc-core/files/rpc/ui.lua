local file = require 'eco.file'
local rpc = require 'oui.rpc'
local cjson = require 'cjson'
local uci = require 'uci'

local M = {}

function M.get_locale()
    local c = uci.cursor()
    local locale = c:get('oui', 'global', 'locale')

    return { locale = locale }
end

function M.get_theme()
    local c = uci.cursor()
    local theme = c:get('oui', 'global', 'theme')

    return { theme = theme }
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

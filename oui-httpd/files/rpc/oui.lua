local uci = require "uci"

local M = {}

local RPC_OUI_MENU_FILES = "/usr/share/oui/menu.d/*.json"

local function menu_files(files)
    for _, file in ipairs(files) do
        local f = io.open(file)
        if not f then return false end
        f:close()
    end

    return true
end

function M.get_lang()
    local c = uci.cursor()

    return { lang = c:get("oui", "main", "lang") }
end

function M.set_lang(params)
    local c = uci.cursor()

    if type(params.lang) ~= "string" then error("Invalid params") end

    c:set("oui", "main", "lang", params.lang)
    c:commit("oui")

    return { lang = c:get("oui", "main", "lang") }
end

function M.menu(params)
	local menus = {}

    local f = io.popen("ls " .. RPC_OUI_MENU_FILES .. " 2>/dev/null")
    if f then
        for file in f:lines() do
            local menu = cjson.decode(utils.readfile(file))

            for path, item in pairs(menu) do
                local access, files = true, true
                local tmp = {}

                for k, v in pairs(item) do
                    if k == "files" then
                        files = menu_files(v)
                    else
                        tmp[k] = v
                    end
                end

                if files then
                    menus[path] = tmp
                end
            end
        end
        f:close()
    end

    return {menu = menus}
end

function M.load_locales(params)
	local locales = {}

	if type(params.locale) ~= "string" then error("Invalid params") end

	local cmd = string.format("ls /www/i18n/*.%s.json 2>/dev/null", params.locale)

	local f = io.popen(cmd)
		if f then
		for file in f:lines() do
			local locale = cjson.decode(utils.readfile(file))
			locales[#locales + 1] = locale
		end
		f:close()
	end

	return locales
end

function M.set_password(params)
    if type(params.password) ~= "string" then
        error("invalid params")
    end

    os.execute("mkdir /etc/oui")
    utils.writefile("/etc/oui/shadow", params.password)
end

return M

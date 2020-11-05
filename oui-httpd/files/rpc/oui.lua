local uci = require "uci"
local sqlite3 = require "lsqlite3"

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

local function get_menu_perm(menu)
    local db = sqlite3.open("/etc/oui-httpd/oh.db")
    local s = __oui_session
    local sql = string.format("SELECT permissions FROM acl_%s WHERE scope = 'menu' AND entry = '%s'", s.acl, menu)
    local perm
    local neg = false

    db:exec(sql, function(udata, cols, values, names)
        perm = values[1]
        return 1
    end)

    db:close()

    if perm then
        neg = perm:sub(1, 1) == "!"
        if neg then
            perm = perm:sub(2)
        end
    end

    if perm == "" then perm = nil end

    return perm, neg
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
                    local perm, neg = get_menu_perm("/" .. path)
                    local allow = true
                    if perm then
                        allow = perm:find("s")
                        if neg then
                            allow = not allow
                        end
                    end
                    if allow then
                        menus[path] = tmp
                    end
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

local function set_password(params)
    if type(params.username) ~= "string" or  type(params.password) ~= "string" then
        error("invalid params")
    end

    local db = sqlite3.open("/etc/oui-httpd/oh.db")

    local found = false

    db:exec(string.format("SELECT password FROM account WHERE username = %s", params.username), function() found = true end)

    if not found then
        db:exec(string.format("INSERT INTO account VALUES('%s', '', '')", params.username))
    end

    local hash = utils.md5(params.username, params.password)
    db:exec(string.format("UPDATE account SET password = '%s' WHERE username = '%s'", hash, params.username))

    db:close()
end

function M.set_password(params)
    local s = __oui_session

    if s.acl ~= "admin" and params.username ~= s.username then
        error("forbidden")
    end
    set_password(params)
end

function M.first_login()
    local c = uci.cursor()

	return {
		first = c:get("oui", "main", "first") == "1"
	}
end

function M.first_set(params)
	if not M.first_login() then
		error("forbidden")
	end

    local c = uci.cursor()

    c:set("oui", "main", "lang", params.lang)
    c:set("oui", "main", "first", "0")
    c:commit("oui")

	set_password(params)
end

return M

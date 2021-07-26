local core = require "oui.utils.utils"

local M = {}

setmetatable(M, {
    __index = core
})

-- The available formats are:
-- "n": reads a numeral and returns it as a float or an integer, following the lexical conventions of Lua.
-- "a": reads the whole file. This is the default format.
-- "l": reads the next line skipping the end of line.
-- "L": reads the next line keeping the end-of-line character (if present).
-- number: reads a string with up to this number of bytes. If number is zero, it reads nothing and returns an empty string.
-- Return nil if the file open failed
M.readfile = function(name, format)
    local f = io.open(name, "r")
    if not f then return nil end

    -- Compatible with the version below 5.3
    if type(format) == "string" and format:sub(1, 1) ~= "*" then format = "*" .. format end

    local data

    if format == "*L" and tonumber(_VERSION:match("%d.%d")) < 5.2 then
        data = f:read("*l")
        if data then data = data .. "\n" end
    else
        data = f:read(format or "*a")
    end

    f:close()
    return data or ""
end

M.writefile = function (name, data, append)
    local f = io.open(name, append and "a" or "w")
    if not f then return nil end
    f:write(data)
    f:close()
    return true
end

return M

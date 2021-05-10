local core = require "oui.utils.utils"

local M = {}

M.readfile = function(name, n)
    local f = io.open(name, "r")
    if not f then return nil end
    local data = f:read(n or "*a")
    f:close()
    return data
end

M.writefile = function (name, data)
    local f = io.open(name, "w")
    if not f then return nil end
    f:write(data)
    f:close()
    return true
end

setmetatable(M, {
    __index = core
})

return M

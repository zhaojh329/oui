local M = {}

function M.readfile(name, n)
    local f = io.open(name, "r")
    if not f then return nil end
    local data = f:read(n or "*a")
    f:close()
    return data
end

function M.writefile(name, data)
    local f = io.open(name, "w")
    if not f then return nil end
    f:write(data)
    f:close()
    return true
end

return M

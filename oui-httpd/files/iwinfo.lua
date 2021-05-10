local iwinfo = require 'iwinfo'

local M = {}

M.info = function(device)
    local iwtype = iwinfo.type(device)

    if not iwtype then
        error("Not support")
    end

    return setmetatable({}, {
        __index = function(t, k)
            return iwinfo[iwtype][k] and iwinfo[iwtype][k](device)
        end
    })
end

return M

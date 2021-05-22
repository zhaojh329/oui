local iwinfo = require 'iwinfo'

local M = {}

setmetatable(M, {
    __index = iwinfo
})

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

function M.assoclist(device)
   return M.info(device)['assoclist']
end

function M.scan(device)
    return M.info(device)['scanlist']
end

function M.freqlist(device)
    return M.info(device)['freqlist']
end

function M.txpwrlist(device)
    return M.info(device)['txpwrlist']
end

function M.countrylist(device)
    return M.info(device)['countrylist']
end

return M

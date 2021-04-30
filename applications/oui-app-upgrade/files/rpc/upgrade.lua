local utils = require "oui.utils"

local M = {}

function M.verify(params)
    local res = {}

    res.code = utils.exec("sysupgrade", "-T", params.path):wait()

    if res.code == 0 then
        res.md5 = utils.md5sum(params.path)
    end

    return res
end

function M.upgrade(params)
    if params.keep then
        utils.exec("sysupgrade", params.path)
    else
        utils.exec("sysupgrade", "-n", params.path)
    end
end

function M.backup_restore(params)
    local code = utils.exec("sysupgrade", "-r", params.path)
    return {code = code}
end

function M.backup(params)
    local code = utils.exec("sysupgrade", "-b", params.path)
    return {code = code}
end

return M

local M = {}

function M.verify(params)
    local code = os.execute("sysupgrade -T " .. params.path)
    local res = {}

    res.code = code

    if res.code == 0 then
        res.md5 = utils.md5sum(params.path)
    end

    return res
end

function M.upgrade(params)
    local cmd = "sysupgrade "
    if not params.keep then
        cmd = cmd .. "-n "
    end
    os.execute(cmd .. params.path .. " &")
end

function M.backup_restore(params)
    local code = os.execute("sysupgrade -r " .. params.path)
    return {code = code}
end

function M.backup(params)
    local code = os.execute("sysupgrade -b " .. params.path)
    return {code = code}
end

return M

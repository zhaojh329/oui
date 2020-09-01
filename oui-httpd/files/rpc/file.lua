local M = {}

function M.md5sum(params)
    return { md5 = utils.md5sum(params.path) }
end

function M.remove(params)
    os.remove(params.path)
end

return M

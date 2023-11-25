local file = require 'eco.file'
local cjson = require 'cjson'
local rpc = require 'oui.rpc'

local M = {}

function M.load()
    local res = {}

    for group, acls in pairs(rpc.get_acls()) do
        local acl = {}

        for cls, info in pairs(acls) do
            acl[#acl + 1] = {
                cls = cls,
                matchs = info.matchs,
                reverse = not not info.reverse
            }
        end

        res[group] = acl
    end

    return res
end

function M.set(params)
    for name in file.dir('/usr/share/oui/acl') do
        if name ~= '.' and name ~= '..' then
            os.remove('/usr/share/oui/acl/' .. name)
        end
    end

    for group, acls in pairs((params.acls)) do
        local acl = {}
        for _, info in ipairs(acls) do
            acl[info.cls] = {
                matchs = info.matchs,
                reverse = info.reverse
            }
        end
        file.writefile('/usr/share/oui/acl/' .. group .. '.json', cjson.encode(acl))
    end

    rpc.load_acl()
end

return M

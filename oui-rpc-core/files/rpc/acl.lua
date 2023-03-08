local ubus = require 'eco.ubus'
local file = require 'eco.file'
local cjson = require 'cjson'
local rpc = require 'oui.rpc'

local M = {}

function M.load()
    return rpc.get_acls()
end

function M.set(params)
    for group, acls in pairs((params.acls)) do
        file.writefile('/usr/share/oui/acl/' .. group .. '.json', cjson.encode(acls))
    end

    rpc.load_acl()
end

return M

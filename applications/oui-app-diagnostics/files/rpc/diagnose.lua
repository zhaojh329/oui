local utils = require "oui.utils"

local M = {}

local function run_cmd(cmd, params)
    local args = { cmd }

    if type(params.args) == "string" then
        for a in params.args:gmatch("%S+") do
            args[#args + 1] = a
        end
    end

    local r, err = utils.exec(unpack(args))
    if not r then
        error(err)
    end

    local code, stdout, stderr = r:wait()
    if not code then
        error(stdout)
    end

    return {
        code = code,
        stdout = stdout,
        stderr = stderr
    }
end

function M.ping(params)
    return run_cmd("ping", params)
end

function M.ping6(params)
    return run_cmd("ping6", params)
end

function M.traceroute(params)
    return run_cmd("traceroute", params)
end

function M.traceroute6(params)
    return run_cmd("traceroute6", params)
end

function M.nslookup(params)
    return run_cmd("nslookup", params)
end

return M
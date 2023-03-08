local time = require 'eco.time'
local sys = require 'eco.sys'

local M = {}

function M.get_cpu_time()
    local result = {}

    for line in io.lines('/proc/stat') do
        local cpu = line:match('^(cpu%d?)')
        if cpu then
            local times = {}
            for field in line:gmatch('%S+') do
                if not field:match('cpu') then
                    times[#times + 1] = tonumber(field)
                end
            end
            result[cpu] = times
        end
    end

    return { times = result }
end

function M.sysupgrade(params)
    time.at(0.5, function()
        local arg = params.keep and '' or '-n'
        os.execute('sysupgrade ' .. arg .. ' /tmp/firmware.bin')
    end)
end

function M.create_backup(params)
    local path = params.path
    sys.exec('sysupgrade', '-b', path)
end

function M.list_backup(params)
    local path = params.path

    local f = io.popen('tar -tzf ' .. path)
    if not f then
        return
    end

    local files = f:read('*a')

    f:close()

    return { files = files }
end

function M.restore_backup(params)
    os.execute('sysupgrade -r ' .. params.path)

    time.at(0.5, function()
        os.execute('reboot')
    end)
end

function M.reset()
    time.at(0.5, function()
        os.execute('firstboot -y && reboot')
    end)
end

return M

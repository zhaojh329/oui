local time = require 'eco.time'
local sys = require 'eco.sys'
local rpc = require 'oui.rpc'
local uci = require 'eco.uci'
local ubus = require 'eco.ubus'

local M = {}

function M.board()
    local result, err = ubus.call('system', 'board', {})
    if err then
        return nil, rpc.ERROR_CODE_UNKNOWN
    end

    return result
end

function M.info()
    local result, err = ubus.call('system', 'info', {})
    if err then
        return nil, rpc.ERROR_CODE_UNKNOWN
    end

    return result
end

function M.reboot()
    local result, err = ubus.call('system', 'reboot', {})
    if err then
        return nil, rpc.ERROR_CODE_UNKNOWN
    end

    return result
end

function M.validate_firmware_image()
    local result, err = ubus.call('system', 'validate_firmware_image', { path = '/tmp/firmware.bin' })
    if err then
        return nil, rpc.ERROR_CODE_UNKNOWN
    end

    return result
end

function M.set_hostname(params)
    local hostname = params.hostname

    if type(hostname) ~= 'string' or #hostname == 0 or #hostname > 253 then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    if not hostname:match('^[%w_]+$') and
       not (hostname:match('^[%w_][%w_%.%-]*[%w]$') and hostname:find('[^%d%.]')) then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    local c<close> = uci.cursor()

    c:set('system', '@system[0]', 'hostname', hostname)
    c:commit('system')

    rpc.reload_config('system')
end

function M.get_timezone()
    local c<close> = uci.cursor()
    local timezone = c:get('system', '@system[0]', 'timezone')
    local zonename = c:get('system', '@system[0]', 'zonename')

    return { timezone = timezone, zonename = zonename }
end

function M.set_timezone(params)
    local timezone = params.timezone
    local zonename = params.zonename

    if type(timezone) ~= 'string' or type(zonename) ~= 'string' or
       timezone:find('%c') or zonename:find('%c') then
        return nil, rpc.ERROR_CODE_INVALID_ARGUMENT
    end

    local c<close> = uci.cursor()

    c:set('system', '@system[0]', 'timezone', timezone)
    c:set('system', '@system[0]', 'zonename', zonename)
    c:commit('system')

    rpc.reload_config('system')
end

function M.get_cpu_time()
    local result = {}

    for line in io.lines('/proc/stat') do
        local cpu = line:match('^(cpu%d*)%s')
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
    sys.sh({ 'sysupgrade', '-b', path })
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

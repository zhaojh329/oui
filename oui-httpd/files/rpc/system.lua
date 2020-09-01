local M = {}

function M.diskfree()
    local fslist = {
        root = "/",
        tmp = "/tmp"
    }
    local resp = {}

    for name, path in pairs(fslist) do
        local total, free, used = utils.statvfs(path)
        if total then
            resp[name] = {
                total = total,
                free = free,
                used = used
            }
        end
    end
    return resp
end

function M.cpu_time()
    local line = utils.readfile("/proc/stat", "*l")
    local times = {}
    for t in line:sub(4):gmatch("%S+") do
        times[#times + 1] = tonumber(t)
    end
    return times
end

function M.syslog()
    local log = {}
    local f = io.popen("logread")
    if f then
        for line in f:lines() do
            local week, month, day, time, year, facility, level, msg = line:match("(%S+) +(%S+) +(%d+) +(%S+) +(%d+) +(%S+)%.(%S+) +(.+)$")
            log[#log + 1] = {
                datetime = string.format("%s %s %s %s %s", week, month, day, time, year),
                facility = facility,
                level = level,
                msg = msg
            }
        end
        f:close()
    end
    return { log = log }
end

function M.dmesg()
    local log = {}
    local f = io.popen("dmesg")
    if f then
        for line in f:lines() do
            local time, msg = line:match("(%d+%.%d+)%] +(.+)$")
            log[#log + 1] = {
                time = time,
                msg = msg
            }
        end
        f:close()
    end
    return { log = log }
end

function M.process_list()
    local processes = {}
    local f = io.popen("top -bn1")
    if f then
        for line in f:lines() do
            local pid, ppid, user, stat, vsz, pvsz, pcpu, cmd = line:match("(%d+) +(%d+) +(%S+) +(%S+) +(%d+) +(%d+)%% +(%d+)%% +(.+)$")
            if pid then
                processes[#processes + 1] = {
                    pid = tonumber(pid),
                    ppid = tonumber(ppid),
                    user = user,
                    stat = stat,
                    vsize = tonumber(vsz),
                    vsize_percent = tonumber(pvsz),
                    cpu_percent = tonumber(pcpu),
                    command = cmd
                }
            end
        end
        f:close()
    end
    return { processes = processes }
end

function M.time(params)
    if params.time then
        os.execute("date -s '" .. os.date("%Y-%m-%d %H:%M:%S'", params.time))
    end
    return { time = os.time() }
end

function M.init_list()
    local initscripts = {}
    local f = io.popen("ls /etc/init.d")
    if f then
        for name in f:lines() do
            local start, stop, enabled = false
            local line = utils.readfile("/etc/init.d/" .. name, "*l")
            if line and line:match("/etc/rc.common") then
                for line in io.lines("/etc/init.d/" .. name) do
                    local k, v = line:match("(%S+)=(%d+)")
                    if k == "START" then
                        start = v
                    elseif k == "STOP" then
                        stop = v
                        break
                    end
                end
                if start then
                    if utils.readfile("/etc/rc.d/S" .. start .. name, 1) then
                        enabled = true
                    end

                    initscripts[#initscripts + 1] = {
                        name = name,
                        start = tonumber(start),
                        stop = tonumber(stop),
                        enabled = enabled
                    }
                end
            end
        end
        f:close()
    end
    return { initscripts = initscripts }
end

function M.init_action(params)
    if type(params.name) ~= "string" then
        error("invalid argument")
    end

    if params.action ~= "start" and params.action ~= "stop" and
            params.action ~= "reload" and params.action ~= "restart" and
            params.action ~= "enable" and params.action ~= "disable" then
        error("invalid argument")
    end

    local cmd = string.format("/etc/init.d/%s %s &", params.name, params.action)
    os.execute(cmd)
end

function M.led_list()
    local leds = {}
    local f = io.popen("ls /sys/class/leds")
    if f then
        for name in f:lines() do
            local data = utils.readfile("/sys/class/leds/" .. name .. "/trigger")
            local active_trigger, brightness, max_brightness
            local triggers = {}

            for trigger in data:gmatch("%S+") do
                if trigger:sub(1, 1) == "[" then
                    trigger = trigger:sub(2, #trigger - 1)
                    active_trigger = trigger
                end
                triggers[#triggers + 1] = trigger
            end

            brightness = utils.readfile("/sys/class/leds/" .. name .. "/brightness", "*l")
            max_brightness = utils.readfile("/sys/class/leds/" .. name .. "/max_brightness", "*l")

            leds[#leds + 1] = {
                name = name,
                triggers = triggers,
                active_trigger = active_trigger,
                brightness = tonumber(brightness),
                max_brightness = tonumber(max_brightness)
            }
        end
        f:close()
    end
    return { leds = leds }
end

function M.factory()
    os.execute("jffs2reset -y -r &")
end

return M

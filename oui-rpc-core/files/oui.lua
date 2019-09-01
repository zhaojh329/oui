#!/usr/bin/lua

local libubus = require "ubus"
local uloop = require "uloop"
local cjson = require "cjson"
local uci = require "uci"
local crypt = require "oui.c".crypt
local statvfs = require "oui.c".statvfs
local parse_route_addr = require "oui.c".parse_route_addr
local parse_route6_addr = require "oui.c".parse_route6_addr
local parse_flow = require "oui.c".parse_flow
local readdir = require "oui.c".readdir

local RPC_OUI_MENU_FILES = "/usr/share/oui/menu.d/*.json"

local UBUS_STATUS_OK = 0
local UBUS_STATUS_INVALID_COMMAND = 1
local UBUS_STATUS_INVALID_ARGUMENT = 2
local UBUS_STATUS_METHOD_NOT_FOUND = 3
local UBUS_STATUS_NOT_FOUND = 4
local UBUS_STATUS_NO_DATA = 5
local UBUS_STATUS_PERMISSION_DENIED = 6
local UBUS_STATUS_TIMEOUT = 7
local UBUS_STATUS_NOT_SUPPORTED = 8
local UBUS_STATUS_UNKNOWN_ERROR = 9
local UBUS_STATUS_CONNECTION_FAILED = 10

local ubus

local function file2json(path)
    local f = io.open(path)
    local json = {}
    if f then
        local data = f:read("*a")
        json = cjson.decode(data)
        f:close()
    end

    return json
end

local function read_file(path, pattern)
    local f = io.open(path)
    local data
    if f then
        data = f:read(pattern or "*a")
        f:close()
    end
    return data
end

local function write_file(path, data)
    local f = io.open(path, "w")
    if f then
        data = f:write(data)
        f:close()
    end
end

local function menu_access(sid, acls)
    for _, acl in ipairs(acls) do
        local r = ubus.call("session", "access", {ubus_rpc_session = sid, scope = "access-group", object = acl, ["function"] = "read"})
        if not r or not r.access then
            return false
        end
    end
    return true
end

local function menu_files(files)
    for _, file in ipairs(files) do
        local f = io.open(file)
        if not f then return false end
        f:close()
    end

    return true
end

local function opkg_list(action, msg)
    local limit = msg.limit or 100
    local offset = msg.offset or 0
    local pattern = msg.pattern

    if offset < 0 then offset = 0 end
    if limit > 100 then limit = 100 end

    local cmd = string.format( "opkg %s --size --nocase %s", action, msg.pattern or "")
    local upgradable = action == "list-upgradable"
    local packages = {}
    local total = 0

    local f = io.popen(cmd)
    if f then
        local i = -1
        local count = 0
        local prev

        for line in f:lines() do
            local name, version, field = line:match("(%S+) %- (%S+) %- (%S+)")
            if name and prev ~= name then
                total = total + 1
                prev = name
                i = i + 1
            end

            if name and i >= offset and count < limit then
                local pkg = packages[name]
                if not pkg then
                    count = count + 1
                    packages[name] = {size = 0, version = version}
                    pkg = packages[name]
                end

                if upgradable then
                    pkg.new_version = field
                else
                    local size = tonumber(field)
                    if size > 0 then pkg.size = size end

                    local description = line:sub(#name + #version + #field + 10)
                    if #description > 0 then pkg.description = description end
                end
            end
        end
        f:close()
    end

    local resp = {packages = {}, total = total}

    for name, pkg in pairs(packages) do
        resp.packages[#resp.packages + 1] = {
            name = name,
            version = pkg.version,
            size = pkg.size,
            new_version = pkg.new_version,
            description = pkg.description
        }
    end

    return resp
end

local function dnsmasq_leasefile()
    local c = uci.cursor()
    local leasefile

    c:foreach("dhcp", "dnsmasq", function(s)
        leasefile = s.leasefile
    end)

    return leasefile
end

local function network_cmd(name, cmd)
    local cmds = {
        ping = {"-c", "5", "-W", "1", name},
        ping6 = {"-c", "5", "-W", "1", name},
        traceroute = {"-q", "1", "-w", "1", "-n", name},
        traceroute6 = {"-q", "1", "-w", "2", "-n", name},
        nslookup = {name}
    }

    return ubus.call("file", "exec", {command = cmd, params = cmds[cmd]})
end

local function network_ifupdown(name, up)
    local cmd = up and "ifup" or "ifdown"
    return ubus.call("file", "exec", {command = cmd, params = {name}})
end

local methods = {
    ["oui.ui"] = {
        lang = {
            function(req, msg)
                local c = uci.cursor()

                if msg.lang then
                    c:set("oui", "main", "lang", msg.lang)
                    c:commit("oui")
                end

                local lang = c:get("oui", "main", "lang")

                ubus.reply(req, {lang = lang})
            end, {lang = libubus.STRING}
        },
        menu = {
            function(req, msg)
                local menus = {}

                local f = io.popen("ls " .. RPC_OUI_MENU_FILES .. " 2>/dev/null")
                if f then
                    for file in f:lines() do
                        local menu = file2json(file)

                        for path, item in pairs(menu) do
                            local access, files = true, true
                            local tmp = {}

                            for k, v in pairs(item) do
                                if k == "acls" then
                                    access = menu_access(msg.ubus_rpc_session, v)
                                elseif k == "files" then
                                    files = menu_files(v)
                                else
                                    tmp[k] = v
                                end
                            end

                            if access and files then
                                menus[path] = tmp
                            end
                        end
                    end
                    f:close()
                end

                ubus.reply(req, {menu = menus})
            end, {}
        },
        acls = {
            function(req, msg)
                local acls = {}

                local f = io.popen("ls /usr/share/rpcd/acl.d/*.json 2>/dev/null")
                if f then
                    for file in f:lines() do
                        local acl = file2json(file)
                        for k, v in pairs(acl) do
                            acls[#acls + 1] = {
                                [k] = v
                            }
                        end
                    end
                    f:close()
                end

                ubus.reply(req, {acls = acls})
            end, {}
        },
        crypt = {
            function(req, msg)
                if not msg.data or #msg.data >= 128 then
                    return UBUS_STATUS_INVALID_ARGUMENT
                end

                local hash = crypt(msg.data, "$1$")
                ubus.reply(req, {crypt = hash})
            end, {data = libubus.STRING}
        }
    },
    ["oui.opkg"] = {
        list = {
            function(req, msg)
                ubus.reply(req, opkg_list("list", msg))
            end, {limit = libubus.INT32, offset = libubus.INT32, pattern = libubus.STRING}
        },
        list_installed = {
            function(req, msg)
                ubus.reply(req, opkg_list("list-installed", msg))
            end, {limit = libubus.INT32, offset = libubus.INT32, pattern = libubus.STRING}
        },
        list_upgradable = {
            function(req, msg)
                ubus.reply(req, opkg_list("list-upgradable", msg))
            end, {limit = libubus.INT32, offset = libubus.INT32}
        },
        install = {
            function(req, msg)
                if not msg.package or #msg.package == 0 then
                    return UBUS_STATUS_INVALID_ARGUMENT
                end

                local r = ubus.call("file", "exec", {command = "opkg", params = {"--force-overwrite", "install", msg.package}})
                ubus.reply(req, r)
            end, {package = libubus.STRING}
        },
        remove = {
            function(req, msg)
                if not msg.package or #msg.package == 0 then
                    return UBUS_STATUS_INVALID_ARGUMENT
                end

                local r = ubus.call("file", "exec", {command = "opkg", params = {"--force-removal-of-dependent-packages", "remove", msg.package}})
                ubus.reply(req, r)
            end, {package = libubus.STRING}
        },
        upgrade = {
            function(req, msg)
                if not msg.package or #msg.package == 0 then
                    return UBUS_STATUS_INVALID_ARGUMENT
                end

                local r = ubus.call("file", "exec", {command = "opkg", params = {"upgrade", msg.package}})
                ubus.reply(req, r)
            end, {package = libubus.STRING}
        },
    },
    ["oui.network"] = {
        conntrack_count = {
            function(req, msg)
                local count = read_file("/proc/sys/net/netfilter/nf_conntrack_count", "*l")
                local limit = read_file("/proc/sys/net/netfilter/nf_conntrack_max", "*l")
                ubus.reply(req, {count = tonumber(count), limit = tonumber(limit)})
            end, {}
        },
        conntrack_table = {
            function(req, msg)
                local entries = {}

                for line in io.lines("/proc/net/nf_conntrack") do
                    local fam, l3, l4, expires, tuples = line:match("^(ipv[46]) +(%d+) +%S+ +(%d+) +(%d+) +(.+)$")

                    if fam and l3 and l4 and expires and not tuples:match("^TIME_WAIT ") then
                        local entry = {
                            ipv6 = fam == "ipv6",
                            protocol = tonumber(l4),
                            expires = tonumber(expires)
                        }

                        local key, val

                        for key, val in tuples:gmatch("(%w+)=(%S+)") do
                            if (key == "src" or key == "dst") and not entry[key] then
                                entry[key] = val
                            elseif key == "sport" or key == "dport" and not entry[key] then
                                entry[key] = tonumber(val)
                            elseif key == "packets" then
                                if entry["rx_packets"] then
                                    entry["tx_packets"] = tonumber(val)
                                else
                                    entry["rx_packets"] = tonumber(val)
                                end
                            elseif key == "bytes" then
                                if entry["rx_bytes"] then
                                    entry["tx_bytes"] = tonumber(val)
                                else
                                    entry["rx_bytes"] = tonumber(val)
                                end
                            end
                        end

                        entries[#entries + 1] = entry
                    end
                end

                ubus.reply(req, {entries = entries})
            end, {}
        },
        arp_table = {
            function(req, msg)
                local entries = {}
                local r, lines = pcall(io.lines, "/proc/net/arp")
                if r then
                    for line in lines do
                        local ipaddr, macaddr, device = line:match("(%S+) +%S+ +%S+ +(%S+) +%S+ +(%S+)")

                        if ipaddr ~= "IP" then
                            entries[#entries + 1] = {
                                ipaddr = ipaddr,
                                macaddr = macaddr,
                                device = device
                            }
                        end
                    end
                end
                ubus.reply(req, {entries = entries})
            end, {}
        },
        dhcp_leases = {
            function(req, msg)
                local leases = {}
                local leasefile = dnsmasq_leasefile()

                local r, lines = pcall(io.lines, leasefile)
                if r then
                    for line in lines do
                        local ts, mac, addr, name = line:match("(%S+) +(%S+) +(%S+) +(%S+)")
                        leases[#leases + 1] = {
                            ipaddr = addr,
                            macaddr = mac,
                            hostname = name ~= "*" and name,
                            expires = ts - os.time()
                        }
                    end
                end

                ubus.reply(req, {leases = leases})
            end, {}
        },
        dhcp6_leases = {
            function(req, msg)
                local leases = {}

                local r, lines = pcall(io.lines, "/tmp/hosts/6relayd")
                if r then
                    for line in lines do
                        local duid, name, ts, addr = line:match("^[^#]%S+ (%S+) +%S+ +(%S+) +(%S+) +%S+ +%S+ +(%S+)")
                        leases[#leases + 1] = {
                            ip6addr = addr,
                            duid = duid,
                            name = name ~= "-" and name,
                            expires = ts - os.time()
                        }
                    end
                else
                    local r, lines = pcall(io.lines, dnsmasq_leasefile())
                    if r then
                        for line in lines do
                            local ts, mac, addr, name, duid  = line:match("(%S+) +(%S+) +(%S+) +(%S+) +(%S+)")

                            if ts and mac and addr and name and duid and addr:find(":") then
                                leases[#leases + 1] = {
                                    ip6addr = addr,
                                    duid = duid ~= "*" and name,
                                    name = name ~= "*" and name,
                                    expires = ts - os.time()
                                }
                            end
                        end
                    end
                end

                ubus.reply(req, {leases = leases})
            end, {}
        },
        routes = {
            function(req, msg)
                local routes = {}
                for line in io.lines("/proc/net/route") do
                    local field = {}
                    for e in line:gmatch("%S+") do
                        field[#field + 1] = e
                    end

                    if field[2] ~= "Destination" then
                        routes[#routes + 1] = {
                            target = parse_route_addr(field[2], field[8]),
                            nexthop = parse_route_addr(field[3]),
                            metric = field[7],
                            device = field[1]
                        }
                    end
                end
                ubus.reply(req, {routes = routes})
            end, {}
        },
        routes6 = {
            function(req, msg)
                local routes = {}
                for line in io.lines("/proc/net/ipv6_route") do
                    local field = {}
                    for e in line:gmatch("%S+") do
                        field[#field + 1] = e
                    end

                    routes[#routes + 1] = {
                        target = parse_route6_addr(field[1], field[2]),
                        source = parse_route6_addr(field[3], field[4]),
                        nexthop = parse_route6_addr(field[5]),
                        metric = tonumber(field[6], 16),
                        device = field[10]
                    }
                end
                ubus.reply(req, {routes = routes})
            end, {}
        },
        switch_info = {
            function(req, msg)
                if not msg.switch then return UBUS_STATUS_INVALID_ARGUMENT end

                local info = {}
                local f = io.popen("swconfig dev " .. msg.switch .. " help")
                if f then
                    local line = f:read("*l")
                    local model, num_ports, cpu_port, num_vlans = line:match("%((%S+)%), ports: (%d+) %(cpu @ (%d+)%), vlans: (%d+)")

                    info.model = model
                    info.num_ports = num_ports
                    info.cpu_port = cpu_port
                    info.num_vlans = num_vlans

                    local attributes
                    for line in f:lines() do
                        local typ = line:match("%-%-(%S+)")
                        if typ then
                            info[typ] = {}
                            attributes = info[typ]
                        else
                            local typ, name, description = line:match("%((%a+)%): (%S+) %((.+)%)$")
                            if typ then
                                attributes[#attributes + 1] = {
                                    type = typ,
                                    name = name,
                                    description = description
                                }
                            end
                        end
                    end

                    f:close()
                end
                ubus.reply(req, {info = info})
            end, {switch = libubus.STRING}
        },
        switch_status = {
            function(req, msg)
                if not msg.switch then return UBUS_STATUS_INVALID_ARGUMENT end

                local ports = {}
                local f = io.popen("swconfig dev " .. msg.switch .. " show")
                if f then
                    for line in f:lines() do
                        if line:match("link") then
                            local up = false
                            local speed = 0
                            local duplex = false
                            local txflow = false
                            local rxflow = false
                            local aneg = false

                            for field in line:gmatch("%S+") do
                                if field:match("link:up") then
                                    up = true
                                elseif field:match("speed:") then
                                    speed = tonumber(field:match("speed:(%d+)"))
                                elseif field == "txflow" then
                                    txflow = true
                                elseif field == "rxflow" then
                                    rxflow = true
                                elseif field == "full-duplex" then
                                    duplex = true
                                elseif field == "auto" then
                                    aneg = true
                                end
                            end

                            ports[#ports + 1] = {
                                link = up,
                                rx_flow_control = rxflow,
                                tx_flow_control = txflow,
                                full_duplex = duplex,
                                auto_negotiation = aneg,
                                speed = speed
                            }
                        end
                    end
                    f:close()
                end
                ubus.reply(req, {ports = ports})
            end, {switch = libubus.STRING}
        },
        ping = {
            function(req, msg)
                if not msg.name then return UBUS_STATUS_INVALID_ARGUMENT end
                ubus.reply(req, network_cmd(msg.name, "ping"))
            end, {name = libubus.STRING}
        },
        ping6 = {
            function(req, msg)
                if not msg.name then return UBUS_STATUS_INVALID_ARGUMENT end
                ubus.reply(req, network_cmd(msg.name, "ping6"))
            end, {name = libubus.STRING}
        },
        traceroute = {
            function(req, msg)
                if not msg.name then return UBUS_STATUS_INVALID_ARGUMENT end
                ubus.reply(req, network_cmd(msg.name, "traceroute"))
            end, {name = libubus.STRING}
        },
        traceroute6 = {
            function(req, msg)
                if not msg.name then return UBUS_STATUS_INVALID_ARGUMENT end
                ubus.reply(req, network_cmd(msg.name, "traceroute6"))
            end, {name = libubus.STRING}
        },
        nslookup = {
            function(req, msg)
                if not msg.name then return UBUS_STATUS_INVALID_ARGUMENT end
                ubus.reply(req, network_cmd(msg.name, "nslookup"))
            end, {name = libubus.STRING}
        },
        ifup = {
            function(req, msg)
                if not msg.name then return UBUS_STATUS_INVALID_ARGUMENT end
                ubus.reply(req, network_ifupdown(msg.name, true))
            end, {name = libubus.STRING}
        },
        ifdown = {
            function(req, msg)
                if not msg.name then return UBUS_STATUS_INVALID_ARGUMENT end
                ubus.reply(req, network_ifupdown(msg.name, false))
            end, {name = libubus.STRING}
        },
        bwm = {
            function(req, msg)
                local entries = {}
                local r, lines = pcall(io.lines, "/proc/oui/term")
                if r then
                    for line in lines do
                        local mac, ip, rx, tx = line:match("(%S+) +(%S+) +(%S+) +(%S+)")

                        if mac and mac ~= "MAC" then
                            entries[#entries + 1] = {
                                macaddr = mac,
                                ipaddr = ip,
                                rx = {parse_flow(rx)},
                                tx = {parse_flow(tx)}
                            }
                        end
                    end
                end
                ubus.reply(req, {entries = entries})
            end, {name = libubus.STRING}
        }
    },
    ["oui.system"] = {
        syslog = {
            function(req, msg)
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
                ubus.reply(req, {log = log})
            end, {}
        },
        dmesg = {
            function(req, msg)
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
                ubus.reply(req, {log = log})
            end, {}
        },
        diskfree = {
            function(req, msg)
                local fslist = {
                    root = "/",
                    tmp = "/tmp"
                }
                local resp = {}

                for name, path in pairs(fslist) do
                    local total, free, used = statvfs(path)
                    if total then
                        resp[name] = {
                            total = total,
                            free = free,
                            used = used
                        }
                    end
                end

                ubus.reply(req, resp)
            end, {}
        },
        process_list = {
            function(req, msg)
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
                ubus.reply(req, {processes = processes})
            end, {}
        },
        process_signal = {
            function(req, msg)
                if not msg.pid or not msg.signal then return UBUS_STATUS_INVALID_ARGUMENT end
                local cmd = string.format("kill -%d %d", msg.signal, msg.pid)
                os.execute(cmd)
            end, {pid = libubus.INT32, signal = libubus.INT32}
        },
        init_list = {
            function(req, msg)
                local initscripts = {}
                local f = io.popen("ls /etc/init.d")
                if f then
                    for name in f:lines() do
                        local start, stop, enabled = false
                        local line = read_file("/etc/init.d/" .. name, "*l")
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
                                if read_file("/etc/rc.d/S" .. start .. name, 1) then
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
                ubus.reply(req, {initscripts = initscripts})
            end, {}
        },
        init_action = {
            function(req, msg)
                if not msg.name then
                    return UBUS_STATUS_INVALID_ARGUMENT
                end

                if msg.action ~= "start" and msg.action ~= "stop" and
                    msg.action ~= "reload" and msg.action ~= "restart" and
                    msg.action ~= "enable" and msg.action ~= "disable" then
                    return UBUS_STATUS_INVALID_ARGUMENT
                end

                local cmd = string.format("/etc/init.d/%s %s &", msg.name, msg.action)
                os.execute(cmd)
            end, {name = libubus.STRING, action = libubus.STRING}
        },
        crontab_get = {
            function(req, msg)
                local entries = {}
                local r, lines = pcall(io.lines, "/etc/crontabs/root")
                if r then
                    for line in lines do
                        local min, hour, day, month, week, cmd = line:match("(%S+) +(%S+) +(%S+) +(%S+) +(%S+) +(%S+)")
                        entries[#entries + 1] = {
                            min = min,
                            hour = hour,
                            day = day,
                            month = month,
                            week = week,
                            command = cmd
                        }
                    end
                end
                ubus.reply(req, {entries = entries})
            end, {}
        },
        crontab_set = {
            function(req, msg)
                if not msg.data then return UBUS_STATUS_INVALID_ARGUMENT end
                os.execute("mkdir -p /etc/crontabs")
                write_file("/etc/crontabs/root", msg.data)
            end, {data = libubus.STRING}
        },
        led_list = {
            function(req, msg)
                local leds = {}
                local f = io.popen("ls /sys/class/leds")
                if f then
                    for name in f:lines() do
                        local data = read_file("/sys/class/leds/" .. name .. "/trigger")
                        local active_trigger, brightness, max_brightness
                        local triggers = {}

                        for trigger in data:gmatch("%S+") do
                            if trigger:sub(1, 1) == "[" then
                                trigger = trigger:sub(2, #trigger - 1)
                                active_trigger = trigger
                            end
                            triggers[#triggers + 1] = trigger
                        end

                        brightness = read_file("/sys/class/leds/" .. name .. "/brightness", "*l")
                        max_brightness = read_file("/sys/class/leds/" .. name .. "/max_brightness", "*l")

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
                ubus.reply(req, {leds = leds})
            end, {}
        },
        backup_restore = {
            function(req, msg)
                local resp = ubus.call("file", "exec", {command = "sysupgrade", params = {"--restore-backup", '/tmp/backup.tar.gz'}})
                ubus.reply(req, resp)
            end, {}
        },
        backup_clean = {
            function(req, msg)
                os.execute("rm -f /tmp/backup.tar.gz")
            end, {}
        },
        cpu_time = {
            function(req, msg)
                local line = read_file("/proc/stat", "*l")
                local times = {}
                for t in line:sub(4):gmatch("%S+") do
                    times[#times + 1] = tonumber(t)
                end
                ubus.reply(req, {times = times})
            end, {}
        },
        read_dir = {
            function(req, msg)
                local path = msg.path or "/"
                local files = readdir(path)

                path = path:gsub("/$", "")

                for _, file in ipairs(files) do
                    file.path = path .. "/" .. file.name
                end

                ubus.reply(req, {files = files})
            end, {path = libubus.STRING}
        },
        new_file = {
            function(req, msg)
                local path = msg.path
                local dir = msg.dir
                local cmd = "touch"

                if dir then cmd = "mkdir" end

                local r = os.execute(cmd .. " " .. path)

                ubus.reply(req, {r = r})
            end, {path = libubus.STRING, dir = libubus.BOOLEAN}
        }
    }
}

function ubus_init()
    local conn = libubus.connect()
    if not conn then
        error("Failed to connect to ubus")
    end

    conn:add(methods)

    return {
        call = function(object, method, params)
            return conn:call(object, method, params or {})
        end,
        reply = function(req, msg)
            conn:reply(req, msg)
        end
    }
end

local function main()
    uloop.init()

    ubus = ubus_init()

    uloop.run()
end

main()

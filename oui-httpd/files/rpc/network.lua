local uci = require "uci"

local M = {}

function M.bwm()
	local entries = {}
    local r, lines = pcall(io.lines, "/proc/oui/term")
    if r then
        for line in lines do
            local mac, ip, rx, tx = line:match("(%S+) +(%S+) +(%S+) +(%S+)")

            if mac and mac ~= "MAC" then
                entries[#entries + 1] = {
                    macaddr = mac,
                    ipaddr = ip,
                    rx = rx,
                    tx = tx
                }
            end
        end
    end
    return {entries = entries}
end

local function dnsmasq_leasefile()
    local c = uci.cursor()
    local leasefile

    c:foreach("dhcp", "dnsmasq", function(s)
        leasefile = s.leasefile
        return false
    end)

    return leasefile
end

function M.dhcp_leases()
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
    return { leases = leases }
end

function M.dhcp6_leases()
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

    return { leases = leases }
end

function M.arp_table()
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

    return {entries = entries}
end

function M.routes()
    local routes = {}
    for line in io.lines("/proc/net/route") do
        local field = {}
        for e in line:gmatch("%S+") do
            field[#field + 1] = e
        end

        if field[2] ~= "Destination" then
            routes[#routes + 1] = {
                target = utils.parse_route_addr(field[2], field[8]),
                nexthop = utils.parse_route_addr(field[3]),
                metric = field[7],
                device = field[1]
            }
        end
    end
    return {routes = routes}
end

function M.routes6()
    local routes = {}
    for line in io.lines("/proc/net/ipv6_route") do
        local field = {}
        for e in line:gmatch("%S+") do
            field[#field + 1] = e
        end

        routes[#routes + 1] = {
            target = utils.parse_route6_addr(field[1], field[2]),
            source = utils.parse_route6_addr(field[3], field[4]),
            nexthop = utils.parse_route6_addr(field[5]),
            metric = tonumber(field[6], 16),
            device = field[10]
        }
    end
    return {routes = routes}
end

function M.switch_info(msg)
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
    return {info = info}
end

function M.switch_status(msg)
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
    return {ports = ports}
end

return M

local M = {}

local nl80211 = require 'eco.nl80211'
local ubus = require 'eco.ubus'

function M.stations()
    local status = ubus.call('network.wireless', 'status', {}) or {}

    local stations = {}

    for _, dev in pairs(status) do
        if dev.up then
            local band = dev.config.band

            for _, ifs in ipairs(dev.interfaces) do
                local ifname = ifs.ifname
                local res = nl80211.get_stations(ifname)

                for _, sta in pairs(res) do
                    stations[#stations + 1] = {
                        macaddr = sta.mac,
                        ifname = ifname,
                        band = band,
                        signal = sta.signal,
                        noise = sta.noise,
                        rx_rate = sta.rx_rate,
                        tx_rate = sta.tx_rate
                    }
                end
            end
        end
    end

    return { stations = stations }
end

return M

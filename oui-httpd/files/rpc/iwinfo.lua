local utils = require "oui.utils"
local iwinfo = require 'oui.iwinfo'
local rpc = require 'oui.rpc'
local fs = require "oui.fs"

local M = {}

function M.devices()
    local devices = {}

    for dev in fs.dir("/sys/class/net") do
        if iwinfo.type(dev) then
            devices[#devices + 1] = dev
        end
    end

    return { devices = devices }
end

function M.info(params)
    local device = params.device

    if type(device) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    local info = iwinfo.info(device)

    local hwmodes = {}
    local htmodes = {}

    for k, v in pairs(info['hwmodelist'] or {}) do
        if v then
            hwmodes[#hwmodes + 1] = k
        end
    end

    for k, v in pairs(info['htmodelist'] or {}) do
        if v then
            htmodes[#htmodes + 1] = k
        end
    end

    return {
        phy = info['phyname'],
        ssid = info['ssid'],
        bssid = info['bssid'],
        country = info['country'],
        mode = info['mode'],
        channel = info['channel'],
        frequency = info['frequency'],
        frequency_offset = info['frequency_offset'],
        txpower = info['txpower'],
        txpower_offset = info['txpower_offset'],
        quality_max = info['quality_max'],
        signal = info['signal'],
        noise = info['noise'],
        bitrate = info['bitrate'],
        hwmodes = hwmodes,
        htmodes = htmodes,
        encryption = info['encryption'],
        hardware = {
            id = info['hardware_id'],
            name = info['hardware_name']
        }
    }
end

function M.assoclist(params)
    local device = params.device

    if type(device) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    return iwinfo.assoclist(device)
end

function M.scan(params)
    local device = params.device

    if type(device) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    return iwinfo.scanlist(device)
end

function M.freqlist(params)
    local device = params.device

    if type(device) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    return iwinfo.freqlist(device)
end

function M.txpowerlist(params)
    local device = params.device

    if type(device) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    return iwinfo.txpwrlist(device)
end

function M.countrylist(params)
    local device = params.device

    if type(device) ~= "string" then
        return rpc.ERROR_CODE_INVALID_PARAMS
    end

    return iwinfo.countrylist(device)
end

return M

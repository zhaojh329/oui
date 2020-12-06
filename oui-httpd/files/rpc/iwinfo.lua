local utils = require "oui.utils"
local iwinfo = require 'iwinfo'

local M = {}

local function get_info(device)
	local iwtype = iwinfo.type(device)

    if not iwtype then
    	error("Not support")
    end

	return setmetatable({}, {
		__index = function(t, k)
			return iwinfo[iwtype][k] and iwinfo[iwtype][k](device)
		end
	})
end

function M.info(params)
	local device = params.device

	if type(device) ~= "string" then
        return nil, __rpc.RPC_ERROR_PARAMS
    end

    local info = get_info(device)

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

function M.freqlist(params)
    local device = params.device

	if type(device) ~= "string" then
        return nil, __rpc.RPC_ERROR_PARAMS
    end

    return get_info(device)['freqlist']
end

function M.txpowerlist(params)
    local device = params.device

	if type(device) ~= "string" then
        return nil, __rpc.RPC_ERROR_PARAMS
    end

    return get_info(device)['txpwrlist']
end

function M.countrylist(params)
    local device = params.device

	if type(device) ~= "string" then
        return nil, __rpc.RPC_ERROR_PARAMS
    end

    return get_info(device)['countrylist']
end

return M

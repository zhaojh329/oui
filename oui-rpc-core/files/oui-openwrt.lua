#!/usr/bin/env eco

-- SPDX-License-Identifier: MIT
-- Author: Jianhui Zhao <zhaojh329@gmail.com>

local rpc = require 'oui.rpc'

function rpc.reload_config(config)
    if type(config) ~= 'string' or #config == 0 then
        return
    end

    local ubus = require 'eco.ubus'

    ubus.call('service', 'event', {
        type = 'config.change',
        data = { package = config }
    })
end

dofile('/usr/share/oui/oui.lua')

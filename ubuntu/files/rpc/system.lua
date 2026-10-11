-- SPDX-License-Identifier: MIT
-- Author: Jianhui Zhao <zhaojh329@gmail.com>

local file = require 'eco.file'
local sys = require 'eco.sys'
local log = require 'eco.log'
local rpc = require 'oui.rpc'

local M = {}

function M.board()
    local release = assert(file.readfile('/etc/os-release'))
    local description = release:match('\nPRETTY_NAME="([^"]+)"') or release:match('^PRETTY_NAME="([^"]+)"')

    return {
        hostname = assert(file.readfile('/proc/sys/kernel/hostname')):match('([^\n]+)'),
        kernel = assert(file.readfile('/proc/sys/kernel/osrelease')):match('([^\n]+)'),
        release = { description = description }
    }
end

function M.info()
    local memory = {}

    for name, value in assert(file.readfile('/proc/meminfo')):gmatch('(%w+):%s+(%d+)%s+kB') do
        if name == 'MemTotal' then
            memory.total = tonumber(value) * 1024
        elseif name == 'MemAvailable' then
            memory.available = tonumber(value) * 1024
        end
    end

    return {
        uptime = tonumber(assert(file.readfile('/proc/uptime')):match('^[%d.]+')),
        memory = memory
    }
end

function M.reboot()
    -- Give the RPC response time to reach the browser before shutting down.
    local p<close>, err = sys.exec('/usr/bin/systemd-run', '--on-active=2s', '/usr/bin/systemctl', 'reboot')
    if not p then
        log.err('schedule reboot:', err)
        return nil, rpc.ERROR_CODE_UNKNOWN
    end

    local pid, status = p:wait(5)
    if not pid then
        p:kill()
        p:wait()
        log.err('schedule reboot:', status)
        return nil, rpc.ERROR_CODE_UNKNOWN
    end

    if not status.exited or status.status ~= 0 then
        log.err('schedule reboot:', p:read_stderr('*a'))
        return nil, rpc.ERROR_CODE_UNKNOWN
    end
end

return M

local uci = require 'uci'
local M = {}

--PAGE1

function M.Get_Port_Forwarding_Info()
    local c = uci.cursor()
    local redirects = {}

    c:foreach('firewall', 'redirect', function(s)
        if s.hidden ~= 1 then
            redirects[#redirects + 1] = {
                name = s.name,--
                src = s.src,--
                src_ip = s.src_ip,--
                --src_dip = s.src_dip,
                src_mac = s.src_mac,--
                src_port = s.src_port,--
                proto = s.proto,--
                dest = s.dest,--
                dest_ip = s.dest_ip,--
                dest_port = s.dest_port,--
                --ipset = s.ipset,
                mark = s.mark,--匹配标记
                --start_date = s.start_date,
                --stop_date = s.stop_date,
                --start_time = s.start_time,
                --stop_time = s.stop_time,
                --weekdays = s.weekdays,
                --monthdays = s.monthdays,
                --utc_time = s.utc_time,
                --target = s.target,
                --family = s.family,
                reflection = s.reflection,--NAT回环
                reflection_src = s.reflection_src,--环路源ip
                --reflection_zone = s.reflection_zone,
                limit = s.limit,--限制匹配
                --limit_burst = s.limit_burst,
                extra = s.extra,--额外参数
                enabled = s.enabled,
                helper = s.helper--匹配助手
            }
        end
    end)
    return redirects
end

function M.Set_Port_Forwarding_Status(params)
    local c = uci.cursor()
    local name = params.name
    local enabled = params.enabled
    local data = {}
    c:foreach('firewall', 'redirect', function(s)
        if s.name == name then
            os.execute('uci set firewall.'..s['.name']..'.enabled='..enabled)
        end
    end)
    data["enabled"] = enabled
    local ret = {data}
    return ret
end

function M.Set_Edit_Port_Forwarding_Config(params)
    local param = params
    local c = uci.cursor()

    local name = param.name
    local new_name = param.new_name

    local src = param.src
    local src_ip = param.src_ip
    local src_mac = param.src_mac
    local src_port = param.src_port
    local proto = param.proto
    local dest = param.dest
    local dest_ip = param.dest_ip
    local dest_port = param.dest_port
    helper = s.helper
    local mark = param.mark
    local reflection = param.reflection
    local reflection_src = param.reflection_src
    local limit = param.limit
    local extra = param.extra
    local flag = 0
    local sname = ''
    c:foreach('firewall', 'redirect', function(s)
        if s.name == name then
            sname = s['.name']
            flag = 2
        end
        if s.name ~= name and s.name == new_name then
            flag = 1
        end
    end)
    local data_exist = {}
    if flag == 0 then
        data_exist["err"] = "not exist"
        return {data_exist}
    end
    if flag == 1 then
        data_exist["err"] = "already exist"
        return {data_exist}
    end
    c:set('firewall', sname, 'name', new_name)
    c:set('firewall', sname, 'src', src)
    c:set('firewall', sname, 'src_ip', src_ip)
    c:set('firewall', sname, 'src_mac', src_mac)
    c:set('firewall', sname, 'src_port', src_port)
    c:set('firewall', sname, 'proto', proto)
    c:set('firewall', sname, 'dest', dest)
    c:set('firewall', sname, 'dest_ip', dest_ip)
    c:set('firewall', sname, 'dest_port', dest_port)
    c:set('firewall', sname, 'helper', helper)
    c:set('firewall', sname, 'mark', mark)
    c:set('firewall', sname, 'reflection', reflection)
    c:set('firewall', sname, 'reflection_src', reflection_src)
    c:set('firewall', sname, 'limit', limit)
    c:set('firewall', sname, 'extra', extra)
    c:commit('firewall')
    -----
    local data = {}
    data["name"] = c:get('firewall', sname, 'name')
    data["src"] = c:get('firewall', sname, 'src')
    data["src_ip"] = c:get('firewall', sname, 'src_ip')
    data["src_mac"] = c:get('firewall', sname, 'src_mac')
    data["src_port"] = c:get('firewall', sname, 'src_port')
    data["proto"] = c:get('firewall', sname, 'proto')
    data["dest"] = c:get('firewall', sname, 'dest')
    data["dest_ip"] = c:get('firewall', sname, 'dest_ip')
    data["dest_port"] = c:get('firewall', sname, 'dest_port')
    data["helper"] = c:get('firewall', sname, 'helper')
    data["mark"] = c:get('firewall', sname, 'mark')
    data["reflection"] = c:get('firewall', sname, 'reflection')
    data["reflection_src"] = c:get('firewall', sname, 'reflection_src')
    data["limit"] = c:get('firewall', sname, 'limit')
    data["extra"] = c:get('firewall', sname, 'extra')
    data["enabled"] = c:get('firewall', sname, 'enabled')
    local ret = {data}
    return ret
end

function M.Set_Delete_Port_Forwarding_Config(params)
    local c = uci.cursor()
    local name = params.name
    local data = {}
    c:foreach('firewall', 'redirect', function(s)
        if s.name == name then
            c:delete('firewall', s['.name'])
            c:commit('firewall')
        end
    end)
end

--PAGE1-1
function M.Set_New_Port_Forwarding_Info(params)
    local param = params
    local c = uci.cursor()

    local name = param.name
    local src = param.src
    local src_ip = param.src_ip
    local src_mac = param.src_mac
    local src_port = param.src_port
    local proto = param.proto
    local dest = param.dest
    local dest_ip = param.dest_ip
    local dest_port = param.dest_port
    local helper = param.helper
    local mark = param.mark
    local reflection = param.reflection
    local reflection_src = param.reflection_src
    local limit = param.limit
    local extra = param.extra

    c:foreach('firewall', 'redirect', function(s)
        if s.name == name then
            exist = true
            return false
        end
    end)
    local data_exist = {}
    data_exist["err"] = "already exist"
    if exist then
        return {data_exist}
    end
    local sid = c:add('firewall', 'redirect')
    c:set('firewall', sid, 'name', name)
    c:set('firewall', sid, 'src', src)
    c:set('firewall', sid, 'src_ip', src_ip)
    c:set('firewall', sid, 'src_mac', src_mac)
    c:set('firewall', sid, 'src_port', src_port)
    c:set('firewall', sid, 'proto', proto)
    c:set('firewall', sid, 'dest', dest)
    c:set('firewall', sid, 'dest_ip', dest_ip)
    c:set('firewall', sid, 'dest_port', dest_port)
    c:set('firewall', sid, 'helper', helper)
    c:set('firewall', sid, 'mark', mark)
    c:set('firewall', sid, 'reflection', reflection)
    c:set('firewall', sid, 'reflection_src', reflection_src)
    c:set('firewall', sid, 'limit', limit)
    c:set('firewall', sid, 'extra', extra)
    c:set('firewall', sid, 'enabled', "1")
    c:commit('firewall')
    -----
    local data = {}
    data["name"] = c:get('firewall', sid, 'name')
    data["src"] = c:get('firewall', sid, 'src')
    data["src_ip"] = c:get('firewall', sid, 'src_ip')
    data["src_mac"] = c:get('firewall', sid, 'src_mac')
    data["src_port"] = c:get('firewall', sid, 'src_port')
    data["proto"] = c:get('firewall', sid, 'proto')
    data["dest"] = c:get('firewall', sid, 'dest')
    data["dest_ip"] = c:get('firewall', sid, 'dest_ip')
    data["dest_port"] = c:get('firewall', sid, 'dest_port')
    data["helper"] = c:get('firewall', sid, 'helper')
    data["mark"] = c:get('firewall', sid, 'mark')
    data["reflection"] = c:get('firewall', sid, 'reflection')
    data["reflection_src"] = c:get('firewall', sid, 'reflection_src')
    data["limit"] = c:get('firewall', sid, 'limit')
    data["extra"] = c:get('firewall', sid, 'extra')
    data["enabled"] = c:get('firewall', sid, 'enabled')
    local ret = {data}
    return ret
end

--PAGE2

function M.Get_Device_Firewall_Info()
    local c = uci.cursor()
    local rules = {}

    c:foreach('firewall', 'rule', function(s)
        if s.hidden ~= 1 then
            rules[#rules + 1] = {
                name = s.name,--
                src = s.src,--
                src_ip = s.src_ip,--
                src_mac = s.src_mac,--
                src_port = s.src_port,--
                proto = s.proto,--
                --icmp_type = s.icmp_type,
                dest = s.dest,--
                dest_ip = s.dest_ip,--
                dest_port = s.dest_port,--
                --ipset = s.ipset,
                mark = s.mark,--
                start_date = s.start_date,--
                stop_date = s.stop_date,--
                start_time = s.start_time,--
                stop_time = s.stop_time,--
                weekdays = s.weekdays,--
                monthdays = s.monthdays,--
                utc_time = s.utc_time,--
                target = s.target,--操作
                --set_mark = s.set_mark,
                --set_xmark = s.set_xmark,
                family = s.family,--限制IP地址类型
                limit = s.limit,--限制匹配
                --limit_burst = s.limit_burst,
                extra = s.extra,--
                enabled = s.enabled,--
                device = s.device,--
                --direction = s.direction,
                --set_helper = s.set_helper,
                helper = s.helper--
                --DSCP QOS???
            }
        end
    end)
    return rules
end

function M.Set_Device_Firewall_Status(params)
    local c = uci.cursor()
    local name = params.name
    local enabled = params.enabled
    local data = {}
    c:foreach('firewall', 'rule', function(s)
        if s.name == name then
            os.execute('uci set firewall.'..s['.name']..'.enabled='..enabled)
        end
    end)
    data["enabled"] = enabled
    local ret = {data}
    return ret
end

function M.Set_Edit_Device_Firewall_Config(params)
    local param = params
    local c = uci.cursor()

    local name = param.name
    local new_name = param.new_name

    local src = param.src--
    local src_ip = param.src_ip--
    local src_mac = param.src_mac--
    local src_port = param.src_port--
    local proto = param.proto--
    --local icmp_type = param.icmp_type
    local dest = param.dest--
    local dest_ip = param.dest_ip--
    local dest_port = param.dest_port--
    --local ipset = param.ipset
    local mark = param.mark--
    local start_date = param.start_date--
    local stop_date = param.stop_date--
    local start_time = param.start_time--
    local stop_time = param.stop_time--
    local weekdays = param.weekdays--
    local monthdays = param.monthdays--
    local utc_time = param.utc_time--
    local target = param.target--操作
    --local set_mark = s.set_mark
    --local set_xmark = s.set_xmark
    local family = param.family--限制IP地址类型
    local limit = param.limit--限制匹配
    --local limit_burst = s.limit_burst
    local extra = param.extra--
    local enabled = param.enabled--
    local device = param.device--
    --local direction = s.direction
    --local set_helper = s.set_helper
    local helper = param.helper--
    --DSCP QOS???

    c:foreach('firewall', 'rule', function(s)
        if s.name == name then
            sname = s['.name']
            flag = 2
        end
        if s.name ~= name and s.name == new_name then
            flag = 1
        end
    end)
    local data_exist = {}
    if flag == 0 then
        data_exist["err"] = "not exist"
        return {data_exist}
    end
    if flag == 1 then
        data_exist["err"] = "already exist"
        return {data_exist}
    end
    c:set('firewall', sname, 'name', new_name)
    c:set('firewall', sid, 'src', src)
    c:set('firewall', sid, 'src_ip', src_ip)
    c:set('firewall', sid, 'src_mac', src_mac)
    c:set('firewall', sid, 'src_port', src_port)
    c:set('firewall', sid, 'proto', proto)
    c:set('firewall', sid, 'dest', dest)
    c:set('firewall', sid, 'dest_ip', dest_ip)
    c:set('firewall', sid, 'dest_port', dest_port)
    c:set('firewall', sid, 'helper', helper)
    c:set('firewall', sid, 'mark', mark)
    c:set('firewall', sid, 'start_date', start_date)
    c:set('firewall', sid, 'stop_date', stop_date)
    c:set('firewall', sid, 'start_time', start_time)
    c:set('firewall', sid, 'stop_time', stop_time)
    c:set('firewall', sid, 'weekdays', weekdays)
    c:set('firewall', sid, 'monthdays', monthdays)
    c:set('firewall', sid, 'utc_time', utc_time)
    c:set('firewall', sid, 'target', target)
    c:set('firewall', sid, 'utc_time', utc_time)
    c:set('firewall', sid, 'family', family)
    c:set('firewall', sid, 'limit', limit)
    c:set('firewall', sid, 'extra', extra)
    c:set('firewall', sid, 'enabled', "1")
    c:set('firewall', sid, 'device', device)
    c:set('firewall', sid, 'helper', helper)
    c:commit('firewall')
    -----
    local data = {}
    data["name"] = c:get('firewall', sid, 'name')
    data["src"] = c:get('firewall', sid, 'src')
    data["src_ip"] = c:get('firewall', sid, 'src_ip')
    data["src_mac"] = c:get('firewall', sid, 'src_mac')
    data["src_port"] = c:get('firewall', sid, 'src_port')
    data["proto"] = c:get('firewall', sid, 'proto')
    data["dest"] = c:get('firewall', sid, 'dest')
    data["dest_ip"] = c:get('firewall', sid, 'dest_ip')
    data["dest_port"] = c:get('firewall', sid, 'dest_port')
    data["helper"] = c:get('firewall', sid, 'helper')
    data["mark"] = c:get('firewall', sid, 'mark')
    data["start_date"] = c:get('firewall', sid, 'start_date')
    data["stop_date"] = c:get('firewall', sid, 'stop_date')
    data["start_time"] = c:get('firewall', sid, 'start_time')
    data["stop_time"] = c:get('firewall', sid, 'stop_time')
    data["weekdays"] = c:get('firewall', sid, 'weekdays')
    data["monthdays"] = c:get('firewall', sid, 'monthdays')
    data["utc_time"] = c:get('firewall', sid, 'utc_time')
    data["family"] = c:get('firewall', sid, 'family')
    data["limit"] = c:get('firewall', sid, 'limit')
    data["extra"] = c:get('firewall', sid, 'extra')
    data["enabled"] = c:get('firewall', sid, 'enabled')
    data["device"] = c:get('firewall', sid, 'device')
    data["helper"] = c:get('firewall', sid, 'helper')
    local ret = {data}
    return ret
end

function M.Set_Delete_Device_Firewall_Config(params)
    local c = uci.cursor()
    local name = params.name
    local data = {}
    c:foreach('firewall', 'rule', function(s)
        if s.name == name then
            c:delete('firewall', s['.name'])
            c:commit('firewall')
        end
    end)
end

--PAGE2-1
function M.Set_New_Firewall_Info(params)
    local param = params
    local c = uci.cursor()

    local name = param.name--
    local src = param.src--
    local src_ip = param.src_ip--
    local src_mac = param.src_mac--
    local src_port = param.src_port--
    local proto = param.proto--
    --local icmp_type = param.icmp_type
    local dest = param.dest--
    local dest_ip = param.dest_ip--
    local dest_port = param.dest_port--
    --local ipset = param.ipset,
    local mark = param.mark--
    local start_date = param.start_date--
    local stop_date = param.stop_date--
    local start_time = param.start_time--
    local stop_time = param.stop_time--
    local weekdays = param.weekdays--
    local monthdays = param.monthdays--
    local utc_time = param.utc_time--
    local target = param.target--操作
    --local set_mark = s.set_mark
    --local set_xmark = s.set_xmark
    local family = param.family--限制IP地址类型
    local limit = param.limit--限制匹配
    --local limit_burst = s.limit_burst
    local extra = param.extra--
    local enabled = param.enabled--
    local device = param.device--
    --local direction = s.direction
    --local set_helper = s.set_helper
    local helper = param.helper--
    --DSCP QOS???

    c:foreach('firewall', 'rule', function(s)
        if s.name == name then
            exist = true
            return false
        end
    end)
    local data_exist = {}
    data_exist["err"] = "already exist"
    if exist then
        return {data_exist}
    end
    local sid = c:add('firewall', 'rule')
    c:set('firewall', sid, 'name', name)
    c:set('firewall', sid, 'src', src)
    c:set('firewall', sid, 'src_ip', src_ip)
    c:set('firewall', sid, 'src_mac', src_mac)
    c:set('firewall', sid, 'src_port', src_port)
    c:set('firewall', sid, 'proto', proto)
    c:set('firewall', sid, 'dest', dest)
    c:set('firewall', sid, 'dest_ip', dest_ip)
    c:set('firewall', sid, 'dest_port', dest_port)
    c:set('firewall', sid, 'helper', helper)
    c:set('firewall', sid, 'mark', mark)
    c:set('firewall', sid, 'start_date', start_date)
    c:set('firewall', sid, 'stop_date', stop_date)
    c:set('firewall', sid, 'start_time', start_time)
    c:set('firewall', sid, 'stop_time', stop_time)
    c:set('firewall', sid, 'weekdays', weekdays)
    c:set('firewall', sid, 'monthdays', monthdays)
    c:set('firewall', sid, 'utc_time', utc_time)
    c:set('firewall', sid, 'target', target)
    c:set('firewall', sid, 'utc_time', utc_time)
    c:set('firewall', sid, 'family', family)
    c:set('firewall', sid, 'limit', limit)
    c:set('firewall', sid, 'extra', extra)
    c:set('firewall', sid, 'enabled', "1")
    c:set('firewall', sid, 'device', device)
    c:set('firewall', sid, 'helper', helper)
    c:commit('firewall')
    -----
    local data = {}
    data["name"] = c:get('firewall', sid, 'name')
    data["src"] = c:get('firewall', sid, 'src')
    data["src_ip"] = c:get('firewall', sid, 'src_ip')
    data["src_mac"] = c:get('firewall', sid, 'src_mac')
    data["src_port"] = c:get('firewall', sid, 'src_port')
    data["proto"] = c:get('firewall', sid, 'proto')
    data["dest"] = c:get('firewall', sid, 'dest')
    data["dest_ip"] = c:get('firewall', sid, 'dest_ip')
    data["dest_port"] = c:get('firewall', sid, 'dest_port')
    data["helper"] = c:get('firewall', sid, 'helper')
    data["mark"] = c:get('firewall', sid, 'mark')
    data["start_date"] = c:get('firewall', sid, 'start_date')
    data["stop_date"] = c:get('firewall', sid, 'stop_date')
    data["start_time"] = c:get('firewall', sid, 'start_time')
    data["stop_time"] = c:get('firewall', sid, 'stop_time')
    data["weekdays"] = c:get('firewall', sid, 'weekdays')
    data["monthdays"] = c:get('firewall', sid, 'monthdays')
    data["utc_time"] = c:get('firewall', sid, 'utc_time')
    data["family"] = c:get('firewall', sid, 'family')
    data["limit"] = c:get('firewall', sid, 'limit')
    data["extra"] = c:get('firewall', sid, 'extra')
    data["enabled"] = c:get('firewall', sid, 'enabled')
    data["device"] = c:get('firewall', sid, 'device')
    data["helper"] = c:get('firewall', sid, 'helper')
    local ret = {data}
    return ret
end

--PAGE3
--PAGE4

--PAGE5

function M.Get_System_Log()
    -----
    local data = {}
    local cmd_exe= io.popen('logread')
    local cmd_output = remove_n(cmd_exe:read("*all"))
    data["log"] = cmd_output
    local ret = {data}
    return ret
end

--PAGE6

function M.Get_Kernel_Log()
    local data = {}
    local cmd_exe= io.popen('dmesg')
    local cmd_output = remove_n(cmd_exe:read("*all"))
    data["log"] = cmd_output
    local ret = {data}
    return ret
end

--PAGE7
function M.Get_Whitelist_Config()
    -----
    local c = uci.cursor()
    local white = {}

    c:foreach('firewall', 'rule', function(s)
        if s.whitelist == '1' then
            white[#white + 1] = {
                name = s.name,
                mac = s.src_mac
            }
        end
    end)
    return white
end

function M.Set_Whitelist_Status_ON(params)
    local c = uci.cursor()
    local data = {}
    s.execute("uci set firewall.@zone[1].input='REJECT'")
    ret["status"] = "ON REJECT"
    return ret
end

function M.Set_Whitelist_Status_OFF(params)
    local c = uci.cursor()
    local data = {}
    s.execute("uci set firewall.@zone[1].input='ACCEPT'")
    ret["status"] = "OFF ACCEPT"
    return ret
end

--PAGE7-1
function M.Set_New_Whitelist(params)
    local c = uci.cursor()

    local name = params.name
    local mac = params.src_mac

    local sid = c:add('firewall', 'rule')
    c:set('firewall', sid, 'name', name)
    c:set('firewall', sid, 'src_mac',src_mac)
    c:set('firewall', sid, 'enabled','1')
    c:set('firewall', sid, 'target','ACCEPT')
    c:set('firewall', sid, 'hidden','1')
    -----
    local data = {}
    data["name"] = c:get('firewall', sid, 'name')
    data["src_mac"] = c:get('firewall', sid, 'src_mac')
    local ret = {data}
    return ret
end



--提交防火墙修改
function M.Firewall_Commit()
    os.execute('uci commit firewall')
    os.execute('/etc/init.d/firewall restart')
end

return M

    
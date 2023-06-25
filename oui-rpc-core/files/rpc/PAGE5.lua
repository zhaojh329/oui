local uci = require 'uci'

local M = {}



function remove_n(target)
    if(string.sub(target,-1,-1) == '\n') --去除\n
    then
        target = string.sub(target,1,-2)
    end
    return target
end

--PAGE1

function M.Get_Language_Config()
    local c = uci.cursor()
    local data = {}
    data["theme"] = c:get('oui', 'global', 'theme')
    data["language"] = c:get('oui', 'global', 'language')
    local ret = {data}
    return ret
end

function M.Set_Language_Config(params)
    local c = uci.cursor()
    local language = params.language
    local theme = params.theme
    c:set('oui', 'global', 'theme', theme)
    c:set('oui', 'global', 'language', language)
    -----
    local data = {}
    data["theme"] = c:get('oui', 'global', 'theme')
    data["language"] = c:get('oui', 'global', 'language')
    local ret = {data}
    return ret
end

function M.Get_Hostname_Config()
    -----
    local data = {}
    local cmd_exe= io.popen('uci get system.@system[0].hostname')
    local cmd_output = remove_n(cmd_exe:read("*all"))
    data["hostname"] = cmd_output
    local ret = {data}
    return ret
end

function M.Set_Hostname_Config(params)
    local hostname = params.hostname
    -----
    local data = {}
    os.execute('uci set system.@system[0].hostname='..hostname)
    local cmd_exe= io.popen('uci get system.@system[0].hostname')
    local cmd_output = remove_n(cmd_exe:read("*all"))
    data["hostname"] = cmd_output
    local ret = {data}
    return ret
end

function Get_Log_Config()
    -----
    local data = {}
    data["log_buffer_size"] = "64"
    data["extern_log_server_ip_addr"] = "0.0.0.0"
    data["extern_log_server_port"] = "114"
    data["extern_log_server_proto"] = "UDP"
    data["log_file_addr"] = "/tmp"
    data["log_level"] = "Debug"
    data["corn_log_level"] = "Warning"
    local ret = {data}
    return ret
end

function M.Get_Device_Log_Config()
    local ret = Get_Log_Config()
    return ret
end

function M.Set_Device_Log_Config(params)
    local log_buffer_size = params.log_buffer_size
    local extern_log_server_ip_addr = params.extern_log_server_ip_addr
    local extern_log_server_port = params.extern_log_server_port
    local extern_log_server_proto = params.extern_log_server_proto
    local log_file_addr = params.log_file_addr
    local log_level = params.log_level
    local corn_log_level = params.corn_log_level
    -----
    local ret = Get_Log_Config()
    return ret
end

function M.Get_TZ_Config() --https://openwrt.org/zh/docs/guide-user/base-system/system_configuration
    -----
    local data = {}
    local cmd_exe= io.popen('uci get system.@system[0].timezone')
    local cmd_output = remove_n(cmd_exe:read("*all"))
    data["tz"] = cmd_output
    local ret = {data}
    return ret
end

function M.Set_TZ_Config(params)
    local tz = params.tz
    -----
    local data = {}
    os.execute('uci set system.@system[0].tz='..tz)
    local cmd_exe= io.popen('uci get system.@system[0].tz')
    local cmd_output = remove_n(cmd_exe:read("*all"))
    data["tz"] = cmd_output
    local ret = {data}
    return ret
end

function M.Get_Systime()
    -----
    local data = {}
    local cmd_exe= io.popen('date')
    local cmd_output = remove_n(cmd_exe:read("*all"))
    data["time"] = cmd_output
    local ret = {data}
    return ret
end

--date -s '2019-10-18 00:00:00'
--推荐使用 “YYYY-MM-DD hh:mm:ss”的格式进行时间设置
function M.Set_Systime(params)
    local time_to_set = params.time
    -----
    local data = {}
    os.execute('date -s '..'"'..time_to_set..'"')
    local cmd_exe= io.popen('date')
    local cmd_output = remove_n(cmd_exe:read("*all"))
    data["time"] = cmd_output
    local ret = {data}
    return ret
end

--PAGE2

function M.Sysupgrade_Auto()
    -----
    local data = {}
    data["return"] = "1"
    local ret = {data}
    return ret
end

function M.Sysupgrade_Manual()
    -----
    local data = {}
    data["return"] = "1"
    local ret = {data}
    return ret
end

function M.Sysupgrade_Flash(params)
    local path = params.path
    -----
    local data = {}
    data["path"] = path
    local ret = {data}
    return ret
end

function M.Sysupgrade_Factary()
    -----
    local data = {}
    data["return"] = "1"
    local ret = {data}
    return ret
end

function M.Sysupgrade_Backup(params)
    local path = params.path
    -----
    local data = {}
    data["path"] = path
    local ret = {data}
    return ret
end

function M.Sysupgrade_Rec(params)
    local path = params.path
    -----
    local data = {}
    data["path"] = path
    local ret = {data}
    return ret
end

--PAGE3

function M.Get_User_Config()
    local c = uci.cursor()
    local users = {}

    c:foreach('oui', 'user', function(s)
        users[#users + 1] = {
            name = s.username,
            phone = s.phone,
            role = s.role,
            id = s.id,
            status = s.status,
            create_time = s.create_time
        }
    end)

    return users
end

function M.Set_User_Status(params)
    local c = uci.cursor()
    local username = params.name
    local status = params.status
    local data = {}
    c:foreach('oui', 'user', function(s)
        if s.username == username then
            os.execute('uci set oui.'..s['.name']..'.status='..status)
        end
    end)
    data["status"] = status
    local ret = {data}
    return ret
end

function M.Set_Edit_User_Config(params)
    local param = params
    local c = uci.cursor()
    local id = param.id
    local password = param.password
    local name = param.name
    local new_name = param.new_name
    local phone = param.phone
    local acl = "admin"
    local role = param.role
    local flag = 0
    local sname =''
    c:foreach('oui', 'user', function(s)
        if s.username == name then
            sname = s['.name']
            flag = 2
        end
        if s.username ~= name and s.username == new_name then
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
    c:set('oui', sname, 'id', id)
    c:set('oui', sname, 'username', new_name)
    c:set('oui', sname, 'password', ngx.md5(name .. ':' .. password))
    c:set('oui', sname, 'phone', phone)
    c:set('oui', sname, 'role', role)
    c:set('oui', sname, 'acl', acl or "")
    local cmd_exe= io.popen('date')
    local cmd_output = remove_n(cmd_exe:read("*all"))
    c:set('oui', sname, 'create_time', cmd_output)
    c:commit('oui')
    -----
    local data = {}
    data["id"] = c:get('oui', sname, 'id')
    data["name"] = c:get('oui', sname, 'name')
    data["phone"] = c:get('oui', sname, 'phone')
    data["role"] = c:get('oui', sname, 'role')
    data["create_time"] = c:get('oui', sname, 'create_time')
    local ret = {data}
    return ret
end

function M.Set_Delete_User_Config(params)
    local c = uci.cursor()
    local username = params.name
    local data = {}
    c:foreach('oui', 'user', function(s)
        if s.username == username then
            c:delete('oui', s['.name'])
            c:commit('oui')
        end
    end)
end

--PAGE3-1
function M.Set_New_User_Config(params)
    local param = params
    local c = uci.cursor()
    local id = param.id
    local password = param.password
    local name = param.name
    local phone = param.phone
    local acl = "admin"
    local role = param.role
    local exist = false
    c:foreach('oui', 'user', function(s)
        if s.username == name then
            exist = true
            return false
        end
    end)
    local data_exist = {}
    data_exist["err"] = "already exist"
    if exist then
        return {data_exist}
    end
    local sid = c:add('oui', 'user')
    c:set('oui', sid, 'id', id)
    c:set('oui', sid, 'password', ngx.md5(name .. ':' .. password))
    c:set('oui', sid, 'username', name)
    c:set('oui', sid, 'phone', phone)
    c:set('oui', sid, 'role', role)
    c:set('oui', sid, 'acl', acl or "")
    c:set('oui', sid, 'status', '1')
    local cmd_exe= io.popen('date')
    local cmd_output = remove_n(cmd_exe:read("*all"))
    c:set('oui', sid, 'create_time', cmd_output)
    c:commit('oui')
    -----
    local data = {}
    data["id"] = id
    data["password"] = password
    data["name"] = name
    data["phone"] = phone
    data["role"] = role
    data["status"] = "1"
    data["create_time"] = cmd_output
    local ret = {data}
    return ret
end


--PAGE4

function M.Get_Role_Config()
    local c = uci.cursor()
    local roles = {}

    c:foreach('oui', 'role', function(s)
        roles[#roles + 1] = {
            role_name = s.role_name,
            role_discription = s.role_discription,
            permission_switch = s.permission_switch,
            create_time = s.create_time
        }
    end)
    return roles
end

function M.Set_Role_Status(params)
    local c = uci.cursor()
    local role_name = params.role_name
    local permission_switch = params.permission_switch
    local data = {}
    c:foreach('oui', 'role', function(s)
        if s.role_name == role_name then
            os.execute('uci set oui.'..s['.name']..'.permission_switch='..permission_switch)
        end
    end)

    data["role_name"] = role_name
    data["permission_switch"] = permission_switch
    local ret = {data}
    return ret
end

function M.Set_Edit_Role_Config(params)
    local param = params
    local c = uci.cursor()

    local role_name = param.role_name
    local role_new_name = param.role_new_name

    local role_discription = param.role_discription
    local permission_index = param.permission_index
    local permission_index_tun = param.permission_index_tun
    local permission_tun = param.permission_tun
    local permission_multi_socket = param.permission_multi_socket
    local permission_port_forwarding = param.permission_port_forwarding
    local permission_custom_link = param.permission_custom_link
    local flag = 0
    local sname = ''
    c:foreach('oui', 'role', function(s)
        if s.role_name == role_name then
            sname = s['.name']
            flag = 2
        end
        if s.role_name ~= role_name and s.name == role_new_name then
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
    c:set('oui', sname, 'role_name', role_new_name)
    c:set('oui', sname, 'role_discription', role_discription)
    c:set('oui', sname, 'permission_index', permission_index)
    c:set('oui', sname, 'permission_index_tun', permission_index_tun)
    c:set('oui', sname, 'permission_tun', permission_tun)
    c:set('oui', sname, 'permission_multi_socket', permission_multi_socket)
    c:set('oui', sname, 'permission_port_forwarding', permission_port_forwarding)
    c:set('oui', sname, 'permission_custom_link', permission_custom_link)
    local cmd_exe= io.popen('date')
    local cmd_output = remove_n(cmd_exe:read("*all"))
    c:set('oui', sname, 'create_time', cmd_output)
    c:commit('oui')
    -----
    local data = {}
    data["role_name"] = c:get('oui', sname, 'role_name')
    data["role_discription"] = c:get('oui', sname, 'role_discription')
    data["permission_index"] = c:get('oui', sname, 'permission_index')
    data["permission_index_tun"] = c:get('oui', sname, 'permission_index_tun')
    data["permission_tun"] = c:get('oui', sname, 'permission_tun')
    data["permission_multi_socket"] = c:get('oui', sname, 'permission_multi_socket')
    data["permission_port_forwarding"] = c:get('oui', sname, 'permission_port_forwarding')
    data["permission_custom_link"] = c:get('oui', sname, 'permission_custom_link')
    local ret = {data}
    return ret
end

function M.Set_Delete_Role_Config(params)
    local c = uci.cursor()
    local role_name = params.role_name
    local data = {}
    c:foreach('oui', 'role', function(s)
        if s.role_name == role_name then
            c:delete('oui', s['.name'])
            c:commit('oui')
        end
    end)
end


--PAGE4-1

function M.Set_New_Role(params)
    local param = params
    local c = uci.cursor()

    local role_name = param.role_name
    local role_discription = param.role_discription
    local permission_index = param.permission_index
    local permission_index_tun = param.permission_index_tun
    local permission_tun = param.permission_tun
    local permission_multi_socket = param.permission_multi_socket
    local permission_port_forwarding = param.permission_port_forwarding
    local permission_custom_link = param.permission_custom_link
    local exist = false
    c:foreach('oui', 'role', function(s)
        if s.role_name == role_name then
            exist = true
            return false
        end
    end)
    local data_exist = {}
    data_exist["err"] = "already exist"
    if exist then
        return {data_exist}
    end
    local sid = c:add('oui', 'role')
    c:set('oui', sid, 'role_name', role_name)
    c:set('oui', sid, 'role_discription',role_discription)
    c:set('oui', sid, 'permission_index', permission_index)
    c:set('oui', sid, 'permission_index_tun', permission_index_tun)
    c:set('oui', sid, 'permission_tun', permission_tun)
    c:set('oui', sid, 'permission_multi_socket', permission_multi_socket)
    c:set('oui', sid, 'permission_port_forwarding', permission_port_forwarding)
    c:set('oui', sid, 'permission_custom_link', permission_custom_link)

    c:set('oui', sid, 'permission_switch', "1")
    local cmd_exe= io.popen('date')
    local cmd_output = remove_n(cmd_exe:read("*all"))
    c:set('oui', sid, 'create_time', cmd_output)
    c:commit('oui')

    -----
    local data = {}
    data["role_name"] = c:get('oui', sid, 'role_name')
    data["role_discription"] = c:get('oui', sid, 'role_discription')
    data["permission_index"] = c:get('oui', sid, 'permission_index')
    data["permission_index_tun"] = c:get('oui', sid, 'permission_index_tun')
    data["permission_tun"] = c:get('oui', sid, 'permission_tun')
    data["permission_multi_socket"] = c:get('oui', sid, 'permission_multi_socket')
    data["permission_port_forwarding"] = c:get('oui', sid, 'permission_port_forwarding')
    data["permission_custom_link"] = c:get('oui', sid, 'permission_custom_link')
    data["permission_switch"] = c:get('oui', sid, 'permission_switch')
    data["create_time"] = cmd_output
    local ret = {data}
    return ret
end

--PAGE5

function M.Get_Debug_Config()
    -----
    local data = {}
    data["debug"] = "1"
    local ret = {data}
    return ret
end

function M.Set_Debug_Config(params)
    local debug = params.debug
    -----
    local data = {}
    data["debug"] = debug
    local ret = {data}
    return ret
end

function M.Get_Transmission_Config()
    -----
    local data = {}
    data["transmission"] = "1"
    local ret = {data}
    return ret
end

function M.Set_Transmission_Config(params)
    local transmission = params.transmission
    -----
    local data = {}
    data["transmission"] = transmission
    local ret = {data}
    return ret
end

function M.Reboot()
    os.execute('reboot')
end

return M
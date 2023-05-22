local M = {}

--PAGE1

function M.Get_Language_Config()
    -----
    local data = {}
    data["language"] = "Chinese"
    data["theme"] = "light"
    local ret = {data}
    return ret
end

function M.Set_Language_Config(params)
    local language = params.language
    local theme = params.theme
    -----
    local data = {}
    data["language"] = language
    data["theme"] = theme
    local ret = {data}
    return ret
end

function M.Get_Hostname_Config()
    -----
    local data = {}
    data["hostname"] = "openwrt"
    local ret = {data}
    return ret
end

function M.Set_Hostname_Config(params)
    local hostname = params.hostname
    -----
    local data = {}
    data["hostname"] = hostname
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

function M.Get_TZ_Config()
    -----
    local data = {}
    data["tz"] = "UTC+8:00"
    local ret = {data}
    return ret
end

function M.Set_TZ_Config(params)
    local tz = params.tz
    -----
    local data = {}
    data["tz"] = tz
    local ret = {data}
    return ret
end

function M.Get_Systime()
    -----
    local data = {}
    data["time"] = "2023/05/20 22:35:35"
    local ret = {data}
    return ret
end

function M.Set_Systime(params)
    local time = params.time
    -----
    local data = {}
    data["time"] = time
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

function M.Sysupgrade_Factary(params)
    local path = params.path
    -----
    local data = {}
    data["path"] = path
    local ret = {data}
    return ret
end

--PAGE3

function M.Get_User_Config()
    -----
    local lable_data1 = {}
    local lable_data2 = {}
    lable_data1["id"] = "121"
    lable_data1["name"] = "A"
    lable_data1["phone"] = "11111111111"
    lable_data1["role"] = "admin"
    lable_data1["status"] = "1"
    lable_data1["create_time"] = "2023/05/02 11:12:25"
    lable_data2["id"] = "1211"
    lable_data2["name"] = "A1"
    lable_data2["phone"] = "11221111111"
    lable_data2["role"] = "user"
    lable_data2["status"] = "0"
    lable_data2["create_time"] = "2023/05/02 11:12:24"
    local ret = {lable_data1,lable_data2}
    return ret
end

--PAGE4

function M.Get_Role_Config()
    -----
    local lable_data1 = {}
    local lable_data2 = {}
    lable_data1["role_name"] = "121"
    lable_data1["role_description"] = "A"
    lable_data1["permission_switch"] = "1"
    lable_data1["create_time"] = "2023/05/02 11:12:24"
    lable_data2["role_name"] = "121"
    lable_data2["role_description"] = "A"
    lable_data2["permission_switch"] = "1"
    lable_data2["create_time"] = "2023/05/02 11:12:24"
    local ret = {lable_data1,lable_data2}
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
    -----
end

return M
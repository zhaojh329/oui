local M = {}

--PAGE1

function M.Get_Port_Forwarding_Info()
    -----
    local table_data1 = {}
    local table_data2 = {}
    table_data1["name"] = "转发1"
    table_data1["rule"] = "IPV4 from WAN to THIS_DEVICE"
    table_data1["function"] = "ROUTE to 192.168.1.1"
    table_data1["is_in_use"] = "1"
    table_data2["name"] = "转发2"
    table_data2["rule"] = "IPV4 from WAN to THIS_DEVICE"
    table_data2["function"] = "ROUTE to 192.168.2.1"
    table_data2["is_in_use"] = "0"
    local ret = {table_data1,table_data2}
    return ret
end

--PAGE1-1
function M.Set_New_Port_Forwarding_Info(params)
    local param = params
    -----
    local data = {}
    data["name"] = "转发1"
    data["proto"] = "UDP"
    data["origin_zone"] = "A"
    data["out_port"] = "222"
    data["target_zone"] = "B"
    data["internal_ip"] = "192.168.1.1"
    data["internal_port"] = "333"
    data["origin_mac"] = "FF:FF:FF:FF:FF"
    data["origin_ip"] = "0.0.0.0"
    data["origin_port"] = "4444"
    data["out_ip"] = "0.0.0.0"
    data["is_nat_loop"] = "1"
    data["loop_origin_ip"] = "0.0.0.0"
    data["match_assistant"] = "A"
    data["match_mark"] = "A"
    data["match_limit"] = "None"
    data["extra_param"] = "None"
    local ret = {data}
    return ret
end

--PAGE2

function M.Get_Device_Firewall_Info()
    -----
    local table_data1 = {}
    local table_data2 = {}
    table_data1["name"] = "allow"
    table_data1["rule"] = "IPV4 UDP from WAN to THIS_DEVICE port 68"
    table_data1["function"] = "ACCESS"
    table_data1["is_in_use"] = "1"
    table_data2["name"] = "allow"
    table_data2["rule"] = "IPV4 UDP from WAN to THIS_DEVICE port 80"
    table_data2["function"] = "ACCESS"
    table_data2["is_in_use"] = "0"
    local ret = {table_data1,table_data2}
    return ret
end

--PAGE2-1
function M.Set_New_Firewall_Info(params)
    local param = params
    -----
    local data = {}
    data["name"] = "allow"
    data["proto"] = "UDP"
    data["origin_zone"] = "A"
    data["origin_ip"] = "192.168.1.1"
    data["origin_port"] = "1234"
    data["target_zone"] = "B"
    data["target_ip"] = "127.0.0.1"
    data["target_port"] = "1234"
    data["rule"] = "allow"
    data["match_device"] = "None"
    data["limit_ip_type"] = "ipv4 and ipv6"
    data["origin_mac"] = "FF:FF:FF:FF:FF"
    data["match_assistant"] = "None"
    data["match_mark"] = "A"
    data["match_dscp"] = "B"
    data["match_limit"] = "None"
    data["extra_param"] = "None"
    data["week"] = "Every"
    data["day"] = "Every"
    data["start_time"] = "None"
    data["stop_time"] = "None"
    data["start_day"] = "None"
    data["stop_day"] = "None"
    data["is_utc_time"] = "1"
    local ret = {data}
    return ret
end

--PAGE3
--PAGE4

--PAGE5

function M.Get_System_Log()
    -----
    local data = {}
    data["log"] = "logloglog"
    local ret = {data}
    return ret
end

--PAGE6

function M.Get_Kernel_Log()
    -----
    local data = {}
    data["log"] = "klogklogklog"
    local ret = {data}
    return ret
end

--PAGE7
function M.Get_Whitelist_Config()
    -----
    local whitelist_status = {}
    local table_data1 = {}
    local table_data2 = {}
    table_data1["name"] = "1"
    table_data1["mac"] = "FF:FF:FF:FF:FF"
    table_data2["name"] = "2"
    table_data2["mac"] = "FF:FF:FF:FF:FF"
    local ret = {table_data1,table_data2}
    return ret
end

--PAGE7-1
function M.Set_New_Whitelist(params)
    local name = params.name
    local mac = params.mac
    -----
    local data = {}
    data["name"] = "1"
    data["mac"] = "FF:FF:FF:FF:FF"
    local ret = {data}
    return ret
end

return M

    
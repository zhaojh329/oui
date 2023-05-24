local M = {}

--PAGE1

function M.Get_Network_Interface_Info()
    -----
    local table_data1 = {}
    local table_data2 = {}
    table_data1["proto"] = "static"
    table_data1["running_time"] = "1d12h23m46s"
    table_data1["mac"] = "FF:FF:FF:FF:FF"
    table_data1["rx"] = "114B(57数据包)"
    table_data1["tx"] = "514B(257数据包)"
    table_data1["ipv4"] = "0.0.0.0"
    table_data1["ipv6"] = "fe80::da0e:67ec:654b:3ee1%17"
    table_data1["info"] = ""
    table_data2["proto"] = "DHCP"
    table_data2["running_time"] = "1d25h23m46s"
    table_data2["mac"] = "FF:FF:FF:FF:FF"
    table_data2["rx"] = "114B(57数据包)"
    table_data2["tx"] = "514B(257数据包)"
    table_data2["ipv4"] = "3.3.3.3"
    table_data2["ipv6"] = "fe80::da0e:67ec:654b:3ae1%17"
    table_data2["info"] = "main"
    local ret = {table_data1,table_data2}
    return ret
end

--PAGE2

function M.Get_Device_Net_Info()
    -----
    local data = {}
    data["conn_type"] = "DHCP"
    data["ip_addr"] = "192.168.102.130"
    data["mask"] = "255.255.255.0"
    data["default_gateway"] = "192.168.1.1"
    data["dns"] = "192.168.1.1"
    data["ipv6_conn_type"] = "None"
    local ret = {data}
    return ret
end

function M.Get_Device_Net_Config()
    -----
    local data = {}
    data["conn_way"] = "DHCP"
    data["auto_dns_conf"] = "1"
    data["manual_dns_conf"] = "0"
    data["dns1"] = "114.114.114.114"
    data["dns2"] = "8.8.8.8"
    local ret = {data}
    return ret
end

function M.Set_Device_Net_Config(params)
    local conn_way = params.conn_way
    local auto_dns_conf = params.auto_dns_conf
    local manual_dns_conf = params.manual_dns_conf
    local dns1 = params.dns1
    local dns2 = params.dns2
    -----
    local data = {}
    data["conn_way"] = conn_way
    data["auto_dns_conf"] = auto_dns_conf
    data["manual_dns_conf"] = manual_dns_conf
    data["dns1"] = dns1
    data["dns2"] = dns2
    local ret = {data}
    return ret
end

function M.Get_Device_MAC_Info()
    local data = {}
    data["mac"] = "FF:FF:FF:FF:FF"
    local ret = {data}
    return ret
end

function M.Set_Device_MAC_Info(params)
    local mac = params.mac
    -----
    local data = {}
    data["mac"] = mac
    local ret = {data}
    return ret
end

--PAGE3

local function Get_Wifi_Config()
    -----
    local data1 = {}
    local data2 = {}
    local data3 = {}
    data1["is_wifi_ssid_2in1"] = "1"
    data2["is_wifi_24_on"] = "1"
    data2["wifi_24_name"] = "ABCDEF"
    data2["is_wifi_24_hide_ssid"] = "1"
    data2["wifi_24_encryption_method"] = "混合加密"
    data2["wifi_24_passwd"] = "Ab12345"
    data2["wifi_24_channel"] = "auto"
    data2["wifi_24_dbm"] = "穿墙"
    data3["is_wifi_5g_on"] = "1"
    data3["wifi_5g_name"] = "ABCDEF"
    data3["is_wifi_5g_hide_ssid"] = "1"
    data3["wifi_5g_ encryption_method"] = "混合加密"
    data3["wifi_5g_passwd"] = "Ab12345"
    data3["wifi_5g_channel"] = "auto"
    data3["wifi_5g_dbm"] = "穿墙"
    local ret = {data1,data2,data3}
    return ret
end

function M.Get_Device_Wifi_Config()
    local ret = Get_Wifi_Config()
    return ret
end

function M.Set_Device_Wifi_Config(params)
    local is_wifi_ssid_2in1 = params.is_wifi_ssid_2in1
    local is_wifi_24_on = params.is_wifi_24_on
    local wifi_24_name = params.wifi_24_name
    local is_wifi_24_hide_ssid = params.is_wifi_24_hide_ssid
    local wifi_24_encryption_method = params.wifi_24_encryption_method
    local wifi_24_passwd = params.wifi_24_passwd
    local wifi_24_channel = params.wifi_24_channel
    local wifi_24_dbm = params.wifi_24_dbm
    local is_wifi_5g_on = params.is_wifi_5g_on
    local wifi_5g_name = params.wifi_5g_name
    local is_wifi_5g_hide_ssid = params.is_wifi_5g_hide_ssid
    local wifi_5g_encryption_method = params.wifi_5g_encryption_method
    local wifi_5g_passwd = params.wifi_5g_passwd
    local wifi_5g_channel = params.wifi_5g_channel
    local wifi_5g_dbm = params.wifi_5g_dbm
    -----
    ret = Get_Wifi_Config()
    return ret
end

--PAGE4

local function Get_LAN_Config()
    -----
    local data1 = {}
    local data2 = {}
    data1["is_dhcp_on"] = "1"
    data1["dhcp_start_ip"] = "192.168.1.101"
    data1["dhcp_stop_ip"] = "192.168.1.255"
    data2["lan_ip"] = "0.0.0.0"
    local ret = {data1,data2}
    return ret
end

function M.Get_Device_LAN_Config()
    local ret = Get_LAN_Config()
    return ret
end

function M.Set_Device_LAN_Config(params)
    local is_dhcp_on = params.is_dhcp_on
    local dhcp_start_ip = params.dhcp_start_ip
    local dhcp_stop_ip = params.dhcp_stop_ip
    local lan_ip = params.lan_ip
    -----
    ret = Get_LAN_Config()
    return ret
end

--PAGE5

local function Get_Static_Ipv4_Route()
    -----
    local table_data1 = {}
    local table_data2 = {}
    table_data1["conn"] = "A"
    table_data1["target"] = "A"
    table_data1["ipv4_mask"] = "255.255.255.0"
    table_data1["ipv4_gateway"] = "127.0.0.1"
    table_data1["jump"] = "6"
    table_data1["on_link_route"] = "A"
    table_data2["conn"] = "A"
    table_data2["target"] = "A"
    table_data2["ipv4_mask"] = "255.255.255.0"
    table_data2["ipv4_gateway"] = "127.0.0.1"
    table_data2["jump"] = "6"
    table_data2["on_link_route"] = "A"
    local ret = {table_data1,table_data2}
    return ret
end

function M.Get_Device_Static_Ipv4_Route()
    local ret = Get_Static_Ipv4_Route()
    return ret
end

local function Get_Static_Ipv6_Route()
    -----
    local table_data1 = {}
    local table_data2 = {}
    table_data1["conn"] = "A"
    table_data1["target"] = "A"
    table_data1["ipv6_gateway"] = "A"
    table_data1["jump"] = "6"
    table_data1["on_link_route"] = "A"
    table_data2["conn"] = "B"
    table_data2["target"] = "B"
    table_data2["ipv6_gateway"] = "B"
    table_data2["jump"] = "8"
    table_data2["on_link_route"] = "B"
    local ret = {table_data1,table_data2}
    return ret
end

function M.Get_Device_Static_Ipv6_Route()
    local ret = Get_Static_Ipv6_Route()
    return ret
end

--PAGE5-1

function M.Set_New_Static_Ipv4_Route(params)
    local param = params
    -----
    local data= {}
    data["conn"] = "A"
    data["target"] = "A"
    data["ipv4_mask"] = "255.255.255.0"
    data["ipv4_gateway"] = "127.0.0.1"
    data["jump"] = "6"
    data["mtu"] = "1500"
    data["route_type"] = "A"
    data["route_table"] = "A"
    data["origin_addr"] = "255.255.255.0"
    data["is_on_link_route"] = "1"
    local ret = {data}
    return ret
end

--PAGE5-2
function M.Set_New_Static_Ipv6_Route(params)
    local param = params
    -----
    local data= {}
    data["conn"] = "A"
    data["target"] = "A"
    data["ipv4_mask"] = "255.255.255.0"
    data["ipv4_gateway"] = "127.0.0.1"
    data["jump"] = "6"
    data["mtu"] = "1500"
    data["route_type"] = "A"
    data["route_table"] = "A"
    data["origin_addr"] = "255.255.255.0"
    data["is_on_link_route"] = "1"
    local ret = {data}
    return ret
end


--PAGE6

function M.Get_Device_ARP_Info()
    -----
    local table_data1 = {}
    local table_data2 = {}
    table_data1["ipv4"] = "192.168.1.14"
    table_data1["ipv6"] = "A"
    table_data1["conn"] = "lan"
    table_data2["ipv4"] = "192.168.1.15"
    table_data2["ipv6"] = "A"
    table_data2["conn"] = "wan"
    local ret = {table_data1,table_data2}
    return ret
end

function M.Get_Device_Active_Ipv4_Info()
    -----
    local table_data1 = {}
    local table_data2 = {}
    table_data1["net"] = "A"
    table_data1["target"] = "A"
    table_data1["ipv4_gateway"] = "A"
    table_data1["jump"] = "A"
    table_data1["table"] = "A"
    table_data2["net"] = "A"
    table_data2["target"] = "A"
    table_data2["ipv4_gateway"] = "A"
    table_data2["jump"] = "A"
    table_data2["table"] = "A"
    local ret = {table_data1,table_data2}
    return ret
end

function M.Get_Device_Ipv6_Neighbors_Info()
    -----
    local table_data1 = {}
    local table_data2 = {}
    table_data1["ipv6"] = "A"
    table_data1["mac"] = "FF:FF:FF:FF:FF"
    table_data1["conn"] = "A"
    table_data2["ipv6"] = "B"
    table_data2["mac"] = "FF:FF:FF:FF:FF"
    table_data2["conn"] = "B"
    local ret = {table_data1,table_data2}
    return ret
end

function M.Get_Device_Active_Ipv6_Info()
    -----
    local table_data1 = {}
    local table_data2 = {}
    table_data1["network"] = "A"
    table_data1["target"] = "B"
    table_data1["from_addr"] = "A"
    table_data1["jump"] = "B"
    table_data1["table"] = "C"
    table_data2["network"] = "A"
    table_data2["target"] = "B"
    table_data2["from_addr"] = "A"
    table_data2["jump"] = "B"
    table_data2["table"] = "C"
    local ret = {table_data1,table_data2}
    return ret
end

return M
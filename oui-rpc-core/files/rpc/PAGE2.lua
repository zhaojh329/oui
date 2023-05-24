local M = {}

--PAGE1
function M.Get_Proxy_Mode()
    -----
    local data = {}
    data["mode"] = "mode"
    data["is_private_proto"] = "1"
    local ret = {data}
    return ret
end

function M.Set_Proxy_Mode(params)
    local mode = params.mode
    local is_private_proto = params.is_private_proto
    -----
    local data ={}
    data["mode"] = mode
    data["is_private_proto"] = is_private_proto
    local ret = {data}
    return ret
end

function M.Get_Type1_Link_Info()
    -----
    local realtime_link_info = {}
    local link_route_info = {}
    local table_data1 = {}
    local table_data2 = {}
    realtime_link_info["entry"] = "广东广州"
    realtime_link_info["node1"] = "广东广州"
    realtime_link_info["land_ip"] = "8.8.8.8"
    realtime_link_info["land_place"] = "英国伦敦"
    link_route_info["jump_num"] = "3"
    link_route_info["out_country"] = "美国"
    table_data1["country"] = "美国1"
    table_data1["ip"] = "1.1.1.1"
    table_data1["delay"] = "1024ms"
    table_data2["country"] = "美国2"
    table_data2["ip"] = "2.2.2.2"
    table_data2["delay"] = "230ms"
    local ret = {realtime_link_info,link_route_info,table_data1,table_data2}
    return ret
end

function M.Set_Type1_Link_Route(params)
    local jump_num = params.jump_num
    local out_country = params.out_country
    -----
    link_route_info = {}
    link_route_info["jump_num"] = "2"
    link_route_info["out_country"] = "英国"
    local ret = {link_route_info}
    return ret
end

function M.Get_Type1_Link_Advance_Info()
    -----
    local data = {}
    data["fix_land_ip"] = "1"
    data["auto_switch"] = "1"
    data["fix_out_country"] = "随机分配"
    data["switch_cycle"] = "天"
    data["switch_cycle_value"] = "0"
    local ret = {data}
    return ret
end

function M.Set_Type1_Link_Advance_Info(params)
    local fix_land_ip = params.fix_land_ip
    local auto_switch = params.auto_switch
    local fix_out_country = params.fix_out_country
    local switch_cycle = params.switch_cycle
    local switch_cycle_value = params.switch_cycle_value
    -----
    local data = {}
    data["fix_land_ip"] = "1"
    data["auto_switch"] = "1"
    data["fix_out_country"] = "随机分配"
    data["switch_cycle"] = "天"
    data["switch_cycle_value"] = "0"
    local ret = {data}
    return ret
end

function M.Get_Type2_Link_Info()
    -----
    local table_data1 = {}
    local table_data2 = {}
    table_data1["name"] = "美国12"
    table_data1["jump"] = "3"
    table_data1["link_detail"] = "美国1-英国2"
    table_data1["land_ip"] = "127.0.0.1"
    table_data2["name"] = "美国23"
    table_data2["jump"] = "6"
    table_data2["link_detail"] = "美国1-法国2"
    table_data2["land_ip"] = "127.0.0.2"
    local ret = {table_data1,table_data2}
    return ret
end


function M.Get_Type2_Link_Advance_Info()
    -----
    local data = {}
    data["fix_land_ip"] = "1"
    data["fix_link"] = "1"
    data["random_personal_link"] = "0"
    data["switch_cycle"] = "天"
    data["switch_cycle_value"] = "0"
    local ret = {data}
    return ret
end

function M.Set_Type2_Link_Advance_Info(params)
    local fix_land_ip = params.fix_land_ip
    local fix_link = params.fix_link
    local random_personal_link = params.random_personal_link
    local switch_cycle = params.switch_cycle
    local switch_cycle_value = params.switch_cycle_value
    -----
    local data = {}
    data["fix_land_ip"] = "1"
    data["fix_link"] = "1"
    data["random_personal_link"] = "随机分配"
    data["switch_cycle"] = "天"
    data["switch_cycle_value"] = "0"
    local ret = {data}
    return ret
end

--PAGE1-1

function M.Get_New_Type2_Link_Info()
    -----
    local data = {}
    local table_data1 = {}
    local table_data2 = {}
    data["name"] = "美国1"
    data["all_jump"] = "8"
    table_data1["jump"] = "3"
    table_data1["country"] = "沪"
    table_data1["out_ip"] = "192.168.1.1"
    table_data2["jump"] = "3"
    table_data2["country"] = "美"
    table_data2["out_ip"] = "192.168.1.1"
    local ret = {data,table_data1,table_data2}
    return ret
end

function M.Set_New_Type2_Link_Info(params)
    local param = params
    -----
    local data = {}
    local table_data1 = {}
    local table_data2 = {}
    data["name"] = "美国1"
    data["all_jump"] = "8"
    table_data1["jump"] = "3"
    table_data1["country"] = "沪"
    table_data1["out_ip"] = "192.168.1.1"
    table_data2["jump"] = "3"
    table_data2["country"] = "美"
    table_data2["out_ip"] = "192.168.1.1"
    local ret = {data,table_data1,table_data2}
    return ret
end


--PAGE2

function M.Get_Multi_Socks_Info()
    -----
    local table_data1 = {}
    local table_data2 = {}
    table_data1["name"] = "新加坡1"
    table_data1["jump"] = "5"
    table_data1["out_country"] = "英国2"
    table_data1["land_ip"] = "127.0.0.3"
    table_data1["device_port"] = "333"
    table_data2["name"] = "俄罗斯2"
    table_data2["jump"] = "4"
    table_data2["out_country"] = "法国2"
    table_data2["land_ip"] = "127.0.0.4"
    table_data1["device_port"] = "4567"
    local ret = {table_data1,table_data2}
    return ret
end

--PAGE2-1
function M.Get_New_Multi_Socks_Info()
    -----
    local data = {}
    local table_data1 = {}
    local table_data2 = {}
    data["device_port"] = "4444"
    data["link_type"] = "System"
    data["jump"] = "3"
    data["out_country"] = "usa"
    table_data1["in_use"] = "1"
    table_data1["country"] = "usa"
    table_data1["out_ip"] = "1.1.1.1"
    table_data1["delay"] = "10ms"
    table_data2["in_use"] = "1"
    table_data2["country"] = "usa"
    table_data2["out_ip"] = "1.1.1.1"
    table_data2["delay"] = "10ms"
    local ret = {data,table_data1,table_data2}
    return ret
end

function M.Set_New_Multi_Socks_Info(params)
    local param = params
    -----
    local data = {}
    local table_data1 = {}
    local table_data2 = {}
    data["device_port"] = "4444"
    data["link_type"] = "System"
    data["jump"] = "3"
    data["out_country"] = "usa"
    table_data1["in_use"] = "1"
    table_data1["country"] = "usa"
    table_data1["out_ip"] = "1.1.1.1"
    table_data1["delay"] = "10ms"
    table_data2["in_use"] = "0"
    table_data2["country"] = "usa"
    table_data2["out_ip"] = "2.2.2.2"
    table_data2["delay"] = "100ms"
    local ret = {data,table_data1,table_data2}
    return ret
end

--PAGE3


function M.Get_Port_Forwarding_Info()
    -----
    local table_data1 = {}
    local table_data2 = {}
    table_data1["name"] = "test"
    table_data1["country"] = "美国"
    table_data1["jump"] = "3"
    table_data1["out_ip"] = "11.11.11.11"
    table_data1["out_port"] = "1111"
    table_data1["in_ip"] = "22.22.22.22"
    table_data1["in_port"] = "2222"
    table_data2["name"] = "test2"
    table_data2["country"] = "美国2"
    table_data2["jump"] = "5"
    table_data2["out_ip"] = "11.11.11.11"
    table_data2["out_port"] = "4444"
    table_data2["in_ip"] = "22.22.22.22"
    table_data2["in_port"] = "5555"
    local ret = {table_data1,table_data2}
    return ret
end

--PAGE3-1
function M.Get_New_Port_Forwarding_Info()
    -----
    local data = {}
    local table_data1 = {}
    local table_data2 = {}
    data["name"] = "test"
    data["link_type"] = "System"   
    data["jump"] = "3"
    data["out_country"] = "usa"
    data["device_ip"] = "192.168.1.1"
    data["device_port"] = "4444"
    table_data1["in_use"] = "1"
    table_data1["country"] = "usa"
    table_data1["out_ip"] = "1.1.1.1"
    table_data1["delay"] = "10ms"
    table_data1["port"] = "555"
    table_data2["in_use"] = "0"
    table_data2["country"] = "usa"
    table_data2["out_ip"] = "1.1.1.1"
    table_data2["delay"] = "10ms"
    table_data1["port"] = "666"
    local ret = {data,table_data1,table_data2}
    return ret
end

function M.Set_New_Multi_Socks_Info(params)
    local param = params
    -----
    local data = {}
    local table_data1 = {}
    local table_data2 = {}
    data["name"] = "test"
    data["link_type"] = "System"   
    data["jump"] = "3"
    data["out_country"] = "usa"
    data["device_ip"] = "192.168.1.1"
    data["device_port"] = "4444"
    table_data1["in_use"] = "1"
    table_data1["country"] = "usa"
    table_data1["out_ip"] = "1.1.1.1"
    table_data1["delay"] = "10ms"
    table_data1["port"] = "555"
    table_data2["in_use"] = "0"
    table_data2["country"] = "usa"
    table_data2["out_ip"] = "1.1.1.1"
    table_data2["delay"] = "10ms"
    table_data1["port"] = "666"
    local ret = {data,table_data1,table_data2}
    return ret
end


--PAGE4

function M.Get_VPN_Info()
    -----
    local table_data1 = {}
    local table_data2 = {}
    local vpn_setting_switch = {}
    table_data1["name"] = "Clash"
    table_data1["proto"] = "Vmess"
    table_data1["server_ip"] = "127.0.0.1"
    table_data1["user_nmae"] = "A"
    table_data1["status"] = "Connected"
    table_data2["name"] = "WireGuard"
    table_data2["proto"] = "UDP"
    table_data2["server_ip"] = "127.0.0.2"
    table_data2["user_nmae"] = "B"
    table_data2["status"] = "Disonnected"
    vpn_setting_switch["smart_vpn_routing"] = "1"
    vpn_setting_switch["service_vpn_routing"] = "1"
    local ret = {table_data1,table_data2,vpn_setting_switch}
    return ret
end

function M.Set_VPN_Info(params)
    local smart_vpn_routing = params.smart_vpn_routing
    local service_vpn_routing = params.service_vpn_routing   
    -----
    local vpn_setting_switch = {}
    vpn_setting_switch["smart_vpn_routing"] = "1"
    vpn_setting_switch["service_vpn_routing"] = "1"
    local ret = {vpn_setting_switch}
    return ret
end

--PAGE4-1
function M.Set_New_VPN_Info(params)
    local name = params.name
    local proto = params.proto
    local server = params.server
    local usr = params.usr
    local password = params.password
    local data = {}
    data["name"] = "test"
    data["proto"] = "PPTP"
    data["server"] = "0.0.0.0"
    data["usr"] = "123"
    data["password"] = "456"
    ret = {data}
    return ret
end

return M
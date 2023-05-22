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
    local lable_data1 = {}
    local lable_data2 = {}
    realtime_link_info["entry"] = "广东广州"
    realtime_link_info["node1"] = "广东广州"
    realtime_link_info["land_ip"] = "8.8.8.8"
    realtime_link_info["land_place"] = "英国伦敦"
    link_route_info["jump_num"] = "3"
    link_route_info["out_country"] = "美国"
    lable_data1["country"] = "美国1"
    lable_data1["ip"] = "1.1.1.1"
    lable_data1["delay"] = "1024ms"
    lable_data2["country"] = "美国2"
    lable_data2["ip"] = "2.2.2.2"
    lable_data2["delay"] = "230ms"
    local ret = {realtime_link_info,link_route_info,lable_data1,lable_data2}
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
    local lable_data1 = {}
    local lable_data2 = {}
    lable_data1["name"] = "美国12"
    lable_data1["jump"] = "3"
    lable_data1["link_detail"] = "美国1-英国2"
    lable_data1["land_ip"] = "127.0.0.1"
    lable_data2["name"] = "美国23"
    lable_data2["jump"] = "6"
    lable_data2["link_detail"] = "美国1-法国2"
    lable_data2["land_ip"] = "127.0.0.2"
    local ret = {lable_data1,lable_data2}
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

--PAGE2

function M.Get_Multi_Socks_Info()
    -----
    local lable_data1 = {}
    local lable_data2 = {}
    lable_data1["name"] = "新加坡1"
    lable_data1["jump"] = "5"
    lable_data1["out_country"] = "英国2"
    lable_data1["land_ip"] = "127.0.0.3"
    lable_data1["device_port"] = "333"
    lable_data2["name"] = "俄罗斯2"
    lable_data2["jump"] = "4"
    lable_data2["out_country"] = "法国2"
    lable_data2["land_ip"] = "127.0.0.4"
    lable_data1["device_port"] = "4567"
    local ret = {lable_data1,lable_data2}
    return ret
end

--PAGE3


function M.Get_Port_Forwarding_Info()
    -----
    local lable_data1 = {}
    local lable_data2 = {}
    lable_data1["name"] = "test"
    lable_data1["country"] = "美国"
    lable_data1["jump"] = "3"
    lable_data1["out_ip"] = "11.11.11.11"
    lable_data1["out_port"] = "1111"
    lable_data1["in_ip"] = "22.22.22.22"
    lable_data1["in_port"] = "2222"
    lable_data2["name"] = "test2"
    lable_data2["country"] = "美国2"
    lable_data2["jump"] = "5"
    lable_data2["out_ip"] = "11.11.11.11"
    lable_data2["out_port"] = "4444"
    lable_data2["in_ip"] = "22.22.22.22"
    lable_data2["in_port"] = "5555"
    local ret = {lable_data1,lable_data2}
    return ret
end


--PAGE4

function M.Get_VPN_Info()
    -----
    local lable_data1 = {}
    local lable_data2 = {}
    local vpn_setting_switch = {}
    lable_data1["name"] = "Clash"
    lable_data1["proto"] = "Vmess"
    lable_data1["server_ip"] = "127.0.0.1"
    lable_data1["user_nmae"] = "A"
    lable_data1["status"] = "Connected"
    lable_data2["name"] = "WireGuard"
    lable_data2["proto"] = "UDP"
    lable_data2["server_ip"] = "127.0.0.2"
    lable_data2["user_nmae"] = "B"
    lable_data2["status"] = "Disonnected"
    vpn_setting_switch["smart_vpn_routing"] = "1"
    vpn_setting_switch["service_vpn_routing"] = "1"
    local ret = {lable_data1,lable_data2,vpn_setting_switch}
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

return M
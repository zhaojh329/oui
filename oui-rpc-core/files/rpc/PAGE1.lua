local M = {}

--PAGE1
function M.Get_Wire_Connection()
    local entry1 = {}
    entry1["name"] = "eth0"
    entry1["time"] = "1"
    entry1["ip"] = "192.168.101.23"
    entry1["mac"] = "52:F2:A2:55:12"
    local entry2 = {}
    entry2["name"] = "eth1"
    entry2["time"] = "2"
    entry2["ip"] = "192.168.101.25"
    entry2["mac"] = "52:F2:A2:55:B2"
    local data = {entry1, entry2}
    return data
end

function M.Get_24_Connection()
    local entry1 = {}
    entry1["name"] = "eth0"
    entry1["time"] = "1"
    entry1["ip"] = "192.168.101.23"
    entry1["mac"] = "52:F2:A2:55:12"
    local entry2 = {}
    entry2["name"] = "eth1"
    entry2["time"] = "2"
    entry2["ip"] = "192.168.101.25"
    entry2["mac"] = "52:F2:A2:55:B2"
    local data = {entry1, entry2}
    return data
end

function M.Get_5G_Connection()
    local entry1 = {}
    entry1["name"] = "eth0"
    entry1["time"] = "1"
    entry1["ip"] = "192.168.101.23"
    entry1["mac"] = "52:F2:A2:55:12"
    local entry2 = {}
    entry2["name"] = "eth1"
    entry2["time"] = "2"
    entry2["ip"] = "192.168.101.25"
    entry2["mac"] = "52:F2:A2:55:B2"
    local data = {entry1, entry2}
    return data
end

function M.Set_Device_Whitelist(params)
    local mac = params.mac
    local status = params.status
    -----
    local data ={}
    data["mac"] = mac
    data["status"] = status
    local ret = {data}
    return ret
end

function M.Set_Device_Ethcontrol(params)
    local mac = params.mac
    local status = params.status
    -----
    local data ={}
    data["mac"] = mac
    data["status"] = status
    local ret = {data}
    return ret
end                                  

--PAGE2

function M.Get_Device_Info()
    -----
    local data = {}
    data["device_name"] = "device_name"
    data["device_type"] = "device_type"
    data["min_jump"] = "min_jump"
    data["max_jump"] = "max_jump"
    data["sys_ver"] = "sys_ver"
    data["sn"] = "sn"
    data["days_available"] = "days"
    data["running_time"] = "0day"
    local ret = {data}
    return ret
end

function M.Get_Node_Info()
    -----
    local data = {}
    data["out_country_num"] = "5"
    data["mid_country_num"] = "12"
    data["out_node_num"] = "13"
    data["mid_node_num"] = "12"
    local ret = {data}
    return ret
end

function M.Get_Link_Info()
    -----
    local data = {}
    data["entry"] = "广东广州"
    data["node1"] = "广东广州"
    data["node2"] = "广东广州"
    data["node3"] = "广东广州"
    data["land_ip"] = "127.0.0.1"
    data["land_place"] = "英国伦敦"
    local ret = {data}
    return ret
end

function M.Get_Device_Ethinfo()
    -----
    local data = {}
    data["proto"] = "DHCP"
    data["gateway"] = "127.0.0.1"
    data["dns1"] = "114.114.114.114"
    data["dns2"] = "8.8.8.8"
    data["ip_addr"] = "0.0.0.0"
    data["mac_addr"] = "FF:FF:FF:FF:FF"
    data["lan"] = "ETH1-ETH4"
    data["wan"] = "ETH0"
    data["mask"] = "255.255.255.0"
    local ret = {data}
    return ret
end

--实时网络状态
--终端流量统计

--PAGE2-1
function M.Get_Out_Country_Detail()
end

--PAGE3

function M.Get_Device_WWWinfo()
    -----
    local data = {}
    data["conn_type"] = "WWW"
    data["ip_addr"] = "45.15.122.000"
    data["dns"] = "0.0.0.0"
    data["gateway"] = "2.2.2.2"
    local ret = {data}
    return ret
end


function M.Get_WWW_Speed()
    -----
    local data = {}
    data["upload_speed"] = "114Mbps"
    data["download_speed"] = "514Mbps"
    local ret = {data}
    return ret
end


return M
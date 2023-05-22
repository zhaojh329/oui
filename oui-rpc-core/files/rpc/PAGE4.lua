local M = {}

--PAGE1

function M.Get_Port_Forwarding_Info()
    -----
    local lable_data1 = {}
    local lable_data2 = {}
    lable_data1["name"] = "转发1"
    lable_data1["rule"] = "IPV4 from WAN to THIS_DEVICE"
    lable_data1["function"] = "ROUTE to 192.168.1.1"
    lable_data1["is_in_use"] = "1"
    lable_data2["name"] = "转发2"
    lable_data2["rule"] = "IPV4 from WAN to THIS_DEVICE"
    lable_data2["function"] = "ROUTE to 192.168.2.1"
    lable_data2["is_in_use"] = "0"
    local ret = {lable_data1,lable_data2}
    return ret
end

--PAGE2

function M.Get_Device_Firewall_Info()
    -----
    local lable_data1 = {}
    local lable_data2 = {}
    lable_data1["name"] = "allow"
    lable_data1["rule"] = "IPV4 UDP from WAN to THIS_DEVICE port 68"
    lable_data1["function"] = "ACCESS"
    lable_data1["is_in_use"] = "1"
    lable_data2["name"] = "allow"
    lable_data2["rule"] = "IPV4 UDP from WAN to THIS_DEVICE port 80"
    lable_data2["function"] = "ACCESS"
    lable_data2["is_in_use"] = "0"
    local ret = {lable_data1,lable_data2}
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
    local lable_data1 = {}
    local lable_data2 = {}
    lable_data1["name"] = "1"
    lable_data1["mac"] = "FF:FF:FF:FF:FF"
    lable_data2["name"] = "2"
    lable_data2["mac"] = "FF:FF:FF:FF:FF"
    local ret = {lable_data1,lable_data2}
    return ret
end

return M

    
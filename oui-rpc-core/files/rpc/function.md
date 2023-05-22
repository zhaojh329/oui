第一页
{"method":"call","param":["PAGE1","",{}]}
--------------------------------------------
function Get_Wire_Connection()
params:None
return:
{
    "result": [
        {
            "mac": "52:F2:A2:55:12",
            "name": "eth0",
            "time": "1",
            "ip": "192.168.101.23"
        },
        {
            "mac": "52:F2:A2:55:B2",
            "name": "eth1",
            "time": "2",
            "ip": "192.168.101.25"
        }
    ]
}

function Get_24_Connection()
params:None
return:
{
    "result": [
        {
            "mac": "52:F2:A2:55:12",
            "name": "eth0",
            "time": "1",
            "ip": "192.168.101.23"
        },
        {
            "mac": "52:F2:A2:55:B2",
            "name": "eth1",
            "time": "2",
            "ip": "192.168.101.25"
        }
    ]
}

function Get_5G_Connection()
params:None
return:
{
    "result": [
        {
            "mac": "52:F2:A2:55:12",
            "name": "eth0",
            "time": "1",
            "ip": "192.168.101.23"
        },
        {
            "mac": "52:F2:A2:55:B2",
            "name": "eth1",
            "time": "2",
            "ip": "192.168.101.25"
        }
    ]
}

function Set_Device_Whitelist(params)
params:{"mac":"","status":""}
return:
{
    "result": [
        {
            "mac": "",
            "status": ""
        }
    ]
}

function Set_Device_Ethcontrol(params)
params:{"mac":"","status":""}
return:
{
    "result": [
        {
            "mac": "",
            "status": ""
        }
    ]
}
--------------------------------------------------
function Get_Device_Info()
params:None
return:
{
    "result": [
        {
            "running_time": "0day",
            "device_name": "device_name",
            "sn": "sn",
            "sys_ver": "sys_ver",
            "max_jump": "max_jump",
            "days_available": "days",
            "min_jump": "min_jump",
            "device_type": "device_type"
        }
    ]
}
function Get_Node_Info()
params:None
return:
{
    "result": [
        {
            "out_country_num": "5",
            "mid_node_num": "12",
            "mid_country_num": "12",
            "out_node_num": "13"
        }
    ]
}

function Get_Link_Info()
params:None
return:
{
    "result": [
        {
            "node1": "广东广州",
            "node2": "广东广州",
            "land_place": "英国伦敦",
            "entry": "广东广州",
            "land_ip": "127.0.0.1",
            "node3": "广东广州"
        }
    ]
}

function Get_Device_Ethinfo()
params:None
return:
{
    "result": [
        {
            "proto": "DHCP",
            "lan": "ETH1-ETH4",
            "dns1": "114.114.114.114",
            "mask": "255.255.255.0",
            "mac_addr": "FF:FF:FF:FF:FF",
            "gateway": "127.0.0.1",
            "wan": "ETH0",
            "ip_addr": "0.0.0.0",
            "dns2": "8.8.8.8"
        }
    ]
}
--------------------------------------------------------
function Get_Device_WWWinfo()
params:None
return:
{
    "result": [
        {
            "gateway": "2.2.2.2",
            "dns": "0.0.0.0",
            "ip_addr": "45.15.122.000",
            "conn_type": "WWW"
        }
    ]
}

function Get_WWW_Speed()
params:None
return:
{
"result":
[{"upload_speed":"","download_speed":""}]
}


第二页
{"method":"call","param":["PAGE2","",{}]}
------------------------------------------------------
function Get_Proxy_Mode()
params:None
return:
{
    "result": [
        {
            "is_private_proto": "1",
            "mode": "mode"
        }
    ]
}

function Set_Proxy_Mode(params)
params:{"mode":"","is_private_proto":""}
return:
{
    "result": [
        {
            "is_private_proto": "",
            "mode": ""
        }
    ]
}

function Get_Type1_Link_Info()
params:None
return:
{
    "result": [
        {
            "node1": "",
            "entry": "",
            "land_ip": "",
            "land_place": ""
        },
        {
            "jump_num": "",
            "out_country": ""
        },
        {
            "ip": "",
            "delay": "",
            "country": ""
        },
        {
            "ip": "",
            "delay": "",
            "country": ""
        }
    ]
}

function Set_Type1_Link_Route(params)
params:{"jump_num":"","out_country":""}
return:
{
    "result": [
        {
            "jump_num": "2",
            "out_country": "英国"
        }
    ]
}

function Get_Type1_Link_Advance_Info()
params:None
return:
{
    "result": [
        {
            "fix_out_country": "随机分配",
            "fix_land_ip": "1",
            "switch_cycle_value": "0",
            "switch_cycle": "天",
            "auto_switch": "1"
        }
    ]
}

function Set_Type1_Link_Advance_Info(params)
params:{"fix_land_ip":"","auto_switch":"","fix_out_country":"","switch_cycle":"","switch_cycle_value":""}
return:
{
    "result": [
        {
            "fix_out_country": "随机分配",
            "fix_land_ip": "1",
            "switch_cycle_value": "0",
            "switch_cycle": "天",
            "auto_switch": "1"
        }
    ]
}

function Get_Type2_Link_Info()
params:None
return:
{
    "result": [
        {
            "land_ip": "127.0.0.1",
            "link_detail": "美国1-英国2",
            "name": "美国12",
            "jump": "3"
        },
        {
            "land_ip": "127.0.0.2",
            "link_detail": "美国1-法国2",
            "name": "美国23",
            "jump": "6"
        }
    ]
}

function Get_Type2_Link_Advance_Info()
params:None
return:
{
    "result": [
        {
            "fix_land_ip": "1",
            "switch_cycle_value": "0",
            "switch_cycle": "天",
            "random_personal_link": "0",
            "fix_link": "1"
        }
    ]
}

function Set_Type1_Link_Advance_Info(params)
params:{"fix_land_ip":"","fix_link":"","random_personal_link":"","switch_cycle":"","switch_cycle_value":""}
return:
{
    "result": [
        {
            "fix_out_country": "随机分配",
            "fix_land_ip": "1",
            "switch_cycle_value": "0",
            "switch_cycle": "天",
            "auto_switch": "1"
        }
    ]
}
-----------------------------------------------
function Get_Type2_Link_Advance_Info()
params:None
return:
{
    "result": [
        {
            "name": "新加坡1",
            "jump": "5",
            "device_port": "4567",
            "out_country": "英国2",
            "land_ip": "127.0.0.3"
        },
        {
            "land_ip": "127.0.0.4",
            "out_country": "法国2",
            "name": "俄罗斯2",
            "jump": "4"
        }
    ]
}
-----------------------------------------------
function Get_Port_Forwarding_Info()
params:None
return:
{
    "result": [
        {
            "in_port": "2222",
            "name": "test",
            "country": "美国",
            "jump": "3",
            "out_port": "1111",
            "in_ip": "22.22.22.22",
            "out_ip": "11.11.11.11"
        },
        {
            "in_port": "5555",
            "name": "test2",
            "country": "美国2",
            "jump": "5",
            "out_port": "4444",
            "in_ip": "22.22.22.22",
            "out_ip": "11.11.11.11"
        }
    ]
}
---------------------------------------------
function Get_VPN_Info()
params:None
return:
{
    "result": [
        {
            "proto": "Vmess",
            "name": "Clash",
            "user_nmae": "A",
            "status": "Connected",
            "server_ip": "127.0.0.1"
        },
        {
            "proto": "UDP",
            "name": "WireGuard",
            "user_nmae": "B",
            "status": "Disonnected",
            "server_ip": "127.0.0.2"
        },
        {
            "service_vpn_routing": "1",
            "smart_vpn_routing": "1"
        }
    ]
}
-----------------------------------------------
function Set_VPN_Info(params)
params:{"smart_vpn_routing":"","service_vpn_routing":""}
return:
{
    "result": [
        {
            "service_vpn_routing": "1",
            "smart_vpn_routing": "1"
        }
    ]
}

第3页
{"method":"call","param":["PAGE3","",{}]}
-----------------------------------
function Get_Network_Interface_Info()
params:None
return:
{
    "result": [
        {
            "mac": "FF:FF:FF:FF:FF",
            "proto": "static",
            "rx": "114B(57数据包)",
            "info": "",
            "ipv6": "fe80::da0e:67ec:654b:3ee1%17",
            "running_time": "1d12h23m46s",
            "tx": "514B(257数据包)",
            "ipv4": "0.0.0.0"
        },
        {
            "mac": "FF:FF:FF:FF:FF",
            "proto": "DHCP",
            "rx": "114B(57数据包)",
            "info": "main",
            "ipv6": "fe80::da0e:67ec:654b:3ae1%17",
            "running_time": "1d25h23m46s",
            "tx": "514B(257数据包)",
            "ipv4": "3.3.3.3"
        }
    ]
}
--------------------------------------
function Get_Device_Net_Info()
params:None
return:
{
    "result": [
        {
            "default_gateway": "192.168.1.1",
            "conn_type": "DHCP",
            "ipv6_conn_type": "None",
            "mask": "255.255.255.0",
            "dns": "192.168.1.1",
            "ip_addr": "192.168.102.130"
        }
    ]
}

function Get_Device_Net_Config()
params:None
return:
{
    "result": [
        {
            "auto_dns_conf": "1",
            "dns2": "8.8.8.8",
            "manual_dns_conf": "0",
            "conn_way": "DHCP",
            "dns1": "114.114.114.114"
        }
    ]
}

function Set_Device_Net_Config(params)
params:{"conn_way":"","auto_dns_conf":"","manual_dns_conf":"","dns1":"","dns2":""}
return:
{
    "result": [
        {
            "auto_dns_conf": "",
            "dns2": "",
            "manual_dns_conf": "",
            "conn_way": "",
            "dns1": ""
        }
    ]
}

function Get_Device_MAC_Info()
params:None
return:
{
    "result": [
        {
            "mac": "FF:FF:FF:FF:FF"
        }
    ]
}

function Set_Device_MAC_Info(params)
params:{"mac":""}
return:
{
    "result": [
        {
            "mac": ""
        }
    ]
}

function Get_Device_Wifi_Config()
params:None
return:
{
    "result": [
        {
            "is_wifi_ssid_2in1": "1"
        },
        {
            "is_wifi_24_hide_ssid": "1",
            "wifi_24_dbm": "穿墙",
            "wifi_24_channel": "auto",
            "wifi_24_passwd": "Ab12345",
            "wifi_24_encryption_method": "混合加密",
            "wifi_24_name": "ABCDEF",
            "is_wifi_24_on": "1"
        },
        {
            "wifi_5g_name": "ABCDEF",
            "wifi_5g_dbm": "穿墙",
            "is_wifi_5g_on": "1",
            "wifi_5g_channel": "auto",
            "wifi_5g_ encryption_method": "混合加密",
            "wifi_5g_passwd": "Ab12345",
            "is_wifi_5g_hide_ssid": "1"
        }
    ]
}

function Set_Device_Wifi_Config(params)
params:
{"is_wifi_ssid_2in1": "","is_wifi_24_hide_ssid": "","wifi_24_dbm": "","wifi_24_channel": "","wifi_24_passwd": "","wifi_24_encryption_method": "","wifi_24_name": "","is_wifi_24_on": "","wifi_5g_name": "","wifi_5g_dbm": "","is_wifi_5g_on": "","wifi_5g_channel": "","wifi_5g_ encryption_method": "","wifi_5g_passwd": "","is_wifi_5g_hide_ssid": ""    
}
return:
{
    "result": [
        {
            "is_wifi_ssid_2in1": "1"
        },
        {
            "is_wifi_24_hide_ssid": "1",
            "wifi_24_dbm": "穿墙",
            "wifi_24_channel": "auto",
            "wifi_24_passwd": "Ab12345",
            "wifi_24_encryption_method": "混合加密",
            "wifi_24_name": "ABCDEF",
            "is_wifi_24_on": "1"
        },
        {
            "wifi_5g_name": "ABCDEF",
            "wifi_5g_dbm": "穿墙",
            "is_wifi_5g_on": "1",
            "wifi_5g_channel": "auto",
            "wifi_5g_ encryption_method": "混合加密",
            "wifi_5g_passwd": "Ab12345",
            "is_wifi_5g_hide_ssid": "1"
        }
    ]
}

--------------------------------------------
function Get_LAN_Config()
params:None
return:
{
    "result": [
        {
            "is_dhcp_on": "1",
            "dhcp_stop_ip": "192.168.1.255",
            "dhcp_start_ip": "192.168.1.101"
        },
        {
            "lan_ip": "0.0.0.0"
        }
    ]
}

function Set_Device_LAN_Config(params)
params:{"is_dhcp_on":"","dhcp_start_ip":"","dhcp_stop_ip":"","lan_ip":""}
return:
{
    "result": [
        {
            "is_dhcp_on": "1",
            "dhcp_stop_ip": "192.168.1.255",
            "dhcp_start_ip": "192.168.1.101"
        },
        {
            "lan_ip": "0.0.0.0"
        }
    ]
}
-----------------------------------------------
function Get_Device_Static_Ipv4_Route()
params:None
return:
{
    "result": [
        {
            "conn": "A",
            "ipv4_mask": "255.255.255.0",
            "jump": "6",
            "target": "A",
            "ipv4_gateway": "127.0.0.1",
            "on_link_route": "A"
        },
        {
            "conn": "A",
            "ipv4_mask": "255.255.255.0",
            "jump": "6",
            "target": "A",
            "ipv4_gateway": "127.0.0.1",
            "on_link_route": "A"
        }
    ]
}

function Get_Device_Static_Ipv6_Route()
params:None
return:
{
    "result": [
        {
            "conn": "A",
            "jump": "6",
            "target": "A",
            "on_link_route": "A",
            "ipv6_gateway": "A"
        },
        {
            "conn": "B",
            "jump": "8",
            "target": "B",
            "on_link_route": "B",
            "ipv6_gateway": "B"
        }
    ]
}
----------------------------------------------------
function Get_Device_ARP_Info()
params:None
return:
{
    "result": [
        {
            "conn": "lan",
            "ipv6": "A",
            "ipv4": "192.168.1.14"
        },
        {
            "conn": "wan",
            "ipv6": "A",
            "ipv4": "192.168.1.15"
        }
    ]
}

function Get_Device_Active_Ipv4_Info()
params:None
return:
{
    "result": [
        {
            "jump": "A",
            "net": "A",
            "table": "A",
            "ipv4_gateway": "A",
            "target": "A"
        },
        {
            "jump": "A",
            "net": "A",
            "table": "A",
            "ipv4_gateway": "A",
            "target": "A"
        }
    ]
}

function Get_Device_Ipv6_Neighbors_Info()
params:None
return:
{
    "result": [
        {
            "mac": "FF:FF:FF:FF:FF",
            "ipv6": "A",
            "conn": "A"
        },
        {
            "mac": "FF:FF:FF:FF:FF",
            "ipv6": "B",
            "conn": "B"
        }
    ]
}

function Get_Device_Active_Ipv6_Info()
params:None
return:
{
    "result": [
        {
            "network": "A",
            "jump": "B",
            "from_addr": "A",
            "table": "C",
            "target": "B"
        },
        {
            "network": "A",
            "jump": "B",
            "from_addr": "A",
            "table": "C",
            "target": "B"
        }
    ]
}



第4页
{"method":"call","param":["PAGE4","",{}]}
-----------------------------------
function Get_Port_Forwarding_Info()
params:None
return:
{
    "result": [
        {
            "function": "ROUTE to 192.168.1.1",
            "is_in_use": "1",
            "rule": "IPV4 from WAN to THIS_DEVICE",
            "name": "转发1"
        },
        {
            "function": "ROUTE to 192.168.2.1",
            "is_in_use": "0",
            "rule": "IPV4 from WAN to THIS_DEVICE",
            "name": "转发2"
        }
    ]
}

-------------------------------------------
function Get_Device_Firewall_Info()
params:None
return:
{
    "result": [
        {
            "function": "ACCESS",
            "is_in_use": "1",
            "rule": "IPV4 UDP from WAN to THIS_DEVICE port 68",
            "name": "allow"
        },
        {
            "function": "ACCESS",
            "is_in_use": "0",
            "rule": "IPV4 UDP from WAN to THIS_DEVICE port 80",
            "name": "allow"
        }
    ]
}

---------------------------------------
---------------------------------------

---------------------------------------
function Get_Kernel_Log()
params:None
return:
{
    "result": [
        {
            "log": "klogklogklog"
        }
    ]
}


function Get_Whitelist_Config()
params:None
return:
{
    "result": [
        {
            "name": "1",
            "mac": "FF:FF:FF:FF:FF"
        },
        {
            "name": "2",
            "mac": "FF:FF:FF:FF:FF"
        }
    ]
}


第5页
{"method":"call","param":["PAGE5","",{}]}
-----------------------------------

function Get_Language_Config()
params:None
return:
{
    "result": [
        {
            "language": "Chinese",
            "theme": "light"
        }
    ]
}

function Set_Language_Config(params)
params:{"language":"","theme":""}
return:
{
    "result": [
        {
            "language": "",
            "theme": ""
        }
    ]
}

function Get_Hostname_Config()
params:None
return:
{
    "result": [
        {
            "hostname": "openwrt"
        }
    ]
}

function Set_Hostname_Config(params)
params:{"hostname":""}
return:
{
    "result": [
        {
            "hostname": ""
        }
    ]
}

function Get_Device_Log_Config()
params:None
return:
{
    "result": [
        {
            "extern_log_server_ip_addr": "0.0.0.0",
            "log_buffer_size": "64",
            "extern_log_server_proto": "UDP",
            "corn_log_level": "Warning",
            "log_level": "Debug",
            "log_file_addr": "/tmp",
            "extern_log_server_port": "114"
        }
    ]
}

function Set_Device_Log_Config(params)
params:{"log_buffer_size":"","extern_log_server_ip_addr":"","extern_log_server_port":"","extern_log_server_proto":"","log_file_addr":"","log_level":"","corn_log_level":""}
return:
{
    "result": [
        {
            "extern_log_server_ip_addr": "0.0.0.0",
            "log_buffer_size": "64",
            "extern_log_server_proto": "UDP",
            "corn_log_level": "Warning",
            "log_level": "Debug",
            "log_file_addr": "/tmp",
            "extern_log_server_port": "114"
        }
    ]
}

function Get_TZ_Config()
params:None
return:
{
    "result": [
        {
            "tz": "UTC+8:00"
        }
    ]
}

function Set_TZ_Config(params)
params:{"tz":""}
return:
{
    "result": [
        {
            "tz": ""
        }
    ]
}

function Get_Systime()
params:None
return:
{
    "result": [
        {
            "time": "2023/05/20 22:35:35"
        }
    ]
}

function Set_Systime(params)
params:{"time":""}
return:
{
    "result": [
        {
            "time": ""
        }
    ]
}

------------------------------------
function Sysupgrade_Auto()
params:None
return:
{
    "result": [
        {
            "return": "1"
        }
    ]
}

function Sysupgrade_Manual()
params:None
return:
{
    "result": [
        {
            "return": "1"
        }
    ]
}

function Sysupgrade_Flash(params)
params:{"path":""}
return:
{
    "result": [
        {
            "path": ""
        }
    ]
}

function Sysupgrade_Factary()
params:None
return:
{
    "result": [
        {
            "return": "1"
        }
    ]
}

function Sysupgrade_Backup(params)
params:{"path":""}
return:
{
    "result": [
        {
            "path": ""
        }
    ]
}

function Sysupgrade_Factary(params)
params:{"path":""}
return:
{
    "result": [
        {
            "path": ""
        }
    ]
}

---------------------------------------
function Get_User_Config()
params:None
return:
{
    "result": [
        {
            "role": "admin",
            "name": "A",
            "phone": "11111111111",
            "create_time": "2023/05/02 11:12:25",
            "status": "1",
            "id": "121"
        },
        {
            "role": "user",
            "name": "A1",
            "phone": "11221111111",
            "create_time": "2023/05/02 11:12:24",
            "status": "0",
            "id": "1211"
        }
    ]
}

---------------------------------------
function Get_Role_Config()
params:None
return:
{
    "result": [
        {
            "create_time": "2023/05/02 11:12:24",
            "role_description": "A",
            "role_name": "121",
            "permission_switch": "1"
        },
        {
            "create_time": "2023/05/02 11:12:24",
            "role_description": "A",
            "role_name": "121",
            "permission_switch": "1"
        }
    ]
}

-------------------------------------------

function Get_Debug_Config()
params:None
return:
{
    "result": [
        {
            "debug": "1"
        }
    ]
}

function Set_Debug_Config(params)
params:{"path":""}
return:
{
    "result": [
        {
            "debug": ""
        }
    ]
}

function Get_Transmission_Config()
params:None
return:
{
    "result": [
        {
            "transmission": "1"
        }
    ]
}

function Set_Transmission_Config(params)
params:{"transmission":""}
return:
{
    "result": [
        {
            "transmission": "1"
        }
    ]
}

function Reboot()
params:None
return:None
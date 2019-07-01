# 介绍

[vue.js]: https://github.com/vuejs/vue
[element-ui]: https://github.com/ElemeFE/element
[LuCI2]: https://git.openwrt.org/?p=project/luci2/ui.git
[json-rpc]: https://www.jsonrpc.org/
[ubus]: https://wiki.openwrt.org/doc/techref/ubus
[uhttpd-mod-ubus]: https://wiki.openwrt.org/doc/techref/ubus#access_to_ubus_over_http

oui使用[vue.js]和[element-ui]实现，灵感来自于[LuCI2]。

oui使用[json-rpc]和OpenWrt子系统通信。通过[json-rpc]调用[ubus]。通过[ubus]存取各种系统数据(通过[uhttpd-mod-ubus]提供基于HTTP的接口API)。

## 它是如何工作的？

``` bash
curl -d '{ "jsonrpc": "2.0", "id": 1, "method": "call", "params": [ "00000000000000000000000000000000", "session", "login", { "username": "root", "password": "secret"  } ] }'  http://your.server.ip/ubus
```

## 特性

* 快
* 多用户
* 前后端分离
* Vue驱动

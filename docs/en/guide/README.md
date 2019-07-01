# Introduction

[vue.js]: https://github.com/vuejs/vue
[element-ui]: https://github.com/ElemeFE/element
[LuCI2]: https://git.openwrt.org/?p=project/luci2/ui.git
[json-rpc]: https://www.jsonrpc.org/
[ubus]: https://wiki.openwrt.org/doc/techref/ubus
[uhttpd-mod-ubus]: https://wiki.openwrt.org/doc/techref/ubus#access_to_ubus_over_http

oui implemented in [vue.js] and [element-ui], inspired by [LuCI2].

oui uses [json-rpc] to communicate with OpenWrt subsystems. Call [ubus] via [json-rpc].
To access any kind of system data through [ubus] with [json-rpc](with the help of [uhttpd-mod-ubus] to provide HTTP based API).

## How It Works

``` bash
curl -d '{ "jsonrpc": "2.0", "id": 1, "method": "call", "params": [ "00000000000000000000000000000000", "session", "login", { "username": "root", "password": "secret"  } ] }'  http://your.server.ip/ubus
```

## Features

* Fast
* Multi User
* Front and rear separation
* Vue-Powered

# Introduction

[vue.js]: https://github.com/vuejs/vue
[Ant Design of Vue]: https://github.com/vueComponent/ant-design-vue
[LuCI2]: https://git.openwrt.org/?p=project/luci2/ui.git
[json-rpc]: https://www.jsonrpc.org/
[ubus]: https://wiki.openwrt.org/doc/techref/ubus
[uhttpd-mod-ubus]: https://wiki.openwrt.org/doc/techref/ubus#access_to_ubus_over_http

oui implemented in [vue.js] and [Ant Design of Vue], inspired by [LuCI2].

oui uses [json-rpc] to communicate with OpenWrt subsystems. Call [ubus] via [json-rpc].
To access any kind of system data through [ubus] with [json-rpc](with the help of [uhttpd-mod-ubus] to provide HTTP based API).

![](/oui/architecture.png)

## Features

* Fast: Use static pages; access any kind of system data via ubus
* Multi User: Support for multi-user and access management
* Vue: Enjoy the dev experience of Vue
* flexible: Flexible, easy to customize UI

# 介绍

[vue.js]: https://github.com/vuejs/vue
[Ant Design of Vue]: https://github.com/vueComponent/ant-design-vue
[LuCI2]: https://git.openwrt.org/?p=project/luci2/ui.git
[json-rpc]: https://www.jsonrpc.org/
[ubus]: https://wiki.openwrt.org/doc/techref/ubus
[uhttpd-mod-ubus]: https://wiki.openwrt.org/doc/techref/ubus#access_to_ubus_over_http

oui使用[vue.js]和[Ant Design of Vue]实现，灵感来自于[LuCI2]。

oui使用[json-rpc]和OpenWrt子系统通信。通过[json-rpc]调用[ubus]。通过[ubus]存取各种系统数据(通过[uhttpd-mod-ubus]提供基于HTTP的接口API)。

![](/oui/architecture.png)

## 特性

* 快: 使用静态页面；通过ubus存取各种系统数据
* 多用户: 支持多用户及权限管理
* Vue: 享受现代化的前端框架 Vue 所带来的开发体验
* 灵活: 灵活的框架，方便自定义UI

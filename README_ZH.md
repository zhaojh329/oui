# oui

[1]: https://img.shields.io/badge/开源协议-MIT-brightgreen.svg?style=plastic
[2]: /LICENSE
[3]: https://img.shields.io/badge/提交代码-欢迎-brightgreen.svg?style=plastic
[4]: https://github.com/zhaojh329/oui/pulls
[5]: https://img.shields.io/badge/提问-欢迎-brightgreen.svg?style=plastic
[6]: https://github.com/zhaojh329/oui/issues/new
[7]: https://travis-ci.org/zhaojh329/oui.svg?branch=master
[8]: https://travis-ci.org/zhaojh329/oui
[11]: https://img.shields.io/badge/支持oui-赞助作者-blueviolet.svg
[12]: https://gitee.com/zhaojh329/oui#project-donate-overview
[13]: https://img.shields.io/badge/技术交流群-点击加入：153530783-brightgreen.svg
[14]: https://jq.qq.com/?_wv=1027&k=5PKxbTV

[![license][1]][2]
[![PRs Welcome][3]][4]
[![Issue Welcome][5]][6]
[![Build Status][7]][8]
[![Support oui][11]][12]
[![Chinese Chat][13]][14]

[vue.js]: https://github.com/vuejs/vue
[element-ui]: https://github.com/ElemeFE/element
[LuCI2]: https://git.openwrt.org/?p=project/luci2/ui.git
[json-rpc]: https://www.jsonrpc.org/
[ubus]: https://wiki.openwrt.org/doc/techref/ubus
[uhttpd-mod-ubus]: https://wiki.openwrt.org/doc/techref/ubus#access_to_ubus_over_http

![](/screenshots_zh.png)

OpenWrt后台管理界面，使用[vue.js]和[element-ui]实现，灵感来自于[LuCI2]。

oui使用[json-rpc]和OpenWrt子系统通信。通过[json-rpc]调用[ubus]。通过[ubus]存取各种系统数据(通过[uhttpd-mod-ubus]提供基于HTTP的接口API)。

![](/docs/.vuepress/public/architecture.png)

# 用户

<a href="https://www.perfectsignal-tech.com"><img src="/users/perfectsignal.jpg" height="80" align="middle"/></a>&nbsp;&nbsp;

# 贡献代码
如果你想帮助[oui](https://github.com/zhaojh329/oui) 变得更好，请参考
[CONTRIBUTING_ZH.md](/CONTRIBUTING_ZH.md)。


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
[Ant Design of Vue]: https://github.com/vueComponent/ant-design-vue
[LuCI2]: https://git.openwrt.org/?p=project/luci2/ui.git
[json-rpc]: https://www.jsonrpc.org/

![](/demo-zh.gif)

OpenWrt后台管理界面，使用[vue.js]和[Ant Design of Vue]实现，灵感来自于[LuCI2]。

oui使用[json-rpc]和OpenWrt子系统通信。

# 如何编译
## 添加 feeds

	echo "src-git oui https://github.com/zhaojh329/oui.git" >> feeds.conf.default
	/scripts/feeds update oui
	./scripts/feeds install -a -p oui

## 配置

	Oui  --->
		Applications  --->
			<*> oui-app-active-routes...................................... Active Routes
			<*> oui-app-admin............................................. Administration
			<*> oui-app-bwm............................................ Bandwidth Monitor
			<*> oui-app-cron............................................. Scheduled Tasks
			<*> oui-app-dhcp.................................................... DHCP/DNS
			<*> oui-app-diagnostics.......................................... Diagnostics
			<*> oui-app-dmesg................................................. Kernel Log
			<*> oui-app-example..................................... Oui Support for rtty
			<*> oui-app-firewall................................................ Firewall
			<*> oui-app-hosts.................................................. Hostnames
			<*> oui-app-interfaces.................................... Network Interfaces
			<*> oui-app-leds........................................... LED Configuration
			<*> oui-app-processes.............................................. Processes
			<*> oui-app-routes.................................................... Routes
			<*> oui-app-rtty........................................ Oui Support for rtty
			<*> oui-app-ssh.......................................................... SSH
			<*> oui-app-switch.................................................... Switch
			<*> oui-app-syslog.................................................... Syslog
			<*> oui-app-system............................................ System Setting
			<*> oui-app-upgrade......................................... Backup / Upgrade
			<*> oui-app-wireless................................................ Wireless
		-*- oui-bwm........................................ Bandwidth Monitor for oui
		-*- oui-httpd................................................ Oui rpc backend
		-*- oui-ui-core.................................................. Oui ui core

## 编译

	make V=s

# 用户

<a href="https://www.perfectsignal-tech.com"><img src="https://nwzimg.wezhan.cn/contents/sitefiles2032/10164349/images/9482755.jpg" height="80" align="middle"/></a>&nbsp;&nbsp;
<a href="http://m.iyunlink.com"><img src="http://m.iyunlink.com/upload/202007/1595823915.png" height="80" align="middle"/></a>&nbsp;&nbsp;

# 贡献代码
如果你想帮助[oui](https://github.com/zhaojh329/oui) 变得更好，请参考
[CONTRIBUTING_ZH.md](/CONTRIBUTING_ZH.md)。


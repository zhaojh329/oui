# oui([中文](/README_ZH.md))

[1]: https://img.shields.io/badge/license-MIT-brightgreen.svg?style=plastic
[2]: /LICENSE
[3]: https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=plastic
[4]: https://github.com/zhaojh329/oui/pulls
[5]: https://img.shields.io/badge/Issues-welcome-brightgreen.svg?style=plastic
[6]: https://github.com/zhaojh329/oui/issues/new
[7]: https://travis-ci.org/zhaojh329/oui.svg?branch=master
[8]: https://travis-ci.org/zhaojh329/oui
[11]: https://img.shields.io/badge/Support%20oui-Donate-blueviolet.svg
[12]: https://paypal.me/zjh329

[![license][1]][2]
[![PRs Welcome][3]][4]
[![Issue Welcome][5]][6]
[![Build Status][7]][8]
[![Support oui][11]][12]

[vue.js]: https://github.com/vuejs/vue
[Ant Design of Vue]: https://github.com/vueComponent/ant-design-vue
[LuCI2]: https://git.openwrt.org/?p=project/luci2/ui.git
[json-rpc]: https://www.jsonrpc.org/

![](/demo.gif)

![](/diagram.png)

OpenWrt web user interface implemented in [vue.js] and [Ant Design of Vue], inspired by [LuCI2].

Oui uses [json-rpc] to communicate with OpenWrt subsystems.

Oui is especially suitable for enterprise custom development.

# How to build
## Add feeds

	echo "src-git oui https://github.com/zhaojh329/oui.git" >> feeds.conf.default
	./scripts/feeds update oui
	./scripts/feeds install -a -p oui

## Configure

	Oui  --->
		Applications  --->
			<*> oui-app-admin............................................. Administration
			<*> oui-app-diagnostics.......................................... Diagnostics
			<*> oui-app-firewall................................................ Firewall
			<*> oui-app-home.......................................... Built-in home page
			<*> oui-app-interfaces.................................... Network Interfaces
			<*> oui-app-system............................................ System Setting
			<*> oui-app-upgrade......................................... Backup / Upgrade
			<*> oui-app-wireless................................................ Wireless
		-*- oui-bwm........................................ Bandwidth Monitor for oui
		-*- oui-httpd................................................ Oui rpc backend
		-*- oui-ui-core.................................................. Oui ui core
	
## Compile

	make V=s

# Jsonrpc example
## General

	{
		"jsonrpc": "2.0",
		"id": 27,
		"method": "call",
		"params": ["sid", "network", "dhcp_leases", {}]
	}

## Ubus

	{
		"jsonrpc": "2.0",
		"id": 7,
		"method": "call",
		"params": ["sid", "ubus", "call", { "object": "system", "method": "board" }]
	}

# In Production

<a href="https://www.perfectsignal-tech.com"><img src="https://nwzimg.wezhan.cn/contents/sitefiles2032/10164349/images/9482755.jpg" height="80" align="middle"/></a>&nbsp;&nbsp;
<a href="http://m.iyunlink.com/"><img src="http://m.iyunlink.com/upload/202007/1595823915.png" height="80" align="middle"/></a>&nbsp;&nbsp;

# Contributing
If you would like to help making [oui](https://github.com/zhaojh329/oui) better,
see the [CONTRIBUTING.md](/CONTRIBUTING.md) file.

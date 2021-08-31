# oui([中文](/README_ZH.md))

[1]: https://img.shields.io/badge/license-MIT-brightgreen.svg?style=plastic
[2]: /LICENSE
[3]: https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=plastic
[4]: https://github.com/zhaojh329/oui/pulls
[5]: https://img.shields.io/badge/Issues-welcome-brightgreen.svg?style=plastic
[6]: https://github.com/zhaojh329/oui/issues/new
[7]: https://travis-ci.org/zhaojh329/oui.svg?branch=master
[8]: https://travis-ci.org/zhaojh329/oui
[9]: https://img.shields.io/badge/Support%20oui-Donate-blueviolet.svg
[10]: https://paypal.me/zjh329

[![license][1]][2]
[![PRs Welcome][3]][4]
[![Issue Welcome][5]][6]
[![Build Status][7]][8]
![visitors](https://visitor-badge.laobi.icu/badge?page_id=zhaojh329.oui)
[![Support oui][9]][10]

[vue.js]: https://github.com/vuejs/vue
[Ant Design of Vue]: https://github.com/vueComponent/ant-design-vue
[json-rpc]: https://www.jsonrpc.org/

![](/demo.gif)

![](/diagram.png)

OpenWrt web user interface implemented in [vue.js] and [Ant Design of Vue].

Oui uses [json-rpc] to communicate with OpenWrt subsystems and support ACL.

Oui is especially suitable for enterprise custom development.

# How to build
## Add feeds

	sed -i '1i\src-git oui https://github.com/zhaojh329/oui.git' feeds.conf.default
	./scripts/feeds update oui
	./scripts/feeds uninstall -a
	./scripts/feeds install -a

## Configure

	Oui  --->
		Applications  --->
			<*> oui-app-admin............................................. Administration
			<*> oui-app-diagnostics.......................................... Diagnostics
			<*> oui-app-firewall................................................ Firewall
			<*> oui-app-home.......................................... Built-in home page
			<*> oui-app-interfaces.................................... Network Interfaces
			<*> oui-app-login........................................ Built-in login page
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

#  How to modify vue
## oui-ui-core
1. Modify
2. Enter directory 'oui/oui-ui-core/vue' and run the follow commands
```
	npm install
	npm run build
```
## application
1. Modify
2. Enter your application directory(e.g. 'oui-app-example') and run the follow commands
```
	cp vue/app.vue ../../build-app/src/
```
3. Enter directory oui/build-app and run the follow commands
```
	npm install
	npm run build
	cp dist/app.common.js ../applications/oui-app-example/vue/dist/app.js
```
# How to debug vue for application(e.g. oui-app-example)
1. Copy oui-app-example/vue/app.vue to oui-ui-core/vue/src/views/oui-app-example.vue
2. Enter directory 'oui/oui-ui-core/vue' and run the follow commands
```
	npm install
	npm run serve
```

# Contributing
If you would like to help making [oui](https://github.com/zhaojh329/oui) better,
see the [CONTRIBUTING.md](/CONTRIBUTING.md) file.

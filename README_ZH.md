# oui

[1]: https://img.shields.io/badge/开源协议-MIT-brightgreen.svg?style=plastic
[2]: /LICENSE
[3]: https://img.shields.io/badge/提交代码-欢迎-brightgreen.svg?style=plastic
[4]: https://github.com/zhaojh329/oui/pulls
[5]: https://img.shields.io/badge/提问-欢迎-brightgreen.svg?style=plastic
[6]: https://github.com/zhaojh329/oui/issues/new
[7]: https://travis-ci.org/zhaojh329/oui.svg?branch=master
[8]: https://travis-ci.org/zhaojh329/oui
[9]: https://img.shields.io/badge/支持oui-赞助作者-blueviolet.svg
[10]: https://gitee.com/zhaojh329/oui#project-donate-overview
[11]: https://img.shields.io/badge/技术交流群-点击加入：153530783-brightgreen.svg
[12]: https://jq.qq.com/?_wv=1027&k=5PKxbTV

[![license][1]][2]
[![PRs Welcome][3]][4]
[![Issue Welcome][5]][6]
[![Build Status][7]][8]
![visitors](https://visitor-badge.laobi.icu/badge?page_id=zhaojh329.oui)
[![Support oui][9]][10]
[![Chinese Chat][11]][12]

[vue.js]: https://github.com/vuejs/vue
[Ant Design of Vue]: https://github.com/vueComponent/ant-design-vue
[json-rpc]: https://www.jsonrpc.org/

![](/demo-zh.gif)

![](/diagram.png)

OpenWrt后台管理界面，使用[vue.js]和[Ant Design of Vue]实现。

Oui使用[json-rpc]和OpenWrt子系统通信, 支持 ACL。

Oui特别适合用于企业定制开发。

# 如何编译
## 添加 feeds

	sed -i '1i\src-git oui https://gitee.com/zhaojh329/oui.git' feeds.conf.default
	./scripts/feeds update oui
	./scripts/feeds uninstall -a
	./scripts/feeds install -a

## 配置

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

## 编译

	make V=s


# Jsonrpc 示例
## 通用

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

# 如何修改 Vue
## oui-ui-core
1. 修改
2. 进入目录 'oui/oui-ui-core/vue' 然后执行如下命令
```
	npm install
	npm run build
```
## Application
1. 修改
2. 进入你的Application目录（例如 oui-app-example）然后执行如下命令
```
	cp vue/app.vue ../../build-app/src/
```
3. 进入目录 oui/build-app 然后执行如下命令
```
	npm install
	npm run build
	cp dist/app.common.js ../applications/oui-app-example/vue/dist/app.js
```
# 如何调试 Application(例如 oui-app-example)
1. 拷贝 oui-app-example/vue/app.vue 到 oui-ui-core/vue/src/views/oui-app-example.vue
2. 进入目录 'oui/oui-ui-core/vue' 然后执行如下命令
```
	npm install
	npm run serve
```

# 贡献代码
如果你想帮助[oui](https://github.com/zhaojh329/oui) 变得更好，请参考
[CONTRIBUTING_ZH.md](/CONTRIBUTING_ZH.md)。


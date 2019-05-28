# oui([中文](/README_ZH.md))

[1]: https://img.shields.io/badge/license-MIT-brightgreen.svg?style=plastic
[2]: /LICENSE
[3]: https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=plastic
[4]: https://github.com/zhaojh329/oui/pulls
[5]: https://img.shields.io/badge/Issues-welcome-brightgreen.svg?style=plastic
[6]: https://github.com/zhaojh329/oui/issues/new
[7]: https://travis-ci.org/zhaojh329/oui.svg?branch=master
[8]: https://travis-ci.org/zhaojh329/oui

[![license][1]][2]
[![PRs Welcome][3]][4]
[![Issue Welcome][5]][6]
[![Build Status][7]][8]

[vue.js]: https://github.com/vuejs/vue
[iview]: https://github.com/iview/iview
[LuCI2]: https://git.openwrt.org/?p=project/luci2/ui.git
[json-rpc]: https://www.jsonrpc.org/
[ubus]: https://wiki.openwrt.org/doc/techref/ubus
[uhttpd-mod-ubus]: https://wiki.openwrt.org/doc/techref/ubus#access_to_ubus_over_http

![](/screen-be6656a.gif)

OpenWrt web user interface implemented in [vue.js] and [iview], inspired by [LuCI2].

oui uses [json-rpc] to communicate with OpenWrt subsystems. Call [ubus] via [json-rpc].
To access any kind of system data through [ubus] with [json-rpc](with the help of [uhttpd-mod-ubus] to provide HTTP based API).

# Requirements

The following package is required to build the oui:

* Node.js >= 8.11

# How to use
Add new feed into "feeds.conf.default":
    
    src-git oui https://github.com/zhaojh329/oui.git

Install oui packages:
    
    ./scripts/feeds update oui
    ./scripts/feeds install -a -p oui

Select package oui in menuconfig and compile new image.

    OUI  --->
        <*>  oui-ui-core..................... Oui ui core

Alternative use existing nodejs installation on the host system(The nodejs is only used to compile code)

[Installing Node.js via package manager](https://nodejs.org/en/download/package-manager/)

    <*> oui-ui-core
        Configuration  --->
            [*] Use oUI node Package feed

# How to develop and debug
First, clone the code to system.

	cd oui/oui-ui-core/src

Then modify the configuration file according to your own environment.
You may need to modify proxy.

	vi vue.config.js

Then execute the following command to start the debug server

	npm run serve

# [Donate](https://gitee.com/zhaojh329/oui#project-donate-overview)

* 深圳市云联芯科技有限公司 - 300¥
* 北京钛铂云老戴 - 200¥
* JunoSky - 100¥
* 许玉善(北京友联智诚科技有限公司) - 100¥
* lemon - 10¥

# Contributing
If you would like to help making [oui](https://github.com/zhaojh329/oui) better,
see the [CONTRIBUTING.md](/CONTRIBUTING.md) file.

# vwrt([中文](/README_ZH.md))

[1]: https://img.shields.io/badge/license-MIT-brightgreen.svg?style=plastic
[2]: /LICENSE
[3]: https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=plastic
[4]: https://github.com/zhaojh329/vwrt/pulls
[5]: https://img.shields.io/badge/Issues-welcome-brightgreen.svg?style=plastic
[6]: https://github.com/zhaojh329/vwrt/issues/new
[7]: https://travis-ci.org/zhaojh329/vwrt.svg?branch=master
[8]: https://travis-ci.org/zhaojh329/vwrt

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

vwrt uses [json-rpc] to communicate with OpenWrt subsystems. Call [ubus] via [json-rpc].
To access any kind of system data through [ubus] with [json-rpc](with the help of [uhttpd-mod-ubus] to provide HTTP based API).

# Requirements

The following package is required to build the vwrt:

* Node.js >= 8.11

# How to use
Add new feed into "feeds.conf.default":
    
    src-git vwrt https://github.com/zhaojh329/vwrt.git

Install vwrt packages:
    
    ./scripts/feeds update vwrt
    ./scripts/feeds install -a -p vwrt

Select package vwrt in menuconfig and compile new image.

    VWRT  --->
        <*>  vwrt-ui-core..................... VWRT UI

Alternative use existing nodejs installation on the host system(The nodejs is only used to compile code)

[Installing Node.js via package manager](https://nodejs.org/en/download/package-manager/)

    <*> vwrt-ui-core
        Configuration  --->
            [*] Use existing nodejs installation on the host system

# How to develop and debug
First, clone the code to system.

	cd vwrt/vwrt-ui-core/src

Then modify the configuration file according to your own environment.
You may need to modify proxy.

	vi vue.config.js

Then execute the following command to start the debug server

	npm run serve

# [Donate](https://gitee.com/zhaojh329/vwrt#project-donate-overview)

# Contributing
If you would like to help making [vwrt](https://github.com/zhaojh329/vwrt) better,
see the [CONTRIBUTING.md](/CONTRIBUTING.md) file.

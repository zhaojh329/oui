# vuci([中文](https://github.com/zhaojh329/vuci/blob/master/README_ZH.md))

![](https://img.shields.io/badge/license-LGPL2-brightgreen.svg?style=plastic "License")

![](/screen-be6656a.gif)

Web Interface for OpenWRT implemented in [vue.js](https://github.com/vuejs/vue) and [Vuetify](https://vuetifyjs.com)

A new web interface with a different architecture. It doesn't use Lua anymore, but use MVVM framework.
To access any kind of system data through [ubus](https://wiki.openwrt.org/doc/techref/ubus)(with the help of
[uhttpd-mod-ubus](https://wiki.openwrt.org/doc/techref/ubus#access_to_ubus_over_http) to provide HTTP based API).

`Keep Watching for More Actions on This Space`

# [Comparison between vuejs and other frameworks](https://vuejs.org/v2/guide/comparison.html)

# How to use
Add new feed into "feeds.conf.default":
    
    src-git vuci https://github.com/zhaojh329/vuci.git

Install vuci packages:
    
    ./scripts/feeds update
    ./scripts/feeds install -a -p vuci

Select package vuci in menuconfig and compile new image.

    VUCI  --->
        <*>  vuci-ui-base. VUCI Web Interface</*>

Compile

    make V=s

# How to develop and debug
First, clone the code to your desktop system.

	cd vuci/vuci-ui-base/src

Then modify the configuration file according to your own environment.
You may need to modify proxy.

	vi vue.config.js

Then execute the following command to start the debug server

	npm run serve

# [Donate](https://gitee.com/zhaojh329/vuci#project-donate-overview)

# Contributing
If you would like to help making [vuci](https://github.com/zhaojh329/vuci) better,
see the [CONTRIBUTING.md](https://github.com/zhaojh329/vuci/blob/master/CONTRIBUTING.md) file.

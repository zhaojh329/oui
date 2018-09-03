# vuci

![](https://img.shields.io/badge/license-LGPL2-brightgreen.svg?style=plastic "License")

![](/screen-be6656a.gif)

OpenWrt后台管理框架，使用[vuejs2](https://github.com/vuejs/vue)和[Vuetify](https://vuetifyjs.com)实现

一个全新的Web接口.它不再使用Lua，而是使用MVVM框架。通过[ubus](https://wiki.openwrt.org/zh-cn/doc/techref/ubus)存取各种系统数据
(通过[uhttpd-mod-ubus](https://wiki.openwrt.org/zh-cn/doc/techref/ubus#通过http访问ubus)提供基于HTTP的接口API)。

`请保持关注以获取最新的项目动态`

# [vuejs与其它前端框架对比](https://cn.vuejs.org/v2/guide/comparison.html)

# 如何使用
在"feeds.conf.default"里面添加新的feed:
    
    src-git vuci https://gitee.com/zhaojh329/vuci.git

安装vuci软件包:
    
    ./scripts/feeds update
    ./scripts/feeds install -a -p vuci

在menuconfig里面选择vuci软件包然后编译新固件.

    VUCI  --->
        <*>  vuci-ui-base. VUCI Web Interface</*>

编译

    make V=s

# 如何开发和调试
首先克隆代码到你的桌面系统，然后执行如下操作

	cd vuci/vuci-ui-base/src

然后根据自己的环境修改配置。您可能需要修改 proxy。

	vi vue.config.js

然后执行如下命令运行调试服务器

	npm run serve

# [捐赠](https://gitee.com/zhaojh329/vuci#project-donate-overview)

# 贡献代码
如果你想帮助[vuci](https://github.com/zhaojh329/vuci) 变得更好，请参考
[CONTRIBUTING_ZH.md](https://github.com/zhaojh329/vuci/blob/master/CONTRIBUTING_ZH.md)。

# 技术交流
QQ群：153530783

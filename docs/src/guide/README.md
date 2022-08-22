# 快速上手

::: tip
阅读本教程，需要开发者具备 [Vue3](https://cn.vuejs.org/) 的知识。

Oui 默认使用 [Naive UI](https://www.naiveui.com/) 作为其 UI 框架。你可以根据自己需求，选择适合自己的 UI 框架。
:::

## 编译

### 添加 feed

``` bash
echo "src-git oui https://github.com/zhaojh329/oui.git" >> feeds.conf.default
```

### 更新feed

``` bash
./scripts/feeds update oui
./scripts/feeds install -a -p oui
```

### 给 Nginx 打上补丁

参考 patches-nginx 目录

### 配置

```
OUI  --->
    Applications  --->
        <*> oui-app-acl. ACL
        <*> oui-app-backup. Backup / Restore
        <*> oui-app-dhcp-lease. DHCP lease
        <*> oui-app-home. Home
        <*> oui-app-layout. Layout
        <*> oui-app-login. Login
        <*> oui-app-stations. Stations
        <*> oui-app-system. System Configure
        <*> oui-app-upgrade. Upgrade
        <*> oui-app-user. User
  -*- oui-rpc-core. Oui rpc core
  -*- oui-ui-core. Oui ui core
  [*] Use existing nodejs installation on the host system
```

::: tip
编译 Oui 需要用到 Node，而且版本不能低于 14.18。

勾选 CONFIG_OUI_USE_HOST_NODE 可节约编译时间，需要确保主机上安装的 Node 版本不低于 14.18。

[在主机上安装新版本的 Node](https://nodejs.org/en/download/package-manager/)
:::

### 编译

``` bash
make V=s
```

## 调试

首先修改 http 代理: oui-ui-core/htdoc/vite.config.js
```js
{
    server: {
        proxy: {
        '/oui-rpc': {
            target: 'https://openwrt.lan',
            secure: false
        },
        '/oui-upload': {
            target: 'https://openwrt.lan',
            secure: false
        },
        '/oui-download': {
            target: 'https://openwrt.lan',
            secure: false
        }
        }
    }
}
```
将其中的 `https://openwrt.lan` 修改为你的调试设备的地址

1. 使用 vscode 打开 oui 项目
2. 进入 `oui-ui-core/htdoc` 目录
3. 执行 `npm install`
4. 执行 `npm run dev`

执行完 `npm run dev` 后，根据提示打开浏览器。此时对代码中的任何修改，都将立即呈现在浏览器中。

:::tip
创建新的 app 后，需要重新执行 `npm run dev`
:::

## oui app

一个基本的 oui app 的目录结构是这样的
```
oui-app-demo/
├── files
│   ├── menu.json
│   └── rpc
│       └── demo.lua
├── htdoc
│   ├── index.vue
│   ├── locale.json
│   ├── package.json
│   ├── package-lock.json
│   └── vite.config.js
└── Makefile

3 directories, 8 files
```

::: tip
如需创建新的 app，直接复制 oui-app-demo 目录，然后重命名即可
:::

### Makefile 配置

```makefile{9,10}
#
# Copyright (C) 2022 Jianhui Zhao <zhaojh329@gmail.com>
#
# This is free software, licensed under the MIT.
#

include $(TOPDIR)/rules.mk

APP_TITLE:=Demo
APP_NAME:=demo

include ../../oui.mk

# call BuildPackage - OpenWrt buildroot signature
```

* `APP_TITLE` - 对应 OpenWrt 软件包中的 TITLE
* `APP_NAME` - 编译过程，菜单配置文件和打包的 js 文件会以 `APP_NAME` 命名

:::warning
`APP_NAME` 不能重复
:::

### 菜单配置

``` json
{
    "/demo": {
        "title": "Oui Demo",
        "view": "demo",
        "index": 60,
        "locales": {
            "en-US": "Oui Demo",
            "zh-CN": "Oui 示范",
            "zh-TW": "Oui 示範"
        },
        "svg":{"-xmlns":"http://www.w3.org/2000/svg","-xmlns:xlink":"http://www.w3.org/1999/xlink","-viewBox":"0 0 512 512","path":{"-d":"M407.72 208c-2.72 0-14.44.08-18.67.31l-57.77 1.52L198.06 48h-62.81l74.59 164.61l-97.31 1.44L68.25 160H16.14l20.61 94.18c.15.54.33 1.07.53 1.59a.26.26 0 0 1 0 .15a15.42 15.42 0 0 0-.53 1.58L15.86 352h51.78l45.45-55l96.77 2.17L135.24 464h63l133-161.75l57.77 1.54c4.29.23 16 .31 18.66.31c24.35 0 44.27-3.34 59.21-9.94C492.22 283 496 265.46 496 256c0-30.06-33-48-88.28-48zm-71.29 87.9z","-fill":"currentColor"}}
    }
}
```

* `view` - 和 Makefile 中的 `APP_NAME` 一致
* `index` - 用于菜单排序
* `locales` - 菜单标题翻译
* `svg` - 菜单图标

:::tip
如何配置菜单图标：到 [xicons](https://www.xicons.org/#/) 复制所需图标的 svg 代码，然后到
[xml2json](https://www.w3cschool.cn/tools/index?name=xmljson) 这个网站上将 svg 的代码转换为 json 格式。
:::

菜单分为一级菜单和二级菜单。oui-ui-core 默认提供了一些常用的一级菜单
```json
{
    "/status": {
        "title": "Status",
        "icon": "md-stats",
        "index": 10,
        "locales": {
            "en-US": "Status",
            "zh-CN": "状态",
            "zh-TW": "狀態"
        },
        "svg":{"-xmlns":"http://www.w3.org/2000/svg","-xmlns:xlink":"http://www.w3.org/1999/xlink","-viewBox":"0 0 24 24","path":{"-d":"M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10s10-4.48 10-10S17.52 2 12 2zm1 15h-2v-6h2v6zm0-8h-2V7h2v2z","-fill":"currentColor"}}
    },
    "/system": {
        "title": "System",
        "icon": "md-settings",
        "index": 20,
        "locales": {
            "en-US": "System",
            "zh-CN": "系统",
            "zh-TW": "系統"
        },
        "svg":{"-xmlns":"http://www.w3.org/2000/svg","-xmlns:xlink":"http://www.w3.org/1999/xlink","-viewBox":"0 0 24 24","g":{"-fill":"none","path":{"-d":"M4.946 5h14.108C20.678 5 22 6.304 22 7.92v8.16c0 1.616-1.322 2.92-2.946 2.92H4.946C3.322 19 2 17.696 2 16.08V7.92C2 6.304 3.322 5 4.946 5zm0 2A.933.933 0 0 0 4 7.92v8.16c0 .505.42.92.946.92h14.108a.933.933 0 0 0 .946-.92V7.92c0-.505-.42-.92-.946-.92H4.946z","-fill":"currentColor"}}}
    },
    "/network": {
        "title": "Network",
        "icon": "md-git-network",
        "index": 30,
        "locales": {
            "en-US": "Network",
            "zh-CN": "网络",
            "zh-TW": "網絡"
        },
        "svg":{"-xmlns":"http://www.w3.org/2000/svg","-xmlns:xlink":"http://www.w3.org/1999/xlink","-viewBox":"0 0 640 512","path":{"-d":"M640 264v-16c0-8.84-7.16-16-16-16H344v-40h72c17.67 0 32-14.33 32-32V32c0-17.67-14.33-32-32-32H224c-17.67 0-32 14.33-32 32v128c0 17.67 14.33 32 32 32h72v40H16c-8.84 0-16 7.16-16 16v16c0 8.84 7.16 16 16 16h104v40H64c-17.67 0-32 14.33-32 32v128c0 17.67 14.33 32 32 32h160c17.67 0 32-14.33 32-32V352c0-17.67-14.33-32-32-32h-56v-40h304v40h-56c-17.67 0-32 14.33-32 32v128c0 17.67 14.33 32 32 32h160c17.67 0 32-14.33 32-32V352c0-17.67-14.33-32-32-32h-56v-40h104c8.84 0 16-7.16 16-16zM256 128V64h128v64H256zm-64 320H96v-64h96v64zm352 0h-96v-64h96v64z","-fill":"currentColor"}}
    }
}
```

### rpc 接口
一个简单的 rpc 接口: demo.lua
```lua
local M = {}

function M.count(param)
    param.n = param.n + 1
    return param
end

return M
```

```js
this.$oui.call('demo', 'count', { n: 1 }).then(({ n }) => {
    console.log(n)
})
```

输出: 2

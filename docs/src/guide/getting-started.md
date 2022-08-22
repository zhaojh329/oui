# 快速上手

::: tip
阅读本教程，需要开发者具备 [Vue3](https://cn.vuejs.org/) 的知识。

Oui 默认使用 [Naive UI](https://www.naiveui.com/) 作为其 UI 框架。你可以根据自己需求，选择适合自己的 UI 框架。
:::

## 编译

添加 feed
``` bash
echo "src-git oui https://github.com/zhaojh329/oui.git" >> feeds.conf.default
```

更新feed
``` bash
./scripts/feeds update oui
./scripts/feeds install -a -p oui
```

给 Nginx 打上补丁
patches-nginx

配置
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

勾选 CONFIG_OUI_USE_HOST_NODE，可节约编译时间

::: tip
如果勾选 CONFIG_OUI_USE_HOST_NODE，需要确保主机上安装的 Node 版本不低于 14.18。
:::

编译
``` bash
make V=s
```

## 如何创建一个页面

一个基本的页面的目录结构是这样的
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

其中的 svg 为菜单图标。到 [xicons](https://www.xicons.org/#/) 复制需要的图标的 svg 代码，然后到 
[xml2json](https://www.w3cschool.cn/tools/index?name=xmljson) 这个网站上将 svg 的代码转换为 json 格式。

---
home: true
heroImage: /hero.png
heroText: oui
tagline: 基于Vue的OpenWrt后台管理UI框架
actionText: 快速上手 →
actionLink: /guide/
features:
- title: 快
  details: 使用静态页面；通过ubus存取各种系统数据
- title: 多用户
  details: 支持多用户及权限管理
- title: 前后端分离
  details: 术业有专攻（开发人员分离）
- title: Vue驱动
  details: 享受 Vue + webpack 的开发体验
footer: MIT Licensed
---

``` bash
# Add feed
echo "src-git oui https://github.com/zhaojh329/oui.git" >> feeds.conf.default
# OR if exist feeds.conf
echo "src-git oui https://github.com/zhaojh329/oui.git" >> feeds.conf

# install feed of oui
./scripts/feeds update oui
./scripts/feeds install -a -p oui

# configure
oui  --->
  <*>  oui-ui-core..................... Oui ui core

# Build
make V=s
# Or only build oui
make package/feeds/oui/oui-ui-core/compile V=s
```

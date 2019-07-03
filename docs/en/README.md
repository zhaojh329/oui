---
home: true
heroImage: /hero.png
heroText: oui
tagline: Vue-based OpenWrt background management UI framework
actionText: Get Started →
actionLink: /en/guide/
features:
- title: Fast
  details: Use static pages; access any kind of system data via ubus
- title: Multi User
  details: Support for multi-user and access management
- title: Vue + Element-UI
  details: Enjoy the dev experience of Vue + Element-UI
footer: MIT Licensed
---

<iframe src="https://ghbtns.com/github-btn.html?user=zhaojh329&repo=oui&type=star&count=true&size=large" frameborder="0" scrolling="0" width="160px" height="30px"></iframe>
<iframe src="https://ghbtns.com/github-btn.html?user=zhaojh329&repo=oui&type=watch&count=true&size=large&v=2" frameborder="0" scrolling="0" width="160px" height="30px"></iframe>
<iframe src="https://ghbtns.com/github-btn.html?user=zhaojh329&repo=oui&type=fork&count=true&size=large" frameborder="0" scrolling="0" width="158px" height="30px"></iframe>

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

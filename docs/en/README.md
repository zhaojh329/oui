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
- title: Front and rear separation
  details: Specialized in the industry (developer separation)
- title: Vue-Powered
  details: Enjoy the dev experience of Vue + webpack
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

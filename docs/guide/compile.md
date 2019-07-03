# 编译

编译oui需要用到[nodejs](https://nodejs.org)8.11或更高版本。

可以通过配置选择使用主机自带的nodejs，或者OpenWrt自带的nodejs软件包，或者oui自带的nodejs软件包。

推荐使用主机自带的nodejs，因为编译nodejs需要相当长的时间，而在主机上[通过包管理器安装nodejs](https://nodejs.org/en/download/package-manager/)非常快。

## 添加feed

``` bash
echo "src-git oui https://github.com/zhaojh329/oui.git" >> feeds.conf.default
```

如果存在feeds.conf
``` bash
echo "src-git oui https://github.com/zhaojh329/oui.git" >> feeds.conf
```

## 更新feed

``` bash
./scripts/feeds update oui
./scripts/feeds install -a -p oui
```

## 配置

```
OUI  --->
  <*>  oui-ui-core..................... Oui ui core
        Configuration  --->
          Node compiler config (Host Node)  --->
```

## 编译

``` bash
make V=s
```

或者只编译oui
``` bash
make package/feeds/oui/oui-ui-core/compile V=s
```
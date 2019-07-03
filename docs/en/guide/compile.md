# Compile

oui requires [nodejs](https://nodejs.org) version 8.11 or above to compile.

You can choose to use the nodejs that comes with the host, or the nodejs package that comes with OpenWrt, or the nodejs package that comes with oui.

It is recommended to use the nodejs that comes with the host, because it takes a long time to compile nodejs,
and on the host [install nodejs via the package manager](https://nodejs.org/en/download/package-manager/) is very fast.

## Add feed

``` bash
echo "src-git oui https://github.com/zhaojh329/oui.git" >> feeds.conf.default
```

If there is feeds.conf
``` bash
echo "src-git oui https://github.com/zhaojh329/oui.git" >> feeds.conf
```

## Update feed

``` bash
./scripts/feeds update oui
./scripts/feeds install -a -p oui
```

## Configure

```
OUI  --->
  <*>  oui-ui-core..................... Oui ui core
        Configuration  --->
          Node compiler config (Host Node)  --->
```

## Compile

``` bash
make V=s
```

Or just compile oui
``` bash
make package/feeds/oui/oui-ui-core/compile V=s
```
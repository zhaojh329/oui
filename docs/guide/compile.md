# Compile

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
  <*>  oui-ui..................... oui web interface
```

## Compile

``` bash
make V=s
```

Or just compile oui
``` bash
make package/feeds/oui/oui-ui/compile V=s
```

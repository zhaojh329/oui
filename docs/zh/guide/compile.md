# 编译

## 添加feed

``` bash
echo "src-git oui https://gitee.com/zhaojh329/oui.git" >> feeds.conf.default
```

如果存在feeds.conf
``` bash
echo "src-git oui https://gitee.com/zhaojh329/oui.git" >> feeds.conf
```

## 更新feed

``` bash
./scripts/feeds update oui
./scripts/feeds install -a -p oui
```

## 配置

```
OUI  --->
  <*>  oui-ui..................... oui web interface
```

## 编译

``` bash
make V=s
```

或者只编译oui
``` bash
make package/feeds/oui/oui-ui/compile V=s
```

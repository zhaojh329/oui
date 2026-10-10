# 权限管理

## 介绍

Oui 将权限分为权限组，每个权限组里面又分为权限类，每个权限类由多个匹配项构成。每个用户需要为其分配一个权限组。

Oui 默认具有一个名为 `admin` 的权限组，其配置文件为：/usr/share/oui/acl/admin.json

```json
{
    "rpc": {
        "matchs": [".+"]
    },
    "menu": {
        "matchs": [".+"]
    }
}
```

目前共有 2 个权限类：

* rpc - `rpc` 接口调用权限
* menu - 菜单隐藏或显示

匹配项为一个数组，`admin` 权限组中全部的匹配项均为 `.+`，表示匹配任意，即每一类都拥有所有权限。

:::tip
这里的匹配项事实上为一个正则表达式。可以是任意的 `Lua` 正则表达式。
:::

## 反向匹配

```json
{
    "rpc": {
        "matchs": ["^ui%.set_theme$"],
        "reverse": true
    }
}
```
给权限类的 `reverse` 属性设置为 `true` 即可反向匹配。

## 匹配项示例

### rpc

```json
{
    "rpc": {
        "matchs": [".+"]
    }
}
```
匹配所有 `rpc` 接口

```json
{
    "rpc": {
        "matchs": ["^ui%..+"]
    }
}
```
匹配 `ui` 模块里面所有的方法

```json
{
    "rpc": {
        "matchs": ["^ui%..+", "^system%..+"]
    }
}
```
匹配 `ui` 和 `system` 模块里面所有的方法

```json
{
    "rpc": {
        "matchs": ["^ui%.set_theme$"]
    }
}
```
匹配 `ui` 模块里面的 `set_theme` 方法

```json
{
    "rpc": {
        "matchs": ["^ui%.set_theme$"],
        "reverse": true
    }
}
```
不匹配 `ui` 模块的 `set_theme` 方法，即除了 `ui` 模块的 `set_theme` 方法不能调用，其余所有接口均能调用。

### menu

```json
{
    "menu": {
        "matchs": ["^/system/"]
    }
}
```
匹配 `/system/` 开头的菜单

```json
{
    "menu": {
        "matchs": ["^/system/upgrade$"]
    }
}
```
匹配 `/system/upgrade` 菜单

```json
{
    "menu": {
        "matchs": ["^/system/upgrade$"],
        "reverse": true
    }
}
```
隐藏 `/system/upgrade` 菜单

前端操作只通过具体业务模块的 RPC 接口授权。

# ACL

## Introduction

Oui divides permissions into permission groups, and each permission group is divided into permission classes.
Each permission class consists of multiple matching items. Each user needs to be assigned a permission group.

By default, Oui has a permission group named `admin`, whose configuration file is: /usr/share/oui/acl/admin.json

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

Currently, there are two permission classes:

* rpc - `rpc` interface call permission
* menu - Hidden or show menu

The matching items are an array, and all the matching items in the `admin` permission group are `.+ `, indicating any matching, that is, each category has all permissions.

:::tip
The match here is actually a regular expression. Can be any `Lua` regular expression.
:::

## Reverse matching

```json
{
    "rpc": {
        "matchs": ["^ui%.set_theme$"],
        "reverse": true
    }
}
```
Set the `reverse` attribute of the permission class to `true` to reverse the matching.

## Examples of matches

### rpc

```json
{
    "rpc": {
        "matchs": [".+"]
    }
}
```
Matches all `rpc` interfaces

```json
{
    "rpc": {
        "matchs": ["^ui%..+"]
    }
}
```
Matches all methods in the `ui` module

```json
{
    "rpc": {
        "matchs": ["^ui%..+", "^system%..+"]
    }
}
```
Matches all methods in the `ui` and `system` modules

```json
{
    "rpc": {
        "matchs": ["^ui%.set_theme$"]
    }
}
```
Matches the `set_theme` method in the `ui` module

```json
{
    "rpc": {
        "matchs": ["^ui%.set_theme$"],
        "reverse": true
    }
}
```
Does not match the `set_theme` method of the `ui` module, that is, except the `set_theme` method of the `ui` module cannot be called, all other interfaces can be called.

### menu

```json
{
    "menu": {
        "matchs": ["^/system/"]
    }
}
```
Matches menus starting with `/system/`

```json
{
    "menu": {
        "matchs": ["^/system/upgrade$"]
    }
}
```
Match `/system/upgrade` menu

```json
{
    "menu": {
        "matchs": ["^/system/upgrade$"],
        "reverse": true
    }
}
```
Hide the `/system/upgrade` menu

Frontend operations are authorized through business-specific RPC methods.

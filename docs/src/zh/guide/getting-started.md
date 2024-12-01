# 快速上手

## 编译/安装

:::tip
Oui 依赖最新版本的 `Lua-eco`。

请确保你使用的 `OpenWrt` 中的 `Lua-eco` 已更新到最新版本。

你可以直接使用 [https://github.com/openwrt/packages/blob/master/lang/lua-eco/Makefile](https://github.com/openwrt/packages/blob/master/lang/lua-eco/Makefile)
替换 `feeds/packages/lang/lua-eco/Makefile`
:::

### 添加 feed

``` bash
echo "src-git oui https://github.com/zhaojh329/oui.git" >> feeds.conf.default
```

### 更新feed

``` bash
./scripts/feeds update -a
./scripts/feeds install -a -p oui
```

### 配置

```
OUI  --->
    Applications  --->
        <*> oui-app-acl. ACL
        <*> oui-app-backup. Backup / Restore
        <*> oui-app-dhcp-lease. DHCP lease
        <*> oui-app-home. OUI built-in home page
        <*> oui-app-layout. OUI built-in layout page
        <*> oui-app-login. OUI built-in login page
        <*> oui-app-stations. Stations
        <*> oui-app-system. System Configure
        <*> oui-app-upgrade. Upgrade
        <*> oui-app-user. User
  -*- oui-rpc-core. Oui rpc core
  -*- oui-ui-core. Oui ui core
  [*] Use existing nodejs installation on the host system
```

::: tip
编译 Oui 需要用到 Node，而且版本不能低于 20.9。

勾选 `Use existing nodejs installation on the host system` 可节约编译时间，需要确保主机上安装的 Node 版本不低于 20.9。

你可以使用 [nvm](https://github.com/nvm-sh/nvm) 管理多个 Node 版本。
:::

### 编译

``` bash
make V=s
```

::: tip
默认用户名：admin

默认密码：123456
:::

## 开发/调试

首先修改 http 代理: oui-ui-core/htdoc/vite.config.js
```js
{
    server: {
        proxy: {
        '/oui-rpc': {
            target: 'http://openwrt.lan',
            secure: false
        },
        '/oui-upload': {
            target: 'http://openwrt.lan',
            secure: false
        },
        '/oui-download': {
            target: 'http://openwrt.lan',
            secure: false
        }
        }
    }
}
```
将其中的 `http://openwrt.lan` 修改为你的调试设备的地址,如 `http://192.168.1.1`

1. 使用 vscode 打开 oui 项目
2. 进入 `oui-ui-core/htdoc` 目录
3. 执行 `npm install`
4. 执行 `npm run dev`

执行完 `npm run dev` 后，根据提示打开浏览器。此时对代码中的任何修改，都将立即呈现在浏览器中。

:::tip
创建新的 app 后，需要重新执行 `npm run dev`

建议在 wsl 或 linux 虚拟机里做开发
:::

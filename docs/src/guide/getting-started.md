# Get Started

## Build & Install

:::tip
Oui depends on the latest version of `Lua-eco`.

Make sure that the `Lua-eco` in OpenWrt you are using is up to date.

You can replace `feeds/packages/lang/lua-eco/Makefile` with
[https://github.com/openwrt/packages/blob/master/lang/lua-eco/Makefile](https://github.com/openwrt/packages/blob/master/lang/lua-eco/Makefile).
:::

### Add feed

``` bash
echo "src-git oui https://github.com/zhaojh329/oui.git" >> feeds.conf.default
```

### Update feed

``` bash
./scripts/feeds update -a
./scripts/feeds install -a -p oui
```

### Configure

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
The `Node.js 20.9+` is required to compile Oui.

Select `Use existing nodejs installation on the host system` to reduce compilation time.

You can manage multiple versions of Node with [nvm](https://github.com/nvm-sh/nvm).
:::

### Build

``` bash
make V=s
```

::: tip
Default username: admin

Default password: 123456
:::

## Development & Debugging

Start by modifying the HTTP proxy: oui-ui-core/htdoc/vite.config.js
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
Change the `http://openwrt.lan` to the address of your debug device, such as `http://192.168.1.1`

1. Open the OUI project using VSCode
2. Enter into the directory: `oui-ui-core/htdoc`
3. Execute `npm install`
4. Execute `npm run dev`

After running `npm run dev`, open the browser as prompted. Any changes made to the code at this point are immediately rendered in the browser.

:::tip
After creating a new app, you need to run `npm run dev` again.
:::

# Get Started

## Build & Install

:::tip
Oui depends on the latest version of `Lua-eco`.

If you are using a earlier OpenWrt, you will need to upgrade it: 

[https://github.com/openwrt/packages/tree/master/lang/lua-eco](https://github.com/openwrt/packages/tree/master/lang/lua-eco)
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
The `Node.js 14.18+` is required to compile Oui.

Select `CONFIG_OUI_USE_HOST_NODE` to reduce compilation time.

[Install the new version of Node on the host](https://nodejs.org/en/download/package-manager/)

You may have selected the configuration related to Luci before, which will conflict with oui, so you need to deselect it.
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

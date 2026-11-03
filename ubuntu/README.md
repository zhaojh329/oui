# Run OUI on Ubuntu

[中文](README_ZH.md)

This example provides login, menus, theme and language settings, an Ubuntu system overview,
and the demo application. lighttpd serves the production frontend and forwards SCGI requests
to the lua-eco backend. It reuses `applications/oui-app-login`, `applications/oui-app-layout`
and `applications/oui-app-demo`, with the Ubuntu home page in this directory.

The default URL is <http://127.0.0.1:8080>. Confirming **Reboot** in the layout restarts the
**Ubuntu host**. Run the backend as root so it can write UCI settings and request a reboot.

## 1. Install dependencies

Skip dependencies already installed. These instructions target Ubuntu 26.04. On older
releases, check the Lua 5.4 and Node.js versions; Lua 5.1/5.3 modules cannot replace Lua 5.4 modules.

```sh
sudo apt update
sudo apt install -y build-essential cmake git pkg-config libjson-c-dev \
    lua5.4 liblua5.4-dev lighttpd lighttpd-modules-lua nodejs npm
node --version
```

The frontend requires Node.js >= 20.9. If your Ubuntu release packages an older version,
install a supported LTS release using the [official Node.js instructions](https://nodejs.org/en/download).

### lua-cjson

Ubuntu 26.04 provides a Lua 5.4 module in the `lua-cjson` package:

```sh
sudo apt install -y lua-cjson
lua5.4 -e 'print(require("cjson").encode({ok = true}))'
```

On older releases without a Lua 5.4 cjson package, build
[lua-cjson from source](https://github.com/mpx/lua-cjson). Specify the Lua version and
header directory explicitly to select the correct ABI:

```sh
mkdir -p "$HOME/src/oui-deps"
cd "$HOME/src/oui-deps"
git clone https://github.com/mpx/lua-cjson.git
cd lua-cjson
make LUA_VERSION=5.4 LUA_INCLUDE_DIR=/usr/include/lua5.4
sudo make install LUA_VERSION=5.4 LUA_INCLUDE_DIR=/usr/include/lua5.4 PREFIX=/usr/local
lua5.4 -e 'print(require("cjson").encode({ok = true}))'
```

### libubox and libuci

OUI uses `eco.uci` for accounts, theme and language settings. This requires libuci and its
libubox dependency, but does not require OpenWrt or a running ubus daemon.

Install [libubox](https://github.com/openwrt/libubox) first:

```sh
mkdir -p "$HOME/src/oui-deps"
cd "$HOME/src/oui-deps"
git clone https://github.com/openwrt/libubox.git
cmake -S libubox -B libubox/build -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DBUILD_LUA=OFF -DBUILD_EXAMPLES=OFF
cmake --build libubox/build -j"$(nproc)"
sudo cmake --install libubox/build
sudo ldconfig
```

Then install [libuci](https://github.com/openwrt/uci):

```sh
cd "$HOME/src/oui-deps"
git clone https://github.com/openwrt/uci.git
cmake -S uci -B uci/build -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_LUA=OFF
cmake --build uci/build -j"$(nproc)"
sudo cmake --install uci/build
sudo ldconfig
```

These options disable the projects' own Lua bindings. OUI uses the binding provided by lua-eco.

### lua-eco

Build [lua-eco](https://github.com/zhaojh329/lua-eco) after installing libuci:

```sh
cd "$HOME/src/oui-deps"
git clone --recursive https://github.com/zhaojh329/lua-eco.git
cmake -S lua-eco -B lua-eco/build -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build lua-eco/build -j"$(nproc)"
sudo cmake --install lua-eco/build
sudo ldconfig
```

The CMake output must not contain `Skip build eco.uci`. This example uses a local Unix
socket and does not require lua-eco TLS or SSH support; enable those modules separately if needed.

## 2. Build the frontend

Build the core frontend, the existing login/layout/demo applications, and Ubuntu home.
`--package-lock=false` avoids creating extra lock files. Environment variables select the
page names without editing `.env.local`.

```sh
(
    cd oui-ui-core/htdoc
    npm install --package-lock=false
    VITE_OUI_LOGIN_VIEW=login VITE_OUI_LAYOUT_VIEW=layout VITE_OUI_HOME_VIEW=home npm run build
)

for app in login layout demo; do
    (
        cd "applications/oui-app-$app/htdoc"
        npm install --package-lock=false
        VITE_APP_NAME="$app" npm run build
    ) || exit 1
done

(
    cd ubuntu/oui-app-home/htdoc
    npm install --package-lock=false
    VITE_APP_NAME=home npm run build
)
```

This uses production builds, not `npm run dev`. No OpenWrt SDK or OpenWrt Makefile is needed.

## 3. Install OUI files

### Shared backend, configuration and Ubuntu RPC

These commands perform a first installation and preserve an existing `/etc/config/oui`:

```sh
sudo install -d /usr/local/sbin /usr/local/share/lua/5.4/oui \
    /etc/config /usr/share/oui/acl /usr/share/oui/rpc /usr/share/oui/menu.d
sudo install -m 0755 oui-rpc-core/files/oui.lua /usr/local/sbin/oui
sudo install -m 0644 oui-rpc-core/files/rpc.lua /usr/local/share/lua/5.4/oui/rpc.lua

if ! sudo test -e /etc/config/oui; then
    sudo install -m 0600 oui-rpc-core/files/oui.config /etc/config/oui
fi

sudo install -m 0644 oui-rpc-core/files/admin.acl /usr/share/oui/acl/admin.json
sudo install -m 0644 oui-rpc-core/files/rpc/ui.lua /usr/share/oui/rpc/ui.lua
sudo install -m 0644 ubuntu/files/rpc/system.lua /usr/share/oui/rpc/system.lua
sudo install -m 0644 ubuntu/files/menu.json /usr/share/oui/menu.d/ubuntu.json
sudo install -m 0644 applications/oui-app-demo/files/menu.json /usr/share/oui/menu.d/demo.json
sudo install -m 0644 applications/oui-app-demo/files/rpc/demo.lua /usr/share/oui/rpc/demo.lua
```

Do not copy the entire `oui-rpc-core/files/rpc/` directory: its `system.lua` and `network.lua`
depend on OpenWrt. This minimal example also omits `user.lua`; manage accounts locally through UCI.
If another OUI example was previously installed, check existing RPC modules and menus for stale entries.

The shared `rpc.lua` does not depend on ubus. OpenWrt packaging installs `oui-openwrt.lua`
as `/usr/sbin/oui`; it injects `rpc.reload_config` before running `/usr/share/oui/oui.lua`.
Ubuntu runs the shared backend directly and does not install that entry point.

### Static files

```sh
sudo install -d /var/www/oui/views
sudo cp -R oui-ui-core/htdoc/dist/. /var/www/oui/
sudo cp -R applications/oui-app-login/htdoc/dist/. /var/www/oui/views/
sudo cp -R applications/oui-app-layout/htdoc/dist/. /var/www/oui/views/
sudo cp -R applications/oui-app-demo/htdoc/dist/. /var/www/oui/views/
sudo cp -R ubuntu/oui-app-home/htdoc/dist/. /var/www/oui/views/
sudo chmod -R a+rX /var/www/oui
```

Keep the entry filename `index.html`. Builds compress some resources into `.gz` files.
The magnet handler below serves them according to the browser's `Accept-Encoding` header.

### lighttpd

Ubuntu uses `conf-available/conf-enabled`. Do not copy the OpenWrt `conf.d` configuration
or run `oui.default`.

```sh
sudo install -m 0644 oui-rpc-core/files/lighttpd/handler.lua /etc/lighttpd/oui-handler.lua
sudo install -m 0644 ubuntu/files/lighttpd.conf /etc/lighttpd/conf-available/50-oui.conf
sudo lighty-enable-mod oui
sudo lighttpd -tt -f /etc/lighttpd/lighttpd.conf
```

`50-oui.conf` loads `mod_scgi` and `mod_magnet`. If another configuration already loads
these modules, keep only one loading declaration. Ubuntu 26.04 includes SCGI support in
lighttpd and supplies magnet through `lighttpd-modules-lua`; do not use the OpenWrt package
name `lighttpd-mod-scgi`.

The configuration adds a `127.0.0.1:8080` listener without replacing the existing port 80 site.
The backend reads `server.username` from `/etc/lighttpd/lighttpd.conf` to set ownership of
`/var/run/oui.sock`. Keep this declaration in the main configuration; Ubuntu defaults to `www-data`.

### Start the backend manually

restart lighttpd to load its configuration:

```sh
sudo service lighttpd restart
```

Run the oui backend:

```sh
sudo eco /usr/local/sbin/oui
```

## 4. Login and daily use

Open <http://127.0.0.1:8080>. The initial credentials are `admin` / `123456`.
If you preserved an existing configuration, use its credentials instead.

The home page displays local system information. Theme and language settings are stored
in `/etc/config/oui`. The reboot action schedules a reboot after two seconds so the current
RPC response can reach the browser first.

To change the initial administrator password, run the following in Bash. Passwords are
stored as `MD5(username:password)`; interactive input keeps the plaintext out of shell history:

```bash
read -r -s -p 'New admin password: ' oui_password
printf '\n'
oui_password_hash=$(printf 'admin:%s' "$oui_password" | md5sum | cut -d ' ' -f 1)
sudo uci set "oui.@user[0].password=$oui_password_hash"
sudo uci commit oui
unset oui_password oui_password_hash
```

This targets the first user, `admin`, in the default configuration. If you changed the
username or section order, adjust both the section and the username used in the hash.

After updating Lua files or changing the password, stop and restart the backend as described above.
Logs go to syslog by default. To use a file, set the following options and restart the backend:

```sh
sudo uci set oui.global.log_path=/var/log/oui.log
sudo uci commit oui
```

```sh
sudo tail -n 50 /var/log/oui.log
sudo tail -n 50 /var/log/lighttpd/error.log
```

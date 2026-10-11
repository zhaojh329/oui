# 在 Ubuntu 上运行 OUI

[English](README.md)

本示例在 Ubuntu 上运行完整的登录、菜单、主题和语言设置，以及 Ubuntu 系统概览。
前端使用生产构建，由 lighttpd 提供静态文件并通过 SCGI 调用 lua-eco 后端。
直接复用 `applications/oui-app-login` 、`applications/oui-app-layout` 和 `applications/oui-app-demo`；
首页使用本目录的 `oui-app-home`，不安装 OpenWrt 专用应用。

默认访问地址为 <http://127.0.0.1:8080>。layout 中的“重启”会在确认后重启 **Ubuntu 主机**。
后端服务以 root 运行，以便写入 UCI 配置和执行重启。

## 1. 安装依赖

已安装的依赖可以跳过。以下命令以 Ubuntu 26.04 为基准；旧版系统需要自行确认
Lua 5.4 和 Node.js 版本，不能用 Lua 5.1/5.3 的模块替代 Lua 5.4 模块。

```sh
sudo apt update
sudo apt install -y build-essential cmake git pkg-config libjson-c-dev \
    lua5.4 liblua5.4-dev lighttpd lighttpd-modules-lua nodejs npm
node --version
```

前端要求 Node.js >= 20.9。旧版 Ubuntu 的 apt 若提供更早版本，先按照
[Node.js 官方安装说明](https://nodejs.org/en/download)安装受支持的 LTS 版本。

### lua-cjson

Ubuntu 26.04 的 `lua-cjson` 包包含 Lua 5.4 模块：

```sh
sudo apt install -y lua-cjson
lua5.4 -e 'print(require("cjson").encode({ok = true}))'
```

旧版 Ubuntu 如果没有 Lua 5.4 对应的 cjson，使用
[lua-cjson 源码](https://github.com/mpx/lua-cjson)编译安装。以下命令明确指定
Lua 版本和头文件目录，避免编译成其他 Lua ABI：

```sh
mkdir -p "$HOME/src/oui-deps"
cd "$HOME/src/oui-deps"
git clone https://github.com/mpx/lua-cjson.git
cd lua-cjson
make LUA_VERSION=5.4 LUA_INCLUDE_DIR=/usr/include/lua5.4
sudo make install LUA_VERSION=5.4 LUA_INCLUDE_DIR=/usr/include/lua5.4 PREFIX=/usr/local
lua5.4 -e 'print(require("cjson").encode({ok = true}))'
```

### libubox 和 libuci

OUI 的账户、主题和语言配置使用 `eco.uci`，因此 Ubuntu 也需要 libuci，
而 libuci 依赖 libubox。它们不要求运行 OpenWrt 或 ubus 守护进程。

先安装 [libubox](https://github.com/openwrt/libubox)：

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

再安装 [libuci](https://github.com/openwrt/uci)：

```sh
cd "$HOME/src/oui-deps"
git clone https://github.com/openwrt/uci.git
cmake -S uci -B uci/build -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_LUA=OFF
cmake --build uci/build -j"$(nproc)"
sudo cmake --install uci/build
sudo ldconfig
```

这里关闭的是这两个项目自己的 Lua 绑定；OUI 使用的绑定由 lua-eco 提供。

### lua-eco

在 libuci 安装完成后编译 [lua-eco](https://github.com/zhaojh329/lua-eco)：

```sh
cd "$HOME/src/oui-deps"
git clone --recursive https://github.com/zhaojh329/lua-eco.git
cmake -S lua-eco -B lua-eco/build -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build lua-eco/build -j"$(nproc)"
sudo cmake --install lua-eco/build
sudo ldconfig
```

检查 CMake 输出中不能出现 `Skip build eco.uci`。本示例的后端使用本机 Unix socket，
不需要 lua-eco 的 TLS 或 SSH 模块；以后需要这些功能时可另行启用。

## 2. 构建前端

构建公共前端、现有 login/layout/demo，以及 Ubuntu home。`--package-lock=false`
避免为这些示例目录生成额外的 lock 文件。环境变量明确指定页面名，不需要修改 `.env.local`。

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

这里不使用 `npm run dev`，也不需要 OpenWrt SDK 或执行 OpenWrt Makefile。

## 3. 安装 OUI 文件

### 公共后端、配置与 Ubuntu RPC

以下是首次安装步骤。已有 `/etc/config/oui` 时保留原有账号和设置。

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

不要直接复制整个 `oui-rpc-core/files/rpc/`：其中 `system.lua`、`network.lua`
依赖 OpenWrt。这个最小示例也不安装 `user.lua`，账户在本地通过 UCI 配置管理。
若机器曾安装其他 OUI 示例，先检查已有 RPC 和菜单，避免混入旧页面。

`rpc.lua` 是公共模块，不依赖 ubus。OpenWrt 打包时将 `oui-openwrt.lua` 安装为
`/usr/sbin/oui`，启动时向公共模块注入 `rpc.reload_config`，再执行
`/usr/share/oui/oui.lua`；Ubuntu 不安装该入口，直接执行公共后端。

### 静态页面

```sh
sudo install -d /var/www/oui/views
sudo cp -R oui-ui-core/htdoc/dist/. /var/www/oui/
sudo cp -R applications/oui-app-login/htdoc/dist/. /var/www/oui/views/
sudo cp -R applications/oui-app-layout/htdoc/dist/. /var/www/oui/views/
sudo cp -R applications/oui-app-demo/htdoc/dist/. /var/www/oui/views/
sudo cp -R ubuntu/oui-app-home/htdoc/dist/. /var/www/oui/views/
sudo chmod -R a+rX /var/www/oui
```

保持入口名 `index.html`。构建会把部分资源压缩成 `.gz`，下面的 magnet 脚本负责
按照浏览器的 `Accept-Encoding` 返回资源。

### lighttpd

Ubuntu 使用 `conf-available/conf-enabled`，不直接使用 OpenWrt 的 `conf.d` 配置，
也不执行 `oui.default`。

```sh
sudo install -m 0644 oui-rpc-core/files/lighttpd/handler.lua /etc/lighttpd/oui-handler.lua
sudo install -m 0644 ubuntu/files/lighttpd.conf /etc/lighttpd/conf-available/50-oui.conf
sudo lighty-enable-mod oui
sudo lighttpd -tt -f /etc/lighttpd/lighttpd.conf
```

`50-oui.conf` 加载 `mod_scgi` 和 `mod_magnet`。若已在其他配置启用这两个模块，
只保留一处模块加载声明。Ubuntu 26.04 的 SCGI 支持在 lighttpd 中，magnet 来自
`lighttpd-modules-lua`，不使用 OpenWrt 的 `lighttpd-mod-scgi` 包名。

该配置增加 `127.0.0.1:8080` 监听，不替换已有 80 端口站点。
公共后端读取 `/etc/lighttpd/lighttpd.conf` 中的 `server.username`，设置
`/var/run/oui.sock` 的属主。Ubuntu 默认是 `www-data`，应保留主配置中的这项声明。

### 手动启动后端

重启 lighttpd 以加载配置：

```sh
sudo service lighttpd restart
```

启动 oui 后端：

```sh
sudo eco /usr/local/sbin/oui
```

## 4. 登录与日常操作

打开 <http://127.0.0.1:8080>。首次安装配置的账号为 `admin`，密码为 `123456`。
保留了旧配置时使用原账号密码。

首页展示本机信息；主题和语言写入 `/etc/config/oui`。重启菜单安排
两秒后的系统重启，让当前 RPC 响应先返回浏览器。

可在本机修改管理员密码。密码存储格式为 `MD5(username:password)`，以下命令
使用交互输入，避免把明文密码写入 shell 历史（在 Bash 中执行）：

```bash
read -r -s -p 'New admin password: ' oui_password
printf '\n'
oui_password_hash=$(printf 'admin:%s' "$oui_password" | md5sum | cut -d ' ' -f 1)
sudo uci set "oui.@user[0].password=$oui_password_hash"
sudo uci commit oui
unset oui_password oui_password_hash
```

以上针对默认配置中的第一个用户 `admin`。账号顺序或用户名已修改时，相应调整
section 和计算散列时的用户名。

更新 Lua 文件或修改密码后，按上面的步骤停止并重新启动后端。
默认日志输出到 syslog；也可以设置日志文件后重新启动：

```sh
sudo uci set oui.global.log_path=/var/log/oui.log
sudo uci commit oui
```

```sh
sudo tail -n 50 /var/log/oui.log
sudo tail -n 50 /var/log/lighttpd/error.log
```

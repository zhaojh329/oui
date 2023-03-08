# Lua 接口

在 Oui 中，Lua 接口以 `模块-方法` 的形式进行组织。

```sh
root@OpenWrt:~# ls /usr/share/oui/rpc/
acl.lua       network.lua   ubus.lua      ui.lua        wireless.lua
demo.lua      system.lua    uci.lua       user.lua
```

这里的每个 Lua 文件代表着一个模块。模块名为 Lua 文件名（不带后缀）。

每个 Lua 接口文件需要返回一个 `Lua Table`，该 `Lua Table` 由多个 `Lua function` 组成。

```lua
-- /usr/share/oui/rpc/test.lua

local M = {}

--[[
params: 前端调用传递的参数
section: 登录的会话信息，为一个 Table，
         包含当前登录的用户名(username)和其所属的权限组(acl)
--]]
function M.func1(params, section)
    local res = {}
    ...
    return res
end

return M
```

```js
this.$oui.call('test', 'func1', {a: 1}).then(res => {
    ...
})
```

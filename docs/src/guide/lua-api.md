# Lua API

In Oui, Lua API are organized as `module-methods`.

```sh
root@OpenWrt:~# ls /usr/share/oui/rpc/
acl.lua       network.lua   ubus.lua      ui.lua        wireless.lua
demo.lua      system.lua    uci.lua       user.lua
```

Each Lua file here represents a module. Module name is Lua file name(without suffix). 

Each Lua API file needs to return a `Lua Table`, which consists of multiple `Lua functions`.

```lua
-- /usr/share/oui/rpc/test.lua

local M = {}

--[[
params: Parameters passed by the front-end call
section: The login session information is a Table.
         Contains the currently logged in username (username) and the permission group (acl) to which it belongs.
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

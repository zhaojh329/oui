# 介绍

[1]: https://github.com/zhaojh329/lua-eco
[2]: https://github.com/vuejs/core
[3]: https://github.com/vitejs/vite

Oui 是一个用来开发 `OpenWrt` Web 接口的`框架`。

Oui 使用 [Lua-eco][1] 开发其静态文件服务器。

Oui 前端使用 [Vue3][2] 编写，使用 [Vite][3] 构建前端代码。

不同于传统的前端项目，所有的页面作为一个整体进行打包。Oui 实现了和 Luci 一样的模块化，每个页面独立打包，互不影响。其处理方式为将每个页面以库的形式进行打包。

::: tip
Oui 默认使用 [Naive UI](https://www.naiveui.com/) 组件库。你可以根据自己的需求，选择适合自己的组件库或者自己开发组件。
:::

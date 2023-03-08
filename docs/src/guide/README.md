# Introduction

[1]: https://github.com/zhaojh329/lua-eco
[2]: https://github.com/vuejs/core
[3]: https://github.com/vitejs/vite

Oui is a `framework` for developing `OpenWrt` Web interfaces.

Oui uses [Lua-eco][1] as to build its static file server.

The Oui front-end is written in [Vue3][2], and the front-end code is build with [Vite][3].

Unlike traditional front-end projects, all pages are packaged as a whole. Oui implements the same modularity as Luci, with each page packaged independently of the other. This is done by packaging each page as a library.

::: tip
Oui uses the [Naive UI](https://www.naiveui.com/) component library by default. You can choose your own library or develop your own components according to your needs.
:::

/*
 * MIT License
 *
 * Copyright (c) 2020 Jianhui Zhao <zhaojh329@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <sys/statvfs.h>
#include <libubox/md5.h>
#include <uhttpd/log.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#include "lua_utils.h"

static int lua_md5sum(lua_State *L)
{
    const char *file = luaL_checkstring(L, 1);
    char md5[33] = "";
    uint8_t buf[16];
    int i;

    if (md5sum(file, buf)) {
        lua_pushnil(L);
        return 1;
    }

    for (i = 0; i < 16; i++)
        sprintf(md5 + i * 2, "%02x", buf[i]);

    lua_pushstring(L, md5);

    return 1;
}

static int lua_statvfs(lua_State *L)
{
    const char *path = lua_tostring(L, 1);
    struct statvfs s;

    if (!path) {
        lua_pushnil(L);
        return 1;
    }

    if (statvfs(path, &s)) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, s.f_blocks * s.f_frsize);
    lua_pushinteger(L, s.f_bfree * s.f_frsize);
    lua_pushinteger(L, (s.f_blocks - s.f_bfree) * s.f_frsize);

    return 3;
}

static int lua_parse_route_addr(lua_State *L)
{
    const char *addr = lua_tostring(L, 1);
    const char *mask = lua_tostring(L, 2);
    char as[sizeof("255.255.255.255/32\0")];
    struct in_addr a;
    int bits;

    if (!addr) {
        lua_pushnil(L);
        return 1;
    }

    a.s_addr = strtoul(addr, NULL, 16);
    inet_ntop(AF_INET, &a, as, sizeof(as));

    if (mask) {
        for (a.s_addr = ntohl(strtoul(mask, NULL, 16)), bits = 0;
             a.s_addr & 0x80000000;
             a.s_addr <<= 1)
            bits++;

        sprintf(as + strlen(as), "/%u", bits);
    }

    lua_pushstring(L, as);
    return 1;
}

static int lua_parse_route6_addr(lua_State *L)
{
    const char *addr = lua_tostring(L, 1);
    const char *mask = lua_tostring(L, 2);
    char as[INET6_ADDRSTRLEN + sizeof("/128")];
    struct in6_addr a;
    int i;

#define hex(x) \
    (((x) <= '9') ? ((x) - '0') : \
        (((x) <= 'F') ? ((x) - 'A' + 10) : \
            ((x) - 'a' + 10)))

    if (!addr) {
        lua_pushnil(L);
        return 1;
    }


    for (i = 0; i < 16; i++, addr += 2)
        a.s6_addr[i] = (16 * hex(*addr)) + hex(*(addr + 1));

    inet_ntop(AF_INET6, &a, as, sizeof(as));

    if (mask)
        sprintf(as + strlen(as), "/%lu", strtoul(mask, NULL, 16));

    lua_pushstring(L, as);
    return 1;
}

static const luaL_Reg regs[] = {
    {"md5sum",            lua_md5sum},
    {"statvfs",           lua_statvfs},
    {"parse_route_addr",  lua_parse_route_addr},
    {"parse_route6_addr", lua_parse_route6_addr},
    {NULL, NULL}
};

void luaopen_utils(lua_State *L)
{
#if LUA_VERSION_NUM <= 501
    luaL_register(L, "utils", regs);
#else
    luaL_newlib(L, regs);
    lua_pushvalue(L, -1);
    lua_setglobal(L, "utils");
#endif

    if (luaL_dofile(L, "/usr/lib/oui-httpd/lib/utils.lua")) {
        uh_log_err("%s\n", lua_tostring(L, -1));
        return;
    }

    if (!lua_istable(L, -1))
        return;

    lua_getglobal(L, "utils");

    lua_pushvalue(L, -2);

    lua_pushnil(L);

    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -3);
        lua_settable(L, -5);
    }
}

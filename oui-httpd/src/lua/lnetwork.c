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

#include <arpa/inet.h>
#include <lauxlib.h>
#include <string.h>
#include <stdlib.h>

#include "lua_compat.h"

/*
 * hexaddr("000011AC")
 * hexaddr("000011AC", "0000FFFF")
 */
static int lua_hexaddr(lua_State *L)
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

static int lua_hex6addr(lua_State *L)
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
    {"hexaddr",  lua_hexaddr},
    {"hex6addr", lua_hex6addr},
    {NULL, NULL}
};

int luaopen_oui_network(lua_State *L)
{
    luaL_newlib(L, regs);

    return 1;
}

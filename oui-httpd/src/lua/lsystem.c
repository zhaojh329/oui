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

#include <sys/sendfile.h>
#include <stdbool.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <shadow.h>
#include <errno.h>
#include <crypt.h>
#include <fcntl.h>

#include "lua_compat.h"

static bool login_test(const char *username, const char *password)
{
    struct crypt_data data = {};
    struct spwd sp, *spp;
    char buf[512];

    if (!username || *username == 0)
        return false;

    if (getspnam_r(username, &sp, buf, sizeof(buf), &spp))
        return false;

    if (!spp || !spp->sp_pwdp[1])
        return false;

    if (!password)
        password = "";

    return !strcmp(crypt_r(password, spp->sp_pwdp, &data), spp->sp_pwdp);
}

static int lua_password(lua_State *L)
{
    const char *username = luaL_checkstring(L, 1);
    const char *old_password = luaL_checkstring(L, 2);
    const char *new_password = luaL_checkstring(L, 3);
    char template[] = "shadow-XXXXXX";
    bool changed = false;
    FILE *ofp, *nfp;
    int ofd, nfd;
    char buf[512];
    long size;
    int ret = 2;

    if (!login_test(username, old_password)) {
        lua_pushboolean(L, false);
        lua_pushstring(L, "Operation not permitted");
        return 2;
    }

    nfd = mkostemp(template, O_RDWR);
    if (nfd < 0) {
        lua_pushboolean(L, false);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    nfp = fdopen(nfd, "w");

    ofp = fopen("/etc/shadow", "r");
    if (ofp < 0) {
        lua_pushboolean(L, false);
        lua_pushstring(L, strerror(errno));
        goto err;
    }

    while (true) {
        struct spwd sp, *spp;

        if (fgetspent_r(ofp, &sp, buf, sizeof(buf), &spp))
            break;

        if (!strcmp(sp.sp_namp, username)) {
            struct crypt_data data = {};
            char *np;

            np = crypt_r(new_password, sp.sp_pwdp, &data);
            strcpy(sp.sp_pwdp, np);

            changed = true;
        }

        putspent(&sp, nfp);
    }

    fclose(ofp);

    if (!changed) {
        lua_pushboolean(L, false);
        lua_pushstring(L, "not found");
        goto err;
    }

    fflush(nfp);

    size = ftell(nfp);

    nfd = fileno(nfp);

    lseek(nfd, 0, SEEK_SET);

    ofd = open("/etc/shadow", O_WRONLY | O_TRUNC);
    if (ofd < 0) {
        lua_pushboolean(L, false);
        lua_pushstring(L, strerror(errno));
        goto err;
    }

    sendfile(ofd, nfd, NULL, size);

    close(ofd);

    ret = 1;
    lua_pushboolean(L, true);

err:
    close(nfd);
    unlink(template);

    return ret;
}

static const luaL_Reg regs[] = {
    {"password", lua_password},
    {NULL, NULL}
};

int luaopen_oui_system(lua_State *L)
{
    luaL_newlib(L, regs);

    return 1;
}

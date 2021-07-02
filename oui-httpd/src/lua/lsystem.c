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

#include <stdbool.h>
#include <lauxlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
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

    if (!spp)
        return false;

    /* empty password */
    if (!spp->sp_pwdp || !spp->sp_pwdp[0])
        return true;

    if (!password)
        password = "";

    return !strcmp(crypt_r(password, spp->sp_pwdp, &data), spp->sp_pwdp);
}

static int lua_login(lua_State *L)
{
    const char *username = luaL_checkstring(L, 1);
    const char *password = lua_tostring(L, 2);
    bool valid = login_test(username, password);

    lua_pushboolean(L, valid);

    return 1;
}

static int lua_password(lua_State *L)
{
    const char *username = luaL_checkstring(L, 1);
    const char *old_password = luaL_checkstring(L, 2);
    const char *new_password = luaL_checkstring(L, 3);
    const char *cmd = "/bin/passwd";
    int fd, fds[2];
    struct stat s;
    pid_t pid;

    if (!login_test(username, old_password)) {
        lua_pushinteger(L, -1);
        lua_pushstring(L, "operation not permitted");
        return 2;
    }

    if (stat(cmd, &s)) {
        cmd = "/usr/bin/passwd";

        if (stat(cmd, &s)) {
            lua_pushinteger(L, -2);
            lua_pushstring(L, "not found command");
            return 2;
        }
    }

    if (!(s.st_mode & S_IXUSR)) {
        lua_pushinteger(L, -1);
        lua_pushstring(L, "operation not permitted");
        return 2;
    }

    if (pipe(fds)) {
        lua_pushinteger(L, -3);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    switch ((pid = fork())) {
    case -1:
        close(fds[0]);
        close(fds[1]);

        lua_pushinteger(L, -3);
        lua_pushstring(L, strerror(errno));
        return 2;

    case 0:
        dup2(fds[0], 0);
        close(fds[0]);
        close(fds[1]);

        if ((fd = open("/dev/null", O_RDWR)) > -1) {
            dup2(fd, 1);
            dup2(fd, 2);
            close(fd);
        }

        execl(cmd, "passwd", username, NULL);

        return 0;

    default:
        close(fds[0]);

        if (write(fds[1], new_password, strlen(new_password)));
        if (write(fds[1], "\n", 1));

        usleep(100 * 1000);

        if (write(fds[1], new_password, strlen(new_password)));
        if (write(fds[1], "\n", 1));

        close(fds[1]);

        waitpid(pid, NULL, 0);

        lua_pushinteger(L, 0);

        return 1;
    }
}

static const luaL_Reg regs[] = {
    {"login", lua_login},
    {"password", lua_password},
    {NULL, NULL}
};

int luaopen_oui_system(lua_State *L)
{
    luaL_newlib(L, regs);

    return 1;
}

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
#include <sys/stat.h>
#include <lauxlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include "lua_compat.h"


#define DIR_MT_NAME "oui-httpd-dir"

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

    /* total bytes */
    lua_pushinteger(L, s.f_blocks * s.f_frsize);

    /* available bytes */
    lua_pushinteger(L, s.f_bfree * s.f_frsize);

    /* used bytes */
    lua_pushinteger(L, (s.f_blocks - s.f_bfree) * s.f_frsize);

    return 3;
}

static int lua_access(lua_State *L)
{
    const char *file = luaL_checkstring(L, 1);
    const char *mode = lua_tostring(L, 2);
    int md = F_OK;

    if (mode) {
        if (strchr(mode, 'x'))
            md |= X_OK;
        else if (strchr(mode, 'w'))
            md |= W_OK;
        else if (strchr(mode, 'r'))
            md |= R_OK;
    }

    lua_pushboolean(L, !access(file, md));

    return 1;
}

static int lua_stat(lua_State *L)
{
    const char *pathname = luaL_checkstring(L, 1);
    struct stat st;

    if (stat(pathname, &st)) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    lua_newtable(L);

    switch (st.st_mode & S_IFMT) {
    case S_IFBLK: lua_pushstring(L, "BLK");  break;
    case S_IFCHR: lua_pushstring(L, "CHR");  break;
    case S_IFDIR: lua_pushstring(L, "DIR");  break;
    case S_IFIFO: lua_pushstring(L, "FIFO"); break;
    case S_IFLNK: lua_pushstring(L, "LNK");  break;
    case S_IFREG: lua_pushstring(L, "REG");  break;
    case S_IFSOCK:lua_pushstring(L, "SOCK"); break;
    default:      lua_pushstring(L, "");     break;
    }
    lua_setfield(L, -2, "type");

    lua_pushinteger(L, st.st_atime);
    lua_setfield(L, -2, "atime");

    lua_pushinteger(L, st.st_mtime);
    lua_setfield(L, -2, "mtime");

    lua_pushinteger(L, st.st_ctime);
    lua_setfield(L, -2, "ctime");

    lua_pushinteger(L, st.st_nlink);
    lua_setfield(L, -2, "nlink");

    lua_pushinteger(L, st.st_uid);
    lua_setfield(L, -2, "uid");

    lua_pushinteger(L, st.st_gid);
    lua_setfield(L, -2, "gid");

    lua_pushinteger(L, st.st_size);
    lua_setfield(L, -2, "size");

    return 1;
}

static int dir_iter(lua_State *L)
{
    DIR **d = (DIR **)lua_touserdata(L, lua_upvalueindex(1));
    struct dirent *e;

    if (!d)
        return 0;

    if ((e = readdir(*d))) {
        lua_pushstring(L, e->d_name);
        return 1;
    }

    closedir(*d);

    *d = NULL;

    return 0;
}

static int lua_dir(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);
    DIR **d = (DIR **)lua_newuserdata(L, sizeof(DIR *));

    luaL_getmetatable(L, DIR_MT_NAME);
    lua_setmetatable(L, -2);

    *d = opendir(path);

    lua_pushcclosure(L, dir_iter, 1);

    return 1;
}

static int lua_readlink(lua_State *L)
{
    const char *path = luaL_checkstring(L, 1);
    char buf[PATH_MAX] = "";
    ssize_t nbytes;

    nbytes = readlink(path, buf, PATH_MAX);
    if (nbytes < 0) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    lua_pushlstring(L, buf, nbytes);

    return 1;
}


static int dir_gc(lua_State *L)
{
    DIR *d = *(DIR **)lua_touserdata(L, 1);

    if (d)
        closedir(d);

    return 0;
}

static const luaL_Reg regs[] = {
    {"statvfs", lua_statvfs},
    {"access", lua_access},
    {"stat", lua_stat},
    {"dir", lua_dir},
    {"readlink", lua_readlink},
    {NULL, NULL}
};

int luaopen_oui_fs(lua_State *L)
{
    luaL_newmetatable(L, DIR_MT_NAME);
    lua_pushcfunction(L, dir_gc);
    lua_setfield(L, -2, "__gc");

    luaL_newlib(L, regs);

    return 1;
}

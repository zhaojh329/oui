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
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <lauxlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include "lua_compat.h"

static int file_is_executable(const char *name)
{
    struct stat s;
    return (!access(name, X_OK) && !stat(name, &s) && S_ISREG(s.st_mode));
}

static char *last_char_is(const char *s, int c)
{
    if (s && *s) {
        size_t sz = strlen(s) - 1;
        s += sz;
        if ((unsigned char) *s == c)
            return (char *) s;
    }
    return NULL;
}

static char *concat_path_file(const char *path, const char *filename)
{
    char *strp;
    char *lc;

    if (!path)
        path = "";
    lc = last_char_is(path, '/');
    while (*filename == '/')
        filename++;
    if (asprintf(&strp, "%s%s%s", path, (lc == NULL ? "/" : ""), filename) < 0)
        return NULL;
    return strp;
}

static char *find_executable(const char *filename, char **path)
{
    char *p, *n;

    p = *path;
    while (p) {
        int ex;

        n = strchr(p, ':');
        if (n) *n = '\0';
        p = concat_path_file(p[0] ? p : ".", filename);
        if (!p)
            break;
        ex = file_is_executable(p);
        if (n) *n++ = ':';
        if (ex) {
            *path = n;
            return p;
        }
        free(p);
        p = n;
    } /* on loop exit p == NULL */
    return p;
}

int which(const char *prog)
{
    char buf[] = "/sbin:/usr/sbin:/bin:/usr/bin";
    char *env_path;
    int missing = 1;

    env_path = getenv("PATH");
    if (!env_path)
        env_path = buf;

    /* If file contains a slash don't use PATH */
    if (strchr(prog, '/')) {
        if (file_is_executable(prog))
            missing = 0;
    } else {
        char *path;
        char *p;

        path = env_path;

        while ((p = find_executable(prog, &path)) != NULL) {
            missing = 0;
            free(p);
            break;
        }
    }
    return missing;
}


static int lua_md5sum(lua_State *L)
{
    const char *file = luaL_checkstring(L, 1);
    char md5[33] = "";
    uint8_t buf[16];
    int i;

    if (md5sum(file, buf) < 0) {
        lua_pushnil(L);
        return 1;
    }

    for (i = 0; i < 16; i++)
        sprintf(md5 + i * 2, "%02x", buf[i]);

    lua_pushstring(L, md5);

    return 1;
}

static int lua_md5(lua_State *L)
{
    int n = lua_gettop(L);
    md5_ctx_t ctx;
    char md5[33] = "";
    uint8_t buf[16];
    const char *s;
    size_t len;
    int i;

    md5_begin(&ctx);

    for (i = 1; i <= n; i++) {
        s = luaL_checklstring(L, i, &len);
        md5_hash(s, len, &ctx);
    }

    md5_end(buf, &ctx);

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

    /* total bytes */
    lua_pushinteger(L, s.f_blocks * s.f_frsize);

    /* available bytes */
    lua_pushinteger(L, s.f_bfree * s.f_frsize);

    /* used bytes */
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

static int lua_exists(lua_State *L)
{
    const char *file = luaL_checkstring(L, 1);

    if (access(file, F_OK))
        lua_pushboolean(L, false);
    else
        lua_pushboolean(L, true);

    return 1;
}

static void read_all(lua_State *L, int fd)
{
    luaL_Buffer b;
    ssize_t nr;

    luaL_buffinit(L, &b);

    while (true) {
        char *p = luaL_prepbuffer(&b);
        nr = read(fd, p, BUFSIZ);
        if (nr <= 0)
            break;
        luaL_addsize(&b, nr);
    }

    luaL_pushresult(&b);
}

static int lua_exec(lua_State *L)
{
    const char *cmd = luaL_checkstring(L, 1);
    int n = lua_gettop(L);
    int opipe[2] = {};
    int epipe[2] = {};
    pid_t pid;

    if (which(cmd)) {
        lua_pushnil(L);
        lua_pushstring(L, "Command not found");
        return 2;
    }

    if (pipe(opipe) < 0 || pipe(epipe) < 0)
        goto err;

    pid = fork();
    if (pid < 0) {
        goto err;
    } else if (pid == 0) {
        const char **args;
        int i, j;

        /* Close unused read end */
        close(opipe[0]);
        close(epipe[0]);

        /* Redirect */
        dup2(opipe[1], STDOUT_FILENO);
        dup2(epipe[1], STDERR_FILENO);
        close(opipe[1]);
        close(epipe[1]);

        args = malloc(sizeof(char *) * 2);

        args[0] = cmd;
        args[1] = NULL;

        j = 1;

        for (i = 2; i <= n; i++) {
            args = realloc(args, sizeof(char *) * (2 + j));
            args[j++] = lua_tostring(L, i);
            args[j] = NULL;
        }

        execvp(cmd, (char *const *) args);
    } else {
        time_t st = time(NULL);
        int wstatus;
        pid_t rc;

        /* Close unused write end */
        close(opipe[1]);
        close(epipe[1]);

wait:
        if (time(NULL) - st > 30) {
            kill(pid, SIGKILL);
            errno = ETIME;
            goto err;
        }

        rc = waitpid(pid, &wstatus, WNOHANG);
        if (rc < 0)
            goto err;

        if (rc == 0) {
            usleep(10);
            goto wait;
        }

        lua_pushinteger(L, WEXITSTATUS(wstatus));

        read_all(L, opipe[0]);
        read_all(L, epipe[0]);

        close(opipe[0]);
        close(epipe[0]);

        return 3;
    }

err:
    lua_pushnil(L);
    lua_pushstring(L, strerror(errno));

    if (opipe[0] > 0) {
        close(opipe[0]);
        close(opipe[1]);
    }

    if (epipe[0] > 0) {
        close(epipe[0]);
        close(epipe[1]);
    }

    return 2;
}

static int lua_sleep(lua_State *L)
{
    lua_Number delay = lua_tonumber(L, 1);
    struct timespec req;
    struct timespec rem;

    req.tv_sec = (time_t)delay;
    req.tv_nsec = (delay - req.tv_sec) * 1000000000;

    while (nanosleep(&req, &rem)) {
        if (errno != EINTR) {
            lua_pushnil(L);
            lua_pushstring(L, strerror(errno));

            return 2;
        }

        req.tv_sec = rem.tv_sec;
        req.tv_nsec = rem.tv_nsec;
    }

    lua_pushinteger(L, 0);

    return 1;
}

static const luaL_Reg regs[] = {
    {"md5sum",            lua_md5sum},
    {"md5",               lua_md5},
    {"statvfs",           lua_statvfs},
    {"parse_route_addr",  lua_parse_route_addr},
    {"parse_route6_addr", lua_parse_route6_addr},
    {"exists", lua_exists},
    {"exec", lua_exec},
    {"sleep", lua_sleep},
    {NULL, NULL}
};

int luaopen_oui_utils_utils(lua_State *L)
{
    luaL_newlib(L, regs);

    return 1;
}

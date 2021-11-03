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

#include "../md5.h"

struct exec_result {
    pid_t pid;
    int stdout_fd;
    int stderr_fd;
};

#define EXEC_RES_MT_NAME "oui-httpd-exec-result"

#define B64_ENCODE_LEN(_len)	((((_len) + 2) / 3) * 4 + 1)
#define B64_DECODE_LEN(_len)	(((_len) / 4) * 3 + 1)

int b64_encode(const void *src, size_t src_len, void *dest, size_t dest_len);
int b64_decode(const void *src, void *dest, size_t dest_len);


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

static int __sleep(lua_Number delay)
{
    struct timespec req;
    struct timespec rem;

    req.tv_sec = (time_t)delay;
    req.tv_nsec = (delay - req.tv_sec) * 1000000000;

    while (nanosleep(&req, &rem)) {
        if (errno != EINTR)
            return -1;

        req.tv_sec = rem.tv_sec;
        req.tv_nsec = rem.tv_nsec;
    }

    return 0;
}

static int lua_exec_wait(lua_State *L)
{
    struct exec_result *er = (struct exec_result *)luaL_checkudata(L, 1, EXEC_RES_MT_NAME);
    int delay = lua_tointeger(L, 2);
    time_t st = time(NULL);
    int wstatus;
    pid_t rc;

    if (delay <= 0)
        delay = 30;

wait:
    if (time(NULL) - st > delay) {
        kill(er->pid, SIGKILL);
        lua_pushnil(L);
        lua_pushstring(L, "timeout");
        return 2;
    }

    rc = waitpid(er->pid, &wstatus, WNOHANG);
    if (rc < 0) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    if (rc == 0) {
        usleep(10);
        goto wait;
    }

    lua_pushinteger(L, WEXITSTATUS(wstatus));

    read_all(L, er->stdout_fd);
    read_all(L, er->stderr_fd);

    return 3;
}

static int lua_exec_gc(lua_State *L)
{
    struct exec_result *er = (struct exec_result *)luaL_checkudata(L, 1, EXEC_RES_MT_NAME);

    if (!er)
        return 0;

    close(er->stdout_fd);
    close(er->stderr_fd);

    /* Libev will catch SIGCHLD and call waitpid  */

    return 0;
}

/**
 * local code, stdout, stderr = utils.exec("echo", "hello"):wait()
 *
 * -- delay 1.1s before execution
 * utils.exec({ cmd = "echo", delay = 1.1 }, "hello")
 */
static int lua_exec(lua_State *L)
{
    const char *cmd = NULL;
    struct exec_result *er;
    int n = lua_gettop(L);
    lua_Number delay = 0.0;
    int opipe[2] = {};
    int epipe[2] = {};
    pid_t pid;

    if (lua_istable(L, 1)) {
        lua_getfield(L, 1, "cmd");
        cmd = luaL_checkstring(L, -1);

        lua_getfield(L, 1, "delay");
        delay = lua_tonumber(L, -1);
    } else {
        cmd = luaL_checkstring(L, 1);
    }

    if (!cmd || which(cmd)) {
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
        if (!args)
            exit(1);

        args[0] = cmd;
        args[1] = NULL;

        j = 1;

        for (i = 2; i <= n; i++) {
            const char **tmp = realloc(args, sizeof(char *) * (2 + j));
            if (!tmp)
                exit(1);
            args = tmp;
            args[j++] = lua_tostring(L, i);
            args[j] = NULL;
        }

        __sleep(delay);

        execvp(cmd, (char *const *) args);

        free(args);
    } else {
        /* Close unused write end */
        close(opipe[1]);
        close(epipe[1]);

        er = (struct exec_result *)lua_newuserdata(L, sizeof(struct exec_result));
        er->pid = pid;
        er->stdout_fd = opipe[0];
        er->stderr_fd = epipe[0];

        luaL_getmetatable(L, EXEC_RES_MT_NAME);
        lua_setmetatable(L, -2);

        return 1;
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

    if (__sleep(delay)) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    lua_pushinteger(L, 0);

    return 1;
}

static int lua_b64_encode(lua_State *L)
{
    size_t src_len, dest_len;
    const char *src;
    char *dest;

    src = luaL_checklstring(L, 1, &src_len);
    if (!src) {
        lua_pushnil(L);
        return 1;
    }

    dest_len = B64_ENCODE_LEN(src_len);

    dest = malloc(dest_len);
    if (!dest) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    if (b64_encode(src, src_len, dest, dest_len) < 0)
        lua_pushnil(L);
    else
        lua_pushstring(L, dest);

    return 1;
}

static int lua_b64_decode(lua_State *L)
{
    size_t src_len, dest_len;
    const char *src;
    char *dest;
    int ret;

    src = luaL_checklstring(L, 1, &src_len);
    if (!src) {
        lua_pushnil(L);
        return 1;
    }

    dest_len = B64_DECODE_LEN(src_len);

    dest = malloc(dest_len);
    if (!dest) {
        lua_pushnil(L);
        lua_pushstring(L, strerror(errno));
        return 2;
    }

    ret = b64_decode(src, dest, dest_len);
    if (ret < 0)
        lua_pushnil(L);
    else
        lua_pushlstring(L, dest, ret);

    return 1;
}

static const luaL_Reg regs[] = {
    {"md5sum", lua_md5sum},
    {"md5", lua_md5},
    {"exec", lua_exec},
    {"sleep", lua_sleep},
    {"b64encode", lua_b64_encode},
    {"b64decode", lua_b64_decode},
    {NULL, NULL}
};

int luaopen_oui_utils_utils(lua_State *L)
{
    luaL_newmetatable(L, EXEC_RES_MT_NAME);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, lua_exec_wait);
    lua_setfield(L, -2, "wait");

    lua_pushcfunction(L, lua_exec_gc);
    lua_setfield(L, -2, "__gc");

    luaL_newlib(L, regs);

    return 1;
}

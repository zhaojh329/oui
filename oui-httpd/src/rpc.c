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

#include <libubox/avl.h>
#include <libubox/avl-cmp.h>
#include <lauxlib.h>
#include <dirent.h>
#include <lualib.h>
#include <errno.h>
#include <time.h>

#include "lua_json.h"
#include "lua_utils.h"
#include "session.h"
#include "utils.h"
#include "rpc.h"

enum {
    RPC_ERROR_PARSE,
    RPC_ERROR_REQUEST,
    RPC_ERROR_METHOD,
    RPC_ERROR_PARAMS,
    RPC_ERROR_INTERNAL,
    RPC_ERROR_ACCESS,
    RPC_ERROR_EXEC_TIMEOUT,
    RPC_ERROR_EXEC_NOTFOUND,
    RPC_ERROR_CALL_OBJECT,
    RPC_ERROR_CALL_METHOD
};

static const struct {
    int code;
    const char *msg;
} rpc_errors[] = {
    [RPC_ERROR_PARSE] = {-32700, "Parse error"},
    [RPC_ERROR_REQUEST] = {-32600, "Invalid request"},
    [RPC_ERROR_METHOD] = {-32601, "Method not found"},
    [RPC_ERROR_PARAMS] = {-32602, "Invalid parameters"},
    [RPC_ERROR_INTERNAL] = {-32603, "Internal error"},
    [RPC_ERROR_ACCESS] = {-32000, "Access denied"},
    [RPC_ERROR_EXEC_TIMEOUT] = {-32001, "exec: timed out"},
    [RPC_ERROR_EXEC_NOTFOUND] = {-32002, "exec: not found"},
    [RPC_ERROR_CALL_OBJECT] = {-32003, "call: Object not found"},
    [RPC_ERROR_CALL_METHOD] = {-32004, "call: Method not found"}
};

struct rpc_exec_context {
    struct uh_connection *conn;
    struct ev_timer tmr;
    struct ev_child child;
    int stdout_fd;
    int stderr_fd;
    json_t *req;
    pid_t pid;
};

struct rpc_object {
    struct avl_node avl;
    lua_State *L;
    char value[0];
};

static struct avl_tree rpc_objects;

static void rpc_set_id(json_t *req, json_t *ret)
{
    json_t *id;

    if (!req)
        goto null_id;

    id = json_object_get(req, "id");
    if (json_is_string(id) || json_is_integer(id)) {
        json_object_set(ret, "id", id);
        return;
    }

null_id:
    json_object_set_new(ret, "id", json_null());
}

static void rpc_error(struct uh_connection *conn, int type, json_t *req)
{
    json_t *ret = json_object();
    json_t *err = json_object();
    char buf[512];
    size_t size;

    json_object_set_new(ret, "jsonrpc", json_string("2.0"));

    rpc_set_id(req, ret);

    json_object_set_new(err, "code", json_integer(rpc_errors[type].code));
    json_object_set_new(err, "message", json_string(rpc_errors[type].msg));
    json_object_set_new(ret, "error", err);

    size = json_dumpb(ret, buf, sizeof(buf), 0);

    conn->send_head(conn, HTTP_STATUS_OK, size, "Content-Type: application/json\r\n");
    conn->send(conn, buf, size);

    json_decref(ret);
    json_decref(req);

    conn->done(conn);
}

static bool parse_rpc(json_t *req, const char **method, json_t **params)
{
    const char *jsonrpc;
    json_t *json;

    jsonrpc = json_object_get_string(req, "jsonrpc");
    if (!jsonrpc || strcmp(jsonrpc, "2.0"))
        return false;

    *method = json_object_get_string(req, "method");
    if (!*method)
        return false;

    json = json_object_get(req, "id");
    if (!json_is_string(json) && !json_is_integer(json))
        return false;

    *params = json_object_get(req, "params");
    if (*params) {
        if (!json_is_array(*params) && !json_is_object(*params))
            return false;
    }

    return true;
}

static void rpc_resp(struct uh_connection *conn, json_t *req, json_t *result)
{
    json_t *root = json_object();
    size_t size;
    char *s;

    json_object_set_new(root, "jsonrpc", json_string("2.0"));

    rpc_set_id(req, root);

    json_object_set_new(root, "result", result);

    s = json_dumps(root, 0);
    size = strlen(s);

    conn->send_head(conn, HTTP_STATUS_OK, size, "Content-Type: application/json\r\n");
    conn->send(conn, s, size);

    free(s);

    json_decref(req);
    json_decref(root);

    conn->done(conn);
}

static void handle_rpc_call(struct uh_connection *conn, const char *sid, const char *object, const char *method,
                            json_t *args, json_t *req)
{
    struct rpc_object *obj;
    lua_State *L;

    obj = avl_find_element(&rpc_objects, object, obj, avl);
    if (!obj) {
        rpc_error(conn, RPC_ERROR_CALL_OBJECT, req);
        return;
    }

    L = obj->L;

    if (!lua_istable(L, -1)) {
        uh_log_err("lua state is broken. No table on stack!\n");
        rpc_error(conn, RPC_ERROR_INTERNAL, req);
        return;
    }

    lua_getfield(L, -1, method);
    if (!lua_isfunction(L, -1)) {
        rpc_error(conn, RPC_ERROR_CALL_METHOD, req);
        return;
    }

    if (!rpc_session_allowed(sid, object, method)) {
        rpc_error(conn, RPC_ERROR_ACCESS, req);
        return;
    }

    if (args)
        json_to_lua(args, L);
    else
        lua_newtable(L);

    if (lua_pcall(L, 1, 1, 0)) {
        uh_log_err("%s\n", lua_tostring(L, -1));
        rpc_error(conn, RPC_ERROR_INTERNAL, req);
        return;
    }

    rpc_resp(conn, req, lua_to_json(L));

    lua_pop(L, 1);
}

static void rpc_exec_timeout_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
    struct rpc_exec_context *ctx = container_of(w, struct rpc_exec_context, tmr);

    ev_child_stop(loop, &ctx->child);

    kill(ctx->pid, SIGKILL);

    close(ctx->stdout_fd);
    close(ctx->stderr_fd);

    rpc_error(ctx->conn, RPC_ERROR_EXEC_TIMEOUT, ctx->req);

    free(ctx);
}

static void rpc_exec_read(int fd, json_t *res, const char *name)
{
    struct buffer b = {};
    uint8_t buf[1024];

    for (;;) {
        ssize_t nr = read(fd, buf, sizeof(buf));
        if (nr <= 0)
            break;
        buffer_put_data(&b, buf, nr);
    }

    json_object_set_new(res, name, json_stringn(buffer_data(&b), buffer_length(&b)));

    buffer_free(&b);

    close(fd);
}

static void rpc_exec_child_exit(struct ev_loop *loop, struct ev_child *w, int revents)
{
    struct rpc_exec_context *ctx = container_of(w, struct rpc_exec_context, child);
    json_t *res = json_object();

    ev_timer_stop(loop, &ctx->tmr);

    json_object_set_new(res, "code", json_integer(WEXITSTATUS(w->rstatus)));

    rpc_exec_read(ctx->stdout_fd, res, "stdout");
    rpc_exec_read(ctx->stderr_fd, res, "stderr");

    rpc_resp(ctx->conn, ctx->req, res);

    /* Fix Segmentation fault */
    ev_child_stop(loop, w);

    free(ctx);
}

static void handle_rpc_exec(struct uh_connection *conn, json_t *params, json_t *req)
{
    int opipe[2] = {};
    int epipe[2] = {};
    const char *cmd;
    pid_t pid;

    cmd = json_array_get_string(params, 0);
    if (which(cmd)) {
        rpc_error(conn, RPC_ERROR_EXEC_NOTFOUND, req);
        return;
    }

    if (pipe(opipe) < 0 || pipe(epipe) < 0)
        goto err;

    pid = fork();
    if (pid < 0) {
        goto err;
    } else if (pid == 0) {
        size_t n = json_array_size(params) - 1;
        const char **args;
        int i;

        /* Close unused read end */
        close(opipe[0]);
        close(epipe[0]);

        /* Redirect */
        dup2(opipe[1], STDOUT_FILENO);
        dup2(epipe[1], STDERR_FILENO);
        close(opipe[1]);
        close(epipe[1]);

        args = calloc(1, sizeof(char *) * (n + 2));
        if (!args)
            exit(1);

        args[0] = cmd;

        for (i = 0; i < n; i++)
            args[i + 1] = json_array_get_string(params, i + 1);

        execvp(cmd, (char *const *) args);
    } else {
        struct rpc_exec_context *ctx = calloc(1, sizeof(struct rpc_exec_context));
        struct ev_loop *loop = conn->srv->loop;

        /* Close unused write end */
        close(opipe[1]);
        close(epipe[1]);

        ctx->conn = conn;
        ctx->req = req;
        ctx->pid = pid;

        ctx->stdout_fd = opipe[0];
        ctx->stderr_fd = epipe[0];

        ev_timer_init(&ctx->tmr, rpc_exec_timeout_cb, 30, 0);
        ev_timer_start(loop, &ctx->tmr);

        ev_child_init(&ctx->child, rpc_exec_child_exit, pid, 0);
        ev_child_start(loop, &ctx->child);
    }

    return;

err:
    if (opipe[0] > 0) {
        close(opipe[0]);
        close(opipe[1]);
    }

    if (epipe[0] > 0) {
        close(epipe[0]);
        close(epipe[1]);
    }

    uh_log_err("rpc exec: %s\n", strerror(errno));

    rpc_error(conn, RPC_ERROR_INTERNAL, req);
}

static void handle_rpc(struct uh_connection *conn, const char *method, json_t *params, json_t *req)
{
    if (!strcmp(method, "login")) {
        const char *password, *sid;
        json_t *result;

        if (!params || json_typeof(params) != JSON_OBJECT) {
            rpc_error(conn, RPC_ERROR_PARAMS, req);
            return;
        }

        password = json_object_get_string(params, "password");

        sid = rpc_login(password);
        if (!sid) {
            rpc_error(conn, RPC_ERROR_ACCESS, req);
            return;
        }

        result = json_object();
        json_object_set_new(result, "sid", json_string(sid));
        rpc_resp(conn, req, result);
    } else if (!strcmp(method, "logout")) {
        const char *sid;

        if (!params || json_typeof(params) != JSON_OBJECT) {
            rpc_error(conn, RPC_ERROR_PARAMS, req);
            return;
        }

        sid = json_object_get_string(params, "sid");
        rpc_logout(sid);

        rpc_resp(conn, req, json_object());
    } else if (!strcmp(method, "alive")) {
        const char *sid;

        if (!params || json_typeof(params) != JSON_OBJECT) {
            rpc_error(conn, RPC_ERROR_PARAMS, req);
            return;
        }

        sid = json_object_get_string(params, "sid");
        if (!rpc_session_get(sid)) {
            rpc_error(conn, RPC_ERROR_ACCESS, req);
            return;
        }

        rpc_resp(conn, req, json_object());
    } else if (!strcmp(method, "call")) {
        const char *sid, *object;
        json_t *json, *args = NULL;
        size_t size;

        if (!params || json_typeof(params) != JSON_ARRAY) {
            rpc_error(conn, RPC_ERROR_PARAMS, req);
            return;
        }

        size = json_array_size(params);
        if (size != 3 && size != 4) {
            rpc_error(conn, RPC_ERROR_PARAMS, req);
            return;
        }

        sid = json_array_get_string(params, 0);
        if (!sid) {
            rpc_error(conn, RPC_ERROR_PARAMS, req);
            return;
        }

        object = json_array_get_string(params, 1);
        if (!object) {
            rpc_error(conn, RPC_ERROR_PARAMS, req);
            return;
        }

        method = json_array_get_string(params, 2);
        if (!method) {
            rpc_error(conn, RPC_ERROR_PARAMS, req);
            return;
        }

        if (size == 4) {
            json = json_array_get(params, 3);
            if (!json_is_object(json)) {
                rpc_error(conn, RPC_ERROR_PARAMS, req);
                return;
            }
            args = json;
        }

        handle_rpc_call(conn, sid, object, method, args, req);
    } else if (!strcmp(method, "exec")) {
        const char *sid;
        size_t size;

        if (!params || json_typeof(params) != JSON_ARRAY) {
            rpc_error(conn, RPC_ERROR_PARAMS, req);
            return;
        }

        size = json_array_size(params);
        if (size < 2) {
            rpc_error(conn, RPC_ERROR_PARAMS, req);
            return;
        }

        sid = json_array_get_string(params, 0);
        if (!sid) {
            rpc_error(conn, RPC_ERROR_PARAMS, req);
            return;
        }

        if (!rpc_session_get(sid)) {
            rpc_error(conn, RPC_ERROR_ACCESS, req);
            return;
        }

        json_array_remove(params, 0);

        handle_rpc_exec(conn, params, req);
    } else {
        rpc_error(conn, RPC_ERROR_METHOD, req);
    }
}

void serve_rpc(struct uh_connection *conn)
{
    struct uh_str body;
    json_t *root;
    json_error_t error;
    json_t *params;
    const char *method;

    if (conn->get_method(conn) != HTTP_POST) {
        conn->error(conn, HTTP_STATUS_METHOD_NOT_ALLOWED, NULL);
        return;
    }

    body = conn->get_body(conn);
    root = json_loadb(body.p, body.len, 0, &error);

    if (!root) {
        rpc_error(conn, RPC_ERROR_PARSE, NULL);
        return;
    }

    switch (json_typeof(root)) {
    case JSON_OBJECT:
        if (!parse_rpc(root, &method, &params)) {
            rpc_error(conn, RPC_ERROR_PARSE, root);
            return;
        }
        handle_rpc(conn, method, params, root);
        break;
    default:
        rpc_error(conn, RPC_ERROR_PARSE, root);
        return;
    }
}

void load_rpc(const char *path)
{
    DIR *dir;
    struct dirent *e;

    avl_init(&rpc_objects, avl_strcmp, false, NULL);

    dir = opendir(path);
    if (!dir) {
        uh_log_err("opendir fail: %s\n", strerror(errno));
        return;
    }

    while ((e = readdir(dir))) {
        char object_path[512] = "";
        struct rpc_object *obj;
        lua_State *L;

        if (e->d_type != DT_REG || e->d_name[0] == '.')
            continue;

        L = luaL_newstate();

        luaL_openlibs(L);
        luaopen_json(L);
        luaopen_utils(L);

        snprintf(object_path, sizeof(object_path) - 1, "%s/%s", path, e->d_name);

        if (luaL_dofile(L, object_path)) {
            uh_log_err("load rpc: %s\n", lua_tostring(L, -1));
            lua_close(L);
            continue;
        }

        if (!lua_istable(L, -1)) {
            uh_log_err("invalid rpc script, need return a table: %s\n", object_path);
            lua_close(L);
            continue;
        }

        obj = calloc(1, sizeof(struct rpc_object) + strlen(e->d_name) + 1);
        obj->L = L;
        strcpy(obj->value, e->d_name);
        obj->avl.key = obj->value;
        avl_insert(&rpc_objects, &obj->avl);
    }

    closedir(dir);
}

void unload_rpc()
{
    struct rpc_object *obj, *temp;

    avl_for_each_element_safe(&rpc_objects, obj, avl, temp) {
        avl_delete(&rpc_objects, &obj->avl);
        lua_close(obj->L);
        free(obj);
    }
}


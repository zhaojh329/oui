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

#include <uhttpd/uhttpd.h>
#include <uhttpd/buffer.h>
#include <libubox/avl.h>
#include <libubox/avl-cmp.h>
#include <lauxlib.h>
#include <sqlite3.h>
#include <dirent.h>
#include <assert.h>
#include <lualib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <uci.h>

#include "lua2json.h"
#include "session.h"
#include "utils.h"
#include "rpc.h"
#include "db.h"

static const struct {
    int code;
    const char *msg;
} rpc_errors[] = {
    [ERROR_CODE_PARSE_ERROR] = {-32700, "Parse error"},
    [ERROR_CODE_INVALID_REQUEST] = {-32600, "Invalid Request"},
    [ERROR_CODE_METHOD_NOT_FOUND] = {-32601, "Method not found"},
    [ERROR_CODE_INVALID_PARAMS] = {-32602, "Invalid params"},
    [ERROR_CODE_INTERNAL_ERROR] = {-32603, "Internal error"},
    [ERROR_CODE_ACCESS] = {-32000, "Access denied"},
    [ERROR_CODE_NOT_FOUND] = {-32001, "Not found"},
    [ERROR_CODE_TIMEOUT] = {-32002, "Timeout"}
};

struct rpc_exec_context {
    struct uh_connection *conn;
    struct ev_timer tmr;
    struct ev_child child;
    int stdout_fd;
    int stderr_fd;
    json_t *id;
    pid_t pid;
};

struct rpc_object {
    struct avl_tree trusted_methods;
    struct avl_node avl;
    lua_State *L;
    char value[0];
};

struct rpc_trusted_method {
    struct avl_node avl;
    char value[0];
};

static struct avl_tree rpc_objects;

static json_t *rpc_error_object(int code, const char *message, json_t *data)
{
    json_t *json = json_pack("{s:i,s:s}", "code", code, "message", message ? message : "");

    if (data)
        json_object_set_new(json, "data", data);

    return json;
}

static json_t *rpc_error_object_predefined(int code, json_t *data)
{
    if (code > -1 && code < __ERROR_CODE_MAX)
        return rpc_error_object(rpc_errors[code].code, rpc_errors[code].msg, data);
    else
        return rpc_error_object(code, NULL, data);
}

static json_t *rpc_error_response(json_t *id, json_t *error)
{
    if (id)
        json_incref(id);
    else
        id = json_null();

    error = error ? error : json_null();

    return json_pack("{s:s,s:o,s:o}", "jsonrpc", "2.0", "id", id, "error", error);
}

static json_t *rpc_result_response(json_t *id, json_t *result)
{
    if (id)
        json_incref(id);
    else
        id = json_null();

    result = result ? result : json_null();

    return json_pack("{s:s,s:o,s:o}", "jsonrpc", "2.0", "id", id, "result", result);
}

static json_t *rpc_validate_request(json_t *req, const char **method, json_t **params, json_t **id)
{
    const char *version = NULL;
    json_t *data = NULL;
    json_error_t error;
    int rc;

    *method = NULL;
    *params = NULL;
    *id = NULL;

    rc = json_unpack_ex(req, &error, 0, "{s:s,s:o,s:s,s?o}",
                        "jsonrpc", &version,
                        "id", id,
                        "method", method,
                        "params", params);
    if (rc < 0) {
        data = json_string(error.text);
        goto invalid;
    }

    if (strcmp(version, "2.0")) {
        data = json_string("\"jsonrpc\" MUST be exactly \"2.0\"");
        goto invalid;
    }

    if (!json_is_string(*id) && !json_is_number(*id) && !json_is_null(*id)) {
        data = json_string("\"id\" MUST be a String, Number, or NULL value");
        goto invalid;
    }

    if (*params) {
        if (!json_is_array(*params) && !json_is_object(*params)) {
            data = json_string("\"params\" MUST be Array or Object if included");
            goto invalid;
        }
    }

    return NULL;

invalid:
    return rpc_error_response(*id, rpc_error_object_predefined(ERROR_CODE_INVALID_REQUEST, data));
}

static void rpc_handle_done_final(struct uh_connection *conn, json_t *resp)
{
    size_t size;
    char *s;

    if (!resp) {
        conn->error(conn, HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL);
        return;
    }

    s = json_dumps(resp, 0);
    if (!s) {
        conn->error(conn, HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL);
        return;
    }

    json_decref(resp);

    size = strlen(s);

    conn->send_head(conn, HTTP_STATUS_OK, size, "Content-Type: application/json\r\n");
    conn->send(conn, s, size);
    conn->done(conn);

    free(s);
}

static void rpc_handle_done_deferred(struct uh_connection *conn, json_t *id, json_t *result, bool is_error)
{
    json_t *resp;

    if (is_error)
        resp = rpc_error_response(id, result);
    else
        resp = rpc_result_response(id, result);

    json_decref(id);

    rpc_handle_done_final(conn, resp);
}

static bool rpc_is_trusted(struct rpc_object *obj, const char *method)
{
    struct rpc_trusted_method *m;

    return avl_find_element(&obj->trusted_methods, method, m, avl);
}

static int rpc_access_cb(void *data, int count, char **value, char **name)
{
    if (value[0])
        strncpy(data, value[0], 4);
    return SQLITE_ABORT;
}

static bool rpc_call_access(struct session *s, const char *object, const char *method)
{
    char perm[5] = "";
    char sql[128];

    /* The admin acl group is always allowed */
    if (!strcmp(s->aclgroup, "admin"))
        return true;

    sprintf(sql, "SELECT permissions FROM acl_%s WHERE scope = 'rpc' AND entry = '%s.%s'",
            s->aclgroup, object, method);

    if (db_query(sql, rpc_access_cb, perm) < 0)
        return false;

    return strchr(perm, 'x');
}

static bool rpc_exec_access(struct session *s, const char *cmd)
{
    char perm[5] = "";
    char sql[128];

    /* The admin acl group is always allowed */
    if (!strcmp(s->aclgroup, "admin"))
        return true;

    sprintf(sql, "SELECT permissions FROM acl_%s WHERE scope = 'exec' AND entry = '*'", s->aclgroup);

    if (db_query(sql, rpc_access_cb, perm) < 0) {
        sprintf(sql, "SELECT permissions FROM acl_%s WHERE scope = 'exec' AND entry = '%s'", s->aclgroup, cmd);
        if (db_query(sql, rpc_access_cb, perm) < 0)
            return false;
    }

    return strchr(perm, 'x');
}

static int rpc_method_login(struct uh_connection *conn, json_t *id, json_t *params, json_t **result)
{
    const char *username, *password = "", *sid;
    json_error_t error;

    if (json_unpack_ex(params, &error, 0, "{s:s,s?s}", "username", &username, "password", &password) < 0) {
        *result = rpc_error_object_predefined(ERROR_CODE_INVALID_PARAMS, json_string(error.text));
        return RPC_METHOD_RETURN_ERROR;
    }

    sid = session_login(username, password);
    if (!sid) {
        *result = rpc_error_object_predefined(ERROR_CODE_ACCESS, NULL);
        return RPC_METHOD_RETURN_ERROR;
    }

    *result = json_pack("{s:s, s:s}", "sid", sid, "username", username);

    return RPC_METHOD_RETURN_SUCCESS;
}

static int rpc_method_logout(struct uh_connection *conn, json_t *id, json_t *params, json_t **result)
{
    json_error_t error;
    const char *sid;

    if (json_unpack_ex(params, &error, 0, "{s:s}", "sid", &sid) < 0) {
        *result = rpc_error_object_predefined(ERROR_CODE_INVALID_PARAMS, json_string(error.text));
        return RPC_METHOD_RETURN_ERROR;
    }

    session_logout(sid);

    return RPC_METHOD_RETURN_SUCCESS;
}

static int rpc_method_alive(struct uh_connection *conn, json_t *id, json_t *params, json_t **result)
{
    json_error_t error;
    const char *sid;

    if (json_unpack_ex(params, &error, 0, "{s:s}", "sid", &sid) < 0) {
        *result = rpc_error_object_predefined(ERROR_CODE_INVALID_PARAMS, json_string(error.text));
        return RPC_METHOD_RETURN_ERROR;
    }

    if (!session_get(sid)) {
        *result = rpc_error_object_predefined(ERROR_CODE_ACCESS, NULL);
        return RPC_METHOD_RETURN_ERROR;
    }

    return RPC_METHOD_RETURN_SUCCESS;
}

static void rpc_exec_timeout_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
    struct rpc_exec_context *ctx = container_of(w, struct rpc_exec_context, tmr);
    json_t *res;

    ev_child_stop(loop, &ctx->child);

    kill(ctx->pid, SIGKILL);

    close(ctx->stdout_fd);
    close(ctx->stderr_fd);

    res = rpc_error_object_predefined(ERROR_CODE_TIMEOUT, NULL);
    rpc_handle_done_deferred(ctx->conn, ctx->id, res, true);

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

    if (buffer_length(&b) == 0)
        json_object_set_new(res, name, json_string(""));
    else
        json_object_set_new(res, name, json_stringn(buffer_data(&b), buffer_length(&b)));

    buffer_free(&b);

    close(fd);
}

static void rpc_exec_child_exit(struct ev_loop *loop, struct ev_child *w, int revents)
{
    struct rpc_exec_context *ctx = container_of(w, struct rpc_exec_context, child);
    json_t *res;

    /* Avoid conflicts caused by two same pid */
    ev_child_stop(loop, w);

    ev_timer_stop(loop, &ctx->tmr);

    res = json_pack("{s:i}", "code", WEXITSTATUS(w->rstatus));

    rpc_exec_read(ctx->stdout_fd, res, "stdout");
    rpc_exec_read(ctx->stderr_fd, res, "stderr");

    rpc_handle_done_deferred(ctx->conn, ctx->id, res, false);

    free(ctx);
}

static int rpc_method_exec(struct uh_connection *conn, json_t *id, json_t *params, json_t **result)
{
    bool is_local = is_loopback_addr(conn->get_addr(conn));
    const char *sid, *cmd;
    struct session *s;
    json_error_t error;
    int opipe[2] = {};
    int epipe[2] = {};
    pid_t pid;

    if (json_unpack_ex(params, &error, 0, "[ss]", &sid, &cmd) < 0) {
        *result = rpc_error_object_predefined(ERROR_CODE_INVALID_PARAMS, json_string(error.text));
        return RPC_METHOD_RETURN_ERROR;
    }

    s = session_get(sid);

    if (!is_local && (!s || !rpc_exec_access(s, cmd))) {
        *result = rpc_error_object_predefined(ERROR_CODE_ACCESS, NULL);
        return RPC_METHOD_RETURN_ERROR;
    }

    if (which(cmd)) {
        *result = rpc_error_object_predefined(ERROR_CODE_NOT_FOUND, json_string("Command not found"));
        return RPC_METHOD_RETURN_ERROR;
    }

    if (pipe(opipe) < 0 || pipe(epipe) < 0)
        goto err;

    pid = fork();
    if (pid < 0) {
        goto err;
    } else if (pid == 0) {
        const char **args;
        json_t *p;
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

        json_array_foreach(params, i, p) {
            if (i < 2 || !json_is_string(p))
                continue;
            args = realloc(args, sizeof(char *) * (2 + j));
            args[j++] = json_string_value(p);
            args[j] = NULL;
        }

        execvp(cmd, (char *const *) args);
    } else {
        struct rpc_exec_context *ctx = calloc(1, sizeof(struct rpc_exec_context));
        struct ev_loop *loop = conn->get_loop(conn);

        /* Close unused write end */
        close(opipe[1]);
        close(epipe[1]);

        ctx->conn = conn;
        ctx->pid = pid;
        ctx->id = id;

        ctx->stdout_fd = opipe[0];
        ctx->stderr_fd = epipe[0];

        ev_timer_init(&ctx->tmr, rpc_exec_timeout_cb, 30, 0);
        ev_timer_start(loop, &ctx->tmr);

        ev_child_init(&ctx->child, rpc_exec_child_exit, pid, 0);
        ev_child_start(loop, &ctx->child);
    }

    return RPC_METHOD_RETURN_DEFERRED;

err:
    if (opipe[0] > 0) {
        close(opipe[0]);
        close(opipe[1]);
    }

    if (epipe[0] > 0) {
        close(epipe[0]);
        close(epipe[1]);
    }

    *result = rpc_error_object_predefined(ERROR_CODE_INTERNAL_ERROR, json_string(strerror(errno)));
    return RPC_METHOD_RETURN_ERROR;
}

static int rpc_method_call(struct uh_connection *conn, json_t *id, json_t *params, json_t **result)
{
    bool is_local = is_loopback_addr(conn->get_addr(conn));
    int ret = RPC_METHOD_RETURN_ERROR;
    const char *sid, *object, *method;
    struct rpc_object *obj;
    json_error_t error;
    struct session *s;
    json_t *args;
    lua_State *L;

    if (json_unpack_ex(params, &error, 0, "[ssso!]", &sid, &object, &method, &args) < 0) {
        *result = rpc_error_object_predefined(ERROR_CODE_INVALID_PARAMS, json_string(error.text));
        return RPC_METHOD_RETURN_ERROR;
    }

    if (!json_is_object(args)) {
        *result = rpc_error_object_predefined(ERROR_CODE_INVALID_PARAMS,
                                              json_string("The argument must be an object"));
        return RPC_METHOD_RETURN_ERROR;
    }

    obj = avl_find_element(&rpc_objects, object, obj, avl);
    if (!obj) {
        *result = rpc_error_object_predefined(ERROR_CODE_NOT_FOUND, json_string("Object not found"));
        return RPC_METHOD_RETURN_ERROR;
    }

    L = obj->L;

    if (!lua_istable(L, -1)) {
        uh_log_err("%s.%s: lua state is broken. No table on stack!\n", object, method);
        *result = rpc_error_object_predefined(ERROR_CODE_INTERNAL_ERROR, NULL);
        return RPC_METHOD_RETURN_ERROR;
    }

    lua_getfield(L, -1, method);
    if (!lua_isfunction(L, -1)) {
        *result = rpc_error_object_predefined(ERROR_CODE_NOT_FOUND, json_string("Method not found"));
        goto done;
    }

    s = session_get(sid);

    if (!is_local && !rpc_is_trusted(obj, method) && (!s || !rpc_call_access(s, object, method))) {
        *result = rpc_error_object_predefined(ERROR_CODE_ACCESS, NULL);
        goto done;
    }

    lua_newtable(L);

    if (s) {
        lua_pushstring(L, s->username);
        lua_setfield(L, -2, "username");

        lua_pushstring(L, s->aclgroup);
        lua_setfield(L, -2, "aclgroup");
    }

    lua_pushboolean(L, is_local);
    lua_setfield(L, -2, "is_local");

    lua_setglobal(L, "__oui_session");

    if (args)
        json_to_lua(args, L);
    else
        lua_newtable(L);

    if (lua_pcall(L, 1, 2, 0)) {
        const char *err_msg = lua_tostring(L, -1);
        json_t *data = json_string(err_msg);

        uh_log_err("%s\n", err_msg);
        *result = rpc_error_object_predefined(ERROR_CODE_INTERNAL_ERROR, data);
        goto done;
    }

    /*
     * Return a number on failure (plus an error message as a second result)
     * Return a table on success
     * Return other type also as success, but the result will be ignored
     */
    if (lua_isnumber(L, -2)) {
        json_t *data = NULL;

        if (lua_isstring(L, -1))
            data = json_string(lua_tostring(L, -1));
        *result = rpc_error_object_predefined(lua_tointeger(L, -2), data);
    } else {
        if (lua_istable(L, -2))
            *result = lua_to_json(L, -2, false);
        ret = RPC_METHOD_RETURN_SUCCESS;
    }

    lua_pop(L, 1);

done:
    lua_pop(L, 1);
    return ret;
}

static struct rpc_method_entry methods[] = {
    {"login",  rpc_method_login},
    {"logout", rpc_method_logout},
    {"alive",  rpc_method_alive},
    {"exec",   rpc_method_exec},
    {"call",   rpc_method_call},
    {}
};

static void rpc_handle_request(struct uh_connection *conn, json_t *req)
{
    json_t *params, *id, *resp, *result = NULL;
    struct rpc_method_entry *entry;
    const char *method;

    resp = rpc_validate_request(req, &method, &params, &id);
    if (resp)
        goto done;

    for (entry = methods; entry->name != NULL; entry++) {
        if (!strcmp(entry->name, method))
            break;
    }

    if (!entry->name) {
        resp = rpc_error_response(id, rpc_error_object_predefined(ERROR_CODE_METHOD_NOT_FOUND, NULL));
        goto done;
    }

    switch (entry->handler(conn, id, params, &result)) {
    case RPC_METHOD_RETURN_ERROR:
        resp = rpc_error_response(id, result);
        break;
    case RPC_METHOD_RETURN_SUCCESS:
        resp = rpc_result_response(id, result);
        break;
    case RPC_METHOD_RETURN_DEFERRED:
        json_incref(id);
        return;
    default:
        resp = rpc_error_response(id, rpc_error_object_predefined(ERROR_CODE_INTERNAL_ERROR, NULL));
        break;
    }

done:
    rpc_handle_done_final(conn, resp);
}

void serve_rpc(struct uh_connection *conn, int event)
{
    json_t *req, *resp = NULL;
    struct uh_str body;
    json_error_t error;

    if (event != UH_EV_COMPLETE)
        return;

    if (conn->get_method(conn) != HTTP_POST) {
        conn->error(conn, HTTP_STATUS_METHOD_NOT_ALLOWED, NULL);
        return;
    }

    body = conn->get_body(conn);
    req = json_loadb(body.p, body.len, 0, &error);
    if (!req) {
        resp = rpc_error_response(NULL, rpc_error_object_predefined(ERROR_CODE_PARSE_ERROR, NULL));
        goto err;
    }

    if (json_is_array(req)) {
        resp = rpc_error_response(NULL, rpc_error_object_predefined(ERROR_CODE_INVALID_REQUEST,
                                                                    json_string("Not support batch")));
        goto err;
    }

    rpc_handle_request(conn, req);

err:
    if (resp)
        rpc_handle_done_final(conn, resp);
    json_decref(req);
}

static void load_rpc_scripts(const char *path)
{
    DIR *dir;
    struct dirent *e;

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
        if (!obj) {
            uh_log_err("calloc: %s\n", strerror(errno));
            break;
        }

        obj->L = L;
        strcpy(obj->value, e->d_name);
        obj->avl.key = obj->value;
        avl_init(&obj->trusted_methods, avl_strcmp, false, NULL);
        avl_insert(&rpc_objects, &obj->avl);
    }

    closedir(dir);
}

static void free_all_trusted_methods(struct rpc_object *obj)
{
    struct rpc_trusted_method *m, *temp;

    avl_for_each_element_safe(&obj->trusted_methods, m, avl, temp) {
        avl_delete(&obj->trusted_methods, &m->avl);
        free(m);
    }
}

static void unload_rpc_scripts()
{
    struct rpc_object *obj, *temp;

    avl_for_each_element_safe(&rpc_objects, obj, avl, temp) {
        avl_delete(&rpc_objects, &obj->avl);
        free_all_trusted_methods(obj);
        lua_close(obj->L);
        free(obj);
    }
}

static int add_trusted_method(struct rpc_object *obj, const char *value)
{
    struct rpc_trusted_method *m = calloc(1, sizeof(struct rpc_trusted_method) + strlen(value) + 1);

    if (!m) {
        uh_log_err("calloc: %s\n", strerror(errno));
        return -1;
    }

    strcpy(m->value, value);
    m->avl.key = m->value;

    avl_insert(&obj->trusted_methods, &m->avl);

    return 0;
}

static int load_trusted()
{
    struct uci_context *uci = uci_alloc_context();
    struct uci_package *p = NULL;
    struct uci_section *s;
    struct uci_element *e;
    struct uci_ptr ptr = {.package = "oui-httpd"};
    int ret = -1;

    if (!uci) {
        uh_log_err("uci_alloc_context fail\n");
        return -1;
    }

    uci_load(uci, ptr.package, &p);

    if (!p) {
        uh_log_err("Load config 'oui-httpd' fail\n");
        goto err;
    }

    uci_foreach_element(&p->sections, e) {
        struct rpc_object *obj;

        s = uci_to_section(e);

        if (strcmp(s->type, "trusted-object"))
            continue;

        ptr.section = s->e.name;
        ptr.s = NULL;

        ptr.option = "object";
        ptr.o = NULL;

        if (uci_lookup_ptr(uci, &ptr, NULL, true) || !ptr.o || ptr.o->type != UCI_TYPE_STRING)
            continue;

        obj = avl_find_element(&rpc_objects, ptr.o->v.string, obj, avl);
        if (!obj)
            continue;

        ptr.option = "method";
        ptr.o = NULL;

        if (uci_lookup_ptr(uci, &ptr, NULL, true) || !ptr.o)
            continue;

        if (ptr.o->type == UCI_TYPE_STRING) {
            if (add_trusted_method(obj, ptr.o->v.string))
                goto err;;
        } else {
            struct uci_element *oe;
            uci_foreach_element(&ptr.o->v.list, oe) {
                if (add_trusted_method(obj, oe->name))
                    goto err;;
            }
        }
    }

    ret = 0;

err:
    if (p)
        uci_unload(uci, p);

    uci_free_context(uci);

    return ret;
}

void rpc_init(const char *path)
{
    avl_init(&rpc_objects, avl_strcmp, false, NULL);

    load_rpc_scripts(path);

    load_trusted();
}

void rpc_deinit()
{
    unload_rpc_scripts();
}

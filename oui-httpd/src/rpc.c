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
#include <sys/sysinfo.h>
#include <netinet/in.h>
#include <pthread.h>
#include <lauxlib.h>
#include <sqlite3.h>
#include <dirent.h>
#include <assert.h>
#include <lualib.h>
#include <unistd.h>
#include <errno.h>

#include "lua2json.h"
#include "session.h"
#include "utils.h"
#include "rpc.h"
#include "db.h"

struct rpc_context {
    struct ev_loop *loop;
    const char *root;
    bool local_auth;
    struct list_head run_queue;
    struct list_head end_queue;
    struct ev_async end_watcher;
    json_t *no_auth_methods;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int nworker;
    bool done;
};

struct rpc_call_context {
    struct list_head node;
    struct uh_connection *conn;
    const char *object;
    const char *method;
    struct session *s;
    bool is_local;
    json_t *params;
    json_t *args;
    json_t *id;
    json_t *res;
};

static struct rpc_context rpc_context;


static const char *rpc_error_message(int code)
{
    switch (code) {
    case RPC_ERROR_CODE_PARSE_ERROR:
        return "Parse error";
    case RPC_ERROR_CODE_INVALID_REQUEST:
        return "Invalid Request";
    case RPC_ERROR_CODE_METHOD_NOT_FOUND:
        return "Method not found";
    case RPC_ERROR_CODE_INVALID_PARAMS:
        return "Invalid params";
    case RPC_ERROR_CODE_INTERNAL_ERROR:
        return "Internal error";
    case RPC_ERROR_CODE_ACCESS:
        return "Access denied";
    case RPC_ERROR_CODE_NOT_FOUND:
        return "Not found";
    default:
        return "Unknown";
    }
}

static json_t *rpc_error_object(int code, json_t *data)
{
    json_t *res = json_pack("{s:i,s:s}", "code", code, "message", rpc_error_message(code));

    if (data)
        json_object_set_new(res, "data", data);

    return res;
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
    return rpc_error_response(*id, rpc_error_object(RPC_ERROR_CODE_INVALID_REQUEST, data));
}

static int rpc_json_dump_callback(const char *buffer, size_t size, void *data)
{
    struct uh_connection *conn = data;

    conn->send(conn, buffer, size);

    return 0;
}

static void rpc_handle_done_final(struct uh_connection *conn, json_t *resp)
{
    if (!resp) {
        conn->send_error(conn, HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL);
        return;
    }

    conn->send_head(conn, HTTP_STATUS_OK, -1, NULL);
    conn->send_header(conn, "Content-Type", "application/json");
    conn->end_headers(conn);

    json_dump_callback(resp, rpc_json_dump_callback, conn, 0);
    json_decref(resp);

    conn->end_response(conn);
}

static bool rpc_is_no_auth(const char *object, const char *method)
{
    json_t *obj = json_object_get(rpc_context.no_auth_methods, object);
    if (!json_is_object(obj))
        return false;

    return json_object_get(obj, method);
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

static int rpc_method_login(struct uh_connection *conn, json_t *id, json_t *params, json_t **result)
{
    const char *username, *password = "", *sid;
    json_error_t error;

    if (json_unpack_ex(params, &error, 0, "{s:s,s?s}", "username", &username, "password", &password) < 0) {
        *result = rpc_error_object(RPC_ERROR_CODE_INVALID_PARAMS, json_string(error.text));
        return RPC_METHOD_RETURN_ERROR;
    }

    sid = session_login(username, password);
    if (!sid) {
        *result = rpc_error_object(RPC_ERROR_CODE_ACCESS, NULL);
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
        *result = rpc_error_object(RPC_ERROR_CODE_INVALID_PARAMS, json_string(error.text));
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
        *result = rpc_error_object(RPC_ERROR_CODE_INVALID_PARAMS, json_string(error.text));
        return RPC_METHOD_RETURN_ERROR;
    }

    if (!session_get(sid)) {
        *result = rpc_error_object(RPC_ERROR_CODE_ACCESS, NULL);
        return RPC_METHOD_RETURN_ERROR;
    }

    return RPC_METHOD_RETURN_SUCCESS;
}

static int rpc_method_call(struct uh_connection *conn, json_t *id, json_t *params, json_t **result)
{
    bool is_local = is_loopback_addr(conn->get_paddr(conn)) && !rpc_context.local_auth;
    const char *sid, *object, *method;
    struct rpc_call_context *ctx;
    const char *fmt = "[sss]";
    json_error_t error;
    json_t *args = NULL;

    if (!json_is_array(params)) {
        *result = rpc_error_object(RPC_ERROR_CODE_INVALID_PARAMS, json_string("Expected array, got object"));
        return RPC_METHOD_RETURN_ERROR;
    }

    if (json_array_size(params) > 3)
        fmt = "[ssso]";

    if (json_unpack_ex(params, &error, 0, fmt, &sid, &object, &method, &args) < 0) {
        *result = rpc_error_object(RPC_ERROR_CODE_INVALID_PARAMS, json_string(error.text));
        return RPC_METHOD_RETURN_ERROR;
    }

    if (args && !json_is_object(args)) {
        *result = rpc_error_object(RPC_ERROR_CODE_INVALID_PARAMS,
                                              json_string("The argument must be an object"));
        return RPC_METHOD_RETURN_ERROR;
    }

    ctx = calloc(1, sizeof(struct rpc_call_context));
    if (!ctx) {
        *result = rpc_error_object(RPC_ERROR_CODE_INTERNAL_ERROR, NULL);
        return RPC_METHOD_RETURN_ERROR;
    }

    conn->incref(conn);

    ctx->s = session_get(sid);
    ctx->is_local = is_local;
    ctx->object = object;
    ctx->method = method;
    ctx->params = params;
    ctx->conn = conn;
    ctx->args = args;
    ctx->id = id;

    json_incref(params);
    json_incref(args);

    pthread_mutex_lock(&rpc_context.mutex);
    list_add_tail(&ctx->node, &rpc_context.run_queue);
    pthread_mutex_unlock(&rpc_context.mutex);

    pthread_cond_signal(&rpc_context.cond);

    log_debug("prepare call %s.%s...\n", object, method);

    return RPC_METHOD_RETURN_DEFERRED;
}

static struct rpc_method_entry methods[] = {
    {"login",  rpc_method_login},
    {"logout", rpc_method_logout},
    {"alive",  rpc_method_alive},
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
        resp = rpc_error_response(id, rpc_error_object(RPC_ERROR_CODE_METHOD_NOT_FOUND, NULL));
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
        resp = rpc_error_response(id, rpc_error_object(RPC_ERROR_CODE_INTERNAL_ERROR, NULL));
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
        conn->send_error(conn, HTTP_STATUS_METHOD_NOT_ALLOWED, NULL);
        return;
    }

    body = conn->get_body(conn);
    req = json_loadb(body.p, body.len, 0, &error);
    if (!req) {
        resp = rpc_error_response(NULL, rpc_error_object(RPC_ERROR_CODE_PARSE_ERROR, NULL));
        goto err;
    }

    if (json_is_array(req)) {
        resp = rpc_error_response(NULL, rpc_error_object(RPC_ERROR_CODE_INVALID_REQUEST,
                                                                    json_string("Not support batch")));
        goto err;
    }

    rpc_handle_request(conn, req);

err:
    if (resp)
        rpc_handle_done_final(conn, resp);
    json_decref(req);
}

static void load_no_auth_methods(const char *file)
{
    json_error_t error;

    if (!file)
        return;

    rpc_context.no_auth_methods = json_load_file(file, 0, &error);
    if (!rpc_context.no_auth_methods) {
        log_err("json_load_file '%s' fail: %s\n", file, error.text);
    }
}

static void *rpc_call_worker(void *arg)
{
    struct rpc_call_context *ctx;
    int id = (intptr_t)arg;
    struct session *s;
    char path[128];
    lua_State *L;
    bool is_err;
    json_t *res;

    log_info("rpc worker(%d) running\n", id);

    while (true) {
        char remote_addr[INET6_ADDRSTRLEN];
        int remote_port;

        is_err = true;
        res = NULL;

        pthread_mutex_lock(&rpc_context.mutex);

        if (rpc_context.done)
            goto done;

        while (list_empty(&rpc_context.run_queue)) {
            pthread_cond_wait(&rpc_context.cond, &rpc_context.mutex);

            if (rpc_context.done)
                goto done;
        }

        ctx = list_first_entry(&rpc_context.run_queue, struct rpc_call_context, node);

        list_del(&ctx->node);

        pthread_mutex_unlock(&rpc_context.mutex);

        snprintf(path, sizeof(path), "%s/%s", rpc_context.root, ctx->object);

        L = luaL_newstate();

        luaL_openlibs(L);

        if (luaL_dofile(L, path)) {
            res = rpc_error_object(RPC_ERROR_CODE_NOT_FOUND, json_string("Object not found"));
            goto call_done;
        }

        lua_getfield(L, -1, ctx->method);
        if (!lua_isfunction(L, -1)) {
            res = rpc_error_object(RPC_ERROR_CODE_NOT_FOUND, json_string("Method not found"));
            goto call_done;
        }

        s = ctx->s;

        if (!ctx->is_local && !rpc_is_no_auth(ctx->object, ctx->method) &&
            (!s || !rpc_call_access(s, ctx->object, ctx->method))) {
            res = rpc_error_object(RPC_ERROR_CODE_ACCESS, NULL);
            goto call_done;
        }

        lua_newtable(L);

        if (s) {
            lua_pushstring(L, s->username);
            lua_setfield(L, -2, "username");

            lua_pushstring(L, s->aclgroup);
            lua_setfield(L, -2, "aclgroup");
        }

        saddr2str((struct sockaddr *)ctx->conn->get_paddr(ctx->conn),
            remote_addr, sizeof(remote_addr), &remote_port);

        lua_pushinteger(L, remote_port);
        lua_setfield(L, -2, "remote_port");

        lua_pushstring(L, remote_addr);
        lua_setfield(L, -2, "remote_addr");

        lua_pushboolean(L, ctx->is_local);
        lua_setfield(L, -2, "is_local");

        lua_setglobal(L, "__oui_session");

        if (ctx->args) {
            json_to_lua(ctx->args, L);
            json_decref(ctx->args);
            ctx->args = NULL;
        } else {
            lua_newtable(L);
        }

        log_debug("call %s.%s...\n", ctx->object, ctx->method);

        if (lua_pcall(L, 1, 2, 0)) {
            const char *err_msg = lua_tostring(L, -1);
            json_t *data = json_string(err_msg);

            log_err("%s\n", err_msg);
            res = rpc_error_object(RPC_ERROR_CODE_INTERNAL_ERROR, data);
            goto call_done;
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
            res = rpc_error_object(lua_tointeger(L, -2), data);
        } else {
            if (lua_istable(L, -2))
                res = lua_to_json(L, -2, true);
            is_err = false;
        }

call_done:
        lua_close(L);

        if (is_err)
            ctx->res = rpc_error_response(ctx->id, res);
        else
            ctx->res = rpc_result_response(ctx->id, res);

        json_decref(ctx->id);
        ctx->id = NULL;

        pthread_mutex_lock(&rpc_context.mutex);
        list_add_tail(&ctx->node, &rpc_context.end_queue);

        if (!ev_async_pending(&rpc_context.end_watcher)) {
            struct uh_connection *conn = ctx->conn;
            ev_async_send(conn->get_loop(conn), &rpc_context.end_watcher);
        }
        pthread_mutex_unlock(&rpc_context.mutex);
    }

done:
    rpc_context.nworker--;
    pthread_mutex_unlock(&rpc_context.mutex);
    log_info("rpc worker(%d) quit\n", id);
    return NULL;
}

static void call_end_cb(struct ev_loop *loop, struct ev_async *w, int revents)
{
    struct rpc_call_context *ctx;
    struct uh_connection *conn;

    while (true) {
        pthread_mutex_lock(&rpc_context.mutex);
        if (list_empty(&rpc_context.end_queue)) {
            pthread_mutex_unlock(&rpc_context.mutex);
            return;
        }
        ctx = list_first_entry(&rpc_context.end_queue, struct rpc_call_context, node);
        conn = ctx->conn;
        list_del(&ctx->node);
        pthread_mutex_unlock(&rpc_context.mutex);

        rpc_handle_done_final(ctx->conn, ctx->res);

        conn->decref(conn);

        log_debug("call %s.%s done\n", ctx->object, ctx->method);

        json_decref(ctx->params);

        free(ctx);
    }
}

int rpc_init(struct ev_loop *loop, const char *path, bool local_auth, const char *no_auth_file, int nworker)
{
    int i;

    rpc_context.loop = loop;
    rpc_context.root = path;

    load_no_auth_methods(no_auth_file);

    INIT_LIST_HEAD(&rpc_context.run_queue);
    INIT_LIST_HEAD(&rpc_context.end_queue);

    pthread_mutex_init(&rpc_context.mutex, NULL);
    pthread_cond_init(&rpc_context.cond, NULL);

    rpc_context.local_auth = local_auth;

    ev_async_init(&rpc_context.end_watcher, call_end_cb);
    ev_async_start(loop, &rpc_context.end_watcher);

    if (nworker > RPC_MAX_WORKER_NUM) {
        nworker = RPC_MAX_WORKER_NUM;
        log_info("The number of worker threads is limited to %d\n", nworker);
    }

    if (nworker < 0)
        nworker = get_nprocs();

    if (nworker == 1) {
        nworker++;
        log_info("At least 2 worker threads must be need\n");
    }

    for (i = 0; i < nworker; i++) {
        pthread_t tid;

        if (pthread_create(&tid, NULL, rpc_call_worker, (void *)(intptr_t)i)) {
            log_err("pthread_create: %s\n", strerror(errno));
            break;
        }

        pthread_detach(tid);

        rpc_context.nworker++;
    }

    if (i < 1) {
        log_err("no rpc call worker created\n");
        return -1;
    }

    return 0;
}

static void free_rpc_call_context(struct rpc_call_context *ctx)
{
    struct uh_connection *conn = ctx->conn;

    json_decref(ctx->args);
    json_decref(ctx->res);
    json_decref(ctx->id);

    list_del(&ctx->node);

    conn->decref(conn);

    free(ctx);
}

static void free_rpc_call_contexts()
{
    struct rpc_call_context *pos, *t;

    list_for_each_entry_safe(pos, t, &rpc_context.run_queue, node)
        free_rpc_call_context(pos);

    list_for_each_entry_safe(pos, t, &rpc_context.end_queue, node)
        free_rpc_call_context(pos);
}

void rpc_deinit(struct ev_loop *loop)
{
    pthread_mutex_lock(&rpc_context.mutex);
    rpc_context.done = true;
    pthread_mutex_unlock(&rpc_context.mutex);

    pthread_cond_broadcast(&rpc_context.cond);

    while (true) {
        pthread_mutex_lock(&rpc_context.mutex);
        if (rpc_context.nworker == 0) {
            pthread_mutex_unlock(&rpc_context.mutex);
            break;
        }
        pthread_mutex_unlock(&rpc_context.mutex);

        usleep(10000);
    }

    free_rpc_call_contexts();

    json_decref(rpc_context.no_auth_methods);
}

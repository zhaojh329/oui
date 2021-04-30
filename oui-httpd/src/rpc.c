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
#include <libubox/avl.h>
#include <libubox/avl-cmp.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <lauxlib.h>
#include <sqlite3.h>
#include <dirent.h>
#include <assert.h>
#include <lualib.h>
#include <unistd.h>
#include <errno.h>
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
    [RPC_ERROR_CODE_PARSE_ERROR] = {-32700, "Parse error"},
    [RPC_ERROR_CODE_INVALID_REQUEST] = {-32600, "Invalid Request"},
    [RPC_ERROR_CODE_METHOD_NOT_FOUND] = {-32601, "Method not found"},
    [RPC_ERROR_CODE_INVALID_PARAMS] = {-32602, "Invalid params"},
    [RPC_ERROR_CODE_INTERNAL_ERROR] = {-32603, "Internal error"},
    [RPC_ERROR_CODE_ACCESS] = {-32000, "Access denied"},
    [RPC_ERROR_CODE_NOT_FOUND] = {-32001, "Not found"},
    [RPC_ERROR_CODE_TIMEOUT] = {-32002, "Timeout"}
};

struct rpc_context {
    struct ev_loop *loop;
    struct avl_tree objects;
    struct ev_stat stat;
    bool local_auth;
    struct list_head run_queue;
    struct list_head end_queue;
    struct ev_async end_watcher;
    int nworker;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool done;
};

struct rpc_call_context {
    struct list_head node;
    struct uh_connection *conn;
    struct rpc_object *obj;
    struct session *s;
    bool is_local;
    json_t *args;
    json_t *id;
    json_t *res;
    char method[0];
};

struct rpc_object {
    struct avl_tree trusted_methods;
    struct avl_node avl;
    pthread_mutex_t mutex;
    struct list_head node;
    struct ev_stat es;
    size_t refcount;
    lua_State *L;
    char path[128];
    char value[0];
};

struct rpc_trusted_method {
    struct avl_node avl;
    char value[0];
};

static struct rpc_context rpc_context;

static json_t *rpc_error_object(int code, const char *message, json_t *data)
{
    json_t *json = json_pack("{s:i,s:s}", "code", code, "message", message ? message : "");

    if (data)
        json_object_set_new(json, "data", data);

    return json;
}

static json_t *rpc_error_object_predefined(int code, json_t *data)
{
    if (code > RPC_ERROR_CODE_NONE && code < __RPC_ERROR_CODE_MAX)
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
    return rpc_error_response(*id, rpc_error_object_predefined(RPC_ERROR_CODE_INVALID_REQUEST, data));
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

static void rpc_object_incref(struct rpc_object *obj)
{
    if (!obj)
        return;

    __sync_add_and_fetch(&obj->refcount, 1);
}

static void free_all_trusted_methods(struct rpc_object *obj)
{
    struct rpc_trusted_method *m, *temp;

    avl_for_each_element_safe(&obj->trusted_methods, m, avl, temp) {
        avl_delete(&obj->trusted_methods, &m->avl);
        free(m);
    }
}

static void rpc_object_decref(struct rpc_object *obj)
{
    if (!obj)
        return;

    if (__sync_sub_and_fetch(&obj->refcount, 1))
        return;

    uh_log_debug("Free rpc object: %p\n", obj);

    ev_stat_stop(rpc_context.loop, &obj->es);

    free_all_trusted_methods(obj);
    lua_close(obj->L);
    free(obj);
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

static int rpc_method_login(struct uh_connection *conn, json_t *id, json_t *params, json_t **result)
{
    const char *username, *password = "", *sid;
    json_error_t error;

    if (json_unpack_ex(params, &error, 0, "{s:s,s?s}", "username", &username, "password", &password) < 0) {
        *result = rpc_error_object_predefined(RPC_ERROR_CODE_INVALID_PARAMS, json_string(error.text));
        return RPC_METHOD_RETURN_ERROR;
    }

    sid = session_login(username, password);
    if (!sid) {
        *result = rpc_error_object_predefined(RPC_ERROR_CODE_ACCESS, NULL);
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
        *result = rpc_error_object_predefined(RPC_ERROR_CODE_INVALID_PARAMS, json_string(error.text));
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
        *result = rpc_error_object_predefined(RPC_ERROR_CODE_INVALID_PARAMS, json_string(error.text));
        return RPC_METHOD_RETURN_ERROR;
    }

    if (!session_get(sid)) {
        *result = rpc_error_object_predefined(RPC_ERROR_CODE_ACCESS, NULL);
        return RPC_METHOD_RETURN_ERROR;
    }

    return RPC_METHOD_RETURN_SUCCESS;
}

static int rpc_method_call(struct uh_connection *conn, json_t *id, json_t *params, json_t **result)
{
    bool is_local = is_loopback_addr(conn->get_addr(conn)) && !rpc_context.local_auth;
    const char *sid, *object, *method;
    struct rpc_call_context *ctx;
    struct rpc_object *obj;
    const char *fmt = "[sss]";
    json_error_t error;
    json_t *args = NULL;

    if (!json_is_array(params)) {
        *result = rpc_error_object_predefined(RPC_ERROR_CODE_INVALID_PARAMS, json_string("Expected array, got object"));
        return RPC_METHOD_RETURN_ERROR;
    }

    if (json_array_size(params) > 3)
        fmt = "[ssso]";

    if (json_unpack_ex(params, &error, 0, fmt, &sid, &object, &method, &args) < 0) {
        *result = rpc_error_object_predefined(RPC_ERROR_CODE_INVALID_PARAMS, json_string(error.text));
        return RPC_METHOD_RETURN_ERROR;
    }

    if (args && !json_is_object(args)) {
        *result = rpc_error_object_predefined(RPC_ERROR_CODE_INVALID_PARAMS,
                                              json_string("The argument must be an object"));
        return RPC_METHOD_RETURN_ERROR;
    }

    obj = avl_find_element(&rpc_context.objects, object, obj, avl);
    if (!obj) {
        *result = rpc_error_object_predefined(RPC_ERROR_CODE_NOT_FOUND, json_string("Object not found"));
        return RPC_METHOD_RETURN_ERROR;
    }

    ctx = calloc(1, sizeof(struct rpc_call_context) + strlen(method) + 1);
    if (!ctx) {
        *result = rpc_error_object_predefined(RPC_ERROR_CODE_INTERNAL_ERROR, NULL);
        return RPC_METHOD_RETURN_ERROR;
    }

    conn->incref(conn);

    ctx->s = session_get(sid);
    ctx->is_local = is_local;
    ctx->conn = conn;
    ctx->args = args;
    ctx->obj = obj;
    ctx->id = id;

    json_incref(args);

    strcpy(ctx->method, method);

    pthread_mutex_lock(&rpc_context.mutex);
    list_add_tail(&ctx->node, &rpc_context.run_queue);
    pthread_mutex_unlock(&rpc_context.mutex);

    pthread_cond_signal(&rpc_context.cond);

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
        resp = rpc_error_response(id, rpc_error_object_predefined(RPC_ERROR_CODE_METHOD_NOT_FOUND, NULL));
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
        resp = rpc_error_response(id, rpc_error_object_predefined(RPC_ERROR_CODE_INTERNAL_ERROR, NULL));
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
        resp = rpc_error_response(NULL, rpc_error_object_predefined(RPC_ERROR_CODE_PARSE_ERROR, NULL));
        goto err;
    }

    if (json_is_array(req)) {
        resp = rpc_error_response(NULL, rpc_error_object_predefined(RPC_ERROR_CODE_INVALID_REQUEST,
                                                                    json_string("Not support batch")));
        goto err;
    }

    rpc_handle_request(conn, req);

err:
    if (resp)
        rpc_handle_done_final(conn, resp);
    json_decref(req);
}

static bool load_rpc_script(lua_State *L, const char *path)
{
    if (luaL_dofile(L, path)) {
        uh_log_err("load rpc: %s\n", lua_tostring(L, -1));
        return false;
    }

    if (!lua_istable(L, -1)) {
        uh_log_err("invalid rpc script, need return a table: %s\n", path);
        return false;
    }

    return true;
}

static void rpc_object_changed(struct ev_loop *loop, struct ev_stat *w, int revents)
{
    struct rpc_object *obj = container_of(w, struct rpc_object, es);

    if (w->attr.st_nlink) {
        uh_log_info("rpc %s changed\n", w->path);

        pthread_mutex_lock(&obj->mutex);
        if (load_rpc_script(obj->L, obj->path)) {
            pthread_mutex_unlock(&obj->mutex);
            return;
        }
        pthread_mutex_unlock(&obj->mutex);
    } else {
        uh_log_info("rpc %s removed\n", w->path);
    }

    pthread_mutex_lock(&rpc_context.mutex);
    avl_delete(&rpc_context.objects, &obj->avl);
    pthread_mutex_unlock(&rpc_context.mutex);

    rpc_object_decref(obj);
}

static void load_rpc_scripts(struct ev_loop *loop, const char *path)
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

        if (e->d_type != DT_REG || strchr(e->d_name, '.'))
            continue;

        snprintf(object_path, sizeof(object_path) - 1, "%s/%s", path, e->d_name);

        obj = avl_find_element(&rpc_context.objects, e->d_name, obj, avl);
        if (obj)
            continue;

        L = luaL_newstate();

        luaL_openlibs(L);

        if (!load_rpc_script(L, object_path))
            goto err;

        obj = calloc(1, sizeof(struct rpc_object) + strlen(e->d_name) + 1);
        if (!obj) {
            uh_log_err("calloc: %s\n", strerror(errno));
            goto err;
        }

        obj->L = L;

        pthread_mutex_init(&obj->mutex, NULL);

        strcpy(obj->value, e->d_name);
        obj->avl.key = obj->value;
        avl_init(&obj->trusted_methods, avl_strcmp, false, NULL);

        strcpy(obj->path, object_path);

        ev_stat_init(&obj->es, rpc_object_changed, obj->path, 0.);
        ev_stat_start(loop, &obj->es);

        rpc_object_incref(obj);

        pthread_mutex_lock(&rpc_context.mutex);
        avl_insert(&rpc_context.objects, &obj->avl);
        pthread_mutex_unlock(&rpc_context.mutex);

        uh_log_info("rpc %s loaded\n", obj->path);

        continue;

err:
        lua_close(L);
    }

    closedir(dir);
}

static void unload_rpc_scripts()
{
    struct rpc_object *obj, *temp;

    avl_for_each_element_safe(&rpc_context.objects, obj, avl, temp) {
        avl_delete(&rpc_context.objects, &obj->avl);
        rpc_object_decref(obj);
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

        obj = avl_find_element(&rpc_context.objects, ptr.o->v.string, obj, avl);
        if (!obj)
            continue;

        ptr.option = "method";
        ptr.o = NULL;

        if (uci_lookup_ptr(uci, &ptr, NULL, true) || !ptr.o)
            continue;

        if (ptr.o->type == UCI_TYPE_STRING) {
            if (add_trusted_method(obj, ptr.o->v.string))
                goto err;
        } else {
            struct uci_element *oe;
            uci_foreach_element(&ptr.o->v.list, oe) {
                if (add_trusted_method(obj, oe->name))
                    goto err;
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

static void rpc_dir_changed(struct ev_loop *loop, struct ev_stat *w, int revents)
{
    load_rpc_scripts(loop, w->path);
}

static void *rpc_call_worker(void *arg)
{
    struct rpc_call_context *ctx;
    int id = (intptr_t)arg;
    struct rpc_object *obj;
    struct session *s;
    lua_State *L;
    bool is_err;
    json_t *res;

    uh_log_info("rpc worker(%d) running\n", id);

    while (true) {
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
        obj = ctx->obj;
        L = obj->L;
        s = ctx->s;

        if (pthread_mutex_trylock(&obj->mutex)) {
            pthread_mutex_unlock(&rpc_context.mutex);
            continue;
        }

        list_del(&ctx->node);

        pthread_mutex_unlock(&rpc_context.mutex);

        if (!lua_istable(L, -1)) {
            uh_log_err("%s.%s: lua state is broken. No table on stack!\n", obj->value, ctx->method);
            res = rpc_error_object_predefined(RPC_ERROR_CODE_INTERNAL_ERROR, NULL);
            goto lua_broken;
        }

        lua_getfield(L, -1, ctx->method);
        if (!lua_isfunction(L, -1)) {
            res = rpc_error_object_predefined(RPC_ERROR_CODE_NOT_FOUND, json_string("Method not found"));
            goto call_done;
        }

        if (!ctx->is_local && !rpc_is_trusted(obj, ctx->method) &&
            (!s || !rpc_call_access(s, obj->value, ctx->method))) {
            res = rpc_error_object_predefined(RPC_ERROR_CODE_ACCESS, NULL);
            goto call_done;
        }

        lua_newtable(L);

        if (s) {
            lua_pushstring(L, s->username);
            lua_setfield(L, -2, "username");

            lua_pushstring(L, s->aclgroup);
            lua_setfield(L, -2, "aclgroup");
        }

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

        if (lua_pcall(L, 1, 2, 0)) {
            const char *err_msg = lua_tostring(L, -1);
            json_t *data = json_string(err_msg);

            uh_log_err("%s\n", err_msg);
            res = rpc_error_object_predefined(RPC_ERROR_CODE_INTERNAL_ERROR, data);
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
            res = rpc_error_object_predefined(lua_tointeger(L, -2), data);
        } else {
            if (lua_istable(L, -2))
                res = lua_to_json(L, -2, true);
            is_err = false;
        }

        lua_pop(L, 1);

call_done:
        lua_pop(L, 1);
lua_broken:
        pthread_mutex_unlock(&obj->mutex);

        if (is_err)
            ctx->res = rpc_error_response(ctx->id, res);
        else
            ctx->res = rpc_result_response(ctx->id, res);

        json_decref(ctx->id);
        ctx->id = NULL;

        pthread_mutex_lock(&rpc_context.mutex);
        list_add_tail(&ctx->node, &rpc_context.end_queue);
        pthread_mutex_unlock(&rpc_context.mutex);

        if (!ev_async_pending(&rpc_context.end_watcher)) {
            struct uh_connection *conn = ctx->conn;
            ev_async_send(conn->get_loop(conn), &rpc_context.end_watcher);
        }
    }

done:
    rpc_context.nworker--;
    pthread_mutex_unlock(&rpc_context.mutex);
    uh_log_info("rpc worker(%d) quit\n", id);
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

        free(ctx);
    }
}

int rpc_init(struct ev_loop *loop, const char *path, bool local_auth, int nworker)
{
    int i;

    rpc_context.loop = loop;

    avl_init(&rpc_context.objects, avl_strcmp, false, NULL);

    load_rpc_scripts(loop, path);

    load_trusted();

    INIT_LIST_HEAD(&rpc_context.run_queue);
    INIT_LIST_HEAD(&rpc_context.end_queue);

    pthread_mutex_init(&rpc_context.mutex, NULL);
    pthread_cond_init(&rpc_context.cond, NULL);

    ev_stat_init(&rpc_context.stat, rpc_dir_changed, path, 0.);
    ev_stat_start(loop, &rpc_context.stat);

    rpc_context.local_auth = local_auth;

    ev_async_init(&rpc_context.end_watcher, call_end_cb);
    ev_async_start(loop, &rpc_context.end_watcher);

    if (nworker > RPC_MAX_WORKER_NUM) {
        nworker = RPC_MAX_WORKER_NUM;
        uh_log_info("The number of worker threads is limited to %d\n", nworker);
    }

    if (nworker < 0)
        nworker = get_nprocs();

    if (nworker == 1) {
        nworker++;
        uh_log_info("At least 2 worker threads must be need\n");
    }

    for (i = 0; i < nworker; i++) {
        pthread_t tid;

        if (pthread_create(&tid, NULL, rpc_call_worker, (void *)(intptr_t)i)) {
            uh_log_err("pthread_create: %s\n", strerror(errno));
            break;
        }

        pthread_detach(tid);

        rpc_context.nworker++;
    }

    if (i < 1) {
        uh_log_err("no rpc call worker created\n");
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
    ev_stat_stop(loop, &rpc_context.stat);

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

    unload_rpc_scripts();
}

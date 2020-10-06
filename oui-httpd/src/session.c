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

#include <libubox/avl-cmp.h>
#include <libubox/utils.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <uci.h>

#include <uhttpd/log.h>

#include "session.h"

struct rpc_trusted_object {
    struct avl_node avl;
    struct avl_tree methods;
    char value[0];
};

struct rpc_trusted_method {
    struct avl_node avl;
    char value[0];
};

static struct avl_tree sessions;
static struct avl_tree trusted;

static int rpc_random(char *dest)
{
    unsigned char buf[16] = {0};
    FILE *f;
    int ret;
    int i;

    f = fopen("/dev/urandom", "r");
    if (!f)
        return -1;

    ret = fread(buf, 1, sizeof(buf), f);
    fclose(f);

    if (ret < 0)
        return ret;

    for (i = 0; i < sizeof(buf); i++)
        sprintf(dest + (i << 1), "%02x", buf[i]);

    return 0;
}


static void rpc_touch_session(struct rpc_session *s)
{
    if (s->timeout > 0) {
        struct ev_loop *loop = ev_default_loop(0);

        ev_timer_stop(loop, &s->tmr);
        ev_timer_set(&s->tmr, s->timeout, 0);
        ev_timer_start(loop, &s->tmr);
    }
}

static void rpc_session_destroy(struct rpc_session *s)
{
    struct ev_loop *loop = ev_default_loop(0);

    ev_timer_stop(loop, &s->tmr);

    avl_delete(&sessions, &s->avl);
    free(s);
}

static void rpc_session_timeout(struct ev_loop *loop, struct ev_timer *w, int revents)
{
    struct rpc_session *s = container_of(w, struct rpc_session, tmr);
    rpc_session_destroy(s);
}

static struct rpc_session *rpc_session_create(int timeout)
{
    struct rpc_session *s;

    s = calloc(1, sizeof(struct rpc_session));
    if (!s)
        return NULL;

    if (rpc_random(s->id))
        return NULL;

    s->avl.key = s->id;
    s->timeout = timeout;

    ev_timer_init(&s->tmr, rpc_session_timeout, s->timeout, 0);

    avl_insert(&sessions, &s->avl);

    rpc_touch_session(s);

    return s;
}

struct rpc_session *rpc_session_get(const char *sid)
{
    struct rpc_session *s;

    if (!sid)
        return NULL;

    s = avl_find_element(&sessions, sid, s, avl);
    if (!s)
        return NULL;

    rpc_touch_session(s);
    return s;
}

static int load_shadow(char shadow[], int len)
{
    FILE *f = fopen("/etc/oui/shadow", "r");
    if (!f) {
        uh_log_err("Load /etc/oui/shadow fail\n");
        return -1;
    }

    memset(shadow, 0, len);

    if (!fgets(shadow, len, f)) {
        fclose(f);
        return -1;
    }

    len = strlen(shadow);

    if (shadow[len - 1] == '\n')
        shadow[len - 1] = '\0';

    fclose(f);

    return 0;
}

const char *rpc_login(const char *password)
{
    struct rpc_session *s = NULL;
    char shadow[128];

    if (!load_shadow(shadow, sizeof(shadow)) && shadow[0] && strcmp(shadow, password))
        return NULL;

    s = rpc_session_create(RPC_DEFAULT_SESSION_TIMEOUT);

    return s ? s->id : NULL;
}

void rpc_logout(const char *sid)
{
    struct rpc_session *s = rpc_session_get(sid);
    if (s)
        rpc_session_destroy(s);
}

static struct rpc_trusted_object *add_trusted_object(const char *value)
{
    struct rpc_trusted_object *obj;

    obj = avl_find_element(&trusted, value, obj, avl);
    if (obj)
        return obj;

    obj = calloc(1, sizeof(struct rpc_trusted_object) + strlen(value) + 1);
    if (!obj) {
        uh_log_err("calloc: %s\n", strerror(errno));
        return NULL;
    }

    strcpy(obj->value, value);
    obj->avl.key = obj->value;

    avl_init(&obj->methods, avl_strcmp, false, NULL);

    avl_insert(&trusted, &obj->avl);

    return obj;
}

static int add_trusted_method(struct rpc_trusted_object *obj, const char *value)
{
    struct rpc_trusted_method *m = calloc(1, sizeof(struct rpc_trusted_method) + strlen(value) + 1);

    if (!m) {
        uh_log_err("calloc: %s\n", strerror(errno));
        return -1;
    }

    strcpy(m->value, value);
    m->avl.key = m->value;

    avl_insert(&obj->methods, &m->avl);

    return 0;
}

static int load_trusted(struct uci_context *uci)
{
    struct uci_package *p = NULL;
    struct uci_section *s;
    struct uci_element *e;
    struct uci_ptr ptr = {.package = "oui-httpd"};

    uci_load(uci, ptr.package, &p);

    if (!p) {
        uh_log_err("Load config 'oui-httpd' fail\n");
        return -1;
    }

    uci_foreach_element(&p->sections, e) {
        struct rpc_trusted_object *obj;

        s = uci_to_section(e);

        if (strcmp(s->type, "trusted"))
            continue;

        ptr.section = s->e.name;
        ptr.s = NULL;

        ptr.option = "object";
        ptr.o = NULL;

        if (uci_lookup_ptr(uci, &ptr, NULL, true) || !ptr.o || ptr.o->type != UCI_TYPE_STRING)
            continue;

        obj = add_trusted_object(ptr.o->v.string);
        if (!obj)
            return -1;

        ptr.option = "method";
        ptr.o = NULL;

        if (uci_lookup_ptr(uci, &ptr, NULL, true) || !ptr.o)
            continue;

        if (ptr.o->type == UCI_TYPE_STRING) {
            if (add_trusted_method(obj, ptr.o->v.string))
                return -1;
        } else {
            struct uci_element *oe;
            uci_foreach_element(&ptr.o->v.list, oe) {
                if (add_trusted_method(obj, oe->name))
                    return -1;
            }
        }
    }

    return 0;
}

int rpc_session_init()
{
    struct uci_context *uci = uci_alloc_context();
    int ret = 0;

    if (!uci) {
        uh_log_err("uci_alloc_context fail\n");
        return -1;
    }

    avl_init(&sessions, avl_strcmp, false, NULL);

    avl_init(&trusted, avl_strcmp, false, NULL);

    ret = load_trusted(uci);

    uci_free_context(uci);

    return ret;
}

static void free_all_session()
{
    struct rpc_session *s, *temp;

    avl_for_each_element_safe(&sessions, s, avl, temp) {
        rpc_session_destroy(s);
    }
}

static void free_all_trusted_methods(struct rpc_trusted_object *obj)
{
    struct rpc_trusted_method *m, *temp;

    avl_for_each_element_safe(&obj->methods, m, avl, temp) {
        avl_delete(&obj->methods, &m->avl);
        free(m);
    }
}

static void free_all_trusted()
{
    struct rpc_trusted_object *obj, *temp;

    avl_for_each_element_safe(&trusted, obj, avl, temp) {
        free_all_trusted_methods(obj);
        avl_delete(&trusted, &obj->avl);
        free(obj);
    }
}

void rpc_session_deinit()
{
    free_all_session();
    free_all_trusted();
}

bool rpc_session_allowed(const char *sid, const char *object, const char *method)
{
    struct rpc_trusted_object *obj;
    struct rpc_trusted_method *m;

    obj = avl_find_element(&trusted, object, obj, avl);
    if (obj) {
        if (avl_find_element(&obj->methods, method, m, avl))
            return true;
    }

    return rpc_session_get(sid) != NULL;
}


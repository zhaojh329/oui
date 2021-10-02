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

#include <uhttpd/log.h>
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "session.h"
#include "utils.h"
#include "md5.h"
#include "db.h"

struct login_param {
    char aclgroup[MAX_ACLGROUP_LEN + 1];
    char pwhash[33];
};

static LIST_HEAD(sessions);
static int nsession;

static int generate_sid(char *dest)
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


static void touch_session(struct session *s)
{
    if (s->timeout > 0) {
        struct ev_loop *loop = ev_default_loop(0);

        ev_timer_stop(loop, &s->tmr);
        ev_timer_set(&s->tmr, s->timeout, 0);
        ev_timer_start(loop, &s->tmr);
    }
}

static void session_destroy(struct session *s)
{
    struct ev_loop *loop = ev_default_loop(0);

    ev_timer_stop(loop, &s->tmr);

    list_del(&s->node);
    free(s);
}

static void session_timeout_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
    struct session *s = container_of(w, struct session, tmr);
    session_destroy(s);
}

static struct session *session_create(int timeout, const char *username, const char *aclgroup)
{
    struct session *s;

    if (strlen(username) > MAX_USERNAME_LEN) {
        log_err("username '%s' too long, more than %d characters\n", username, MAX_USERNAME_LEN);
        return NULL;
    }

    if (strlen(aclgroup) > MAX_ACLGROUP_LEN) {
        log_err("aclgroup '%s' too long, more than %d characters\n", aclgroup, MAX_ACLGROUP_LEN);
        return NULL;
    }

    s = calloc(1, sizeof(struct session));
    if (!s)
        return NULL;

    if (generate_sid(s->id)) {
        free(s);
        return NULL;
    }

    s->timeout = timeout;

    ev_timer_init(&s->tmr, session_timeout_cb, s->timeout, 0);

    strncpy(s->username, username, sizeof(s->username) - 1);
    strncpy(s->aclgroup, aclgroup, sizeof(s->aclgroup) - 1);

    list_add(&s->node, &sessions);

    touch_session(s);

    nsession++;

    return s;
}

struct session *session_get(const char *sid)
{
    struct session *s;

    if (!sid)
        return NULL;

    list_for_each_entry(s, &sessions, node) {
        if (!strcmp(s->id, sid)) {
            touch_session(s);
            return s;
        }
    }

    return NULL;
}

static inline int hex2num(int x)
{
    if (x >= '0' && x <= '9')
        return x - '0';
    else if (x >= 'A' && x <= 'F')
        return x - 'A' + 10;
    else if (x >= 'a' && x <= 'f')
        return x - 'a' + 10;
    else
        return 0;
}

static int login_cb(void *data, int count, char **value, char **name)
{
    struct login_param *param = data;

    if (value[0])
        strncpy(param->aclgroup, value[0], MAX_ACLGROUP_LEN);

    if (value[1])
        strncpy(param->pwhash, value[1], 32);

    return SQLITE_ABORT;
}

const char *session_login(const char *username, const char *password)
{
    struct login_param param = {};
    struct session *s;
    uint8_t md5[16];
    md5_ctx_t ctx;
    char sql[256];
    int i;

    if (nsession == MAX_SESSION)
        return NULL;

    sprintf(sql, "SELECT acl, password FROM account WHERE username = '%s'", username);

    if (db_query(sql, login_cb, &param) < 0)
        return NULL;

    /* Not found or password is empty */
    if (!param.pwhash[0])
        return NULL;

    md5_begin(&ctx);
    md5_hash(username, strlen(username), &ctx);
    md5_hash(password, strlen(password), &ctx);
    md5_end(md5, &ctx);

    for (i = 0; i < 16; i++) {
        if (((hex2num(param.pwhash[i * 2]) << 4) | hex2num(param.pwhash[i * 2 + 1])) != md5[i])
            return NULL;
    }

    s = session_create(SESSION_TIMEOUT, username, param.aclgroup);

    return s ? s->id : NULL;
}

void session_logout(const char *sid)
{
    struct session *s = session_get(sid);
    if (s)
        session_destroy(s);
}

static void free_all_session()
{
    struct session *s, *temp;

    list_for_each_entry_safe(s, temp, &sessions, node) {
        session_destroy(s);
    }
}

void session_deinit()
{
    free_all_session();
}

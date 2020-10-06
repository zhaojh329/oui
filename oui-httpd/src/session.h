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

#ifndef __SESSION_H
#define __SESSION_H

#include <libubox/avl.h>
#include <ev.h>

#include "stdbool.h"

#define RPC_DEFAULT_SESSION_TIMEOUT 300

#define RPC_SID_LEN    32

struct rpc_session {
    struct avl_node avl;
    char id[RPC_SID_LEN + 1];

    struct ev_timer tmr;

    int timeout;
};

int rpc_session_init();
void rpc_session_deinit();

const char *rpc_login(const char *password);

void rpc_logout(const char *sid);

struct rpc_session *rpc_session_get(const char *sid);

bool rpc_session_allowed(const char *sid, const char *object, const char *method);

#endif

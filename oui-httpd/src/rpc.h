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

#ifndef __RPC_H
#define __RPC_H

#include <stdbool.h>
#include <jansson.h>

enum {
    RPC_ERROR_CODE_NONE,
    RPC_ERROR_CODE_PARSE_ERROR = -32700,
    RPC_ERROR_CODE_INVALID_REQUEST = -32600,
    RPC_ERROR_CODE_METHOD_NOT_FOUND = -32601,
    RPC_ERROR_CODE_INVALID_PARAMS = -32602,
    RPC_ERROR_CODE_INTERNAL_ERROR = -32603,

    /* Custom error code */
    RPC_ERROR_CODE_ACCESS = -32000,
    RPC_ERROR_CODE_NOT_FOUND = -32001
};

enum {
    RPC_METHOD_RETURN_SUCCESS = 0,
    RPC_METHOD_RETURN_ERROR = -1,
    RPC_METHOD_RETURN_DEFERRED = 1
};

#define RPC_MAX_WORKER_NUM  10

struct uh_connection;

/* return value: reference RPC_METHOD_RETURN_xx */
typedef int (*rpc_method_prototype)(struct uh_connection *conn, json_t *id, json_t *params, json_t **result);

struct rpc_method_entry {
    const char *name;
    rpc_method_prototype handler;
};

int rpc_init(struct ev_loop *loop, const char *path, bool local_auth, const char *no_auth_file, int nworker);

void rpc_deinit(struct ev_loop *loop);

void serve_rpc(struct uh_connection *conn, int event);

#endif

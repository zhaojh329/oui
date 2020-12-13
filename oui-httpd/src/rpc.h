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

#include <jansson.h>

enum {
    ERROR_CODE_PARSE_ERROR,
    ERROR_CODE_INVALID_REQUEST,
    ERROR_CODE_METHOD_NOT_FOUND,
    ERROR_CODE_INVALID_PARAMS,
    ERROR_CODE_INTERNAL_ERROR,

    /* Custom error code */
    ERROR_CODE_ACCESS,
    ERROR_CODE_NOT_FOUND,
    ERROR_CODE_TIMEOUT,

    __ERROR_CODE_MAX
};

enum {
    RPC_METHOD_RETURN_SUCCESS = 0,
    RPC_METHOD_RETURN_ERROR = -1,
    RPC_METHOD_RETURN_DEFERRED = 1
};

struct uh_connection;

/* return value: reference RPC_METHOD_RETURN_xx */
typedef int (*rpc_method_prototype)(struct uh_connection *conn, json_t *id, json_t *params, json_t **result);

struct rpc_method_entry {
    const char *name;
    rpc_method_prototype handler;
};

void rpc_init(const char *path);

void rpc_deinit();

void serve_rpc(struct uh_connection *conn);

#endif

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
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "session.h"
#include "utils.h"

struct download_params {
    char sid[33];
    char path[512];
    char filename[256];
};

enum {
    PARSE_STATE_UNKNOWN,
    PARSE_STATE_SID,
    PARSE_STATE_PATH,
    PARSE_STATE_FILENAME
};

static void parse_data(const char *body, int body_len, struct download_params *params)
{
    int s = PARSE_STATE_UNKNOWN;
    const char *p;
    int i;

    for (p = body, i = 0; i < body_len; i++) {
        if (body[i] == '=') {
            if (!strncmp(p, "sid", 3))
                s = PARSE_STATE_SID;
            else if (!strncmp(p, "path", 3))
                s = PARSE_STATE_PATH;
            else if (!strncmp(p, "filename", 3))
                s = PARSE_STATE_FILENAME;
            else
                s = PARSE_STATE_UNKNOWN;
            p = body + i + 1;
        } else if (body[i] == '&' || i == body_len - 1) {
            int len = i - (p - body);

            if (body[i] != '&' && i == body_len - 1)
                len++;

            switch (s) {
            case PARSE_STATE_SID:
                strncpy(params->sid, p, len);
                break;
            case PARSE_STATE_PATH:
                strncpy(params->path, p, len);
                break;
            case PARSE_STATE_FILENAME:
                strncpy(params->filename, p, len);
                break;
            }

            p = body + i + 1;

            s = PARSE_STATE_UNKNOWN;
        }
    }

    urldecode(params->path, sizeof(params->path), params->path, strlen(params->path));
    urldecode(params->filename, sizeof(params->filename), params->filename, strlen(params->filename));
}

void serve_download(struct uh_connection *conn, int event)
{
    struct uh_str var = conn->get_header(conn, "Content-Type");
    struct download_params params = {};
    struct uh_str body;

    if (event != UH_EV_COMPLETE)
        return;

    if (conn->get_method(conn) != HTTP_POST) {
        conn->send_error(conn, HTTP_STATUS_METHOD_NOT_ALLOWED, NULL);
        return;
    }

    if (!var.p || var.len < 33 || strncmp(var.p, "application/x-www-form-urlencoded", 33)) {
        conn->send_error(conn, HTTP_STATUS_BAD_REQUEST, NULL);
        return;
    }

    body = conn->get_body(conn);

    parse_data(body.p, body.len, &params);

    if (!params.path[0] || !params.filename[0]) {
        conn->send_error(conn, HTTP_STATUS_BAD_REQUEST, NULL);
        return;
    }

    if (!is_loopback_addr(conn->get_paddr(conn)) && (!params.sid[0] || !session_get(params.sid))) {
        conn->send_error(conn, HTTP_STATUS_FORBIDDEN, NULL);
        return;
    }

    conn->download_file(conn, params.path, params.filename);
}

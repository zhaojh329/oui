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

static bool urldecode(char *buf)
{
    char *c, *p;

    if (!buf || !*buf)
        return true;

#define hex(x) \
    (((x) <= '9') ? ((x) - '0') : \
        (((x) <= 'F') ? ((x) - 'A' + 10) : \
            ((x) - 'a' + 10)))

    for (c = p = buf; *p; c++) {
        if (*p == '%') {
            if (!isxdigit(*(p + 1)) || !isxdigit(*(p + 2)))
                return false;

            *c = (char) (16 * hex(*(p + 1)) + hex(*(p + 2)));

            p += 3;
        } else if (*p == '+') {
            *c = ' ';
            p++;
        } else {
            *c = *p++;
        }
    }

    *c = 0;

    return true;
}

struct download_params {
    char path[512];
    char filename[256];
};

enum {
    PARSE_STATE_UNKNOWN,
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
            if (!strncmp(p, "path", 3))
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

    urldecode(params->path);
    urldecode(params->filename);
}

void serve_download(struct uh_connection *conn)
{
    struct uh_str var = conn->get_header(conn, "Content-Type");
    struct download_params params = {};
    struct uh_str body;
    struct stat st;

    if (conn->get_method(conn) != HTTP_POST) {
        conn->error(conn, HTTP_STATUS_METHOD_NOT_ALLOWED, NULL);
        return;
    }

    if (!var.p || var.len < 33 || strncmp(var.p, "application/x-www-form-urlencoded", 33)) {
        conn->error(conn, HTTP_STATUS_BAD_REQUEST, NULL);
        return;
    }

    body = conn->get_body(conn);

    parse_data(body.p, body.len, &params);

    if (!params.path[0] || !params.filename[0]) {
        conn->error(conn, HTTP_STATUS_FORBIDDEN, NULL);
        return;
    }

    if (stat(params.path, &st) < 0) {
        int code;

        switch (errno) {
        case EACCES:
            code = HTTP_STATUS_FORBIDDEN;
            break;
        case ENOENT:
            code = HTTP_STATUS_NOT_FOUND;
            break;
        default:
            code = HTTP_STATUS_INTERNAL_SERVER_ERROR;
        };

        conn->error(conn, code, NULL);
        return;
    }

    if (!S_ISLNK(st.st_mode) && !S_ISREG(st.st_mode)) {
        conn->error(conn, 403, NULL);
        return;
    }

    conn->send_file(conn, params.path);

    conn->send_status_line(conn, HTTP_STATUS_OK, "Content-Type: application/octet-stream\r\n");
    conn->printf(conn, "Content-Length: %lld\r\n", st.st_size);
    conn->printf(conn, "Content-Disposition: attachment; filename=\"%s\"\r\n", params.filename);
    conn->printf(conn, "\r\n");
    conn->send_file(conn, params.path);
    conn->done(conn);
}

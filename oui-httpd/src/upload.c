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
#include <errno.h>
#include <fcntl.h>

#include "multipart_parser.h"

enum part {
    PART_UNKNOWN,
    PART_PATH,
    PART_FILE
};

struct state {
    struct uh_connection *conn;
    bool is_content_disposition;
    enum part parttype;
    char path[256];
    int fd;
};

const char *parts[] = {
    "path",
    "file"
};

static int header_field(struct multipart_parser *p, const char *data, size_t len)
{
    struct state *st = multipart_parser_get_data(p);
    st->is_content_disposition = !strncasecmp(data, "Content-Disposition", len);
    return 0;
}

static int header_value(struct multipart_parser *p, const char *data, size_t len)
{
    struct state *st = multipart_parser_get_data(p);
    size_t i, j;

    if (!st->is_content_disposition)
        return 0;

    if (len < 10 || strncasecmp(data, "form-data", 9))
        return 0;

    for (data += 9, len -= 9; *data == ' ' || *data == ';'; data++, len--);

    if (len < 8 || strncasecmp(data, "name=\"", 6))
        return 0;

    for (data += 6, len -= 6, i = 0; i <= len; i++) {
        if (*(data + i) != '"')
            continue;

        for (j = 0; j < sizeof(parts) / sizeof(parts[0]); j++)
            if (!strncmp(data, parts[j], i))
                st->parttype = j + 1;

        break;
    }

    return 0;
}

static int data_begin_cb(struct multipart_parser *p)
{
    struct state *st = multipart_parser_get_data(p);
    struct uh_connection *conn = st->conn;

    if (st->parttype == PART_FILE) {
        if (!st->path[0]) {
            uh_log_err("Not found path\n");
            conn->error(conn, HTTP_STATUS_FORBIDDEN, NULL);
            return 1;
        }

        st->fd = open(st->path, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
        if (st->fd < 0) {
            uh_log_err("Create '%s' fail: %s\n", st->path, strerror(errno));
            conn->error(conn, HTTP_STATUS_FORBIDDEN, NULL);
            return 1;
        }
    }

    return 0;
}

static int data_cb(struct multipart_parser *p, const char *data, size_t len)
{
    struct state *st = multipart_parser_get_data(p);
    struct uh_connection *conn = st->conn;
    int wlen = len;

    switch (st->parttype) {
    case PART_PATH:
        if (strlen(st->path) + len > sizeof(st->path) - 1) {
            uh_log_err("path too long\n");
            return 1;
        }
        strncat(st->path, data, len);
        break;

    case PART_FILE:
        if (write(st->fd, data, len) != wlen) {
            close(st->fd);
            uh_log_err("write fail: %s\n", strerror(errno));
            conn->error(conn, HTTP_STATUS_INTERNAL_SERVER_ERROR, NULL);
            return 1;
        }

        break;

    default:
        break;
    }

    return 0;
}

static int data_end_cb(struct multipart_parser *p)
{
    struct state *st = multipart_parser_get_data(p);

    if (st->parttype == PART_FILE) {
        close(st->fd);
    }

    st->parttype = PART_UNKNOWN;
    return 0;
}

static int body_end_cb(struct multipart_parser *p)
{
    struct state *st = multipart_parser_get_data(p);
    struct uh_connection *conn = st->conn;

    if (st->fd > 0) {
        conn->send_head(conn, HTTP_STATUS_OK, 2, NULL);
        conn->send(conn, "OK", 2);
        return 0;
    }

    conn->error(conn, HTTP_STATUS_FORBIDDEN, NULL);
    return 1;
}

static struct multipart_parser *init_parser(struct uh_connection *conn)
{
    static struct multipart_parser_settings s = {
        .on_header_field     = header_field,
        .on_header_value     = header_value,
        .on_headers_complete = data_begin_cb,
        .on_part_data        = data_cb,
        .on_part_data_end    = data_end_cb,
        .on_body_end         = body_end_cb
    };
    struct uh_str var = conn->get_header(conn, "Content-Type");
    struct multipart_parser *p;
    static struct state st;
    char *boundary;

    if (conn->get_method(conn) != HTTP_POST) {
        conn->error(conn, HTTP_STATUS_METHOD_NOT_ALLOWED, NULL);
        return NULL;
    }

    if (!var.p || var.len < 20 || strncmp(var.p, "multipart/form-data;", 20)) {
        conn->error(conn, HTTP_STATUS_BAD_REQUEST, NULL);
        return NULL;
    }

    for (var.p += 20, var.len -= 20; var.len > 0 && *var.p != '='; var.p++, var.len--);

    if (*var.p++ != '=') {
        conn->error(conn, HTTP_STATUS_BAD_REQUEST, NULL);
        return NULL;
    }
    var.len--;

    boundary = malloc(var.len + 3);
    if (!boundary) {
        conn->error(conn, HTTP_STATUS_SERVICE_UNAVAILABLE, NULL);
        return NULL;
    }

    strcpy(boundary, "--");
    strncpy(boundary + 2, var.p, var.len);
    boundary[var.len + 2] = '\0';

    p = multipart_parser_init(boundary, &s);

    memset(&st, 0, sizeof(st));

    st.conn = conn;
    st.fd = -1;

    multipart_parser_set_data(p, &st);

    free(boundary);

    return p;
}

void serve_upload(struct uh_connection *conn)
{
    struct multipart_parser *p = init_parser(conn);
    struct uh_str body;

    if (!p)
        return;

    body = conn->get_body(conn);

    multipart_parser_execute(p, body.p, body.len);
    multipart_parser_free(p);

    conn->done(conn);
}

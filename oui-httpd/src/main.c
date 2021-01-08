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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

#include <uhttpd/uhttpd.h>

#include "session.h"
#include "rpc.h"
#include "db.h"

enum {
    LONG_OPT_RPC = 1,
    LONG_OPT_HOME,
    LONG_OPT_INDEX,
    LONG_OPT_DB
};

void serve_upload(struct uh_connection *conn, int event);

void serve_download(struct uh_connection *conn, int event);

static void default_handler(struct uh_connection *conn, int event)
{
    if (event != UH_EV_COMPLETE)
        return;

    conn->serve_file(conn);
}

static void signal_cb(struct ev_loop *loop, ev_signal *w, int revents)
{
    if (w->signum == SIGINT) {
        ev_break(loop, EVBREAK_ALL);
        uh_log_info("Normal quit\n");
    }
}


static void usage(const char *prog)
{
    fprintf(stderr, "Usage: %s [option]\n"
                    "          -a addr           # listen addr(default is 0.0.0.0)\n"
                    "          -p port           # listen port(default is 8080)\n"
                    "          --rpc dir         # rpc directory(default is .)\n"
                    "          --home dir        # document root(default is .)\n"
                    "          --index oui.html  # index page(default is oui.html)\n"
                    "          --db oh.db        # database file(default is ./oh.db)\n"
                    "          -w n              # worker process number (default automatically to available CPUs)\n"
                    "          -v                # verbose\n", prog);
    exit(1);
}

static struct option long_options[] = {
    {"rpc",   required_argument, NULL, LONG_OPT_RPC},
    {"home",  required_argument, NULL, LONG_OPT_HOME},
    {"index", required_argument, NULL, LONG_OPT_INDEX},
    {"db",    required_argument, NULL, LONG_OPT_DB}
};

int main(int argc, char **argv)
{
    struct ev_loop *loop;
    struct ev_signal sigint_watcher;
    struct uh_server *srv = NULL;
    const char *addr = "0.0.0.0";
    const char *rpc_dir = ".";
    const char *db = "oh.db";
    const char *home_dir = ".";
    const char *index_page = "oui.html";
    bool verbose = false;
    int port = 8080;
    int option_index;
    int ret = 0;
    int opt;

    while ((opt = getopt_long(argc, argv, "a:p:v", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'a':
            addr = optarg;
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 'v':
            verbose = true;
            break;
        case LONG_OPT_RPC:
            rpc_dir = optarg;
            break;
        case LONG_OPT_HOME:
            home_dir = optarg;
            break;
        case LONG_OPT_INDEX:
            index_page = optarg;
            break;
        case LONG_OPT_DB:
            db = optarg;
            break;
        default: /* '?' */
            usage(argv[0]);
        }
    }

    if (!verbose)
        uh_log_threshold(LOG_ERR);

    uh_log_info("libuhttpd version: %s\n", UHTTPD_VERSION_STRING);

    signal(SIGPIPE, SIG_IGN);

    db_init(db);

    session_init();

    loop = EV_DEFAULT;

    rpc_init(rpc_dir);

    srv = uh_server_new(loop, addr, port);
    if (!srv) {
        ret = 1;
        goto err;
    }

    srv->set_docroot(srv, home_dir);
    srv->set_index_page(srv, index_page);

    srv->set_default_handler(srv, default_handler);
    srv->add_path_handler(srv, "/rpc", serve_rpc);
    srv->add_path_handler(srv, "/upload", serve_upload);
    srv->add_path_handler(srv, "/download", serve_download);

    ev_signal_init(&sigint_watcher, signal_cb, SIGINT);
    ev_signal_start(loop, &sigint_watcher);

    ev_run(loop, 0);

err:
    if (srv) {
        srv->free(srv);
        free(srv);
    }

    session_deinit();

    rpc_deinit();

    ev_loop_destroy(loop);

    return ret;
}

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
    LONG_OPT_DB,
    LONG_OPT_LOCAL_AUTH,
    LONG_OPT_NO_AUTH_FILE
};

void serve_upload(struct uh_connection *conn, int event);
void serve_download(struct uh_connection *conn, int event);

static void signal_cb(struct ev_loop *loop, ev_signal *w, int revents)
{
    if (w->signum == SIGINT) {
        ev_break(loop, EVBREAK_ALL);
        log_info("Normal quit\n");
    }
}


static void usage(const char *prog)
{
    fprintf(stderr, "Usage: %s [option]\n"
                    "          -a [addr:]port    # Bind to specified address and port, multiple allowed\n"
#ifdef SSL_SUPPORT
                    "          -s [addr:]port    # Like -a but provide HTTPS on this port\n"
                    "          -C file           # server certificate file\n"
                    "          -K file           # server private key file\n"
#endif
                    "          --rpc dir         # rpc directory(default is .)\n"
                    "          --home dir        # document root(default is .)\n"
                    "          --index oui.html  # index page(default is oui.html)\n"
                    "          --db oh.db        # database file(default is ./oh.db)\n"
                    "          --local-auth      # local auth\n"
                    "          --no-auth-file    # no auth required methods in json file\n"
                    "          -w n              # rpc call workers number\n"
                    "          -v                # verbose\n", prog);
    exit(1);
}

static struct option long_options[] = {
    {"rpc",   required_argument, NULL, LONG_OPT_RPC},
    {"home",  required_argument, NULL, LONG_OPT_HOME},
    {"index", required_argument, NULL, LONG_OPT_INDEX},
    {"db",    required_argument, NULL, LONG_OPT_DB},
    {"local-auth", no_argument, NULL, LONG_OPT_LOCAL_AUTH},
    {"no-auth-file", required_argument, NULL, LONG_OPT_NO_AUTH_FILE}
};

int main(int argc, char **argv)
{
    struct ev_loop *loop = EV_DEFAULT;
    struct ev_signal sigint_watcher;
    const char *shortopts = "a:C:K:w:x:v";
    int verbose = 0;
    struct uh_server *srv = NULL;
    const char *rpc_dir = ".";
    const char *db = "oh.db";
    const char *home_dir = ".";
    const char *index_page = "oui.html";
    const char *cgi_prefix = "^/cgi-bin/";
    const char *no_auth_file = NULL;
    bool local_auth = false;
#ifdef SSL_SUPPORT
    const char *cert = NULL;
    const char *key = NULL;
#endif
    int option_index;
    int nworker = -1;
    int ret = 0;
    int opt;

    srv = uh_server_new(loop);
    if (!srv)
        return -1;

#ifdef SSL_SUPPORT
    shortopts = "a:s:C:K:w:x:v";
#endif

    while ((opt = getopt_long(argc, argv, shortopts, long_options, &option_index)) != -1) {
        switch (opt) {
        case 'a':
            if (srv->listen(srv, optarg, false) < 1)
                goto srv_err;
            break;
#ifdef SSL_SUPPORT
        case 's':
            if (srv->listen(srv, optarg, true) < 1)
                goto srv_err;
            break;
        case 'C':
            cert = optarg;
            break;
        case 'K':
            key = optarg;
            break;
#endif
        case 'w':
            nworker = atoi(optarg);
            break;
        case 'x':
            cgi_prefix = optarg;
            break;
        case 'v':
            if (!verbose) {
                verbose++;
                log_level(LOG_INFO);
            } else {
                log_level(LOG_DEBUG);
            }
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
        case LONG_OPT_LOCAL_AUTH:
            local_auth = true;
            break;
        case LONG_OPT_NO_AUTH_FILE:
            no_auth_file = optarg;
            break;
        default: /* '?' */
            usage(argv[0]);
        }
    }

    log_info("libuhttpd version: %s\n", UHTTPD_VERSION_STRING);

#ifdef SSL_SUPPORT
    if (cert && key) {
        if (srv->ssl_init(srv, cert, key) < 0)
            goto srv_err;
    }
#endif

    signal(SIGPIPE, SIG_IGN);

    db_init(db);

    if (rpc_init(loop, rpc_dir, local_auth, no_auth_file, nworker) < 0)
        goto rpc_err;

    srv->set_docroot(srv, home_dir);
    srv->set_index_page(srv, index_page);

    /* use built-in handlers from libuhttpd */
    srv->set_default_handler(srv, file_handler);
    srv->add_path_handler(srv, cgi_prefix, cgi_handler);

    srv->add_path_handler(srv, "^/rpc$", serve_rpc);
    srv->add_path_handler(srv, "^/upload$", serve_upload);
    srv->add_path_handler(srv, "^/download$", serve_download);

    ev_signal_init(&sigint_watcher, signal_cb, SIGINT);
    ev_signal_start(loop, &sigint_watcher);

    ev_run(loop, 0);

rpc_err:
    session_deinit();

    rpc_deinit(loop);

srv_err:
    if (srv) {
        srv->free(srv);
        free(srv);
    }

    ev_loop_destroy(loop);

    return ret;
}

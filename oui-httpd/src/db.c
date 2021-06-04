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
#include <stdio.h>
#include <errno.h>

#include "db.h"

static const char *db_path;

static struct sqlite3 *db_open()
{
    struct sqlite3 *db;

    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        log_err("%s\n", db ? sqlite3_errmsg(db) : "Open DB fail");
        sqlite3_close(db);
        return NULL;
    }

    return db;
}

void db_init(const char *path)
{
    char sql[128];

    db_path = path;

    sprintf(sql, "CREATE TABLE IF NOT EXISTS account(username TEXT PRIMARY KEY NOT NULL, password TEXT NOT NULL, acl TEXT NOT NULL)");
    db_exec(sql);
}

int db_exec(const char *sql)
{
    struct sqlite3 *db;
    char *errmsg;
    int ret = 0;

    db = db_open();
    if (!db)
        return -1;

    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK) {
        log_err("sqlite3_exec: %s\n", errmsg);
        sqlite3_free(errmsg);
        ret = -1;
    }

    sqlite3_close(db);

    return ret;
}

int db_query(const char *sql, int (*cb)(void *data, int count, char **value, char **name), void *data)
{
    struct sqlite3 *db;
    char *errmsg;
    int ret = 0;

    db = db_open();
    if (!db)
        return -1;

    ret = sqlite3_exec(db, sql, cb, data, &errmsg);
    if (ret != SQLITE_OK && ret != SQLITE_ABORT){
        log_err("sqlite3_exec: %s\n", errmsg);
        sqlite3_free(errmsg);
        ret = -1;
    }

    sqlite3_close(db);

    return ret;
}


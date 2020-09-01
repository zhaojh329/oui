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

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

static int file_is_executable(const char *name)
{
    struct stat s;
    return (!access(name, X_OK) && !stat(name, &s) && S_ISREG(s.st_mode));
}

static char *last_char_is(const char *s, int c)
{
    if (s && *s) {
        size_t sz = strlen(s) - 1;
        s += sz;
        if ((unsigned char) *s == c)
            return (char *) s;
    }
    return NULL;
}

static char *concat_path_file(const char *path, const char *filename)
{
    char *strp;
    char *lc;


    if (!path)
        path = "";
    lc = last_char_is(path, '/');
    while (*filename == '/')
        filename++;
    if (asprintf(&strp, "%s%s%s", path, (lc == NULL ? "/" : ""), filename) < 0)
        return NULL;
    return strp;
}

static char *find_executable(const char *filename, char **path)
{
    char *p, *n;

    p = *path;
    while (p) {
        int ex;

        n = strchr(p, ':');
        if (n) *n = '\0';
        p = concat_path_file(p[0] ? p : ".", filename);
        if (!p)
            break;
        ex = file_is_executable(p);
        if (n) *n++ = ':';
        if (ex) {
            *path = n;
            return p;
        }
        free(p);
        p = n;
    } /* on loop exit p == NULL */
    return p;
}

int which(const char *prog)
{
    char buf[] = "/sbin:/usr/sbin:/bin:/usr/bin";
    char *env_path;
    int missing = 1;

    env_path = getenv("PATH");
    if (!env_path)
        env_path = buf;

    /* If file contains a slash don't use PATH */
    if (strchr(prog, '/')) {
        if (file_is_executable(prog))
            missing = 0;
    } else {
        char *path;
        char *p;

        path = env_path;

        while ((p = find_executable(prog, &path)) != NULL) {
            missing = 0;
            free(p);
            break;
        }
    }
    return missing;
}
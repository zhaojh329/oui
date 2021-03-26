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

#include <netinet/in.h>
#include <string.h>

#include "utils.h"

bool is_loopback_addr(const struct sockaddr *addr)
{
    if (addr->sa_family == AF_INET) {
        struct sockaddr_in *sin = (struct sockaddr_in *)addr;
        return sin->sin_addr.s_addr == htonl(INADDR_LOOPBACK);
    } else {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)addr;
        return !memcmp(&sin6->sin6_addr, &in6addr_loopback, sizeof(in6addr_loopback));
    }
}

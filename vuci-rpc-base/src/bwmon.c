/*
 * rpcd - UBUS RPC server
 *
 *   Copyright (C) 2017 Jianhui Zhao <jianhuizhao329@gmail.com>
 *   Copyright (C) 2013 Jo-Philipp Wich <jow@openwrt.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <net/if.h>
#include <libubus.h>
#include <libubox/avl.h>
#include <libubox/avl-cmp.h>
#include <rpcd/plugin.h>

#define RPC_BWMON_HISTORY			120

enum {
	RX_BYTES = 0,
	TX_BYTES = 1,
	RX_PACKETS = 2,
	TX_PACKETS = 3
};

const char *types[] = {
	"rx_bytes",
	"tx_bytes",
	"rx_packets",
	"tx_packets"
};

struct rpc_bwmon_device {
	struct avl_node avl;
	char ifname[IF_NAMESIZE];
	int pos;
	uint64_t prev[4];
	uint32_t values[4][RPC_BWMON_HISTORY];
};

static struct blob_buf buf;
static struct avl_tree devices;

enum {
	RPC_STATS_DEVICE,
	__RPC_STATS_MAX,
};

static const struct blobmsg_policy rpc_stats_policy[__RPC_STATS_MAX] = {
	[RPC_STATS_DEVICE] = { .name = "device", .type = BLOBMSG_TYPE_STRING },
};

static int rpc_bwmon_devices(struct ubus_context *ctx, struct ubus_object *obj,
                  struct ubus_request_data *req, const char *method,
                  struct blob_attr *msg)
{
	void *c;
	struct rpc_bwmon_device *dev;

	blob_buf_init(&buf, 0);
	c = blobmsg_open_array(&buf, "devices");

	avl_for_each_element(&devices, dev, avl)
		blobmsg_add_string(&buf, NULL, dev->ifname);

	blobmsg_close_array(&buf, c);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}


static void rpc_bwmon_dump_stats(struct rpc_bwmon_device *dev)
{
	void *c;
	int i, j;

	for (i = 0; i < 4; i++) {
		c = blobmsg_open_array(&buf, types[i]);

		for (j = 0; j < RPC_BWMON_HISTORY; j++)
			blobmsg_add_u32(&buf, NULL,
			                dev->values[i][(dev->pos + j) % RPC_BWMON_HISTORY]);

		blobmsg_close_array(&buf, c);
	}
}

static int rpc_bwmon_stats(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
	void *c, *d;
	struct rpc_bwmon_device *dev;
	struct blob_attr *tb[__RPC_STATS_MAX];

	blobmsg_parse(rpc_stats_policy, __RPC_STATS_MAX, tb,
	              blob_data(msg), blob_len(msg));

	blob_buf_init(&buf, 0);

	if (tb[RPC_STATS_DEVICE]) {
		dev = avl_find_element(&devices, blobmsg_get_string(tb[RPC_STATS_DEVICE]), dev, avl);

		if (!dev)
			return UBUS_STATUS_NOT_FOUND;

		c = blobmsg_open_table(&buf, "statistics");
		rpc_bwmon_dump_stats(dev);
		blobmsg_close_table(&buf, c);

		ubus_send_reply(ctx, req, buf.head);
		return 0;
	}

	c = blobmsg_open_table(&buf, "statistics");

	avl_for_each_element(&devices, dev, avl) {
		d = blobmsg_open_table(&buf, dev->ifname);
		rpc_bwmon_dump_stats(dev);
		blobmsg_close_table(&buf, d);
	}

	blobmsg_close_table(&buf, c);

	ubus_send_reply(ctx, req, buf.head);
	return 0;
}


static uint64_t read_int(const char *ifname, const char *name)
{
	FILE *file;
	uint64_t val = 0;
	char buf[32] = { }, path[PATH_MAX] = { };

	snprintf(path, sizeof(path) - 1, "/sys/class/net/%s/%s", ifname, name);

	if ((file = fopen(path, "r")) != NULL) {
		if (fread(buf, 1, sizeof(buf) - 1, file) > 0)
			val = strtoull(buf, NULL, 0);

		fclose(file);
	}

	return val;
}

static struct rpc_bwmon_device *get_device(const char *ifname, bool create)
{
	struct rpc_bwmon_device *dev;

	dev = avl_find_element(&devices, ifname, dev, avl);

	if (!dev && create) {
		dev = calloc(1, sizeof(*dev));

		if (!dev)
			return NULL;

		dev->pos = -1;
		dev->avl.key = strcpy(dev->ifname, ifname);

		avl_insert(&devices, &dev->avl);
	}

	return dev;
}

static void put_value(struct rpc_bwmon_device *dev, int type, uint64_t value)
{
	if (dev->pos >= 0)
		dev->values[type][dev->pos] = (uint32_t)(value - dev->prev[type]);

	dev->prev[type] = value;
}

static void rpc_bwmon_collect(struct uloop_timeout *t)
{
	DIR *dir;
	bool up;
	struct dirent *e;
	struct rpc_bwmon_device *dev;

	if ((dir = opendir("/sys/class/net"))) {
		while ((e = readdir(dir)) != NULL) {
			if (!strcmp(e->d_name, "lo"))
				continue;

			up = read_int(e->d_name, "flags") & 1;
			dev = get_device(e->d_name, up);

			if (!dev)
				continue;

			put_value(dev, RX_BYTES, read_int(e->d_name, "statistics/rx_bytes"));

			put_value(dev, TX_BYTES, read_int(e->d_name, "statistics/tx_bytes"));

			put_value(dev, RX_PACKETS, read_int(e->d_name, "statistics/rx_packets"));

			put_value(dev, TX_PACKETS, read_int(e->d_name, "statistics/tx_packets"));

			dev->pos = (dev->pos + 1) % RPC_BWMON_HISTORY;
		}

		closedir(dir);
	}

	uloop_timeout_set(t, 1000);
}


static int rpc_bwmon_api_init(const struct rpc_daemon_ops *o, struct ubus_context *ctx)
{
	static const struct ubus_method bwmon_methods[] = {
		UBUS_METHOD_NOARG("devices", rpc_bwmon_devices),
		UBUS_METHOD("statistics",    rpc_bwmon_stats, rpc_stats_policy)
	};

	static struct ubus_object_type bwmon_type =
		UBUS_OBJECT_TYPE("vuci-rpc-bwmon", bwmon_methods);

	static struct ubus_object bwmon_obj = {
		.name = "vuci.network.bwmon",
		.type = &bwmon_type,
		.methods = bwmon_methods,
		.n_methods = ARRAY_SIZE(bwmon_methods),
	};

	static struct uloop_timeout t = {
		.cb = rpc_bwmon_collect
	};

	avl_init(&devices, avl_strcmp, false, NULL);

	uloop_timeout_set(&t, 1000);

	return ubus_add_object(ctx, &bwmon_obj);
}

struct rpc_plugin rpc_plugin = {
	.init = rpc_bwmon_api_init
};

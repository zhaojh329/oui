/*
 *	Copyright (C) 2019 jianhui zhao <zhaojh329@gmail.com>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 */

#ifndef __SUBNET_
#define __SUBNET_

#include <linux/ip.h>
#include <linux/netfilter_ipv4.h>

struct subnet {
	struct list_head list;
    __be32 addr;
	__be32 mask;
};

bool match_subnet(__be32 addr);
int subnet_init(struct proc_dir_entry *proc);
void subnet_free(void);

#endif

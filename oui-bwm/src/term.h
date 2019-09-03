/*
 *	Copyright (C) 2019 jianhui zhao <zhaojh329@gmail.com>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 */

#ifndef __TERM_
#define __TERM_

#include <linux/if_ether.h>

struct terminal {
	struct hlist_node hlist;
	/* write-heavy members should not affect lookups */
	unsigned long updated ____cacheline_aligned_in_smp;
	struct rcu_head rcu;
	u8 mac[ETH_ALEN];
	__be32 ip;
	u64 tx;
	u64 rx;
};

struct terminal *find_term(const u8 *mac, bool creat);
void update_term(struct terminal *term, __be32 ip, u32 rx, u32 tx);
void set_term_ttl(unsigned long t);
unsigned long get_term_ttl(void);

int term_init(struct proc_dir_entry *proc);
void term_free(void);

#endif

/*
 *  Copyright (C) 2019 jianhui zhao <zhaojh329@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/slab.h>
#include <linux/jhash.h>
#include <asm/unaligned.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/etherdevice.h>

#include "term.h"

#define TERM_HASH_SIZE              (1 << 8)

static struct kmem_cache *term_cache __read_mostly;
static struct hlist_head terms[TERM_HASH_SIZE];
static u32 hash_rnd __read_mostly;
struct delayed_work gc_work;
static unsigned long ttl;
static DEFINE_RWLOCK(lock);

static inline int term_mac_hash(const u8 *mac)
{
    /* use 1 byte of OUI and 3 bytes of NIC */
    u32 key = get_unaligned((u32 *)(mac + 2));
    return jhash_1word(key, hash_rnd) & (TERM_HASH_SIZE - 1);
}

static struct terminal *___find_term(const u8 *mac)
{
    u32 hash = term_mac_hash(mac);
    struct terminal *term;

    read_lock(&lock);
    hlist_for_each_entry(term, &terms[hash], hlist) {
        if (ether_addr_equal(term->mac, mac)) {
            read_unlock(&lock);
            return term;
        }
    }
    read_unlock(&lock);
    return NULL;
}

static struct terminal *create_term(const u8 *mac)
{
    u32 hash = term_mac_hash(mac);
    struct terminal *term;

    term = kmem_cache_zalloc(term_cache, GFP_ATOMIC);
    if (!term)
        return NULL;

    term->updated = jiffies;
    memcpy(term->mac, mac, ETH_ALEN);

    write_lock(&lock);
    hlist_add_head(&term->hlist, &terms[hash]);
    write_unlock(&lock);

    return term;
}

static void term_delete(struct terminal *term)
{
    hlist_del_init(&term->hlist);
    kmem_cache_free(term_cache, term);
}

static void term_flush(void)
{
    int i;

    write_lock(&lock);
    for (i = 0; i < TERM_HASH_SIZE; i++) {
        struct terminal *term;
        struct hlist_node *n;

        hlist_for_each_entry_safe(term, n, &terms[i], hlist) {
            term_delete(term);
        }
    }
    write_unlock(&lock);
}

static int proc_show(struct seq_file *s, void *v)
{
    int i;

    seq_printf(s, "%-17s  %-16s  %-16s  %-16s\n", "MAC", "IP", "Rx", "Tx");

    read_lock(&lock);
    for (i = 0; i < TERM_HASH_SIZE; i++) {
        struct terminal *term;

        hlist_for_each_entry_rcu(term, &terms[i], hlist) {
            seq_printf(s, "%pM  %-16pI4  %-16llu  %-16llu\n",
                term->mac, &(term->ip), term->rx, term->tx);
        }
    }
    read_unlock(&lock);

    return 0;
}

static ssize_t proc_write(struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
    char data[128] = "";
    char *e;

    if (size > sizeof(data) - 1)
        return -EINVAL;

    if (copy_from_user(data, buf, sizeof(data) - 1))
        return -EFAULT;

    e = strchr(data, '\n');
    if (e)
        *e = 0;

    if (data[0] == 'c') {
        term_flush();
        return size;
    }

    return size;
}

static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}

const static struct file_operations proc_ops = {
    .owner      = THIS_MODULE,
    .open       = proc_open,
    .read       = seq_read,
    .write      = proc_write,
    .llseek     = seq_lseek,
    .release    = single_release
};

static void term_cleanup(struct work_struct *work)
{
    unsigned long work_delay = ttl;
    unsigned long now = jiffies;
    int i;

    write_lock(&lock);
    for (i = 0; i < TERM_HASH_SIZE; i++) {
        struct terminal *term;
        struct hlist_node *n;

        hlist_for_each_entry_safe(term, n, &terms[i], hlist) {
            unsigned long this_timer = term->updated + ttl;

            if (time_after(this_timer, now))
                work_delay = min(work_delay, this_timer - now);
            else
                term_delete(term);
        }
    }
    write_unlock(&lock);

    /* Cleanup minimum 10 milliseconds apart */
    work_delay = max_t(unsigned long, work_delay, msecs_to_jiffies(10));
    mod_delayed_work(system_long_wq, &gc_work, work_delay);
}

struct terminal *find_term(const u8 *mac, bool creat)
{
    struct terminal *term = ___find_term(mac);

    if (term || !creat)
        return term;

    return create_term(mac);
}

void update_term(struct terminal *term, __be32 ip, u32 rx, u32 tx)
{
    write_lock(&lock);
    term->ip = ip;
    term->rx += rx;
    term->tx += tx;
    term->updated = jiffies;
    write_unlock(&lock);
}

void set_term_ttl(unsigned long t)
{
    if (t > 0)
        ttl = t * HZ;

    mod_delayed_work(system_long_wq, &gc_work, 0);
}

unsigned long get_term_ttl(void)
{
    return ttl / HZ;
}

int term_init(struct proc_dir_entry *proc)
{
    int i;

    term_cache = kmem_cache_create("term_cache", sizeof(struct terminal), 0, 0, NULL);
    if (!term_cache)
        return -ENOMEM;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
    proc_create_single("term", 0644, proc, proc_show);
#else
    proc_create("term", 0644, proc, &proc_ops);
#endif

    for (i = 0; i < TERM_HASH_SIZE; i++) {
        INIT_HLIST_HEAD(&terms[i]);
    }

    get_random_bytes(&hash_rnd, sizeof(hash_rnd));

    INIT_DELAYED_WORK(&gc_work, term_cleanup);

    set_term_ttl(30);

    return 0;
}

void term_free(void)
{
    cancel_delayed_work_sync(&gc_work);

    term_flush();

    kmem_cache_destroy(term_cache);
}

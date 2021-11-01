/*
 *  Copyright (C) 2019 jianhui zhao <zhaojh329@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/inetdevice.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/inet.h>

#include "subnet.h"

static LIST_HEAD(subnets);
static DEFINE_SPINLOCK(lock);

static void subnet_rcu_free(struct rcu_head *head)
{
    struct subnet *n = container_of(head, struct subnet, rcu);

    kfree(n);
}

static void clear_subnet(void)
{
    struct subnet *pos, *n;

    spin_lock(&lock);
    list_for_each_entry_safe(pos, n, &subnets, list) {
        list_del_rcu(&pos->list);
        call_rcu(&pos->rcu, subnet_rcu_free);
    }
    spin_unlock(&lock);
}

static bool subnet_exist(__be32 addr, __be32 mask)
{
    struct subnet *net;

    list_for_each_entry_rcu(net, &subnets, list) {
        if (net->addr == addr && net->mask == mask)
            return true;
    }

    return false;
}

static void add_subnet(__be32 addr, __be32 mask)
{
    struct subnet *net;

    if (subnet_exist(addr, mask))
        return;

    net = kmalloc(sizeof(struct subnet), GFP_KERNEL);

    net->addr = addr;
    net->mask = mask;

    spin_lock(&lock);
    list_add_tail_rcu(&net->list, &subnets);
    spin_unlock(&lock);
}

static int proc_show(struct seq_file *s, void *v)
{
    struct subnet *net;

    list_for_each_entry(net, &subnets, list) {
        seq_printf(s, "%pI4/%d\n", &net->addr, inet_mask_len(net->mask));
    }

    return 0;
}

static ssize_t proc_write(struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
    char addr[INET_ADDRSTRLEN + 4] = "";
    char *mask, *endp;
    __be32 ip, m = 0;

    if (size > sizeof(addr) - 1)
        return -EINVAL;

    if (copy_from_user(addr, buf, sizeof(addr) - 1))
        return -EFAULT;

    if (addr[0] == 'c') {
        clear_subnet();
        return size;
    }

    mask = strchr(addr, '/');
    if (mask)
        *mask++ = 0;

    if (!in4_pton(addr, strlen(addr), (u8 *)&ip, '\0', NULL)) {
        pr_err("Invalid ip addr\n");
        return -EINVAL;
    }

    if (mask) {
        m = simple_strtoul(mask, &endp, 10);
        if (endp == mask || m > 32)
            return -EINVAL;
    }

    add_subnet(ip, inet_make_mask(m));

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

int subnet_init(struct proc_dir_entry *proc)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
    proc_create_single("subnet", 0644, proc, proc_show);
#else
    proc_create("subnet", 0644, proc, &proc_ops);
#endif

    return 0;
}

void subnet_free(void)
{
    clear_subnet();
}

bool match_subnet(__be32 addr)
{
    struct subnet *net;

    list_for_each_entry_rcu(net, &subnets, list) {
        if (!((addr ^ net->addr) & net->mask))
            return true;
    }

    return false;
}

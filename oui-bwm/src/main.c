/*
 *  Copyright (C) 2019 jianhui zhao <zhaojh329@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <net/arp.h>
#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include "term.h"
#include "subnet.h"

static int proc_show(struct seq_file *s, void *v)
{
    seq_printf(s, "ttl: %ld\n", get_term_ttl());

    return 0;
}

static ssize_t proc_write(struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
    char conf[128] = "";
    char *endp, *value;

    if (size > sizeof(conf) - 1)
        return -EINVAL;

    if (copy_from_user(conf, buf, sizeof(conf) - 1))
        return -EFAULT;

    value = strchr(conf, '=');
    if (!value)
        return -EINVAL;
    *value++ = 0;

    if (!strcmp(conf, "ttl")) {
        unsigned long t = simple_strtoul(value, &endp, 10);
        if (endp == value)
            return -EINVAL;
        set_term_ttl(t);
    } else {
        return -EINVAL;
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

static u32 oui_bwm_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct ethhdr *ehdr = eth_hdr(skb);
    struct iphdr *iph = ip_hdr(skb);
    struct terminal *term = NULL;
    __be32 saddr, daddr;
    u32 tx = 0, rx = 0, addr;

    saddr = iph->saddr;
    daddr = iph->daddr;

    /* local -> remote */
    if (match_subnet(saddr)) {
        term = find_term(ehdr->h_source, true);
        tx = skb->len;
        addr = saddr;
    }
    /* remote -> local */
    else if (match_subnet(daddr)) {
        struct neighbour *n = __ipv4_neigh_lookup_noref(state->out, daddr);
        if (n) {
            term = find_term(n->ha, true);
            rx = skb->len;
            addr = daddr;
        }
    }

    if (term)
        update_term(term, addr, rx, tx);

    return NF_ACCEPT;
}

static struct nf_hook_ops oui_bwm_ops[] __read_mostly = {
    {
        .hook       = oui_bwm_hook,
        .pf         = PF_INET,
        .hooknum    = NF_INET_FORWARD,
        .priority   = NF_IP_PRI_LAST
    }
};

static int __init oui_bwm_init(void)
{
    struct proc_dir_entry *proc;
    int ret = 0;

    proc = proc_mkdir("oui-bwm", NULL);
    if (!proc) {
        pr_err("can't create dir /proc/oui-bwm/\n");
        return -ENODEV;;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
    proc_create_single("config", 0644, proc, proc_show);
#else
    proc_create("config", 0644, proc, &proc_ops);
#endif

    subnet_init(proc);

    ret = term_init(proc);
    if (ret)
        goto subnet_free;

#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 12, 14)
    ret = nf_register_net_hooks(&init_net, oui_bwm_ops, ARRAY_SIZE(oui_bwm_ops));
#else
    ret = nf_register_hooks(oui_bwm_ops, ARRAY_SIZE(oui_bwm_ops));
#endif
    if (ret < 0) {
        pr_err("can't register hook\n");
        goto term_free;
    }

    pr_info("oui-bwm: (C) 2019 jianhui zhao <zhaojh329@gmail.com>\n");

    return 0;

term_free:
    term_free();
subnet_free:
    subnet_free();

    remove_proc_subtree("oui-bwm", NULL);
    return ret;
}

static void __exit oui_bwm_exit(void)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 12, 14)
    nf_unregister_net_hooks(&init_net, oui_bwm_ops, ARRAY_SIZE(oui_bwm_ops));
#else
    nf_unregister_hooks(oui_bwm_ops, ARRAY_SIZE(oui_bwm_ops));
#endif

    term_free();
    subnet_free();

    remove_proc_subtree("oui-bwm", NULL);
}

module_init(oui_bwm_init);
module_exit(oui_bwm_exit);

MODULE_AUTHOR("jianhui zhao <zhaojh329@gmail.com>");
MODULE_LICENSE("GPL");

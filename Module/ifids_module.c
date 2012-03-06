/*
 * Intranet Firwall and Intrusion Detection System (IFIDS)    
 * Copyright (C) 2012  Christopher King, Michael Davis, Sam Doerr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/
 *
 * For more information, contact Christopher King at ccking@smu.edu
 *
 */


#define NIPQUAD(addr) \
	    ((unsigned char *)&addr)[0], \
    ((unsigned char *)&addr)[1], \
    ((unsigned char *)&addr)[2], \
    ((unsigned char *)&addr)[3]

#define _KERNEL_
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/net.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <net/ip.h>
static struct nf_hook_ops netfilter_opts_in;
static struct nf_hook_ops netfilter_opts_out;

unsigned int main_hook(unsigned int hooknum,
			struct sk_buff *skb,
			const struct net_device *in,
			const struct net_device *out,
			int (*okfn)(struct sk_buff*))
{
	struct iphdr *iph = NULL;
	struct tcphdr *tcph = NULL;
	if ((skb != NULL) && skb->pkt_type == PACKET_HOST && skb->protocol == htons(ETH_P_IP))
	{
		iph = ip_hdr(skb);
		if (iph == NULL)
			printk( KERN_ALERT "iph was null\n");
		else
		{
			if (iph->protocol == IPPROTO_TCP)
			{
				tcph = tcp_hdr(skb);
				if (tcph == NULL)
					printk(KERN_ALERT "tcph was null\n");
				else
				{
					if (in) printk("In: %s\n", in->name);
					if (out) printk("Out: %s\n", out->name);
					printk(KERN_ALERT "Source IP: %u.%u.%u.%u\nDestination IP: %u.%u.%u.%u\nSource Port: %u\nDestination Port: %u\n\n\n", NIPQUAD(iph->saddr), NIPQUAD(iph->daddr), ntohs(tcph->source), ntohs(tcph->dest));
				}
			}
		}
	}
	return NF_ACCEPT;
}
static int __init init_this(void)
{
	printk("Starting module\n");
	netfilter_opts_in.hook = main_hook;
	netfilter_opts_in.pf = PF_INET;
	netfilter_opts_in.hooknum = NF_INET_PRE_ROUTING;
	netfilter_opts_in.priority = NF_IP_PRI_FIRST;
	netfilter_opts_out.hook = main_hook;
	netfilter_opts_out.pf = PF_INET;
	netfilter_opts_out.hooknum = NF_INET_POST_ROUTING;
	netfilter_opts_out.priority = NF_IP_PRI_FIRST;
	nf_register_hook(&netfilter_opts_in);
	nf_register_hook(&netfilter_opts_out);
	return 0;
}

static void __exit cleanup(void)
{
	printk("Stopping module\n");
	nf_unregister_hook(&netfilter_opts_in);
	nf_unregister_hook(&netfilter_opts_out);
}

module_init(init_this);
module_exit(cleanup);

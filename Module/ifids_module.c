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

// The NIPQUAD macro is simply to translate the sk_buff form of an IP address into human readable.
// This was apart of the Linux kernel in the 2.* series, but was taken out somewhere around v2.6
#define NIPQUAD(addr) \
	    ((unsigned char *)&addr)[0], \
    ((unsigned char *)&addr)[1], \
    ((unsigned char *)&addr)[2], \
    ((unsigned char *)&addr)[3]

#define _KERNEL_
#include <linux/kernel.h> // This is apart of the kernel, so we need this
#include <linux/module.h> // This is a module, so obviously this is required
#include <linux/netfilter.h> // Netfilter is what is being used to drop/accept packets
#include <linux/netfilter_ipv4.h> // ipv4 for Netfilter (maybe we will add ipv6 in the future?)
#include <linux/net.h> // This is used for sockets....do we use this ever?  Might want to look into it
#include <linux/if_ether.h> // This defines the different types of packets that we can get.  ETH_P_IP is defined here
#include <linux/if_packet.h> // This defines where the packet is directed.  PACKET_HOST is defined here
#include <linux/ip.h> // This lets us grab the IP header of the packet.  Function ip_hdr is defined here
#include <net/ip.h> // Provides many functions that can be used to manipulate the packets.  Not sure what is used in it currently though

// Struct of type nf_hook_ops that will correspond to incoming packets
static struct nf_hook_ops netfilter_opts_in;

// Struct of type nf_hook_ops that will correspond to outgoing packets
static struct nf_hook_ops netfilter_opts_out;

// This is the main function that will be called when an incoming or outgoing packet is caught
unsigned int main_hook(unsigned int hooknum,
			struct sk_buff *skb,
			const struct net_device *in,
			const struct net_device *out,
			int (*okfn)(struct sk_buff*))
{
	struct iphdr *iph = NULL; // struct pointer declaration to hold the ip header
	struct tcphdr *tcph = NULL; // struct pointer declaration to help the tcp header
	// If the packet isn't null, the packet is directed to us, and the protocol is an IP protocol
	if ((skb != NULL) && skb->pkt_type == PACKET_HOST && skb->protocol == htons(ETH_P_IP))
	{
		iph = ip_hdr(skb); // Get the IP header
		if (iph == NULL) // Make sure the IP header isn't NULL
			printk( KERN_ALERT "iph was null\n");
		else
		{
			if (iph->protocol == IPPROTO_TCP) // If the packet uses TCP
			{
				tcph = tcp_hdr(skb); // Get the TCP header
				if (tcph == NULL) // Make sure the TCP header isn't NULL
					printk(KERN_ALERT "tcph was null\n");
				else
				{
					if (in) printk("In: %s\n", in->name); // Print out interface name
					if (out) printk("Out: %s\n", out->name); // Print out interface name
					// Print out information regarding the packet into the kernel log (dmesg)
					printk(KERN_ALERT "Source IP: %u.%u.%u.%u\nDestination IP: %u.%u.%u.%u\nSource Port: %u\nDestination Port: %u\n\n\n", NIPQUAD(iph->saddr), NIPQUAD(iph->daddr), ntohs(tcph->source), ntohs(tcph->dest));
				}
			}
		}
	}
	return NF_ACCEPT; // Accept the packet no matter what...we aren't doing anything with it yet
}

// init_this is the function that is called when the module first loads
static int __init init_this(void)
{
	printk("Starting module\n");
	netfilter_opts_in.hook = main_hook; // Set main_hook function to be used as the function that will execute
	netfilter_opts_in.pf = PF_INET; // Tell the filter to only filter based on protocol family inet
	netfilter_opts_in.hooknum = NF_INET_PRE_ROUTING; // The filter should only catch PRE_ROUTING packets 
	netfilter_opts_in.priority = NF_IP_PRI_FIRST; // Its a high priority
	netfilter_opts_out.hook = main_hook; // Set main_hook function to be used as the function that will execute
	netfilter_opts_out.pf = PF_INET; // Tell the filter to only filter based on protocol family inet
	netfilter_opts_out.hooknum = NF_INET_POST_ROUTING; // The filter should only catch POST_ROUTING packets
	netfilter_opts_out.priority = NF_IP_PRI_FIRST; // Its a high priority
	nf_register_hook(&netfilter_opts_in); // Register the "in" netfilter filter with the kernel
	nf_register_hook(&netfilter_opts_out); // Reigster the "out" netfilter filter with the kernel
	return 0; // Return no error
}

// cleanup is called when the module is unloaded.  It unregisters the filters and cleans up the memory
static void __exit cleanup(void)
{
	printk("Stopping module\n");
	nf_unregister_hook(&netfilter_opts_in); // Unregisters the "in" netfilter filter from the kernel
	nf_unregister_hook(&netfilter_opts_out); // Unregisters the "out" netfilter filter from the kernel
}

module_init(init_this); // Tell the kernel to execute init_this function when module starts
module_exit(cleanup); // Tell the kernel to execute cleanup function when module exits


#include <linux/module.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/miscdevice.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/types.h>
#include <linux/file.h>
#include <linux/net.h>
#include "../include/packet.h"
#include "print.h"

static unsigned int ioctl_set_address = 0;

static int test_daddr(struct sk_buff* skb) {
  struct iphdr* iph = ip_hdr(skb);

  if (ioctl_set_address == 0) {
    print("ioctl set address must be called", DEBUG);
    return 0;
  }

  return iph->daddr == ioctl_set_address;
}

static u32 packet_hook(void* priv, struct sk_buff* sbuf, const struct nf_hook_state* state) {
  struct iphdr* iph;
  struct tcphdr* tcph;
  
  if (!sbuf)
    return NF_ACCEPT;

  iph = ip_hdr(sbuf);
  if (iph->protocol != IPPROTO_TCP) // protocol must be TCP
    return NF_ACCEPT;
  
  tcph = tcp_hdr(sbuf);
  if (!tcph->syn || tcph->ack) // Check if connection intialized
    return NF_ACCEPT;

  if (!test_daddr(sbuf)) // Check if destination address matches ioctl filter
    return NF_ACCEPT;


  pr_info("[TCP] tcp connection intiated from %pI4:%u\n", &iph->saddr, ntohs(tcph->source));
  pr_info("[IP HEADER]: DST IP:%pI4", &iph->daddr);
  pr_info("[IP HEADER]: SRC IP:%pI4", &iph->saddr);
  pr_info("[IP HEADER]: protocol:%u", iph->protocol);

  return NF_ACCEPT;
}

static struct nf_hook_ops packet_ops = {
  .hook      = packet_hook,
  .pf        = NFPROTO_IPV4,
  .hooknum   = NF_INET_LOCAL_OUT,
  .priority  = NF_IP_PRI_FIRST,
};

static long packet_ioctl(struct file* f, unsigned int cmd, unsigned long arg) {
  switch (cmd) {
    case MY_IOCTL_FILTER_ADDRESS: {
      if (copy_from_user(&ioctl_set_address, (unsigned int*)arg, sizeof(unsigned int)))
        return -EFAULT;
      pr_info("FIlter set to address: %pI4\n", &ioctl_set_address);
      break;
    }

    default:
      return -EINVAL;
  }

  return 0;
}

static const struct file_operations fops = {
  .owner          = THIS_MODULE,
  .unlocked_ioctl = packet_ioctl,
};

static struct miscdevice dev = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = "network_filter",
  .fops = &fops,
};

static int packet_init(void) {
  print("[packed_init] intialized kernel module", DEBUG);

  int register_net = nf_register_net_hook(&init_net, &packet_ops);
  if (register_net < 0) {
    print("Error registering network hook", ERROR);
    return -1;
  }

  misc_register(&dev);

  return 0;
}

static void packet_exit(void) {
  print("[packed_exit] exit kernel module", DEBUG);

  nf_unregister_net_hook(&init_net, &packet_ops);
  misc_deregister(&dev);
}

MODULE_DESCRIPTION("Displaying network packets in kernel space");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(packet_init);
module_exit(packet_exit);

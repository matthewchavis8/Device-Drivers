#include <linux/module.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/types.h>
#include <linux/net.h>
#include "print.h"

static u32 packet_hook(void* priv, struct sk_buff* sbuf, const struct nf_hook_state* state) {


  return NF_ACCEPT;
}

static struct nf_hook_ops packet_ops = {
  .hook      = packet_hook,
  .pf        = NFPROTO_IPV4,
  .hooknum   = NF_INET_LOCAL_OUT,
  .priority  = NF_IP_PRI_FIRST,
};


static int packet_init(void) {
  print("[packed_init] intialized kernel module", DEBUG);

  int register_net = nf_register_net_hook(&init_net, &packet_ops);
  if (register_net < 0) {
    print("Error registering network hook", ERROR);
    return -1;
  }

  return 0;
}

static void packet_exit(void) {
  print("[packed_exit] exit kernel module", DEBUG);

  nf_unregister_net_hook(&init_net, &packet_ops);
}

MODULE_DESCRIPTION("Displaying network packets in kernel space");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(packet_init);
module_exit(packet_exit);

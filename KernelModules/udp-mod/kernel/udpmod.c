#include "print.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/in.h>
#include <net/sock.h>

#define PORT 60001
#define MESSAGE "Hello from kernel space world!\n"

static struct socket* sock;

static int udpmod_init(void) {
  struct sockaddr_in addr;
  struct msghdr msg;
  struct kvec iov;
  int res;
  int len;

  res = sock_create_kern(&init_net, AF_INET, SOCK_DGRAM, IPPROTO_UDP, &sock);
  if (res < 0) {
    print("[UDP SOCKET] sock_create_kern failed: %d\n", res);
    return res;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  memset(&msg, 0, sizeof(msg));
  msg.msg_name        = &addr;
  msg.msg_namelen     = sizeof(addr);
  msg.msg_control     = NULL;
  msg.msg_controllen  = 0;
  msg.msg_flags       = 0;

  len = strlen(MESSAGE);
  iov.iov_base = (void*) MESSAGE;
  iov.iov_len  = len;

  res = kernel_sendmsg(sock, &msg, &iov, 1, len);
  if (res < 0) {
    print("[UDP SOCKET] kernel_sendmsg failed\n", ERROR);
    goto release;
  }

  pr_info("[UDP SOCKET] send %d bytes to 127.0.0.1:%d\n", res, PORT);
  return 0;

  release:
   sock_release(sock);
   sock = NULL;
   return res;
}

static void udpmod_exit(void) {
  if (sock) {
    sock_release(sock);
    sock = NULL;
  }
}

MODULE_DESCRIPTION("Creating a network socket");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(udpmod_init);
module_exit(udpmod_exit);

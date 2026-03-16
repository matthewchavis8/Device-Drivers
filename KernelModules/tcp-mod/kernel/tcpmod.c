#include "print.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/in.h>
#include <net/sock.h>

#define LISTEN_BACKLOG 5
#define PORT 60000

static struct socket* sock;
static struct socket* new_sock;

static int tcpmod_init(void) {
  struct sockaddr_in addr;
  struct sockaddr_in raddr;

  int res = sock_create_kern(&init_net, AF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
  if (res < 0) {
    print("[TCP_SOCKET] sock_create_kern failed \n", ERROR);
    return res;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  res = sock->ops->bind(sock, (struct sockaddr*) &addr, sizeof(addr));
  if (res < 0) {
    print("[TCP_SOCKET] bind failed \n", ERROR);
    goto socket_failed;
  }

  res = sock->ops->listen(sock, LISTEN_BACKLOG);
  if (res < 0) {
    print("[TCP_SOCKET] bind failed \n", ERROR);
    goto socket_failed;
  }

  res = sock_create_lite(AF_INET, SOCK_STREAM, IPPROTO_TCP, &new_sock);
  if (res < 0) {
    print("[TCP_SOCKET] sock create lite \n", ERROR);
    goto NEW_SOCKET_FAILED;
  }

  new_sock->ops = sock->ops;

  res = sock->ops->accept(sock, new_sock, 0, true);
  if (res < 0) {
    print("[TCP_SOCKET] accept failed\n", ERROR);
    goto NEW_SOCKET_FAILED;
  }

  res = new_sock->ops->getname(new_sock, (struct sockaddr*) &raddr, 1);
  if (res < 0) {
    print("[TCP_SOCKET] getname failed\n", ERROR);
    goto NEW_SOCKET_FAILED;
  }
  
  pr_info("[TCP_SOCKET] listening on 127.0.0.1:%d\n", PORT);
  return 0;

  socket_failed:
    sock_release(sock);
    sock = NULL;
    return res;
  NEW_SOCKET_FAILED:
    sock_release(new_sock);
    new_sock = NULL;
    return res;
}

static void tcpmod_exit(void) {
  if (sock) {
    sock_release(sock);
    sock = NULL;
  }
  
  if (new_sock) {
    sock_release(new_sock);
    new_sock = NULL;
  }
}

MODULE_DESCRIPTION("Creating a network socket");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(tcpmod_init);
module_exit(tcpmod_exit);

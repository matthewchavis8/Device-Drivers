#include "socket.h"
#include <arpa/inet.h>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket(): m_fd {::socket(AF_INET, SOCK_STREAM, 0)}
{
  if (m_fd < 0)
    throw std::runtime_error("[ERROR] Error intializing socket file descriptor\n");
}

void Socket::connect(std::string_view ip, uint16_t port) const {
  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port   = htons(port);

  // Convert IP address into a binary network representation and write into addr
  int addr_pton = ::inet_pton(AF_INET, ip.data(), &addr.sin_addr) != 1;
  if (addr_pton != 1)
    throw std::runtime_error("[ERROR] inet_pton() failed\n");

  int tcp_connect = ::connect(m_fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));
  if (tcp_connect < 1)
    throw std::runtime_error("[ERROR] connection failed\n");
}

void Socket::send(std::string_view msg) const {
  const ssize_t res = ::send(m_fd, msg.data(), msg.size(), 0);
  if (res < 0)
    throw std::runtime_error("[ERROR] failed to send message\n");
}

Socket::~Socket() {
  if (m_fd >= 0)
    ::close(m_fd);
}

#include "packetDriver.h"
#include "../../include/packet.h"
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>

PacketDriver::PacketDriver() {
  m_fd = ::open(PATH.data(), O_RDONLY); 
  if (m_fd < 0)
    throw std::runtime_error("[ERROR] Failed to open packet Driver\n");
}

void PacketDriver::filter_address(std::string_view ip) const {
  unsigned int addr;
  if (::inet_pton(AF_INET, ip.data(), &addr) != 1)     // Convert ip address -> binary network byte order
    throw std::runtime_error("[ERROR] Invalid IP ADDRESS\n");

  if (ioctl(m_fd, MY_IOCTL_FILTER_ADDRESS, &addr) < 0)
    throw std::runtime_error("[ERROR] Failed to filter address\n");
}

PacketDriver::~PacketDriver() {
  if (m_fd >= 0)
    ::close(m_fd);
}

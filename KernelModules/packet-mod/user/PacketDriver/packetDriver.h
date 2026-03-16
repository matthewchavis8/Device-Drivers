#ifndef __DRIVER_H__
#define __DRIVER_H__

#include <string_view>
#include <cstdint>
#include <fcntl.h>

class PacketDriver {
  private:
    int m_fd { -1 };
    static constexpr std::string_view PATH { "/dev/network_filter" };
  public:
    PacketDriver();

    void filter_address(std::string_view ip) const;

    PacketDriver(const PacketDriver&) = delete;
    PacketDriver& operator=(PacketDriver&) = delete;

    ~PacketDriver();
};


#endif // !__DRIVER_H__

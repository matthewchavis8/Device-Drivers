#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <string_view>
#include <cstdint>

class Socket {
  private:
    int m_fd { -1 };
  public:
    Socket();

    void connect(std::string_view ip, uint16_t port) const;
    
    void send(std::string_view msg) const;

    ~Socket();

    Socket& operator=(const Socket&) = delete;
    Socket(const Socket&) = delete;
};

#endif // !__SOCKET_H__

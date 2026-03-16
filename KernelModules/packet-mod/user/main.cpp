#include "socket/socket.h"
#include "PacketDriver/packetDriver.h"

int main() {
  // TEST PACKET FILTER
  PacketDriver driver;
  driver.filter_address("127.0.0.1");
  
  Socket socket1;
  socket1.connect("127.0.0.1", 8080);
  socket1.send("Hello from the user space world\n");
  
  Socket socket2;
  socket2.connect("127.0.0.2", 8080);
  socket2.send("THIS MESSAE SHOULD NOT BE SEEN\n");

  return 0;
}

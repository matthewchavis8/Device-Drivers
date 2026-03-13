#include "socket/socket.h"

int main() {
  Socket socket;
  socket.connect("127.0.0.1", 8080);
  socket.send("Hello from the user space world\n");

  return 0;
}

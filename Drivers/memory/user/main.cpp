#include <iostream>
#include <fcntl.h>

static const char* filePath = "/dev/memory_driver";
static inline void log(std::string_view msg) { std::cout << "[LOG] " << msg << '\n'; }

int main() {
  int fd = open(filePath, O_RDONLY);

  if (fd == 0)
    log("Failed to open memory driver");
}

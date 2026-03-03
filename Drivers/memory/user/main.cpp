#include <iostream>
#include <sys/mman.h>
#include <cstdint>
#include <fcntl.h>

static const char* filepath = "/dev/memory_driver";
static inline void log(std::string_view msg) { std::cout << "[log] " << msg << '\n'; }

int main() {
  int fd = open(filepath, O_RDWR);
  if (fd == -1)
    log("failed to open memory driver");

  // get kernel space memory
  void* mapped = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mapped == MAP_FAILED) {
    log("failed to get map");
    return 1;
  }

  volatile uint32_t* reg = (uint32_t*)mapped;
  uint32_t val = reg[8];
  std::cout << val << '\n';
}

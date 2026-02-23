#include "../include/deferred.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("[ERROR] zero args were passed\n");
    return 1;
  }

  int fd = open("/dev/timerDriver", O_RDONLY);
  if (fd < 0) {
    printf("Failed to open timer Driver\n");
    return 1;
  }


  switch (*argv[1]) {
    case 's': // Timer set
      if (argc != 3) {
        printf("[ERROR] timer set arg cnt was not met\n");
        return 1;
      }
      
      // attempt to set timer for Driver
      if (ioctl(fd, MY_IOCTL_TIMER_SET, argv[2]) < 0) {
        printf("[ERROR] timer_set failed\n");
        close(fd);
        return 1;
      }
      break;

    case 'c':
      if (argc != 2) {
        printf("[ERROR] timer cancel arg cnt not met\n");
        close(fd);
        return 1;
      }

      // attempt to cancel timer for Driver
      if (ioctl(fd, MY_IOCTL_TIMER_CANCEL) < 0) {
        printf("[ERROR] timer cancel failed\n");
        close(fd);
        return 1;
      }
      break;

    default:
      break;
  }

  close(fd);
  return 0;
}

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../include/deferred.h"
#include <fstream>

static const char* DRIVERPATH = "/dev/timerDriver";

class TimerDriver {
  private:
    int m_fd{};
  public:
    TimerDriver() { m_fd = open(DRIVERPATH, O_RDONLY); }

    void setTimer(int time) {
      if (ioctl(m_fd, MY_IOCTL_TIMER_SET, time) < 0) {
        std::cout << "[ERROR] Failed to set timer" << '\n';
        close(m_fd);
        return;
      }
    }
    
    void cancelTimer() {
      if (ioctl(m_fd, MY_IOCTL_TIMER_CANCEL) < 0) {
        std::cout << "[ERROR] timer failed to cancel" << '\n';
        close(m_fd);
        return;
      }
    }
};

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "[ERROR] zero args were passed" << '\n';
    return 1;
  }

  TimerDriver timer;
  char cmd = *argv[1];

  switch (cmd) {
    case 's': {
      if (argc != 3) {
        std::cout << "[ERROR] argc not met" << '\n';
        return 1;
      }

      int duration = std::stoi(std::to_string(*argv[2]));
      timer.setTimer(duration);
      break;
    }

    case 'c': {
      if (argc != 2) {
        std::cout << "[ERROR] argc not met" << '\n';
        return 1;
      }

      timer.cancelTimer();
      break;
    }

    default:
      std::cout << "[ERROR] unknown cmd" << cmd << '\n';
  }
  
  return 0;
}

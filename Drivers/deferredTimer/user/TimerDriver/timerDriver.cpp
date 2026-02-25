#include "timerDriver.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../../include/deferred.h"
#include <fstream>


void TimerDriver::setTimer(int time) {
  if (ioctl(m_fd, MY_IOCTL_TIMER_SET, time) < 0) {
    std::cout << "[ERROR] Failed to set timer" << '\n';
    close(m_fd);
    return;
  }
}
    
void TimerDriver::cancelTimer() {
  if (ioctl(m_fd, MY_IOCTL_TIMER_CANCEL) < 0) {
    std::cout << "[ERROR] timer failed to cancel" << '\n';
    close(m_fd);
    return;
  }
}

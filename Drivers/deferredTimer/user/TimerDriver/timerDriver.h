#ifndef __TIMERDRIVER_H__
#define __TIMERDRIVER_H__

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../../include/deferred.h"
#include <iostream>
#include <fstream>

static const char* DRIVERPATH = "/dev/timerDriver";
class TimerDriver {
  private:
    int m_fd{};
  public:
    TimerDriver() { m_fd = open(DRIVERPATH, O_RDONLY); }

    void setTimer(int time);
    
    void cancelTimer();
};

#endif // !TIMERDRIVER_H__

#include <iostream>
#include <string>
#include "TimerDriver/timerDriver.h"

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

      int duration = std::stoi(argv[2]);
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

    case 'a': {
      if (argc != 2) {
        std::cout << "[ERROR] argc not met" << '\n';
        return 1;
      }

      timer.allocTimer();
      break;
    }
    
    case 'p': {
      if (argc != 3) {
        std::cout << "[ERROR] argc not met" << '\n';
        return 1;
      }
      
      unsigned long pid = static_cast<unsigned long>(std::stoi(argv[2]));
      timer.monitorTimer(pid);
      break;
    }
                
    default:
      std::cout << "[ERROR] unknown cmd" << cmd << '\n';
  }
  
  return 0;
}

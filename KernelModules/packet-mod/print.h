#ifndef __PRINT_H__
#define __PRINT_H__
#include <linux/kernel.h>

enum LOG_LEVEL {
  DEBUG,
  ERROR,
};

static inline void print(const char* msg, enum LOG_LEVEL level) {
  switch (level) {
    case DEBUG:
      pr_info("[LOG]:%s\n", msg);
      break;
    case ERROR:
      pr_info("[ERROR]:%s\n", msg);
      break;
    default:
      pr_info("[???]: unknown log level\n");
      break;
  }
}

#endif // !__PRINT_H__

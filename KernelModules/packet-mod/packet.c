#include <linux/kernel.h>
#include <linux/module.h>

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

static int packet_init(void) {
  print("[packed_init] intialized kernel module", DEBUG);

  return 0;
}

static void packet_exit(void) {
  print("[packed_exit] exit kernel module", DEBUG);

}

MODULE_DESCRIPTION("Displaying network packets in kernel space");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(packet_init);
module_exit(packet_exit);

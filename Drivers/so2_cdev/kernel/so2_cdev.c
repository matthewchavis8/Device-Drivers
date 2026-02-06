#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#define MAJOR_NUMBER 42
#define MINOR_NUMBER 0
#define NUM_MINORS 0
#define MODULE_NAME  "so2_cdev"

static int so2_cdev_init(void) {
  int result;

  // Register device in the kernel
  result = register_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS, MODULE_NAME);

  if (result != 0) {
    pr_debug("[ERROR] failed to register device in chrdev region\n");
    return result;
  }
  pr_info("[LOG] registered %s char device in memory success\n", MODULE_NAME);

  return 0;
}

static void so2_cdev_exit(void) {
  unregister_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS);
  pr_info("[LOG] Unregistered %s from kernel space\n", MODULE_NAME);

}


MODULE_DESCRIPTION("Making my first char device driver");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(so2_cdev_init);
module_exit(so2_cdev_exit);

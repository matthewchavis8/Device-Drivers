
#include "linux/printk.h"
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is my first Module for hello world");

static int hello_init(void) {
  printk(KERN_ALERT " AM I alive\n %s\n", __FILE__);
  return 0;
}

static void hello_exit(void) {
  printk(KERN_ALERT "Goodbye world \n");
}

module_init(hello_init);
module_exit(hello_exit);

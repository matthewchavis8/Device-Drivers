
#include "linux/printk.h"
#include <linux/module.h>
#include <linux/init.h>


static int hello_init(void) {
  printk(KERN_ALERT "Hello WORLD I AM ALIVE\n");
  return 0;
}

static void hello_exit(void) {
  printk(KERN_ALERT "Goodbye world \n");
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is my first Module for hello world");

module_init(hello_init);
module_exit(hello_exit);

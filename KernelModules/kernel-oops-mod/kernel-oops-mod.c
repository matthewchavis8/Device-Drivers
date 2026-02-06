#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>

static int my_oops_init(void) {
  char* c = 0;

  printk("Before Intialization\n");
  *c = 'a';
  printk("After Intialization\n");

  return 0;
}

static void my_oops_exit(void) {
  printk("Module is exiting\n");
}

MODULE_DESCRIPTION("Module to practice debugging a kernel live using GDB");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(my_oops_init);
module_exit(my_oops_exit);

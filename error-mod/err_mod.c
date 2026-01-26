#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include "linux/printk.h"


static u32 n1 = 0;
static u32 n2 = 0;

static int err_init(void) {
	n1 = 1;
  n2 = 2;
	printk("n1 is %d, n2 is %d\n", n1, n2);

	return 0;
}

static void err_exit(void) {
	printk("sum is %d\n", n1 + n2);
}

MODULE_DESCRIPTION("Error module");
MODULE_AUTHOR("Kernel Hacker");
MODULE_LICENSE("GPL");
module_init(err_init);
module_exit(err_exit);

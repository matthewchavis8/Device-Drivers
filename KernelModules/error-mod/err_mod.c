/**
 * @file err_mod.c
 * @brief Simple module demonstrating variable initialization and arithmetic in kernel space
 * @author Matthew Chavis
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include "linux/printk.h"


static u32 n1 = 0;
static u32 n2 = 0;

/**
 * @brief Initialize the module and print two numbers
 * @return 0 on success
 */
static int err_init(void) {
	n1 = 1;
  n2 = 2;
	printk("n1 is %d, n2 is %d\n", n1, n2);

	return 0;
}

/**
 * @brief Clean up the module and print the sum of n1 and n2
 */
static void err_exit(void) {
	printk("sum is %d\n", n1 + n2);
}

MODULE_DESCRIPTION("Error module");
MODULE_AUTHOR("Kernel Hacker");
MODULE_LICENSE("GPL");
module_init(err_init);
module_exit(err_exit);

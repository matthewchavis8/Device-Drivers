/**
 * @file hello-1.c
 * @brief Minimal "Hello World" kernel module
 * @author Matthew Chavis
 */

#include "linux/printk.h"
#include <linux/module.h>
#include <linux/init.h>

/**
 * @brief Initialize the module and print a greeting
 * @return 0 on success
 */
static int hello_init(void) {
  printk(" AM I alive\n %s\n", __FILE__);
  return 0;
}

/**
 * @brief Clean up the module and print a farewell message
 */
static void hello_exit(void) {
  printk("Goodbye world \n");
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This is my first Module for hello world");
module_init(hello_init);
module_exit(hello_exit);

/**
 * @file kernel-oops-mod.c
 * @brief Module that intentionally triggers a kernel oops for GDB debugging practice
 * @author Matthew Chavis
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>

/**
 * @brief Initialize the module and trigger a kernel oops via NULL pointer dereference
 *
 * Intentionally writes to a NULL pointer to generate a kernel oops,
 * which can be caught and analyzed with GDB for debugging practice.
 *
 * @return 0 on success (never reached due to the intentional oops)
 */
static int my_oops_init(void) {
  char* c = 0;

  printk("Before Intialization\n");
  *c = 'a';
  printk("After Intialization\n");

  return 0;
}

/**
 * @brief Clean up the module
 */
static void my_oops_exit(void) {
  printk("Module is exiting\n");
}

MODULE_DESCRIPTION("Module to practice debugging a kernel live using GDB");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(my_oops_init);
module_exit(my_oops_exit);

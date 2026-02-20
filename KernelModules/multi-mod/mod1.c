/**
 * @file mod1.c
 * @brief Primary module in a multi-file build demonstrating cross-module function calls
 * @author Matthew Chavis
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/printk.h>

/** @brief External add function defined in mod2.c and linked at build time */
extern int add(int x, int y);

static int num1 = 5;
static int num2 = 5;

/**
 * @brief Initialize the module and print the two operands
 * @return 0 on success
 */
static int printNums(void) {
  printk("The Paramerter 1 is: %d\n The Parameter 2 is: %d\n", num1, num2);

  return 0;
}

/**
 * @brief Clean up the module by calling add() and printing the result
 */
static void exitModule(void) {
  int total = add(num1, num2);
  printk("The sum is: %d\n", total);
}

MODULE_DESCRIPTION("Built a multi module to practice linking multible srcs to one exe");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(printNums);
module_exit(exitModule);

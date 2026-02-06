#include <linux/module.h>
#include <linux/types.h>
#include <linux/printk.h>

extern int add(int x, int y);

static int num1 = 5;
static int num2 = 5;

static int printNums(void) {
  printk("The Paramerter 1 is: %d\n The Parameter 2 is: %d\n", num1, num2);
  
  return 0;
}

static void exitModule(void) {
  int total = add(num1, num2);
  printk("The sum is: %d\n", total);
}

MODULE_DESCRIPTION("Built a multi module to practice linking multible srcs to one exe");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(printNums);
module_exit(exitModule);

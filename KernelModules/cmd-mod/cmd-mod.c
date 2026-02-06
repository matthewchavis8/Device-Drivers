#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/printk.h>


static char *str = "the D:)";


static int cmd_init(void) {
	printk("Early bird gets %s\n", str);
	return 0;
}

static void cmd_exit(void) {
	printk("The Bird left the %s\n", str);
}

/*
 * First Param: param name
 * Second Param: param type
 * Third Param:  Param permissions
 */
module_param(str, charp, 0000);
MODULE_PARM_DESC(str, "A simple string");

MODULE_DESCRIPTION("Command-line args module");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(cmd_init);
module_exit(cmd_exit);

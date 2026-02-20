/**
 * @file cmd-mod.c
 * @brief Module demonstrating command-line parameter passing via module_param
 * @author Matthew Chavis
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/printk.h>


static char *str = "the D:)";

/**
 * @brief Initialize the module and print the string parameter
 * @return 0 on success
 */
static int cmd_init(void) {
	printk("Early bird gets %s\n", str);
	return 0;
}

/**
 * @brief Clean up the module and print the string parameter
 */
static void cmd_exit(void) {
	printk("The Bird left the %s\n", str);
}

/**
 * module_param(str, charp, 0000) - Register @str as a loadable module parameter.
 *   - First param:  parameter name
 *   - Second param: parameter type (charp = char pointer / string)
 *   - Third param:  sysfs permissions (0000 = not visible in sysfs)
 */
module_param(str, charp, 0000);
MODULE_PARM_DESC(str, "A simple string");

MODULE_DESCRIPTION("Command-line args module");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(cmd_init);
module_exit(cmd_exit);

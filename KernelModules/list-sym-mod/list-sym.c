/**
 * @file list-sym.c
 * @brief Test module that exercises exported symbols from list-sync.c
 * @author Matthew Chavis
 *
 * Depends on list-sync being loaded first. Uses the exported functions
 * task_info_add_for_current(), task_info_print_list(), and
 * task_info_remove_expired() to manipulate the shared list.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

/** @brief Adds current process hierarchy info to the shared list (from list-sync) */
extern void task_info_add_for_current(void);
/** @brief Removes expired entries from the shared list (from list-sync) */
extern void task_info_remove_expired(void);
/** @brief Prints all entries in the shared list with a label (from list-sync) */
extern void task_info_print_list(const char *msg);

/**
 * @brief Initialize the module by adding entries and printing the shared list
 * @return 0 on success
 */
static int list_test_init(void)
{
	task_info_add_for_current();
	task_info_print_list("after new addition");

	return 0;
}

/**
 * @brief Clean up the module by removing expired entries and printing the list
 */
static void list_test_exit(void)
{
	task_info_remove_expired();
	task_info_print_list("after removing expired");
}

MODULE_DESCRIPTION("Testing out the exported symbols from list-sync.c");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(list_test_init);
module_exit(list_test_exit);

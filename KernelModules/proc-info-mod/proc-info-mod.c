/**
 * @file proc-info-mod.c
 * @brief Module that iterates over all processes and prints their PID and name
 * @author Matthew Chavis
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/sched.h>

/**
 * @brief Initialize the module by printing the current process and iterating all tasks
 *
 * Prints the PID and command name of the current process, then walks the
 * entire task list using for_each_process() to print every process on the system.
 *
 * @return 0 on success
 */
static int proc_print_process(void) {
  struct task_struct* process;

  printk("The Process ID is: %d\n", current->pid);
  printk("The Process name is: %s\n", current->comm);

  for_each_process(process) {
    printk("The Process ID is: %d\n", process->pid);
    printk("The Process name is: %s\n", process->comm);
  }

  return 0;
}

/**
 * @brief Clean up the module and print the current process info on exit
 */
static void proc_exit(void) {
  printk("The Process ID is: %d\n", current->pid);
  printk("The Process name is: %s\n", current->comm);
}

MODULE_DESCRIPTION("Module for messing around with the task_struct");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(proc_print_process);
module_exit(proc_exit);

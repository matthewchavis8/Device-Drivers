#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/sched.h>

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

static void proc_exit(void) {
  printk("The Process ID is: %d\n", current->pid);
  printk("The Process name is: %s\n", current->comm);
}

MODULE_DESCRIPTION("Module for messing around with the task_struct");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(proc_print_process);
module_exit(proc_exit);

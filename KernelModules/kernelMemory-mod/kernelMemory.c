#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>

typedef struct taskInfo {
  pid_t pid;
  ulong timestamp;
} taskInfo;

// 4 TaskInfo objects
/*static taskInfo* t1;*/
/*static taskInfo* t2;*/
/*static taskInfo* t3;*/
/*static taskInfo* t4;*/

static taskInfo* alloc_task_info(pid_t pid) {
  taskInfo* t;

  // attempt to alloc
  t = kmalloc(sizeof(*t), GFP_KERNEL);
  if (t == NULL) {
    pr_debug("[ERROR] Failed to allocate memory for taskInfo\n");
    return NULL;
  }

  // Assigning members vars
  t->pid       = pid;
  t->timestamp = jiffies;

  return t;
}

static void print_task_info(taskInfo* tsk) {
  pr_info("tsk->id: %d\n", tsk->pid);
  pr_info("tsk->timestamp: %lx\n", tsk->timestamp);
}

static int memoryInit(void) {
  taskInfo* currProcess             = alloc_task_info(current->pid);
  taskInfo* parentProcess           = alloc_task_info(current->parent->pid);
  taskInfo* nextParentProcess       = alloc_task_info(current->parent->parent->pid);
  taskInfo* nextNextParentProcess   = alloc_task_info(current->parent->parent->parent->pid);

  print_task_info(currProcess);
  print_task_info(parentProcess);
  print_task_info(nextParentProcess);
  print_task_info(nextNextParentProcess);

  if (currProcess && parentProcess && nextNextParentProcess && nextParentProcess) {
    kfree(currProcess);
    kfree(parentProcess);
    kfree(nextParentProcess);
    kfree(nextNextParentProcess);
  }

  return 0;
}

static void memoryExit(void) {
  pr_info("[LOG] kernemMemory Module has been unloaded\n");
}

MODULE_DESCRIPTION("Module for messing around with putting threads to sleep");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(memoryInit);
module_exit(memoryExit);

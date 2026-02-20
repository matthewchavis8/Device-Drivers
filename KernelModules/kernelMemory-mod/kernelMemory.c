/**
 * @file kernelMemory.c
 * @brief Module demonstrating dynamic allocation of task info structs from the process hierarchy
 * @author Matthew Chavis
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>

/**
 * struct taskInfo - Stores a snapshot of a process's PID and timestamp
 * @pid:       Process ID captured at allocation time
 * @timestamp: Jiffies value when the struct was allocated
 */
typedef struct taskInfo {
  pid_t pid;
  ulong timestamp;
} taskInfo;

/**
 * @brief Allocate and initialize a taskInfo struct for a given PID
 * @param pid Process ID to store in the new taskInfo
 * @return Pointer to the allocated taskInfo, or NULL on allocation failure
 */
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

/**
 * @brief Print the PID and timestamp of a taskInfo struct
 * @param tsk Pointer to the taskInfo to print
 */
static void print_task_info(taskInfo* tsk) {
  pr_info("tsk->id: %d\n", tsk->pid);
  pr_info("tsk->timestamp: %lx\n", tsk->timestamp);
}

/**
 * @brief Initialize the module by allocating taskInfo for the current process hierarchy
 *
 * Allocates taskInfo structs for the current process, its parent,
 * grandparent, and great-grandparent, prints them, then frees the memory.
 *
 * @return 0 on success
 */
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

/**
 * @brief Clean up the module
 */
static void memoryExit(void) {
  pr_info("[LOG] kernemMemory Module has been unloaded\n");
}

MODULE_DESCRIPTION("Module for messing around with putting threads to sleep");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(memoryInit);
module_exit(memoryExit);

/**
 * @file list.c
 * @brief Module demonstrating kernel linked list operations with task info nodes
 * @author Matthew Chavis
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

/**
 * struct taskInfo - Node storing a process snapshot in a kernel linked list
 * @pid:       Process ID
 * @timestamp: Jiffies value when the node was allocated
 * @node:      Kernel list_head for linking into the list
 */
typedef struct taskInfo {
  pid_t pid;
  ulong timestamp;
  struct list_head node;
} taskInfo;

static taskInfo* alloc_task_info(pid_t pid);
static void print_task_info(taskInfo* tsk);
static void task_info_add_for_current(int pid);
static void task_info_print_list(void);
static void task_info_purge_list(void);

/** @brief Head of the kernel linked list */
static struct list_head head;

/**
 * @brief Initialize the module and populate the list with process hierarchy info
 *
 * Initializes the list head, then adds taskInfo nodes for the current process,
 * its parent, grandparent, and great-grandparent.
 *
 * @return 0 on success
 */
static int init_kernel_list(void) {
	INIT_LIST_HEAD(&head);
  task_info_add_for_current(current->pid);
  task_info_add_for_current(current->parent->pid);
  task_info_add_for_current(current->parent->parent->pid);
  task_info_add_for_current(current->parent->parent->parent->pid);

  pr_info("[LOG] Succesffuly added the task info into the linked list\n");

  return 0;
}

/**
 * @brief Clean up the module by printing and then purging the entire list
 */
static void exit_kernel_list(void) {
  task_info_print_list();
  task_info_purge_list();
  pr_info("[LOG] Successfully purged the linked list");
  pr_info("[LOG] unloading the kernel list module\n");
}

/**
 * @brief Walk the list and print each taskInfo entry
 */
static void task_info_print_list(void) {
  struct list_head* curr; // Current Node
  taskInfo* currTsk;      // Current Task

  list_for_each(curr, &head) {
    // Get the current Node and delete it from the linked list and free it
    currTsk = list_entry(curr, taskInfo, node);
    print_task_info(currTsk);
  }
}

/**
 * @brief Remove and free every entry from the list using list_for_each_safe
 */
static void task_info_purge_list(void) {
  struct list_head* curr; // Current Node
  struct list_head* tmp;  // tmp     Node
  taskInfo* currTsk;      // Current Task

  list_for_each_safe(curr, tmp, &head) {
    // Get the current Node and delete it from the linked list and free it
    currTsk = list_entry(curr, taskInfo, node);
    list_del(curr);
    kfree(currTsk);
  }
}

/**
 * @brief Allocate a taskInfo for a PID, initialize its list node, and add it to the list
 * @param pid Process ID to create a taskInfo entry for
 */
static void task_info_add_for_current(int pid) {
  // Alloc memory for the taskInfo
  taskInfo* tsk;
  tsk = alloc_task_info(pid);

  if (tsk == NULL) {
    pr_debug("[ERROR] failed to allocate memory for taskInfo\n");
    return;
  }

  INIT_LIST_HEAD(&tsk->node);
  list_add(&tsk->node, &head);
}

/**
 * @brief Allocate and initialize a taskInfo struct for a given PID
 * @param pid Process ID to store in the new taskInfo
 * @return Pointer to the allocated taskInfo, or NULL on failure
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


MODULE_DESCRIPTION("Module for messing around with Linux API for freeing and using heap memory in userspace");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(init_kernel_list);
module_exit(exit_kernel_list);

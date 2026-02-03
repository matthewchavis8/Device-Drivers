#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

typedef struct taskInfo {
  pid_t pid;
  ulong timestamp;
  struct list_head node;
} taskInfo;

// @brief Allocate via kmalloc
static taskInfo* alloc_task_info(pid_t pid);
// @brief print out the taskInfo
static void print_task_info(taskInfo* tsk);
// @brief appends task_info to the list
static void task_info_add_for_current(int pid);
// @brief print the list out
static void task_info_print_list(void);
// @brief delete all list entries
static void task_info_purge_list(void);

// Start of the Kernel List
static struct list_head head;

static int init_kernel_list(void) {
	INIT_LIST_HEAD(&head);
  task_info_add_for_current(current->pid);
  task_info_add_for_current(current->parent->pid);
  task_info_add_for_current(current->parent->parent->pid);
  task_info_add_for_current(current->parent->parent->parent->pid);

  pr_info("[LOG] Succesffuly added the task info into the linked list\n");

  return 0;
}

static void exit_kernel_list(void) {
  task_info_print_list();
  task_info_purge_list();
  pr_info("[LOG] Successfully purged the linked list");
  pr_info("[LOG] unloading the kernel list module\n");
}

static void task_info_print_list(void) {
  struct list_head* curr; // Current Node
  taskInfo* currTsk;      // Current Task

  list_for_each(curr, &head) {
    // Get the current Node and delete it from the linked list and free it
    currTsk = list_entry(curr, taskInfo, node);
    print_task_info(currTsk);
  }
}

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


MODULE_DESCRIPTION("Module for messing around with Linux API for freeing and using heap memory in userspace");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(init_kernel_list);
module_exit(exit_kernel_list);

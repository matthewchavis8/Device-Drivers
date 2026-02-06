#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/atomic.h>

typedef struct taskInfo {
  pid_t pid;
  atomic_t cnt;
  ulong timestamp;
  struct list_head node;
} taskInfo;

static struct list_head head;

static void print_task_info(taskInfo* t);

static void print_full_list(void) {
  taskInfo* t;
  struct list_head* curr;
  struct list_head* tmp;

  list_for_each_safe(tmp, curr, &head) {
    t = list_entry(tmp, taskInfo, node);
    print_task_info(t);
  }
}
static void print_task_info(taskInfo* t) {
    pr_info("[LOG] PID: %d\n", t->pid);
    pr_info("[LOG] CNT: %d\n", atomic_read(&t->cnt));
    pr_info("[LOG] timestamp: %lx\n", t->timestamp);
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

  // increment the atomic_t
  atomic_inc(&t->cnt);

  return t;
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

static taskInfo* task_info_find_pid(int pid) {
  struct list_head* tmp;
  struct list_head* curr;
  taskInfo* tsk;
  
  list_for_each_safe(curr, tmp, &head) {
    tsk = list_entry(curr, taskInfo, node);
    if (tsk->pid == pid)
      return tsk;
  }

  return NULL;
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

static void task_info_remove_expired(int pid) {
  struct list_head* tmp;
  struct list_head* curr;
  taskInfo* tsk;
  
  list_for_each_safe(curr, tmp, &head) {
    tsk = list_entry(curr, taskInfo, node);
    if (tsk->pid == pid && atomic_read(&tsk->cnt) < 5 && (jiffies - tsk->timestamp) > 3*HZ) {
      list_del(curr);
      kfree(tsk);
    }
  }
}



static int init_full_kernel_list(void) {
	INIT_LIST_HEAD(&head);
  task_info_add_for_current(current->pid);
  task_info_add_for_current(current->parent->pid);
  task_info_add_for_current(current->parent->parent->pid);
  task_info_add_for_current(current->parent->parent->parent->pid);

  task_info_find_pid(current->parent->pid);

  pr_info("[LOG] Succesffuly added the task info into the linked list\n");

  task_info_remove_expired(current->parent->parent->pid);

  return 0;
}

static void exit_full_kernel_list(void) {
  print_full_list();
  task_info_purge_list();
  pr_info("[LOG] Successfully purged the linked list");
  pr_info("[LOG] unloading the kernel list module\n");
}

MODULE_DESCRIPTION("Module messing around with full kernel list");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(init_full_kernel_list);
module_exit(exit_full_kernel_list);

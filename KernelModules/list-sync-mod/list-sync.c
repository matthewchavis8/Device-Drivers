/**
 * @file list-sync.c
 * @brief Synchronized kernel linked list module using rwlock for concurrent access
 * @author Matthew Chavis
 *
 * Exports task_info_add_for_current(), task_info_print_list(), and
 * task_info_remove_expired() via EXPORT_SYMBOL so other modules (e.g.
 * list-sym.c) can manipulate the shared list.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/sched/signal.h>

/**
 * struct task_info - Node storing a process snapshot with atomic reference counting
 * @pid:       Process ID
 * @timestamp: Jiffies value when the node was allocated
 * @count:     Atomic counter tracking accesses or updates to this entry
 * @list:      Kernel list_head for linking into the shared list
 */
struct task_info {
	pid_t pid;
	unsigned long timestamp;
	atomic_t count;
	struct list_head list;
};

// FN prototypes
void task_info_add_for_current(void);
void task_info_print_list(const char *msg);
void task_info_remove_expired(void);
static struct list_head head;

/** @brief Read-write lock protecting the shared task_info list */
DEFINE_RWLOCK(lock);

/**
 * @brief Allocate and initialize a task_info struct for a given PID
 * @param pid Process ID to store in the new task_info
 * @return Pointer to the allocated task_info, or NULL on failure
 */
static struct task_info *task_info_alloc(int pid)
{
	struct task_info *ti;

	ti = kmalloc(sizeof(*ti), GFP_KERNEL);
	if (ti == NULL)
		return NULL;
	ti->pid = pid;
	ti->timestamp = jiffies;
	atomic_set(&ti->count, 0);

	return ti;
}

/**
 * @brief Search the list for a task_info matching a given PID (caller must hold lock)
 * @param pid Process ID to search for
 * @return Pointer to the matching task_info, or NULL if not found
 */
static struct task_info *task_info_find_pid(int pid)
{
	struct list_head *p;
	struct task_info *ti;

	list_for_each(p, &head) {
		ti = list_entry(p, struct task_info, list);
		if (ti->pid == pid) {
			return ti;
		}
	}

	return NULL;
}

/**
 * @brief Add a PID to the list, or update its timestamp if already present
 *
 * Acquires the write lock to check for an existing entry. If found, updates
 * its timestamp and increments the count. Otherwise allocates a new entry
 * and inserts it at the head of the list.
 *
 * @param pid Process ID to add or update
 */
static void task_info_add_to_list(int pid)
{
	struct task_info *ti;

	write_lock(&lock);
	ti = task_info_find_pid(pid);
	if (ti != NULL) {
		ti->timestamp = jiffies;
		atomic_inc(&ti->count);
		write_unlock(&lock);
		return;
	}
	write_unlock(&lock);

	ti = task_info_alloc(pid);
	write_lock(&lock);
	list_add(&ti->list, &head);
	write_unlock(&lock);
}

/**
 * @brief Add task_info entries for the current process and its neighbors
 *
 * Adds entries for the current PID, its parent, and the next two tasks
 * in the task list. Exported via EXPORT_SYMBOL for use by other modules.
 */
void task_info_add_for_current(void) {
	task_info_add_to_list(current->pid);
	task_info_add_to_list(current->parent->pid);
	task_info_add_to_list(next_task(current)->pid);
	task_info_add_to_list(next_task(next_task(current))->pid);
}
EXPORT_SYMBOL(task_info_add_for_current);

/**
 * @brief Print all entries in the list under a read lock
 * @param msg Label prefix printed before the list contents
 *
 * Exported via EXPORT_SYMBOL for use by other modules.
 */
void task_info_print_list(const char *msg)
{
	struct list_head *p;
	struct task_info *ti;

	pr_info("%s: [ ", msg);

	read_lock(&lock);
	list_for_each(p, &head) {
		ti = list_entry(p, struct task_info, list);
		pr_info("(%d, %lu) ", ti->pid, ti->timestamp);
	}
	read_unlock(&lock);
	pr_info("]\n");
}
EXPORT_SYMBOL(task_info_print_list);

/**
 * @brief Remove entries that are expired and under-referenced
 *
 * An entry is removed if more than 3 seconds (3 * HZ jiffies) have elapsed
 * since its timestamp and its atomic count is less than 5. Acquires the
 * write lock for safe deletion.
 *
 * Exported via EXPORT_SYMBOL for use by other modules.
 */
void task_info_remove_expired(void)
{
	struct list_head *p, *q;
	struct task_info *ti;

	write_lock(&lock);
	list_for_each_safe(p, q, &head) {
		ti = list_entry(p, struct task_info, list);
		if (jiffies - ti->timestamp > 3 * HZ && atomic_read(&ti->count) < 5) {
			list_del(p);
			kfree(ti);
		}
	}
	write_unlock(&lock);
}
EXPORT_SYMBOL(task_info_remove_expired);

/**
 * @brief Remove and free every entry from the list under a write lock
 */
static void task_info_purge_list(void)
{
	struct list_head *p, *q;
	struct task_info *ti;

	write_lock(&lock);
	list_for_each_safe(p, q, &head) {
		ti = list_entry(p, struct task_info, list);
		list_del(p);
		kfree(ti);
	}
	write_unlock(&lock);
}

/**
 * @brief Initialize the module, populate the list, then sleep for 5 seconds
 *
 * Sleeps after populating the list to allow external modules (loaded via
 * list-sym) to call the exported functions during the timeout window.
 *
 * @return 0 on success
 */
static int list_sync_init(void)
{
	INIT_LIST_HEAD(&head);

	task_info_add_for_current();
	task_info_print_list("after first add");

	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(5 * HZ);

	return 0;
}

/**
 * @brief Clean up the module by marking one entry, removing expired, and purging
 */
static void list_sync_exit(void)
{
	struct task_info *ti;

	ti = list_entry(head.prev, struct task_info, list);
	atomic_set(&ti->count, 10);

	task_info_remove_expired();
	task_info_print_list("after removing expired");
	task_info_purge_list();
}

module_init(list_sync_init);
module_exit(list_sync_exit);
MODULE_DESCRIPTION("Messing around with critical section in list");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");

/**
 * @file sched-spin.c
 * @brief Module demonstrating intentional misuse of sleeping while holding a spinlock
 * @author Matthew Chavis
 *
 * This module acquires a spinlock and then calls schedule_timeout_interruptible(),
 * which sleeps. Sleeping while holding a spinlock is a bug in production code and
 * will trigger a "BUG: scheduling while atomic" warning. This is intentional for
 * learning and debugging purposes.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>


/**
 * @brief Initialize the module, acquire a spinlock, sleep, then release
 *
 * Intentionally calls schedule_timeout_interruptible() while holding a
 * spinlock to observe the kernel's "scheduling while atomic" diagnostic.
 *
 * @return 0 on success
 */
static int sched_spin_init(void) {
  pr_info("[LOG] Loading Module\n");

  // Make a Lock with a critical section and hold it for 5 seconds then release it
  spinlock_t lock;

  // Initializing the LOCK
  spin_lock_init(&lock);
  // Acquire Lock
  spin_lock(&lock);

  pr_info("[LOG] Grabbing the lock\n");
  // Critical Section
  // Set task as interruptible and sleep for 5 seconds before releasing
  set_current_state(TASK_INTERRUPTIBLE);
  schedule_timeout_interruptible(5 * HZ);

  // Unlock Lock
  spin_unlock(&lock);
  pr_info("[LOG] Releasing the lock\n");

  return 0;
}

/**
 * @brief Clean up the module
 */
static void sched_spin_exit(void) {
  pr_info("[LOG] Unloading sched_spin module\n");
}


MODULE_DESCRIPTION("Module for messing around with putting threads to sleep");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(sched_spin_init);
module_exit(sched_spin_exit);

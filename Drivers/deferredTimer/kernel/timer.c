#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/timer.h>
#include "../include/deferred.h"

#define MAJOR_NUMBER  42
#define MINOR_NUMBER  0
#define NUM_MINORS    1
#define MODULE_NAME   "timerDriver"

#define TIMER_TYPE_NONE    -1
#define TIMER_TYPE_SET      0
#define TIMER_TYPE_ALLOC    1
#define TIMER_TYPE_MONITOR	2

typedef unsigned int  ui;
typedef unsigned long ul;

// Simulates a blocking operation
static void alloc_io(void) {
  int secs = 10;
  set_current_state(TASK_INTERRUPTIBLE);
  schedule_timeout(secs * HZ);

  pr_info("[LOG] I went to sleep for %d seconds im sleepy\n", secs);
}

struct monitor_process {
	struct task_struct* task;
	struct list_head    list;
};

static struct monitor_process* get_proc(pid_t pid) {
	struct task_struct *task;
	struct monitor_process *p;

	rcu_read_lock();
	task = pid_task(find_vpid(pid), PIDTYPE_PID);
	rcu_read_unlock();
	if (!task)
		return ERR_PTR(-ESRCH);

	p = kmalloc(sizeof(*p), GFP_ATOMIC);
	if (!p)
		return ERR_PTR(-ENOMEM);

	get_task_struct(task);
	p->task = task;

	return p;

}

// Runs alloc_io in process context since timers run in interrupt context
static void work_handler(struct work_struct* tsk) { alloc_io(); }

typedef struct timer_dev {
  struct cdev         cdev;
  struct timer_list   timer;
  struct work_struct  work_queue;
  struct list_head    list;
  int                 flag;
  spinlock_t          lock;
} timer_dev;

static struct timer_dev devices[NUM_MINORS];

// Handler called when timer handler is called routes the correct flag to trigger
static void timer_handler(struct timer_list* t) {
  timer_dev* dev = (struct timer_dev*) from_timer(dev, t, timer);
  pr_info("[timer_handler]\n");

  switch (dev->flag) {
    case TIMER_TYPE_SET:
      break;
    case TIMER_TYPE_ALLOC:
      schedule_work(&dev->work_queue);
      break;
    case TIMER_TYPE_MONITOR: {
      ul flags;
      struct monitor_process* process;
      struct monitor_process* number;
      spin_lock_irqsave(&dev->lock, flags); // CRITICAL SECTION
                                            //
      list_for_each_entry_safe(process, number, &dev->list, list) {
        if (process->task->__state == TASK_DEAD) {
          pr_info("task %s (%d) is dead\n", process->task->comm,
            process->task->pid);
          put_task_struct(process->task);
          list_del(&process->list);
          kfree(process);
        }
      }

      spin_unlock_irqrestore(&dev->lock, flags);
      break;
    }
    default:
      break;
  }

}

// FILE OPS
static int timer_open(struct inode* inode, struct file* file) {
  struct timer_dev* data = container_of(inode->i_cdev, timer_dev, cdev);
  file->private_data = data;
  pr_info("[deferred_open]\n");

  return 0;
}

static int timer_release(struct inode* inode, struct file* file) {
  pr_info("[deferred_release]\n");
  return 0;
}

static long timer_ioctl(struct file* file, ui cmd, ul arg) {
  struct timer_dev* data = (struct timer_dev*) file->private_data;

  switch (cmd) {
    case MY_IOCTL_TIMER_SET:
      pr_info("[TIMER SET]\n");
      data->flag = TIMER_TYPE_SET;
      mod_timer(&data->timer, jiffies + arg * HZ);
      break;

    case MY_IOCTL_TIMER_CANCEL:
      pr_info("[TIMER CANCEL]\n");
      del_timer(&data->timer);
      break;

    case MY_IOCTL_TIMER_ALLOC:
      pr_info("[TIMER ALLOC]\n");
      data->flag = TIMER_TYPE_ALLOC;
      mod_timer(&data->timer, jiffies + arg * HZ);
      break;
    
    case MY_IOCTL_TIMER_MONITOR: {
      pr_info("[TIMER MONITOR]\n");
			struct monitor_process* p = get_proc(arg);
			if (IS_ERR(p))
				return PTR_ERR(p);

			spin_lock_bh(&data->lock);
			list_add(&p->list, &data->list);
			spin_unlock_bh(&data->lock);

			data->flag = TIMER_TYPE_MONITOR;
			mod_timer(&data->timer, jiffies + HZ);
			break;
		}

    default:
      break;
  }

  return 0;
}

static const struct file_operations timer_fops = {
  .owner          = THIS_MODULE,
  .open           = timer_open,
  .release        = timer_release,
  .unlocked_ioctl = timer_ioctl,
};


static int deferred_init(void) {
  // Register device in the kernel
  int result = register_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS, MODULE_NAME);

  if (result != 0) {
    pr_debug("[ERROR] failed to register device in chrdev region\n");
    return result;
  }
  // Add the devices to the registered driver
  for (int i = 0; i < NUM_MINORS; i++) {
    cdev_init(&devices[i].cdev, &timer_fops);
    cdev_add(&devices[i].cdev, MKDEV(MAJOR_NUMBER, i), 1);

    timer_setup(&devices[i].timer, timer_handler, 0);
    INIT_WORK(&devices[i].work_queue, work_handler);
    spin_lock_init(&devices[i].lock);
    INIT_LIST_HEAD(&devices[i].list);

    devices[i].flag = TIMER_TYPE_NONE;
  }

  pr_info("[LOG] registered %s char device in memory success\n", MODULE_NAME);

  return 0;
}

static void deferred_exit(void) {
  pr_info("[LOG] Unregistered %s from kernel space\n", MODULE_NAME);

  struct monitor_process* proc;
  struct monitor_process* n;
  // Delete all the devices
  for (int i = 0; i < NUM_MINORS; i++) {
    cdev_del(&devices[i].cdev);
    del_timer(&devices[i].timer);
    cancel_work_sync(&devices[i].work_queue);
	
    list_for_each_entry_safe(proc, n, &devices[i].list, list) {
		put_task_struct(proc->task);
		list_del(&proc->list);
		kfree(proc);
	}
  }

  unregister_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS);
}


MODULE_DESCRIPTION("Driver for timer inside of kernelModules/deferred-mod");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(deferred_init);
module_exit(deferred_exit);

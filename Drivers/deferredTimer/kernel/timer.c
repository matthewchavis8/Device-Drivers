#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
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

#define TIMER_TYPE_NONE		-1
#define TIMER_TYPE_SET		0
#define TIMER_TYPE_ALLOC	1
#define TIMER_TYPE_MON		2

typedef unsigned int  ui;
typedef unsigned long ul;

// Simulates a blocking operation
static void alloc_io(void) {
  int secs = 10;
  set_current_state(TASK_INTERRUPTIBLE);
  schedule_timeout(secs * HZ);

  pr_info("[LOG] I went to sleep for %d seconds im sleepy\n", secs);
}

// Runs alloc_io in process context since timers run in interrupt context
static void work_handler(struct work_struct* tsk) { alloc_io(); }

typedef struct timer_dev {
  struct cdev         cdev;
  struct timer_list   timer;
  struct work_struct  work_queue;
  int                 flag;
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
    devices[i].flag = TIMER_TYPE_NONE;
  }

  pr_info("[LOG] registered %s char device in memory success\n", MODULE_NAME);

  return 0;
}

static void deferred_exit(void) {
  pr_info("[LOG] Unregistered %s from kernel space\n", MODULE_NAME);
  // Delete all the devices
  for (int i = 0; i < NUM_MINORS; i++) {
    cdev_del(&devices[i].cdev);
    del_timer(&devices[i].timer);
    cancel_work_sync(&devices[i].work_queue);
  }

  unregister_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS);
}


MODULE_DESCRIPTION("Driver for timer inside of kernelModules/deferred-mod");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(deferred_init);
module_exit(deferred_exit);

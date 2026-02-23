#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
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

static void timer_handler(struct timer_list* timer) {
  pr_info("[timer_handler]");
}

typedef struct timer_dev {
  struct cdev       cdev;
  struct timer_list timer;
  int               flag;
} timer_dev;

static struct timer_dev devices[NUM_MINORS];

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
  }

  unregister_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS);
}


MODULE_DESCRIPTION("Driver for timer inside of kernelModules/deferred-mod");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(deferred_init);
module_exit(deferred_exit);

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define MAJOR_NUMBER  42
#define MINOR_NUMBER  0
#define NUM_MINORS    1
#define MODULE_NAME   "timerDriver"

typedef struct timer_dev {
  struct cdev cdev;
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

static const struct file_operations timer_fops = {
  .owner   = THIS_MODULE,
  .open    = timer_open,
  .release = timer_release,

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
  }

  pr_info("[LOG] registered %s char device in memory success\n", MODULE_NAME);

  return 0;
}

static void deferred_exit(void) {
  pr_info("[LOG] Unregistered %s from kernel space\n", MODULE_NAME);
  // Delete all the devices
  for (int i = 0; i < NUM_MINORS; i++) {
    cdev_del(&devices[i].cdev);
  }

  unregister_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS);
}


MODULE_DESCRIPTION("Driver for timer inside of kernelModules/deferred-mod");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(deferred_init);
module_exit(deferred_exit);

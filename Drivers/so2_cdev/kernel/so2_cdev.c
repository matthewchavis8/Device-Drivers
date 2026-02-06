#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define MAJOR_NUMBER 42
#define MINOR_NUMBER 0
#define NUM_MINORS 1
#define MODULE_NAME  "so2_cdev"

struct so2_device_data {
  struct cdev cdev; // Represents char device
  atomic_t isOpen;  // Atomic variable to ensure only one reader
};

// Number of devices
struct so2_device_data devices[NUM_MINORS];

static int so2_open(struct inode* inode, struct file* file) {
  struct so2_device_data* data;
  pr_info("%s method 'open' was called!\n", MODULE_NAME);

  // Get the current device who is called open
  data = container_of(inode->i_cdev, struct so2_device_data, cdev);

  // Cache the data so we do not have to look it up everytime with container_of
  file->private_data = data;

  // Check if the is
  if (atomic_cmpxchg(&data->isOpen, 0, 1) != 0)
    return -EBUSY;

  return 0;
}

static int so2_release(struct inode* inode, struct file* file) {
  struct so2_device_data* data;
  pr_info("%s method 'close' was called!\n", MODULE_NAME);

  // Retrieve the cached data and recast it back from void->our device data
  data = (struct so2_device_data*) file->private_data;

  // Reset access to open
  atomic_set(&data->isOpen, 0);

  // Artificially stiimulate a time out
  /*set_current_state(TASK_INTERRUPTIBLE);*/
  /*schedule_timeout(10 * HZ);*/
  return 0;
}

// Driver operations that will be overloaded
const struct file_operations so2_fops = {
  .owner    = THIS_MODULE,
  .open     = so2_open,
  .release  = so2_release,
};

static int so2_cdev_init(void) {
  int result;

  // Register device in the kernel
  result = register_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS, MODULE_NAME);

  if (result != 0) {
    pr_debug("[ERROR] failed to register device in chrdev region\n");
    return result;
  }
  // Add the devices to the registered driver
  for (int i = 0; i < NUM_MINORS; i++) {
    cdev_init(&devices[i].cdev, &so2_fops);
    cdev_add(&devices[i].cdev, MKDEV(MAJOR_NUMBER, i), 1);
  }

  pr_info("[LOG] registered %s char device in memory success\n", MODULE_NAME);

  return 0;
}

static void so2_cdev_exit(void) {
  pr_info("[LOG] Unregistered %s from kernel space\n", MODULE_NAME);
  // Delete all the devices
  for (int i = 0; i < NUM_MINORS; i++) {
    cdev_del(&devices[i].cdev);
  }

  unregister_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS);
}


MODULE_DESCRIPTION("Making my first char device driver");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(so2_cdev_init);
module_exit(so2_cdev_exit);

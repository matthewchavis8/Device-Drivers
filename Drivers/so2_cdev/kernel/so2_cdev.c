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
#define MODULE_NAME   "so2_cdev"
#define MESSAGE       "the dog is matt"
#define MSG_MAX       128

struct so2_device_data {
  struct cdev cdev; // Represents char device
  atomic_t isOpen;  // Atomic variable to ensure only one reader
  char     msg[MSG_MAX];
  size_t   msg_len;
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

static ssize_t so2_read(struct file* file, char __user* uBuff, size_t size, loff_t* offset) {
  pr_info("%s method 'read' was called!\n", MODULE_NAME);

  struct so2_device_data* data = (struct so2_device_data*) file->private_data;
  size_t lenOfStr = strlen(data->msg);
  // Check if msg is NULL
  if (!data)
    return -EINVAL;
  // Check string length
  if (*offset >= lenOfStr)
    return 0;
  
  size_t available = lenOfStr - (size_t)*offset;
  size_t n = (size < available) ? size : available;
  // Copy kernel buff -> user buff
  if (copy_to_user(uBuff, data->msg + *offset, n))
    return -EFAULT;

  // Advance the byte
  *offset += n;

  return (ssize_t)size;
}

static ssize_t so2_write(struct file* file, const char* uBuff, size_t size, loff_t* offset) {
  pr_info("%s method 'write' was called!\n", MODULE_NAME);
  
  struct so2_device_data* data = (struct so2_device_data*) file->private_data;
  size_t n = (size < (MSG_MAX - 1)) ? size : (MSG_MAX - 1);

  // Copy user buff -> kernel buff
  if (copy_from_user(data->msg, uBuff, n))
    return -EFAULT;

  data->msg[n] = '\0';
  data->msg_len = n;

  *offset = 0;

  return (ssize_t) size;
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
  .read     = so2_read,
  .open     = so2_open,
  .write    = so2_write,
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
    strscpy(devices[i].msg, MESSAGE, MSG_MAX);
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

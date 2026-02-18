#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/irqreturn.h>

#define MAJOR_NUMBER  42
#define MINOR_NUMBER  0
#define NUM_MINORS    1
#define MODULE_NAME   "kbd_driver"

#define I8042_KBD_IRQ		  1
#define I8042_STATUS_REG	0x64
#define I8042_DATA_REG		0x60

irqreturn_t kbd_irq_handler(int irq, void* devID) {
  return IRQ_NONE;
}

static int kbd_init(void) {
  pr_info("[LOG] kbd driver intialized\n");
  int res = 0;
  // Request space for char device
  res = register_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS, MODULE_NAME);
  if (res != 0) {
    pr_debug("[ERROR] failed to allocate char device region\n");
    goto failedToAlloc;
  }

  // request I/O ports
  if (!request_region(I8042_DATA_REG, 1, MODULE_NAME)) {
    res = -EBUSY;
    goto releaseDataPort;
  }
  
  if (!request_region(I8042_STATUS_REG, 1, MODULE_NAME)) {
    res = -EBUSY;
    goto releaseStatusPort;
  }

  releaseStatusPort:
    release_region(I8042_STATUS_REG, 1);
  releaseDataPort:
    release_region(I8042_DATA_REG, 1);
  /*releaseRegion:*/ // NOT USED FOR NOW
  /*  unregister_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS);*/
  failedToAlloc:
    return res;

  return 0;
}
static void kbd_exit(void) {
  pr_info("[LOG] kbd driver exited\n");

  // releasing the I/O ports
  release_region(I8042_STATUS_REG, 1);
  release_region(I8042_DATA_REG, 1);

  // release char device region
  unregister_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS);
}

module_init(kbd_init);
module_exit(kbd_exit);

MODULE_DESCRIPTION("Making a keyboard device driver");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");

/**
 * @file kbd.c
 * @brief Keyboard character device driver using the i8042 controller
 * @author Matthew Chavis
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/irqreturn.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/spinlock.h>

#define MAJOR_NUMBER  42
#define MINOR_NUMBER  0
#define NUM_MINORS    1
#define MODULE_NAME   "memory_driver"
#define BUFF_SIZE     100

#define I8042_KBD_IRQ		  1
#define I8042_STATUS_REG	0x64
#define I8042_DATA_REG		0x60

#define NPAGES 3

typedef struct mem_dev {
  struct cdev cdev;
} mem_dev;

static inline void logg(char* msg) {
  pr_info("[LOG]: %s\n", msg);
}

static void* kmalloc_ptr;
static void* kmalloc_area;

int memory_mmap(struct file* flip, struct vm_area_struct* vma);
int memory_mmap(struct file* flip, struct vm_area_struct* vma) {

  unsigned long size = vma->vm_end - vma->vm_start;
  if (size > NPAGES * PAGE_SIZE)
    return -EIO;

  unsigned long pfn = virt_to_phys(kmalloc_area) >> PAGE_SHIFT;

  return remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot);
}

static const struct file_operations dev_fops = {
  .mmap = memory_mmap,
};

static mem_dev devs[NUM_MINORS];

static int mydriver_init(void) {
  logg("memory driver has been loaded");
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

  int i = 0;
  // Register device minors
  for (i = 0; i < NUM_MINORS; i++) {
    cdev_init(&devs[i].cdev, &dev_fops);
    cdev_add(&devs[i].cdev, MKDEV(MAJOR_NUMBER, MINOR_NUMBER), 1);
  }

  // Get ptr to unaligned kernel memory
  kmalloc_ptr = kmalloc(NPAGES + 2, GFP_KERNEL);
  if (!kmalloc_ptr)
    return -ENOMEM;

  // aligned memory
  kmalloc_area = (void*)PAGE_ALIGN((unsigned long)kmalloc_ptr);

  // mark pages reserved
  for (i = 0; i < NPAGES; i++) {
    SetPageReserved(virt_to_page(kmalloc_area + i * PAGE_SIZE));
  }

  // Magic number for testing
  for (i = 0; i < NPAGES; i++) {
    ((char*)kmalloc_area)[i * PAGE_SIZE + 0] = 0xaa;
    ((char*)kmalloc_area)[i * PAGE_SIZE + 1] = 0xbb;
    ((char*)kmalloc_area)[i * PAGE_SIZE + 2] = 0xcc;
    ((char*)kmalloc_area)[i * PAGE_SIZE + 3] = 0xdd;
  }
  
  // Register IRQ handler
  /*for (int i = 0; i < NUM_MINORS; i++) {*/
  /*  res = request_irq(I8042_KBD_IRQ, kbd_irq_handler, IRQF_SHARED, MODULE_NAME, &devs[i]);*/
  /*}*/

  releaseStatusPort:
    release_region(I8042_STATUS_REG, 1);
  releaseDataPort:
    release_region(I8042_DATA_REG, 1);
  /*releaseRegion:*/ // NOT USED FOR NOW
  /*  unregister_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS);*/
  failedToAlloc:
    return res;

  return res;
}
/**
 * @brief Clean up and unregister the keyboard driver module
 */
static void mydriver_exit(void) {
  pr_info("[LOG] memory driver exited\n");

  // free IRQ
  int i = 0;
  for (i = 0; i < NUM_MINORS; i++) {
    free_irq(I8042_KBD_IRQ, &devs[i].cdev);
  }
  
  // release devices 
  for (i = 0; i < NUM_MINORS; i++) {
    cdev_del(&devs[i].cdev);
  }

  // releasing the I/O ports
  release_region(I8042_STATUS_REG, 1);
  release_region(I8042_DATA_REG, 1);

  // release char device region
  unregister_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS);

  // free pages
  for (i = 0; i < NPAGES; i++) {
    ClearPageReserved(virt_to_page(kmalloc_area * i * PAGE_SIZE));
  }
  
  kfree(kmalloc_ptr);
}

module_init(mydriver_init);
module_exit(mydriver_exit);

MODULE_DESCRIPTION("Making a keyboard device driver");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");

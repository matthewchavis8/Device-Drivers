#include "asm/current.h"
#include "asm/memory.h"
#include "asm/page-def.h"
#include "linux/gfp.h"
#include "linux/mm_types.h"
#include "linux/page-flags.h"
#include "linux/printk.h"
#include "linux/proc_fs.h"
#include "linux/sched/mm.h"
#include "linux/seq_file.h"
#include "linux/vmalloc.h"
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/irqreturn.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/spinlock.h>

#define MAJOR_NUMBER  42
#define MINOR_NUMBER  0
#define NUM_MINORS    1
#define MODULE_NAME   "nonmemory_driver"
#define NPAGES 3
#define PROC_ENTRY_NAME "nonmemory_driver_proc"

void* vmalloc_area;

static inline void logg(char* msg) { pr_info("[LOG]: %s\n", msg); }

typedef struct memory_dev {
  struct cdev cdev;
} memory_dev;

memory_dev devs[NUM_MINORS];

int memory_open(struct inode* inode, struct file* file);
int memory_open(struct inode* inode, struct file* file) {
  struct memory_dev* dev = container_of(inode->i_cdev, memory_dev, cdev);
  file->private_data = dev;
  logg("[memory_open called]");

  return 0;
}
int memory_release(struct inode* inode, struct file* file);
int memory_release(struct inode* inode, struct file* file) {
  logg("[memory_close called]");

  return 0;
}

int memory_mmap(struct file* file, struct vm_area_struct* vma);
int memory_mmap(struct file* file, struct vm_area_struct* vma) {
  unsigned long size = vma->vm_end - vma->vm_start;
  if (size > NPAGES * PAGE_SIZE) {
    return -1;
  }

  unsigned long offset = 0;
  for (offset = 0; offset < size; offset += PAGE_SIZE) {
    unsigned long pfn = vmalloc_to_pfn(vmalloc_area + offset);

    int ret = remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot);
    if (ret)
      return ret;
  }

  return 0;
}

static int memory_seq_show(struct seq_file* seq, void* v);
static int memory_seq_show(struct seq_file *seq, void *v) {
    struct vm_area_struct *vma;
    unsigned long size = 0;
    // Get memory descriptor of the current calling process
    struct mm_struct *mm = get_task_mm(current);

    if (!mm)
        return 0;

    VMA_ITERATOR(chunk, mm, 0);
    for_each_vma(chunk, vma) {
        pr_info("%lx %lx\n", vma->vm_start, vma->vm_end);
        size += vma->vm_end - vma->vm_start;
    }

    mmput(mm); // frees the mm reference count
    seq_printf(seq, "%lu", size);
    return 0;
}

static int memory_seq_open(struct inode* inode, struct file* file) {
  return single_open(file, memory_seq_show, NULL);
}
static const struct file_operations fops = {
  .open = memory_open,
  .release = memory_release,
  .mmap = memory_mmap,
};

static const struct proc_ops proc_ops = {
  .proc_open = memory_seq_open,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,
};

static int driver_initt(void) {
  logg("non-contignous memory driver has been initalized");
  int res = 0;
  proc_create(PROC_ENTRY_NAME, 0, NULL, &proc_ops);
  // Request space for char device
  res = register_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS, MODULE_NAME);
  if (res != 0) {
    pr_debug("[ERROR] failed to allocate char device region\n");
    return res;
  }

  // Register device minors
  int i = 0;
  for (i = 0; i < NUM_MINORS; i++) {
    cdev_init(&devs[i].cdev, &fops);
    cdev_add(&devs[i].cdev, MKDEV(MAJOR_NUMBER, MINOR_NUMBER), 1);
  }

  // allocating non-contignous memory
  vmalloc_area = vmalloc(PAGE_SIZE * NPAGES);

  // reserve pages so kernel does not swap it out
  for (i = 0; i < NPAGES; i++) {
    struct page* page = vmalloc_to_page(vmalloc_area + PAGE_SIZE * i);
    SetPageReserved(page);
  }

  return res;
}

static void driver_exit(void) {
  logg("non-contignous memory driver has exited");
  // release devices 
  int i = 0;
  for (i = 0; i < NUM_MINORS; i++) {
    cdev_del(&devs[i].cdev);
  }

  // release char device region
  unregister_chrdev_region(MKDEV(MAJOR_NUMBER, MINOR_NUMBER), NUM_MINORS);
  
  // unrserve pages
  for (i = 0; i < NPAGES; i++) {
    struct page* page = vmalloc_to_page(vmalloc_area + PAGE_SIZE * i);
    ClearPageReserved(page);
  }
  vfree(vmalloc_area);

  remove_proc_entry(PROC_ENTRY_NAME, NULL);
}

module_init(driver_initt);
module_exit(driver_exit);

MODULE_DESCRIPTION("messing around with non-contignous memory to userspace");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");

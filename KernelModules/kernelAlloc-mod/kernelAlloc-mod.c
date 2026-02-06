#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

static char* kMemBuff;
static char* strToCpy = "The Dog is Black\n";

static int allocBuff(void) {
  // Allocating the buffer
  kMemBuff = kmalloc(4096, GFP_KERNEL);
  size_t len = strlen(strToCpy);

  pr_info("[LOG] loaded module and allocating a buffer of 4096 bytes\n");

  if (kMemBuff == NULL) {
    pr_debug("[ERROR] Failed to allocate buffer\n");
    return -ENOMEM;
  }

  // Copying the string into the kmalloc buff
  memcpy(kMemBuff, strToCpy, len);

  // Printing the string
  pr_info("[LOG] The string is: \n");
  for (int i = 0; i < len; i++) {
    pr_info("%c", kMemBuff[i]);
  }

  return 0;
}

static void exitKernelAlloc(void) {
  // Freeing the Memory for kalloc
  if (kMemBuff)
    kfree(kMemBuff);

  pr_info("[LOG] Unloading Module\n");
}


MODULE_DESCRIPTION("Module for messing around with Linux API for freeing and using heap memory in userspace");
MODULE_AUTHOR("Matthew Chavis");
MODULE_LICENSE("GPL");
module_init(allocBuff);
module_exit(exitKernelAlloc);

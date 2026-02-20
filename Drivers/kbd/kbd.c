/**
 * @file kbd.c
 * @brief Keyboard character device driver using the i8042 controller
 * @author Matthew Chavis
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/irqreturn.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/spinlock.h>

#define MAJOR_NUMBER  42
#define MINOR_NUMBER  0
#define NUM_MINORS    1
#define MODULE_NAME   "kbd_driver"
#define BUFF_SIZE     100

#define I8042_KBD_IRQ		  1
#define I8042_STATUS_REG	0x64
#define I8042_DATA_REG		0x60

#define SCANCODE_RELEASED_MASK	0x80
/**
 * struct kbd_dev - Per-device state for the keyboard driver
 * @cdev:   Character device structure registered with the kernel
 * @lock:   Spinlock protecting the circular buffer
 * @buff:   Circular buffer storing received key characters
 * @putIdx: Write index into the circular buffer
 * @getIdx: Read index into the circular buffer
 * @cnt:    Number of characters currently stored in the buffer
 */
typedef struct kbd_dev {
  struct cdev cdev;
  spinlock_t lock;
  char buff[BUFF_SIZE];
  size_t putIdx;
  size_t getIdx;
  size_t cnt;
} kbd_dev;

/**
 * @brief Retrieve the next character from the circular buffer
 * @param c    Pointer to store the retrieved character
 * @param data Pointer to the keyboard device containing the buffer
 * @return true if a character was available, false if the buffer was empty
 */
static bool get_char(char *c, struct kbd_dev *data) {
	if (data->cnt > 0) {
		*c = data->buff[data->getIdx];
		data->getIdx = (data->getIdx + 1) % BUFF_SIZE;
		data->cnt--;
		return true;
	}

	return false;
}
/**
 * @brief Open the keyboard device
 * @param inode Inode structure for the device file
 * @param file  File structure to associate with the device
 * @return 0 on success
 */
static int kbd_open(struct inode* inode, struct file* file) {
  struct kbd_dev* data = container_of(inode->i_cdev, kbd_dev, cdev);
  file->private_data = data;

  return 0;
}
/**
 * @brief Release the keyboard device
 * @param inode Inode structure for the device file
 * @param file  File structure associated with the device
 * @return 0 on success
 */
static int kbd_release(struct inode* inode, struct file* file) {
  return 0;
}
/**
 * @brief Read characters from the keyboard buffer into userspace
 * @param file   File structure for the open device
 * @param uBuff  Userspace buffer to copy characters into
 * @param size   Maximum number of bytes to read
 * @param offset File position (unused)
 * @return Number of bytes read, or -EFAULT on copy failure
 */
static ssize_t kbd_read(struct file *file,  char *uBuff, size_t size, loff_t *offset) {
	struct kbd_dev *data = (struct kbd_dev *) file->private_data;
	size_t read = 0;
	unsigned long flags;
	char ch;
	bool more = true;

	while (size--) {
		spin_lock_irqsave(&data->lock, flags);
		more = get_char(&ch, data);
		spin_unlock_irqrestore(&data->lock, flags);

		if (!more)
			break;

		if (put_user(ch, uBuff++))
			return -EFAULT;

		read++;
	}

	return read;
}

/**
 * @brief Reset the circular buffer to its initial empty state
 * @param dev Pointer to the keyboard device whose buffer is reset
 */
static void reset_buffer(kbd_dev* dev) {
  dev->cnt    = 0;
  dev->getIdx = 0;
  dev->putIdx = 0;
}

/**
 * @brief Write handler that clears the keyboard buffer
 * @param file   File structure for the open device
 * @param uBuff  Userspace buffer (unused, buffer is simply cleared)
 * @param size   Number of bytes written
 * @param offset File position (unused)
 * @return The number of bytes requested (always succeeds)
 */
static ssize_t kbd_write(struct file* file, const char* uBuff, size_t size, loff_t* offset) {
  kbd_dev* dev = (kbd_dev*) file->private_data;
  unsigned long flags;

  spin_lock_irqsave(&dev->lock, flags);
  reset_buffer(dev);
  spin_unlock_irqrestore(&dev->lock, flags);

  return size;
}

/**
 * @brief File operations table for the keyboard character device
 */
static const struct file_operations kbd_fops = {
  .owner = THIS_MODULE,
  .open  = kbd_open,
  .release = kbd_release,
  .read   = kbd_read,
  .write  = kbd_write
};

// intialzie max amount of devices under teh driver
static kbd_dev devs[NUM_MINORS];

/**
 * @brief Read a scancode byte from the i8042 data register
 * @return The scancode byte read from the keyboard controller
 */
static u8 i8042_read_data(void) {
 u8 val = inb(I8042_DATA_REG);
 return val;
}

/**
 * @brief Check if a scancode corresponds to a key press event
 * @param scancode Raw scancode from the keyboard controller
 * @return Non-zero if the scancode is a key press, 0 if a key release
 */
static int is_key_press(unsigned int scancode) {
	return !(scancode & SCANCODE_RELEASED_MASK);
}

/**
 * @brief Store a character into the circular buffer
 * @param data Pointer to the keyboard device containing the buffer
 * @param ch   Character to store
 */
static void put_char(kbd_dev* data, char ch) {
  data->buff[data->getIdx] = ch;
	data->putIdx = (data->putIdx + 1) % BUFF_SIZE;
  data->cnt++;
}

/**
 * @brief Convert a scancode to its ASCII character representation
 * @param scancode Raw scancode from the keyboard controller
 * @return ASCII character for alphanumeric/space/enter keys, '?' for others
 */
static int get_ascii(unsigned int scancode) {
	static char *row1 = "1234567890";
	static char *row2 = "qwertyuiop";
	static char *row3 = "asdfghjkl";
	static char *row4 = "zxcvbnm";

	scancode &= ~SCANCODE_RELEASED_MASK;
	if (scancode >= 0x02 && scancode <= 0x0b)
		return *(row1 + scancode - 0x02);
	if (scancode >= 0x10 && scancode <= 0x19)
		return *(row2 + scancode - 0x10);
	if (scancode >= 0x1e && scancode <= 0x26)
		return *(row3 + scancode - 0x1e);
	if (scancode >= 0x2c && scancode <= 0x32)
		return *(row4 + scancode - 0x2c);
	if (scancode == 0x39)
		return ' ';
	if (scancode == 0x1c)
		return '\n';
	return '?';
}

/**
 * @brief Interrupt handler for keyboard key press events
 * @param irq_no IRQ number that triggered the interrupt
 * @param devID  Pointer to the kbd_dev associated with this IRQ
 * @return IRQ_NONE (shared IRQ, other handlers may also process)
 */
static irqreturn_t kbd_irq_handler(int irq_no, void* devID) {
  u32 scancode = i8042_read_data();
  int pressed = is_key_press(scancode);
  int ch = get_ascii(scancode);

	pr_info("IRQ %d: scancode=0x%x (%u) pressed=%d ch=%c\n",
		irq_no, scancode, scancode, pressed, ch);

  if (pressed) {
    kbd_dev* dev = (kbd_dev*) devID;
    spin_lock(&dev->lock);
    put_char(dev, ch);
    spin_unlock(&dev->lock);

  }
  
  return IRQ_NONE;
}

/**
 * @brief Initialize the keyboard driver module
 * @return 0 on success, negative error code on failure
 */
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

  // Register device minors
  for (int i = 0; i < NUM_MINORS; i++) {
    cdev_init(&devs[i].cdev, &kbd_fops);
    cdev_add(&devs[i].cdev, MKDEV(MAJOR_NUMBER, MINOR_NUMBER), 1);
    spin_lock_init(&devs[i].lock);
  }
  
  // Register IRQ handler
  for (int i = 0; i < NUM_MINORS; i++) {
    res = request_irq(I8042_KBD_IRQ, kbd_irq_handler, IRQF_SHARED, MODULE_NAME, &devs[i]);
  }

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
static void kbd_exit(void) {
  pr_info("[LOG] kbd driver exited\n");

  // free IRQ
  for (int i = 0; i < NUM_MINORS; i++) {
    free_irq(I8042_KBD_IRQ, &devs[i].cdev);
  }
  
  // release devices 
  for (int i = 0; i < NUM_MINORS; i++) {
    cdev_del(&devs[i].cdev);
  }

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

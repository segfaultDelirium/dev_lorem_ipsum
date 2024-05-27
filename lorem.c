#include <linux/cdev.h>   // Needed for character device registration
#include <linux/device.h> // Needed for the device creation functions
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "lorem"
#define CLASS_NAME "lorem_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Linux char driver for lorem ipsum text");
MODULE_VERSION("0.1");

static int majorNumber;
static char *lorem_text =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod "
    "tempor incididunt ut labore et dolore magna aliqua. ";
static int lorem_length;
static struct class *loremClass = NULL;
static struct device *loremDevice = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .release = dev_release,
};

static int __init lorem_init(void) {
  printk(KERN_INFO "Lorem: Initializing the Lorem Ipsum LKM\n");

  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  if (majorNumber < 0) {
    printk(KERN_ALERT "Lorem failed to register a major number\n");
    return majorNumber;
  }
  printk(KERN_INFO "Lorem: registered correctly with major number %d\n",
         majorNumber);

  // Register the device class
  loremClass = class_create(CLASS_NAME);
  if (IS_ERR(loremClass)) {
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "Failed to register device class\n");
    return PTR_ERR(loremClass);
  }
  printk(KERN_INFO "Lorem: device class registered correctly\n");

  // Register the device driver
  loremDevice =
      device_create(loremClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  if (IS_ERR(loremDevice)) {
    class_destroy(loremClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_ALERT "Failed to create the device\n");
    return PTR_ERR(loremDevice);
  }
  printk(KERN_INFO "Lorem: device class created correctly\n");

  lorem_length = strlen(lorem_text);

  return 0;
}

static void __exit lorem_exit(void) {
  device_destroy(loremClass, MKDEV(majorNumber, 0));
  class_unregister(loremClass);
  class_destroy(loremClass);
  unregister_chrdev(majorNumber, DEVICE_NAME);
  printk(KERN_INFO "Lorem: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "Lorem: Device has been opened\n");
  return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len,
                        loff_t *offset) {
  int bytes_read = 0;
  int lorem_index = *offset % lorem_length;

  while (len && (lorem_index < lorem_length)) {
    put_user(lorem_text[lorem_index], buffer++);
    len--;
    bytes_read++;
    lorem_index++;
  }

  *offset += bytes_read;

  return bytes_read;
}

static int dev_release(struct inode *inodep, struct file *filep) {
  printk(KERN_INFO "Lorem: Device successfully closed\n");
  return 0;
}

module_init(lorem_init);
module_exit(lorem_exit);

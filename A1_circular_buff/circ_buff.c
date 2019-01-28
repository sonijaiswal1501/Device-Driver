/* Implement a char driver that supports only write function. The write function uses a buffer
   to accept the data, but the size of the buffer is not defined. The memory to the buffer will be
   allocated based on the size of the data written from the user-space. Hence, the write function
   will be something like this :

   * Calculate the size of the buffer required
   * Create a buffer dynamically as per the size of the data
   * Copy the data into the buffer
   * Print the data
   * Deallocate the buffer

   Hint : Use kmalloc() and kfree() functions for dynamic memory allocation at kernel level. */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/circ_buf.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define MAJORNO 42
#define N_MINORS 1
#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soni sneha");

static struct circ_buf cbuf;


int char_dev_open(struct inode *inodp, struct file *filp)
{
	printk("In open call\n");
	return 0;
}

int char_dev_close(struct inode *inodp, struct file *filp)
{
	printk("In close call\n");
	return 0;
}

ssize_t char_dev_read(struct file *filp, char __user *ubuff, size_t cnt, loff_t *off)
{
	int i, ret;

	printk("In read system call\n");

	for(i=0; i<cnt; i++)
        {
          ret = copy_to_user(ubuff+i, cbuf.buf + cbuf.tail, 1);
          if(ret)
          {
           printk("Error copying\n");
           return -1;
          }

          cbuf.tail = (cbuf.tail + 1) & (SIZE - 1);
        }

	return 0;
}

ssize_t char_dev_write(struct file *filp, const char __user *ubuff, size_t cnt, loff_t *off)
{
	int i, ret;

	printk("In write system call\n");

	for(i=0; i<cnt; i++)
	{
	  ret = copy_from_user(cbuf.buf + cbuf.head, ubuff + i, 1);
	  if(ret)
	  {
	   printk("Error copying\n");
	   return -1;
	  }

	  cbuf.head = (cbuf.head + 1) & (SIZE - 1);
	}
	return 0;
}

static dev_t devno;
static struct circ_buf cbuf;
static struct cdev _cdev;

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = char_dev_open,
	.release = char_dev_close,
	.read = char_dev_read,
	.write = char_dev_write,
};

static __init int char_dev_init(void)
{
	int ret;

	devno = MKDEV(MAJORNO, 0);

	printk("Inside Init");
	ret = register_chrdev_region(devno, 1, "testdev");

	if(ret)
	{
	   printk("Error registering device number\n");
	   return ret;
	}

	cbuf.buf = kmalloc(SIZE, GFP_KERNEL);

	if(!cbuf.buf)
	{
	  printk("Error allocating memory\n");
	  unregister_chrdev_region(devno, 1);
	  return -1;
	}

	cdev_init(&_cdev, &fops);

	ret = cdev_add(&_cdev, devno, 1);
	kfree(cbuf.buf);

	if(ret)
	{
	   unregister_chrdev_region(devno, 1);
	   return ret;
	}

	return 0;
}

static __exit void char_dev_exit(void)
{
	printk("\nExit\n");
	kfree(cbuf.buf);
	cdev_del(&_cdev);

	unregister_chrdev_region(devno, 1);
}

module_init(char_dev_init);
module_exit(char_dev_exit);



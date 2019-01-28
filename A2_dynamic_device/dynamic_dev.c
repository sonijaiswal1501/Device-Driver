/* Implement the above program using dynamic device number assignment. */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/circ_buf.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define N_MINORS 1
#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soni sneha");

static struct circ_buf cbuf;

int dynamic_dev_open(struct inode *inodp, struct file *filp)
{
	printk("\nIn open call\n");
	return 0;
}

int dynamic_dev_close(struct inode *inodp, struct file *filp)
{
	printk("\nIn close call\n");
	return 0;
}

ssize_t dynamic_dev_read(struct file *filp, char __user *ubuff, size_t cnt, loff_t *off)
{
	printk("In read system call\n");printk("t2 w %s\n", ubuff);

        return 0;
}

ssize_t dynamic_dev_write(struct file *filp, const char __user *ubuff, size_t cnt, loff_t *off)
{
	printk("In write system call\n"); printk("t1 w %s\n", ubuff);

        return 0;
}

static dev_t devno;
static struct cdev _cdev;
static struct circ_buf cbuf;

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = dynamic_dev_open,
	.release = dynamic_dev_close,
	.read = dynamic_dev_read,
	.write = dynamic_dev_write,
};

static __init int dynamic_dev_init(void)
{
        int ret;
	printk("Inside Init\n");

	ret = alloc_chrdev_region(&devno, 0 , N_MINORS ,  "test_dev");

	if(ret)
	{
	   printk("Error registering device number\n");
	   kfree(cbuf.buf);
	   return ret;
	}

	cbuf.buf = kmalloc(SIZE, GFP_KERNEL);
	if(!cbuf.buf)
	{
	  printk("\nError allocating memory\n");
	  unregister_chrdev_region(devno, 1);
	  return -1;
	}

	cdev_init(&_cdev, &fops);

	ret = cdev_add(&_cdev, devno , 1);

	if(ret)
	{
	   unregister_chrdev_region(devno, 1);
	   kfree(cbuf.buf);
	   return ret;
	}

	return 0;
}

static __exit void dynamic_dev_exit(void)
{
	printk("\nExit\n");

	kfree(cbuf.buf);
	cdev_del(&_cdev);
	unregister_chrdev_region(devno, 1);
}

module_init(dynamic_dev_init);
module_exit(dynamic_dev_exit);


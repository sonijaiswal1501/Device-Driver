/* Write a character driver with open, read, write and close functionalities

     *Test the driver through a user application by reading data from the
      driver and writing data to the driver */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>


#define MAJORNO 42
#define N_MINORS 1
//#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soni sneha");


int file_ops_open(struct inode *inodp, struct file *filp)
{
	printk("\nIn open call\n");
	return 0;
}

int file_ops_close(struct inode *inodp, struct file *filp)
{
	printk("\nIn close call\n");
	return 0;
}

ssize_t file_ops_read(struct file *filp, char __user *ubuff, size_t cnt, loff_t *off)
{
	printk("\nIn read system call\n");
	return 0;
}

ssize_t file_ops_write(struct file *filp, const char __user *ubuff, size_t cnt, loff_t *off)
{
	printk("\nIn write system call\n");
	return 0;
}

static dev_t devno;
static struct cdev _cdev;
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = file_ops_open,
	.release = file_ops_close,
	.read = file_ops_read,
	.write = file_ops_write,
};

static __init int file_ops_init(void)
{
	devno = MKDEV(MAJORNO, 0);
	int ret;
	printk("Inside Init\n");
	ret = register_chrdev_region(devno, 1, "testdev");

	if(ret)
	{
	   printk("Error registering device number\n");
	   return ret;
	}

	cdev_init(&_cdev, &fops);

	ret = cdev_add(&_cdev, devno, 1);

	if(ret)
	{
	   unregister_chrdev_region(devno, 1);
	   return ret;
	}

	return 0;
}

static __exit void file_ops_exit(void)
{
	printk("\nExit\n");

	cdev_del(&_cdev);

	unregister_chrdev_region(devno, 1);
}

module_init(file_ops_init);
module_exit(file_ops_exit);


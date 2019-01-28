#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>

#define MAJORNO 42
#define N_MINORS 1
#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soni sneha");


ssize_t file_ops_write(struct file *filp, const char __user *ubuff, size_t cnt)
{
	int i, ret;
	char *buff;

	printk("\nIn write system call\n");

	printk("count is : %d\n", cnt);
	buff = kmalloc(cnt, GFP_KERNEL);

	for(i=0; i<=cnt; i++)
	{
	  ret = copy_from_user(buff + i, ubuff + i, 1);

	    if(ret)
	       kfree(buff);
	}
	printk("\nEntered message in buffer: %s and %d\n", ubuff, sizeof(ubuff));
	return 0;
}

static dev_t devno;
static struct cdev _cdev;

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.write = file_ops_write,
};

static __init int file_ops_init(void)
{
	int ret;
	devno = MKDEV(MAJORNO, 0);

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


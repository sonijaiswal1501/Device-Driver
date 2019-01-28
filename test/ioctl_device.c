#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/circ_buf.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/ioctl.h>
#include "my_ioctl.h"

#define DEVCOUNT 1

static dev_t devno;
static 	struct cdev _cdev;

struct ioctl_struct
{
	int id;
	char name[20];
	unsigned long data;
};


MODULE_LICENSE("GPL");
MODULE_AUTHOR("DESD");


int open_func(struct inode *inodp,struct file *filep)
{
	printk("Inside open func\n");

	return 0;

}

int close_func(struct inode *inodp,struct file *filep)
{
	printk("Inside close\n");
	return 0;
}
long ioctl_func(struct file *filep,unsigned int cmd,unsigned long arg)
{
	int ret = 0;

	struct ioctl_struct *my_struct = kmalloc(sizeof(struct ioctl_struct),GFP_KERNEL);
	struct ioctl_struct *my_struct1 = kmalloc(sizeof(struct ioctl_struct),GFP_KERNEL);
	switch(cmd)
	{
		case CMD_WRITE_STRUCT:
				printk("Inside CMD_WRITE_STRUCT\n");
				ret = copy_from_user(my_struct,(struct ioctl_struct *)arg,sizeof(struct ioctl_struct));
				if(ret > 0)
				{
					printk("Error copy_from_user\n");
					return -1;
				}
				printk("Structure received from user app:\nid = %d\nname = %s\ndata = %ld\n",my_struct->id,my_struct->name,my_struct->data);	
				break;
		case CMD_READ_STRUCT:
				printk("Inside CMD_READ_STRUCT\n");
				my_struct1->id = 99;
				strcpy(my_struct1->name,"Hello User!");
				my_struct1->data = 65535;
				ret = copy_to_user((char *)arg,my_struct1,sizeof(struct ioctl_struct));
				if(ret)
				{
					printk("Error copy_to_user\n");
				}
				printk("Sent structure to user\n");
				break;

		default: printk("Invalid ioctl cmd\n");
	}
	return 0;

}


static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = open_func,
	.release = close_func,
	.unlocked_ioctl = ioctl_func,
};

static __init int init_func(void)
{
	printk("Initialising driver\n");
	int ret;
	devno = MKDEV(104,0);

	ret = register_chrdev_region(devno,DEVCOUNT,"test3");

	if(ret)
	{
		printk("Error registering device\n");
		return -1;
	}

	cdev_init(&_cdev,&fops);

		ret = cdev_add(&_cdev,devno,DEVCOUNT);
		if(ret)
		{
			printk("Error adding to cdev\n");

			unregister_chrdev_region(devno,DEVCOUNT);
			return -1;
		}

	return 0;

}

static __exit int exit_func(void)
{
	printk("Exiting!\n");

	cdev_del(&_cdev);

	unregister_chrdev_region(devno,DEVCOUNT);
	return 0;
}

module_init(init_func);
module_exit(exit_func);


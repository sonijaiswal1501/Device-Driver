#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/circ_buf.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include "my_ioctl.h"

#define MAJORNO 42
#define N_MINORS 1
#define SIZE 32


MODULE_LICENSE("GPL");
MODULE_AUTHOR("soni sneha");

static struct circ_buf cbuf;
int delay;


int fops_open(struct inode *inodp, struct file *filp)
{
        printk("\nIn open call\n");
        return 0;
}

int fops_close(struct inode *inodp, struct file *filp)
{
        printk("\nIn close call\n");
        return 0;
}
ssize_t fops_read(struct file *filp, char __user *ubuff, size_t cnt, loff_t *off)
{
	int i, ret, nowtime;
	char *buff;

        printk("In read system call\n");

	buff = "\nHello\n";

	nowtime = jiffies;

	while(time_before(jiffies, ( delay*HZ + nowtime )));
//	msleep((nowtime + (HZ*delay)) - jiffies);

        for(i=0; i<15; i++)
        {
          ret = copy_to_user(ubuff + i, buff + i , 1);

          if(ret)
          {
             printk("Error copying\n");
             return -1;
          }

        }
	printk("\nthe string is %s\n",ubuff);
        return 0;
}

static long module_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        printk("\nInside ioctl\n");

        switch(cmd)
        {
                case CMD_DELAY :printk("\nInside CMD DELAY\n");
				delay = (int)arg;
				printk("\nThe value of delay is:%d\n", delay);
                                break;

                       default:printk("\nNot valid ioctl\n");
                               return -ENOTTY;
        }

	return 0;
 }

static dev_t devno;
static struct cdev _cdev;

static struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = fops_open,
        .release = fops_close,
        .read = fops_read,
        .unlocked_ioctl = module_ioctl,
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



       

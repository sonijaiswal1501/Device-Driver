/* Write a character driver with open and close functionality

   * Test the driver by writing an application that opens and closes the device driver.
     When open or closed, the open and close calls in the driver should be executed. */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>


#define MAJORNO 42
#define N_MINORS 1


MODULE_LICENSE("GPL");
MODULE_AUTHOR("soni sneha");

static char *buff;

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


static dev_t devno;
static struct cdev _cdev;

static struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = file_ops_open,
        .release = file_ops_close,
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



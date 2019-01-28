/* Write a character driver to dynamically allocate a maj, min no pair from the kernel.

    *Test the same and conclude */


#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define N_MINORS 1
#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("i am");

static char *buff;

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
        int i, ret;

	printk("\nIn read system call\n");

	for(i=0; i<cnt; i++)
        {
          ret = copy_to_user(ubuff + i, buff + i,  1);

          if(ret)
          {
             printk("Error copying\n");
             return -1;
          }

        }

        return cnt;
}

ssize_t dynamic_dev_write(struct file *filp, const char __user *ubuff, size_t cnt, loff_t *off)
{
	int i, ret;

	printk("\nIn write system call\n");
        
        buff = kmalloc(cnt, GFP_KERNEL);
       
        for(i=0; i<=cnt; i++)
        {
          ret = copy_from_user(buff + i, ubuff + i, 1);

            if(ret)
               kfree(buff);
        }
        
        return cnt;
}

static dev_t devno;
static struct cdev _cdev;

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

        ret = alloc_chrdev_region(&devno, 1 , 1, "test_dev");

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

static __exit void dynamic_dev_exit(void)
{
        printk("\nExit\n");

        cdev_del(&_cdev);
        unregister_chrdev_region(devno, 1);
}

module_init(dynamic_dev_init);
module_exit(dynamic_dev_exit);



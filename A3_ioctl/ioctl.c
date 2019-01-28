#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/circ_buf.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include "my_ioctl.h"

#define MAJORNO 42
#define N_MINORS 1
#define SIZE 32
//#define MY_TYPE 'x'

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soni sneha");

static struct circ_buf cbuf;


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
	int i, ret, m;

	printk("In read system call\n");

        m = min((int)cnt, CIRC_CNT(cbuf.head, cbuf.tail, SIZE));

        for(i=0; i<m; i++)
        {
          ret = copy_to_user(ubuff + i, cbuf.buf + cbuf.tail,  1);

          if(ret)
          {
             printk("Error copying\n");
             return -1;
          }

          cbuf.tail = (cbuf.tail + 1) & (SIZE - 1);
        }
        return m;
}

ssize_t fops_write(struct file *filp, const char __user *ubuff, size_t cnt, loff_t *off)
{
	int i, ret, m;

        printk("In write system call\n");

	cbuf.buf = kmalloc(SIZE, GFP_KERNEL);

        m = min((int)cnt, CIRC_SPACE(cbuf.head, cbuf.tail, SIZE));

        for(i=0; i<m; i++)
        {
          ret = copy_from_user(cbuf.buf + cbuf.head, ubuff + i, 1);

          if(ret)
          {
             printk("Error copying\n");
             return -1;
          }

          cbuf.head = (cbuf.head + 1) & (SIZE - 1);
        }

        return m;

}

static long module_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int i;

        printk("\nInside ioctl\n");

        switch(cmd)
        {
                case CMD_TEST1:printk("\nInside CMD TEST1\n");
                                printk("\nCMD\n");
                                break;

                case CMD_TEST2:if(!capable(CAP_SYS_ADMIN))
                                   return -EPERM;
				else
                                   printk("\nIn CMD TEST2 DEMO\n");
                                   put_user(30,(int __user *)arg);
                                   break;

		case BUF_CNT:	printk("\nInside buffer count\n");
                             	put_user(CIRC_CNT(cbuf.head, cbuf.tail, SIZE), (int __user *)arg);
                                break;

		case BUF_CLR:if(!capable(CAP_SYS_ADMIN))
                                   return -EPERM;

				   cbuf.head = 0;
				   for(i=0; i<CIRC_CNT(cbuf.head, cbuf.tail, SIZE); i++)
				   {
				     *(cbuf.buf + cbuf.head) = '\0';
				     cbuf.head = ( cbuf.head + 1) & (SIZE - 1);
				   }
                                   break;

		case BUF_DISP:
                                   printk("\nThe data into buffer is: %s\n", cbuf.buf);

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
        .write = fops_write,
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




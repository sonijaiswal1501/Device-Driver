#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

#define MAJORNO 42
#define N_MINORS 1
#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soni sneha");

static dev_t devno;
static char *work_buf;
static struct cdev _cdev;
static struct workqueue_struct *w_q;
static struct work_struct w_struct;

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

        for(i=0; i<15; i++)
        {
          ret = copy_to_user(ubuff + i, work_buf + i,  1);

          if(ret)
          {
             printk("Error copying\n");
             return -1;
          }
//         *(task_buf + i) = '\0';
        }

        return 8;
}

void work_handler(struct work_struct *w_struct)
{
        printk("\nfrom your work queue\n");
        work_buf = kmalloc(16, GFP_KERNEL);
	strcpy(work_buf, "Soni");
}

static struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = char_dev_open,
        .release = char_dev_close,
        .read = char_dev_read,
};

static __init int char_dev_init(void)
{
        int ret;

        devno = MKDEV(MAJORNO, 0);

	printk("Inside Init");
        ret = register_chrdev_region(devno, 1 , "testdev");

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

	w_q = create_singlethread_workqueue("\nsonisneha\n");
        INIT_WORK(&w_struct, work_handler);
        queue_work(w_q , &w_struct);



        return 0;
}

static __exit void char_dev_exit(void)
{
        printk("\nExit\n");

        flush_workqueue(w_q);
	destroy_workqueue(w_q);
        cdev_del(&_cdev);

        unregister_chrdev_region(devno, 1);
}

module_init(char_dev_init);
module_exit(char_dev_exit);




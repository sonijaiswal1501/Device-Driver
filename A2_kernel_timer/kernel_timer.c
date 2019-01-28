#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/timer.h>


#define MAJORNO 42
#define N_MINORS 1
#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soni sneha");


static int flag = 1;
static dev_t devno;
static struct cdev _cdev;
static struct timer_list timer;


void timer_handler(unsigned long delay)
{
	printk("\nInterruption in time handler\n");

	if(flag)
	  mod_timer(&timer, jiffies+HZ*5);

}
static struct file_operations fops = {
        .owner = THIS_MODULE,
};

static __init int char_dev_init(void)
{
	int ret;

        devno = MKDEV(MAJORNO, 0);

        printk("\nInside Init\n");
        ret = register_chrdev_region(devno, 1 , "testdev");

        if(ret)
        {
           printk("Error registering device number\n");
           return ret;
        }

	init_timer(&timer);

        cdev_init(&_cdev, &fops);

        ret = cdev_add(&_cdev, devno, 1);

	setup_timer(&timer, timer_handler , 10);
        mod_timer(&timer, jiffies+HZ*5);

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
	del_timer_sync(&timer);
        cdev_del(&_cdev);

        unregister_chrdev_region(devno, 1);
}

module_init(char_dev_init);
module_exit(char_dev_exit);




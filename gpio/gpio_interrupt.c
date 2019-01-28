#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/circ_buf.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <asm-generic/gpio.h>
#include <linux/irqreturn.h>
#include <linux/irq.h>


#define GPIO_INIT_PIN 115
#define MAJORNO 42
#define N_MINORS 1
#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soni sneha");

static dev_t devno;
static struct cdev _cdev;
static wait_queue_head_t wq;
static struct circ_buf cbuf;
static int irq_num;

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
	int i, ret, m;

        printk("In read system call\n");

	if(filp->f_flags & O_NONBLOCK)
	   goto skip;

        wait_event_interruptible(wq, CIRC_CNT(cbuf.head, cbuf.tail, SIZE));
skip:
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

ssize_t char_dev_write(struct file *filp, const char __user *ubuff, size_t cnt, loff_t *off)
{
	int i, ret, m;

        printk("In write system call\n");

	cbuf.buf = kmalloc(SIZE, GFP_KERNEL);
	if(!cbuf.buf)
        {
           printk("Error allocating memory\n");
           unregister_chrdev_region(devno, 1);
           return -1;
        }

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
        wake_up(&wq);

        return m;
}

irq_handler_t gpio_handler(int irq, void* dev_id, struct pt_regs *regs)
{
		printk("\nGot interrupt\n");
		return IRQ_HANDLED;
}

static struct file_operations fops = {
        .owner = THIS_MODULE,
        .open = char_dev_open,
        .release = char_dev_close,
        .read = char_dev_read,
        .write = char_dev_write,
};

static __init int char_dev_init(void)
{
        int ret;
	devno = MKDEV(MAJORNO, 0);

        printk("Inside Init");
        ret = register_chrdev_region(devno, 1, "testdev");

        if(ret)
        {
           printk("Error registering device number\n");
           return ret;
        }

	ret = gpio_is_valid(GPIO_INIT_PIN);

	ret = gpio_request(GPIO_INIT_PIN,"SYS_115");
	if(ret)
	{
	   printk("\nError in requesting gpio\n");
	   return ret;
	}

	gpio_direction_input(GPIO_INIT_PIN);

	ret = gpio_set_debounce(GPIO_INIT_PIN,200);
	if(ret)
        {
            printk("\nError in setting debounce\n");
	    return ret;
	}

	irq_num = gpio_to_irq(GPIO_INIT_PIN);
	if(irq_num<0)
	{
	    printk("\n Error in getting irq number\n");
	    return ret;
	}

	ret=request_irq(irq_num,(irq_handler_t) gpio_handler,IRQF_TRIGGER_RISING,"GPIO DEMO",NULL);

        init_waitqueue_head(&wq);
        cdev_init(&_cdev, &fops);

        ret = cdev_add(&_cdev, devno, 1);
	if(ret)
        {
           unregister_chrdev_region(devno, 1);
           kfree(cbuf.buf);
           return ret;
        }

        return 0;
}

static __exit void char_dev_exit(void)
{
        printk("\nExit\n");

        kfree(cbuf.buf);
        cdev_del(&_cdev);

        unregister_chrdev_region(devno, 1);
}

module_init(char_dev_init);
module_exit(char_dev_exit);




/*Re-implement the above problem by modifying the blocking operations using the concept of wait queues.*/


#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/circ_buf.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/uaccess.h>


#define MAJORNO 42
#define N_MINORS 1
#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soni sneha");

static dev_t devno;
static struct cdev _cdev;
static wait_queue_head_t wq;
static struct circ_buf cbuf;


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
	printk("In read system call\n");

	int i, ret, m;

	if(filp->f_flags & O_NONBLOCK)
	   goto skip;

	wait_event_interruptible(wq, CIRC_CNT(cbuf.head, cbuf.tail, SIZE));
skip:
	m = min(cnt, CIRC_CNT(cbuf.head, cbuf.tail, SIZE));

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
	printk("In write system call\n");

	int i, ret, m;

	m = min(cnt, CIRC_SPACE(cbuf.head, cbuf.tail, SIZE));

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
	return i;
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
	devno = MKDEV(MAJORNO, 0);
	int ret;
	printk("Inside Init");
	ret = register_chrdev_region(devno, 1, "testdev");

	if(ret)
	{
	   printk("Error registering device number\n");
	   return ret;
	}

	cbuf.buf = kmalloc(SIZE, GFP_KERNEL);

	if(!cbuf.buf)
	{
	   printk("Error allocating memory\n");
	   unregister_chrdev_region(devno, 1);
	   return -1;
	}
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

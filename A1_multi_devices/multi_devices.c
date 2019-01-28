#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/circ_buf.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <linux/kernel.h>


#define N_MINOR 3
#define SIZE 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("soni sneha");

static dev_t devno;
static struct tdev {
	struct cdev _cdev;
	wait_queue_head_t wq;
	struct circ_buf cbuf;
}_tdev[N_MINOR];


int char_dev_open(struct inode *inodp, struct file *filp)
{
        printk("In open call\n");
	filp->private_data = container_of(inodp->i_cdev, struct tdev , _cdev);
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
	struct tdev *_tdev = filp->private_data;
	printk("In read system call\n");

	if(filp->f_flags & O_NONBLOCK)
	   goto skip;

        wait_event_interruptible(_tdev->wq, CIRC_CNT(_tdev->cbuf.head, _tdev->cbuf.tail, SIZE));
skip:
        m = min((int)cnt, CIRC_CNT(_tdev->cbuf.head, _tdev->cbuf.tail, SIZE));

        for(i=0; i<m; i++)
        {
          ret = copy_to_user(ubuff + i, _tdev->cbuf.buf + _tdev->cbuf.tail,  1);

          if(ret)
          {
             printk("Error copying\n");
             return -1;
          }

          _tdev->cbuf.tail = (_tdev->cbuf.tail + 1) & (SIZE - 1);
        }
        return i;
}

ssize_t char_dev_write(struct file *filp, const char __user *ubuff, size_t cnt, loff_t *off)
{
	int i, ret, m;
	struct tdev *_tdev = filp->private_data;
        printk("In write system call\n");

        m = min((int)cnt, CIRC_SPACE(_tdev->cbuf.head, _tdev->cbuf.tail, SIZE));

        for(i=0; i<m; i++)
        {
          ret = copy_from_user(_tdev->cbuf.buf + _tdev->cbuf.head, ubuff + i, 1);

          if(ret)
          {
             printk("Error copying\n");
             return -1;
          }

          _tdev->cbuf.head = (_tdev->cbuf.head + 1) & (SIZE - 1);
        }
        wake_up(&_tdev->wq);
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
	int i, j, ret;
	dev_t tempdev;

	ret = alloc_chrdev_region(&devno, 1 , N_MINOR , "multi_dev");

        printk("Inside Init");

	for(i=0; i<N_MINOR; i++)
	{
           _tdev[i].cbuf.buf = kmalloc(SIZE, GFP_KERNEL);

           if(!_tdev[i].cbuf.buf)
           {
             printk("Error allocating memory\n");
             unregister_chrdev_region(devno, 1);
             return -1;
           }

	   if(!_tdev[i].cbuf.buf)
	      goto cbuf_err;

        init_waitqueue_head(&_tdev[i].wq);
        cdev_init(&_tdev[i]._cdev, &fops);
	}
	for(j=0; j<N_MINOR; j++)
	{
	  tempdev = MKDEV(MAJOR(devno), MINOR(devno)+j);
          ret = cdev_add(&_tdev[j]._cdev, tempdev, 1);
	  if(ret)
             goto cdev_err;
	}
	return 0;

	cdev_err:
		for(--j; j>=0; j--)
	           cdev_del(&_tdev[j]._cdev);
	cbuf_err:
		for(--i; i>=0; i--)
                   kfree(_tdev[i].cbuf.buf);

	unregister_chrdev_region(devno, N_MINOR);

        return -1;
}

static __exit void char_dev_exit(void)
{
	int i;
	printk("\nExit\n");
	for(i=0; i<N_MINOR; i++)
	{
        kfree(_tdev[i].cbuf.buf);
        cdev_del(&_tdev[i]._cdev);
	}

        unregister_chrdev_region(devno, N_MINOR);
}

module_init(char_dev_init);
module_exit(char_dev_exit);




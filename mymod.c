#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kernel_stat.h>
#include<linux/fs.h>
#include<linux/cdev.h>

MODULE_LICENSE("Proprietary");
MODULE_AUTHOR("beibei");

#define K_MAJOR 500
#define K_MINOR 127

struct cdev kyouko3_cdev;

//open the video card
int kyouko3_open(struct inode *inode, struct file *fp)//what is struct inode?
{
    printk(KERN_ALERT "open the card!\n");
    return 0;
}

int kyouko3_release(struct inode *inode, struct file *fp)
{
    printk(KERN_ALERT "release the card!\n");
    return 0;
}

//map the device routine with open,close,etc.
struct file_operations kyouko3_fops = {
    .open = kyouko3_open,
    .release = kyouko3_release,
    .owner = THIS_MODULE
};

//register the video card
int kyouko3_init_func(void)//need to return a value of 0
{
    cdev_init(&kyouko3_cdev,&kyouko3_fops);
    cdev_add(&kyouko3_cdev, MKDEV(K_MAJOR, K_MINOR),1);
    printk(KERN_ALERT "initiate card!\n");
    return 0;
}

//unregister the video card
void kyouko3_exit_func(void)
{
    cdev_del(&kyouko3_cdev);
    printk(KERN_ALERT "delete card!\n");
}

//register the module
module_init(kyouko3_init_func);//need an int return value, or else get warning!
module_exit(kyouko3_exit_func);




#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kernel_stat.h>
#include<linux/fs.h>
#include<linux/pci.h>//need this for pci device!
#include<linux/cdev.h>
#include<linux/delay.h>//need this for delay!

MODULE_LICENSE("Proprietary");
MODULE_AUTHOR("beibei");

#define K_MAJOR 500
#define K_MINOR 127

#define Device_RAM 0x0020
#define K_CONTROL_SIZE 65536

#define PCI_VENDER_ID_CCORSI 0x1234
#define PCI_DEVICE_ID_CCORSI_KYOUKO3 0x1113

struct cdev kyouko3_cdev;

static struct _kyouko3{
    //better to make them as a pointer, as they are exactly pointer to a space.
    unsigned int *k_control_base;
    unsigned int *k_card_ram_base;
    struct pci_dev *pci_dev;
    u32 p_control_base;
    u32 p_ram_card_base;
}kyouko3;





//kernel mode register read (only read control region)
unsigned int K_READ_REG(unsigned int reg)
{
    unsigned int value;
    udelay(1);
    rmb();//read memory barrier, what for? XXX
    //As the pointer doesn't point to a stated argument, we need to convert it to (unsigned int)
    value = *(unsigned int *)(kyouko3.k_control_base + (reg>>2));
    return value;
}


//open the video card
int kyouko3_open(struct inode *inode, struct file *fp)//what is struct inode?
{
    //initiate the base address of virtual address
    unsigned int ram_size;//XXX ISO C90 only permit to put declaration in the front!

    //ioremap() used to re map physical address ( initiate in init function ) to virtual address.( a size is given )
    kyouko3.k_control_base = (unsigned int *)ioremap(kyouko3.p_control_base, K_CONTROL_SIZE);

    ram_size = K_READ_REG(Device_RAM);
    ram_size *= (1024*1024); //1MB
    kyouko3.k_card_ram_base = (unsigned int *)ioremap(kyouko3.p_ram_card_base,ram_size);

    printk(KERN_ALERT "open the card!\n");
    return 0;
}

int kyouko3_release(struct inode *inode, struct file *fp)
{
    iounmap(kyouko3.k_control_base);
    iounmap(kyouko3.k_card_ram_base);

    printk(KERN_ALERT "release the card!\n");
    return 0;
}

//used to give user access to the 64KB control region of the card.
int kyouko3_mmap(struct file *fp, struct vm_area_struct *vma)//XXX the return value must be INT!!!
{
    //MUST USE PARA FOR TRANSFER
    int ret;
    ret = io_remap_pfn_range(vma, vma->vm_start, kyouko3.p_control_base>>PAGE_SHIFT, vma->vm_end-vma->vm_start, vma->vm_page_prot);
    return ret;
}


int kyouko3_probe(struct pci_dev *pci_dev, const struct pci_device_id * pci_id)// what is the different with *pci_id and * pci_id?
{
    int dev;
    kyouko3.p_control_base = pci_resource_start(pci_dev,1);
    kyouko3.p_ram_card_base = pci_resource_start(pci_dev,2);
    //pci_enable_device return the device !!
    dev = pci_enable_device(pci_dev);
    if(dev){//
        printk(KERN_ALERT "enable device: %d\n",dev);
        return dev;
    }
    pci_set_master(pci_dev);
    return 0;
}

//an array of device ids, device id can various? Device can be various? XXX
struct pci_device_id kyouko3_dev_ids[] = {
    {PCI_DEVICE(PCI_VENDER_ID_CCORSI, PCI_DEVICE_ID_CCORSI_KYOUKO3)},
    {0}
};

//remove device from pci
void kyouko3_remove(struct pci_dev *pci_dev)
{
    pci_disable_device(pci_dev);
}

//some basic information of the video card
struct pci_driver kyouko3_pci_drv = {
    .name = "kyouko_video_card",
    .id_table = kyouko3_dev_ids,
    .probe = kyouko3_probe,
    .remove = kyouko3_remove
};
// XXX what are probe and remove?

//map the device routine with open,close,etc.
struct file_operations kyouko3_fops = {
    .open = kyouko3_open,
    .mmap = kyouko3_mmap,
    .release = kyouko3_release,
    .owner = THIS_MODULE
};

//register the video card
int kyouko3_init_func(void)//need to return a value of 0
{
    int ret;
    cdev_init(&kyouko3_cdev,&kyouko3_fops);// XXX
    cdev_add(&kyouko3_cdev, MKDEV(K_MAJOR, K_MINOR),1);//XXX
    //will use the argument to load up the table device-vendor pairs, search the pci bus for matching entries
    //register
    ret = pci_register_driver(&kyouko3_pci_drv);//finding the device!
    if(ret){
        printk(KERN_ALERT "pci regist device error: %d\n", ret);
        return ret;
    }
    printk(KERN_ALERT "initiate module!\n");
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

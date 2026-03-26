#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

#define DEV_NAME "ch2_mod"

#define CH2_IOCTL_NUM 'c'

#define CH2_GET _IOWR(CH2_IOCTL_NUM, 1, unsigned long)
#define CH2_SET _IOWR(CH2_IOCTL_NUM, 2, unsigned long)
#define CH2_ADD _IOWR(CH2_IOCTL_NUM, 3, unsigned long)
#define CH2_MUL _IOWR(CH2_IOCTL_NUM, 4, unsigned long)

static long result = 0;
static dev_t dev_num;
static struct cdev* cd_cdev;

static int ch2_open(struct inode *inode, struct file *file)
{
    printk("ch2_mod: open\n");
    return 0;
}

static int ch2_release(struct inode *inode, struct file *file)
{
    printk("ch2_mod: release\n");
    return 0;
}

static long ch2_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch(cmd) {
        case CH2_GET:
            printk("ch2_mod: GET result = %ld\n", result);
            return result;
            
        case CH2_SET:
            result = (long)arg;
            printk("ch2_mod: SET result to %ld\n", result);
            return result;
            
        case CH2_ADD:
            result += (long)arg;
            printk("ch2_mod: ADD %ld, result = %ld\n", (long)arg, result);
            return result;
            
        case CH2_MUL:
            result *= (long)arg;
            printk("ch2_mod: MUL %ld, result = %ld\n", (long)arg, result);
            return result;
            
        default:
            return -1; 
    }
}
struct file_operations ch2_fops = {
    .open = ch2_open,
    .release = ch2_release,
    .unlocked_ioctl = ch2_ioctl,
};

static int __init ch2_init(void) {
    printk("ch2_mod: init module\n");
    alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &ch2_fops);
    cdev_add(cd_cdev, dev_num, 1);
    return 0;
}

static void __exit ch2_exit(void) {
    printk("ch2_mod: exit module\n");
    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(ch2_init);
module_exit(ch2_exit);

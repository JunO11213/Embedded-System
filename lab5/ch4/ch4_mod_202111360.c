#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include "ch4.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Message Queue Kernel Module with Blocking I/O");

#define DEV_NAME "ch4_dev"

struct msg_list {
    struct list_head list;
    struct msg_st *msg;
};

static struct msg_list msg_list_head;
static spinlock_t my_lock;
static dev_t dev_num;
static struct cdev *cd_cdev;
static DECLARE_WAIT_QUEUE_HEAD(my_wq);

static int ch4_read(struct msg_st __user *user_buf)
{
    struct msg_list *first_node;
    struct msg_st temp_msg;
    int ret;

    /* 메시지가 들어올 때까지 block */
    ret = wait_event_interruptible(my_wq, !list_empty(&msg_list_head.list));
    if (ret < 0) {
        return ret;   /* signal로 깬 경우 */
    }

    spin_lock(&my_lock);

    /* 깨어난 뒤 다시 확인 */
    if (list_empty(&msg_list_head.list)) {
        spin_unlock(&my_lock);
        return -EFAULT;
    }

    first_node = list_entry(msg_list_head.list.next, struct msg_list, list);
    memcpy(&temp_msg, first_node->msg, sizeof(struct msg_st));
    list_del(&first_node->list);

    spin_unlock(&my_lock);

    if (copy_to_user(user_buf, &temp_msg, sizeof(struct msg_st)) != 0) {
        kfree(first_node->msg);
        kfree(first_node);
        return -EFAULT;
    }

    kfree(first_node->msg);
    kfree(first_node);

    return 0;
}

static int ch4_write(struct msg_st __user *user_buf)
{
    struct msg_list *new_node;

    new_node = kmalloc(sizeof(struct msg_list), GFP_KERNEL);
    if (!new_node) {
        return -ENOMEM;
    }

    new_node->msg = kmalloc(sizeof(struct msg_st), GFP_KERNEL);
    if (!new_node->msg) {
        kfree(new_node);
        return -ENOMEM;
    }

    if (copy_from_user(new_node->msg, user_buf, sizeof(struct msg_st)) != 0) {
        kfree(new_node->msg);
        kfree(new_node);
        return -EFAULT;
    }

    spin_lock(&my_lock);
    list_add_tail(&new_node->list, &msg_list_head.list);
    spin_unlock(&my_lock);

    /* reader 깨우기 */
    wake_up_interruptible(&my_wq);

    return 0;
}

static long ch4_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct msg_st __user *user_buf = (struct msg_st __user *)arg;

    switch (cmd) {
    case CH4_IOCTL_READ:
        return ch4_read(user_buf);

    case CH4_IOCTL_WRITE:
        return ch4_write(user_buf);

    default:
        return -EINVAL;
    }
}

static const struct file_operations ch4_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = ch4_ioctl,
};

static int __init ch4_init(void)
{
    int err;

    printk(KERN_INFO "ch4_mod Init\n");

    err = alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    if (err < 0) {
        return err;
    }

    cd_cdev = cdev_alloc();
    if (!cd_cdev) {
        unregister_chrdev_region(dev_num, 1);
        return -ENOMEM;
    }

    cdev_init(cd_cdev, &ch4_fops);
    cd_cdev->owner = THIS_MODULE;

    err = cdev_add(cd_cdev, dev_num, 1);
    if (err < 0) {
        printk(KERN_ERR "ch4_mod fail\n");
        kfree(cd_cdev);
        unregister_chrdev_region(dev_num, 1);
        return err;
    }

    INIT_LIST_HEAD(&msg_list_head.list);
    spin_lock_init(&my_lock);

    return 0;
}

static void __exit ch4_exit(void)
{
    struct msg_list *tmp_node;
    struct list_head *pos, *q;

    printk(KERN_INFO "ch4_mod Exit\n");

    /* 혹시 자고 있는 reader가 있으면 깨움 */
    wake_up_interruptible(&my_wq);

    list_for_each_safe(pos, q, &msg_list_head.list) {
        tmp_node = list_entry(pos, struct msg_list, list);
        list_del(pos);
        kfree(tmp_node->msg);
        kfree(tmp_node);
    }

    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(ch4_init);
module_exit(ch4_exit);
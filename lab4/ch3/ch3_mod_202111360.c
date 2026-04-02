#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/list.h>
#include "ch3.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Message Queue Kernel Module");

#define DEV_NAME "ch3_dev"

struct msg_list {
    struct list_head list;
    struct msg_st *msg;
};

static struct msg_list msg_list_head;
spinlock_t my_lock;
static dev_t dev_num;
static struct cdev *cd_cdev;

static int ch3_read(struct msg_st *user_buf) {
    struct msg_list *first_node;
    struct msg_st temp_msg;
    int ret;

    spin_lock(&my_lock);
    
    if (list_empty(&msg_list_head.list)) {
        spin_unlock(&my_lock);
        
        memset(&temp_msg, 0, sizeof(struct msg_st));
        temp_msg.str[0] = '0';
        temp_msg.str[1] = '\0';
        
        ret = copy_to_user(user_buf, &temp_msg, sizeof(struct msg_st));
    } 
    else {
        first_node = list_entry(msg_list_head.list.next, struct msg_list, list);
        memcpy(&temp_msg, first_node->msg, sizeof(struct msg_st));
        
        list_del(&first_node->list);
        spin_unlock(&my_lock);

        ret = copy_to_user(user_buf, &temp_msg, sizeof(struct msg_st));

        kfree(first_node->msg);
        kfree(first_node);
    }
    
    return ret;
}

static int ch3_write(struct msg_st *user_buf) {
    struct msg_list *new_node;
    int ret;

    new_node = (struct msg_list *)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
    new_node->msg = (struct msg_st *)kmalloc(sizeof(struct msg_st), GFP_KERNEL);

    ret = copy_from_user(new_node->msg, user_buf, sizeof(struct msg_st));

    spin_lock(&my_lock);
    list_add_tail(&new_node->list, &msg_list_head.list);
    spin_unlock(&my_lock);

    return ret;
}

static long ch3_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct msg_st *user_buf = (struct msg_st *)arg;
    int ret;

    switch(cmd) {
        case CH3_IOCTL_READ:
            ret = ch3_read(user_buf);
            break;

        case CH3_IOCTL_WRITE:
            ret = ch3_write(user_buf);
            break;
    }
    return 0;
}

struct file_operations ch3_fops = {
    .unlocked_ioctl = ch3_ioctl,
};

static int __init ch3_init(void) {
    int err;
    printk("ch3_mod Init\n");

    alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &ch3_fops);

    err = cdev_add(cd_cdev, dev_num, 1);
    if (err < 0) {
        printk(KERN_ERR "ch3_mod fail\n");
        return err;
    }

    INIT_LIST_HEAD(&msg_list_head.list);
    spin_lock_init(&my_lock);

    return 0;
}

static void __exit ch3_exit(void) {
    struct msg_list *tmp_node;
    struct list_head *pos, *q;

    printk("ch3_mod Exit\n");

    list_for_each_safe(pos, q, &msg_list_head.list) {
        tmp_node = list_entry(pos, struct msg_list, list);
        list_del(pos);
        kfree(tmp_node->msg);
        kfree(tmp_node);
    }

    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(ch3_init);
module_exit(ch3_exit);
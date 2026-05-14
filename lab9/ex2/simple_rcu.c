#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/rcupdate.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple example for RCU");

#define DEV_NAME "simple_rcu_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2

#define SIMPLE_IOCTL_NUM 'z'
#define IOCTL_READ  _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM1, unsigned long *)
#define IOCTL_WRITE _IOWR(SIMPLE_IOCTL_NUM, IOCTL_NUM2, unsigned long *)

static dev_t dev_num;
static struct cdev *cd_cdev;

unsigned long *my_data;

static long simple_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    unsigned long *new, *old;

    switch(cmd){

        case IOCTL_READ:
            rcu_read_lock(); // RCU Read Lock 획득 (Preemption disable)
            old = rcu_dereference(my_data); // RCU 포인터 참조
            
            if(old != NULL){
                printk("simple_rcu : [R] Read my_data = %ld\n", *old);
                mdelay(500); // 지연 발생시켜 동작 확인
                printk("simple_rcu : [R] After delay, Read my_data = %ld\n", *old);
            } else {
                printk("simple_rcu : [R] my_data is NULL\n");
            }

            rcu_read_unlock(); // RCU Read Lock 해제
            break;

        case IOCTL_WRITE:
            printk("simple_rcu : [W] write new data = %ld\n", arg);
            
            // 1. Copy & Update: 새로운 메모리 할당 및 값 쓰기
            new = (unsigned long *)kmalloc(sizeof(unsigned long), GFP_KERNEL);
            *new = arg;
            
            old = my_data;
            
            // 2. 포인터 변경: 새로운 Reader는 이제 새 데이터를 읽음
            rcu_assign_pointer(my_data, new);
            mdelay(200);
            
            // 3. 기존 원본 데이터를 읽고 있던 Reader가 끝날 때까지 대기
            synchronize_rcu();
            
            // 4. 이전 데이터 해제
            if(old != NULL)
                kfree(old);
                
            break;

        default:
            return -1;
    }

    return 0;
}

static int simple_rcu_open(struct inode *inode, struct file *file){
    return 0;
}

static int simple_rcu_release(struct inode *inode, struct file *file){
    return 0;
}

struct file_operations simple_rcu_fops = {
    .unlocked_ioctl = simple_ioctl,
    .open = simple_rcu_open,
    .release = simple_rcu_release
};

static int __init simple_rcu_init(void){
    my_data = (unsigned long *)kmalloc(sizeof(unsigned long), GFP_KERNEL);
    *my_data = 0;

    alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &simple_rcu_fops);
    cdev_add(cd_cdev, dev_num, 1);

    printk("simple_rcu : init module\n");
    return 0;
}

static void __exit simple_rcu_exit(void){
    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);

    if(my_data != NULL){
        kfree(my_data);
        my_data = NULL;
    }

    printk("simple_rcu : exit module\n");
}

module_init(simple_rcu_init);
module_exit(simple_rcu_exit);
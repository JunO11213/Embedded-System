/*
 * pi2_led_driver.c
 * Raspberry Pi 2 server-side LED character device driver
 *
 * Device file: /dev/pi2_led_dev
 * Usage from user app:
 *   write(fd, "1", 1);  // LED ON
 *   write(fd, "0", 1);  // LED OFF
 *
 * GPIO number uses BCM GPIO number by default.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/mutex.h>
#include <linux/version.h>

#define DEVICE_NAME "pi2_led_driver"
#define DEVFILE_NAME "pi2_led_dev"
#define DEFAULT_LED_GPIO 5

static int led_gpio = DEFAULT_LED_GPIO;
module_param(led_gpio, int, 0644);
MODULE_PARM_DESC(led_gpio, "BCM GPIO number for inside LED");

static dev_t dev_num;
static struct cdev pi2_led_cdev;
static struct class *pi2_led_class;
static DEFINE_MUTEX(led_lock);
static int led_state;

static int pi2_led_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "pi2_led_driver: open\n");
    return 0;
}

static int pi2_led_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "pi2_led_driver: release\n");
    return 0;
}

static ssize_t pi2_led_read(struct file *file, char __user *buf,
                            size_t count, loff_t *ppos)
{
    char kbuf[2];

    if (*ppos > 0)
        return 0;

    mutex_lock(&led_lock);
    kbuf[0] = led_state ? '1' : '0';
    mutex_unlock(&led_lock);
    kbuf[1] = '\n';

    if (count < sizeof(kbuf))
        return -EINVAL;

    if (copy_to_user(buf, kbuf, sizeof(kbuf)))
        return -EFAULT;

    *ppos += sizeof(kbuf);
    return sizeof(kbuf);
}

static ssize_t pi2_led_write(struct file *file, const char __user *buf,
                             size_t count, loff_t *ppos)
{
    char cmd;

    if (count < 1)
        return -EINVAL;

    if (copy_from_user(&cmd, buf, 1))
        return -EFAULT;

    mutex_lock(&led_lock);

    if (cmd == '1') {
        gpio_set_value(led_gpio, 1);
        led_state = 1;
        printk(KERN_INFO "pi2_led_driver: LED ON\n");
    } else if (cmd == '0') {
        gpio_set_value(led_gpio, 0);
        led_state = 0;
        printk(KERN_INFO "pi2_led_driver: LED OFF\n");
    } else {
        mutex_unlock(&led_lock);
        printk(KERN_WARNING "pi2_led_driver: invalid command %c\n", cmd);
        return -EINVAL;
    }

    mutex_unlock(&led_lock);
    return count;
}

static const struct file_operations pi2_led_fops = {
    .owner = THIS_MODULE,
    .open = pi2_led_open,
    .release = pi2_led_release,
    .read = pi2_led_read,
    .write = pi2_led_write,
};

static int __init pi2_led_init(void)
{
    int ret;

    printk(KERN_INFO "pi2_led_driver: init, led_gpio=%d\n", led_gpio);

    if (!gpio_is_valid(led_gpio)) {
        printk(KERN_ERR "pi2_led_driver: invalid GPIO %d\n", led_gpio);
        return -EINVAL;
    }

    ret = gpio_request(led_gpio, "pi2_inside_led");
    if (ret) {
        printk(KERN_ERR "pi2_led_driver: gpio_request failed: %d\n", ret);
        return ret;
    }

    ret = gpio_direction_output(led_gpio, 0);
    if (ret) {
        printk(KERN_ERR "pi2_led_driver: gpio_direction_output failed: %d\n", ret);
        gpio_free(led_gpio);
        return ret;
    }
    led_state = 0;

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "pi2_led_driver: alloc_chrdev_region failed\n");
        gpio_free(led_gpio);
        return ret;
    }

    cdev_init(&pi2_led_cdev, &pi2_led_fops);
    pi2_led_cdev.owner = THIS_MODULE;

    ret = cdev_add(&pi2_led_cdev, dev_num, 1);
    if (ret < 0) {
        printk(KERN_ERR "pi2_led_driver: cdev_add failed\n");
        unregister_chrdev_region(dev_num, 1);
        gpio_free(led_gpio);
        return ret;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 4, 0)
    pi2_led_class = class_create(DEVICE_NAME);
#else
    pi2_led_class = class_create(THIS_MODULE, DEVICE_NAME);
#endif
    if (IS_ERR(pi2_led_class)) {
        printk(KERN_ERR "pi2_led_driver: class_create failed\n");
        cdev_del(&pi2_led_cdev);
        unregister_chrdev_region(dev_num, 1);
        gpio_free(led_gpio);
        return PTR_ERR(pi2_led_class);
    }

    if (IS_ERR(device_create(pi2_led_class, NULL, dev_num, NULL, DEVFILE_NAME))) {
        printk(KERN_ERR "pi2_led_driver: device_create failed\n");
        class_destroy(pi2_led_class);
        cdev_del(&pi2_led_cdev);
        unregister_chrdev_region(dev_num, 1);
        gpio_free(led_gpio);
        return -EINVAL;
    }

    printk(KERN_INFO "pi2_led_driver: registered major=%d minor=%d\n",
           MAJOR(dev_num), MINOR(dev_num));
    printk(KERN_INFO "pi2_led_driver: device file may be /dev/%s\n", DEVFILE_NAME);

    return 0;
}

static void __exit pi2_led_exit(void)
{
    printk(KERN_INFO "pi2_led_driver: exit\n");

    gpio_set_value(led_gpio, 0);
    led_state = 0;

    device_destroy(pi2_led_class, dev_num);
    class_destroy(pi2_led_class);
    cdev_del(&pi2_led_cdev);
    unregister_chrdev_region(dev_num, 1);
    gpio_free(led_gpio);
}

module_init(pi2_led_init);
module_exit(pi2_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ESS Team Project - Pi2");
MODULE_DESCRIPTION("Pi2 inside LED character device driver");

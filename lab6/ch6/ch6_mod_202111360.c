#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");

#define LED_GPIO    5
#define SWITCH_GPIO 12

struct ch6_timer_info {
    struct timer_list timer;
    unsigned long delay_jiffies;
};

static struct ch6_timer_info my_timer;

static void ch6_timer_func(struct timer_list *t)
{
    int sw;

    sw = gpio_get_value(SWITCH_GPIO);

    if (sw == 1) {
        gpio_set_value(LED_GPIO, 1);
        printk(KERN_INFO "ch6: switch=1, LED ON\n");
    } else {
        gpio_set_value(LED_GPIO, 0);
        printk(KERN_INFO "ch6: switch=0, LED OFF\n");
    }

    mod_timer(&my_timer.timer, jiffies + my_timer.delay_jiffies);
}

static int __init ch6_init(void)
{
    int ret;

    printk(KERN_INFO "ch6: init\n");

    ret = gpio_request_one(LED_GPIO, GPIOF_OUT_INIT_LOW, "ch6_led");
    if (ret) {
        printk(KERN_ERR "ch6: failed to request LED GPIO\n");
        return ret;
    }

    ret = gpio_request_one(SWITCH_GPIO, GPIOF_IN, "ch6_switch");
    if (ret) {
        printk(KERN_ERR "ch6: failed to request SWITCH GPIO\n");
        gpio_free(LED_GPIO);
        return ret;
    }

    my_timer.delay_jiffies = msecs_to_jiffies(500);

    timer_setup(&my_timer.timer, ch6_timer_func, 0);
    my_timer.timer.expires = jiffies + my_timer.delay_jiffies;
    add_timer(&my_timer.timer);

    return 0;
}

static void __exit ch6_exit(void)
{
    printk(KERN_INFO "ch6: exit\n");

    del_timer(&my_timer.timer);
    gpio_set_value(LED_GPIO, 0);

    gpio_free(SWITCH_GPIO);
    gpio_free(LED_GPIO);
}

module_init(ch6_init);
module_exit(ch6_exit);
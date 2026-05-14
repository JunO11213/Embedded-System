#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/spinlock.h>

#define PIR_GPIO 17
#define LED_GPIO 5

static int pir_irq;
static int led_on = 0;

static struct timer_list led_timer;
static DEFINE_SPINLOCK(led_lock);

/*
 * 10초가 지나면 호출되는 타이머 함수
 * 역할: LED를 끄고 led_on 상태를 0으로 변경
 */
static void led_timer_func(struct timer_list *t)
{
    unsigned long flags;

    spin_lock_irqsave(&led_lock, flags);

    gpio_set_value(LED_GPIO, 0);
    led_on = 0;

    spin_unlock_irqrestore(&led_lock, flags);

    printk(KERN_INFO "ch7_mod: timer expired, LED OFF\n");
}

/*
 * PIR 센서에서 인체 감지가 발생하면 호출되는 인터럽트 핸들러
 * 역할:
 * 1. LED가 이미 켜져 있으면 한 번 끔
 * 2. LED를 다시 켬
 * 3. 10초 타이머를 새로 설정
 */
static irqreturn_t pir_irq_handler(int irq, void *dev_id)
{
    unsigned long flags;

    printk(KERN_INFO "ch7_mod: PIR detected\n");

    spin_lock_irqsave(&led_lock, flags);

    /*
     * 과제 핵심 조건:
     * LED가 켜져 있는 동안 다시 감지되면
     * 반드시 한 번 끄는 과정이 들어가야 함
     */
    if (led_on) {
        gpio_set_value(LED_GPIO, 0);
        printk(KERN_INFO "ch7_mod: LED briefly OFF\n");
    }

    gpio_set_value(LED_GPIO, 1);
    led_on = 1;

    /*
     * 현재 시각 기준 10초 뒤에 led_timer_func 실행
     * 이미 타이머가 동작 중이면 새 시간으로 갱신됨
     */
    mod_timer(&led_timer, jiffies + 10 * HZ);

    spin_unlock_irqrestore(&led_lock, flags);

    printk(KERN_INFO "ch7_mod: LED ON for 10 seconds\n");

    return IRQ_HANDLED;
}

/*
 * 모듈 삽입 시 실행
 */
static int __init ch7_mod_init(void)
{
    int ret;

    printk(KERN_INFO "ch7_mod: init module\n");

    /*
     * 1. PIR GPIO 요청
     */
    ret = gpio_request(PIR_GPIO, "pir_gpio");
    if (ret) {
        printk(KERN_ERR "ch7_mod: failed to request PIR GPIO\n");
        return ret;
    }

    /*
     * 2. PIR GPIO를 입력으로 설정
     */
    ret = gpio_direction_input(PIR_GPIO);
    if (ret) {
        printk(KERN_ERR "ch7_mod: failed to set PIR GPIO as input\n");
        gpio_free(PIR_GPIO);
        return ret;
    }

    /*
     * 3. LED GPIO 요청
     */
    ret = gpio_request(LED_GPIO, "led_gpio");
    if (ret) {
        printk(KERN_ERR "ch7_mod: failed to request LED GPIO\n");
        gpio_free(PIR_GPIO);
        return ret;
    }

    /*
     * 4. LED GPIO를 출력으로 설정, 초기값은 OFF
     */
    ret = gpio_direction_output(LED_GPIO, 0);
    if (ret) {
        printk(KERN_ERR "ch7_mod: failed to set LED GPIO as output\n");
        gpio_free(LED_GPIO);
        gpio_free(PIR_GPIO);
        return ret;
    }

    /*
     * 5. 타이머 초기화
     */
    timer_setup(&led_timer, led_timer_func, 0);

    /*
     * 6. PIR GPIO 번호를 IRQ 번호로 변환
     */
    pir_irq = gpio_to_irq(PIR_GPIO);
    if (pir_irq < 0) {
        printk(KERN_ERR "ch7_mod: failed to get IRQ number\n");
        gpio_free(LED_GPIO);
        gpio_free(PIR_GPIO);
        return pir_irq;
    }

    /*
     * 7. 인터럽트 핸들러 등록
     * IRQF_TRIGGER_RISING:
     * PIR OUT 신호가 LOW에서 HIGH로 변할 때 인터럽트 발생
     */
    ret = request_irq(
        pir_irq,
        pir_irq_handler,
        IRQF_TRIGGER_RISING,
        "pir_irq",
        NULL
    );

    if (ret) {
        printk(KERN_ERR "ch7_mod: failed to request IRQ\n");
        del_timer_sync(&led_timer);
        gpio_free(LED_GPIO);
        gpio_free(PIR_GPIO);
        return ret;
    }

    printk(KERN_INFO "ch7_mod: module loaded successfully\n");

    return 0;
}

/*
 * 모듈 제거 시 실행
 */
static void __exit ch7_mod_exit(void)
{
    printk(KERN_INFO "ch7_mod: exit module\n");

    /*
     * 타이머 제거
     */
    del_timer_sync(&led_timer);

    /*
     * 인터럽트 해제
     */
    free_irq(pir_irq, NULL);

    /*
     * LED 끄기
     */
    gpio_set_value(LED_GPIO, 0);
    led_on = 0;

    /*
     * GPIO 해제
     */
    gpio_free(LED_GPIO);
    gpio_free(PIR_GPIO);

    printk(KERN_INFO "ch7_mod: module unloaded\n");
}

module_init(ch7_mod_init);
module_exit(ch7_mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("202111360");
MODULE_DESCRIPTION("Challenge #7: PIR Sensor + LED + Timer");
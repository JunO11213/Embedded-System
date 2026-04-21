#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

#define SWITCH 12

static int count = 0;

static int __init simple_switch_init(void){
    int ret = 0;
    printk("simple_switch: init module \n");
    
    // GPIO 12번을 입력(IN) 모드로 요청
    gpio_request_one(SWITCH, GPIOF_IN, "SWITCH");

    // count가 5가 될 때까지 반복
    while(count != 5){
        ret = gpio_get_value(SWITCH); // 스위치 상태 읽기
        printk("ret = %d \n", ret);

        if(ret){ // 스위치가 눌렸다면 (High 신호 시)
            count++;
            printk("simple_switch: Pushed Button, Count = %d\n", count);
        }
        
        // 1초(1000ms) 대기
        msleep(1000);
    }

    printk("simple_switch : Finish Pushed Button\n");

    return 0;
}

static void __exit simple_switch_exit(void){
    printk("simple_switch : exit module\n");
    
    // 사용한 GPIO 자원 해제
    gpio_free(SWITCH);
}

module_init(simple_switch_init);
module_exit(simple_switch_exit);
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");

// LED 연결 핀 번호 정의 (GPIO 5)
// 하드웨어 연결: 긴 쪽(+)은 저항을 거쳐 GPIO핀에, 짧은 쪽(-)은 GND에 연결
#define LED1 5 

static int __init simple_led_init(void){
    printk("Hello LED\n");
    
    // GPIO 사용 요청 및 초기 상태를 LOW(0)로 설정
    gpio_request_one(LED1, GPIOF_OUT_INIT_LOW, "LED1");
    
    // LED 켜기 (하이 레벨 신호 출력)
    gpio_set_value(LED1, 1);

    return 0;
}

static void __exit simple_led_exit(void){
    printk("Bye LED \n");

    // LED 끄기 (로우 레벨 신호 출력)
    gpio_set_value(LED1, 0);

    // 사용했던 GPIO 자원 해제
    gpio_free(LED1);
}

module_init(simple_led_init);
module_exit(simple_led_exit);
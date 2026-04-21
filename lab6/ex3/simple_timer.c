#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

// 타이머 정보를 담을 구조체 정의
struct my_timer_info{
    struct timer_list timer;
    long delay_jiffies;
    int data;
};

static struct my_timer_info my_timer;

// 타이머가 만료되었을 때 호출되는 콜백 함수
static void my_timer_func(struct timer_list *t){
    // timer_list 구조체를 포함하는 상위 구조체(my_timer_info)의 주소를 찾음
    struct my_timer_info *info = from_timer(info, t, timer);

    printk("simple_timer: Jiffies %ld, Data %d \n", jiffies, info->data);
    
    info->data++;

    // 타이머 재설정 (주기적인 반복을 위함)
    mod_timer(&my_timer.timer, jiffies + info->delay_jiffies);
}

static int __init simple_timer_init(void){
    printk("simple_timer: init modules \n");

    // 2초를 jiffies 단위로 변환 (1초 = HZ)
    my_timer.delay_jiffies = msecs_to_jiffies(2000); /* 2 seconds */
    my_timer.data = 100;

    // 타이머 설정 (함수 연결)
    timer_setup(&my_timer.timer, my_timer_func, 0);

    // 타이머 만료 시간 설정 및 커널에 등록
    my_timer.timer.expires = jiffies + my_timer.delay_jiffies;
    add_timer(&my_timer.timer);

    return 0;
}

static void __exit simple_timer_exit(void){
    printk("simple_timer: exit modules\n");

    // 모듈 종료 시 타이머 제거
    del_timer(&my_timer.timer);
}

module_init(simple_timer_init);
module_exit(simple_timer_exit);
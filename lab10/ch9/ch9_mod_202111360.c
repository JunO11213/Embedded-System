#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

#define PIN1 6
#define PIN2 13
#define PIN3 19
#define PIN4 26

#define STEPS 8
#define ONEROUND 512

// 교재 50페이지 제어 테이블(Half-step)을 기준으로 오타 수정된 배열 값
int blue[8]   = {1, 1, 0, 0, 0, 0, 0, 1};
int pink[8]   = {0, 1, 1, 1, 0, 0, 0, 0};
int yellow[8] = {0, 0, 0, 1, 1, 1, 0, 0};
int orange[8] = {0, 0, 0, 0, 0, 1, 1, 1};

void setstep(int p1, int p2, int p3, int p4) {
    gpio_set_value(PIN1, p1);
    gpio_set_value(PIN2, p2);
    gpio_set_value(PIN3, p3);
    gpio_set_value(PIN4, p4);
}

// 챌린지 9: 특정 각도를 움직일 수 있는 함수
void moveDegree(int degree, int delay, int direction) {
    int i = 0, j = 0;
    
    // 360도 회전 시 필요한 라운드 수가 512(ONEROUND)이므로, 이를 비례식으로 계산
    int target_rounds = (degree * ONEROUND) / 360;

    for(i = 0; i < target_rounds; i++) {
        if (direction == 0) { 
            // 0 = forward (시계 방향)
            for(j = 0; j < STEPS; j++) {
                setstep(blue[j], pink[j], yellow[j], orange[j]);
                udelay(delay);
            }
        } else if (direction == 1) { 
            // 1 = backward (반시계 방향)
            // 반시계 방향은 시계 방향 스텝의 역순으로 동작
            for(j = STEPS - 1; j >= 0; j--) {
                setstep(blue[j], pink[j], yellow[j], orange[j]);
                udelay(delay);
            }
        }
    }
    // 동작이 끝난 후 모터의 부하를 줄이기 위해 모든 핀의 출력을 0으로 설정
    setstep(0, 0, 0, 0); 
}

static int __init simple_motor_init(void) {
    gpio_request_one(PIN1, GPIOF_OUT_INIT_LOW, "p1");
    gpio_request_one(PIN2, GPIOF_OUT_INIT_LOW, "p2");
    gpio_request_one(PIN3, GPIOF_OUT_INIT_LOW, "p3");
    gpio_request_one(PIN4, GPIOF_OUT_INIT_LOW, "p4");

    // 교재 59페이지 챌린지 요구 동작 순서
    moveDegree(90, 3000, 0);   // 시계 방향 90도
    mdelay(1200);              // 1.2초 대기
    
    moveDegree(45, 3000, 1);   // 반시계 방향 45도
    mdelay(1200);              // 1.2초 대기
    
    moveDegree(45, 3000, 1);   // 반시계 방향 45도
    mdelay(1200);              // 1.2초 대기
    
    moveDegree(180, 3000, 0);  // 시계 방향 180도

    return 0;
}

static void __exit simple_motor_exit(void) {
    gpio_free(PIN1);
    gpio_free(PIN2);
    gpio_free(PIN3);
    gpio_free(PIN4);
}

module_init(simple_motor_init);
module_exit(simple_motor_exit);
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include "ch3.h"

int main(void) {
    int dev;
    struct msg_st user_str;

    // 디바이스 열고 ioctl로 읽어오기 (Dequeue)
    dev = open("/dev/ch3_dev", O_RDWR);
    if (dev < 0) {
        printf("디바이스 파일을 열 수 없습니다.\n");
        return -1;
    }

    ioctl(dev, CH3_IOCTL_READ, &user_str);
    
    // 예외처리: 비어있을 때 '0'이 반환됨
    if (strcmp(user_str.str, "0") == 0) {
        printf("Reader: 큐가 비어있습니다!\n");
    } else {
        printf("Reader: 큐에서 [%s] 메시지를 꺼냈습니다.\n", user_str.str);
    }

    close(dev);
    return 0;
}
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include "ch4.h"

int main(void)
{
    int dev;
    int ret;
    struct msg_st user_str;

    dev = open("/dev/ch4_dev", O_RDWR);
    if (dev < 0) {
        printf("디바이스 파일을 열 수 없습니다.\n");
        return -1;
    }

    ret = ioctl(dev, CH4_IOCTL_READ, &user_str);
    if (ret < 0) {
        perror("ioctl");
        close(dev);
        return -1;
    }

    printf("Reader: 큐에서 [%s] 메시지를 꺼냈습니다.\n", user_str.str);

    close(dev);
    return 0;
}
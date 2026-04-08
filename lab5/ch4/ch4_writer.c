#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include "ch4.h"

int main(int argc, char *argv[])
{
    int dev;
    int ret;
    struct msg_st user_str;

    if (argc != 2) {
        printf("사용법: sudo ./ch4_writer \"보낼 메시지\"\n");
        return -1;
    }

    strncpy(user_str.str, argv[1], 127);
    user_str.str[127] = '\0';
    user_str.len = strlen(user_str.str);

    dev = open("/dev/ch4_dev", O_RDWR);
    if (dev < 0) {
        printf("디바이스 파일을 열 수 없습니다.\n");
        return -1;
    }

    ret = ioctl(dev, CH4_IOCTL_WRITE, &user_str);
    if (ret < 0) {
        perror("ioctl");
        close(dev);
        return -1;
    }

    printf("Writer: 큐에 [%s] 메시지를 넣었습니다.\n", user_str.str);

    close(dev);
    return 0;
}
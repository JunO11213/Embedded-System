#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include "ch3.h"

int main(int argc, char *argv[]) {
    int dev;
    struct msg_st user_str;

    if (argc != 2) {
        printf("사용법: sudo ./ch3_writer \"보낼 메시지\"\n");
        return -1;
    }

    // 터미널에서 입력한 문자열을 구조체에 복사
    strncpy(user_str.str, argv[1], 127);
    user_str.str[127] = '\0'; // 안전을 위해 널 문자 추가
    user_str.len = strlen(user_str.str);

    // 디바이스 열고 ioctl로 전송 (Enqueue)
    dev = open("/dev/ch3_dev", O_RDWR);
    if (dev < 0) {
        printf("디바이스 파일을 열 수 없습니다.\n");
        return -1;
    }

    ioctl(dev, CH3_IOCTL_WRITE, &user_str);
    printf("Writer: 큐에 [%s] 메시지를 넣었습니다.\n", user_str.str);

    close(dev);
    return 0;
}
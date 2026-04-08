#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "simple_block_2.h"

void main(void){
    int fd, ret, cmd;
    printf("[%d] Choose cmd (1:WQ, 2:WQ_EX): ", getpid());
    scanf("%d", &cmd);

    fd = open("/dev/simple_block_2_dev", O_RDWR);
    if (fd < 0) { perror("open"); return; }

    switch(cmd) {
        case 1:
            ret = ioctl(fd, WQ, 0);
            break;
        case 2:
            ret = ioctl(fd, WQ_EX, 0);
            break;
        default:
            printf("Invalid cmd\n");
            close(fd);
            return;
    }
    printf("success to consume: %d\n", ret);
    close(fd);
}
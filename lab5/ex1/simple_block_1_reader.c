#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "simple_block_1.h"

void main(void){
    int fd, ret, cmd;
    printf("Choose cmd (1:WQ, 2:WQ_INT, 3:WQ_TIMEOUT): ");
    scanf("%d", &cmd);

    fd = open("/dev/simple_block_1_dev", O_RDWR); 
    if (fd < 0) { perror("open"); return; }

    switch (cmd) {
        case 1:
            ret = ioctl(fd, WQ, 0); 
            break;
        case 2:
            ret = ioctl(fd, WQ_INTERRUPTIBLE, 0); 
            break;
        case 3:
            ret = ioctl(fd, WQ_INTERRUPTIBLE_TIMEOUT, 0); 
            break;
        default:
            printf("Invalid cmd\n");
            close(fd);
            return;
    }
    printf("success to consume: %d\n", ret);
    close(fd); 
}
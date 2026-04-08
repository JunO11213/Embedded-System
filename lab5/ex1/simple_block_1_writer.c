#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "simple_block_1.h"

void main(void) {
    int fd, ret;
    long value;
    printf("input value to produce: ");
    scanf("%ld", &value);

    fd = open("/dev/simple_block_1_dev", O_RDWR); 
    if (fd < 0) { perror("open"); return; }

    ret = ioctl(fd, WQ_WAKE_UP, (unsigned long)value); 
    printf("success to produce: %d\n", ret); 
    close(fd); 
}
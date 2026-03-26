#include <stdio.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define CH2_IOCTL_NUM 'c'
#define CH2_GET _IOWR(CH2_IOCTL_NUM, 1, unsigned long)
#define CH2_SET _IOWR(CH2_IOCTL_NUM, 2, unsigned long)
#define CH2_ADD _IOWR(CH2_IOCTL_NUM, 3, unsigned long)
#define CH2_MUL _IOWR(CH2_IOCTL_NUM, 4, unsigned long)

void main(void) {
    int dev;
    long result = 200;

    dev = open("/dev/ch2_mod", O_RDWR);

    ioctl(dev, CH2_SET, 200);
    result = ioctl(dev, CH2_GET, NULL);
    printf("%ld\n", result); 


    ioctl(dev, CH2_ADD, 300);
    result = ioctl(dev, CH2_GET, NULL);
    printf("%ld\n", result); 

    ioctl(dev, CH2_MUL, 100);
    result = ioctl(dev, CH2_GET, NULL);
    printf("%ld\n", result);

    close(dev);
}
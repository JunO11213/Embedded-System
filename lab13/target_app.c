#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    int fd;
    char buf[128];
    ssize_t n;

    printf("target_app start\n");

    fd = open("./target_app.c", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    n = read(fd, buf, sizeof(buf) - 1);
    if (n < 0) {
        perror("read");
        close(fd);
        return 1;
    }

    buf[n] = '\0';

    printf("read success: %zd bytes\n", n);

    close(fd);

    printf("target_app end\n");

    return 0;
}
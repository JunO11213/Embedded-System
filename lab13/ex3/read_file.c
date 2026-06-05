#include <stdio.h>

#include <fcntl.h>

#include <unistd.h>

void main() {

    char filename[100] = "./test.txt";

    int pid = getpid();

    while (1) {

        int fd = open(filename, O_RDONLY);

        char buf[100] = {0};

        int ret = read(fd, buf, 8);

        printf("read %d bytes: %s from %s\n", ret, buf, filename);

        close(fd);

        sleep(1);

    }

}
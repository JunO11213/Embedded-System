#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void main() {
    printf("target_app start\n");
    
    // 3번 정도 반복하며 천천히 실행되도록 유도
    for(int i = 0; i < 3; i++) {
        int fd = open("target_app.c", O_RDONLY, 0400);
        printf("Iteration %d - fd: %d\n", i, fd);
        close(fd);
        
        // 1초 대기: 이 때 CPU가 다른 프로세스(kworker 등)로 넘어가면서 sched_switch 발생
        sleep(1); 
    }
    
    printf("target_app end\n");
}
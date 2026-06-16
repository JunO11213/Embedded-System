#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
#include <sched.h>

#ifndef __NR_sched_setattr
#ifdef __x86_64__
#define __NR_sched_setattr 314
#endif // __x86_64__

#ifdef __aarch64__
#define __NR_sched_setattr 274
#endif // __aarch64__
#endif // __NR_sched_setattr

#ifndef __NR_sched_getattr
#ifdef __x86_64__
#define __NR_sched_getattr 315
#endif // __x86_64__

#ifdef __aarch64__
#define __NR_sched_getattr 275
#endif // __aarch64__
#endif // __NR_sched_getattr

#ifndef SCHED_DEADLINE
#define SCHED_DEADLINE 6
#endif // SCHED_DEADLINE

#ifndef SCHED_EXT
#define SCHED_EXT 7
#endif // SCHED_EXT

typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed int s32;

struct sched_attr {
    u32 size;

    u32 sched_policy;
    u64 sched_flags;

    s32 sched_nice;

    u32 sched_priority;

    u64 sched_runtime;
    u64 sched_deadline;
    u64 sched_period;
};

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags){
    return syscall(__NR_sched_setattr, pid, attr, flags);
}

int sched_getattr(pid_t pid, struct sched_attr *attr, unsigned int size, unsigned int flags){
    return syscall(__NR_sched_getattr, pid, attr, size, flags);
}

/* Busy Loop for sec */
void delay(double sec){
    clock_t start = clock();
    while((double)(clock() - start) / CLOCKS_PER_SEC < sec);
}

int main(void) {
    pid_t pid;
    int scheduler;
    cpu_set_t mask;
    struct sched_attr param;
    int cpu = 0; /* CPU Number to Set */

    // 1. Set CPU affinity to the specified CPU
    /*CPU_ZERO(&mask);
    CPU_SET(cpu, &mask);
    pid = getpid();
    if(sched_setaffinity(pid, sizeof(mask), &mask)) {
        printf("failed to set affinity\n");
        exit(1);
    }
    */
    // 2. Get current scheduling attributes
    if (sched_getattr(0, &param, sizeof(struct sched_attr), 0)) {
        printf("failed to get attr \n");
        exit(1);
    }

    printf("Process %d \n", getpid());

    // 3. Set scheduling policy
    param.sched_policy = SCHED_DEADLINE;
    //param.sched_priority = sched_get_priority_min(SCHED_FIFO);
    param.sched_runtime = 2000000;
    param.sched_deadline = 10000000;
    param.sched_period = 10000000;
    if (sched_setattr(0, &param, 0) == -1) {
        printf("failed to set attr \n");
        exit(1);
    }

    scheduler = sched_getscheduler(0);
    switch (scheduler) {
        case SCHED_OTHER:
            printf("Default scheduler is beding used \n");
            break;
        case SCHED_FIFO:
            printf("FIFO scheduler is being used \n");
            break;
        case SCHED_RR:
            printf("Round robin scheduler is being used \n");
            break;
        case SCHED_DEADLINE:
            printf("Deadline scheduler is being used \n");
            break;
        case SCHED_EXT:
            printf("Extended scheduler is being used \n");
            break;
        default:
            printf("failed to get scheduler\n");
            exit(1);
    }

    // 4. Execute Example
    pid = fork();
    delay(0.1); //100ms

    return 0;
}
#include <uapi/linux/ptrace.h>

#include <linux/sched.h>

#include <linux/fs.h>

TRACEPOINT_PROBE(syscalls, sys_enter_openat) {

    char filename[256];

    // 두 문자열 길이는 동일해야 함

    char target[] = "./test.txt";

    char bypass[] = "./hack.txt";

    bpf_probe_read_user_str(filename, sizeof(filename), args->filename);

    int match = 1;

#pragma unroll

    for (int i = 0; i < 10; i++) {

        if (filename[i] != target[i]) {

            match = 0;

            break;

        }

    }

    if (match) {

        bpf_probe_write_user((void *)args->filename, bypass, sizeof(bypass));

        bpf_trace_printk("Redirected openat: %s -> %s\n", target, bypass);

    }

    return 0;

}
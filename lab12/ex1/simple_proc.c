#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

#define BUF_SIZE 128
static struct proc_dir_entry *parent_proc;

static int id = 202077777;
static char name[100] = "Embedded_System_Software";

static int proc_info_show(struct seq_file *seq, void *v) {
    seq_printf(seq, "%d\n", id);
    seq_printf(seq, "%s\n", name);
    return 0;
}

static int proc_info_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_info_show, NULL);
}

static ssize_t proc_info_write(struct file *file, const char __user *ubuf, size_t ubuf_len, loff_t *pos) {
    char buf[BUF_SIZE];

    if (ubuf_len > BUF_SIZE)
        return -1;

    if (copy_from_user(buf, ubuf, ubuf_len)) {
        return -1;
    }

    // 이름 초기화 후 입력값 파싱
    memset(name, 0, sizeof(name));
    sscanf(buf, "%d %s", &id, name);

    printk("simple_proc: id = %d, name = %s\n", id, name);

    return ubuf_len;
}

// 최신 커널용 proc_ops 구조체 정의
static const struct proc_ops proc_info_fops = {
    .proc_open    = proc_info_open,
    .proc_read    = seq_read,
    .proc_write   = proc_info_write,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int __init simple_proc_init(void) {
    parent_proc = proc_mkdir("simple_proc_dir", NULL);
    if (!parent_proc) return -ENOMEM;
    
    proc_create("proc_info", 0666, parent_proc, &proc_info_fops);
    return 0;
}

static void __exit simple_proc_exit(void) {
    proc_remove(parent_proc);
}

module_init(simple_proc_init);
module_exit(simple_proc_exit);
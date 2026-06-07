#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");

#define BUF_SIZE 50
static struct proc_dir_entry *parent_proc;

static char read_str[BUF_SIZE + 1];

static int proc_info_show(struct seq_file *seq, void *v) {
    int i;
    int count=0;

    for (i=0; read_str[i] != '\0'; i++){
        if(read_str[i]==' '){
            count++;
        }
    }
    seq_printf(seq,"str : %s\n", read_str);
    seq_printf(seq,"count: %d\n", count);

    return 0;
}

static int proc_info_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_info_show, NULL);
}

static ssize_t proc_info_write(struct file *file, const char __user *ubuf, size_t ubuf_len, loff_t *pos) {
    size_t len = (ubuf_len > BUF_SIZE) ? BUF_SIZE : ubuf_len;

    if (copy_from_user(read_str, ubuf, len)) {
        return -EFAULT;
    }

    read_str[len] = '\0'; 

    
    if (len > 0 && read_str[len-1] == '\n') {
        read_str[len-1] = '\0';
    }

    return ubuf_len;
}

static const struct proc_ops proc_info_fops = {
    .proc_open    = proc_info_open,
    .proc_read    = seq_read,
    .proc_write   = proc_info_write,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int __init simple_proc_init(void) {
    parent_proc = proc_mkdir("ch11_proc_dir", NULL);
    if (!parent_proc) return -ENOMEM;
    
    proc_create("proc_info", 0666, parent_proc, &proc_info_fops);
    return 0;
}

static void __exit simple_proc_exit(void) {
    proc_remove(parent_proc);
}

module_init(simple_proc_init);
module_exit(simple_proc_exit);
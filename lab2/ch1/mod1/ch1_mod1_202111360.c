#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static void __init ch1_mod1_init(void){}
static void __exit ch1_mod1_exit(void){}

module_init(ch1_mod1_init);
module_exit(ch1_mod1_exit);

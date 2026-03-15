#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

int my_id = 123456789;
EXPORT_SYMBOL(my_id);

static int __init mod1_init(void){
	printk(KERN_NOTICE"Hello, Mod1\n");
	return 0;
}

static void __exit mod1_exit(void){
	printk(KERN_NOTICE "Goodbye, Mod1\n");
}

module_init(mod1_init);
module_exit(mod1_exit);

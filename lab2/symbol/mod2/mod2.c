#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

extern int my_id;

static int __init mod2_init(void){
	printk(KERN_NOTICE "Hello, Mod2\n");
	printk(KERN_NOTICE "My ID : %d\n" ,  my_id);
	return 0;
}
static void __exit mod2_exit(void){
	printk(KERN_NOTICE "Goodbye, Mod2\n");
}

module_init(mod2_init);
module_exit(mod2_exit);

#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static int my_id = 123456789;

int get_my_id(void){
    return my_id;
}
EXPORT_SYMBOL(get_my_id);

int set_my_id(int id){
    my_id = id;
    return 1;
}
EXPORT_SYMBOL(set_my_id);

static int __init ch1_mod1_init(void){
    return 0;
}
static void __exit ch1_mod1_exit(void){}

module_init(ch1_mod1_init);
module_exit(ch1_mod1_exit);

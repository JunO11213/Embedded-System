#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple linked list example");

struct test_list {
    struct list_head list;
    int id;
};

struct test_list mylist;

static int __init simple_linked_list_init(void){
    struct test_list *tmp = 0;
    struct list_head *pos = 0;
    unsigned int i = 0;

    printk("simple_linked_list: Init module\n");

    // [1] Initialize the list head
    INIT_LIST_HEAD(&mylist.list);

    // [2] Add 5 nodes to the list
    for(i = 0; i < 5; i++){
        // Allocate memory for a new node and set its id
        tmp = (struct test_list *)kmalloc(sizeof(struct test_list), GFP_KERNEL);
        tmp->id = i + 100;
        printk("simple_linked_list: add the node (id: %d)\n", tmp->id);
        // Add the new node at the front of the list
        list_add(&tmp->list, &mylist.list);
    }

    // [3] Traverse the list and print the id of each node
    printk("simple_linked_list: use list_for_each & list_entry\n");
    i = 0;
    list_for_each(pos, &mylist.list){
        tmp = list_entry(pos, struct test_list, list);
        printk("simple_linked_list: pos[%d], id: %d\n", i, tmp->id);
        i++;
    }

    return 0;
}

static void __exit simple_linked_list_exit(void){
    struct test_list *tmp = 0;
    struct list_head *pos = 0;
    struct list_head *q = 0;
    unsigned int i = 0;

    // [4] Traverse the list and free the memory allocated for each node
    printk("simple_linked_list: Exit module\n");
    i = 0;
    list_for_each_safe(pos, q, &mylist.list){
        tmp = list_entry(pos, struct test_list, list);
        printk("simple_linked_list: free pos[%d], id: %d\n", i, tmp->id);
        list_del(pos); // Remove the node from the list
        kfree(tmp); // Free the memory allocated for the node
        i++;
    }
}

module_init(simple_linked_list_init);
module_exit(simple_linked_list_exit);
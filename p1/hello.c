#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

int hello(void)
{
        printk(KERN_ALERT "Hello world!\n");
        return 0;
}


void goodbye(void)
{
        printk(KERN_ALERT "Goodbye world!\n");
}


module_init(hello);
module_exit(goodbye);

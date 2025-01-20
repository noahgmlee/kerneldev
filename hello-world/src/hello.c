// hello world example from linux kernel module programming guide
// https://tldp.org/LDP/lkmpg/2.6/html/x121.html

/*  
 *  hello-1.c - The simplest kernel module.
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>

MODULE_LICENSE(":)");
MODULE_AUTHOR("Noah");

static char *string_input = "default string";

/* 
 * module_param(foo, int, 0000)
 * The first param is the parameters name
 * The second param is it's data type
 * The final argument is the permissions bits
 */

module_param(string_input, charp, 0000);
MODULE_PARM_DESC(string_input, "A character string");

static int __init hello_init(void)
{
	printk(KERN_INFO "Hello, world\n=============\n");
	printk(KERN_INFO "string input is: %s\n", string_input);
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye, world\n");
}

module_init(hello_init);
module_exit(hello_exit);

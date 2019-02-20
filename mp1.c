#define LINUX

#include <linux/module.h>
#include <linux/kernel.h>
#include "mp1_given.h"

// #include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
// #include <linux/spinlock.h>
// #include <linux/list.h>
#include <linux/slab.h>
// #include <linux/timer.h>
// #include <linux/workqueue.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group_ID");
MODULE_DESCRIPTION("CS-423 MP1");

#define FILENAME "status"
#define DIRECTORY "mp1"
#define DEBUG 1

static const struct file_operations mp1_file = {
.owner = THIS_MODULE,
.read = mp1_read,
.write = mp1_write,
};

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_entry;

static ssize_t mp1_read (struct file *file, char __user *buffer, size_t count, loff_t *data){
   // implementation goes here...
   int copied;
   char * buf;
   buf = (char *) kmalloc(count,GFP_KERNEL);
   copied = 0;
   //… put something into the buf, updated copied
   copy_to_user(buffer, buf, copied);
   kfree(buf);
   return copied ;
}

static ssize_t mp1_write (struct file *file, const char __user *buffer, size_t count, loff_t *data){
   // implementation goes here...
   return 0;
}

// mp1_init - Called when module is loaded
int __init mp1_init(void)
{
   #ifdef DEBUG
   printk(KERN_ALERT "MP1 MODULE LOADING\n");
   #endif
   // Insert your code here ...   
   proc_dir = proc_mkdir(DIRECTORY, NULL);
   proc_entry = proc_create(FILENAME, 0666, proc_dir, & mp1_file);
   printk(KERN_ALERT "MP1 MODULE LOADED\n");
   return 0;   
}

// mp1_exit - Called when module is unloaded
void __exit mp1_exit(void)
{
   #ifdef DEBUG
   printk(KERN_ALERT "MP1 MODULE UNLOADING\n");
   #endif
   remove_proc_entry(FILENAME, proc_dir);
   remove_proc_entry(DIRECTORY, NULL);  
   // Insert your code here ...
   
   

   printk(KERN_ALERT "MP1 MODULE UNLOADED\n");
}

// Register init and exit funtions
module_init(mp1_init);
module_exit(mp1_exit);

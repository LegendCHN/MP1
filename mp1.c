#define LINUX

#include <linux/module.h>
#include <linux/kernel.h>
#include "mp1_given.h"

// #include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h> 
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
#define MASK 0666

static const struct file_operations mp1_file = {
.owner = THIS_MODULE,
.read = mp1_read,
.write = mp1_write,
};

static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_entry;
struct linkedlist reglist;
struct linkedlist *tmp;
struct list_head *pos, *q;
struct mutex lock;

void free_linkedlist(void){
   list_for_each_safe(pos, q, &reglist.list){
       tmp= list_entry(pos, struct linkedlist, list);
       list_del(pos);
       kfree(tmp);
   }
}

static ssize_t mp1_read (struct file *file, char __user *buffer, size_t count, loff_t *data){
   int copied;
   copied = 0;
   int length;
   char * buf;
   buf = (char *) kmalloc(count, GFP_KERNEL);

   if(*data > 0){
      kfree(buf);
      return 0;
   }

   list_for_each_entry(tmp, &reglist.list, list){
      length = sprintf(buf + copied, "PID: %d, TIME: %lu\n", tmp->pid, tmp->time);
      copied += length;
   }
   copy_to_user(buffer, buf, copied);
   kfree(buf);

   *data += copied;
   return copied ;
}

static ssize_t mp1_write (struct file *file, const char __user *buffer, size_t count, loff_t *data){
   tmp = (struct linkedlist*)kmalloc(sizeof(struct linkedlist), GFP_KERNEL);
   char buf[100];
   copy_from_user(buf, buffer, count);
   sscanf(buf, "%d", &tmp->pid);
   mutex_lock(&lock);
   list_add(&(tmp->list), &(reglist.list));
   mutex_unlock(&lock);
   return count;
}

// mp1_init - Called when module is loaded
int __init mp1_init(void)
{
   #ifdef DEBUG
   printk(KERN_ALERT "MP1 MODULE LOADING\n");
   #endif
   // Insert your code here ...   
   proc_dir = proc_mkdir(DIRECTORY, NULL);
   proc_entry = proc_create(FILENAME, MASK, proc_dir, & mp1_file);
   INIT_LIST_HEAD(&reglist.list);
   mutex_init(&lock);

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
   free_linkedlist();
   printk(KERN_ALERT "MP1 MODULE UNLOADED\n");
}

// Register init and exit funtions
module_init(mp1_init);
module_exit(mp1_exit);

#define LINUX

#include <linux/module.h>
#include <linux/kernel.h>
#include "mp1_given.h"

#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h> 
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group_ID");
MODULE_DESCRIPTION("CS-423 MP1");

#define FILENAME "status"
#define DIRECTORY "mp1"
#define DEBUG 1
#define MASK 0666
#define TIMELIMIT 5000

// file operations for file status
static const struct file_operations mp1_file = {
   .owner = THIS_MODULE,
   .read = mp1_read,
   .write = mp1_write,
};
// variables declaration
static struct proc_dir_entry *proc_dir;
static struct proc_dir_entry *proc_entry;
static struct linkedlist reglist;
static struct linkedlist *tmp;
static struct list_head *pos, *q;
static struct mutex lock;
static struct timer_list timer;
static struct workqueue_struct *workqueue;
static struct work_struct *worker;

// helper function to free linkedlist space
void free_linkedlist(void){
   list_for_each_safe(pos, q, &reglist.list){
       tmp= list_entry(pos, struct linkedlist, list);
       list_del(pos);
       kfree(tmp);
   }
}

// callback function to create and push worker to workqueue
void update_time(unsigned long data){
   worker = (struct work_struct*)kmalloc(sizeof(struct work_struct), GFP_KERNEL);
   INIT_WORK(worker, workfunc);
   queue_work(workqueue, worker);
   mod_timer(&timer, jiffies + msecs_to_jiffies(TIMELIMIT));
}

// workfunction to inc cpu time or remove if process finishes
void workfunc(struct work_struct *worker){
   int ret;
   mutex_lock(&lock);
   list_for_each_safe(pos, q, &reglist.list) {
      tmp = list_entry(pos, struct linkedlist, list);
      ret = get_cpu_use(tmp->pid, &tmp->time);
      // change units from jiffies to msecs
      if (ret != -1){
         tmp->time = jiffies_to_msecs(cputime_to_jiffies(tmp->time));
      }
      else{
         // remove finished process
         list_del(pos);
         kfree(tmp);
      }
   }
   mutex_unlock(&lock);
   kfree(worker);
}

// read function to show pid and cpu time
static ssize_t mp1_read (struct file *file, char __user *buffer, size_t count, loff_t *data){
   int copied;
   int length;
   char * buf;
   buf = (char *) kmalloc(count, GFP_KERNEL);
   copied = 0;
   // destroy to prevent furthur read
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

// write function to add pid list entry to linkedlist
static ssize_t mp1_write (struct file *file, const char __user *buffer, size_t count, loff_t *data){
   char buf[100];
   tmp = (struct linkedlist*)kmalloc(sizeof(struct linkedlist), GFP_KERNEL);
   copy_from_user(buf, buffer, count);
   // read in cpu number and initialize time to 0
   sscanf(buf, "%d", &tmp->pid);
   tmp->time = 0;
   // critical section to write
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
   // create dir and file under /proc
   proc_dir = proc_mkdir(DIRECTORY, NULL);
   proc_entry = proc_create(FILENAME, MASK, proc_dir, & mp1_file);

   // initialize linkedlist and mutex
   INIT_LIST_HEAD(&reglist.list);
   mutex_init(&lock);

   // initialize timer
   setup_timer(&timer, update_time, 0);
   mod_timer(&timer, jiffies + msecs_to_jiffies(TIMELIMIT));
   // initialize workqueue
   workqueue = create_workqueue("workqueue");

   printk(KERN_ALERT "MP1 MODULE LOADED\n");
   return 0;   
}

// mp1_exit - Called when module is unloaded
void __exit mp1_exit(void)
{
   #ifdef DEBUG
   printk(KERN_ALERT "MP1 MODULE UNLOADING\n");
   #endif
   // destroy workqueue
   flush_workqueue(workqueue);
   destroy_workqueue(workqueue);
   // destroy timer
   del_timer(&timer);
   // destroy linked list
   free_linkedlist();
   // destroy dir and file
   remove_proc_entry(FILENAME, proc_dir);
   remove_proc_entry(DIRECTORY, NULL);
   printk(KERN_ALERT "MP1 MODULE UNLOADED\n");
}

// Register init and exit funtions
module_init(mp1_init);
module_exit(mp1_exit);

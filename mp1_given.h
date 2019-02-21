#ifndef __MP1_GIVEN_INCLUDE__
#define __MP1_GIVEN_INCLUDE__

#include <linux/pid.h>
#include <linux/sched.h>

#define find_task_by_pid(nr) pid_task(find_vpid(nr), PIDTYPE_PID)

struct linkedlist{
  struct list_head list;
  int pid;
  unsigned long time;
};

//THIS FUNCTION RETURNS 0 IF THE PID IS VALID AND THE CPU TIME IS SUCCESFULLY RETURNED BY THE PARAMETER CPU_USE. OTHERWISE IT RETURNS -1
int get_cpu_use(int pid, unsigned long *cpu_use){
  struct task_struct* task;
  rcu_read_lock();
  task=find_task_by_pid(pid);
  if (task!=NULL) {  
    *cpu_use=task->utime;
    rcu_read_unlock();
    return 0;
  }
  else {
    rcu_read_unlock();
    return -1;
  }
}

// read function to show pid and cpu time
static ssize_t mp1_read (struct file *file, char __user *buffer, size_t count, loff_t *data);
// write function to add pid list entry to linkedlist
static ssize_t mp1_write (struct file *file, const char __user *buffer, size_t count, loff_t *data);
// helper function to free linkedlist space
void free_linkedlist(void);
// callback function to create and push worker to workqueue
void update_time(unsigned long data);
// workfunction to inc cpu time or remove if process finishes
void workfunc(struct work_struct *worker);
// mp1_init - Called when module is loaded
int __init mp1_init(void);
// mp1_exit - Called when module is unloaded
void __exit mp1_exit(void);
#endif

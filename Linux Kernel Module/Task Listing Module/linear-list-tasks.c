#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>

/* This function is called when the module is loaded. */
int task_list_init(void)
{
  printk(KERN_INFO "List Tasks Module Init\n");

  printk(KERN_INFO "========Current Processes======\n");
  struct task_struct *task;
  for_each_process(task) {
    /* on each iteration task points to the next task */
    printk(KERN_INFO "\tpdf:%d name:%s state:%ld\n",task->pid,task->comm,task->state);
  }
  printk(KERN_INFO "================End===========\n");

  return 0;
}

/* This function is called when the module is removed. */ 
void task_list_exit(void)
{
  printk(KERN_INFO "Removing Task List Module\n"); 
}

/* Macros for registering module entry and exit points. */
module_init(task_list_init);
module_exit(task_list_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Task List Module"); 
MODULE_AUTHOR("YADI");

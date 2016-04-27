#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>

void dfs(struct task_struct* task){
  struct task_struct *task_next;
  struct list_head *list;

  list_for_each(list, &task->children) {
    task_next = list_entry(list, struct task_struct, sibling);
    
    printk(KERN_INFO "\tpid:%d pname:%s state:%ld\n", task_next->pid, task_next->comm, task_next->state);

    dfs(task_next);
  }  
}

/* This function is called when the module is loaded. */
int task_list_init(void)
{
  printk(KERN_INFO "DFS List Tasks Module Init\n");

  printk(KERN_INFO "========Current Processes======\n");
  dfs(&init_task);

  printk(KERN_INFO "================End===========\n");

  return 0;
}

/* This function is called when the module is removed. */ 
void task_list_exit(void)
{
  printk(KERN_INFO "Removing DFS Task List Module\n"); 
}

/* Macros for registering module entry and exit points. */
module_init(task_list_init);
module_exit(task_list_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DFS Task List Module"); 
MODULE_AUTHOR("YADI");

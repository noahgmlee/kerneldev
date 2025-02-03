#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/ktime.h>

// contains task_struct and should include current
#include <linux/sched.h>
// access cred struct in task_struct
#include <linux/cred.h>
// for k_uidt variable type
#include <linux/uidgid.h>
// for init_task
#include <linux/sched/task.h>
// for for_each_process macro utility
#include <linux/signal.h>

static int pcb_traverse_and_display(struct seq_file *m) {
  // Get the front of the process control block
  struct task_struct *task_ptr = NULL; // for_each_process does ptr assignment
  for_each_process(task_ptr) {
    // log info
    seq_printf(m, "Process: %s [PID = %d]\n", task_ptr->comm, task_ptr->pid);
  }

  return 0;
}

static int current_proc_show(struct seq_file *m, void *v) {
  struct task_struct *cur_task = current;

  seq_printf(m, "Current Process PCB Information\n");

  // process name
  seq_printf(m, "Name = %s\n", cur_task->comm);

  // pid
  seq_printf(m, "PID = %d\n", (int)cur_task->pid);

  // ppid
  seq_printf(m, "PPID = %d\n",(int) task_ppid_nr(cur_task));

  // state
  /*
  volatile long state (volatile meaning the variable var can chance any time)
  can compare to the defined variables in sched.h
  read process states on http://books.gigatux.nl/mirror/kerneldevelopment/0672327201/ch03lev1sec1.html
  for what each defined value means
  */
  
  char *state_str = "Unknown";

  // task is Ready or Running if state is TASK_RUNNING
  if ( cur_task->__state == TASK_RUNNING){
    state_str = "Running";

  }
  // waiting for interruptible and uninterruptible states
  else if ( (cur_task->__state == TASK_INTERRUPTIBLE) || (cur_task->__state == TASK_UNINTERRUPTIBLE) ) state_str = "Waiting";
  // stopped for the other states
  else if ( cur_task->__state == TASK_STOPPED ) state_str = "Stopped";
  
  seq_printf(m, "State = %s\n", state_str);

  // uids
  // method that works for any process, by accessing their task_struct
  // will read from cred and real_cred (<linux/cred.h>) structs in task_struct
  // returns it in kuid struct which is defined in uidgid.h, read val for int of value
  seq_printf(m, "Real UID = %d\n", cur_task->cred->uid.val);
  seq_printf(m, "Effective UID = %d\n", cur_task->cred->euid.val);
  seq_printf(m, "Saved UID = %d\n", cur_task->cred->suid.val);

  // gids
  seq_printf(m, "Real GID = %d\n", cur_task->cred->gid.val);
  seq_printf(m, "Effective GID = %d\n", cur_task->cred->egid.val);
  seq_printf(m, "Saved GID = %d\n", cur_task->cred->sgid.val);

  seq_printf(m, "\n\nTRAVERSING PCB AND LOGGING\n\n");

  return pcb_traverse_and_display(m);
}

static int file_open_callback(struct inode *inode, struct  file *file) {
  return single_open(file, current_proc_show, NULL);
}


static const struct proc_ops g_fops = {
  .proc_open = file_open_callback,
  .proc_read = seq_read,
  .proc_lseek = seq_lseek,
  .proc_release = single_release,
};

static int __init pcb_init(void) {
  proc_create("pcb-info", 0, NULL, &g_fops);
  printk(KERN_INFO "pcb-info in\n");
  return 0;
}

static void __exit pcb_exit(void) {
  remove_proc_entry("pcb-info", NULL);
  printk(KERN_INFO "pcb-info out\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Noah");
module_init(pcb_init);
module_exit(pcb_exit);

#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/pid.h>

SYSCALL_DEFINE2(memuse_syscall, int, p_id, int, mem_type)
{
        struct pid *s_pid;
        struct task_struct *s_task;
        struct mm_struct *s_mm;
        struct vm_area_struct *s_vma;

        printk(KERN_ALERT "memuse_syscall: p_id %d, mem_type %d \n", p_id, mem_type);

        // p_id should be greater than 0
        if (p_id <= 0)
        {
                printk(KERN_ALERT "memuse_syscall: invalid p_id \n");
                return -1;
        }
        // mem_type should be between 1 to 3
        if (mem_type < 1 || mem_type > 3)
        {
                printk(KERN_ALERT "memuse_syscall: invalid mem_type \n");
                return -1;
        }
        s_pid = find_get_pid(p_id);
        if (NULL == s_pid)
        {
                printk(KERN_ALERT "memuse_syscall: no process with p_id %d \n", p_id);
                return -1;
        }
        s_task = get_pid_task(s_pid, PIDTYPE_PID);
        if (NULL == s_task)
        {
                printk(KERN_ALERT "memuse_syscall: task_struct is NULL with p_id %d \n", p_id);
                return -1;
        }
        // all kernel threads share the same memory space, so it does not have its own mm_struct
        s_mm = s_task->mm;
        if (NULL == s_mm)
        {
                printk(KERN_ALERT "memuse_syscall: mm is NULL, this is a kernel thread with p_id %d \n", p_id);
                return -1;
        }
        // start_stack is the start of stack memory
        if (mem_type == 1) {
                // mm_struct does not provide the end of stack
                // so we go through mmap to find the area where stack is and get the end of area
                for (s_vma = s_mm->mmap; s_vma; s_vma = s_vma->vm_next)
                {
                        if (s_vma->vm_start <= s_mm->start_stack && s_mm->start_stack <= s_vma->vm_end) {
                                return s_vma->vm_end - s_mm->start_stack;
                        }
                }
                printk(KERN_ALERT "memsue_syscall: unable to find start_stack in mmap\n");
                return -1;
        }
        // size of kernel stack is fixed, in x86_64 it is 8kb (2 pages); each thread has its own kernel stack
        if (mem_type == 2) {
                return (long) atomic_read(&s_mm->mm_users) * 8192;
        }
        // start_brk is the start of heap memory, brk is the end of heap memory
        return s_mm->brk - s_mm->start_brk;
}

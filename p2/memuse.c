#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/pid.h>

SYSCALL_DEFINE2(memuse_syscall, int, p_id, int, mem_type)
{
        pid_t pid;
        printk(KERN_ALERT "memuse_syscall: p_id %d, mem_type %d \n", p_id, mem_type);

        // p_id should be greater than 0
        if (p_id <= 0)
        {
                return -1;
        }
        // mem_type should be between 1 to 3
        if (mem_type < 1 || mem_type > 3)
        {
                return -1;
        }
        pid = p_id;
        // return type is struct pid *, return NULL if process does not exist
        if (NULL == find_get_pid(pid))
        {
                return -1;
        }
        return 1;
}

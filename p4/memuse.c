#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/pid.h>

#define KERNEL_STACK_PAGES 4

// this function calculate the physical memory size with virtual memory begin and end as input
long cal_physical_memory(struct mm_struct *s_mm, unsigned long start, unsigned long end)
{
        unsigned long p_page;
        long result = 0;
        pgd_t *pgd;
        p4d_t *p4d;
        pud_t *pud;
        pmd_t *pmd;
        pte_t *pte;

        // looks down the page table level by level
        // if the page has an valid entry in page table, the page in currently in physical memory
        for (p_page = start; p_page < end; p_page += PAGE_SIZE)
        {
                pgd = pgd_offset(s_mm, p_page);
                if (pgd_none(*pgd) || pgd_bad(*pgd))
                        continue;
                p4d = p4d_offset(pgd, p_page);
                if (p4d_none(*p4d) || p4d_bad(*p4d))
                        continue;
                pud = pud_offset(p4d, p_page);
                if (pud_none(*pud) || pud_bad(*pud))
                        continue;
                pmd = pmd_offset(pud, p_page);
                if (pmd_none(*pmd) || pmd_bad(*pmd))
                        continue;
                if (!(pte = pte_offset_map(pmd, p_page)))
                        continue;
                if (NULL == pte)
                        continue;                
                if (pte_present(*pte))
                        result += PAGE_SIZE;
        }

        return result;
}

SYSCALL_DEFINE2(memuse_syscall, int, p_id, int, mem_type)
{
        struct pid *s_pid;
        struct task_struct *s_task;
        struct mm_struct *s_mm;
        struct vm_area_struct *s_vma;
        unsigned long kernel_stack_start;

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
        if (mem_type == 1) {
                // mm_struct provides the start of stack
                // so we go through mmap to find the area where stack is and get the size of this area
                for (s_vma = s_mm->mmap; s_vma; s_vma = s_vma->vm_next)
                {
                        if (s_vma->vm_start <= s_mm->start_stack && s_mm->start_stack <= s_vma->vm_end) {
                                return cal_physical_memory(s_mm, s_vma->vm_start, s_vma->vm_end);
                        }
                }
                printk(KERN_ALERT "memsue_syscall: unable to find start_stack in mmap\n");
                return -1;
        }
         // size of kernel stack is fixed, in x86_64 it is 8kb (2 pages)
        if (mem_type == 2) {
                // task_struct provides the start of kernel stack
                kernel_stack_start = (unsigned long)(s_task->stack);
                return cal_physical_memory(s_mm, kernel_stack_start, kernel_stack_start + PAGE_SIZE * KERNEL_STACK_PAGES);
        }
        // start_brk is the start of heap memory, brk is the end of heap memory
        else {
                return cal_physical_memory(s_mm, s_mm->start_brk, s_mm->brk);
        }

}

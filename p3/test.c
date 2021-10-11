#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>

#define __NR_memuse 440

long memuse_syscall(int p_id, int mem_type)
{
        long result;
        asm volatile // volatile prevents gcc optimizer modify or remove the code
        (
                "syscall" // template
                : "=a"(result) // = means write-only, a means %rax
                : "0"(__NR_memuse), "D"(p_id), "S"(mem_type) // 0 means the same register of first operand, D means %rdi, S means %rsi
                : "rcx", "r11", "memory" // cloberred list, registers used by system call
        );
        return result;
}

void try(int p_id, int mem_type)
{
        long result;
        result = memuse_syscall(p_id, mem_type);
        printf("memuse_syscall result is %ld with p_id %d and mem_type %d\n", result, p_id, mem_type);
}

int main(int argc, char *argv[])
{
        try(1, 1);
        try(0, 1);
        try(-1, 1);

        try(18, 2);
        try(36, 3);
        try(54, 4);
        try(1, 0);

        try(923747, 1);

        return 0;
}

#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>

#define __NR_memuse 440

long memuse_syscall(int p_id, int mem_type)
{
        return syscall(__NR_memuse, p_id, mem_type);
}

void try(int p_id, int mem_type)
{
        long result;
        result = memuse_syscall(p_id, mem_type);
        printf("memuse_syscall result is %ld with p_id %d and mem_type %d\n", result, p_id, mem_type);
}

int main(int argc, char *argv[])
{
        try(1, 1); // Expect 1
        try(0, 1); // Expect -1 by p_id check
        try(-1, 1); // Expect -1 by p_id check

        try(18, 2); // Expect 1
        try(36, 3); // Expect 1
        try(54, 4); // Expect -1 by mem_type check
        try(1, 0); // Expect -1 by mem_type check
        try(923747, 1); // Expect -1 by process not exist

        return 0;
}

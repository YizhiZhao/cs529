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

        try(1, 1);
        try(1, 2);
        try(1, 3);

        try(2072, 1);
        try(2072, 2);
        try(2072, 3);


        try(-1, 1);
        try(3232323, 3);
        try(1, 0);
        try(1, 4);

        return 0;
}

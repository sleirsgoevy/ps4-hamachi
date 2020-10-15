#include <stdlib.h>
#include <unistd.h>

static inline unsigned long long get_xfast_syscall()
{
    unsigned int a, c = 0xc0000082, d;
    asm volatile("rdmsr":"=a"(a),"=d"(d):"c"(c));
    return ((unsigned long long)d) << 32 | a;
}

static inline unsigned long long get_rootvnode()
{
    return *(unsigned long long*)(get_xfast_syscall() - 0x1c0 + 0x2300320);
}

static inline void k_unchroot(unsigned long long*** td)
{
    td[1][9][2] = td[1][9][3] = td[1][9][4] = get_rootvnode();
}

asm("kexec:\nmov $11, %rax\nmov %rcx, %r10\nsyscall\nret");
void kexec(void*);

int unchroot()
{
    kexec(k_unchroot);
    return 0;
}

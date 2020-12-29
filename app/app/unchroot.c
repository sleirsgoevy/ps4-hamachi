#include <stdlib.h>
#include <unistd.h>
#include <orbis/libkernel.h>

static inline unsigned long long get_xfast_syscall()
{
    unsigned int a, c = 0xc0000082, d;
    asm volatile("rdmsr":"=a"(a),"=d"(d):"c"(c));
    return ((unsigned long long)d) << 32 | a;
}

static inline unsigned long long get_rootvnode(int ver)
{
    int vnode_offset;
    if(ver == 0x672)
        vnode_offset = 0x2300320;
    else if(ver == 0x702)
        vnode_offset = 0x22c5750;
    else
        vnode_offset = 0xdeadbeef;
    return *(unsigned long long*)(get_xfast_syscall() - 0x1c0 + vnode_offset);
}

struct uap
{
    void* f;
    int arg;
};

static inline void k_unchroot(unsigned long long*** td, struct uap* uap)
{
    td[1][9][2] = td[1][9][3] = td[1][9][4] = get_rootvnode(uap->arg);
}

asm("kexec:\nmov $11, %rax\nmov %rcx, %r10\nsyscall\nret");
void kexec(void*, int);

int unchroot()
{
    OrbisKernelSwVersion sw_ver;
    sceKernelGetSystemSwVersion(&sw_ver);
    kexec(k_unchroot, sw_ver.i_version >> 16);
    return 0;
}

asm("kexec:\nmov $11, %rax\nmov %rcx, %r10\nsyscall\nret");
void kexec(void* fn);

asm("blob:\n.incbin \"blob-ps4.bin\"\nblob_end:");

extern char blob[];
extern char blob_end[];

static unsigned long long get_syscall()
{
    unsigned int eax, ecx = 0xc0000082, edx;
    asm volatile("rdmsr":"=a"(eax),"=d"(edx):"c"(ecx));
    return ((unsigned long long)edx)<<32|eax;
}

static void load_start_module(void* td)
{
    // 6.72 offsets
    unsigned long long kernel_base = get_syscall() - 0x1c0;
    unsigned long long kernel_map = *(unsigned long long*)(kernel_base + 0x220dfc0);
    void*(*kmem_alloc)(unsigned long long, unsigned long long) = (void*)(kernel_base + 0x250730);
    int(*copyin)(const void*, void*, unsigned long long) = (void*)(kernel_base + 0x3c17a0);
    char* buf = kmem_alloc(kernel_map, blob_end - blob);
    copyin(blob, buf, blob_end - blob);
    ((void(*)(void*))buf)(td);
}

void load_tun_tsr()
{
    kexec(load_start_module);
}

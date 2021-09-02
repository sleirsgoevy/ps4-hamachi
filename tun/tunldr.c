asm("kexec:\nmov $11, %rax\nmov %rcx, %r10\nsyscall\nret");
void kexec(void* fn, int ver);

asm("blob_505:\n.incbin \"blob-ps4-505.bin\"\nblob_505_end:");
asm("blob_672:\n.incbin \"blob-ps4-672.bin\"\nblob_672_end:");
asm("blob_702:\n.incbin \"blob-ps4-702.bin\"\nblob_702_end:");
asm("blob_755:\n.incbin \"blob-ps4-755.bin\"\nblob_755_end:");

extern char blob_505[];
extern char blob_505_end[];
extern char blob_672[];
extern char blob_672_end[];
extern char blob_702[];
extern char blob_702_end[];
extern char blob_755[];
extern char blob_755_end[];

static unsigned long long get_syscall()
{
    unsigned int eax, ecx = 0xc0000082, edx;
    asm volatile("rdmsr":"=a"(eax),"=d"(edx):"c"(ecx));
    return ((unsigned long long)edx)<<32|eax;
}

struct uap
{
    void* fn;
    int arg;
};

static void load_start_module(void* td, struct uap* uap)
{
    unsigned long long kernel_base = get_syscall() - 0x1c0;
    unsigned long long kernel_map;
    void*(*kmem_alloc)(unsigned long long, unsigned long long);
    int(*copyin)(const void*, void*, unsigned long long);
    char* blob;
    char* blob_end;
    if(uap->arg == 0x505)
    {
        // 5.05 offsets
        kernel_map = *(unsigned long long*)(kernel_map + 0x1ac60e0);
        kmem_alloc = (void*)(kernel_base + 0xfcc80);
        copyin = (void*)(kernel_base + 0x1ea710);
        blob = blob_505;
        blob_end = blob_505_end;
    }
    else if(uap->arg == 0x672)
    {
        // 6.72 offsets
        kernel_map = *(unsigned long long*)(kernel_base + 0x220dfc0);
        kmem_alloc = (void*)(kernel_base + 0x250730);
        copyin = (void*)(kernel_base + 0x3c17a0);
        blob = blob_672;
        blob_end = blob_672_end;
    }
    else if(uap->arg == 0x702)
    {
        // 7.02 offsets
        kernel_map = *(unsigned long long*)(kernel_base + 0x21c8ee0);
        kmem_alloc = (void*)(kernel_base + 0x1170f0);
        copyin = (void*)(kernel_base + 0x2f230);
        blob = blob_702;
        blob_end = blob_702_end;
    }
    else if(uap->arg >= 0x750 && uap->arg <= 0x755)
    {
        // 7.5x offsets
        kernel_map = *(unsigned long long*)(kernel_base + 0x21405b8);
        kmem_alloc = (void*)(kernel_base + 0x1753e0);
        copyin = (void*)(kernel_base + 0x28f9f0);
        blob = blob_755;
        blob_end = blob_755_end;
    }
    else
        return;
    char* buf = kmem_alloc(kernel_map, blob_end - blob);
    copyin(blob, buf, blob_end - blob);
    ((void(*)(void*))buf)(td);
}

void load_tun_tsr(int ver)
{
    kexec(load_start_module, ver);
}

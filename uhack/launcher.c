#include <sys/syscall.h>
#include <unistd.h>
#include <sys/mman.h>

asm("blob:\n.incbin \"blob.bin\"\nblob_end:\n");

extern char blob[];
extern char blob_end[];

void load_start_module(void* td)
{
    unsigned long long kernel_map = *(unsigned long long*)0xffffffff8115af28;
    void*(*kmem_alloc)(unsigned long long, unsigned long long) = (void*)0xffffffff80a85990;
    int(*copyin)(const void*, void*, size_t) = (void*)0xffffffff80b16200;
    char* buf = kmem_alloc(kernel_map, blob_end - blob);
    copyin(blob, buf, blob_end - blob);
    ((void(*)(void*))buf)(td);
}

int main()
{
    syscall(20);
    syscall(11, load_start_module);
}

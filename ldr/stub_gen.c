#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

asm("stub_entry:\npop %rdi\njmp stub");

void stub_entry(void);

extern FILE* log_handle;

void stub(unsigned char* it)
{
    printf("FATAL: %s stub called!\n", it+19);
    if(log_handle)
        fprintf(log_handle, "FATAL: %s stub called!\n", it+19);
    abort();
}

void* generate_stub(const char* name)
{
    unsigned char* page = mmap(NULL, 16384, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    page[0] = 0xe8;
    page[1] = page[2] = page[3] = page[4] = 0;
    page[5] = 0xff;
    page[6] = 0x25;
    page[7] = 5;
    page[8] = page[9] = page[10] = 0;
    *(unsigned long long*)(page + 16) = (unsigned long long)&stub_entry;
    for(int i = 0; name[i]; i++)
        page[i+24] = name[i];
    return page;
}

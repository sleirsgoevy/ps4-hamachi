#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int readall(int fd, void* b, int cnt)
{
    char* buf = (char*)b;
    while(cnt > 0)
    {
        int chk = read(fd, buf, cnt);
        if(chk <= 0)
            return -1;
        buf += chk;
        cnt -= chk;
    }
    return 0;
}

void* lookup_function(const char* name);
void* lookup_data(const char* name, void* tgt);

int load_elf(const char* path, unsigned long long* entry)
{
    int fd;
    if(!strncmp(path, "/dev/fd/", 8))
    {
        fd = /*dup*/(atoi(path + 8));
        lseek(fd, 0, SEEK_SET);
    }
    else
        fd = open(path, O_RDONLY);
    if(fd < 0)
    {
        perror("open");
        return 1;
    }
    char data[64];
    if(readall(fd, data, 64))
    {
        perror("readall(ehdr)");
        return 1;
    }
    *entry = *(unsigned long long*)(data + 24);
    unsigned long long phoff = *(unsigned long long*)(data + 32);
    if(*(unsigned short*)(data + 54) != 56)
    {
        fprintf(stderr, "invalid ELF!\n");
        return 1;
    }
    int phnum = *(unsigned short*)(data + 56);
    char ph[phnum][56];
    if(lseek(fd, phoff, SEEK_SET) != phoff)
    {
        perror("seek");
        return 1;
    }
    if(readall(fd, ph, phnum*56))
    {
        perror("readall(ph)");
        return 1;
    }
    unsigned long long low_addr = ~0ull;
    unsigned long long high_addr = 0;
    for(int i = 0; i < phnum; i++)
    {
        char* phdr = ph[i];
        if(*(int*)phdr == 1)
        {
            unsigned long long vaddr = *(unsigned long long*)(phdr+16);
            unsigned long long msz = *(unsigned long long*)(phdr+40);
            if(vaddr < low_addr)
                low_addr = vaddr;
            if(vaddr + msz > high_addr)
                high_addr = vaddr + msz;
        }
    }
    low_addr &= -(unsigned long long)(PAGE_SIZE);
    high_addr = (high_addr - 1ull | ((unsigned long long)(PAGE_SIZE)) - 1ull) + 1ull;
    if(mmap((void*)low_addr, high_addr-low_addr, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) != (void*)low_addr)
    {
        perror("mmap");
        return 1;
    }
    for(int i = 0; i < phnum; i++)
    {
        char* phdr = ph[i];
        if(*(int*)phdr == 1)
        {
            unsigned long long vaddr = *(unsigned long long*)(phdr+16);
            unsigned long long offset = *(unsigned long long*)(phdr+8);
            unsigned long long fsz = *(unsigned long long*)(phdr+32);
            if(lseek(fd, offset, SEEK_SET) != offset)
            {
                perror("lseek");
                return 1;
            }
            if(readall(fd, (char*)vaddr, fsz))
            {
                perror("readall(segment)");
                return 1;
            }
        }
    }
    unsigned long long dyn_addr = 0;
    unsigned long long dyn_sz = 0;
    for(int i = 0; i < phnum; i++)
    {
        char* phdr = ph[i];
        if(*(int*)phdr == 2)
        {
            dyn_addr = *(unsigned long long*)(phdr+16);
            dyn_sz = *(unsigned long long*)(phdr+40);
        }
    }
    unsigned long long reladyn = 0;
    unsigned long long reladyn_sz = 0;
    unsigned long long relaplt = 0;
    unsigned long long relaplt_sz = 0;
    unsigned long long symtab = 0;
    unsigned long long strtab = 0;
    for(int i = 0; i < dyn_sz; i += 16)
    {
        unsigned long long key = *(unsigned long long*)(dyn_addr+i);
        unsigned long long value = *(unsigned long long*)(dyn_addr+i+8);
        if(key == 7)
            reladyn = value;
        else if(key == 8)
            reladyn_sz = value;
        else if(key == 23)
            relaplt = value;
        else if(key == 2)
            relaplt_sz = value;
        else if(key == 6)
            symtab = value;
        else if(key == 5)
            strtab = value;
    }
    for(int i = 0; i < reladyn_sz; i += 24)
    {
        unsigned long long a = *(unsigned long long*)(reladyn+i);
        unsigned long long b = *(unsigned long long*)(reladyn+i+8);
        unsigned long long c = *(unsigned long long*)(reladyn+i+16);
        unsigned long long sym = symtab + (b >> 32) * 24;
        unsigned long long name = strtab + *(unsigned int*)sym;
        //printf("%016llx %016llx %016llx %s\n", a, b, c, (char*)name);
        unsigned int kind = (unsigned int)b;
        if(kind == 6);
        else if(kind == 5)
            *(void**)a = lookup_data((const char*)name, (void*)a);
        else if(kind == 7)
            *(void**)a = lookup_function((const char*)name);
        else
        {
            printf("FATAL: unknown relocation %d\n", kind);
            abort();
        }
    }
    //printf("================ ================ ================\n");
    for(int i = 0; i < relaplt_sz; i += 24)
    {
        unsigned long long a = *(unsigned long long*)(relaplt+i);
        unsigned long long b = *(unsigned long long*)(relaplt+i+8);
        unsigned long long c = *(unsigned long long*)(relaplt+i+16);
        unsigned long long sym = symtab + (b >> 32) * 24;
        unsigned long long name = strtab + *(unsigned int*)sym;
        //printf("%016llx %016llx %016llx %s\n", a, b, c, (char*)name);
        unsigned int kind = (unsigned int)b;
        if(kind == 6);
        else if(kind == 5)
            *(void**)a = lookup_data((const char*)name, (void*)a);
        else if(kind == 7)
            *(void**)a = lookup_function((const char*)name);
        else
        {
            printf("FATAL: unknown relocation %d\n", kind);
            abort();
        }
    }
    return 0;
}

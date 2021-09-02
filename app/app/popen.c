#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void* popen_data;
static size_t popen_sz;
static size_t popen_cap;
int is_in_popen;

void popen_write(const void* buf, size_t sz)
{
    size_t new_cap = popen_cap;
    if(sz && !new_cap)
        new_cap++;
    while(popen_sz + sz > new_cap)
        new_cap *= 2;
    if(new_cap > popen_cap)
        popen_data = realloc(popen_data, popen_cap = new_cap);
    memcpy((char*)popen_data + popen_sz, buf, sz);
    popen_sz += sz;
}

void popen_get(void** buf, size_t* sz)
{
    *buf = popen_data;
    *sz = popen_sz;
    popen_data = 0;
    popen_sz = 0;
    popen_cap = 0;
    is_in_popen = 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int my_popen(void** but, size_t* sz, int argc, ...);

char* error_string(void* data, size_t sz)
{
    if(sz >= 5 && !memcmp(((char*)data) + sz - 5, ". ok\n", 5))
    {
        free(data);
        return 0;
    }
    data = realloc(data, sz + 1);
    ((char*)data)[sz] = 0;
    return (char*)data;
}

int errcmp(const char* err, const char* needle)
{
    size_t sz1 = strlen(err);
    size_t sz2 = strlen(needle);
    return !(sz2 <= sz1 && !strcmp(err + sz1 - sz2, needle));
}

static void split_lines(char* data, size_t sz)
{
    for(size_t i = 0; i < sz; i++)
        if(data[i] == '\n')
            data[i] = 0;
}

char** list_networks(int* cnt)
{
    void* buf = 0;
    size_t sz = 0;
    printf("[DEBUG] here!\n");
    my_popen(&buf, &sz, 2, "hamachi", "list");
    printf("[DEBUG] here!\n");
    buf = realloc(buf, sz + 1);
    ((char*)buf)[sz] = 0;
    split_lines((char*)buf, sz);
    char* p = (char*)buf;
    char* endp = p + sz;
    int nnets = 0;
    size_t lnets = 0;
    while(p != endp)
    {
        printf("[DEBUG] %s\n", p);
        if(!strncmp(p, " * [", 4))
        {
            char* ss = strchr(p, ']');
            if(ss)
            {
                nnets++;
                lnets += ss - (p + 4) + 1;
            }
        }
        p += strlen(p) + 1;
    }
    *cnt = nnets;
    void* ans = malloc(nnets * sizeof(char*) + lnets);
    char** ptrs = (char**)ans;
    char* ptrc = (char*)(ptrs + nnets);
    p = (char*)buf;
    while(p != endp)
    {
        if(!strncmp(p, " * [", 4))
        {
            char* ss = strchr(p, ']');
            if(ss)
            {
                *ptrs++ = ptrc;
                memcpy(ptrc, p + 4, ss - (p + 4));
                ptrc += ss - (p + 4);
                *ptrc++ = 0;
            }
        }
        p += strlen(p) + 1;
    }
    printf("[DEBUG] here!\n");
    return (char**)ans;
}

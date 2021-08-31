#if 1
#include <pthread.h>
#include <sys/types.h>
#include <stddef.h>
#include <string.h>

void* musl_malloc(size_t);
void* musl_realloc(void*, size_t);
void musl_free(void*);

//static pthread_mutex_t mtx;
static int mtx;

int init_malloc_lock(void)
{
    //return pthread_mutex_init(&mtx, NULL);
}

void* malloc(size_t sz)
{
    //pthread_mutex_lock(&mtx);
    while(__atomic_exchange_n(&mtx, 1, __ATOMIC_ACQUIRE));
    void** ans = musl_malloc(sz+8);
    //pthread_mutex_unlock(&mtx);
    __atomic_exchange_n(&mtx, 0, __ATOMIC_RELEASE);
    ans[0] = (void*)0xfee1deaddeadbeefll;
    return ans+1;
}

void* realloc(void* old, size_t sz)
{
    if(!old)
        return malloc(sz);
    //pthread_mutex_lock(&mtx);
    while(__atomic_exchange_n(&mtx, 1, __ATOMIC_ACQUIRE));
    void** p = (void**)old;
    p--;
    if(p[0] != (void*)0xfee1deaddeadbeefll)
        abort();
    p[0] = 0;
    void** ans = musl_realloc(p, sz+8);
    ans[0] = (void*)0xfee1deaddeadbeefll;
    //pthread_mutex_unlock(&mtx);
    __atomic_exchange_n(&mtx, 0, __ATOMIC_RELEASE);
    return ans+1;
}

void free(void* p)
{
    if(!p)
        return;
    void** pp = p;
    pp--;
    if(pp[0] != (void*)0xfee1deaddeadbeefll)
        abort();
    pp[0] = 0;
    //pthread_mutex_lock(&mtx);
    while(__atomic_exchange_n(&mtx, 1, __ATOMIC_ACQUIRE));
    musl_free(pp);
    //pthread_mutex_unlock(&mtx);
    __atomic_exchange_n(&mtx, 0, __ATOMIC_RELEASE);
}

char* strdup(const char* src)
{
    char* ans = malloc(strlen(src) + 1);
    strcpy(ans, src);
    return ans;
}
#else
#define stderr __stderrp
#include <stdio.h>
#include <stdlib.h>
#define getenv(...) ((void*)0)
#define stderr __stderrp
#include "../../../ldl/source/libdislocator/libdislocator.so.c"

char* strdup(const char* src)
{
    char* ans = malloc(strlen(src) + 1);
    strcpy(ans, src);
    return ans;
}
#endif

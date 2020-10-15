#include <pthread.h>
#include <sys/types.h>
#include <stddef.h>

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
    void* ans = musl_malloc(sz);
    //pthread_mutex_unlock(&mtx);
    __atomic_exchange_n(&mtx, 0, __ATOMIC_RELEASE);
    return ans;
}

void* realloc(void* old, size_t sz)
{
    //pthread_mutex_lock(&mtx);
    while(__atomic_exchange_n(&mtx, 1, __ATOMIC_ACQUIRE));
    void* ans = musl_realloc(old, sz);
    //pthread_mutex_unlock(&mtx);
    __atomic_exchange_n(&mtx, 0, __ATOMIC_RELEASE);
    return ans;
}

void free(void* p)
{
    //pthread_mutex_lock(&mtx);
    while(__atomic_exchange_n(&mtx, 1, __ATOMIC_ACQUIRE));
    musl_free(p);
    //pthread_mutex_unlock(&mtx);
    __atomic_exchange_n(&mtx, 0, __ATOMIC_RELEASE);
}

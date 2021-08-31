#include <stdlib.h>
#include "handles.h"

#define MAGIC ((void*)0xfee1deaddeadbeefll)

struct Resource
{
    void* magic;
    struct Resource* next;
    struct Resource** prev;
    void(*free)(void*);
};

static int lock;
static struct Resource* list;
#define LOCK() while(__atomic_exchange_n(&lock, 1, __ATOMIC_ACQUIRE))
#define UNLOCK() __atomic_exchange_n(&lock, 0, __ATOMIC_RELEASE)

void* resource_alloc(size_t sz, void(*free)(void*))
{
    LOCK();
    struct Resource* buf = malloc(sz+sizeof(struct Resource));
    buf->magic = MAGIC;
    buf->next = list;
    buf->prev = &list;
    buf->free = free;
    list = buf;
    UNLOCK();
    return buf + 1;
}

void* resource_realloc(void* h, size_t new_sz)
{
    LOCK();
    struct Resource* cur = (struct Resource*)h - 1;
    if(cur->magic != MAGIC)
        abort();
    cur = realloc(cur, new_sz + sizeof(struct Resource));
    *cur->prev = cur;
    cur->next->prev = &cur->next;
    UNLOCK();
    return cur + 1;
}

void resource_free(void* h)
{
    if(!h)
        return;
    struct Resource* res = (struct Resource*)h - 1;
    if(res->magic != MAGIC)
        abort();
    LOCK();
    res->free(h);
    *res->prev = res->next;
    res->next->prev = res->prev;
    UNLOCK();
    free(res);
}

void resource_collect(void)
{
    LOCK();
    while(list)
    {
        struct Resource* cur = list;
        if(cur->magic != MAGIC)
            abort();
        cur->free(cur+1);
        list = cur->next;
        free(cur);
    }
    UNLOCK();
}

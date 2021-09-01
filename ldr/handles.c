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
    if(list)
        list->prev = &buf->next;
    buf->magic = MAGIC;
    buf->next = list;
    buf->prev = &list;
    buf->free = free;
    list = buf;
    UNLOCK();
    return buf + 1;
}

#if 0

static int check_heap(void* badres)
{
    struct Resource** prev = &list;
    struct Resource* cur = *prev;
    while(cur)
    {
        if((cur + 1) == badres)
            return 1;
        if(cur->prev != prev)
            abort();
        prev = &cur->next;
        cur = *prev;
    }
    return 0;
}

#else

#define check_heap(...) 0 && 0

#endif

void* resource_realloc(void* h, size_t new_sz)
{
    LOCK();
    struct Resource* cur = (struct Resource*)h - 1;
    if(cur->magic != MAGIC)
        abort();
    if(!check_heap(h))
        abort();
    struct Resource* bak = cur;
    cur = realloc(cur, new_sz + sizeof(struct Resource));
    *cur->prev = cur;
    cur->next->prev = &cur->next;
    if(bak != cur && check_heap(h))
        abort();
    UNLOCK();
    return cur + 1;
}

void resource_free(void* h)
{
    if(!h)
        return;
    struct Resource* res = (struct Resource*)h - 1;
    LOCK();
    if(res->magic != MAGIC)
        abort();
    if(!check_heap(h))
        abort();
    res->free(h);
    *res->prev = res->next;
    res->next->prev = res->prev;
    if(check_heap(h))
        abort();
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

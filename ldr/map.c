#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#define MASK 0xdeadbeefu

struct hashmap_entry
{
    in_addr_t key;
    char mac[6];
    struct hashmap_entry* next;
};

static struct hashmap_entry** data = 0;
static int l = 0;
static int cap = 0;

char* get_mac_for_ip_internal(in_addr_t addr)
{
    if(!cap)
        return 0;
    int hsh = (addr ^ MASK) % cap;
    for(struct hashmap_entry* x = data[hsh]; x; x = x->next)
        if(x->key == addr)
            return x->mac;
    return 0;
}

int get_mac_for_ip(in_addr_t addr, char* mac)
{
    char* ans = get_mac_for_ip_internal(addr);
    if(ans)
    {
        for(int i = 0; i < 6; i++)
            mac[i] = ans[i];
        return 1;
    }
    return 0;
}

void set_mac_for_ip(in_addr_t addr, const char* mac)
{
    char* ans = get_mac_for_ip_internal(addr);
    if(ans)
    {
        for(int i = 0; i < 6; i++) 
            ans[i] = mac[i];
        return;
    }
    if(l == 2 * cap)
    {
        int ll = 2 * cap + 1;
        struct hashmap_entry** e = malloc(sizeof(*e) * ll);
        for(int i = 0; i < cap; i++)
            for(struct hashmap_entry* x = data[i]; x;)
            {
                struct hashmap_entry* next = x->next;
                int hsh = (x->key ^ MASK) % ll;
                x->next = e[hsh];
                e[hsh] = x;
                x = next;
            }
        free(data);
        data = e;
        cap = ll;
    }
    int hsh = (addr ^ MASK) % cap;
    struct hashmap_entry* cur = malloc(sizeof(*cur));
    cur->key = addr;
    for(int i = 0; i < 6; i++)
        cur->mac[i] = mac[i];
    cur->next = data[hsh];
    data[hsh] = cur;
}

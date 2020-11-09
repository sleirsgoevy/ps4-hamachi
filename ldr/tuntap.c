#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdio.h>

#ifdef DEBUG_PRINTS
#define dbg_printf printf
#else
#define dbg_printf(...)
#endif

int get_mac_for_ip(in_addr_t, char*);
void set_mac_for_ip(in_addr_t, const char*);

char macaddr[6];
in_addr_t ipaddr;
in_addr_t brdaddr;

void tuntap_set_hwaddr(const char* m)
{
    for(int i = 0; i < 6; i++)
        macaddr[i] = m[i];
}

void tuntap_set_ipaddr(in_addr_t i)
{
    ipaddr = i;
}

void tuntap_set_brdaddr(in_addr_t b)
{
    brdaddr = b;
}

int get_mac_for_ip_mbbr(in_addr_t addr, char* mac)
{
    if(addr == brdaddr)
    {
        for(int i = 0; i < 6; i++)
            mac[i] = (char)0xff;
        return 1;
    }
    return get_mac_for_ip(addr, mac);
}

void ethernet_header(char* pkt, const char* dst, const char* src, uint16_t kind)
{
    for(int i = 0; i < 6; i++)
    {
        pkt[i] = dst[i];
        pkt[i+6] = src[i];
    }
    pkt[12] = (char)(unsigned char)(kind >> 8);
    pkt[13] = (char)(unsigned char)kind;
}

void arp_request(char* pkt, int q7, const char* src_mac, const char* dst_mac, in_addr_t src, in_addr_t dst)
{
    pkt[0] = 0;
    pkt[1] = 1;
    pkt[2] = 8;
    pkt[3] = 0;
    pkt[4] = 6;
    pkt[5] = 4;
    pkt[6] = 0;
    pkt[7] = q7;
    for(int i = 0; i < 6; i++)
    {
        pkt[8 + i] = src_mac[i];
        pkt[18 + i] = dst_mac[i];
    }
    *(in_addr_t*)(pkt + 14) = src;
    *(in_addr_t*)(pkt + 24) = dst;
}

int is_arp(const char* pkt)
{
    return pkt[0] == 0
        && pkt[1] == 1
        && pkt[2] == 8
        && pkt[3] == 0
        && pkt[4] == 6
        && pkt[5] == 4
        && pkt[6] == 0
        && (pkt[7] == 1 || pkt[7] == 2);
}

#define BROADCAST "\xff\xff\xff\xff\xff\xff"
#define UNKNOWN "\0\0\0\0\0\0"

static char tuntap_fakepkt[65536];
static int tuntap_fakepkt_sz;

int tuntap_has_fake_pkt()
{
    return tuntap_fakepkt_sz;
}

ssize_t tuntap_read(int fd, char* buf, size_t cnt)
{
    if(tuntap_fakepkt_sz)
    {
        dbg_printf("[tuntap] injecting fake packet\n");
        int sz = tuntap_fakepkt_sz;
        if(sz > cnt)
            sz = cnt;
        for(int i = 0; i < sz; i++)
            buf[i] = tuntap_fakepkt[i];
        tuntap_fakepkt_sz = 0;
        return sz;
    }
    ssize_t ans = read(fd, buf, cnt);
    if(ans <= 0)
        return ans;
    dbg_printf("[tuntap] len = %d\n", ans);
    if(ans < 20)
    {
        dbg_printf("[tuntap] %d < 20\n", ans);
        goto skip_packet;
    }
    in_addr_t src = *(in_addr_t*)(buf + 12);
    in_addr_t dst = *(in_addr_t*)(buf + 16);
    char dst_mac[6];
    if(!get_mac_for_ip_mbbr(dst, dst_mac)) // dst unknown, need arp
    {
        dbg_printf("[tuntap] dst unknown, sending arp\n");
        if(cnt < 42) // not enough space for arp
        {
            dbg_printf("[tuntap] %d < 42\n", ans);
            goto skip_packet;
        }
        ethernet_header(buf, BROADCAST, macaddr, 0x0806);
        arp_request(buf+14, 1, macaddr, UNKNOWN, src, dst);
        return 42;
    }
    else // dst known
    {
        dbg_printf("[tuntap] dst known, sending encapsulated ip\n");
        if(ans + 14 > cnt)
        {
            dbg_printf("[tuntap] %d < %d\n", cnt, ans + 14);
            goto skip_packet;
        }
        for(off_t i = ans - 1; i >= 0; i--)
            buf[i+14] = buf[i];
        ethernet_header(buf, dst_mac, macaddr, 0x0800);
        return ans + 14;
    }
skip_packet:
    errno = 11; // linux EAGAIN
    return -1;
}

ssize_t tuntap_write(int fd, const char* buf, size_t cnt)
{
    int is_broadcast = 1;
    int is_targeted = 1;
    if(cnt < 14)
    {
        dbg_printf("[tuntap] packet too short, not ethernet\n");
        return cnt; // drop
    }
    for(int i = 0; i < 6; i++)
    {
        if(buf[i] != macaddr[i])
            is_targeted = 0;
        if(buf[i] != (char)0xff)
            is_broadcast = 0;
    }
    if(!is_targeted && !is_broadcast)
    {
        dbg_printf("[tuntap] packet is not for us, dropping\n");
        return cnt; // drop
    }
    int protocol = ((unsigned char)buf[12]) << 8 | (unsigned char)buf[13];
    if(protocol == 0x0800) // ip
    {
        dbg_printf("[tuntap] ip packet received\n");
        /*dbg_printf("[tuntap]");
        for(off_t i = 14; i < cnt; i++)
            dbg_printf(" %02x", (unsigned int)(unsigned char)buf[i]);
        dbg_printf("\n");*/
        ssize_t ans = write(fd, buf + 14, cnt - 14);
        if(ans < 0)
        {
            dbg_printf("[tuntap] errno = %d\n", errno);
            return ans;
        }
        return ans + 14;
    }
    else if(protocol == 0x0806) // arp
    {
        dbg_printf("[tuntap] arp packet received\n");
        if(cnt < 42 || !is_arp(buf + 14))
        {
            dbg_printf("[tuntap] unknown arp packet received\n");
            return cnt; // drop
        }
        set_mac_for_ip(*(const in_addr_t*)(buf + 28), buf + 22);
        if(buf[21] == 1 && *(in_addr_t*)(buf + 38) == ipaddr) // arp request
        {
            dbg_printf("[tuntap] injecting arp response\n");
            ethernet_header(tuntap_fakepkt, buf+6, macaddr, 0x0806);
            arp_request(tuntap_fakepkt+14, 2, macaddr, buf+6, ipaddr, *(in_addr_t*)(buf + 28));
            tuntap_fakepkt_sz = 42;
        }
        return cnt; // eaten
    }
    else
        return cnt; // drop
}

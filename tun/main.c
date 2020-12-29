#define m_free not_m_free
#include <sys/types.h>
#include <sys/conf.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/malloc.h>
#include <sys/proc.h>
#include <sys/uio.h>
#include <sys/param.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/syscall.h>
#include <sys/sysent.h>
#include <sys/sockio.h>
#include <sys/queue.h>
#include <machine/atomic.h>
#include <net/if.h>
#include <net/if_types.h>
#include <net/if_var.h>
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <net/netisr.h>
#include <netinet/in.h>
#include <stddef.h>

#ifdef PS4
#define bpfattach(...)
#define bpfdetach(...)
#define bpf_mtap2(...)
#else
#include <net/bpf.h>
#endif

#ifdef PS4
#define IF_OUTPUT(x) (*((typeof((x).if_output)*)&(x).if_ioctl))
#define IF_ADDR_MTX(x) (*((typeof((x).if_addr_mtx)*)(((unsigned long long)&x)+0x4c0)))
#else
#define IF_OUTPUT(x) ((x).if_output)
#define IF_ADDR_MTX(x) ((x).if_addr_mtx)
#endif

struct malloc_type* get_M_TEMP(void);
struct sysent* get_sysent(void);

asm("uma_zfree_arg:\nret");
asm("m_free_ext:\nret");

#undef m_free
void m_free(struct mbuf*);

static volatile int used[10] = {0};
static struct ifnet* volatile ifaces[10] = {0};

int acquire_idx(void)
{
    for(int i = 0; i < 10; i++)
        if(atomic_cmpset_int(used+i, 0, 1))
            return i;
    return -1;
}

void release_idx(int i)
{
    atomic_cmpset_int(used+i, 1, 0);
}

#define IFACE_ACQUIRED ((struct ifnet*)1)

struct ifnet* acquire_iface(int i)
{
    for(;;)
    {
        struct ifnet* ans = ifaces[i];
        if(ans == 0)
            return 0;
        if(ans != IFACE_ACQUIRED && atomic_cmpset_ptr((volatile intptr_t*)(ifaces+i), (intptr_t)ans, (intptr_t)IFACE_ACQUIRED))
            return ans;
    }
}

struct ifnet* release_iface(int i, struct ifnet* iface)
{
    atomic_store_rel_ptr((volatile intptr_t*)(ifaces+i), (intptr_t)iface);
}

typedef volatile int tinylock;

static inline void tiny_lock(tinylock* x)
{
    while(!atomic_cmpset_int(x, 0, 1));
}

static inline void tiny_unlock(tinylock* x)
{
    atomic_cmpset_int(x, 1, 0);
}

struct opaque
{
    struct socket* sender;
    struct socket* receiver;
    struct ifnet* iface;
    int idx;
    struct mtx lock;
    tinylock free_lock;
};

static int family = 0x0800;

int tun_output(struct ifnet* i, struct mbuf* m, struct sockaddr* a, struct route* r)
{
    bpf_mtap2(i->if_bpf, &family, 4, m);
    struct opaque* op = (struct opaque*)i->if_softc;
    if(!op)
        return 0;
    mtx_lock(&op->lock);
    if(a->sa_family == AF_UNSPEC)
        a->sa_family = *(typeof(&a->sa_family))a->sa_data;
    size_t sz = m_length(m, NULL);
    if(a->sa_family == AF_INET && sz <= 65536)
    {
        unsigned char* data = malloc(sz, get_M_TEMP(), M_WAITOK);
        m_copydata(m, 0, sz, data);
        struct iovec iov = {data, sz};
        struct uio uio = {
            .uio_iov = &iov,
            .uio_iovcnt = 1,
            .uio_offset = 0,
            .uio_resid = sz,
            .uio_segflg = UIO_SYSSPACE,
            .uio_rw = UIO_WRITE,
            .uio_td = NULL
        };
        int q = sosend(op->sender, NULL, &uio, NULL, NULL, MSG_DONTWAIT, curthread);
        free(data, get_M_TEMP());
    }
    mtx_unlock(&op->lock);
    m_freem(m);
    return 0;
}

void opaque_free(void* o)
{
    //TODO: may be racey
    struct opaque* op = (struct opaque*)o;
    printf("[tun] shutting down...\n");
    mtx_lock(&op->lock);
    acquire_iface(op->idx);
    atomic_store_rel_ptr((intptr_t*)(ifaces+op->idx), 0);
    printf("[tun] tun%d: in destructor\n", op->idx);
    soshutdown(op->receiver, SHUT_RDWR);
    printf("[tun] receiver shut down\n");
    tiny_lock(&op->free_lock);
#ifdef IFNET_CACHE
    if_down(op->iface);
    op->iface->if_softc = 0;
    printf("[tun] now down!\n");
#else
    bpfdetach(op->iface);
    if_detach(op->iface);
    if_free(op->iface);
    printf("[tun] freed!\n");
#endif
    soclose(op->sender);
    printf("[tun] closed #1\n");
    soclose(op->receiver);
    printf("[tun] closed #2\n");
    printf("[tun] free(0x%llx) [idx = %d]\n", op, op->idx);
    tiny_unlock(&op->free_lock);
    mtx_unlock(&op->lock);
    mtx_destroy(&op->lock);
    free(op, get_M_TEMP());
    printf("[tun] opaque freed\n");
    int idx = op->idx;
    release_idx(idx);
}

#ifdef IFNET_CACHE
struct ifnet* ifnet_cache[10] = {0};
#endif

struct opaque* opaque_alloc(struct thread* td, int idx)
{
    struct opaque* op = malloc(sizeof(struct opaque), get_M_TEMP(), M_ZERO|M_WAIT);
    printf("[tun] opaque = 0x%llx\n", op);
    mtx_init(&op->lock, "tun op->lock", NULL, MTX_DEF);
    op->free_lock = 0;
    mtx_lock(&op->lock);
    tiny_lock(&op->free_lock);
    op->idx = idx;
    printf("[tun] .idx = %d\n", idx);
    socreate(AF_UNIX, &op->sender, SOCK_DGRAM, 0, td->td_ucred, td);
    printf("[tun] .sender = 0x%llx\n", op->sender);
    socreate(AF_UNIX, &op->receiver, SOCK_DGRAM, 0, td->td_ucred, td);
    printf("[tun] .receiver = 0x%llx\n", op->receiver);
    soconnect2(op->sender, op->receiver);
    soconnect2(op->receiver, op->sender);
#ifdef IFNET_CACHE
    op->iface = ifnet_cache[idx];
#else
    op->iface = if_alloc(IFT_OTHER);
    op->iface->if_flags |= IFF_BROADCAST;
    if_initname(op->iface, "tun", idx);
    op->iface->if_softc = NULL;
    op->iface->if_mtu = 512;
    IF_OUTPUT(*op->iface) = tun_output;
    if_attach(op->iface);
    bpfattach(op->iface, DLT_NULL, 4);
#endif
    printf("[tun] .iface = 0x%llx\n", op->iface);
    op->iface->if_softc = op;
    printf("[tun] tun%d attached!\n", idx);
    tiny_unlock(&op->free_lock);
    release_iface(idx, op->iface);
    mtx_unlock(&op->lock);
    return op;
}

int tun_open(struct cdev* cdev, int oflags, int devtype, struct thread* td)
{
    int i = acquire_idx();
    if(i < 0)
        return (ENOMEM);
    return devfs_set_cdevpriv(opaque_alloc(td, i), opaque_free);
}

int tun_read(struct cdev* cdev, struct uio* uio, int ioflag)
{
    struct opaque* o = NULL;
    devfs_get_cdevpriv((void**)&o);
    tiny_lock(&o->free_lock);
    int ans = soreceive(o->receiver, NULL, uio, NULL, NULL, 0);
    tiny_unlock(&o->free_lock);
    return ans;
}

int tun_write(struct cdev* cdev, struct uio* uio, int ioflag)
{
    struct opaque* o = NULL;
    devfs_get_cdevpriv((void**)&o);
    size_t sz = uio->uio_resid;
    if(sz >= 65536 /* max allowed ip packet size */)
        return (ENOMEM);
    unsigned char* buf = malloc(sz, get_M_TEMP(), M_NOWAIT);
    if(!buf)
        return (ENOMEM);
    int error = uiomove(buf, uio->uio_resid, uio);
    if(error)
    {
        free(buf, get_M_TEMP());
        return error;
    }
    mtx_lock(&o->lock);
    struct mbuf* m = m_devget(buf, sz, 0, o->iface, NULL);
    if(!m)
    {
        free(buf, get_M_TEMP());
        mtx_unlock(&o->lock);
        return (ENOMEM);
    }
    bpf_mtap2(o->iface->if_bpf, &family, 4, m);
    netisr_queue(NETISR_IP, m);
    free(buf, get_M_TEMP());
    mtx_unlock(&o->lock);
    return (0);
}

int tun_ioctl(struct cdev* dev, u_long cmd, caddr_t data, int fflag, struct thread* td)
{
    printf("ioctl(0x%llx, 0x%llx, 0x%llx, %d, 0x%llx)\n", dev, cmd, data, fflag, td);
    if((unsigned int)cmd == 0x4004beef)
    {
        struct opaque* o = NULL;
        devfs_get_cdevpriv((void**)&o);
        *(int*)data = o->idx;
        return (0);
    }
    return (ENOTTY);
}

int tun_poll(struct cdev* dev, int events, struct thread* td)
{
    struct opaque* o = NULL;
    devfs_get_cdevpriv((void**)&o);
    tiny_lock(&o->free_lock);
    int ans = sopoll(o->receiver, events, td->td_ucred, td);
    tiny_unlock(&o->free_lock);
    return ans;
}

struct cdevsw tun_vtable = {
    .d_version = D_VERSION,
    .d_open = NULL,
    .d_close = NULL,
    .d_read = NULL,
    .d_write = NULL,
    .d_ioctl = NULL,
    .d_poll = NULL,
    .d_name = NULL,
};

int (*p_old_sendto)(struct thread*, void*);

int new_sendto(struct thread* td, void* uap)
{
    unsigned long long* params = (unsigned long long*)uap;
    if(!params[4] || params[5] > 64) // len
        return p_old_sendto(td, uap);
    char buf[64];
    int rv = copyin((void*)params[4], buf, params[5]);
    if(rv)
        return (rv);
    struct sockaddr_in* sin = (struct sockaddr_in*)buf;
    if(sin->sin_family != AF_INET || sin->sin_addr.s_addr != INADDR_BROADCAST)
        return p_old_sendto(td, uap);
    int(*sys_ioctl)(struct thread*, void*) = get_sysent()[SYS_ioctl].sy_call;
    params[4] = (unsigned long long)sin;
    for(int i = 0; i < 10; i++)
    {
        struct ifnet* iface = acquire_iface(i);
        if(!iface)
            continue;
        mtx_lock(&IF_ADDR_MTX(*iface));
        struct ifaddr* ia;
        TAILQ_FOREACH(ia, &iface->if_addrhead, ifa_link)
        {
#ifdef PS4
            struct ifaddr* ia2 = *(void**)(((unsigned long long)ia)+208); //wtf is this??
#else
            struct ifaddr* ia2 = ia;
#endif
            if(!ia2 || !ia2->ifa_broadaddr || ia2->ifa_broadaddr->sa_family != AF_INET)
            {
                printf("[tun] tun%d: wtf: invalid address\n", i);
                continue;
            }
            sin->sin_addr = ((struct sockaddr_in*)ia2->ifa_broadaddr)->sin_addr;
            //printf("[tun] tun%d: broadcast to %08x\n", i, __builtin_bswap32(sin->sin_addr.s_addr));
            if((rv = p_old_sendto(td, params)))
                printf("[tun] tun%d: could not sendto: %d\n", i, rv);
        }
        mtx_unlock(&IF_ADDR_MTX(*iface));
        release_iface(i, iface);
    }
    sin->sin_addr.s_addr = INADDR_BROADCAST;
    return p_old_sendto(td, params);
}

int main(struct thread* td)
{
    printf("[tun] Hello, kernel world!\n");
#ifdef IFNET_CACHE
    for(int i = 0; i < 10; i++)
    {
        ifnet_cache[i] = if_alloc(IFT_OTHER);
        if_initname(ifnet_cache[i], "tun", i);
        ifnet_cache[i]->if_softc = NULL;
        ifnet_cache[i]->if_mtu = 1500;
        IF_OUTPUT(*ifnet_cache[i]) = tun_output;
        if_attach(ifnet_cache[i]);
        bpfattach(ifnet_cache[i], DLT_NULL, 4);
        printf("[tun] tun%d preallocated\n", i);
    }
#endif
    struct cdev* tun;
    tun_vtable.d_open = tun_open;
    tun_vtable.d_read = tun_read;
    tun_vtable.d_write = tun_write;
    tun_vtable.d_ioctl = tun_ioctl;
    tun_vtable.d_poll = tun_poll;
    tun_vtable.d_name = "tun";
    p_old_sendto = get_sysent()[SYS_sendto].sy_call;
    get_sysent()[SYS_sendto].sy_call = new_sendto;
    make_dev_p(MAKEDEV_CHECKNAME | MAKEDEV_WAITOK, &tun, &tun_vtable, 0, UID_ROOT, GID_WHEEL, 0600, "tun");
    printf("[tun] Goodbye, kernel world!\n");
    return 0;
}

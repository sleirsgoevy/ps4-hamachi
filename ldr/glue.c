#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/utsname.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/sockio.h>
#include <sys/un.h>
#include <netdb.h>
#include "linux_structs.h"

#ifdef DEBUG_PRINTS
#define dbg_printf printf
#else
#define dbg_printf(...)
#endif

void impl_exit(int);

extern int my_argc;
extern const char** my_argv;

void impl___libc_start_main(int(*main)(int, const char**), int _1, void* _2, void(*init)(void), void(*fini)(void), void(*rtld_fini)(void), void* stack_end)
{
    init();
    impl_exit(main(my_argc, my_argv));
}

const char* impl_getenv(const char* name)
{
    dbg_printf("getenv(%s)\n", name);
    return 0;
}

void* impl_malloc(size_t sz)
{
    void* ans = malloc(sz);
    //printf("malloc(%lld) = %p\n", sz, ans);
    return ans;
}

void* impl_realloc(void* ptr, size_t new_sz)
{
    void* ans = realloc(ptr, new_sz);
    //printf("realloc(%p, %lld) = %p\n", ptr, new_sz);
    return ans;
}

void impl_free(void* ptr)
{
    //printf("free(%p)\n", ptr);
    if(ptr != (void*)0x97c4e0 && ptr != (void*)0x600c4520)
        free(ptr);
}

void* lookup_data(const char* name, void* zero);

int impl_getopt(int argc, char** argv, const char* optstring)
{
    dbg_printf("getopt(%d, [", argc);
    for(int i = 0; i < argc; i++)
        dbg_printf("\"%s\", ", argv[i]);
    dbg_printf("], \"%s\")", optstring);
    int ans = getopt(argc, argv, optstring);
    dbg_printf(" = %d\n", ans);
    static char** p_optarg;
    static int* p_optind;
    static int* p_opterr;
    static int* p_optopt;
    if(!p_optarg)
        p_optarg = lookup_data("optarg", 0);
    if(!p_optind)
        p_optind = lookup_data("optind", 0);
    if(!p_opterr)
        p_opterr = lookup_data("opterr", 0);
    if(!p_optopt)
        p_optopt = lookup_data("optopt", 0);
    *p_optarg = optarg;
    *p_optind = optind;
    *p_opterr = opterr;
    *p_optopt = optopt;
    return ans; 
}

int impl___printf_chk(int flag, const char* fmt, ...)
{
    va_list x;
    va_start(x, fmt);
    int ans = vprintf(fmt, x);
    va_end(x);
    return ans;
}

int impl___vprintf_chk(int flag, const char* fmt, va_list vl)
{
    return vprintf(fmt, vl);
}

int impl___fprintf_chk(FILE* f, int flag, const char* fmt, ...)
{
    va_list x;
    va_start(x, fmt);
    int ans = vfprintf(f, fmt, x);
    va_end(x);
    return ans;
}

int impl___vfprintf_chk(FILE* f, int flag, const char* fmt, va_list vl)
{
    return vfprintf(f, fmt, vl);
}

int impl___vsnprintf_chk(char* s, size_t ml, int f, size_t l, const char* fmt, va_list vl)
{
    return vsnprintf(s, ml, fmt, vl);
}

int impl___snprintf_chk(char* s, size_t ml, int f, size_t l, const char* fmt, ...)
{
    va_list v;
    va_start(v, fmt);
    int ans = vsnprintf(s, ml, fmt, v);
    va_end(v);
    return ans;
}

int impl___sprintf_chk(char* s, int flag, size_t l, const char* fmt, ...)
{
    va_list v;
    va_start(v, fmt);
    int ans = vsprintf(s, fmt, v);
    va_end(v);
    return ans;
}

void* impl___memcpy_chk(void* dest, const void* src, size_t len, size_t destlen)
{
    return memcpy(dest, src, len);
}

char* impl___strcpy_chk(char* dst, const char* src, size_t dstlen)
{
    return strcpy(dst, src);
}

void impl_exit(int code)
{
    dbg_printf("exit(%d)\n", code);
    exit(code);
}

int impl_getuid(void)
{
    return 0;
}

static char* build_fake_path(const char* path)
{
    size_t l = strlen(path);
    char* ans = malloc(sizeof(PREFIX)+l);
    strcpy(ans, PREFIX);
    strcpy(ans + sizeof(PREFIX) - 1, path);
    return ans;
}

static void xstat64_copy(struct linux_stat64* stat_buf, struct stat* fbsd_stat_buf)
{
    stat_buf->st_dev = fbsd_stat_buf->st_dev;
    stat_buf->st_ino = fbsd_stat_buf->st_ino;
    stat_buf->st_nlink = fbsd_stat_buf->st_nlink;
    stat_buf->st_mode = fbsd_stat_buf->st_mode;
    stat_buf->st_uid = fbsd_stat_buf->st_uid;
    stat_buf->st_gid = fbsd_stat_buf->st_gid;
    stat_buf->st_rdev = fbsd_stat_buf->st_rdev;
    stat_buf->st_size = fbsd_stat_buf->st_size;
    stat_buf->st_blksize = fbsd_stat_buf->st_blksize;
    stat_buf->st_blocks = fbsd_stat_buf->st_blocks;
    stat_buf->st_atim.tv_sec = fbsd_stat_buf->st_atim.tv_sec;
    stat_buf->st_atim.tv_nsec = fbsd_stat_buf->st_atim.tv_nsec;
    stat_buf->st_mtim.tv_sec = fbsd_stat_buf->st_mtim.tv_sec;
    stat_buf->st_mtim.tv_nsec = fbsd_stat_buf->st_mtim.tv_nsec;
    stat_buf->st_ctim.tv_sec = fbsd_stat_buf->st_ctim.tv_sec;
    stat_buf->st_ctim.tv_nsec = fbsd_stat_buf->st_ctim.tv_nsec;
}

int impl___xstat64(int ver, const char* path, struct linux_stat64* stat_buf)
{
    dbg_printf("__xstat64(%d, \"%s\", %p)", ver, path, stat_buf);
    if(!strcmp(path, "/dev/net/tun"))
    {
        dbg_printf(" = FAKE\n");
        stat_buf->st_mode = 020666;
        stat_buf->st_ino = 179;
        stat_buf->st_dev = 6;
        stat_buf->st_nlink = 1;
        stat_buf->st_uid = 0;
        stat_buf->st_gid = 0;
        stat_buf->st_size = 0;
        stat_buf->st_atime = 0;
        stat_buf->st_mtime = 0;
        stat_buf->st_ctime = 0;
        return 0;
    }
    struct stat fbsd_stat_buf;
    char* pp = build_fake_path(path);
    int ans = stat(pp, &fbsd_stat_buf);
    dbg_printf(" = %d\n", ans);
    free(pp);
    if(!ans)
        xstat64_copy(stat_buf, &fbsd_stat_buf);
    return ans;
}

int impl___fxstat64(int ver, int fd, struct linux_stat64* stat_buf)
{
    struct stat fbsd_stat_buf;
    int ans = fstat(fd, &fbsd_stat_buf);
    if(!ans)
        xstat64_copy(stat_buf, &fbsd_stat_buf);
    return ans;
}

int impl_mkdir(const char* path, mode_t mode)
{
    dbg_printf("mkdir(\"%s\")", path);
    char* pp = build_fake_path(path);
    int ans = mkdir(pp, mode);
    free(pp);
    dbg_printf(" = %d\n", ans);
    return ans;
}

int impl_open64(const char* path, int flags, mode_t mode)
{
    dbg_printf("open64(\"%s\", %d, %d)", path, flags, mode);
    int fd;
    int flags2 = flags & 3;
    if(flags & 64)
        flags2 |= O_CREAT;
    if(flags & 2048)
        flags2 |= O_NONBLOCK;
    if(!strcmp(path, "/dev/urandom"))
        fd = open(path, flags2, mode);
    else if(!strcmp(path, "/dev/net/tun"))
        fd = open("/dev/tun", flags2, mode);
    else
    {
        char* pp = build_fake_path(path);
        fd = open(pp, flags2, mode);
        free(pp);
    }
    dbg_printf(" = %d\n", fd);
    return fd;
}

int* impl___errno_location(void)
{
    return &errno;
}

int impl_uname(struct linux_utsname* u)
{
    struct utsname uu;
    strncpy(u->sysname, "PS4", 65);
    strncpy(u->nodename, "localhost", 65);
    strncpy(u->release, "", 65);
    strncpy(u->version, "", 65);
    strncpy(u->machine, "", 65);
    strncpy(u->domainname, "localdomain", 65);
    return 0;
}

int impl_signal(int sig, void* hdlr)
{
    dbg_printf("signal(%d, %p)\n", sig, hdlr);
    return 0;
}

int impl_daemon(int nochdir, int noclose)
{
    dbg_printf("daemon(%d, %d)\n", nochdir, noclose);
    return 0;
}

FILE* impl_fopen64(const char* path, const char* mode)
{
    dbg_printf("fopen64(\"%s\", \"%s\")", path, mode);
    FILE* ans;
    if(!strcmp(path, "/var/lib/logmein-hamachi/h2-engine.log"))
        ans = fopen("/dev/console", mode);
    else
    {
        char* pp = build_fake_path(path);
        ans = fopen(pp, mode);
        free(pp);
    }
    dbg_printf(" = %p\n", ans);
    return ans;
}

struct linux_tm* impl_localtime(const time_t* t)
{
    struct tm* x = localtime(t);
    if(!x)
        return 0;
    static struct linux_tm ans;
    ans.tm_sec = x->tm_sec;
    ans.tm_min = x->tm_min;
    ans.tm_hour = x->tm_hour;
    ans.tm_mday = x->tm_mday;
    ans.tm_mon = x->tm_mon;
    ans.tm_year = x->tm_year;
    ans.tm_wday = x->tm_wday;
    ans.tm_yday = x->tm_yday;
    ans.tm_isdst = x->tm_isdst;
    return &ans;
}

int impl_system(const char* cmd)
{
    dbg_printf("system(\"%s\")\n", cmd);
    return -1;
}

int impl_unlink(const char* path)
{
    dbg_printf("unlink(\"%s\")\n", path);
    char* pp = build_fake_path(path);
    int ans = unlink(pp);
    free(pp);
    return ans;
}

static int have_epoll = 0;
static fd_set epoll_in;
static fd_set epoll_out;
static fd_set epoll_err;
static linux_epoll_data_t epoll_per_fd_data[FD_SETSIZE];

static int tun_fd = -1;

int impl_close(int fd)
{
    dbg_printf("close(%d)\n", fd);
    if(fd == 65536 && have_epoll)
    {
        have_epoll = 0;
        return 0;
    }
    else
    {
        if(fd == tun_fd)
            tun_fd = -1;
        return close(fd);
    }
}

int impl_epoll_create(int sz)
{
    if(have_epoll)
    {
        dbg_printf("FATAL: epoll_create called twice!\n");
        abort();
    }
    have_epoll = 1;
    FD_ZERO(&epoll_in);
    FD_ZERO(&epoll_out);
    FD_ZERO(&epoll_err);
    return 65536;
}

int impl_epoll_ctl(int fd, int op, int fd2, struct linux_epoll_event* e)
{
    dbg_printf("epoll_ctl(%d, %d, %d)\n", fd, op, fd2);
    if(fd != 65536 || !have_epoll)
    {
        errno = EBADF;
        return -1;
    }
    if(op == LINUX_EPOLL_CTL_ADD || op == LINUX_EPOLL_CTL_MOD)
    {
        if(op == LINUX_EPOLL_CTL_MOD)
        {
            if(!(e->events & LINUX_EPOLLIN))
                FD_SET(fd2, &epoll_in);
            if(!(e->events & LINUX_EPOLLOUT))
                FD_SET(fd2, &epoll_out);
            if(!(e->events & LINUX_EPOLLERR))
                FD_SET(fd2, &epoll_err);
        }
        if(e->events & LINUX_EPOLLIN)
            FD_SET(fd2, &epoll_in);
        if(e->events & LINUX_EPOLLOUT)
            FD_SET(fd2, &epoll_out);
        if(e->events & LINUX_EPOLLERR)
            FD_SET(fd2, &epoll_err);
        epoll_per_fd_data[fd2] = e->data;
        return 0;
    }
    else if(op == LINUX_EPOLL_CTL_DEL)
    {
        FD_CLR(fd2, &epoll_in);
        FD_CLR(fd2, &epoll_out);
        FD_CLR(fd2, &epoll_err);
        return 0;
    }
    dbg_printf("epoll_ctl(%d, %d, %d)\n", op, fd2, e->events);
    abort();
}

int tuntap_has_fake_pkt();

int impl_epoll_wait(int fd, struct linux_epoll_event* ee, int maxevents, int timeout)
{
    if(fd != 65536 || !have_epoll)
    {
        errno = EBADF;
        return -1;
    }
    //printf("epoll_wait(%d, %d)...\n", maxevents, timeout);
    struct timeval ts = {.tv_sec = timeout / 1000, .tv_usec = (timeout % 1000) * 1000};
    fd_set r = epoll_in;
    fd_set w = epoll_out;
    fd_set x = epoll_err;
    int need_fake = 0;
    if(tun_fd >= 0 && FD_ISSET(tun_fd, &r) && tuntap_has_fake_pkt())
    {
        ts.tv_sec = ts.tv_usec = 0;
        need_fake = 1;
    }
    int nfds = select(FD_SETSIZE, &r, &w, &x, &ts);
    //printf("nfds = %d, errno = %d\n", nfds, errno);
    if(nfds < 0)
        return -1;
    int idx = 0;
    for(int i = 0; i < FD_SETSIZE && idx < maxevents; i++)
    {
        if(idx < maxevents && FD_ISSET(i, &r))
        {
            dbg_printf("epoll_wait: %d is readable\n", i);
            ee[idx++] = (struct linux_epoll_event){LINUX_EPOLLIN, epoll_per_fd_data[i]};
        }
        else if(idx < maxevents && i == tun_fd && need_fake)
        {
            dbg_printf("epoll_wait: %d has fake data\n", i);
            ee[idx++] = (struct linux_epoll_event){LINUX_EPOLLIN, epoll_per_fd_data[i]};
        }
        if(idx < maxevents && FD_ISSET(i, &w))
        {
            dbg_printf("epoll_wait: %d is writable\n", i);
            ee[idx++] = (struct linux_epoll_event){LINUX_EPOLLOUT, epoll_per_fd_data[i]};
        }
        if(idx < maxevents && FD_ISSET(i, &x))
        {
            dbg_printf("epoll_wait: %d has errors\n", i);
            ee[idx++] = (struct linux_epoll_event){LINUX_EPOLLERR, epoll_per_fd_data[i]};
        }
    }
    //printf("epoll_wait(%d, %d) = %d\n", maxevents, timeout, idx);
    return idx;
}

int impl_socket(int domain, int type, int protocol)
{
    if(domain == 10)
        domain = AF_INET6;
    return socket(domain, type, protocol);
}

union bsd_addr
{
    struct sockaddr addr;
    struct sockaddr_in addr_in;
    struct sockaddr_in6 addr_in6;
    struct sockaddr_un addr_un;
};

union linux_addr
{
    struct linux_sockaddr addr;
    struct linux_sockaddr_in addr_in;
    struct linux_sockaddr_in6 addr_in6;
    struct linux_sockaddr_un addr_un;
};

void linux_addr_to_bsd_addr(const struct linux_sockaddr* addr, union bsd_addr* bsd_addr, socklen_t* bsd_len, const char* fn)
{
    if(addr->sa_family == AF_INET)
    {
        struct linux_sockaddr_in* addr_in = (void*)addr;
        bsd_addr->addr_in.sin_family = AF_INET;
        bsd_addr->addr_in.sin_addr.s_addr = addr_in->sin_addr.s_addr;
        bsd_addr->addr_in.sin_port = addr_in->sin_port;
        *bsd_len = sizeof(bsd_addr->addr_in);
    }
    else if(addr->sa_family == 10)
    {
        struct linux_sockaddr_in6* addr_in6 = (void*)addr;
        bsd_addr->addr_in6.sin6_family = AF_INET6;
        bsd_addr->addr_in6.sin6_port = addr_in6->sin6_port;
        memcpy(&bsd_addr->addr_in6.sin6_addr, &addr_in6->sin6_addr, 16);
        *bsd_len = sizeof(bsd_addr->addr_in6);
    }
    else if(addr->sa_family == AF_UNIX)
    {
        struct linux_sockaddr_un* addr_un = (void*)addr;
        bsd_addr->addr_un.sun_family = AF_UNIX;
        dbg_printf("%s(AF_UNIX, \"%s\")\n", fn, addr_un->sun_path);
        char* pp = build_fake_path(addr_un->sun_path);
        strncpy(bsd_addr->addr_un.sun_path, pp, sizeof(bsd_addr->addr_un.sun_path));
        bsd_addr->addr_un.sun_path[sizeof(bsd_addr->addr_un.sun_path)-1] = 0;
        *bsd_len = sizeof(bsd_addr->addr_un);
        free(pp);
    }
    else
        *bsd_len = 0;
}

void bsd_addr_to_linux_addr(const struct sockaddr* addr, union linux_addr* linux_addr, socklen_t* linux_len, const char* fn)
{
    if(addr->sa_family == AF_INET)
    {
        struct sockaddr_in* addr_in = (void*)addr;
        linux_addr->addr_in.sin_family = AF_INET;
        linux_addr->addr_in.sin_addr.s_addr = addr_in->sin_addr.s_addr;
        linux_addr->addr_in.sin_port = addr_in->sin_port;
        *linux_len = sizeof(linux_addr->addr_in);
    }
    else if(addr->sa_family == AF_INET6)
    {
        struct sockaddr_in6* addr_in6 = (void*)addr;
        linux_addr->addr_in6.sin6_family = 10;
        linux_addr->addr_in6.sin6_port = addr_in6->sin6_port;
        memcpy(&linux_addr->addr_in6.sin6_addr, &addr_in6->sin6_addr, 16);
        *linux_len = sizeof(linux_addr->addr_in6);
    }
    else if(addr->sa_family == AF_UNIX)
    {
        struct sockaddr_un* addr_un = (void*)addr;
        linux_addr->addr_un.sun_family = AF_UNIX;
        linux_addr->addr_un.sun_path[0] = 0;
        *linux_len = sizeof(linux_addr->addr_un);
    }
    else
        *linux_len = 0;
}

int impl_bind(int sockfd, const struct linux_sockaddr* addr, socklen_t l)
{
    union bsd_addr bsd_addr;
    socklen_t bsd_len;
    linux_addr_to_bsd_addr(addr, &bsd_addr, &bsd_len, "bind");
    if(!bsd_len)
    {
        errno = ENOSYS;
        return -1;
    }
    return bind(sockfd, &bsd_addr.addr, bsd_len);
}

int impl_connect(int sockfd, const struct linux_sockaddr* addr, socklen_t l)
{
    union bsd_addr bsd_addr;
    socklen_t bsd_len;
    linux_addr_to_bsd_addr(addr, &bsd_addr, &bsd_len, "connect");
    if(!bsd_len)
    {
        errno = ENOSYS;
        return -1;
    }
    if(addr->sa_family == AF_INET)
    {
        dbg_printf("connect(AF_INET, %d, %hd)\n", ((const struct linux_sockaddr_in*)addr)->sin_addr.s_addr, ((const struct linux_sockaddr_in*)addr)->sin_port);
        if(((const struct linux_sockaddr_in*)addr)->sin_addr.s_addr == htonl(0xa9fea9fe))
        {
            errno = EHOSTUNREACH;
            return -1;
        }
    }
    int ans = connect(sockfd, &bsd_addr.addr, bsd_len);
    if(errno == EINPROGRESS)
        errno = 115; // EINPROGRESS
    return ans;
}

int impl_chmod(const char* path, mode_t mode)
{
    dbg_printf("chmod(\"%s\", 0%o)\n", path, mode);
    return 0;
}

void tuntap_set_hwaddr(char*);
void tuntap_set_ipaddr(in_addr_t);
void tuntap_set_brdaddr(in_addr_t);
ssize_t tuntap_read(int, char*, size_t);
ssize_t tuntap_write(int, const char*, size_t);

int impl_ioctl(int fd, int cmd, void* data)
{
    dbg_printf("ioctl(%d, %d, ...)\n", fd, cmd);
    struct linux_ifreq* r = data;
    if(cmd == 0x400454ca) // TUNSETIFF
    {
        if(tun_fd != -1)
        {
            dbg_printf("FATAL: tried to create 2 tun interfaces!\n");
            abort();
        }
        tun_fd = fd;
        dbg_printf("TUNSETIFF!\n");
        if(r->ifr_ifru.ifru_flags != (LINUX_IFF_TAP | LINUX_IFF_NO_PI))
        {
            dbg_printf("unknown flags value: %hd\n", r->ifr_ifru.ifru_flags);
            errno = ENOTTY;
            return -1;
        }
        dbg_printf("requested interface name: %s\n", r->ifr_ifrn.ifrn_name);
        return 0;
    }
    else if(cmd == 35108) // SIOCSIFHWADDR
    {
        dbg_printf("SIOCSIFHWADDR!\n");
        tuntap_set_hwaddr(r->ifr_ifru.ifru_hwaddr.sa_data);
        return 0;
    }
    else if(cmd == 35106) // SIOCSIFMTU
    {
        dbg_printf("SIOCSIFMTU! (stub)\n");
        dbg_printf("mtu = %d\n", r->ifr_ifru.ifru_mtu);
        return 0;
    }
    else if(cmd == 35091) // SIOCGIFLAGS
    {
        dbg_printf("SIOCGIFLAGS! (stub)\n");
        return 0;
    }
    else if(cmd == 35092) // SIOCSIFLAGS
    {
        dbg_printf("SIOCSIFLAGS! (stub)\n");
        return 0;
    }
    else if(cmd == 35094 /*SIOCSIFADDR*/ || cmd == 35100 /*SIOCSIFNETMASK*/ || cmd == 35098 /*SIOCSIFBRDADDR*/)
    {
        int tun_idx = 0;
        ioctl(tun_fd, 0x4004beef, &tun_idx);
        struct ifreq ifr;
        sprintf(ifr.ifr_name, "tun%d", tun_idx);
        //ifr.ifr_addr = r->ifr_ifru.ifru_addr;
        socklen_t l;
        if(cmd == 35094)
        {
            dbg_printf("SIOCSIFADDR!\n");
            linux_addr_to_bsd_addr(&r->ifr_ifru.ifru_addr, (union bsd_addr*)&ifr.ifr_addr, &l, "ioctl");
            cmd = SIOCSIFADDR;
            tuntap_set_ipaddr(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr.s_addr);
            ifr.ifr_addr.sa_len = l;
        }
        else if(cmd == 35100)
        {
            dbg_printf("SIOCSIFNETMASK!\n");
            linux_addr_to_bsd_addr(&r->ifr_ifru.ifru_netmask, (union bsd_addr*)&ifr.ifr_addr, &l, "ioctl");
            cmd = SIOCSIFNETMASK;
            ifr.ifr_addr.sa_len = l;
        }
        else
        {
            dbg_printf("SIOCSIFBRDADDR!\n");
            linux_addr_to_bsd_addr(&r->ifr_ifru.ifru_broadaddr, (union bsd_addr*)&ifr.ifr_broadaddr, &l, "ioctl");
            cmd = SIOCSIFBRDADDR;
            ifr.ifr_broadaddr.sa_len = l;
            tuntap_set_brdaddr(((struct sockaddr_in*)&ifr.ifr_broadaddr)->sin_addr.s_addr);
        }
        int ans = ioctl(fd, (unsigned)cmd, &ifr);
        dbg_printf("ans = %d, errno = %d\n", ans, errno);
        return ans;
    }
    abort();
}

int impl_accept(int sock, struct linux_sockaddr* addr, socklen_t* l)
{
    if(!addr || !l)
    {
        int ans = accept(sock, 0, 0);
        dbg_printf("accept(%d, NULL) = %d\n", sock, ans);
        return ans;
    }
    union bsd_addr a_b;
    union linux_addr a_l;
    socklen_t ll = sizeof(a_b);
    int ans = accept(sock, &a_b.addr, &ll);
    bsd_addr_to_linux_addr(&a_b.addr, &a_l, &ll, "accept");
    if(ll > *l)
        ll = *l;
    memcpy(addr, &a_l, ll);
    dbg_printf("accept(%d, ...) = %d\n", sock, ans);
    return ans;
}

ssize_t impl_send(int fd, void* data, size_t sz, int flags)
{
    dbg_printf("send(%d, %p, %llu, %d)...\n", fd, data, sz, flags);
    ssize_t ans = send(fd, data, sz, flags);
    dbg_printf("send(%d, %p, %llu, %d) = %lld\n", fd, data, sz, flags, ans);
    if(errno == EAGAIN)
        errno = 11; // EAGAIN
    return ans;
}

ssize_t impl_recv(int fd, void* data, size_t sz, int flags)
{
    dbg_printf("recv(%d, %p, %llu, %d)...\n", fd, data, sz, flags);
    ssize_t ans = recv(fd, data, sz, flags);
    dbg_printf("recv(%d, %p, %llu, %d) = %lld\n", fd, data, sz, flags, ans);
    if(errno == EAGAIN)
        errno = 11; // EAGAIN
    return ans;
}

int impl_fcntl(int fd, int cmd, int arg)
{
    dbg_printf("fcntl(%d, %d, %d)\n", fd, cmd, arg);
    if(cmd == F_GETFL)
    {
        int q = fcntl(fd, F_GETFL);
        int ans = (q & 3);
        if(q & O_NONBLOCK)
            ans |= 2048; // O_NONBLOCK
        return ans;
    }
    else if(cmd == F_SETFL)
    {
        int q = (arg & 3);
        if(arg & 2048) // O_NONBLOCK
            q |= O_NONBLOCK;
        return fcntl(fd, F_SETFL, q);
    }
    else
    {
        dbg_printf("fcntl(fd, %d) = ?\n", cmd);
        abort();
    }
}

struct laax
{
    struct linux_addrinfo ai;
    union linux_addr sa;
};

struct host_addrinfo {
	int ai_flags;
	int ai_family;
	int ai_socktype;
	int ai_protocol;
	socklen_t ai_addrlen;
	struct sockaddr *ai_addr;
	char *ai_canonname;
	struct host_addrinfo *ai_next;
};

void bsd_to_linux_addrinfo(struct host_addrinfo* arg, struct linux_addrinfo** ans)
{
    while(arg)
    {
        struct laax* i = malloc(sizeof(*i));
        socklen_t l;
        i->ai.ai_flags = arg->ai_flags;
        i->ai.ai_family = (arg->ai_family==AF_INET6?10:arg->ai_family);
        i->ai.ai_socktype = arg->ai_socktype;
        i->ai.ai_protocol = arg->ai_protocol;
        bsd_addr_to_linux_addr(arg->ai_addr, &i->sa, &i->ai.ai_addrlen, "getaddrinfo");
        i->ai.ai_addr = &i->sa.addr;
        i->ai.ai_canonname = strdup(arg->ai_canonname);
        *ans = (struct linux_addrinfo*)i;
        ans = &i->ai.ai_next;
        arg = arg->ai_next;
    }
    *ans = 0;
}

int impl_getaddrinfo(const char* node, const char* service, const struct linux_addrinfo* hints, struct linux_addrinfo** res)
{
    dbg_printf("getaddrinfo(\"%s\", \"%s\", %p, %p)\n", node, service, hints, res);
    struct host_addrinfo bsd_hints;
    bsd_hints.ai_flags = hints->ai_flags;
    bsd_hints.ai_family = (hints->ai_family==10?AF_INET6:hints->ai_family);
    bsd_hints.ai_socktype = hints->ai_socktype;
    bsd_hints.ai_protocol = hints->ai_protocol;
    struct host_addrinfo* bsd_res = 0;
    int ans = getaddrinfo(node, service, (struct addrinfo*)&bsd_hints, (struct addrinfo**)&bsd_res);
    dbg_printf("res=%d errno=%d\n", ans, errno);
    if(bsd_res)
    {
        bsd_to_linux_addrinfo(bsd_res, res);
        freeaddrinfo((struct addrinfo*)bsd_res);
    }
    else
        *res = 0;
    return ans;
}

void impl_freeaddrinfo(struct linux_addrinfo* ai)
{
    while(ai)
    {
        struct linux_addrinfo* i = ai->ai_next;
        free(ai->ai_canonname);
        free(ai);
        ai = i;
    }
}

int impl_getsockname(int sock, struct linux_sockaddr* addr, socklen_t* addrlen)
{
    union bsd_addr b_addr;
    socklen_t sz = sizeof(b_addr);
    int ans = getsockname(sock, &b_addr.addr, &sz);
    union linux_addr l_addr;
    bsd_addr_to_linux_addr(&b_addr.addr, &l_addr, &sz, "getsockname");
    if(sz > *addrlen)
        sz = *addrlen;
    *addrlen = sz;
    memcpy(addr, &l_addr, sz);
    return ans;
}

/*void translate_sockopt(int* l, int* o)
{
    int level = *l;
    int optname = *o;
    if(level == 1)
    {
        level = SOL_SOCKET;
        if(optname == 8)
            optname = SO_RCVBUF;
        else if(optname == 7)
            optname = SO_SNDBUF;
        else
            abort();
    }
    else if(level != 0) // SOL_IP ??
        abort();
    *l = level;
    *o = optname;
}*/

int impl_setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen)
{
    dbg_printf("setsockopt(%d, %d, %d, %p, %d)\n", sockfd, level, optname, optval, optlen);
    return 0;
}

int impl_getsockopt(int sockfd, int level, int optname, void* optval, socklen_t* optlen)
{
    dbg_printf("getsockopt(%d, %d, %d, %p, %d)\n", sockfd, level, optname, optval, optlen);
    return 0;
}

int impl_sendto(int sockfd, const void* buf, size_t len, int flags, const struct linux_sockaddr* sa, socklen_t l)
{
    union bsd_addr a;
    linux_addr_to_bsd_addr(sa, &a, &l, "sendto");
    int ans = sendto(sockfd, buf, len, flags, &a.addr, l);
    if(errno == EAGAIN)
        errno = 11;
    return ans;
}

int impl_recvfrom(int sockfd, void* buf, size_t len, int flags, struct linux_sockaddr* sa, socklen_t* l)
{
    union bsd_addr a;
    socklen_t ll = sizeof(a);
    int ans = recvfrom(sockfd, buf, len, flags, &a.addr, &ll);
    union linux_addr aa;
    bsd_addr_to_linux_addr(&a.addr, &aa, &ll, "recvfrom");
    if(ll > *l)
        ll = *l;
    *l = ll;
    memcpy(sa, &aa.addr, ll);
    if(errno == EAGAIN)
        errno = 11;
    return ans;
}

int impl_sigemptyset(void* p)
{
    dbg_printf("sigemptyset(%p)\n", p);
    return 0;
}

int impl_sigaction(int signum, const void* act, void* oldact)
{
    dbg_printf("sigaction(%d, %p, %p\n", signum, act, oldact);
    return 0;
}

ssize_t impl_read(int fd, void* buf, size_t cnt)
{
    if(fd == tun_fd)
        return tuntap_read(fd, (char*)buf, cnt);
    return read(fd, buf, cnt);
}

ssize_t impl_write(int fd, const void* buf, size_t cnt)
{
    if(fd == tun_fd)
        return tuntap_write(fd, (const char*)buf, cnt);
    return write(fd, buf, cnt);
}

int impl_pthread_mutex_init(void)
{
    return 0;
}

int impl_gethostname(char* tgt, int l)
{
    if(l < 10)
    {
        errno = 36;
        return -1;
    }
    strcpy(tgt, "localhost");
    return 0;
}

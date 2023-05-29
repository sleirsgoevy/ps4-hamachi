#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/file.h>
#include <sys/time.h>
#include <poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <netinet/in.h>
#include <resolv.h>
#include <arpa/nameser.h>

//undo fbsd defines
#undef stdout
extern FILE* stdout;
#undef res_search
typeof(__res_search) res_search;
#undef inet_addr
typeof(__inet_addr) inet_addr;
#undef ns_initparse
typeof(__ns_initparse) ns_initparse;
#undef ns_parserr
typeof(__ns_parserr) ns_parserr;

void* generate_stub(const char* name);

struct symbol_desc
{
    const char* name;
    void* value;
    int is_fn;
    void* reloc_addr;
};

extern char impl___cxa_atexit[];
extern char impl___libc_start_main[];
extern char impl_getenv[];
extern char impl_malloc[];
extern char impl_realloc[];
extern char impl_free[];
extern char impl_getopt[];
extern char impl___printf_chk[];
extern char impl___vprintf_chk[];
extern char impl___fprintf_chk[];
extern char impl___vfprintf_chk[];
extern char impl___vsnprintf_chk[];
extern char impl___snprintf_chk[];
extern char impl___sprintf_chk[];
extern char impl_exit[];
extern char impl_getuid[];
extern char impl___xstat64[];
extern char impl_mkdir[];
extern char impl_open64[];
extern char impl___errno_location[];
extern char impl_uname[];
extern char impl_signal[];
extern char impl_daemon[];
extern char impl_fopen64[];
extern char impl_localtime[];
extern char impl_system[];
extern char impl_unlink[];
extern char impl___fxstat64[];
extern char impl___memcpy_chk[];
extern char impl_close[];
extern char impl_epoll_create[];
extern char impl_epoll_ctl[];
extern char impl_epoll_wait[];
extern char impl_socket[];
extern char impl_bind[];
extern char impl_connect[];
extern char impl_accept[];
extern char impl___strcpy_chk[];
extern char impl_chmod[];
extern char impl_fcntl[];
extern char impl_ioctl[];
extern char impl_send[];
extern char impl_recv[];
extern char impl_getaddrinfo[];
extern char impl_freeaddrinfo[];
extern char impl_getsockname[];
extern char impl_setsockopt[];
extern char impl_getsockopt[];
extern char impl_sendto[];
extern char impl_recvfrom[];
extern char impl_sigemptyset[];
extern char impl_sigaction[];
extern char impl_read[];
extern char impl_write[];
extern char impl_pthread_mutex_init[];
extern char impl_gethostname[];
extern char impl_fclose[];
extern char impl_fflush[];
extern char impl_fileno[];
extern char impl_fread[];
extern char impl_fwrite[];
extern char impl_opendir[];
extern char impl_strdup[];
extern char __ctype_b_loc[];
extern char __ctype_tolower_loc[];
extern char impl_getpeername[];
void empty(){}
unsigned long long zero = 0;

FILE** p_stdout = &stdout;

//NOTE: this list must be sorted in ASCII order!
struct symbol_desc symbols[] = {
    {"__ctype_b_loc", __ctype_b_loc, 1},
    {"__ctype_tolower_loc", __ctype_tolower_loc, 1},
    {"__cxa_atexit", impl___cxa_atexit, 1},
    {"__errno_location", impl___errno_location, 1},
    {"__fprintf_chk", impl___fprintf_chk, 1},
    {"__fxstat", impl___fxstat64, 1}, // same on x86_64
    {"__fxstat64", impl___fxstat64, 1},
    {"__libc_start_main", impl___libc_start_main, 1},
    {"__memcpy_chk", impl___memcpy_chk, 1},
    {"__printf_chk", impl___printf_chk, 1},
    {"__res_search", res_search, 1},
    {"__snprintf_chk", impl___snprintf_chk, 1},
    {"__sprintf_chk", impl___sprintf_chk, 1},
    {"__strcpy_chk", impl___strcpy_chk, 1},
    {"__vfprintf_chk", impl___vfprintf_chk, 1},
    {"__vprintf_chk", impl___vprintf_chk, 1},
    {"__vsnprintf_chk", impl___vsnprintf_chk, 1},
    {"__xstat64", impl___xstat64, 1},
    {"accept", impl_accept, 1},
    {"bind", impl_bind, 1},
    {"chmod", impl_chmod, 1},
    {"close", impl_close, 1},
    {"connect", impl_connect, 1},
    {"daemon", impl_daemon, 1},
    {"epoll_create", impl_epoll_create, 1},
    {"epoll_ctl", impl_epoll_ctl, 1},
    {"epoll_wait", impl_epoll_wait, 1},
    {"exit", impl_exit, 1},
    {"fclose", impl_fclose, 1},
    {"fcntl", impl_fcntl, 1},
    {"fflush", impl_fflush, 1},
    {"fileno", impl_fileno, 1},
    {"flock", flock, 1},
    {"fopen64", impl_fopen64, 1},
    {"fread", impl_fread, 1},
    {"free", impl_free, 1},
    {"freeaddrinfo", impl_freeaddrinfo, 1},
    {"fsync", fsync, 1},
    {"fwrite", impl_fwrite, 1},
    {"getaddrinfo", impl_getaddrinfo, 1},
    {"getenv", impl_getenv, 1},
    {"gethostname", impl_gethostname, 1},
    {"getopt", impl_getopt, 1},
    {"getpid", getpid, 1},
    {"getpeername", impl_getpeername, 1},
    {"getsockname", impl_getsockname, 1},
    {"getsockopt", impl_getsockopt, 1},
    {"gettimeofday", gettimeofday, 1},
    {"getuid", impl_getuid, 1},
    {"htonl", htonl, 1},
    {"htons", htons, 1},
    {"inet_addr", inet_addr, 1},
    {"inet_ntop", inet_ntop, 1},
    {"ioctl", impl_ioctl, 1},
    {"isspace", isspace, 1},
    {"listen", listen, 1},
    {"localtime", impl_localtime, 1},
    {"malloc", impl_malloc, 1},
    {"memchr", memchr, 1},
    {"memcmp", memcmp, 1},
    {"memcpy", memcpy, 1},
    {"memmove", memmove, 1},
    {"memset", memset, 1},
    {"mkdir", impl_mkdir, 1},
    {"ns_initparse", ns_initparse, 1},
    {"ns_msg_getflag", ns_msg_getflag, 1},
    {"ns_parserr", ns_parserr, 1},
    {"open", impl_open64, 1},
    {"open64", impl_open64, 1},
    {"opendir", impl_opendir, 1},
    {"optarg", &zero, 0},
    {"opterr", &zero, 0},
    {"optind", &zero, 0},
    {"optopt", &zero, 0},
    {"pipe", pipe, 1},
    {"poll", poll, 1},
    {"pthread_create", pthread_create, 1},
    {"pthread_exit", pthread_exit, 1},
    {"pthread_mutex_init", impl_pthread_mutex_init, 1},
    {"pthread_mutex_lock", pthread_mutex_lock, 1},
    {"puts", puts, 1},
    {"qsort", qsort, 1},
    {"rand", rand, 1},
    {"read", impl_read, 1},
    {"realloc", impl_realloc, 1},
    {"recv", impl_recv, 1},
    {"recvfrom", impl_recvfrom, 1},
    {"send", impl_send, 1},
    {"sendto", impl_sendto, 1},
    {"setsockopt", impl_setsockopt, 1},
    {"sigaction", impl_sigaction, 1},
    {"sigemptyset", impl_sigemptyset, 1},
    {"signal", impl_signal, 1},
    {"snprintf", snprintf, 1},
    {"socket", impl_socket, 1},
    {"sscanf", sscanf, 1},
    {"stdout", &p_stdout, 0},
    {"strchr", strchr, 1},
    {"strcmp", strcmp, 1},
    {"strcpy", strcpy, 1},
    {"strdup", impl_strdup, 1},
    {"strerror", strerror, 1},
    {"strlen", strlen, 1},
    {"strncasecmp", strncasecmp, 1},
    {"strncmp", strncmp, 1},
    {"strncpy", strncpy, 1},
    {"strrchr", strrchr, 1},
    {"strstr", strstr, 1},
    {"strtol", strtol, 1},
    {"system", impl_system, 1},
    {"time", time, 1},
    {"tolower", tolower, 1},
    {"uname", impl_uname, 1},
    {"unlink", impl_unlink, 1},
    {"usleep", usleep, 1},
    {"write", impl_write, 1},
};

struct symbol_desc* auxsymbols = 0;
int nauxsymbols = 0;

void setauxsymbols(struct symbol_desc* syms)
{
    auxsymbols = syms;
    while(syms[nauxsymbols].name)
        nauxsymbols++;
}

struct symbol_desc* lookup_in(const char* name, struct symbol_desc* symbols, int nsymbols)
{
    int start = 0;
    int end = nsymbols;
    int idx = 0;
    for(;;)
    {
        int l = start - 1;
        int h = end;
        while(h - l > 1)
        {
            int m = (l + h) / 2;
            if(symbols[m].name[idx] >= name[idx])
                h = m;
            else
                l = m;
        }
        if(h == end || symbols[h].name[idx] > name[idx])
            return 0;
        start = h;
        if(!name[idx])
            return symbols + start;
        l = start - 1;
        h = end;
        while(h - l > 1)
        {
            int m = (l + h) / 2;
            if(symbols[m].name[idx] > name[idx])
                h = m;
            else
                l = m;
        }
        end = h;
        idx++;
    }
}

struct symbol_desc* lookup(const char* name)
{
    struct symbol_desc* sym = lookup_in(name, symbols, sizeof(symbols) / sizeof(symbols[0]));
    if(sym)
        return sym;
    return lookup_in(name, auxsymbols, nauxsymbols);
}

void* lookup_function(const char* name)
{
    struct symbol_desc* x = lookup(name);
    if(x && x->is_fn)
        return x->value;
    return generate_stub(name);
}

void* lookup_data(const char* name, void* tgt)
{
    struct symbol_desc* x = lookup(name);
    if(x && !x->is_fn)
    {
        if(tgt > (void*)1)
        {
            void* ans = *(void**)x->value;
            x->reloc_addr = tgt;
            return ans;
        }
        else if(!tgt && x->reloc_addr)
            return x->reloc_addr;
        else
            return x->value;
    }
    static long long counter = 1;
    long long ans = counter++ << 32 | 0xdeadbeef;
    return (void*)ans;
}

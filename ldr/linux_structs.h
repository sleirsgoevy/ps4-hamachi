typedef unsigned long int linux_dev_t;
typedef unsigned int linux_uid_t;
typedef unsigned int linux_gid_t;
typedef unsigned long int linux_ino_t;
typedef unsigned int linux_mode_t;
typedef unsigned long int linux_nlink_t;
typedef long int linux_off_t;
typedef long int linux_time_t;
typedef long int linux_blksize_t;
typedef long int linux_blkcnt_t;
struct linux_timespec
{
    linux_time_t tv_sec;
    long tv_nsec;
};
struct linux_stat64
{
    linux_dev_t st_dev;
    linux_ino_t st_ino;
    linux_nlink_t st_nlink;
    linux_mode_t st_mode;
    linux_uid_t st_uid;
    linux_gid_t st_gid;
    int __pad0;
    linux_dev_t st_rdev;
    linux_off_t st_size;
    linux_blksize_t st_blksize;
    linux_blkcnt_t st_blocks;
    struct linux_timespec st_atim;
    struct linux_timespec st_mtim;
    struct linux_timespec st_ctim;
    long __glibc_reserved[3];
};
struct linux_utsname
{
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
    char domainname[65];
    char __domainname[65];
};
struct linux_tm
{
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
  long int tm_gmtoff;
  const char *tm_zone;
};
typedef unsigned short int linux_sa_family_t;
typedef uint32_t linux_in_addr_t;
typedef uint16_t linux_in_port_t;
struct linux_in_addr
{
    linux_in_addr_t s_addr;
};
struct linux_sockaddr
{
    linux_sa_family_t sa_family;
    char sa_data[14];
};
struct linux_sockaddr_in
{
    linux_sa_family_t sin_family;
    linux_in_port_t sin_port;
    struct linux_in_addr sin_addr;
    unsigned char sin_zero[sizeof (struct linux_sockaddr) -
      (sizeof (unsigned short int)) -
      sizeof (linux_in_port_t) -
      sizeof (struct linux_in_addr)];
};
struct linux_in6_addr
{
    union
    {
        uint8_t __u6_addr8[16];
        uint16_t __u6_addr16[8];
        uint32_t __u6_addr32[4];
    } __in6_u;
};
struct linux_sockaddr_in6
{
    linux_sa_family_t sin6_family;
    linux_in_port_t sin6_port;
    uint32_t sin6_flowinfo;
    struct linux_in6_addr sin6_addr;
    uint32_t sin6_scope_id;
};
struct linux_sockaddr_un
{
    linux_sa_family_t sun_family;
    char sun_path[108];
};
typedef union linux_epoll_data
{
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;
} linux_epoll_data_t;
struct linux_epoll_event
{
  uint32_t events;
  linux_epoll_data_t data;
} __attribute__ ((__packed__));
enum LINUX_EPOLL_EVENTS
{
    LINUX_EPOLLIN = 0x001,
    LINUX_EPOLLPRI = 0x002,
    LINUX_EPOLLOUT = 0x004,
    LINUX_EPOLLRDNORM = 0x040,
    LINUX_EPOLLRDBAND = 0x080,
    LINUX_EPOLLWRNORM = 0x100,
    LINUX_EPOLLWRBAND = 0x200,
    LINUX_EPOLLMSG = 0x400,
    LINUX_EPOLLERR = 0x008,
    LINUX_EPOLLHUP = 0x010,
    LINUX_EPOLLRDHUP = 0x2000,
    LINUX_EPOLLEXCLUSIVE = 1u << 28,
    LINUX_EPOLLWAKEUP = 1u << 29,
    LINUX_EPOLLONESHOT = 1u << 30,
    LINUX_EPOLLET = 1u << 31
};
#define LINUX_EPOLL_CTL_ADD 1
#define LINUX_EPOLL_CTL_DEL 2
#define LINUX_EPOLL_CTL_MOD 3
#define	LINUX_IFNAMSIZ	16
struct linux_ifreq {
    union
    {
    	char	ifrn_name[LINUX_IFNAMSIZ];		/* if name, e.g. "en0" */
    } ifr_ifrn;
    union {
    	struct	linux_sockaddr ifru_addr;
    	struct	linux_sockaddr ifru_dstaddr;
    	struct	linux_sockaddr ifru_broadaddr;
    	struct	linux_sockaddr ifru_netmask;
    	struct  linux_sockaddr ifru_hwaddr;
    	short	ifru_flags;
    	int	ifru_ivalue;
    	int	ifru_mtu;
    	//struct  ifmap ifru_map;
    	char	ifru_slave[LINUX_IFNAMSIZ];	/* Just fits the size */
    	char	ifru_newname[LINUX_IFNAMSIZ];
    	void *	ifru_data;
    	//struct	if_settings ifru_settings;
    } ifr_ifru;
};
#define LINUX_IFF_TUN		0x0001
#define LINUX_IFF_TAP		0x0002
#define LINUX_IFF_NO_PI		0x1000
struct linux_addrinfo
{
  int ai_flags;
  int ai_family;
  int ai_socktype;
  int ai_protocol;
  socklen_t ai_addrlen;
  struct linux_sockaddr *ai_addr;
  char *ai_canonname;
  struct linux_addrinfo *ai_next;
};

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void load_tun_tsr(void);

int maybe_load_tun(void)
{
    int fd = open("/dev/tun", 0);
    if(fd < 0)
    {
        if(errno == ENOENT)
        {
            load_tun_tsr();
            return 0;
        }
        return -1;
    }
    close(fd);
    return 0;
}

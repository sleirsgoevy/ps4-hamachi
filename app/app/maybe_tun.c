#include "libjbc/utils.h"
#include <errno.h>

void load_tun_tsr(int);
int open(const char*, int);
int close(int);

uint32_t get_fw_version(void);

static void do_maybe_load_tun(void* p_status)
{
    int* status = p_status;
    *status = 0;
    int fd = open("/dev/tun", 0);
    if(fd < 0)
    {
        if(errno == ENOENT)
        {
            load_tun_tsr(get_fw_version() >> 16);
            *status = 0;
        }
        *status = -1;
        return;
    }
    close(fd);
    return;
}

int maybe_load_tun(void)
{
    int status;
    jbc_run_as_root(do_maybe_load_tun, &status, CWD_RESET);
    return status;
}

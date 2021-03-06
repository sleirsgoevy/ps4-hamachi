#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <orbis/libkernel.h>

void load_tun_tsr(int);
void load_uhack_tsr(int);

int maybe_load_tun(void)
{
    int fd = open("/dev/tun", 0);
    if(fd < 0)
    {
        if(errno == ENOENT)
        {
            OrbisKernelSwVersion sw_ver;
            sceKernelGetSystemSwVersion(&sw_ver);
            load_tun_tsr(sw_ver.i_version >> 16);
            return 0;
        }
        return -1;
    }
    close(fd);
    return 0;
}

asm("unmount:\nmov $22, %rax\nmov %rcx, %r10\nsyscall\nret");
int unmount(const char*, int);

int maybe_load_uhack(void)
{
    int q = unmount(0x179, 0);
    if(q)
    {
        if(q == EFAULT)
        {
            OrbisKernelSwVersion sw_ver;
            sceKernelGetSystemSwVersion(&sw_ver);
            load_uhack_tsr(sw_ver.i_version >> 16);
            return 0;
        }
        return -1;
    }
    return 0;
}

#include <sys/syscall.h>
#include <sys/sysent.h>
#include <errno.h>
#include <stdio.h>

struct sysent* get_sysent(void);

int (*p_old_unmount)(struct thread*, void*);

int new_unmount(struct thread* td, void* uap)
{
    unsigned long long* args = (unsigned long long*)uap;
    if(args[0] == 0x179)
        return 0; // magic check to avoid double hook
    int q;
    while((q = p_old_unmount(td, uap)) == EBUSY)
        printf("[uhack] sys_unmount() returned EBUSY, retrying\n");
    return q;
}

int main(struct thread* td)
{
    printf("[uhack] Hello, kernel world!\n");
    p_old_unmount = get_sysent()[SYS_unmount].sy_call;
    get_sysent()[SYS_unmount].sy_call = new_unmount;
    printf("[uhack] Goodbye, kernel world!\n");
    return 0;
}

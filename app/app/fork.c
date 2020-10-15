#include <signal.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <orbis/libkernel.h>
#include <pthread.h>

asm("sys_fork:\nmov $2, %rax\nmov %rcx, %r10\nsyscall\nret");
pid_t sys_fork(void);

void* watchdog_thread(void* o)
{
    int fd = (int)(intptr_t)o;
    char c;
    read(fd, &c, 1);
    raise(SIGKILL);
    return 0;
}

pid_t my_fork(int keep1, int keep2, int keep3, int* master_fd)
{
    int magicpipe[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, magicpipe);
    pid_t p = sys_fork();
    if(p)
    {
        close(magicpipe[0]);
        *master_fd = magicpipe[1];
        return p;
    }
    else
    {
        close(magicpipe[1]);
        pthread_t thr;
        pthread_create(&thr, 0, &watchdog_thread, (void*)(intptr_t)magicpipe[0]);
        for(int i = 3; i < 10000; i++)
            if(i != keep1 && i != keep2 && i != keep3 && i != magicpipe[0])
                close(i);
        return 0;
    }
}

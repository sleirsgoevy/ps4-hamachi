#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <signal.h>

int jailbreak(void);
int my_fork(void);
void my_exit(int);
int ldr_main(int argc, const char** argv);

void vexecute(int argc, va_list va)
{
    const char* argv[argc + 4];
    argv[0] = "./ldr";
    argv[1] = "/mnt/sandbox/BREW00179_000/app0/hamachid";
    argv[2] = "/mnt/sandbox/BREW00179_000/app0/cpp.elf";
    for(int i = 0; i < argc; i++)
        argv[i + 3] = va_arg(va, const char*);
    argv[argc + 3] = 0;
    my_exit(ldr_main(argc + 3, argv));
}

void execute(int argc, ...)
{
    va_list va;
    va_start(va, argc);
    vexecute(argc, va);
    va_end(va);
}

void daemon_thread(void)
{
    int pid_file = open("/mnt/daemons/HOME00179/pid", O_WRONLY|O_CREAT, 0777);
    if(pid_file >= 0)
    {
        pid_t p = getpid();
        write(pid_file, &p, sizeof(p));
        close(pid_file);
    }
    int master_fd;
    for(;;)
    {
        unlink("/user/home/hamachi/var/run/logmein-hamachi/hamachid.lock");
        int p = my_fork();
        if(!p)
            execute(1, "hamachid");
    }
}

extern int is_in_popen;
ssize_t popen_get(void**, size_t*);

void my_popen(void** ptr, size_t* sz, int argc, ...)
{
    if(*ptr || *sz)
        abort();
    va_list va;
    va_start(va, argc);
    is_in_popen = 1;
    pid_t p = my_fork();
    if(!p)
    {
        vexecute(argc, va);
    }
    va_end(va);
    popen_get(ptr, sz);
}

int init_daemon(void)
{
    if(jailbreak())
        return -1;
    int cnsl = open("/dev/console", O_RDWR);
    dup2(cnsl, 0);
    dup2(cnsl, 1);
    dup2(cnsl, 2);
    close(cnsl);
    return 0;
}

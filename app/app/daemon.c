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
int maybe_load_tun(void);
int maybe_load_uhack(void);
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

void my_popen(void** ptr, size_t* sz, int argc, ...)
{
    va_list va;
    va_start(va, argc);
    int pipe[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, pipe);
    int out_fd = dup(1);
    pid_t p = my_fork();
    if(!p)
    {
        dup2(pipe[1], 1);
        close(pipe[1]);
        vexecute(argc, va);
    }
    va_end(va);
    dup2(out_fd, 1);
    close(out_fd);
    char* buf = (char*)*ptr;
    size_t buf_sz = *sz;
    size_t offset = 0;
    for(;;)
    {
        if(offset == buf_sz)
        {
            buf_sz = 2 * buf_sz + 1;
            buf = realloc(buf, buf_sz);
        }
        size_t chk = read(pipe[0], buf + offset, buf_sz - offset);
        if(chk <= 0)
            break;
        offset += chk;
    }
    *ptr = buf;
    *sz = offset;
    close(pipe[0]);
}

int init_daemon(void)
{
    if(jailbreak())
        return -1;
    int fd = open("/dev/console", O_RDWR);
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    if(maybe_load_tun())
        return -1;
    if(maybe_load_uhack())
        return -1;
    return 0;
}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

int blob_hamachid, blob_cpp_elf;

int jailbreak(void);
int maybe_load_tun(void);
int maybe_load_uhack(void);
pid_t my_fork(int keep1, int keep2, int keep3, int* masterfd);
int ldr_main(int argc, const char** argv);

void vexecute(int argc, va_list va)
{
    char dev_fd_1[32], dev_fd_2[32];
    sprintf(dev_fd_1, "/dev/fd/%d", blob_hamachid);
    sprintf(dev_fd_2, "/dev/fd/%d", blob_cpp_elf);
    const char* argv[argc + 4];
    argv[0] = "./ldr";
    argv[1] = dev_fd_1;
    argv[2] = dev_fd_2;
    for(int i = 0; i < argc; i++)
        argv[i + 3] = va_arg(va, const char*);
    argv[argc + 3] = 0;
    exit(ldr_main(argc + 3, argv));
}

void execute(int argc, ...)
{
    va_list va;
    va_start(va, argc);
    vexecute(argc, va);
    va_end(va);
}

void* daemon_thread(void* _)
{
    int master_fd;
    for(;;)
    {
        unlink("/user/app/BREW00179/hamachi/var/run/logmein-hamachi/hamachid.lock");
        pid_t p = my_fork(blob_hamachid, blob_cpp_elf, -1, &master_fd);
        if(!p)
            execute(1, "hamachid");
        char c;
        read(master_fd, &c, 1); // wait for termination
        close(master_fd);
    }
}

void start_daemon(void)
{
    //daemon_thread(0);
    pthread_t shit;
    pthread_create(&shit, 0, daemon_thread, 0);
}

void my_popen(void** ptr, size_t* sz, int argc, ...)
{
    va_list va;
    va_start(va, argc);
    int master_fd;
    int pipe[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, pipe);
    pid_t p = my_fork(blob_hamachid, blob_cpp_elf, pipe[1], &master_fd);
    if(!p)
    {
        dup2(pipe[1], 1);
        vexecute(argc, va);
    }
    va_end(va);
    close(pipe[1]);
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
    close(master_fd);
}

int init_daemon(void)
{
    blob_hamachid = open("/app0/hamachid", O_RDONLY);
    blob_cpp_elf = open("/app0/cpp.elf", O_RDONLY);
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

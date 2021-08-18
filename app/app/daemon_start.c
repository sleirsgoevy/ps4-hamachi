#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <alloca.h>
#include <errno.h>
#include <signal.h>
#include <orbis/libkernel.h>

int jailbreak(void);

#define APP_ID "BREW00179"
#define DAEMON_ID "HOME00179"

asm("nmount:\nmov $378, %eax\nsyscall\nret");
asm("unmount:\nmov $22, %eax\nsyscall\nret");

int nmount(const struct iovec*, unsigned int n, int flags);
int unmount(const char* path, int flags);
enum { MNT_FORCE = 0x80000, MNT_RDONLY = 1 };

int do_unmount(const char* path)
{
    printf("do_unmont(%s)\n", path);
    int x;
    while((x = unmount(path, 0)) == EBUSY);
    printf("... = %d\n", x);
    return x;
}

#define ASPRINTF(p, ...) \
{\
    char ASPRINTF_CH;\
    size_t ASPRINTF_SZ = 1 + snprintf(&ASPRINTF_CH, 1, __VA_ARGS__);\
    p = alloca(ASPRINTF_SZ);\
    snprintf(p, ASPRINTF_SZ, __VA_ARGS__);\
}

int copy_file(const char* tgt, const char* src)
{
    int fd1 = open(src, O_RDONLY);
    if(fd1 < 0)
        return -1;
    int fd2 = open(tgt, O_WRONLY|O_CREAT, 0777);
    if(fd2 < 0)
    {
        close(fd1);
        close(fd2);
        return -1;
    }
    char buf[4096];
    ssize_t chk;
    while((chk = read(fd1, buf, sizeof(buf))) > 0)
    {
        char* p = buf;
        size_t sz = chk;
        while((chk = write(fd2, p, sz)) > 0)
        {
            p += chk;
            sz -= chk;
        }
        if(sz)
        {
            chk = -1;
            break;
        }
    }
    ftruncate(fd2, lseek(fd2, 0, SEEK_CUR));
    fsync(fd2);
    close(fd1);
    close(fd2);
    return chk;
}

int mount_nullfs(const char* mnt, const char* tgt)
{
    struct iovec data[6] = {
        {"fstype", 7}, {"nullfs", 7},
        {"fspath", 7}, {(char*)mnt, strlen(mnt)+1},
        {"target", 7}, {(char*)tgt, strlen(tgt)+1},
    };
    return nmount(data, 6, MNT_RDONLY);
}

int mount_nullfs_in(const char* mnt, const char* tgt, const char* name)
{
    char* p1;
    char* p2;
    ASPRINTF(p1, "%s/%s", mnt, name);
    ASPRINTF(p2, "%s/%s", tgt, name);
    mkdir(p1, 0777);
    return mount_nullfs(p1, p2);
}

void mount_all(const char* mnt_dir, const char* tgt_dir)
{
    DIR* d = opendir(tgt_dir);
    if(!d)
        return;
    struct dirent* de;
    while((de = readdir(d)))
    {
        if(!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;
        mount_nullfs_in(mnt_dir, tgt_dir, de->d_name);
    }
    closedir(d);
}

int umount_in(const char* a, const char* b)
{
    char* p;
    ASPRINTF(p, "%s/%s", a, b);
    return do_unmount(p);
}

void umount_all(const char* mnt_dir)
{
    DIR* d = opendir(mnt_dir);
    struct dirent* de;
    while((de = readdir(d)))
    {
        if(!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;
        umount_in(mnt_dir, de->d_name);
    }
    closedir(d);
}

typedef struct LncAppParam
{
    uint32_t sz;
    uint32_t user_id;
    uint32_t app_opt;
    uint64_t crash_report;
    int check_flag;
} LncAppParam;

int sceSystemServiceLaunchApp(const char* titleid, const char** argv, LncAppParam*);

int is_daemon_outdated(void)
{
    struct stat st1, st2;
    if(stat("/data/homebrew/" DAEMON_ID "-eboot.bin", &st1)
    || stat("/user/app/" APP_ID "/app.pkg", &st2))
        return 1; //assume the worst
    return st2.st_mtime >= st1.st_mtime;
}

int start_daemon(void)
{
    if(jailbreak())
        return 1;
    int outd = is_daemon_outdated();
    int pid_file = open("/mnt/daemons/" DAEMON_ID "/pid", O_RDONLY);
    if(pid_file >= 0)
    {
        pid_t p;
        read(pid_file, &p, sizeof(p));
        close(pid_file);
        if(!outd)
            //keep running old version
            return -1;
        unlink("/mnt/daemons/" DAEMON_ID "/pid");
        kill(p, SIGKILL);
    }
    int cnsl = open("/dev/console", O_RDWR);
    dup2(cnsl, 0);
    dup2(cnsl, 1);
    dup2(cnsl, 2);
    close(cnsl);
    if(mkdir("/mnt/daemons/" DAEMON_ID, 0777) && errno != EEXIST)
    {
        printf("mounting nullfs\n");
        mkdir("/mnt/daemons", 0777);
        mount_all("/mnt/daemons", "/system/vsh/app");
        mount_nullfs("/system/vsh/app", "/mnt/daemons");
        mount_all("/system/vsh/app", "/mnt/daemons");
        mkdir("/mnt/daemons/" DAEMON_ID, 0777);
    }
    printf("copying daemon\n");
    if(outd)
    {
        mkdir("/data/homebrew", 0777);
        copy_file("/data/homebrew/" DAEMON_ID "-eboot.bin", "/mnt/sandbox/" APP_ID "_000/app0/eboot.bin");
    }
    symlink("/data/homebrew/" DAEMON_ID "-eboot.bin", "/mnt/daemons/" DAEMON_ID "/eboot.bin");
    mkdir("/mnt/daemons/" DAEMON_ID "/sce_sys", 0777);
    copy_file("/mnt/daemons/" DAEMON_ID "/sce_sys/param.sfo", "/mnt/sandbox/" APP_ID "_000/app0/daemon.sfo");
    printf("launching daemon\n");
    LncAppParam param = { .sz = sizeof(param) };
    const char* new_argv[] = {"--daemon", 0};
    while(sceSystemServiceLaunchApp(DAEMON_ID, new_argv, &param) == 0x8094000c /* already running */);
    return 0;
}

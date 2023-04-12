#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

static int is_mountpoint(const char* path)
{
    struct stat st1, st2;
    return !stat("/mnt", &st1) && !stat(path, &st2) && st1.st_dev != st2.st_dev;
}

static int get_usb_no(void)
{
    for(int i = 0; i < 8; i++)
    {
        char path[10];
        sprintf(path, "/mnt/usb%d", i);
        if(is_mountpoint(path))
            return i;
    }
    return -1;
}

int copy_file(const char* tgt, const char* src);

const char* dump_logs(void)
{
    int usb_no = get_usb_no();
    if(usb_no < 0)
        return "USB device not found";
    char path[32];
    sprintf(path, "/mnt/usb%d/h2-engine.log.txt", usb_no);
    if(copy_file(path, "/user/home/hamachi/var/lib/logmein-hamachi/h2-engine.log"))
    {
        static char error[256];
        sprintf(error, "Error copying file: %s", strerror(errno));
        return error;
    }
    return 0;
}

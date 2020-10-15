#include "mira_header.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <sys/syscall.h>

extern "C" int jailbreak()
{
    MiraThreadCredentials params = {
        .State = GSState::Set,
        .ProcessId = getpid(),
        .ThreadId = -1,
        .EffectiveUserId = 0,
        .RealUserId = 0,
        .SavedUserId = 0,
        .RealGroupId = 0,
        .SavedGroupId = 0,
        .Prison = MiraThreadCredentialsPrison::Root,
        .SceAuthId = SceAuthenticationId::SceSysCore,
        .Capabilities = {SceCapabilites::Max, SceCapabilites::Max, SceCapabilites::Max, SceCapabilites::Max},
    };
    int fd = open("/dev/mira", 0);
    if(fd < 0)
        return -1;
    if(ioctl(fd, 0xc0704d01, &params))
    {
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

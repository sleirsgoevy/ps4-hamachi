#include "libjbc/jailbreak.h"

int jailbreak()
{
    struct jbc_cred cr;
    if(jbc_get_cred(&cr))
        return -1;
    if(jbc_jailbreak_cred(&cr))
        return -1;
    cr.jdir = 0;
    cr.sceProcType = 0x3800000000000010;
    cr.sonyCred = 0x40001c0000000000;
    cr.sceProcCap = 0x900000000000ff00;
    if(jbc_set_cred(&cr))
        return -1;
    return 0;
}

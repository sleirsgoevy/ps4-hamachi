#include "libjbc/jailbreak.h"

int jailbreak()
{
    struct jbc_cred cr;
    if(jbc_get_cred(&cr))
        return -1;
    if(jbc_jailbreak_cred(&cr))
        return -1;
    if(jbc_set_cred(&cr))
        return -1;
    return 0;
}

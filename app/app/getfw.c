#include <orbis/libkernel.h>

int dynlib_get_obj_member(int module_id, int which, void** out);

//obtain the target firmware of libkernel.sprx. should be the same as the current firmware, and bypasses spoofs
uint32_t get_fw_version(void)
{
    int libkernel = sceKernelLoadStartModule("libkernel.sprx", 0, 0, 0, 0, 0);
    if(libkernel < 0)
        return -1;
    void* sce_proc_param;
    if(dynlib_get_obj_member(libkernel, 8, &sce_proc_param))
        return -1;
    uint32_t* spp = sce_proc_param;
    return spp[4];
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <orbis/libkernel.h>
#include <signal.h>
#include "gui.h"

int init_daemon(void);
void start_daemon(void);
int my_popen(void** buf, size_t* sz, int argc, ...);

int ldr_main(int argc, const char** argv);
pid_t my_fork(int, int, int, int*);

char* error_string(void*, size_t);
char** list_networks(int*);
int errcmp(const char*, const char*);

char* concat3(const char* s1, const char* s2, const char* s3)
{
    size_t sz1 = strlen(s1);
    size_t sz2 = strlen(s2);
    size_t sz3 = strlen(s3);
    char* buf = malloc(sz1 + sz2 + sz3 + 1);
    strcpy(buf, s1);
    strcpy(buf + sz1, s2);
    strcpy(buf + sz1 + sz2, s3);
    return buf;
}

int is_sw_version_supported(void)
{
    OrbisKernelSwVersion sw_ver;
    sceKernelGetSystemSwVersion(&sw_ver);
    int ver = sw_ver.i_version >> 16;
    return ver == 0x672 || ver == 0x702 || (ver >= 0x750 && ver <= 0x755);
}

int main()
{
    gui_preinit();
    if(!is_sw_version_supported())
    {
        gui_init();
        gui_show_error_screen("Your firmware version is not supported. Supported versions: 6.72, 7.02, 7.5X"); //noreturn
    }
    if(init_daemon())
    {
        gui_init();
        gui_show_error_screen("Privilege escalation failed. Make sure you are running Mira, not just HEN."); //noreturn
    }
    start_daemon();
    gui_init();
    void* buf = 0;
    size_t sz = 0;
    char* errs;
    gui_show_status_message("Launching daemon...");
    sceKernelUsleep(5000000);
    gui_show_status_message("Logging in...");
    my_popen(&buf, &sz, 2, "hamachi", "login");
    if((errs = error_string(buf, sz)))
        gui_show_error_screen(errs); //noreturn
    sceKernelUsleep(500000);
    buf = 0;
    sz = 0;
    int action, netid = 0;
    for(;;)
    {
        int nnets;
        char** netlist = list_networks(&nnets);
        gui_show_network_list((const char**)netlist, nnets, &netid, &action);
        if(action == 1) // create
        {
            char* login;
            char* password;
            gui_login("Create network", &login, &password);
            if(login && password)
            {
                char* msg = concat3("Creating ", login, "...");
                gui_show_status_message(msg);
                free(msg);
                my_popen(&buf, &sz, 4, "hamachi", "create", login, password);
                free(login);
                free(password);
                errs = error_string(buf, sz);
                buf = 0;
                sz = 0;
                if(errs)
                {
                    gui_show_error_dialog(errs);
                    free(errs);
                    goto action_fail;
                }
                sceKernelUsleep(500000);
            }
        }
        else if(action == 2) // join
        {
            char* login;
            char* password;
            gui_login("Join network", &login, &password);
            if(login && password)
            {
                char* msg = concat3("Joining ", login, "...");
                gui_show_status_message(msg);
                free(msg);
                my_popen(&buf, &sz, 4, "hamachi", "join", login, password);
                free(login);
                free(password);
                errs = error_string(buf, sz);
                buf = 0;
                sz = 0;
                if(errs)
                {
                    gui_show_error_dialog(errs);
                    free(errs);
                    goto action_fail;
                }
                sceKernelUsleep(500000);
            }
        }
        else if(action == 3) // delete
        {
            char* msg;
            msg = concat3("Do you want to leave ", netlist[netid], "?");
            int ans = gui_show_question(msg);
            free(msg);
            if(!ans)
                goto action_fail;
            msg = concat3("Leaving ", netlist[netid], "...");
            gui_show_status_message(msg);
            free(msg);
            my_popen(&buf, &sz, 3, "hamachi", "leave", netlist[netid]);
            errs = error_string(buf, sz);
            buf = 0;
            sz = 0;
            if(errs && !errcmp(errs, ". failed, denied\n"))
            {
                free(errs);
                msg = concat3("You are the owner of ", netlist[netid], ". Delete network?");
                int ans = gui_show_question(msg);
                free(msg);
                if(!ans)
                    goto action_fail;
                msg = concat3("Deleting ", netlist[netid], "...");
                gui_show_status_message(msg);
                free(msg);
                my_popen(&buf, &sz, 3, "hamachi", "delete", netlist[netid]);
                errs = error_string(buf, sz);
                buf = 0;
                sz = 0;
            }
            if(errs)
            {
                gui_show_error_dialog(errs);
                free(errs);
                goto action_fail;
            }
            sceKernelUsleep(500000);
        }
    action_fail:
        free(netlist);
    }
}

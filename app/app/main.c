#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>
#include <signal.h>
#include "gui.h"

int maybe_load_tun(void);
int init_daemon(void);
int start_daemon(void);
void daemon_thread(void);
int my_popen(void** buf, size_t* sz, int argc, ...);

int ldr_main(int argc, const char** argv);

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

uint32_t get_fw_version(void);

void check_is_sw_version_supported(void)
{
    int ver = get_fw_version() >> 16;
    if(ver == 0x505 || ver == 0x672 || ver == 0x702 || (ver >= 0x750 && ver <= 0x755) || ver == 0x900)
        return;
    char errormsg[256];
    sprintf(errormsg, "Your firmware version is not supported. Supported versions: 5.05, 6.72, 7.02, 7.5X, 9.00. Your version: %x.%02x", ver >> 8, ver & 255);
    gui_init();
    gui_show_error_screen(errormsg);
}

int main(int argc, const char** argv)
{
    if(argc >= 2 && !strcmp(argv[1], "--daemon"))
    {
        sceSysmoduleLoadModuleInternal(0x8000001c); //libSceNet.sprx
        init_daemon();
        daemon_thread();
        return 0;
    }
    sceSysmoduleLoadModule(0x96); //libSceImeDialog.sprx
    gui_preinit();
    check_is_sw_version_supported();
    maybe_load_tun();
    int d_st = start_daemon();
    if(d_st > 0)
    {
        gui_init();
        gui_show_error_screen("Could not start daemon."); //noreturn
    }
    if(init_daemon())
    {
        gui_init();
        gui_show_error_screen("Privilege escalation failed."); //noreturn
    }
    gui_init();
    void* buf = 0;
    size_t sz = 0;
    char* errs;
    if(!d_st)
    {
        gui_show_status_message("Launching daemon...");
        sceKernelUsleep(5000000);
    }
    gui_show_status_message("Logging in...");
    my_popen(&buf, &sz, 2, "hamachi", "login");
    if((errs = error_string(buf, sz)) && strcmp(errs, "Already logged in.\n"))
        gui_show_error_screen(errs); //noreturn
    sceKernelUsleep(1000000);
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
                sceKernelUsleep(1000000);
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
                sceKernelUsleep(1000000);
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
            sceKernelUsleep(1000000);
        }
    action_fail:
        free(netlist);
    }
}

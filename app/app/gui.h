#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void gui_preinit(void);
void gui_init(void);
void gui_show_error_screen(const char*);
void gui_show_error_dialog(const char*);
void gui_show_notification(const char*);
void gui_show_status_message(const char*);
void gui_show_network_list(const char**, int, int*, int*);
int gui_show_question(const char*);
void gui_login(const char*, char**, char**);

#ifdef __cplusplus
}
#endif

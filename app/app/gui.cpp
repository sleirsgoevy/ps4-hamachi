#include "graphics.h"
#include "gui.h"
#include <orbis/Pad.h>
#include <orbis/UserService.h>
#include <orbis/ImeDialog.h>

//std::stringstream debugLogStream;

Scene2D* scene;
FT_Face face;
static int curFrame = 0;
static int userID = -1;
static int padHandle = -1;

static void flip()
{
    scene->SubmitFlip(curFrame);
    scene->FrameWait(curFrame);
    scene->FrameBufferSwap();
    curFrame++;
}

static void drawSkeleton()
{
    Color blue = {128, 128, 255};
    Color white = {255, 255, 255};
    for(int i = 0; i < 1080; i++)
        for(int j = 0; j < 1920; j++)
            scene->DrawPixel(j, i, i<60?blue:white);
    scene->DrawText((char*)"LogMeIn Hamachi (unofficial)", face, 0, 40, blue, white);
}

static void drawExclamationMark()
{
    Color red = {255, 0, 0};
    Color black = {0, 0, 0};
    // triangle
    for(int i = 250; i < 500; i++)
        for(int j = 835; j < 1085; j++)
            if(j >= 1085 - i / 2 && j <= 835 + i / 2)
                scene->DrawPixel(j, i, red);
    for(int i = 325; i < 475; i++)
        for(int j = 955; j < 965; j++)
            if(i < 465 || i >= 470)
                scene->DrawPixel(j, i, black);
}

static void drawMessage(const char* message, int y = 530)
{
    Color white = {255, 255, 255};
    Color black = {0, 0, 0};
    int w, h;
    scene->GetTextDimensions((char*)message, face, w, h);
    scene->DrawText((char*)message, face, 960 - w / 2, y, white, black);
}

static void drawNetworks(const char** networks, int cnt, int cur)
{
    Color white = {255, 255, 255};
    Color black = {0, 0, 0};
    Color gray = {128, 128, 128};
    scene->DrawRectangle(0, 65 + 40 * cur, 1920, 35, gray);
    for(int i = 0; i < cnt; i++)
        scene->DrawText((char*)networks[i], face, 0, 90 + 40 * i, i==cur?gray:white, i==cur?white:black);
}

static void drawCircle(int x, int y)
{
    Color red = {255, 0, 0};
    for(int i = 0; i < 50; i++)
        for(int j = 0; j < 50; j++)
        {
            int d = (i - 25) * (i - 25) + (j - 25) * (j - 25);
            if(d >= 12 * 12 && d <= 15 * 15)
                scene->DrawPixel(x + i, y + j, red);
        }
}

static void drawTriangle(int x, int y)
{
    Color green = {0, 255, 0};
    for(int j = 12; j <= 37; j++)
        for(int i = 25 - (j - 9) * 5 / 6 ; i < 25 + (j - 9) * 5 / 6; i++)
            if(j >= 35 || i <= 27 - (j - 12) * 5 / 6 || i >= 23 + (j - 12) * 5 / 6)
                scene->DrawPixel(x + i, y + j, green);
}

static void drawSquare(int x, int y)
{
    Color purple = {255, 64, 255};
    for(int i = 10; i < 40; i++)
        for(int j = 0; j < 4; j++)
        {
            scene->DrawPixel(x + i, y + j + 10, purple);
            scene->DrawPixel(x + j + 10, y + i, purple);
            scene->DrawPixel(x + i, y - j + 40, purple);
            scene->DrawPixel(x - j + 40, y + i, purple);
        }
}

static void drawOptionsButton(int x, int y)
{
    Color gray = {64, 64, 64};
    for(int i = 0; i < 15; i++)
        for(int j = 0; j < 30; j++)
        {
            int ii = (i >= 7 ? 14 - i : i);
            int jj = (j >= 15 ? 29 - j : j);
            if(ii >= 5 || jj >= 5 || (ii - 5) * (ii - 5) + (jj - 5) * (jj - 5) < 25)
                scene->DrawPixel(x + i, y + j + 10, gray);
        }
}

static void drawNetworkListToolbox()
{
    Color white = {255, 255, 255};
    Color black = {0, 0, 0};
    int w1, h1, w2, h2, w3, h3, w4, h4;
    scene->GetTextDimensions((char*)"Create network", face, w1, h1);
    scene->GetTextDimensions((char*)"Connect to network", face, w2, h2);
    scene->GetTextDimensions((char*)"Leave network", face, w3, h3);
    scene->GetTextDimensions((char*)"Dump logs to USB", face, w4, h4);
    drawTriangle(0, 1030);
    scene->DrawText((char*)"Create network", face, 50, 1065, white, black);
    drawSquare(100 + w1, 1030);
    scene->DrawText((char*)"Connect to network", face, 150 + w1, 1065, white, black);
    drawCircle(200 + w1 + w2, 1030);
    scene->DrawText((char*)"Leave network", face, 250 + w1 + w2, 1065, white, black);
    drawOptionsButton(300 + w1 + w2 + w3, 1030);
    scene->DrawText((char*)"Dump logs to USB", face, 330 + w1 + w2 + w3, 1065, white, black);
}

static unsigned int pollPad()
{
    OrbisPadData data;
    do
        scePadReadState(padHandle, &data);
    while(data.buttons);
    do
        scePadReadState(padHandle, &data);
    while(!data.buttons);
    return data.buttons;
}

static void drawInput(int y, int margin, Color bgColor, Color borderColor, Color textColor, const char* text)
{
    for(int x = margin; x <= 1920 - margin; x++)
    {
        scene->DrawPixel(x, y, borderColor);
        scene->DrawPixel(x, y + 40, borderColor);
    }
    for(int i = y; i <= y + 40; i++)
    {
        scene->DrawPixel(margin, i, borderColor);
        scene->DrawPixel(1920 - margin, i, borderColor);
    }
    for(int i = margin + 1; i < 1920 - margin; i++)
        for(int j = y + 1; j < y + 40; j++)
            scene->DrawPixel(i, j, bgColor);
    scene->DrawText((char*)text, face, margin + 5, y + 30, bgColor, textColor);
}

static void drawLoginFormText(const char* title)
{
    Color white = {255, 255, 255};
    Color black = {0, 0, 0};
    drawSkeleton();
    scene->DrawText((char*)title, face, 10, 90, white, black);
    scene->DrawText((char*)"Network name:", face, 20, 140, white, black);
    scene->DrawText((char*)"Password:", face, 20, 240, white, black);
}

static void drawInput(int y, int margin, const char* msg)
{
    Color grey = {192, 192, 192};
    Color gray = {128, 128, 128};
    drawInput(y, margin, grey, gray, gray, msg);
}

static void drawFocusedInput(int y, int margin, const char* msg)
{
    Color white = {255, 255, 255};
    Color black = {0, 0, 0};
    drawInput(y, margin, white, black, black, msg);
}

static void drawLoginForm(int* ys, int* margins, char** ptrs, int idx)
{
    drawLoginFormText(ptrs[2]);
    for(int i = 0; i < 3; i++)
        if(i == idx)
            drawFocusedInput(ys[i], margins[i], ptrs[i]);
        else
            drawInput(ys[i], margins[i], ptrs[i]);
}

static void drawError(const char* message, bool ild = false)
{
    drawSkeleton();
    drawExclamationMark();
    drawMessage(message);
    if(!ild)
        drawMessage("Press OPTIONS to dump logs", 650);
}

void showErrorDialog(const char* message, bool ild = false);
void gui_show_notification(const char* message);
extern "C" const char* dump_logs(void);

static void doDumpLogs()
{
    const char* err = dump_logs();
    if(!err)
        gui_show_notification("Logs have been dumped to USB");
    else
        showErrorDialog(err, true);
}

void showErrorDialog(const char* message, bool ild)
{
    Color blue = {128, 128, 255};
    Color white = {255, 255, 255};
    for(;;)
    {
        drawError(message, ild);
        scene->DrawRectangle(720, 540, 480, 40, blue);
        int w, h;
        scene->GetTextDimensions((char*)"OK", face, w, h);
        scene->DrawText((char*)"OK", face, 960 - w/2, 570, blue, white);
        flip();
        unsigned int keys = pollPad();
        if(!ild && (keys & ORBIS_PAD_BUTTON_OPTIONS))
            doDumpLogs();
        if((keys & ORBIS_PAD_BUTTON_CROSS))
            return;
    }
}

void convertFromAscii(char* ptr, size_t sz)
{
    size_t l = strlen(ptr);
    unsigned char* p = (unsigned char*)ptr + l;
    uint16_t* q = (uint16_t*)ptr + l;
    char* end = (char*)q;
    *q = *p;
    for(size_t i = 0; i < l; i++)
        *--q = *--p;
    memset(end, 0, ptr+sz-end);
}

void convertToAscii(char* ptr)
{
    const uint16_t* src = (const uint16_t*)ptr;
    unsigned char* dst = (unsigned char*)ptr;
    while((*dst++ = *src++));
}

extern "C" void gui_preinit(void)
{
    scene = new Scene2D(1920, 1080, 4);
    scene->InitFreeType();
    scene->InitFont(&face, "/app0/font.ttf", 30);
}

extern "C" void gui_init(void)
{
    scene->Init(1920*1080*4*2, 2);
    scene->SetActiveFrameBuffer(0);
    scene->FrameBufferClear();
    OrbisUserServiceInitializeParams param;
    param.priority = ORBIS_KERNEL_PRIO_FIFO_LOWEST;
    sceUserServiceInitialize(&param);
    sceUserServiceGetInitialUser(&userID);
    scePadInit();
    padHandle = scePadOpen(userID, 0, 0, 0);
}

extern "C" void gui_show_error_screen(const char* message)
{
    for(;;)
    {
        drawError(message);
        flip();
        unsigned int keys = pollPad();
        if((keys & ORBIS_PAD_BUTTON_OPTIONS))
            doDumpLogs();
    }
}

extern "C" void gui_show_error_dialog(const char* message)
{
    return showErrorDialog(message);
}

extern "C" void gui_show_notification(const char* message)
{
    Color blue = {128, 128, 255};
    Color white = {255, 255, 255};
    for(;;)
    {
        drawSkeleton();
        drawMessage(message);
        scene->DrawRectangle(720, 540, 480, 40, blue);
        int w, h;
        scene->GetTextDimensions((char*)"OK", face, w, h);
        scene->DrawText((char*)"OK", face, 960 - w/2, 570, blue, white);
        flip();
        unsigned int keys = pollPad();
        if((keys & ORBIS_PAD_BUTTON_CROSS))
            return;
    }
}

extern "C" void gui_show_status_message(const char* message)
{
    drawSkeleton();
    drawMessage(message);
    flip();
}

extern "C" void gui_show_network_list(const char** networks, int net_cnt, int* cur_net, int* action)
{
    if(!net_cnt)
    {
        for(;;)
        {
            drawSkeleton();
            drawMessage("You're not in any networks");
            drawNetworkListToolbox();
            flip();
            unsigned int keys = pollPad();
            if((keys & ORBIS_PAD_BUTTON_TRIANGLE))
            {
                *action = 1;
                return;
            }
            else if((keys & ORBIS_PAD_BUTTON_SQUARE))
            {
                *action = 2;
                return;
            }
            else if((keys & ORBIS_PAD_BUTTON_OPTIONS))
                doDumpLogs();
        }
    }
    if(*cur_net >= net_cnt)
        *cur_net = net_cnt - 1;
    for(;;)
    {
        drawSkeleton();
        drawNetworks(networks, net_cnt, *cur_net);
        drawNetworkListToolbox();
        flip();
        unsigned int keys = pollPad();
        if((keys & ORBIS_PAD_BUTTON_UP))
        {
            if(*cur_net)
                --*cur_net;
        }
        else if((keys & ORBIS_PAD_BUTTON_DOWN))
        {
            if(*cur_net + 1 < net_cnt)
                ++*cur_net;
        }
        else if((keys & ORBIS_PAD_BUTTON_TRIANGLE))
        {
            *action = 1;
            return;
        }
        else if((keys & ORBIS_PAD_BUTTON_SQUARE))
        {
            *action = 2;
            return;
        }
        else if((keys & ORBIS_PAD_BUTTON_CIRCLE))
        {
            *action = 3;
            return;
        }
        else if((keys & ORBIS_PAD_BUTTON_OPTIONS))
            doDumpLogs();
    }
}

extern "C" int gui_show_question(const char* msg)
{
    Color white = {255, 255, 255};
    Color gray = {128, 128, 128};
    Color blue = {128, 128, 255};
    int choice = 0;
    for(;;)
    {
        int w, h;
        drawSkeleton();
        drawMessage(msg);
        scene->DrawRectangle(482, 540, 476, 40, choice?gray:blue);
        scene->DrawRectangle(962, 540, 476, 40, choice?blue:gray);
        scene->GetTextDimensions((char*)"Cancel", face, w, h);
        scene->DrawText((char*)"Cancel", face, 720 - w/2, 570, choice?gray:blue, white);
        scene->GetTextDimensions((char*)"OK", face, w, h);
        scene->DrawText((char*)"OK", face, 1200 - w / 2, 570, choice?blue:gray, white);
        flip();
        unsigned int keys = pollPad();
        if((keys & ORBIS_PAD_BUTTON_RIGHT))
            choice = 1;
        else if((keys & ORBIS_PAD_BUTTON_LEFT))
            choice = 0;
        else if((keys & ORBIS_PAD_BUTTON_CIRCLE))
            return 0;
        else if((keys & ORBIS_PAD_BUTTON_CROSS))
            return choice;
    }
}

extern "C" void gui_login(const char* form_name, char** login, char** password)
{
    char* ptrs[3] = {(char*)malloc(1), (char*)malloc(1), (char*)form_name};
    ptrs[0][0] = ptrs[1][0] = 0;
    int ys[3] = {150, 250, 300};
    int margins[3] = {20, 20, 720};
    int idx = 0;
    for(;;)
    {
        drawLoginForm(ys, margins, ptrs, idx);
        flip();
        unsigned int pad = pollPad();
        if((pad & ORBIS_PAD_BUTTON_UP))
        {
            if(idx > 0)
                idx--;
        }
        else if((pad & ORBIS_PAD_BUTTON_DOWN))
        {
            if(idx < 2)
                idx++;
        }
        else if((pad & ORBIS_PAD_BUTTON_CIRCLE))
        {
            free(ptrs[0]);
            free(ptrs[1]);
            *login = *password = 0;
            return;
        }
        else if((pad & ORBIS_PAD_BUTTON_CROSS))
        {
            for(;;)
            {
                if(idx == 2)
                {
                    *login = ptrs[0];
                    *password = ptrs[1];
                    return;
                }
                ptrs[idx] = (char*)realloc(ptrs[idx], 4096);
                convertFromAscii(ptrs[idx], 4096);
                OrbisImeDialogSetting ime_params = {
                    .userId = (uint32_t)userID,
                    .type = ORBIS_TYPE_BASIC_LATIN,
                    .maxTextLength = 2047,
                    .inputTextBuffer = (uint16_t*)ptrs[idx],
                    .posy = (float)ys[idx],
                };
                sceImeDialogInit(&ime_params, 0);
                while(sceImeDialogGetStatus() != 2)
                    sceKernelUsleep(100000);
                OrbisDialogResult res;
                memset(&res, 0, sizeof(res));
                sceImeDialogGetResult(&res);
                sceImeDialogForceClose();
                convertToAscii(ptrs[idx]);
                if(res.endstatus != ORBIS_DIALOG_OK)
                    break;
                idx++;
                drawLoginForm(ys, margins, ptrs, idx);
                flip();
            } 
        }
    }
}

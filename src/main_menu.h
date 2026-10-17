#ifndef MAIN_MENU_H
#define MAIN_MENU_H
extern "C"
{
#include "psx.h"
}
struct MenuScene
{
    const char *library;
    const char *bank;
    uint32 pos[3];
    uint32 radius;
    uint32 height;
};

void ResetMenuScenes();
void AdvanceMenuScene();
const MenuScene &GetCurMenuScene();
uint32 QueueSelectedLevelResources();
uint32 RunMainMenu();
#endif

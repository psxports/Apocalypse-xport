#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "vector3.h"
extern "C"
{
#include "psx.h"
}

struct MenuScene
{
    const char *library;
    const char *bank;
    Vector3 pos;
    uint32 radius;
    uint32 height;
};

void ResetMenuScenes();
uint32 RunMainMenu();
#endif

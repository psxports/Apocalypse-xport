#ifndef SCREEN_FLASH_H
#define SCREEN_FLASH_H

extern "C"
{
#include "psx.h"
}

void ResetScreenFlash();
uint32 IsScreenFadeComplete();
uint32 SetScreenFlash(uint32 red, uint32 green, uint32 blue, uint32 duration, uint32 priority, uint32 mode);
uint32 FadeScreenToWhite(uint32 duration, uint32 mode);
uint32 UpdateScreenFlash();
uint32 DrawScreenFlash();

#endif

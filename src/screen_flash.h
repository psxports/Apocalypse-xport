#ifndef SCREEN_FLASH_H
#define SCREEN_FLASH_H

extern "C"
{
#include "psx.h"
}

void InitializeScrFlash();
void ResetScrFlash();
uint32 IsScrFadeComplete();
uint32 SetScrFlash(uint8 r, uint8 g, uint8 b, uint32 duration, uint8 priority, uint32 mode);
uint32 FadeScrToWhite(uint32 duration, uint32 mode);
uint32 UpdateScrFlash();
uint32 DrawScrFlash();

#endif

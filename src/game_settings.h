#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

extern "C"
{
#include "psx.h"
}

uint32 SaveGameSettings(uint8 *record);
uint32 RestoreGameSettings(const uint8 *record);

#endif

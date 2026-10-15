#ifndef PLAYER_SAVE_STATE_H
#define PLAYER_SAVE_STATE_H

extern "C"
{
#include "psx.h"
}

uint32 FindLevelEntry(uint32 name);
uint32 CaptureCurrentPlayerSave();
uint32 CalculatePlayerSaveChecksum(const uint8 *record);
uint32 RestorePlayerInventory(uint8 *record);

#endif

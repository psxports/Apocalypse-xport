#ifndef PLAYER_SAVE_STATE_H
#define PLAYER_SAVE_STATE_H

#include "player_save_record.h"
#include "player_save_progress.h"

extern "C"
{
#include "psx.h"
}

#include "game_settings.h"

enum
{
    PlayerSaveSize = 136
};

struct PlayerSaveStorage
{
    PlayerSaveRecord cur;
    PlayerSaveRecord sel;
    PlayerSaveRecord replay_backup;
};

extern PlayerSaveStorage player_saves;
void ResetPlayerSaveStorage();
void ResetPlayerSaveTemplates();
const PlayerSaveRecord &GetDefaultPlayerSave();
PlayerSaveRecord &GetDemoPlayerSave(uint32 idx);
uint32 CaptureCurPlayerSave();
void DecodePlayerSave(PlayerSaveRecord &rec, const uint8 *data, uint32 bytes);
void EncodePlayerSave(uint8 *data, uint32 bytes, const PlayerSaveRecord &rec);

#endif

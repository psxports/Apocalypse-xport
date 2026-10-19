#ifndef PLAYER_SAVE_STATE_H
#define PLAYER_SAVE_STATE_H

extern "C"
{
#include "psx.h"
}

#include "game_settings.h"

enum
{
    PlayerSaveSize = 136
};

struct PlayerSaveProgress
{
    void SetLevel(const char *name);
    void SetCheckpoint(const char *name);

    char level[9];
    char checkpoint[53];
    uint8 weapon_mask;
    uint8 sel_weapon;
    uint32 ammo[8];
    sint32 health;
    uint32 smartbombs;
    uint32 lives;
};

struct PlayerSaveRecord
{
    uint32 checksum;
    PlayerSaveProgress progress;
    SavedGameSettings settings;
};

struct PlayerSaveStorage
{
    PlayerSaveRecord current;
    PlayerSaveRecord selected;
    PlayerSaveRecord replay_backup;
};

extern PlayerSaveStorage player_saves;
void ResetPlayerSaveStorage();
void ResetPlayerSaveTemplates();
const PlayerSaveRecord &GetDefaultPlayerSave();
PlayerSaveRecord &GetDemoPlayerSave(uint32 index);
uint32 CaptureCurrentPlayerSave();
void DecodePlayerSave(PlayerSaveRecord &record, const uint8 *data, uint32 bytes);
void EncodePlayerSave(uint8 *data, uint32 bytes, const PlayerSaveRecord &record);
uint32 CalculatePlayerSaveChecksum(const PlayerSaveRecord &record);
uint32 RestorePlayerInventory(PlayerSaveRecord &record);

#endif

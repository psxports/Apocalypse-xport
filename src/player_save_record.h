#ifndef PLAYER_SAVE_RECORD_H
#define PLAYER_SAVE_RECORD_H

#include "psx.h"
#include "player_save_progress.h"
#include "game_settings.h"

struct PlayerSaveRecord
{
    uint32 checksum;
    PlayerSaveProgress progress;
    SavedGameSettings settings;

public:
    uint32 CalculateChecksum() const;
    uint8 RestoreInventory();
    void LoadLevel();
};

#endif

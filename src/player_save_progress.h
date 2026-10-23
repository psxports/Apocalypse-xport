#ifndef PLAYER_SAVE_PROGRESS_H
#define PLAYER_SAVE_PROGRESS_H

#include "psx.h"

struct PlayerSaveProgress
{
    char level[9];
    char checkpoint[53];
    uint8 weapon_mask;
    uint8 sel_weapon;
    uint32 ammo[8];
    sint32 health;
    uint32 smartbombs;
    uint32 lives;

public:
    void SetLevel(const char *name);
    void SetCheckpoint(const char *name);
};

#endif

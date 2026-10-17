#ifndef CHEAT_INPUT_H
#define CHEAT_INPUT_H

extern "C"
{
#include "psx.h"
}

struct CheatOptions
{
    uint32 weapon_cheat;
    uint32 invulnerable;
    uint32 unlimited_lives;
    uint32 status_hud;
    uint32 enemy_finish_cheat;
    uint32 free_movement;
    uint32 level_select;
    uint32 autotest;
    uint32 level_skip;
    uint32 debug_panel;
    uint32 suspend_resource_loading;
    uint32 war_record_spawn;
    uint32 build_date;
};

CheatOptions &GetCheatOptions();
void ClearCheatInputBuffer();
uint32 UpdateCheatInput();

#endif

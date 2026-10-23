#ifndef LEVEL_SESSION_H
#define LEVEL_SESSION_H

extern "C"
{
#include "psx.h"
}

#include "game_settings.h"

struct GameSessionState
{
    VibrationMode saved_vibration;
    uint8 demo_idx;
    uint32 exit_reason;
    // TODO Trace the original producer of the demo display and catch-up gate
    uint32 demo_mode;
    bool movie_any_button_skip;
    bool demo_next;
    uint8 catchup_blocked;
    sint32 weapon_activity;
};

extern GameSessionState game_session;
void ResetGameSession();

struct LevelStatistics
{
    uint32 enemy_total;
    uint32 enemy_kills;
};

struct CompletionProgress
{
    bool completed;
    uint32 blink_ticks;
    uint32 target_percent;
    uint32 disp_percent;
    uint32 voices;
};

struct GameProgress
{
    LevelStatistics level;
    CompletionProgress completion;
    uint32 lives;
};

extern GameProgress game_progress;
void ResetGameProgress();

struct LevelEnvironment
{
    uint32 area_kind;
    sint32 water_y;
    uint32 water_damage;
};

extern LevelEnvironment level_environment;
void ResetLevelEnvironment();
const char *GetLevelTestCaption();
void RunGameSession();
void ReleaseLevelLoadResources();
const char *GetCurLevelName();
void ResetCurLevelName();

uint32 CompleteLevel(bool completed);

#endif

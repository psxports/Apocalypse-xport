#ifndef LEVEL_SESSION_H
#define LEVEL_SESSION_H

extern "C"
{
#include "psx.h"
}

struct GameSessionState
{
    uint32 saved_vibration;
    uint8 demo_index;
    uint32 exit_reason;
    // TODO Trace the original producer of the demo display and catch-up gate
    uint32 demo_mode;
    uint32 movie_any_button_skip;
    uint8 demo_next;
    uint8 catchup_blocked;
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
    uint32 completed;
    uint32 blink_ticks;
    uint32 target_percent;
    uint32 display_percent;
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
const char *GetCurrentLevelName();
void ResetCurrentLevelName();

uint32 CompleteLevel(uint32 completed);

#endif

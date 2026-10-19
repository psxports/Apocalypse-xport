#ifndef GAME_TICK_H
#define GAME_TICK_H

extern "C"
{
#include "psx.h"
}

struct GameClock
{
    uint32 tick;
    uint32 step;
    uint32 elapsed;
    uint32 active_frame;
    uint32 vblank_frame;
};

extern GameClock game_clock;
void ResetGameClock();
uint32 AdvanceGameTick(void);

#endif

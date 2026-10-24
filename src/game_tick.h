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
#if defined(_DEBUG)
struct SceneTimingCounters
{
    bool active;
    uint64 started_us, sampled_us;
    uint32 logic_calls, logic_ticks, pad_polls;
    uint32 vblank_callbacks, music_callbacks, service_calls;
    uint32 source_frames, hd_frames, hd_redraws;
    uint64 source_prepare_us;
};
extern SceneTimingCounters scene_timing;
#endif
void ResetGameClock();
uint32 AdvanceGameTick();

#endif

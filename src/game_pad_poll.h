#ifndef GAME_PAD_POLL_H
#define GAME_PAD_POLL_H
#include "psx.h"
#ifdef __cplusplus
struct PadButton
{
    PadButton() : down(0), pressed(0), held(0), released(0), elapsed(0)
    {
    }

    uint8 down;
    uint8 pressed;
    uint32 held;
    uint32 released;
    uint32 elapsed;
    uint32 Update(uint32 value);
    uint32 Record(uint32 value, uint32 index);
    uint32 Replay(uint32 index);
};

struct GamePadInput
{
    GamePadInput();
    PadButton buttons[20];
    uint32 digital_masks[4];
    uint32 analog_masks[4];
    uint8 axis_map[4];
    uint8 raw_axis[4];
    uint8 axis[4];
    uint32 type;
    uint32 reset_ticks;
    uint8 actuators[2];
    uint8 actuator_ready;
    uint16 actuator_ticks[2];
    uint32 Reset();
    uint32 ClearPresses();
    uint32 SetButtonMasks(uint32 mask0, uint32 mask1, uint32 mask2, uint32 mask3);
    uint32 SetAxes(uint32 axis0, uint32 axis1, uint32 axis2, uint32 axis3, uint32 mask0, uint32 mask1, uint32 mask2, uint32 mask3);
};

struct PadCalibration
{
    PadCalibration() : min(0), max(255), centre(128)
    {
    }

    uint32 min;
    uint32 max;
    uint32 centre;
};

extern PadCalibration pad_calibration[4];

struct PadPolling
{
    uint32 idle_ticks;
    uint32 frame;
    uint32 prev_axes[8];
};

extern GamePadInput game_pads[2];
extern PadPolling pad_polling;
void ResetGamePadState();
void ClearGamePadActuator(uint32 port, uint32 actuator);
uint32 InitializeGamePads();
uint32 SetupGamePadInput();
void SetGamePadActuator(uint32 controller, uint32 value, uint32 actuator, uint32 enabled);
uint32 PollPadPackets();
#endif
#endif

#ifndef GAME_PAD_INPUT_H
#define GAME_PAD_INPUT_H

#include "pad_button.h"
#include "psx.h"

#ifdef __cplusplus

struct GamePadInput
{
    PadButton buttons[20];
    uint32 digital_masks[4];
    uint32 analog_masks[4];
    uint8 axis_map[4];
    uint8 raw_axis[4];
    // Replay stores neutral 128 while live motion reads signed low bytes
    uint8 axis[4];
    uint8 type;
    uint32 reset_ticks;
    uint8 actuators[2];
    bool actuator_ready;
    uint16 actuator_ticks[2];

public:
    uint32 SetButtonMasks(uint32 mask0, uint32 mask1, uint32 mask2, uint32 mask3);
    uint32 SetAxes(uint8 axis0, uint8 axis1, uint8 axis2, uint32 axis3, uint32 mask0, uint32 mask1, uint32 mask2, uint32 mask3);
    static uint32 Setup();
    static uint32 Initialize();
    uint32 Reset();
    uint32 ClearPresses();
    static void SetActuator(uint8 pad_idx, uint16 ticks, uint8 actuator, uint8 value);
    static void ClearActuator(uint8 port, uint8 actuator);

private:
    static void NormalizePackets();
    static void CopyPacket(uint8 *dst, const uint8 *src);
    static sint32 ScaleAxis(sint32 raw, sint32 minimum, sint32 maximum, sint32 centre);

public:
    static uint32 Poll(void);
    GamePadInput();
    static void ResetAll();

private:
    static uint16 ReadPacketButtons(const uint8 *packet);
    static void RestartCommunication();
};

struct PadCalibration
{
    PadCalibration() : min(0), max(255), centre(128)
    {
    }

    sint32 min;
    sint32 max;
    sint32 centre;
};

extern PadCalibration pad_calibration[4];

struct PadPolling
{
    uint32 idle_ticks;
    uint32 frame;
    uint8 prev_axes[8];
};

extern GamePadInput game_pads[2];
extern PadPolling pad_polling;

#endif
#endif

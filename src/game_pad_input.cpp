#include <stdlib.h>
#include <string.h>

#include "level_session.h"
#include "input_recording.h"
#include "game_tick.h"
#include "resource_io.h"
#include "game_pad_input.h"
#include "game_diagnostics.h"
#include "game_settings.h"
#include "global.h"

GamePadInput game_pads[2];
PadCalibration pad_calibration[4];
static uint8 special_buttons[256];
static uint8 raw_packets[2][34];
static uint8 packets[2][4][8];
static uint8 actuator_alignment[6] = {0, 1, 255, 255, 255, 255};
PadPolling pad_polling = {0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};

void GamePadInput::ResetAll()
{
    PadStopCom();
    memset(raw_packets, 0, sizeof(raw_packets));
    memset(packets, 0, sizeof(packets));
    psx_pad_bind_owner_packets(0, 0, 0, 0, 0, 0);
    for (uint32 port = 0; port < 2; ++port)
        game_pads[port] = GamePadInput();
    for (uint32 idx = 0; idx < 256; ++idx)
        special_buttons[idx] = 0;
    game_settings.bindings = ControlBindings();
    for (uint32 axis = 0; axis < 4; ++axis)
        pad_calibration[axis] = PadCalibration();
    pad_polling.idle_ticks = pad_polling.frame = 0;
    for (uint32 idx = 0; idx < 8; ++idx)
        pad_polling.prev_axes[idx] = 0;
}

uint32 GamePadInput::SetButtonMasks(uint32 mask0, uint32 mask1, uint32 mask2, uint32 mask3)
{
    FUNCTION_MARKER(0x8006FDFC, "SLUS_003.73");
    digital_masks[0] = mask0;
    digital_masks[1] = mask1;
    digital_masks[2] = mask2;
    digital_masks[3] = mask3;
    return mask3;
}

uint16 GamePadInput::ReadPacketButtons(const uint8 *packet)
{
    uint8 type = packet[1], high, low;
    if (type == 83)
    {
        uint8 idx = packet[3];
        high = packet[2];
        low = special_buttons[idx];
    }
    else if (type == 65 || type == 115)
    {
        high = packet[2];
        low = packet[3];
    }
    else
        return 0;
    return ~((high << 8) | low);
}

void GamePadInput::RestartCommunication()
{
    PadStopCom();
    PadStartCom();
}

uint32 GamePadInput::SetAxes(uint8 axis0, uint8 axis1, uint8 axis2, uint32 axis3, uint32 mask0, uint32 mask1, uint32 mask2, uint32 mask3)
{
    FUNCTION_MARKER(0x8006FE14, "SLUS_003.73");
    axis_map[0] = axis0;
    axis_map[1] = axis1;
    axis_map[2] = axis2;
    axis_map[3] = axis3;
    analog_masks[0] = mask0;
    analog_masks[1] = mask1;
    analog_masks[2] = mask2;
    analog_masks[3] = mask3;
    return axis3;
}

uint32 GamePadInput::Setup()
{
    uint32 port = 0;
    uint32 count = 0;
    uint32 idx;
    FUNCTION_MARKER(0x8006FE4C, "SLUS_003.73");
    do
    {
        uint32 mask0 = game_settings.bindings.digital[0];
        uint32 mask1 = game_settings.bindings.digital[1];
        uint32 mask2 = game_settings.bindings.digital[2];
        uint32 mask3 = game_settings.bindings.digital[3];

        ++count;
        game_pads[port].SetButtonMasks(mask0, mask1, mask2, mask3);
        mask0 = game_settings.bindings.analog[0];
        mask1 = game_settings.bindings.analog[1];
        mask2 = game_settings.bindings.digital[2];
        mask3 = game_settings.bindings.digital[3];
        ++port;
        game_pads[port - 1].SetAxes(3, 2, 1, 0, mask0, mask1, mask2, mask3);
    } while (count < 2);
    for (idx = 0; idx < 256; ++idx)
    {
        uint8 val = 0;
        if (idx & 1)
            val |= 1;
        if (idx & 2)
            val |= 4;
        if (idx & 4)
            val |= 0x80;
        if (idx & 8)
            val |= 0x10;
        if (idx & 0x10)
            val |= 8;
        if (idx & 0x20)
            val |= 0x20;
        if (idx & 0x40)
            val |= 0x40;
        if (idx & 0x80)
            val |= 2;
        special_buttons[idx] = val;
    }
    GamePadInput::Initialize();
    PadInit(0);
    PadInitDirect(raw_packets[0], raw_packets[1]);
    PadStartCom();
    pad_polling.idle_ticks = 0;
    return 1;
}

uint32 GamePadInput::Initialize()
{
    FUNCTION_MARKER(0x8007001C, "SLUS_003.73");
    for (uint32 port = 0; port < 2; ++port)
    {
        game_pads[port].Reset();
        game_pads[port].reset_ticks = 0;
    }
    pad_polling.frame = 0;
    input_recording.ResetCursor();
    return 0;
}

uint32 GamePadInput::Reset()
{
    FUNCTION_MARKER(0x800700B0, "SLUS_003.73");
    for (uint32 idx = 0; idx < 20; ++idx)
        buttons[idx] = PadButton();
    // Original initialization always addresses the first port axes
    for (uint32 idx = 0; idx < 4; ++idx)
        game_pads[0].axis[idx] = 128;
    return 128;
}

uint32 GamePadInput::ClearPresses()
{
    FUNCTION_MARKER(0x80070100, "SLUS_003.73");
    for (uint32 idx = 0; idx < 20; ++idx)
        buttons[idx].pressed = false;
    return 0xFFFFFFFF;
}

void GamePadInput::SetActuator(uint8 pad_idx, uint16 ticks, uint8 actuator, uint8 value)
{
    FUNCTION_MARKER(0x8007011C, "SLUS_003.73");
    InputRecording::Mode mode = input_recording.GetMode();
    if (mode)
        return;
    uint32 port = pad_idx ? 16 : 0;
    uint32 state = PadGetState(port);
    if (!state)
        return;
    if (pad_idx >= 2 || actuator >= 2)
    {
        GameDiagnostic(("Invalid game pad actuator index\n"));
        abort();
    }
    GamePadInput &pad = game_pads[pad_idx];
    if (state == 1)
        pad.actuator_ready = false;
    if (!pad.actuator_ready)
    {
        PadSetAct(port, pad.actuators, 2);
        if (state == 2 || (state == 6 && PadSetActAlign(port, actuator_alignment)))
            pad.actuator_ready = true;
    }
    pad.actuators[actuator] = value;
    pad.actuator_ticks[actuator] = ticks;
}

void GamePadInput::ClearActuator(uint8 port, uint8 actuator)
{
    FUNCTION_MARKER(0x80070288, "SLUS_003.73");
    InputRecording::Mode mode = input_recording.GetMode();
    if (!mode)
    {
        if (port >= 2 || actuator >= 2)
        {
            GameDiagnostic(("Invalid game pad actuator index\n"));
            abort();
        }
        game_pads[port].actuators[actuator] = 0;
        game_pads[port].actuator_ticks[actuator] = 0;
    }
}

GamePadInput::GamePadInput() : type(0), reset_ticks(0), actuator_ready(false)
{
    for (uint32 idx = 0; idx < 4; ++idx)
    {
        digital_masks[idx] = analog_masks[idx] = 0;
        axis_map[idx] = raw_axis[idx] = axis[idx] = 0;
    }
    for (uint32 idx = 0; idx < 2; ++idx)
    {
        actuators[idx] = 0;
        actuator_ticks[idx] = 0;
    }
}

void GamePadInput::NormalizePackets()
{
    FUNCTION_MARKER(0x8007047C, "SLUS_003.73");
    for (uint32 port = 0; port < 2; ++port)
    {
        const uint8 *src = raw_packets[port];
        if (src[1] == 128)
        {
            for (uint32 slot = 0; slot < 4; ++slot)
                GamePadInput::CopyPacket(packets[port][slot], src + 2 + 8 * slot);
        }
        else
        {
            GamePadInput::CopyPacket(packets[port][0], src);
            for (uint32 slot = 1; slot < 4; ++slot)
                packets[port][slot][1] = 0;
        }
    }
}

void GamePadInput::CopyPacket(uint8 *dst, const uint8 *src)
{
    FUNCTION_MARKER(0x800705F8, "SLUS_003.73");
    uint8 status = src[0];
    uint8 prev_type = dst[1];
    dst[0] = status;
    for (uint32 idx = 1; idx < 8; ++idx)
        dst[idx] = src[idx];
    if (src[0] == 255)
        dst[1] = 0;
    else if (prev_type != dst[1])
    {
        GamePadInput::RestartCommunication();
        game_pads[0].actuator_ready = false;
    }
}

sint32 GamePadInput::ScaleAxis(sint32 raw, sint32 minimum, sint32 maximum, sint32 centre)
{
    sint32 distance;
    FUNCTION_MARKER(0x800706B0, "SLUS_003.73");
    if (raw < minimum)
        return -127;
    if (maximum < raw)
        return 127;
    if (raw < centre - 32)
    {
        pad_polling.idle_ticks = 0;
        distance = centre - raw - 32;
        return DivideSignedWord(-127 * distance, centre - minimum - 32);
    }
    if (centre + 32 < raw)
    {
        pad_polling.idle_ticks = 0;
        distance = raw - 32 - centre;
        return DivideSignedWord(127 * distance, maximum - 32 - centre);
    }
    return 0;
}

uint32 GamePadInput::Poll(void)
{
    FUNCTION_MARKER(0x80070748, "SLUS_003.73");
#if defined(_DEBUG)
    if (scene_timing.active)
        ++scene_timing.pad_polls;
#endif
    /* Service native elapsed VBlank time while the game polls hardware input */
    VSync(-1);
    static const uint32 masks[16] = {16, 128, 32, 64, 4, 1, 8, 2, 32768, 8192, 4096, 16384, 512, 1024, 2048, 256};
    uint32 port, i, buttons0, buttons1, buttons;
    uint8 type0, type1;
    bool unchanged = true;
    InputRecording::Mode mode;
    pad_polling.frame = pad_polling.frame + 1;
    for (port = 0; port < 2; ++port)
        for (i = 0; i < 2; ++i)
        {
            uint16 timer = game_pads[port].actuator_ticks[i];
            if (timer)
            {
                game_pads[port].actuator_ticks[i] = timer - 1;
                if (timer == 1)
                    GamePadInput::ClearActuator(port, i);
            }
        }
    GamePadInput::NormalizePackets();
    buttons0 = GamePadInput::ReadPacketButtons(packets[0][0]);
    buttons1 = GamePadInput::ReadPacketButtons(packets[1][0]);
    buttons = (buttons1 << 16) | buttons0;
    type0 = packets[0][0][1];
    type1 = packets[1][0][1];
    game_pads[1].type = type1;
    game_pads[0].type = type0;
    const uint32 *action_masks = (type0 == 115 || type0 == 83) ? game_pads[0].analog_masks : game_pads[0].digital_masks;
    mode = input_recording.GetMode();
    if (mode)
    {
        if (buttons)
            pad_polling.idle_ticks = 0;
    }
    else
    {
        uint8 axes[8];
        for (i = 0; i < 8; ++i)
        {
            axes[i] = packets[i / 4][0][4 + (i & 3)] & 252;
        }
        if (buttons)
            unchanged = false;
        else
            for (i = 0; i < 8; ++i)
                if (axes[i] != pad_polling.prev_axes[i])
                {
                    unchanged = false;
                    break;
                }
        if (unchanged)
            pad_polling.idle_ticks = pad_polling.idle_ticks + 1;
        else
            for (i = 0; i < 8; ++i)
                pad_polling.prev_axes[i] = axes[i];
        if ((buttons1 & 0x900) == 0x900)
            game_pads[1].reset_ticks = game_pads[1].reset_ticks + 1;
        else
            game_pads[1].reset_ticks = 0;
        if (game_pads[0].reset_ticks == 120 || game_pads[1].reset_ticks == 120)
            return 1;
    }
    mode = input_recording.GetMode();
    if (mode == InputRecording::Replay)
    {
        if (buttons0)
        {
            game_session.exit_reason = 5;
            return 0;
        }
        for (i = 0; i < 4; ++i)
        {
            game_pads[0].raw_axis[i] = 128;
            game_pads[0].axis[i] = 128;
        }
        game_pads[0].type = 65;
        for (i = 0; i < 20; ++i)
        {
            game_pads[0].buttons[i].Replay(i);
            if (input_recording.AtEnd())
            {
                game_session.exit_reason = 4;
                return 0;
            }
        }
        return 0;
    }
    if (mode != InputRecording::Live && mode != InputRecording::Recording)
        return 0;
    if (!mode)
    {
        type0 = game_pads[0].type;
    }
    if (!mode && (type0 == 115 || type0 == 83))
    {
        for (i = 0; i < 4; ++i)
        {
            game_pads[0].raw_axis[i] = packets[0][0][4 + game_pads[0].axis_map[i]];
            game_pads[1].raw_axis[i] = packets[1][0][4 + game_pads[1].axis_map[i]];
        }
        sint32 scaled = 0;
        for (i = 0; i < 4; ++i)
        {
            uint8 raw = game_pads[0].raw_axis[3 - i];
            const PadCalibration &limits = pad_calibration[3 - i];
            if (i)
                game_pads[0].axis[4 - i] = scaled;
            scaled = GamePadInput::ScaleAxis(raw, limits.min, limits.max, limits.centre);
        }
        game_pads[0].axis[0] = scaled;
    }
    for (i = 0; i < 16; ++i)
    {
        if (mode)
            game_pads[0].buttons[i].Record(buttons & masks[i], i);
        else
            game_pads[0].buttons[i].Update(buttons & masks[i]);
    }
    for (i = 0; i < 4; ++i)
    {
        if (mode)
            game_pads[0].buttons[i + 16].Record(buttons & action_masks[i], i + 16);
        else
            game_pads[0].buttons[i + 16].Update(buttons & action_masks[i]);
    }
    for (i = 0; i < 16; ++i)
        game_pads[1].buttons[i].Update(buttons1 & masks[i]);
    return 0;
}

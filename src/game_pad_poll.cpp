#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "level_session.h"
#include "input_recording.h"
#include "game_tick.h"
#include "resource_io.h"
#include "game_pad_poll.h"
#include "game_diagnostics.h"
#include "game_settings.h"

GamePadInput game_pads[2];
PadCalibration pad_calibration[4];
static uint8 special_buttons[256];
static uint8 raw_packets[2][34];
static uint8 packets[2][4][8];
static uint8 actuator_alignment[6] = {0, 1, 255, 255, 255, 255};
PadPolling pad_polling = {0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};

void ResetGamePadState()
{
    PadStopCom();
    memset(raw_packets, 0, sizeof(raw_packets));
    memset(packets, 0, sizeof(packets));
    psx_pad_bind_owner_packets(0, 0, 0, 0, 0, 0);
    for (uint32 port = 0; port < 2; ++port)
        game_pads[port] = GamePadInput();
    for (uint32 index = 0; index < 256; ++index)
        special_buttons[index] = 0;
    game_settings.bindings = ControlBindings();
    for (uint32 axis = 0; axis < 4; ++axis)
        pad_calibration[axis] = PadCalibration();
    pad_polling.idle_ticks = pad_polling.frame = 0;
    for (uint32 index = 0; index < 8; ++index)
        pad_polling.prev_axes[index] = 0;
}

GamePadInput::GamePadInput() : type(0), reset_ticks(0), actuator_ready(0)
{
    for (uint32 index = 0; index < 4; ++index)
    {
        digital_masks[index] = analog_masks[index] = 0;
        axis_map[index] = raw_axis[index] = axis[index] = 0;
    }
    for (uint32 index = 0; index < 2; ++index)
    {
        actuators[index] = 0;
        actuator_ticks[index] = 0;
    }
}

uint32 PadButton::Record(uint32 value, uint32 index)
{
    FUNCTION_MARKER(0x80070354, "SLUS_003.73");
    uint32 new_down = value != 0;
    ++elapsed;
    if (!down && new_down)
    {
        pressed = 1;
        elapsed = 0;
    }
    if (down != new_down)
        input_recording.Append(game_clock.elapsed, (uint16)pad_polling.frame, index, new_down);
    down = new_down;
    if (down)
    {
        released = 0;
        return ++held;
    }
    held = 0;
    return ++released;
}

uint32 PadButton::Replay(uint32 index)
{
    FUNCTION_MARKER(0x80070410, "SLUS_003.73");
    uint32 frame = pad_polling.frame;
    uint32 result = (sint32)frame < (sint32)input_recording.NextFrame();
    if (result)
        return result;
    RecordedInputEvent event = input_recording.ReadEvent();
    if (event.button != index)
        return event.button;
    Update(event.down);
    input_recording.AdvanceReplay(event.timestamp);
    return 1;
}

static sint32 DividePadWord(sint32 n, sint32 d)
{
    if (!d)
        return n < 0 ? 1 : -1;
    if (n == INT_MIN && d == -1)
        return INT_MIN;
    return n / d;
}

static sint32 ScalePadAxis(uint32 raw, uint32 minimum, uint32 maximum, uint32 centre)
{
    sint32 value = (sint32)raw, low = (sint32)minimum, high = (sint32)maximum;
    uint32 distance, numerator, denominator;
    FUNCTION_MARKER(0x800706B0, "SLUS_003.73");
    if (value < low)
        return -127;
    if (high < value)
        return 127;
    if (value < (sint32)(centre - 32))
    {
        pad_polling.idle_ticks = 0;
        distance = centre - (raw + 32);
        numerator = distance - (distance << 7);
        denominator = centre - (minimum + 32);
        return DividePadWord((sint32)numerator, (sint32)denominator);
    }
    if ((sint32)(centre + 32) < value)
    {
        pad_polling.idle_ticks = 0;
        distance = raw - 32 - centre;
        numerator = (distance << 7) - distance;
        denominator = maximum - 32 - centre;
        return DividePadWord((sint32)numerator, (sint32)denominator);
    }
    return 0;
}

static uint16 ReadPacketButtons(const uint8 *packet)
{
    uint32 type = packet[1], high, low;
    if (type == 83)
    {
        uint32 index = packet[3];
        high = packet[2];
        low = special_buttons[index];
    }
    else if (type == 65 || type == 115)
    {
        high = packet[2];
        low = packet[3];
    }
    else
        return 0;
    return (uint16) ~((high << 8) | low);
}

static void RestartPadCommunication()
{
    PadStopCom();
    PadStartCom();
}

static void CopyGamePadPacket(uint8 *dst, const uint8 *src)
{
    FUNCTION_MARKER(0x800705F8, "SLUS_003.73");
    uint8 first = src[0];
    uint8 prev_type = dst[1];
    dst[0] = first;
    for (uint32 idx = 1; idx < 8; ++idx)
        dst[idx] = src[idx];
    if (src[0] == 255)
        dst[1] = 0;
    else if (prev_type != dst[1])
    {
        RestartPadCommunication();
        game_pads[0].actuator_ready = 0;
    }
}

static void NormalizePadPackets()
{
    FUNCTION_MARKER(0x8007047C, "SLUS_003.73");
    for (uint32 port = 0; port < 2; ++port)
    {
        const uint8 *src = raw_packets[port];
        if (src[1] == 128)
        {
            for (uint32 slot = 0; slot < 4; ++slot)
                CopyGamePadPacket(packets[port][slot], src + 2 + 8 * slot);
        }
        else
        {
            CopyGamePadPacket(packets[port][0], src);
            for (uint32 slot = 1; slot < 4; ++slot)
                packets[port][slot][1] = 0;
        }
    }
}

uint32 PollPadPackets(void)
{
    FUNCTION_MARKER(0x80070748, "SLUS_003.73");
    /* Service native elapsed VBlank time while the game polls hardware input */
    VSync(-1);
    static const uint32 masks[16] = {16, 128, 32, 64, 4, 1, 8, 2, 32768, 8192, 4096, 16384, 512, 1024, 2048, 256};
    uint32 port, i, timer, first, second, buttons, mode, unchanged = 1, first_type, second_type;
    pad_polling.frame = pad_polling.frame + 1;
    for (port = 0; port < 2; ++port)
        for (i = 0; i < 2; ++i)
        {
            timer = game_pads[port].actuator_ticks[i];
            if (timer)
            {
                game_pads[port].actuator_ticks[i] = timer - 1;
                if ((uint16)(timer - 1) == 0)
                    ClearGamePadActuator(port, i);
            }
        }
    NormalizePadPackets();
    first = ReadPacketButtons(packets[0][0]);
    second = ReadPacketButtons(packets[1][0]);
    buttons = (second << 16) | first;
    first_type = packets[0][0][1];
    second_type = packets[1][0][1];
    game_pads[1].type = second_type;
    game_pads[0].type = first_type;
    const uint32 *action_masks = (first_type == 115 || first_type == 83) ? game_pads[0].analog_masks : game_pads[0].digital_masks;
    mode = input_recording.GetMode();
    if (mode)
    {
        if (buttons)
            pad_polling.idle_ticks = 0;
    }
    else
    {
        uint32 axes[8];
        for (i = 0; i < 8; ++i)
        {
            axes[i] = packets[i / 4][0][4 + (i & 3)] & 252;
        }
        if (buttons)
            unchanged = 0;
        else
            for (i = 0; i < 8; ++i)
                if (axes[i] != pad_polling.prev_axes[i])
                {
                    unchanged = 0;
                    break;
                }
        if (unchanged)
            pad_polling.idle_ticks = pad_polling.idle_ticks + 1;
        else
            for (i = 0; i < 8; ++i)
                pad_polling.prev_axes[i] = axes[i];
        if ((second & 0x900) == 0x900)
            game_pads[1].reset_ticks = game_pads[1].reset_ticks + 1;
        else
            game_pads[1].reset_ticks = 0;
        if (game_pads[0].reset_ticks == 120 || game_pads[1].reset_ticks == 120)
            return 1;
    }
    mode = input_recording.GetMode();
    if (mode == InputRecording::Replay)
    {
        if (first)
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
        first_type = game_pads[0].type;
    }
    if (!mode && (first_type == 115 || first_type == 83))
    {
        for (i = 0; i < 4; ++i)
        {
            game_pads[0].raw_axis[i] = packets[0][0][4 + game_pads[0].axis_map[i]];
            game_pads[1].raw_axis[i] = packets[1][0][4 + game_pads[1].axis_map[i]];
        }
        sint32 scaled = 0;
        for (i = 0; i < 4; ++i)
        {
            uint32 raw = game_pads[0].raw_axis[3 - i];
            const PadCalibration &limits = pad_calibration[3 - i];
            if (i)
                game_pads[0].axis[4 - i] = scaled;
            scaled = ScalePadAxis(raw, limits.min, limits.max, limits.centre);
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
        game_pads[1].buttons[i].Update(second & masks[i]);
    return 0;
}

uint32 PadButton::Update(uint32 value)
{
    FUNCTION_MARKER(0x800702E0, "SLUS_003.73");
    ++elapsed;
    if (!down && value)
    {
        pressed = 1;
        elapsed = 0;
    }
    if (value)
    {
        pad_polling.idle_ticks = 0;
        down = 1;
    }
    else
        down = 0;
    if (down)
    {
        released = 0;
        return ++held;
    }
    held = 0;
    return ++released;
}

void ClearGamePadActuator(uint32 port, uint32 button)
{
    FUNCTION_MARKER(0x80070288, "SLUS_003.73");
    uint32 mode = input_recording.GetMode();
    if (!mode)
    {
        port &= 255;
        button &= 255;
        if (port >= 2 || button >= 2)
        {
            GameDiagnostic("Invalid game pad actuator index\n");
            abort();
        }
        game_pads[port].actuators[button] = 0;
        game_pads[port].actuator_ticks[button] = 0;
    }
}

uint32 GamePadInput::Reset()
{
    FUNCTION_MARKER(0x800700B0, "SLUS_003.73");
    for (uint32 index = 0; index < 20; ++index)
        buttons[index] = PadButton();
    // Original initialization always addresses the first port axes
    for (uint32 index = 0; index < 4; ++index)
        game_pads[0].axis[index] = 128;
    return 128;
}

uint32 GamePadInput::ClearPresses()
{
    FUNCTION_MARKER(0x80070100, "SLUS_003.73");
    for (uint32 index = 0; index < 20; ++index)
        buttons[index].pressed = 0;
    return 0xFFFFFFFF;
}

uint32 InitializeGamePads()
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

void SetGamePadActuator(uint32 controller, uint32 value, uint32 actuator, uint32 enabled)
{
    FUNCTION_MARKER(0x8007011C, "SLUS_003.73");
    uint32 mode = input_recording.GetMode();
    if (mode)
        return;
    uint32 index = controller & 255;
    uint32 port = index ? 16 : 0;
    uint32 state = PadGetState(port);
    if (!state)
        return;
    actuator &= 255;
    if (index >= 2 || actuator >= 2)
    {
        GameDiagnostic("Invalid game pad actuator index\n");
        abort();
    }
    GamePadInput &pad = game_pads[index];
    if (state == 1)
        pad.actuator_ready = 0;
    if (!pad.actuator_ready)
    {
        PadSetAct(port, pad.actuators, 2);
        if (state == 2 || (state == 6 && PadSetActAlign(port, actuator_alignment)))
            pad.actuator_ready = 1;
    }
    pad.actuators[actuator] = enabled;
    pad.actuator_ticks[actuator] = value;
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

uint32 GamePadInput::SetAxes(uint32 axis0, uint32 axis1, uint32 axis2, uint32 axis3, uint32 mask0, uint32 mask1, uint32 mask2, uint32 mask3)
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

uint32 SetupGamePadInput()
{
    uint32 port = 0;
    uint32 count = 0;
    uint32 index;
    FUNCTION_MARKER(0x8006FE4C, "SLUS_003.73");
    do
    {
        uint32 x = game_settings.bindings.digital[0];
        uint32 y = game_settings.bindings.digital[1];
        uint32 width = game_settings.bindings.digital[2];
        uint32 height = game_settings.bindings.digital[3];

        ++count;
        game_pads[port].SetButtonMasks(x, y, width, height);
        x = game_settings.bindings.analog[0];
        y = game_settings.bindings.analog[1];
        width = game_settings.bindings.digital[2];
        height = game_settings.bindings.digital[3];
        ++port;
        game_pads[port - 1].SetAxes(3, 2, 1, 0, x, y, width, height);
    } while (count < 2);
    for (index = 0; index < 256; ++index)
    {
        uint8 value = 0;
        if (index & 1)
            value |= 1;
        if (index & 2)
            value |= 4;
        if (index & 4)
            value |= 0x80;
        if (index & 8)
            value |= 0x10;
        if (index & 0x10)
            value |= 8;
        if (index & 0x20)
            value |= 0x20;
        if (index & 0x40)
            value |= 0x40;
        if (index & 0x80)
            value |= 2;
        special_buttons[index] = value;
    }
    InitializeGamePads();
    PadInit(0);
    PadInitDirect(raw_packets[0], raw_packets[1]);
    PadStartCom();
    pad_polling.idle_ticks = 0;
    return 1;
}

#include "resource_io.h"
#include "game_pad_poll.h"
extern "C"
{
#include "game_pad_startup.h"
#include "draft_first_signatures.h"
}
#include <limits.h>
#include "native_object_storage.h"

static uint32 ClearGamePadButton(uint32 port, uint32 button);

static uint32 RecordButton(uint32 state, uint32 pressed, uint32 index)
{
    uint32 old, down = pressed != 0, record, frame, cursor, result;
    FUNCTION_MARKER(0x80070354, "SLUS_003.73");
    result = r_u32(state + 12) + 1;
    w_u32(state + 12, result);
    old = r_u8(state);
    if (!old)
    {
        if (down)
        {
            w_u8(state + 1, 1);
            w_u32(state + 12, 0);
        }
        old = r_u8(state);
    }
    if (old != down)
    {
        record = r_u32(0x800FF820);
        frame = r_u16(0x800FF830);
        w_u8(record + 6, index);
        cursor = r_u32(0x800FF820);
        w_u16(record + 4, frame);
        w_u8(cursor + 7, down);
        cursor = r_u32(0x800FF820);
        frame = r_u32(0x800FF38C);
        w_u32(cursor, frame);
        cursor += 8;
        w_u32(0x800FF820, cursor);
        w_u16(cursor + 4, 65535);
    }
    w_u8(state, down);
    if (down)
    {
        result = r_u32(state + 4);
        w_u32(state + 8, 0);
        ++result;
        w_u32(state + 4, result);
        return result;
    }
    result = r_u32(state + 8);
    w_u32(state + 4, 0);
    ++result;
    w_u32(state + 8, result);
    return result;
}

static uint32 ReplayButton(uint32 state, uint32 index)
{
    uint32 record, frame, result, seed;
    FUNCTION_MARKER(0x80070410, "SLUS_003.73");
    record = r_u32(0x800FF820);
    frame = r_u32(0x800FF830);
    result = (sint32)frame < (sint32)r_u16(record + 4);
    if (result)
        return result;
    result = r_u8(record + 6);
    if (result != index)
        return result;
    UpdateButtonState(state, r_u8(record + 7));
    record = r_u32(0x800FF820);
    seed = r_u32(record);
    w_u32(0x800FF820, record + 8);
    w_u32(0x800FF390, seed);
    return record + 8;
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
        w_u32(0x800FF814, 0);
        distance = centre - (raw + 32);
        numerator = distance - (distance << 7);
        denominator = centre - (minimum + 32);
        return DividePadWord((sint32)numerator, (sint32)denominator);
    }
    if ((sint32)(centre + 32) < value)
    {
        w_u32(0x800FF814, 0);
        distance = raw - 32 - centre;
        numerator = (distance << 7) - distance;
        denominator = maximum - 32 - centre;
        return DividePadWord((sint32)numerator, (sint32)denominator);
    }
    return 0;
}

static uint16 ReadPacketButtons(uint32 packet)
{
    uint32 type = r_u8(packet + 1), high, low;
    if (type == 83)
    {
        uint32 index = r_u8(packet + 3);
        high = r_u8(packet + 2);
        low = r_u8(0x801027A8 + index);
    }
    else if (type == 65 || type == 115)
    {
        high = r_u8(packet + 2);
        low = r_u8(packet + 3);
    }
    else
        return 0;
    return (uint16)~((high << 8) | low);
}

static uint32 ReadPadPacketByte(const uint8 *packet, uint32 address, uint32 offset)
{
    return address ? r_u8(address + offset) : packet[offset];
}

static void WritePadPacketByte(uint8 *packet, uint32 address, uint32 offset, uint32 value)
{
    if (address)
        w_u8(address + offset, value);
    else
        packet[offset] = (uint8)value;
}

static uint32 CopyGamePadPacket(uint8 *destination, const uint8 *source)
{
    FUNCTION_MARKER(0x800705F8, "SLUS_003.73");
    uint32 destination_address = GuestMemoryAddress(destination);
    uint32 source_address = GuestMemoryAddress(source);
    uint32 first = ReadPadPacketByte(source, source_address, 0);
    uint32 previous = ReadPadPacketByte(destination, destination_address, 1);
    WritePadPacketByte(destination, destination_address, 0, first);
    for (uint32 index = 1; index < 8; ++index)
    {
        uint32 value = ReadPadPacketByte(source, source_address, index);
        WritePadPacketByte(destination, destination_address, index, value);
    }
    if (ReadPadPacketByte(source, source_address, 0) == 255)
        WritePadPacketByte(destination, destination_address, 1, 0);
    else if (previous != ReadPadPacketByte(destination, destination_address, 1))
    {
        sub_8009C7EC();
        sub_8009C7CC();
        w_u8(0x800EC26F, 0);
        return 0x800F0000;
    }
    return previous;
}

uint32 NormalizePadPackets(void)
{
    FUNCTION_MARKER(0x8007047C, "SLUS_003.73");
    uint32 port, source, destination;
    for (port = 0; port < 2; ++port)
    {
        source = 0x800EC3F0 + 34 * port;
        destination = 0x800EC434 + 32 * port;
        if (r_u8(source + 1) == 128)
        {
            CopyGamePadPacket((uint8 *)psx_addr(destination, 8), (const uint8 *)psx_addr(source + 2, 8));
            CopyGamePadPacket((uint8 *)psx_addr(destination + 8, 8), (const uint8 *)psx_addr(source + 10, 8));
            CopyGamePadPacket((uint8 *)psx_addr(destination + 16, 8), (const uint8 *)psx_addr(source + 18, 8));
            CopyGamePadPacket((uint8 *)psx_addr(destination + 24, 8), (const uint8 *)psx_addr(source + 26, 8));
        }
        else
        {
            CopyGamePadPacket((uint8 *)psx_addr(destination, 8), (const uint8 *)psx_addr(source, 8));
            w_u8(destination + 9, 0);
            w_u8(destination + 17, 0);
            w_u8(destination + 25, 0);
        }
    }
    return 128;
}

uint32 PollPadPackets(void)
{
    FUNCTION_MARKER(0x80070748, "SLUS_003.73");
    /* Service native elapsed VBlank time while the game polls hardware input */
    VSync(-1);
    static const uint32 masks[16] = {16, 128, 32, 64, 4, 1, 8, 2, 32768, 8192, 4096, 16384, 512, 1024, 2048, 256};
    uint32 port, i, address, timer, first, second, buttons, table, mode, unchanged = 1, first_type, second_type;
    w_u32(0x800FF830, r_u32(0x800FF830) + 1);
    for (port = 0; port < 2; ++port)
        for (i = 0; i < 2; ++i)
        {
            address = 0x800EC270 + 380 * port + 2 * i;
            timer = r_u16(address);
            if (timer)
            {
                w_u16(address, timer - 1);
                if ((uint16)(timer - 1) == 0)
                    ClearGamePadButton(port, i);
            }
        }
    NormalizePadPackets();
    first = ReadPacketButtons(0x800EC434);
    second = ReadPacketButtons(0x800EC454);
    buttons = (second << 16) | first;
    first_type = r_u8(0x800EC435);
    second_type = r_u8(0x800EC455);
    w_u32(0x800EC3E0, second_type);
    w_u32(0x800EC264, first_type);
    table = (first_type == 115 || first_type == 83) ? 0x800EC248 : 0x800EC238;
    mode = r_u32(0x800FF818);
    if (mode)
    {
        if (buttons)
            w_u32(0x800FF814, 0);
    }
    else
    {
        uint32 axes[8];
        for (i = 0; i < 8; ++i)
        {
            address = (i < 4 ? 0x800EC438 : 0x800EC458) + (i & 3);
            axes[i] = r_u8(address) & 252;
        }
        if (buttons)
            unchanged = 0;
        else
            for (i = 0; i < 8; ++i)
                if (axes[i] != r_u32(0x800FFC80 + 4 * i))
                {
                    unchanged = 0;
                    break;
                }
        if (unchanged)
            w_u32(0x800FF814, r_u32(0x800FF814) + 1);
        else
            for (i = 0; i < 8; ++i)
                w_u32(0x800FFC80 + 4 * i, axes[i]);
        if ((second & 0x900) == 0x900)
            w_u32(0x800EC3E4, r_u32(0x800EC3E4) + 1);
        else
            w_u32(0x800EC3E4, 0);
        if (r_u32(0x800EC268) == 120 || r_u32(0x800EC3E4) == 120)
            return 1;
    }
    mode = r_u32(0x800FF818);
    if (mode == 2)
    {
        if (first)
        {
            w_u32(0x800FF2EC, 5);
            return 0;
        }
        for (i = 0; i < 8; ++i)
            w_u8(0x800EC25C + i, 128);
        w_u32(0x800EC264, 65);
        for (i = 0; i < 20; ++i)
        {
            ReplayButton(0x800EC0F8 + 16 * i, i);
            if (r_u16(r_u32(0x800FF820) + 4) == 65535)
            {
                w_u32(0x800FF2EC, 4);
                return 0;
            }
        }
        return 0;
    }
    if (mode != 0 && mode != 1)
        return 0;
    if (!mode)
    {
        first_type = r_u32(0x800EC264);
    }
    if (!mode && (first_type == 115 || first_type == 83))
    {
        for (i = 0; i < 4; ++i)
        {
            w_u8(0x800EC25C + i, r_u8(0x800EC438 + r_u8(0x800EC258 + i)));
            w_u8(0x800EC3D8 + i, r_u8(0x800EC458 + r_u8(0x800EC3D4 + i)));
        }
        static const uint32 limits[4][3] = {
            {0x800FF7F0, 0x800FF7F4, 0x800FF808},
            {0x800FF7F8, 0x800FF7FC, 0x800FF80C},
            {0x800FF7E0, 0x800FF7E4, 0x800FF800},
            {0x800FF7E8, 0x800FF7EC, 0x800FF804}
        };
        sint32 scaled = 0;
        for (i = 0; i < 4; ++i)
        {
            uint32 raw = r_u8(0x800EC25F - i);
            uint32 minimum = r_u32(limits[i][0]);
            uint32 maximum = r_u32(limits[i][1]);
            uint32 centre = r_u32(limits[i][2]);
            if (i)
                w_u8(0x800EC264 - i, scaled);
            scaled = ScalePadAxis(raw, minimum, maximum, centre);
        }
        w_u8(0x800EC260, scaled);
    }
    for (i = 0; i < 16; ++i)
    {
        if (mode)
            RecordButton(0x800EC0F8 + 16 * i, buttons & masks[i], i);
        else
            UpdateButtonState(0x800EC0F8 + 16 * i, buttons & masks[i]);
    }
    for (i = 0; i < 4; ++i)
    {
        if (mode)
            RecordButton(0x800EC1F8 + 16 * i, buttons & r_u32(table + 4 * i), i + 16);
        else
            UpdateButtonState(0x800EC1F8 + 16 * i, buttons & r_u32(table + 4 * i));
    }
    for (i = 0; i < 16; ++i)
        UpdateButtonState(0x800EC274 + 16 * i, second & masks[i]);
    return 0;
}

void StopPadCommunication(void)
{
    xport_bios_enter_critical();
    ChangeClearRCnt(3, 1);
    SysDeqIntRP(2, 0x80107CC8);
    xport_bios_exit_critical();
    PadStopCom();
}

extern "C" void sub_8009C7EC(void)
{
    FUNCTION_MARKER(0x8009C7EC, "SLUS_003.73");
    StopPadCommunication();
}


uint32 UpdateButtonState(uint32 state, uint32 pressed)
{
    uint32 elapsed, old, result;
    FUNCTION_MARKER(0x800702E0, "SLUS_003.73");
    elapsed = r_u32(state + 12);
    old = r_u8(state);
    w_u32(state + 12, elapsed + 1);
    if (!old && pressed)
    {
        w_u8(state + 1, 1);
        w_u32(state + 12, 0);
    }
    if (pressed)
    {
        w_u32(0x800FF814, 0);
        w_u8(state, 1);
    }
    else
        w_u8(state, 0);
    if (r_u8(state))
    {
        result = r_u32(state + 4);
        w_u32(state + 8, 0);
        ++result;
        w_u32(state + 4, result);
    }
    else
    {
        result = r_u32(state + 8);
        w_u32(state + 4, 0);
        ++result;
        w_u32(state + 8, result);
    }
    return result;
}

extern "C" uint32 sub_800702E0(uint32 state, uint32 pressed)
{
    FUNCTION_MARKER(0x800702E0, "SLUS_003.73");
    return UpdateButtonState(state, pressed);
}
extern "C" uint32 sub_80070354(uint32 state, uint32 pressed, uint32 index)
{
    FUNCTION_MARKER(0x80070354, "SLUS_003.73");
    return RecordButton(state, pressed, index);
}
extern "C" uint32 sub_80070410(uint32 state, uint32 index)
{
    FUNCTION_MARKER(0x80070410, "SLUS_003.73");
    return ReplayButton(state, index);
}
extern "C" uint32 sub_800706B0(uint32 raw, uint32 minimum, uint32 maximum, uint32 centre)
{
    FUNCTION_MARKER(0x800706B0, "SLUS_003.73");
    return (uint32)ScalePadAxis(raw, minimum, maximum, centre);
}
extern "C" uint32 apocalypse_pad_poll(void) { return PollPadPackets(); }
extern "C" uint32 apocalypse_pad_normalize(void) { return NormalizePadPackets(); }
extern "C" void apocalypse_pad_stop(void) { StopPadCommunication(); }

static uint32 ClearGamePadButton(uint32 port, uint32 button)
{
    FUNCTION_MARKER(0x80070288, "SLUS_003.73");
    uint32 result = r_u32(0x800FF818);
    if (!result)
    {
        uint32 offset = 380 * (port & 255);
        button &= 255;
        result = 0x800EC270 + offset + 2 * button;
        w_u8(0x800EC26C + offset + button, 0);
        w_u16(result, 0);
    }
    return result;
}

extern "C" uint32 sub_80070288(uint32 port, uint32 button)
{
    FUNCTION_MARKER(0x80070288, "SLUS_003.73");
    return ClearGamePadButton(port, button);
}

extern "C" uint32 sub_800705F8(uint32 destination, uint32 source)
{
    FUNCTION_MARKER(0x800705F8, "SLUS_003.73");
    return CopyGamePadPacket((uint8 *)psx_addr(destination, 8), (const uint8 *)psx_addr(source, 8));
}
extern "C" uint32 sub_8007047C(void)
{
    FUNCTION_MARKER(0x8007047C, "SLUS_003.73");
    return NormalizePadPackets();
}

static uint32 InitializeGamePadPort(uint32 controller)
{
    FUNCTION_MARKER(0x800700B0, "SLUS_003.73");
    for (uint32 index = 0; index < 20; ++index)
    {
        w_u8(controller, 0);
        w_u8(controller + 1, 0);
        w_u32(controller + 4, 0);
        w_u32(controller + 8, 0);
        w_u32(controller + 12, 0);
        controller += 16;
    }
    w_u8(0x800EC260, 128);
    w_u8(0x800EC261, 128);
    w_u8(0x800EC262, 128);
    w_u8(0x800EC263, 128);
    return 128;
}

static uint32 ClearGamePadButtonStates(uint32 controller)
{
    FUNCTION_MARKER(0x80070100, "SLUS_003.73");
    for (uint32 index = 0; index < 20; ++index)
    {
        w_u8(controller + 1, 0);
        controller += 16;
    }
    return 0xFFFFFFFF;
}

uint32 InitializeGamePads()
{
    FUNCTION_MARKER(0x8007001C, "SLUS_003.73");
    uint32 controller = 0x800EC0F8;
    for (uint32 index = 0; index < 2; ++index)
    {
        InitializeGamePadPort(controller);
        w_u32(controller + 368, 0);
        controller += 380;
    }
    uint32 allocation = r_u32(0x800FF81C);
    w_u32(0x800FF830, 0);
    w_u32(0x800FF820, allocation);
    return allocation;
}

extern "C" uint32 sub_800700B0(uint32 controller)
{
    FUNCTION_MARKER(0x800700B0, "SLUS_003.73");
    return InitializeGamePadPort(controller);
}
extern "C" uint32 sub_80070100(uint32 controller)
{
    FUNCTION_MARKER(0x80070100, "SLUS_003.73");
    return ClearGamePadButtonStates(controller);
}
extern "C" uint32 sub_8007001C(void)
{
    FUNCTION_MARKER(0x8007001C, "SLUS_003.73");
    return InitializeGamePads();
}

extern "C" uint32 sub_80070748(void)
{
    FUNCTION_MARKER(0x80070748, "SLUS_003.73");
    return PollPadPackets();
}

uint32 SetGamePadActuator(uint32 controller, uint32 value, uint32 actuator, uint32 enabled)
{
    FUNCTION_MARKER(0x8007011C, "SLUS_003.73");
    uint32 result = r_u32(0x800FF818);
    if (result)
        return result;
    uint32 index = controller & 255;
    uint32 port = index ? 16 : 0;
    uint32 state = (uint32)PadGetState((sint32)port);
    if (!state)
        return state;
    uint32 owner = 0x800EC0F8 + 380 * index;
    if (state == 1)
        w_u8(owner + 375, 0);
    if (!r_u8(owner + 375))
    {
        PadSetActPSX(port, owner + 372, 2);
        if (state == 2 || (state == 6 && PadSetActAlignPSX(port, 0x800FF824)))
            w_u8(owner + 375, 1);
    }
    result = 0x800EC270 + 380 * index;
    actuator &= 255;
    w_u8(0x800EC26C + 380 * index + actuator, enabled);
    w_u16(result + 2 * actuator, value);
    return result;
}

extern "C" uint32 sub_8007011C(uint32 controller, uint32 value, uint32 actuator, uint32 enabled)
{
    FUNCTION_MARKER(0x8007011C, "SLUS_003.73");
    return SetGamePadActuator(controller, value, actuator, enabled);
}

static uint32 ConfigureGamePadButtonMasks(uint32 controller, uint32 first, uint32 second, uint32 third, uint32 fourth)
{
    FUNCTION_MARKER(0x8006FDFC, "SLUS_003.73");
    w_u32(controller + 320, first);
    w_u32(controller + 324, second);
    w_u32(controller + 328, third);
    w_u32(controller + 332, fourth);
    return fourth;
}

static uint32 ConfigureGamePadAxes(uint32 controller, uint32 first_axis, uint32 second_axis, uint32 third_axis, uint32 fourth_axis, uint32 first_mask, uint32 second_mask, uint32 third_mask, uint32 fourth_mask)
{
    FUNCTION_MARKER(0x8006FE14, "SLUS_003.73");
    w_u8(controller + 352, first_axis);
    w_u8(controller + 353, second_axis);
    w_u8(controller + 354, third_axis);
    w_u8(controller + 355, fourth_axis);
    w_u32(controller + 336, first_mask);
    w_u32(controller + 340, second_mask);
    w_u32(controller + 344, third_mask);
    w_u32(controller + 348, fourth_mask);
    return fourth_axis;
}

uint32 SetupGamePadInput()
{
    uint32 controller = 0x800EC0F8;
    uint32 count = 0;
    uint32 index;
    FUNCTION_MARKER(0x8006FE4C, "SLUS_003.73");
    do
    {
        uint32 x = (uint32)(sint32)(sint16)r_u16(0x800EC474);
        uint32 y = (uint32)(sint32)(sint16)r_u16(0x800EC476);
        uint32 width = (uint32)(sint32)(sint16)r_u16(0x800EC478);
        uint32 height = (uint32)(sint32)(sint16)r_u16(0x800EC47A);
        uint32 destination;
        ++count;
        ConfigureGamePadButtonMasks(controller, x, y, width, height);
        destination = controller;
        x = (uint32)(sint32)(sint16)r_u16(0x800EC494);
        y = (uint32)(sint32)(sint16)r_u16(0x800EC496);
        width = (uint32)(sint32)(sint16)r_u16(0x800EC478);
        height = (uint32)(sint32)(sint16)r_u16(0x800EC47A);
        controller += 0x17C;
        ConfigureGamePadAxes(destination, 3, 2, 1, 0, x, y, width, height);
    } while (count < 2);
    for (index = 0; index < 256; ++index)
    {
        uint32 destination = 0x801027A8 + index;
        w_u8(destination, 0);
        if ((index & 1) != 0)
            w_u8(destination, 1);
        if ((index & 2) != 0)
            w_u8(destination, r_u8(destination) | 4);
        if ((index & 4) != 0)
            w_u8(destination, r_u8(destination) | 0x80);
        if ((index & 8) != 0)
            w_u8(destination, r_u8(destination) | 0x10);
        if ((index & 0x10) != 0)
            w_u8(destination, r_u8(destination) | 8);
        if ((index & 0x20) != 0)
            w_u8(destination, r_u8(destination) | 0x20);
        if ((index & 0x40) != 0)
            w_u8(destination, r_u8(destination) | 0x40);
        if ((index & 0x80) != 0)
            w_u8(destination, r_u8(destination) | 2);
    }
    InitializeGamePads();
    apocalypse_pad_init_mtap(0x800EC3F0, 0x800EC412);
    sub_8009C7CC();
    w_u32(0x800FF814, 0);
    w_u32(0x800FF82C, 1);
    return 1;
}

extern "C" uint32 sub_8006FDFC(uint32 controller, uint32 first, uint32 second, uint32 third, uint32 fourth)
{
    FUNCTION_MARKER(0x8006FDFC, "SLUS_003.73");
    return ConfigureGamePadButtonMasks(controller, first, second, third, fourth);
}
extern "C" uint32 sub_8006FE14(uint32 controller, uint32 first_axis, uint32 second_axis, uint32 third_axis, uint32 fourth_axis, uint32 first_mask, uint32 second_mask, uint32 third_mask, uint32 fourth_mask)
{
    FUNCTION_MARKER(0x8006FE14, "SLUS_003.73");
    return ConfigureGamePadAxes(controller, first_axis, second_axis, third_axis, fourth_axis, first_mask, second_mask, third_mask, fourth_mask);
}
extern "C" uint32 sub_8006FE4C(void)
{
    FUNCTION_MARKER(0x8006FE4C, "SLUS_003.73");
    return SetupGamePadInput();
}


uint32 LoadGamePadConfiguration()
{
    uint32 index, first, second, third, fourth;
    FUNCTION_MARKER(0x80071054, "SLUS_003.73");
    for (index = 0; index < 2048; ++index)
        w_u16(0x800ED472 - index * 2, 0x6969);
    LookupResource((const char *)psx_addr(0x800A3AEC, 1));
    BeginResourceRead(0x800EC474);
    WaitForResourceRead();
    first = (uint32)(sint32)(sint16)r_u16(0x800EC474);
    second = (uint32)(sint32)(sint16)r_u16(0x800EC476);
    third = (uint32)(sint32)(sint16)r_u16(0x800EC478);
    fourth = (uint32)(sint32)(sint16)r_u16(0x800EC47A);
    ConfigureGamePadButtonMasks(0x800EC0F8, first, second, third, fourth);
    first = (uint32)(sint32)(sint16)r_u16(0x800EC494);
    second = (uint32)(sint32)(sint16)r_u16(0x800EC496);
    third = (uint32)(sint32)(sint16)r_u16(0x800EC498);
    fourth = (uint32)(sint32)(sint16)r_u16(0x800EC49A);
    ConfigureGamePadAxes(0x800EC0F8, 3, 2, 1, 0, first, second, third, fourth);
    LookupResource((const char *)psx_addr(0x800A3AF8, 1));
    BeginResourceRead(0x800ECC74);
    return WaitForResourceRead();
}

uint32 ReloadGamePadConfiguration()
{
    FUNCTION_MARKER(0x8007113C, "SLUS_003.73");
    return LoadGamePadConfiguration();
}

extern "C" uint32 sub_80071054(void)
{
    FUNCTION_MARKER(0x80071054, "SLUS_003.73");
    return LoadGamePadConfiguration();
}
extern "C" uint32 sub_8007113C(void)
{
    FUNCTION_MARKER(0x8007113C, "SLUS_003.73");
    return ReloadGamePadConfiguration();
}

#include "game_pad_poll.h"
#include "draft_first_signatures.h"
#include <limits.h>

static uint32 record_button(uint32 state, uint32 pressed, uint32 index)
{
    uint32 old = r_u8(state), down = pressed != 0, record;
    w_u32(state + 12, r_u32(state + 12) + 1);
    if (!old && down)
    {
        w_u8(state + 1, 1);
        w_u32(state + 12, 0);
    }
    if (old != down)
    {
        record = r_u32(0x800FF820u);
        w_u8(record + 6, index);
        w_u16(record + 4, r_u16(0x800FF830u));
        w_u8(record + 7, down);
        w_u32(record, r_u32(0x800FF38Cu));
        w_u32(0x800FF820u, record + 8);
        w_u16(record + 12, 65535);
    }
    w_u8(state, down);
    if (down)
    {
        w_u32(state + 8, 0);
        w_u32(state + 4, r_u32(state + 4) + 1);
        return r_u32(state + 4);
    }
    w_u32(state + 4, 0);
    w_u32(state + 8, r_u32(state + 8) + 1);
    return r_u32(state + 8);
}

static void replay_button(uint32 state, uint32 index)
{
    uint32 record = r_u32(0x800FF820u);
    if ((sint32)r_u32(0x800FF830u) < (sint32)r_u16(record + 4))
        return;
    if (r_u8(record + 6) != index)
        return;
    sub_800702E0(state, r_u8(record + 7));
    record = r_u32(0x800FF820u);
    w_u32(0x800FF390u, r_u32(record));
    w_u32(0x800FF820u, record + 8);
}

static sint32 signed_divide(sint32 n, sint32 d)
{
    if (!d)
        return n < 0 ? 1 : -1;
    if (n == INT_MIN && d == -1)
        return INT_MIN;
    return n / d;
}

static sint32 axis(uint32 raw, uint32 minimum, uint32 maximum, uint32 centre)
{
    sint32 value = (sint32)raw, low = (sint32)minimum, high = (sint32)maximum;
    uint32 distance, numerator, denominator;
    if (value < low)
        return -127;
    if (high < value)
        return 127;
    if (value < (sint32)(centre - 32))
    {
        w_u32(0x800FF814u, 0);
        distance = centre - (raw + 32);
        numerator = distance - (distance << 7);
        denominator = centre - (minimum + 32);
        return signed_divide((sint32)numerator, (sint32)denominator);
    }
    if ((sint32)(centre + 32) < value)
    {
        w_u32(0x800FF814u, 0);
        distance = raw - 32 - centre;
        numerator = (distance << 7) - distance;
        denominator = maximum - 32 - centre;
        return signed_divide((sint32)numerator, (sint32)denominator);
    }
    return 0;
}

static uint16 packet_buttons(uint32 packet)
{
    uint32 type = r_u8(packet + 1), second = r_u8(packet + 3);
    if (type == 83)
        second = r_u8(0x801027A8u + second);
    else if (type != 65 && type != 115)
        return 0;
    return (uint16) ~((r_u8(packet + 2) << 8) | second);
}

uint32 apocalypse_pad_normalize(void)
{
    uint32 port, source, destination;
    for (port = 0; port < 2; ++port)
    {
        source = 0x800EC3F0u + 34 * port;
        destination = 0x800EC434u + 32 * port;
        if (r_u8(source + 1) == 128)
        {
            sub_800705F8(destination, source + 2);
            sub_800705F8(destination + 8, source + 10);
            sub_800705F8(destination + 16, source + 18);
            sub_800705F8(destination + 24, source + 26);
        }
        else
        {
            sub_800705F8(destination, source);
            w_u8(destination + 9, 0);
            w_u8(destination + 17, 0);
            w_u8(destination + 25, 0);
        }
    }
    return 128;
}

uint32 apocalypse_pad_poll(void)
{
    /* Service native elapsed VBlank time while the game polls hardware input */
    VSync(-1);
    static const uint32 masks[16] = {16, 128, 32, 64, 4, 1, 8, 2, 32768, 8192, 4096, 16384, 512, 1024, 2048, 256};
    uint32 port, i, address, timer, first, second, buttons, table, mode, unchanged = 1;
    w_u32(0x800FF830u, r_u32(0x800FF830u) + 1);
    for (port = 0; port < 2; ++port)
        for (i = 0; i < 2; ++i)
        {
            address = 0x800EC270u + 380 * port + 2 * i;
            timer = r_u16(address);
            if (timer)
            {
                w_u16(address, timer - 1);
                if ((uint16)(timer - 1) == 0)
                    sub_80070288(port, i);
            }
        }
    apocalypse_pad_normalize();
    first = packet_buttons(0x800EC434u);
    second = packet_buttons(0x800EC454u);
    buttons = (second << 16) | first;
    w_u32(0x800EC264u, r_u8(0x800EC435u));
    w_u32(0x800EC3E0u, r_u8(0x800EC455u));
    table = (r_u8(0x800EC435u) == 83 || r_u8(0x800EC435u) == 115) ? 0x800EC248u : 0x800EC238u;
    mode = r_u32(0x800FF818u);
    if (mode)
    {
        if (buttons)
            w_u32(0x800FF814u, 0);
    }
    else
    {
        for (i = 0; i < 8; ++i)
        {
            address = (i < 4 ? 0x800EC438u : 0x800EC458u) + (i & 3);
            if ((r_u8(address) & 252) != r_u32(0x800FFC80u + 4 * i))
                unchanged = 0;
        }
        if (!buttons && unchanged)
            w_u32(0x800FF814u, r_u32(0x800FF814u) + 1);
        else
            for (i = 0; i < 8; ++i)
            {
                address = (i < 4 ? 0x800EC438u : 0x800EC458u) + (i & 3);
                w_u32(0x800FFC80u + 4 * i, r_u8(address) & 252);
            }
        if ((second & 0x900) == 0x900)
            w_u32(0x800EC3E4u, r_u32(0x800EC3E4u) + 1);
        else
            w_u32(0x800EC3E4u, 0);
        if (r_u32(0x800EC268u) == 120 || r_u32(0x800EC3E4u) == 120)
            return 1;
    }
    if (mode == 2)
    {
        if (first)
        {
            w_u32(0x800FF2ECu, 5);
            return 0;
        }
        for (i = 0; i < 8; ++i)
            w_u8(0x800EC25Cu + i, 128);
        w_u32(0x800EC264u, 65);
        for (i = 0; i < 20; ++i)
        {
            replay_button(0x800EC0F8u + 16 * i, i);
            if (r_u16(r_u32(0x800FF820u) + 4) == 65535)
            {
                w_u32(0x800FF2ECu, 4);
                return 0;
            }
        }
        return 0;
    }
    if (mode != 0 && mode != 1)
        return 0;
    if (!mode && (r_u32(0x800EC264u) == 115 || r_u32(0x800EC264u) == 83))
    {
        for (i = 0; i < 4; ++i)
        {
            w_u8(0x800EC25Cu + i, r_u8(0x800EC438u + r_u8(0x800EC258u + i)));
            w_u8(0x800EC3D8u + i, r_u8(0x800EC458u + r_u8(0x800EC3D4u + i)));
        }
        w_u8(0x800EC263u, axis(r_u8(0x800EC25Fu), r_u32(0x800FF7F0u), r_u32(0x800FF7F4u), r_u32(0x800FF808u)));
        w_u8(0x800EC262u, axis(r_u8(0x800EC25Eu), r_u32(0x800FF7F8u), r_u32(0x800FF7FCu), r_u32(0x800FF80Cu)));
        w_u8(0x800EC261u, axis(r_u8(0x800EC25Du), r_u32(0x800FF7E0u), r_u32(0x800FF7E4u), r_u32(0x800FF800u)));
        w_u8(0x800EC260u, axis(r_u8(0x800EC25Cu), r_u32(0x800FF7E8u), r_u32(0x800FF7ECu), r_u32(0x800FF804u)));
    }
    for (i = 0; i < 16; ++i)
    {
        if (mode)
            record_button(0x800EC0F8u + 16 * i, buttons & masks[i], i);
        else
            sub_800702E0(0x800EC0F8u + 16 * i, buttons & masks[i]);
    }
    for (i = 0; i < 4; ++i)
    {
        if (mode)
            record_button(0x800EC1F8u + 16 * i, buttons & r_u32(table + 4 * i), i + 16);
        else
            sub_800702E0(0x800EC1F8u + 16 * i, buttons & r_u32(table + 4 * i));
    }
    for (i = 0; i < 16; ++i)
        sub_800702E0(0x800EC274u + 16 * i, second & masks[i]);
    return 0;
}

void apocalypse_pad_stop(void)
{
    xport_bios_enter_critical();
    ChangeClearRCnt(3, 1);
    SysDeqIntRP(2, 0x80107CC8u);
    xport_bios_exit_critical();
    PadStopCom();
}

void sub_8009C7EC(void)
{
    apocalypse_pad_stop();
}

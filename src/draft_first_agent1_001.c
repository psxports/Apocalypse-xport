#include "game_pad_poll.h"
#include "game_movie_startup.h"
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include "game_startup_adapters.h"
#include <stdlib.h>
/* Unverified draft; TODO Refine unresolved callbacks and ABI during integration */

uint32 sub_800700B0(uint32 controller)
{
    uint32 index;
    FUNCTION_MARKER(0x800700B0u, "SLUS_003.73");
    for (index = 0u; index < 20u; ++index)
    {
        w_u8(controller, 0u);
        w_u8(controller + 1u, 0u);
        w_u32(controller + 4u, 0u);
        w_u32(controller + 8u, 0u);
        w_u32(controller + 12u, 0u);
        controller += 16u;
    }
    w_u8(0x800EC260u, 0x80u);
    w_u8(0x800EC261u, 0x80u);
    w_u8(0x800EC262u, 0x80u);
    w_u8(0x800EC263u, 0x80u);
    return 0x80u;
}

uint32 sub_8009CD74(void)
{
    FUNCTION_MARKER(0x8009CD74u, "SLUS_003.73");
    w_u32(0x80107CCCu, 0x8009CE08u);
    w_u32(0x80107CD0u, 0x8009CDA0u);
    w_u32(0x80107CC8u, 0u);
    w_u32(0x80107CD4u, 0u);
    return 0x80107CCCu;
}


uint32 sub_8009CE08(void)
{
    uint32 flag;
    uint32 count;
    uint32 first;
    uint32 last;
    uint32 base;
    uint32 target;
    uint32 port;
    FUNCTION_MARKER(0x8009CE08u, "SLUS_003.73");
    flag = r_u32(0x800FEE44u);
    w_u32(0x800FEE5Cu, 1u);
    if (flag != 0u)
    {
        count = r_u32(0x80107CD8u);
        if ((sint32)count < 150)
            w_u32(0x80107CD8u, count + 1u);
    }
    if (r_u32(0x800FEE48u) == 0u)
    {
        count = r_u32(0x80107CDCu);
        if ((sint32)count < 150)
            w_u32(0x80107CDCu, count + 1u);
    }
    if (r_u32(0x800FEE2Cu) == 0u)
        return 0u;
    first = r_u32(0x800FEE44u);
    last = r_u32(0x800FEE48u);
    if ((sint32)last < (sint32)first)
        return 0u;
    base = r_u32(0x800FEE28u);
    w_u32(0x800FEE38u, 0u);
    w_u32(0x800FEE34u, first);
    if (sub_8009D098(base + first * 240u) == 0u)
    {
        target = r_u32(0x800FEDF4u);
        switch (target)
        {
        case 0x8009EA80u:
            sub_8009EA80(0xFFFFu);
            break;
        default:
            /* TODO Unknown callback target or adapter */
            abort();
        }
    }
    first = r_u32(0x800FEE34u);
    last = r_u32(0x800FEE48u);
    w_u32(0x800FEE3Cu, 0u);
    while ((sint32)last >= (sint32)first)
    {
        base = r_u32(0x800FEE28u);
        sub_8009D2AC(base + first * 240u);
        first = r_u32(0x800FEE34u);
        last = r_u32(0x800FEE48u);
    }
    port = r_u32(0x800FEE58u);
    w_u16(port + 14u, 0x88u);
    return 0u;
}


uint32 sub_8009D098(uint32 controller)
{
    uint32 port;
    uint32 index;
    uint32 counter_address;
    uint32 count;
    uint32 argument;
    uint32 target;
    uint32 control;
    FUNCTION_MARKER(0x8009D098u, "SLUS_003.73");
    port = r_u32(0x800FEE58u);
    w_u16(port + 10u, 0x40u);
    w_u16(port + 10u, 0u);
    w_u16(port + 8u, 13u);
    w_u16(port + 14u, 0x88u);
    sub_8009F92C(r_u8(controller + 232u) == 8u ? 0x50u : 0x91u);
    index = r_u32(0x800FEE34u);
    port = r_u32(0x800FEE58u);
    w_u16(port + 10u, index == 0u ? 0x1003u : 0x3003u);
    count = r_u32(0x800FEE4Cu + index * 4u);
    if ((sint32)count >= 0)
    {
        if ((sint32)count > 0)
        {
            do
            {
                index = r_u32(0x800FEE34u);
                counter_address = 0x800FEE4Cu + index * 4u;
                count = r_u32(counter_address) - 1u;
                w_u32(counter_address, count);
                argument = r_u32(controller + 12u);
                target = r_u32(0x800FEE14u);
                switch (target)
                {
                case 0x8009F240u:
                    sub_8009F240(argument + count * 240u);
                    break;
                default:
                    /* TODO Unknown callback target or adapter */
            abort();
                }
                index = r_u32(0x800FEE34u);
                count = r_u32(0x800FEE4Cu + index * 4u);
            } while ((sint32)count > 0);
        }
        index = r_u32(0x800FEE34u);
        counter_address = 0x800FEE4Cu + index * 4u;
        if (r_u32(counter_address) == 0u)
        {
            target = r_u32(0x800FEE14u);
            w_u32(counter_address, 0xFFFFFFFFu);
            switch (target)
            {
            case 0x8009F240u:
                sub_8009F240(controller);
                break;
            default:
                /* TODO Unknown callback target or adapter */
            abort();
            }
            target = r_u32(0x800FEE18u);
            switch (target)
            {
            case 0x8009EB90u:
                sub_8009EB90(controller);
                break;
            default:
                /* TODO Unknown callback target or adapter */
            abort();
            }
        }
    }
    port = r_u32(0x800FEE58u);
    if ((r_u16(port + 4u) & 0x200u) != 0u)
    {
        control = r_u16(port + 10u);
        w_u16(port + 10u, control | 0x10u);
        if ((r_u16(port + 4u) & 0x200u) != 0u)
        {
            w_u8(port, 1u);
            sub_8009D810();
            port = r_u32(0x800FEE58u);
            (void)r_u8(port);
            return 0u;
        }
        port = r_u32(0x800FEE54u);
        w_u32(port, 0xFFFFFF7Fu);
    }
    if (r_u8(controller + 80u) != 0u && r_u8(controller + 54u) != 0u)
        return 0u;
    argument = r_u32(controller + 60u);
    w_u8(argument, 0u);
    return 1u;
}


uint32 sub_8009D2AC(uint32 controller)
{
    uint32 index;
    uint32 target;
    uint32 result;
    FUNCTION_MARKER(0x8009D2ACu, "SLUS_003.73");
    index = r_u32(0x800FEE38u);
    target = r_u32(0x800FEE78u + index * 4u);
    w_u32(0x800FEE38u, index + 1u);
    switch (target)
    {
    case 0x8009E1BCu: result = sub_8009E1BC(controller); break;
    case 0x8009E1FCu: result = sub_8009E1FC(controller); break;
    case 0x8009E2D4u: result = sub_8009E2D4(controller); break;
    case 0x8009E394u: result = sub_8009E394(controller); break;
    case 0x8009E420u: result = sub_8009E420(controller); break;
    default: /* TODO Unknown callback target or adapter */
            abort();
    }
    if ((sint32)result < 0)
    {
        target = r_u32(0x800FEDF4u);
        switch (target)
        {
        case 0x8009EA80u: return sub_8009EA80(result);
        default: /* TODO Unknown callback target or adapter */
            abort();
        }
    }
    index = r_u32(0x800FEE38u);
    if (index != 0u)
    {
        sub_8009F92C(60u);
        if (sub_8009D780() == 0u)
        {
            target = r_u32(0x800FEDF4u);
            switch (target)
            {
            case 0x8009EA80u: sub_8009EA80(0xFFFFFFFDu); break;
            default: /* TODO Unknown callback target or adapter */
            abort();
            }
        }
    }
    index = r_u32(0x800FEE38u);
    result = index - 1u;
    if ((sint32)index >= 5)
        w_u32(0x800FEE38u, result);
    return result;
}

uint32 sub_8009D780(void)
{
    uint32 mask_port;
    uint32 serial_port;
    uint32 control;
    FUNCTION_MARKER(0x8009D780u, "SLUS_003.73");
    mask_port = r_u32(0x800FEE54u);
    serial_port = r_u32(0x800FEE58u);
    w_u32(mask_port, 0xFFFFFF7Fu);
    if ((r_u16(serial_port + 4u) & 0x80u) != 0u)
    {
        do
        {
            if (sub_8009F94C() != 0u)
                return 0u;
            serial_port = r_u32(0x800FEE58u);
        } while ((r_u16(serial_port + 4u) & 0x80u) != 0u);
    }
    serial_port = r_u32(0x800FEE58u);
    control = r_u16(serial_port + 10u);
    w_u16(serial_port + 10u, control | 0x10u);
    return 1u;
}


uint32 sub_8009D54C(uint32 controller, uint32 command)
{
    uint32 buffer;
    uint32 mode;
    uint32 port;
    uint32 received;
    uint32 offset;
    uint32 mask_port;
    uint32 start;
    uint32 limit;
    uint32 timer;
    uint32 elapsed;
    uint32 counter;
    FUNCTION_MARKER(0x8009D54Cu, "SLUS_003.73");
    buffer = r_u32(controller + 60u);
    mode = 0x88u;
    if ((r_u8(buffer) >> 4) == 8u && r_u8(controller + 68u) >= 9u)
        mode = 0x22u;
    port = r_u32(0x800FEE58u);
    while ((r_u16(port + 4u) & 2u) == 0u)
    {
    }
    sub_8009F92C(400u);
    port = r_u32(0x800FEE58u);
    received = r_u8(port);
    offset = r_u8(controller + 68u);
    if (offset == 0u && (received >> 4) == 8u)
        w_u16(port + 14u, 0x22u);
    else
        w_u16(port + 14u, mode);
    mask_port = r_u32(0x800FEE54u);
    if ((r_u32(mask_port) & 0x80u) == 0u)
    {
        start = r_u32(0x801086E8u);
        limit = r_u32(0x801086ECu);
        do
        {
            timer = r_u16(0x1F801120u);
            if (timer < start)
            {
                if (r_u16(0x1F801128u) != 0u)
                    timer += r_u16(0x1F801128u);
                else
                    timer += 0x10000u;
            }
            counter = r_u16(0x1F801124u);
            elapsed = timer - start;
            if ((counter & 0x200u) != 0u && elapsed >= limit)
                return 0xFFFFFFFEu;
            if ((elapsed >> 3) >= limit)
                return 0xFFFFFFFEu;
        } while ((r_u32(mask_port) & 0x80u) == 0u);
    }
    if (r_u8(controller + 232u) != 8u && r_u32(0x800FEE38u) == 2u)
    {
        sub_8009F92C(60u);
        while (sub_8009F94C() == 0u)
        {
        }
    }
    port = r_u32(0x800FEE58u);
    w_u8(port, command);
    counter = r_u8(controller + 69u);
    offset = r_u8(controller + 68u);
    w_u8(controller + 69u, counter + 1u);
    if (offset != 0xFFu)
    {
        offset = r_u8(controller + 68u);
        buffer = r_u32(controller + 60u);
        w_u8(buffer + offset, received);
    }
    offset = r_u8(controller + 68u);
    w_u8(controller + 68u, offset + 1u);
    return received;
}

uint32 sub_8009F5C0(uint32 a1)
{
    uint32 type = r_u8(r_u32(a1 + 60u)) >> 4;
    uint32 stored_type = r_u8(a1 + 232u);
    uint32 pointer, siblings, index, state, count, result;
    if (stored_type != 8u && type == 8u)
    {
        pointer = r_u32(a1 + 48u);
        w_u8(a1 + 232u, 8u);
        w_u8(pointer, 255u);
        w_u8(r_u32(a1 + 48u) + 1u, 128u);
        w_u8(a1 + 68u, 2u);
        stored_type = r_u8(a1 + 232u);
    }
    if (stored_type == 8u)
    {
        pointer = r_u32(a1 + 12u);
        for (index = 0u; index < 4u; ++index)
        {
            sub_8009F5C0(pointer);
            pointer += 240u;
        }
    }
    siblings = r_u32(r_u32(a1 + 16u) + 12u);
    if (r_u32(a1 + 12u) != 0u)
    {
        if (r_u8(a1 + 54u) == 0u)
            for (index = 0u; index < 4u; ++index)
                if (r_u8(siblings + index * 240u + 55u) != 0u)
                    goto busy;
        siblings = r_u32(r_u32(a1 + 16u) + 12u);
        if (r_u32(a1 + 12u) != 0u)
            goto update;
    }
    if (r_u8(a1 + 55u) == 0u)
    {
        if (r_u8(r_u32(a1 + 16u) + 54u) != 0u)
            goto busy;
        for (index = 0u; index < 4u; ++index)
            if (r_u8(siblings + index * 240u + 55u) != 0u)
                goto busy;
    }
update:
    state = r_u8(a1 + 70u);
    w_u32(a1 + 76u, r_u32(a1 + 76u) + 1u);
    if (state == 1u)
    {
        count = r_u8(a1 + 74u);
        if (count >= 2u || r_u8(r_u32(a1 + 16u) + 232u) == 8u)
        {
            w_u8(a1 + 73u, 2u);
            w_u8(a1 + 70u, 255u);
            return 255u;
        }
        w_u8(a1 + 74u, count + 1u);
        return count + 1u;
    }
    if (state != 0u)
    {
        count = r_u8(a1 + 74u);
        if (count < 4u)
        {
            w_u8(a1 + 74u, count + 1u);
            return count + 1u;
        }
    }
    if (r_u8(a1 + 73u) != 0u)
    {
        w_u8(r_u32(a1 + 48u), 255u);
        w_u8(r_u32(a1 + 48u) + 1u, 0u);
        w_u8(a1 + 232u, 0u);
        w_u8(a1 + 68u, 0u);
        /* TODO Dispatch callback at 0x800FEDF8 with controller argument */
        abort();
    }
    result = 8u;
    if (type == 0u)
    {
        pointer = r_u32(a1 + 48u);
        w_u8(a1 + 232u, 0u);
        w_u8(pointer + 1u, 0u);
    }
    if (type == 8u)
    {
        result = 128u;
        if (r_u32(a1 + 12u) != 0u)
        {
            pointer = r_u32(a1 + 48u);
            w_u8(a1 + 232u, 8u);
            w_u8(pointer + 1u, 128u);
        }
    }
    return result;
busy:
    if (r_u8(a1 + 74u) == 0u)
        w_u8(a1 + 74u, 1u);
    return 1u;
}

uint32 sub_80087F60(void)
{
    return r_u32(0x800FCE4Cu);
}

uint32 sub_8009F87C(uint32 a1)
{
    uint32 pointer, index, state;
    if (r_u16(a1 + 230u) == 0u)
        return 1u;
    if (r_u32(a1 + 12u) != 0u)
    {
        if (r_u8(a1 + 70u) != 255u)
            return 1u;
        pointer = r_u32(r_u32(a1 + 16u) + 12u);
        for (index = 0u; index < 4u; ++index)
        {
            state = r_u8(pointer + 70u);
            if (state != 255u && state != 0u)
                return 1u;
            pointer += 240u;
        }
        return 0u;
    }
    if (r_u8(r_u32(a1 + 16u) + 70u) != 255u)
        return 1u;
    return r_u8(a1 + 70u) != 255u;
}

/* TODO Host buffer adapters are declared only; integration is deferred */

uint32 sub_8009A4BC(uint32 a1, uint32 a2)
{
    uint8 location[16];
    uint32 result, reason;
    w_u32(0x800FDD84u, a2);
    if ((a1 & 255u) != 1u)
        w_u32(0x800FDD64u, 0xFFFFFFFFu);
    else if ((sint32)r_u32(0x800FDD64u) > 0)
    {
        if (r_u32(0x800FDD60u) == 512u)
        {
            if ((r_u32(0x800FDD80u) & 1u) != 0u)
            {
                sub_80098214(0u);
                xport_draft_host_sub_800981F4_p1(location, 3u);
                sub_80098238(0u);
                sub_80098214(0x8009A71Cu);
            }
            else
            {
                xport_draft_host_sub_800981D4_p1(location, 3u);
            }
            if (xport_draft_host_sub_8009835C_p1(location) != r_u32(0x800FDD70u))
            {
                sub_8008BF34(0x800A4844u);
                w_u32(0x800FDD64u, 0xFFFFFFFFu);
            }
        }
        if ((r_u32(0x800FDD80u) & 1u) != 0u)
            sub_800981F4(r_u32(0x800FDD58u), r_u32(0x800FDD60u));
        else
        {
            sub_800981D4(r_u32(0x800FDD58u), r_u32(0x800FDD60u));
            w_u32(0x800FDD58u, r_u32(0x800FDD58u) + (r_u32(0x800FDD60u) << 2));
            w_u32(0x800FDD64u, r_u32(0x800FDD64u) - 1u);
            w_u32(0x800FDD70u, r_u32(0x800FDD70u) + 1u);
        }
    }
    w_u32(0x800FDD68u, sub_8008632C(0xFFFFFFFFu));
    if ((sint32)r_u32(0x800FDD64u) < 0)
        sub_8009A7E8(1u);
    result = sub_8008632C(0xFFFFFFFFu);
    if ((sint32)(r_u32(0x800FDD6Cu) + 1200u) < (sint32)result)
        w_u32(0x800FDD64u, 0xFFFFFFFFu);
    if (r_u32(0x800FDD64u) != 0u)
    {
        result = sub_8008632C(0xFFFFFFFFu);
        if ((sint32)(r_u32(0x800FDD6Cu) + 1200u) >= (sint32)result)
            return result;
    }
    sub_80097DD0(r_u32(0x800FDD74u));
    sub_80097DE4(r_u32(0x800FDD78u));
    if ((r_u32(0x800FDD80u) & 1u) != 0u)
        sub_80098214(r_u32(0x800FDD7Cu));
    result = sub_80097F34(9u, 0u);
    if (r_u32(0x800FDD4Cu) != 0u)
    {
        reason = r_u32(0x800FDD64u) == 0u ? 2u : 5u;
        (void)reason;
        /* TODO Dispatch FDD4C callback with reason and a2 */
        abort();
    }
    return result;
}

uint32 sub_8002F130(void)
{
    uint8 volume[8];
    uint8 location[8];
    sint16 rectangle[4];
    uint32 active, index, limit, pointer;
    w_u32(0x800FF294u, 0u);
    if (r_u32(0x800FF254u) != 0u)
        w_u32(0x800FF254u, 0u);
    active = r_u32(0x800FF250u);
    if (active == 0u)
        return active;
    volume[0] = volume[1] = volume[2] = volume[3] = 0u;
    xport_draft_host_sub_800981B4_p1(volume);
    w_u32(0x800FF250u, 0u);
    sub_8009BED8(0u);
    sub_8009AD7C();
    xport_draft_host_sub_80098258_p2(r_u32(0x800FF278u), location);
    xport_draft_host_sub_80097DF8_p2(22u, location, 0u);
    if (r_u32(0x800FFB84u) != 0u)
        w_u32(0x800FFB84u, 0u);
    if (r_u32(0x800FFB88u) != 0u)
        w_u32(0x800FFB88u, 0u);
    limit = r_u8(0x800FF288u);
    for (index = 0u; index < limit; ++index)
    {
        pointer = 0x800A5F68u + index * 4u;
        if (r_u32(pointer) != 0u)
            w_u32(pointer, 0u);
    }
    for (index = 0u; index < 2u; ++index)
    {
        pointer = 0x800FF2A4u + index * 4u;
        if (r_u32(pointer) != 0u)
            w_u32(pointer, 0u);
    }
    rectangle[2] = (sint16)r_u16(0x800FF290u);
    rectangle[0] = (sint16)r_u16(0x800FF28Au);
    rectangle[1] = (sint16)r_u16(0x800FF28Cu);
    rectangle[3] = (sint16)r_u16(0x800FF292u);
    return xport_draft_host_sub_8008847C_p1(rectangle, 1u, 1u, 1u);
}

uint32 sub_80016560(void)
{
    sint16 rectangle[4];
    uint32 result = r_u32(0x800FF050u);
    uint32 value, maximum, color;
    if (result == 0u)
        return result;
    if (r_u32(0x800FF700u) != 0u)
    {
        w_u32(0x800FF074u, 1u);
        return 1u;
    }
    if (r_u32(0x800FF074u) == 0u)
        return 0x80100000u;
    rectangle[0] = (sint16)r_u16(0x800FF078u);
    rectangle[1] = (sint16)r_u16(0x800FF07Au);
    rectangle[2] = (sint16)r_u16(0x800FF07Cu);
    rectangle[3] = (sint16)r_u16(0x800FF07Eu);
    value = r_u32(0x800FF054u) + r_u32(0x800FF6FCu);
    maximum = r_u32(0x800FF058u);
    w_u32(0x800FF054u, value);
    if (maximum != 0u && maximum >= value)
    {
        rectangle[2] = (sint16)((296u * value) / maximum);
        color = 150u;
        result = xport_draft_host_sub_8008847C_p1(rectangle, color, 0u, 0u);
    }
    else
    {
        rectangle[2] = 296;
        result = xport_draft_host_sub_8008847C_p1(rectangle, 0u, 0u, 255u);
    }
    w_u32(0x800FF074u, 0u);
    return result;
}

uint32 sub_800981D4(uint32 destination, uint32 count)
{
    /* TODO Bind existing SDK destination/count API during integration */
    return sub_8009986C(destination, count) == 0u;
}

uint32 sub_8006B2A8(void)
{
    return game_startup_read_step();
}

uint32 sub_8002E738(void)
{
    uint8 location[24];
    uint32 index, entry, value;
    for (index = 0u; index < 34u; ++index)
    {
        entry = 0x800A5B8Cu + 28u * r_u8(0x800A5F44u + index);
        if (r_u32(entry + 24u) != 0u)
        {
            if (xport_draft_host_sub_80099B3C_p1(location, r_u32(entry)) != 0u)
                value = xport_draft_host_sub_8009835C_p1(location);
            else
                value = 0u;
            entry = 0x800A5B8Cu + 28u * r_u8(0x800A5F44u + index);
            w_u32(entry + 24u, value);
        }
    }
    return 0u;
}

uint32 sub_80068270(uint32 a1, uint32 a2)
{
    uint32 result;
    /* TODO Resolve whether the callee consumes the second argument */
    (void)a2;
    sub_8006BC20(a1);
    result = r_u32(0x800FF670u) - 1u;
    w_u32(0x800FF670u, result);
    return result;
}

uint32 sub_8006832C(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    uint32 result = sub_80068298(0x800FF678u);
    w_u16(result, a1);
    w_u16(result + 2u, a2);
    w_u16(result + 4u, a3);
    w_u16(result + 6u, a4);
    return result;
}

uint32 sub_800981B4(uint32 volume)
{
    sub_8009928C(volume);
    return 1u;
}

uint32 sub_8002F7C8(void)
{
    sub_8009BC3C(0u);
    sub_8009BED8(0x8002F84Cu);
    sub_80097B1C(r_u32(0x800FFB84u), r_u32(0x800FF284u));
    sub_8009AEEC(r_u8(0x800FF298u), 0u, 0xFFFFFFFFu, 0u, 0u);
    w_u32(0x800FF2B8u, 0u);
    w_u32(0x800FF2BCu, 1u);
    w_u32(0x800FF2C0u, 0u);
    sub_8002FA48();
    return sub_80097F34(12u, 0u);
}

uint32 sub_800705F8(uint32 a1, uint32 a2)
{
    uint32 index, previous = r_u8(a1 + 1u);
    /* TODO Reconsider partial overlap if callers alias these buffers */
    for (index = 0u; index < 8u; ++index)
        w_u8(a1 + index, r_u8(a2 + index));
    if (r_u8(a2) == 255u)
        w_u8(a1 + 1u, 0u);
    else if (previous != r_u8(a1 + 1u))
    {
        sub_8009C7EC();
        sub_8009C7CC();
        w_u8(0x800EC26Fu, 0u);
        return 0x800F0000u;
    }
    return previous;
}

uint32 sub_80070100(uint32 a1)
{
    uint32 index;
    for (index = 0u; index < 20u; ++index)
    {
        w_u8(a1 + 1u, 0u);
        a1 += 16u;
    }
    return 0xFFFFFFFFu;
}

void sub_8009AD7C(void)
{
    sub_80085E6C();
    if (r_u32(0x800FDA48u) == 1u)
    {
        sub_8009C788(0u);
        sub_8009C760(0u);
    }
    else
    {
        sub_80098214(0u);
        sub_80097DE4(0u);
    }
    w_u8(r_u32(0x800FDD9Cu), 0u);
    w_u8(r_u32(0x800FDDA8u), 0u);
    /* TODO Critical-section bridge takes the inherited value in the existing ABI */
    sub_80085E7C(xport_draft_unknown_critical_argument_8009AD7C());
}

uint32 sub_80010074(void)
{
    uint32 output = r_u32(0x800FEEC0u);
    uint32 input, token, count, index, value;
    if (output != 0u)
        return output;
    output = sub_8006B864(r_u32(r_u32(0x800FEEBCu) + 8u), 1, 1);
    w_u32(0x800FEEC0u, output);
    input = r_u32(0x800FEEBCu) + 12u;
    for (;;)
    {
        token = r_u16(input);
        if (token == 0u)
            return token;
        count = token & 0x7FFFu;
        input += 2u;
        if ((token & 0x8000u) != 0u)
        {
            for (index = 0u; index < count; ++index)
            {
                value = r_u16(input);
                w_u16(output, value);
                output += 2u;
            }
            input += 2u;
        }
        else
            for (index = 0u; index < count; ++index)
            {
                value = r_u16(input);
                input += 2u;
                w_u16(output, value);
                output += 2u;
            }
    }
}

uint32 sub_800101CC(uint32 a1)
{
    sint16 rectangle[4];
    uint32 result = r_u32(0x800FEEC0u);
    if (result != 0u)
    {
        sub_80015EC8();
        rectangle[0] = 0;
        rectangle[1] = (sint16)((sint32)(240u - a1) / 2);
        rectangle[2] = 512;
        rectangle[3] = (sint16)a1;
        xport_draft_host_sub_800885A4_p1(rectangle, r_u32(0x800FEEC0u));
        xport_draft_host_sub_80088664_p1(rectangle, 0u, (uint32)(sint32)rectangle[1] + 256u);
        return sub_800882F8(0u);
    }
    return result;
}

uint32 sub_80071054(void)
{
    uint32 index;
    for (index = 0u; index < 2048u; ++index)
        w_u16(0x800ED472u - index * 2u, 0x6969u);
    sub_8006B04C(0x800A3AECu);
    sub_8006B234(0x800EC474u);
    sub_8006B44C();
    sub_8006FDFC(0x800EC0F8u, (uint32)(sint32)(sint16)r_u16(0x800EC474u),
        (uint32)(sint32)(sint16)r_u16(0x800EC476u), (uint32)(sint32)(sint16)r_u16(0x800EC478u),
        (uint32)(sint32)(sint16)r_u16(0x800EC47Au));
    sub_8006FE14(0x800EC0F8u, 3u, 2u, 1u, 0u,
        (uint32)(sint32)(sint16)r_u16(0x800EC494u), (uint32)(sint32)(sint16)r_u16(0x800EC496u),
        (uint32)(sint32)(sint16)r_u16(0x800EC498u), (uint32)(sint32)(sint16)r_u16(0x800EC49Au));
    sub_8006B04C(0x800A3AF8u);
    sub_8006B234(0x800ECC74u);
    return sub_8006B44C();
}

uint32 sub_8006A334(uint32 a1)
{
    uint32 result = sub_8009013C(a1 & 255u);
    if ((a1 & 255u) != 0u)
    {
        sub_800901EC();
        result = 240u;
        w_u32(0x800FF6CCu, 240u);
    }
    else
        w_u32(0x800FF6CCu, 0u);
    return result;
}




#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <stdio.h>
#include <stdlib.h>

static uint32 ResetPadQuery(uint32 controller);
static uint32 ReadPadModelResponse(uint32 controller);
static uint32 ReadPadCapabilities(uint32 controller);

/* Unverified draft; TODO Refine unresolved callbacks and ABI during integration */

extern "C" uint32 sub_8009CD74(void)
{
    FUNCTION_MARKER(0x8009CD74, "SLUS_003.73");
    w_u32(0x80107CCC, 0x8009CE08);
    w_u32(0x80107CD0, 0x8009CDA0);
    w_u32(0x80107CC8, 0);
    w_u32(0x80107CD4, 0);
    return 0x80107CCC;
}

/* TODO Resolve only observed callback targets within the selected set */
static uint32 FailSelectedControllerCallback(uint32 owner, uint32 slot, uint32 target, uint32 argument0, uint32 argument1)
{
    if (slot == 0x800FEDF8 && target == 0x8009E9BC)
        return ResetPadQuery(argument0);
    GameDiagnostic("Missing selected callback owner=%08X slot=%08X target=%08X args=%08X,%08X\n", owner, slot, target, argument0, argument1);
    abort();
}

extern "C" uint32 sub_8009CE08(void)
{
    uint32 flag;
    uint32 count;
    uint32 first;
    uint32 last;
    uint32 base;
    uint32 target;
    uint32 port;
    FUNCTION_MARKER(0x8009CE08, "SLUS_003.73");
    flag = r_u32(0x800FEE44);
    w_u32(0x800FEE5C, 1);
    if (flag != 0)
    {
        count = r_u32(0x80107CD8);
        if ((sint32)count < 150)
            w_u32(0x80107CD8, count + 1);
    }
    if (r_u32(0x800FEE48) == 0)
    {
        count = r_u32(0x80107CDC);
        if ((sint32)count < 150)
            w_u32(0x80107CDC, count + 1);
    }
    if (r_u32(0x800FEE2C) == 0)
        return 0;
    first = r_u32(0x800FEE44);
    last = r_u32(0x800FEE48);
    if ((sint32)last < (sint32)first)
        return 0;
    base = r_u32(0x800FEE28);
    w_u32(0x800FEE38, 0);
    w_u32(0x800FEE34, first);
    if (sub_8009D098(base + first * 240) == 0)
    {
        target = r_u32(0x800FEDF4);
        switch (target)
        {
            case 0x8009EA80:
                sub_8009EA80(0xFFFF);
                break;
            default:
                /* TODO Unknown callback target */
                (void)FailSelectedControllerCallback(0x8009CE08, 0x800FEDF4, target, 0xFFFF, 0);
        }
    }
    first = r_u32(0x800FEE34);
    last = r_u32(0x800FEE48);
    w_u32(0x800FEE3C, 0);
    while ((sint32)last >= (sint32)first)
    {
        base = r_u32(0x800FEE28);
        sub_8009D2AC(base + first * 240);
        first = r_u32(0x800FEE34);
        last = r_u32(0x800FEE48);
    }
    port = r_u32(0x800FEE58);
    w_u16(port + 14, 0x88);
    return 0;
}

extern "C" uint32 sub_8009D098(uint32 controller)
{
    uint32 port;
    uint32 index;
    uint32 counter_address;
    uint32 count;
    uint32 argument;
    uint32 target;
    uint32 control;
    FUNCTION_MARKER(0x8009D098, "SLUS_003.73");
    port = r_u32(0x800FEE58);
    w_u16(port + 10, 0x40);
    w_u16(port + 10, 0);
    w_u16(port + 8, 13);
    w_u16(port + 14, 0x88);
    sub_8009F92C(r_u8(controller + 232) == 8 ? 0x50 : 0x91);
    index = r_u32(0x800FEE34);
    port = r_u32(0x800FEE58);
    w_u16(port + 10, index == 0 ? 0x1003 : 0x3003);
    count = r_u32(0x800FEE4C + index * 4);
    if ((sint32)count >= 0)
    {
        if ((sint32)count > 0)
        {
            do
            {
                index = r_u32(0x800FEE34);
                counter_address = 0x800FEE4C + index * 4;
                count = r_u32(counter_address) - 1;
                w_u32(counter_address, count);
                argument = r_u32(controller + 12);
                target = r_u32(0x800FEE14);
                switch (target)
                {
                    case 0x8009F240:
                        sub_8009F240(argument + count * 240);
                        break;
                    default:
                        /* TODO Unknown callback target */
                        (void)FailSelectedControllerCallback(0x8009D098, 0x800FEE14, target, argument + count * 240, 0);
                }
                index = r_u32(0x800FEE34);
                count = r_u32(0x800FEE4C + index * 4);
            } while ((sint32)count > 0);
        }
        index = r_u32(0x800FEE34);
        counter_address = 0x800FEE4C + index * 4;
        if (r_u32(counter_address) == 0)
        {
            target = r_u32(0x800FEE14);
            w_u32(counter_address, 0xFFFFFFFF);
            switch (target)
            {
                case 0x8009F240:
                    sub_8009F240(controller);
                    break;
                default:
                    /* TODO Unknown callback target */
                    (void)FailSelectedControllerCallback(0x8009D098, 0x800FEE14, target, controller, 0);
            }
            target = r_u32(0x800FEE18);
            switch (target)
            {
                case 0x8009EB90:
                    sub_8009EB90(controller);
                    break;
                default:
                    /* TODO Unknown callback target */
                    (void)FailSelectedControllerCallback(0x8009D098, 0x800FEE18, target, controller, 0);
            }
        }
    }
    port = r_u32(0x800FEE58);
    if ((r_u16(port + 4) & 0x200) != 0)
    {
        control = r_u16(port + 10);
        w_u16(port + 10, control | 0x10);
        if ((r_u16(port + 4) & 0x200) != 0)
        {
            w_u8(port, 1);
            sub_8009D810();
            port = r_u32(0x800FEE58);
            (void)r_u8(port);
            return 0;
        }
        port = r_u32(0x800FEE54);
        w_u32(port, 0xFFFFFF7F);
    }
    if (r_u8(controller + 80) != 0 && r_u8(controller + 54) != 0)
        return 0;
    argument = r_u32(controller + 60);
    w_u8(argument, 0);
    return 1;
}

extern "C" uint32 sub_8009D2AC(uint32 controller)
{
    uint32 index;
    uint32 target;
    uint32 result;
    FUNCTION_MARKER(0x8009D2AC, "SLUS_003.73");
    index = r_u32(0x800FEE38);
    target = r_u32(0x800FEE78 + index * 4);
    w_u32(0x800FEE38, index + 1);
    switch (target)
    {
        case 0x8009E1BC:
            result = sub_8009E1BC(controller);
            break;
        case 0x8009E1FC:
            result = sub_8009E1FC(controller);
            break;
        case 0x8009E2D4:
            result = sub_8009E2D4(controller);
            break;
        case 0x8009E394:
            result = sub_8009E394(controller);
            break;
        case 0x8009E420:
            result = sub_8009E420(controller);
            break;
        default: /* TODO Unknown callback target */
            result = FailSelectedControllerCallback(0x8009D2AC, 0x800FEE78 + index * 4, target, controller, 0);
    }
    if ((sint32)result < 0)
    {
        target = r_u32(0x800FEDF4);
        switch (target)
        {
            case 0x8009EA80:
                return sub_8009EA80(result);
            default: /* TODO Unknown callback target */
                return FailSelectedControllerCallback(0x8009D2AC, 0x800FEDF4, target, result, 0);
        }
    }
    index = r_u32(0x800FEE38);
    if (index != 0)
    {
        sub_8009F92C(60);
        if (sub_8009D780() == 0)
        {
            target = r_u32(0x800FEDF4);
            switch (target)
            {
                case 0x8009EA80:
                    sub_8009EA80(0xFFFFFFFD);
                    break;
                default: /* TODO Unknown callback target */
                    (void)FailSelectedControllerCallback(0x8009D2AC, 0x800FEDF4, target, 0xFFFFFFFD, 0);
            }
        }
    }
    index = r_u32(0x800FEE38);
    result = index - 1;
    if ((sint32)index >= 5)
        w_u32(0x800FEE38, result);
    return result;
}

extern "C" uint32 sub_8009D780(void)
{
    uint32 mask_port;
    uint32 serial_port;
    uint32 control;
    FUNCTION_MARKER(0x8009D780, "SLUS_003.73");
    mask_port = r_u32(0x800FEE54);
    serial_port = r_u32(0x800FEE58);
    w_u32(mask_port, 0xFFFFFF7F);
    if ((r_u16(serial_port + 4) & 0x80) != 0)
    {
        do
        {
            if (sub_8009F94C() != 0)
                return 0;
            serial_port = r_u32(0x800FEE58);
        } while ((r_u16(serial_port + 4) & 0x80) != 0);
    }
    serial_port = r_u32(0x800FEE58);
    control = r_u16(serial_port + 10);
    w_u16(serial_port + 10, control | 0x10);
    return 1;
}

extern "C" uint32 sub_8009D54C(uint32 controller, uint32 command)
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
    FUNCTION_MARKER(0x8009D54C, "SLUS_003.73");
    buffer = r_u32(controller + 60);
    mode = 0x88;
    if ((r_u8(buffer) >> 4) == 8 && r_u8(controller + 68) >= 9)
        mode = 0x22;
    port = r_u32(0x800FEE58);
    while ((r_u16(port + 4) & 2) == 0)
    {
    }
    sub_8009F92C(400);
    port = r_u32(0x800FEE58);
    received = r_u8(port);
    offset = r_u8(controller + 68);
    if (offset == 0 && (received >> 4) == 8)
        w_u16(port + 14, 0x22);
    else
        w_u16(port + 14, mode);
    mask_port = r_u32(0x800FEE54);
    if ((r_u32(mask_port) & 0x80) == 0)
    {
        start = r_u32(0x801086E8);
        limit = r_u32(0x801086EC);
        do
        {
            timer = r_u16(0x1F801120);
            if (timer < start)
            {
                if (r_u16(0x1F801128) != 0)
                    timer += r_u16(0x1F801128);
                else
                    timer += 0x10000;
            }
            counter = r_u16(0x1F801124);
            elapsed = timer - start;
            if ((counter & 0x200) != 0 && elapsed >= limit)
                return 0xFFFFFFFE;
            if ((elapsed >> 3) >= limit)
                return 0xFFFFFFFE;
        } while ((r_u32(mask_port) & 0x80) == 0);
    }
    if (r_u8(controller + 232) != 8 && r_u32(0x800FEE38) == 2)
    {
        sub_8009F92C(60);
        while (sub_8009F94C() == 0)
        {
        }
    }
    port = r_u32(0x800FEE58);
    w_u8(port, command);
    counter = r_u8(controller + 69);
    offset = r_u8(controller + 68);
    w_u8(controller + 69, counter + 1);
    if (offset != 0xFF)
    {
        offset = r_u8(controller + 68);
        buffer = r_u32(controller + 60);
        w_u8(buffer + offset, received);
    }
    offset = r_u8(controller + 68);
    w_u8(controller + 68, offset + 1);
    return received;
}

extern "C" uint32 sub_8009F5C0(uint32 a1)
{
    FUNCTION_MARKER(0x8009F5C0, "SLUS_003.73");
    uint32 type = r_u8(r_u32(a1 + 60)) >> 4;
    uint32 stored_type = r_u8(a1 + 232);
    uint32 pointer, siblings, index, state, count, result;
    if (stored_type != 8 && type == 8)
    {
        pointer = r_u32(a1 + 48);
        w_u8(a1 + 232, 8);
        w_u8(pointer, 255);
        w_u8(r_u32(a1 + 48) + 1, 128);
        w_u8(a1 + 68, 2);
        stored_type = r_u8(a1 + 232);
    }
    if (stored_type == 8)
    {
        pointer = r_u32(a1 + 12);
        for (index = 0; index < 4; ++index)
        {
            sub_8009F5C0(pointer);
            pointer += 240;
        }
    }
    siblings = r_u32(r_u32(a1 + 16) + 12);
    if (r_u32(a1 + 12) != 0)
    {
        if (r_u8(a1 + 54) == 0)
            for (index = 0; index < 4; ++index)
                if (r_u8(siblings + index * 240 + 55) != 0)
                    goto busy;
        siblings = r_u32(r_u32(a1 + 16) + 12);
        if (r_u32(a1 + 12) != 0)
            goto update;
    }
    if (r_u8(a1 + 55) == 0)
    {
        if (r_u8(r_u32(a1 + 16) + 54) != 0)
            goto busy;
        for (index = 0; index < 4; ++index)
            if (r_u8(siblings + index * 240 + 55) != 0)
                goto busy;
    }
update:
    count = r_u32(a1 + 76);
    state = r_u8(a1 + 70);
    w_u32(a1 + 76, count + 1);
    if (state == 1)
    {
        count = r_u8(a1 + 74);
        if (count >= 2 || r_u8(r_u32(a1 + 16) + 232) == 8)
        {
            w_u8(a1 + 73, 2);
            w_u8(a1 + 70, 255);
            return 255;
        }
        w_u8(a1 + 74, count + 1);
        return count + 1;
    }
    if (state != 0)
    {
        count = r_u8(a1 + 74);
        if (count < 4)
        {
            w_u8(a1 + 74, count + 1);
            return count + 1;
        }
    }
    if (r_u8(a1 + 73) != 0)
    {
        w_u8(r_u32(a1 + 48), 255);
        w_u8(r_u32(a1 + 48) + 1, 0);
        w_u8(a1 + 232, 0);
        w_u8(a1 + 68, 0);
        /* TODO Missing native 9E9BC callback or unknown target */
        (void)FailSelectedControllerCallback(0x8009F5C0, 0x800FEDF8, r_u32(0x800FEDF8), a1, 0);
    }
    result = 8;
    if (type == 0)
    {
        pointer = r_u32(a1 + 48);
        w_u8(a1 + 232, 0);
        w_u8(pointer + 1, 0);
    }
    if (type == 8)
    {
        result = 128;
        if (r_u32(a1 + 12) != 0)
        {
            pointer = r_u32(a1 + 48);
            w_u8(a1 + 232, 8);
            w_u8(pointer + 1, 128);
        }
    }
    return result;
busy:
    if (r_u8(a1 + 74) == 0)
        w_u8(a1 + 74, 1);
    return 1;
}

extern "C" uint32 sub_8009F87C(uint32 a1)
{
    FUNCTION_MARKER(0x8009F87C, "SLUS_003.73");
    uint32 pointer, index, state;
    if (r_u16(a1 + 230) == 0)
        return 1;
    if (r_u32(a1 + 12) != 0)
    {
        if (r_u8(a1 + 70) != 255)
            return 1;
        pointer = r_u32(r_u32(a1 + 16) + 12);
        for (index = 0; index < 4; ++index)
        {
            state = r_u8(pointer + 70);
            if (state != 255 && state != 0)
                return 1;
            pointer += 240;
        }
        return 0;
    }
    if (r_u8(r_u32(a1 + 16) + 70) != 255)
        return 1;
    return r_u8(a1 + 70) != 255;
}

static uint32 FailControllerCallback(uint32 target, uint32 argument, uint32 argument_count)
{
    /* TODO Controller callback needs its actual native contract */
    GameDiagnostic("Missing controller callback target=%08X argument=%08X argc=%u\n", target, argument, (unsigned)argument_count);
    abort();
}

extern "C" uint32 sub_8009CDA0(void)
{
    FUNCTION_MARKER(0x8009CDA0, "SLUS_003.73");
    uint32 port = r_u32(0x800FEE54), target;
    if (!(r_u32(port + 4) & 1))
        return 0;
    if (!(r_u32(port) & 1))
        return 0;
    target = r_u32(0x800FEE1C);
    if (target)
        (void)FailControllerCallback(target, 0, 0);
    return 1;
}

extern "C" uint32 sub_8009E1BC(uint32 controller)
{
    FUNCTION_MARKER(0x8009E1BC, "SLUS_003.73");
    uint32 target;
    uint32 result;
    target = r_u32(0x800FEE0C);
    switch (target)
    {
        case 0x8009F090:
            result = sub_8009F090(controller);
            break;
        default:
            result = FailControllerCallback(target, controller, 1);
    }
    w_u32(0x800FEE74, result);
    return sub_8009D374(controller, 0xFFFFFFFE);
}

extern "C" uint32 sub_8009D374(uint32 controller, uint32 command)
{
    FUNCTION_MARKER(0x8009D374, "SLUS_003.73");
    uint32 port;
    uint32 buffer;
    uint32 received;
    uint32 mode;
    uint32 timer;
    uint32 status;
    uint32 mask_port;
    uint32 counter;
    uint32 offset;
    if ((sint32)command < 0)
    {
        port = r_u32(0x800FEE58);
        buffer = r_u32(controller + 64);
        received = r_u8(port);
        w_u8(controller + 68, 0xFF);
        w_u8(controller + 69, 1);
        w_u8(buffer, ~command);
        port = r_u32(0x800FEE58);
        while ((r_u16(port + 4) & 1) == 0)
        {
        }
        while (sub_8009F94C() == 0)
        {
        }
        port = r_u32(0x800FEE58);
        w_u8(port, ~command);
        return received;
    }
    buffer = r_u32(controller + 60);
    mode = 0x88;
    if ((r_u8(buffer) >> 4) == 8 && r_u8(controller + 68) >= 9)
        mode = 0x22;
    port = r_u32(0x800FEE58);
    timer = r_u16(0x1F801120);
    status = r_u16(port + 4);
    w_u32(0x801086EC, 0x1AE);
    w_u32(0x801086E8, timer);
    if ((status & 2) == 0)
    {
        while ((r_u16(port + 4) & 2) == 0)
        {
        }
    }
    port = r_u32(0x800FEE58);
    mask_port = r_u32(0x800FEE54);
    received = r_u8(port);
    w_u16(port + 14, mode);
    status = r_u32(mask_port);
    if ((status & 0x80) == 0)
    {
        do
        {
            if (sub_8009F94C() != 0)
                return 0xFFFFFFEC;
            mask_port = r_u32(0x800FEE54);
            status = r_u32(mask_port);
        } while ((status & 0x80) == 0);
    }
    port = r_u32(0x800FEE58);
    w_u8(port, command);
    counter = r_u8(controller + 69);
    offset = r_u8(controller + 68);
    buffer = r_u32(controller + 60);
    w_u8(controller + 69, counter + 1);
    w_u8(buffer + offset, received);
    counter = r_u8(controller + 68);
    w_u8(controller + 68, counter + 1);
    return received;
}

extern "C" uint32 sub_8009E2D4(uint32 controller)
{
    FUNCTION_MARKER(0x8009E2D4, "SLUS_003.73");
    uint32 argument;
    uint32 target;
    uint32 mode = 0;
    uint32 result;
    uint32 length;
    if (r_u32(0x800FEE74) != 0)
    {
        argument = r_u32(controller + 12);
        target = r_u32(0x800FEE0C);
        switch (target)
        {
            case 0x8009F090:
                sub_8009F090(argument + 0x1E0);
                break;
            default:
                (void)FailControllerCallback(target, argument + 0x1E0, 1);
        }
        argument = r_u32(controller + 12);
        target = r_u32(0x800FEE0C);
        switch (target)
        {
            case 0x8009F090:
                sub_8009F090(argument + 0x2D0);
                break;
            default:
                (void)FailControllerCallback(target, argument + 0x2D0, 1);
        }
    }
    if (r_u8(controller + 54) == 0)
        mode = r_u32(0x800FEE40);
    result = sub_8009D54C(controller, mode);
    if ((sint32)result < 0)
        return result;
    if ((result & 0xF0) == 0)
        return 0xFFFFFFF7;
    length = (result & 0xF) << 1;
    w_u32(0x800FEE6C, length);
    if (length == 0)
        w_u32(0x800FEE6C, 0x20);
    return 0;
}

extern "C" uint32 sub_8009D810(void)
{
    FUNCTION_MARKER(0x8009D810, "SLUS_003.73");
    uint32 result;
    do
    {
        result = r_u16(r_u32(0x800FEE58) + 4) & 2;
    } while (!result);
    return result;
}

static uint32 FailPadReceiverCallback(uint32 target, uint32 receiver)
{
    if (target == 0x8009E9BC)
        return ResetPadQuery(receiver);
    if (target == 0x8009D8FC)
        return ReadPadModelResponse(receiver);
    if (target == 0x8009DBFC)
        return ReadPadCapabilities(receiver);
    /* TODO Native controller callback adapter */
    GameDiagnostic("Missing controller callback target=%08X receiver=%08X\n", target, receiver);
    abort();
}

extern "C" uint32 sub_8009F240(uint32 a1)
{
    FUNCTION_MARKER(0x8009F240, "SLUS_003.73");
    sint32 result;
    uint32 v3;
    sint32 v4;
    sint32 v5;
    uint32 v6;
    uint32 v7;
    sint8 v8;
    uint32 v9;
    uint32 v10;
    uint32 v11;
    sint32 i;
    sint8 v13;
    sint32 v14;
    sint32 v15;
    uint32 v16;
    sint8 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    uint32 v24;
    if ((((a1 != r_u32(((uint32)(((uint32)(a1) + (uint32)(16)))))) && r_u8(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))))) + (uint32)(1)))))) && (r_u8(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))))) + (uint32)(1))))) != 90)))
        return sub_8009F5C0(a1);
    v3 = r_u32(((uint32)(((uint32)(a1) + (uint32)(48)))));
    v4 = r_u8(((uint32)(((uint32)(a1) + (uint32)(232)))));
    v5 = (r_u8(r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))))) >> 4);
    if ((v4 == 8))
    {
        w_u8(v3, 0);
    }
    else if ((v5 != 15))
    {
        v6 = r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))));
        w_u8(((uint32)(((uint32)(a1) + (uint32)(232)))), v5);
        w_u8(v3, 0);
        v7 = (v3 + (1) * 1);
        v8 = ((sint8)(r_u8(v6)));
        v9 = (v6 + (1) * 1);
        w_u8(v7, v8);
        v10 = (v7 + (1) * 1);
        if ((a1 == r_u32(((uint32)(((uint32)(a1) + (uint32)(16)))))))
        {
            v14 = (v5 != 8);
            v15 = 2;
            if (v14)
            {
                v16 = (v9 + (1) * 1);
                if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(68))))) > 2))
                {
                    do
                    {
                        v17 = r_u8(((v16 += 1) - 1));
                        ++v15;
                        w_u8(((v10 += 1) - 1), v17);
                    } while ((((sint32)(v15)) < r_u8(((uint32)(((uint32)(a1) + (uint32)(68)))))));
                }
            }
            else
            {
                w_u8(((uint32)(((uint32)(a1) + (uint32)(68)))), 2);
            }
        }
        else
        {
            w_u8(((uint32)(((uint32)(a1) + (uint32)(68)))), 8);
            v11 = (v9 + (1) * 1);
            for (i = 2; (((sint32)(i)) < 8); ++i)
            {
                v13 = r_u8(((v11 += 1) - 1));
                w_u8(((v10 += 1) - 1), v13);
            }
        }
    }
    if ((((!(r_u8(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))))) + (uint32)(1)))))) && ((r_u8(((uint32)(((uint32)(a1) + (uint32)(70))))) != 1) || r_u32(((uint32)(((uint32)(a1) + (uint32)(20))))))) && !(r_u8(((uint32)(((uint32)(a1) + (uint32)(80))))))) || ((((r_u8(((uint32)(((uint32)(a1) + (uint32)(232))))) != v4) && !(r_u8(((uint32)(((uint32)(a1) + (uint32)(74))))))) && !sub_8009F87C(a1)) && ((a1 != r_u32(((uint32)(((uint32)(a1) + (uint32)(16))))))) ? (v18 = r_u8(((uint32)(((uint32)(a1) + (uint32)(56)))))) : (v18 = r_u8(((uint32)(((uint32)(a1) + (uint32)(55)))))), !v18)))
    {
        FailPadReceiverCallback(r_u32(0x800FEDF8), a1);
    }
    v19 = r_u8(((uint32)(((uint32)(a1) + (uint32)(70)))));
    result = 255;
    w_u8(((uint32)(((uint32)(a1) + (uint32)(74)))), 0);
    if ((v19 != 255))
    {
        if (((((unsigned char)(((uint32)(v19) - (uint32)(2)))) < 0xFC) && (r_u8(r_u32(((uint32)(((uint32)(a1) + (uint32)(60)))))) != 243)))
            FailPadReceiverCallback(r_u32(0x800FEDF8), a1);
        v20 = r_u8(((uint32)(((uint32)(a1) + (uint32)(70)))));
        if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(70))))) && (v20 != 255)))
        {
            if ((a1 == r_u32(((uint32)(((uint32)(a1) + (uint32)(16)))))))
                result = r_u8(((uint32)(((uint32)(a1) + (uint32)(54)))));
            else
                result = r_u8(((uint32)(((uint32)(a1) + (uint32)(55)))));
            if (!result)
                return result;
            v20 = r_u8(((uint32)(((uint32)(a1) + (uint32)(70)))));
        }
        if ((v20 == 1))
        {
            v23 = r_u8(((uint32)(((uint32)(a1) + (uint32)(70)))));
            w_u8(((uint32)(((uint32)(a1) + (uint32)(71)))), 0);
            goto LABEL_52;
        }
        result = 254;
        if ((((sint32)(v20)) >= 2))
        {
            result = 255;
            if ((v20 == 254))
            {
                w_u8(((uint32)(((uint32)(a1) + (uint32)(70)))), -1);
                return result;
            }
            if ((v20 == 255))
                return result;
            v24 = ((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(24)))))));
            if (v24)
                result = FailPadReceiverCallback(v24, a1);
            else
                result = FailPadReceiverCallback(0x8009D8FC, a1);
            v22 = r_u8(a1 + 70) + (uint32)result;
            goto LABEL_58;
        }
        if (r_u8(((uint32)(((uint32)(a1) + (uint32)(232))))))
        {
            v21 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
            if (v21 || (result = 8, !r_u8(a1 + 54)))
            {
                if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(232))))) == 8))
                {
                    v22 = r_u8(r_u32(((uint32)(((uint32)(v21) + (uint32)(60))))));
                    result = 2;
                    if ((v22 == 255))
                    {
                        w_u8(((uint32)(((uint32)(a1) + (uint32)(73)))), 2);
                    LABEL_58:
                        w_u8(((uint32)(((uint32)(a1) + (uint32)(70)))), v22);

                        return result;
                    }
                }
                v23 = r_u8(((uint32)(((uint32)(a1) + (uint32)(70)))));
                w_u8(((uint32)(((uint32)(a1) + (uint32)(73)))), 1);
            LABEL_52:
                result = ((uint32)(v23) + (uint32)(1));

                w_u8(((uint32)(((uint32)(a1) + (uint32)(70)))), result);
            }
        }
    }
    return result;
}

static void FailControllerGlobalCallback(uint32 target)
{
    /* TODO Supply the controller global callback adapter */
    GameDiagnostic("Missing controller global callback target=%08X\n", target);
    abort();
}

/* Unverified draft C; integration remains TODO */

extern "C" uint32 sub_8009F05C(void)
{
    FUNCTION_MARKER(0x8009F05C, "SLUS_003.73");
    w_u32(0x800FEE0C, 0x8009F090);
    w_u32(0x800FEE10, 0x8009F87C);
    w_u32(0x800FEE14, 0x8009F240);
    return 0x8009F240;
}

extern "C" uint32 sub_8009F090(uint32 controller)
{
    uint32 child;
    uint32 parent;
    uint32 entry;
    uint32 pointer;
    uint32 index;
    uint32 state;
    uint32 target;
    FUNCTION_MARKER(0x8009F090, "SLUS_003.73");
    child = r_u32(controller + 12);
    if (child == 0)
    {
        if (r_u8(controller + 55) != 0)
            return 0;
        parent = r_u32(controller + 16);
        entry = r_u32(parent + 12) + 44;
        for (index = 0; index < 4; ++index)
        {
            if (r_u8(entry + 10) == 0x43 || r_u8(entry + 11) == 0x43)
            {
                pointer = r_u32(entry);
                if (r_u8(pointer) == 1)
                    return 0;
            }
            entry += 240;
        }
    }
    pointer = r_u32(controller + 60);
    if (r_u8(pointer) == 0xF3 && r_u8(controller + 232) == 0)
    {
        sub_8009E108(controller, 0);
        return 0;
    }
    state = r_u8(controller + 70);
    if (state == 1)
    {
        sub_8009E108(controller, 1);
        return 0;
    }
    if (state == 0)
        return 0;
    if (state == 0xFE)
    {
        sub_8009E108(controller, 0);
        return 0;
    }
    if (state == 0xFF)
    {
        if (r_u8(controller + 232) != 8)
            return 0;
        child = r_u32(controller + 12);
        if (child == 0)
            return 0;
        for (index = 0; index < 4; ++index)
        {
            if (r_u8(child + 70) == 1)
            {
                sub_8009F090(child);
                return 0;
            }
            child += 240;
        }
        return 1;
    }
    target = r_u32(controller + 20);
    if (target != 0)
    {
        switch (target)
        {
            case 0x8009DB54:
                sub_8009DB54(controller);
                break;
            case 0x8009DED8:
                sub_8009DED8(controller);
                break;
            case 0x8009E054:
                sub_8009E054(controller);
                break;
            default:
                xport_draft_guest_call1(target, controller);
                break;
        }
        return 0;
    }
    sub_8009D878(controller);
    return 0;
}

extern "C" uint32 sub_8009E1FC(uint32 controller)
{
    uint32 first;
    uint32 selected;
    uint32 target;
    uint32 argument;
    uint32 command;
    FUNCTION_MARKER(0x8009E1FC, "SLUS_003.73");
    first = r_u32(0x800FEE34);
    selected = r_u32(0x800FEE44);
    if (first == selected && r_u32(0x800FEE30) != 0)
    {
        target = r_u32(0x800FEE24);
        FailControllerGlobalCallback(target);
        FailControllerGlobalCallback(r_u32(0x800FEE20));
    }
    if (r_u32(0x800FEE74) != 0)
    {
        argument = r_u32(controller + 12);
        target = r_u32(0x800FEE0C);
        switch (target)
        {
            case 0x8009F090:
                sub_8009F090(argument);
                break;
            default:
                xport_draft_guest_call1(target, argument);
                break;
        }
        argument = r_u32(controller + 12);
        target = r_u32(0x800FEE0C);
        switch (target)
        {
            case 0x8009F090:
                sub_8009F090(argument + 240);
                break;
            default:
                xport_draft_guest_call1(target, argument + 240);
                break;
        }
    }
    command = r_u8(controller + 54);
    if (command == 0)
        command = 0x42;
    else
        command = r_u8(controller + 54);
    return sub_8009D54C(controller, command);
}

extern "C" uint32 sub_8009E394(uint32 controller)
{
    uint32 pointer;
    uint32 mode = 0;
    uint32 target;
    uint32 result;
    FUNCTION_MARKER(0x8009E394, "SLUS_003.73");
    pointer = r_u32(controller + 60);
    if ((r_u8(pointer) >> 4) == 8)
        mode = r_u8(controller + 54) == 0;
    target = r_u32(0x800FEDFC);
    switch (target)
    {
        case 0x8009EBD4:
            result = sub_8009EBD4(controller, mode);
            break;
        default:
            result = xport_draft_guest_call2(target, controller, mode);
            break;
    }
    result = sub_8009D54C(controller, result & 0xFF);
    if (result == 0x5A || result == 0 || (sint32)result < 0)
        return result;
    return 0xFFFFFFF7;
}

static void FailControllerTransferCallback(uint32 slot, uint32 target, uint32 argument0, uint32 argument1)
{
    /* TODO Supply the exact controller callback target */
    GameDiagnostic("Controller callback missing slot=%08X target=%08X a0=%08X a1=%08X\n", slot, target, argument0, argument1);
    abort();
}

static void FailControllerCarriedState(uint32 target, uint32 counter, uint32 index)
{
    /* TODO Resolve an asynchronous transition without a carried controller */
    GameDiagnostic("Controller carry missing slot=800FEE14 target=%08X counter=%08X index=%08X\n", target, counter, index);
    abort();
}

extern "C" uint32 sub_8009E420(uint32 controller)
{
    uint32 target;
    uint32 mode;
    uint32 pointer;
    uint32 count;
    uint32 index;
    uint32 stride;
    uint32 argument;
    uint32 result;
    uint32 counter_address;
    uint32 offset;
    uint32 source;
    uint32 buffer;
    FUNCTION_MARKER(0x8009E420, "SLUS_003.73");
    target = r_u32(0x800FEE00);
    if (target != 0x8009ECE4)
        FailControllerTransferCallback(0x800FEE00, target, controller, 0);
    sub_8009ECE4(controller);
    mode = 0;
    if (r_u32(0x800FEE40) != 0)
    {
        pointer = r_u32(controller + 60);
        if ((r_u8(pointer) >> 4) == 8)
            mode = r_u8(controller + 54) == 0;
    }
    if (mode != 0)
    {
        index = 0xFFFFFFFF;
        stride = 0xFFFFFF10;
        do
        {
            count = r_u32(0x800FEE6C) - 1;
            w_u32(0x800FEE6C, count);
            if ((sint32)count <= 0)
                break;
            if ((sint32)index >= 0)
            {
                argument = r_u32(controller + 12);
                target = r_u32(0x800FEE00);
                if (target != 0x8009ECE4)
                    FailControllerTransferCallback(0x800FEE00, target, argument + stride, 0);
                sub_8009ECE4(argument + stride);
            }
            target = r_u32(0x800FEDFC);
            if (target != 0x8009EBD4)
                FailControllerTransferCallback(0x800FEDFC, target, controller, 1);
            result = sub_8009EBD4(controller, 1);
            result = sub_8009D54C(controller, result & 0xFF);
            if ((sint32)result < 0)
                return result;
            sub_8009F92C(60);
            result = sub_8009D780();
            index += 1;
            if (result == 0)
                return 0xFFFFFFFD;
            stride += 240;
        } while ((sint32)index < 4);
    }
    index = r_u32(0x800FEE34);
    count = r_u32(0x800FEE6C);
    if ((sint32)count >= 2)
    {
        index = index == 0;
        counter_address = 0x800FEE4C + index * 4;
        stride = index * 240;
        do
        {
            count = r_u32(counter_address);
            if ((sint32)count < 0)
                break;
            if ((sint32)count > 0)
            {
                uint32 previous_controller;
                pointer = r_u32(0x800FEE28);
                pointer = r_u32(pointer + stride + 12);
                previous_controller = pointer + count * 240 - 240;
                target = r_u32(0x800FEE14);
                if (target != 0x8009F240)
                    FailControllerTransferCallback(0x800FEE14, target, previous_controller, 0);
                sub_8009F240(previous_controller);
                count = r_u32(counter_address);
                if (count == 3)
                {
                    target = r_u32(0x800FEE14);
                    if (target != 0x8009F240)
                        FailControllerTransferCallback(0x800FEE14, target, previous_controller - 240, 0);
                    sub_8009F240(previous_controller - 240);
                    w_u32(counter_address, 1);
                    goto controller_receive;
                }
            }
            else
                count = r_u32(counter_address);
            if (count == 3)
            {
                /* TODO Define asynchronous zero-to-three counter transition */
                FailControllerCarriedState(r_u32(0x800FEE14), counter_address, count);
            }
            else if (count == 4)
                w_u32(counter_address, 3);
            else if ((sint32)count >= 0 && (sint32)count < 2)
            {
                pointer = r_u32(0x800FEE28);
                pointer += stride;
                target = r_u32(0x800FEE14);
                if (target != 0x8009F240)
                    FailControllerTransferCallback(0x800FEE14, target, pointer, 0);
                sub_8009F240(pointer);
                target = r_u32(0x800FEE18);
                if (target != 0x8009EB90)
                    FailControllerTransferCallback(0x800FEE18, target, pointer, 0);
                sub_8009EB90(pointer);
                w_u32(counter_address, 0xFFFFFFFF);
            }
        controller_receive:
            target = r_u32(0x800FEDFC);
            if (target != 0x8009EBD4)
                FailControllerTransferCallback(0x800FEDFC, target, controller, mode);
            result = sub_8009EBD4(controller, mode);
            result = sub_8009D374(controller, result & 0xFF);
            if ((sint32)result < 0)
                return result;
            sub_8009F92C(60);
            if (sub_8009D780() == 0)
                return 0xFFFFFFFD;
            count = r_u32(0x800FEE6C) - 1;
            w_u32(0x800FEE6C, count);
        } while ((sint32)count >= 2);
    }
    for (;;)
    {
        count = r_u32(0x800FEE6C) - 1;
        w_u32(0x800FEE6C, count);
        if ((sint32)count <= 0)
            break;
        target = r_u32(0x800FEDFC);
        if (target != 0x8009EBD4)
            FailControllerTransferCallback(0x800FEDFC, target, controller, mode);
        result = sub_8009EBD4(controller, mode);
        result = sub_8009D374(controller, result & 0xFF);
        if ((sint32)result < 0)
            return result;
        sub_8009F92C(60);
        if (sub_8009D780() == 0)
            return 0xFFFFFFFD;
    }
    sub_8009D810();
    offset = r_u8(controller + 68);
    w_u8(controller + 68, offset + 1);
    source = r_u32(0x800FEE70);
    buffer = r_u32(controller + 60);
    result = r_u8(source);
    w_u8(buffer + offset, result);
    target = r_u32(0x800FEDF4);
    if (target != 0x8009EA80)
        FailControllerTransferCallback(0x800FEDF4, target, 0, 0);
    sub_8009EA80(0);
    return 0;
}

// Recovered SDK command builders use the original controller packet fields
static uint32 QueuePadByteCommand(uint32 controller, uint32 command, uint32 value)
{
    w_u8(controller + 54, command);
    w_u32(controller + 44, controller + 36);
    w_u8(controller + 36, value);
    w_u8(controller + 53, 1);
    return 1;
}

static uint32 QueuePadModelQuery(uint32 controller)
{
    FUNCTION_MARKER(0x8009E128, "SLUS_003.73");
    w_u8(controller + 54, 69);
    w_u32(controller + 44, 0);
    w_u8(controller + 53, 0);
    return 69;
}

static uint32 QueuePadActuatorQuery(uint32 controller, uint32 value)
{
    FUNCTION_MARKER(0x8009E13C, "SLUS_003.73");
    return QueuePadByteCommand(controller, 76, value);
}

static uint32 QueuePadCombinationQuery(uint32 controller, uint32 value)
{
    FUNCTION_MARKER(0x8009E15C, "SLUS_003.73");
    return QueuePadByteCommand(controller, 70, value);
}

static uint32 QueuePadModeQuery(uint32 controller, uint32 value)
{
    FUNCTION_MARKER(0x8009E17C, "SLUS_003.73");
    return QueuePadByteCommand(controller, 71, value);
}

static uint32 QueuePadCurrentModeQuery(uint32 controller)
{
    FUNCTION_MARKER(0x8009E19C, "SLUS_003.73");
    w_u8(controller + 54, 75);
    w_u32(controller + 44, 0);
    w_u8(controller + 53, 0);
    return 75;
}

extern "C" uint32 sub_8009E108(uint32 controller, uint32 mode)
{
    FUNCTION_MARKER(0x8009E108, "SLUS_003.73");
    return QueuePadByteCommand(controller, 67, mode);
}

extern "C" uint32 sub_8009D878(uint32 controller)
{
    uint32 stage = r_u8(controller + 70);
    FUNCTION_MARKER(0x8009D878, "SLUS_003.73");
    if (stage == 3)
        return QueuePadActuatorQuery(controller, r_u8(controller + 228));
    if (stage < 4)
        return stage == 2 ? QueuePadModelQuery(controller) : 2;
    return stage == 4 ? QueuePadModeQuery(controller, r_u8(controller + 71)) : 4;
}

extern "C" uint32 sub_8009DB54(uint32 controller)
{
    uint32 stage = r_u8(controller + 70);
    FUNCTION_MARKER(0x8009DB54, "SLUS_003.73");
    if (stage == 3)
        return QueuePadCombinationQuery(controller, r_u8(controller + 71));
    if (stage < 4)
        return stage == 2 ? QueuePadActuatorQuery(controller, r_u8(controller + 71)) : 2;
    if (stage != 4)
        return 4;
    return r_u8(controller + 72) ? QueuePadCurrentModeQuery(controller) : QueuePadModeQuery(controller, r_u8(controller + 71));
}

extern "C" uint32 sub_8009DED8(uint32 controller)
{
    uint32 data = r_u32(controller + 32);
    FUNCTION_MARKER(0x8009DED8, "SLUS_003.73");
    w_u8(controller + 54, 77);
    w_u8(controller + 53, 6);
    w_u32(controller + 44, data);
    return 6;
}

extern "C" uint32 sub_8009E054(uint32 controller)
{
    uint32 stage = r_u8(controller + 70);
    FUNCTION_MARKER(0x8009E054, "SLUS_003.73");
    if (stage == 2)
    {
        w_u8(controller + 54, 68);
        w_u32(controller + 44, controller + 81);
        w_u8(controller + 53, stage);
        return controller + 81;
    }
    if (stage != 3)
        return 77;
    w_u8(controller + 54, 77);
    w_u32(controller + 44, controller + 93);
    w_u8(controller + 53, 6);
    return 6;
}

extern "C" uint32 sub_8009EB90(uint32 controller)
{
    uint32 previous = r_u8(controller + 54), child = r_u32(controller + 12) + 54;
    FUNCTION_MARKER(0x8009EB90, "SLUS_003.73");
    w_u8(controller + 54, 0);
    w_u8(controller + 55, previous);
    for (uint32 index = 0; index < 4; ++index)
    {
        uint32 older = r_u8(child + 1), current = r_u8(child);
        w_u8(child, 0);
        w_u8(child + 2, older);
        w_u8(child + 1, current);
        child += 240;
    }
    return 0;
}

extern "C" uint32 sub_8009EA80(uint32 reason)
{
    uint32 result, controller, index;
    FUNCTION_MARKER(0x8009EA80, "SLUS_003.73");
    do
    {
        index = r_u32(0x800FEE34);
        controller = 0x80107D88 + index * 240;
        if (reason != 0xFFFFFFF7)
        {
            if (reason)
            {
                sub_8009F5C0(controller);
                sub_8009EB90(controller);
            }
            else
                w_u32(0x800FEE4C + index * 4, (r_u8(r_u32(controller + 60)) >> 4) == 8 ? 4 : 0);
        }
        uint32 port = r_u32(0x800FEE8C);
        index = r_u32(0x800FEE34);
        w_u32(0x800FEE38, 0);
        w_u16(port + 10, 0);
        ++index;
        w_u32(0x800FEE34, index);
        result = (sint32)r_u32(0x800FEE48) < (sint32)index ? 1 : sub_8009D098(0x80107D88 + index * 240);
        reason = 65535;
    } while (!result);
    return result;
}

extern "C" uint32 sub_8009EBD4(uint32 controller, uint32 mode)
{
    sint32 byte = (sint32)r_u8(controller + 69) - 3;
    FUNCTION_MARKER(0x8009EBD4, "SLUS_003.73");
    if (mode)
    {
        sint32 group = byte / 8;
        byte = byte - group * 8 - 2;
        controller = r_u32(controller + 12) + (uint32)group * 240;
    }
    uint32 command = r_u8(controller + 54);
    if (byte < 0)
        return byte == -2 ? (command ? command : 66) : 0;
    if (!command)
    {
        if (byte < 6 && !r_u8(controller + (uint32)byte + 87))
            return 0;
        return byte < r_u8(controller + 52) ? r_u8(r_u32(controller + 40) + (uint32)byte) : 0;
    }
    if (byte >= r_u8(controller + 53))
        return command == 77 ? 255 : 0;
    return r_u8(r_u32(controller + 44) + (uint32)byte);
}

extern "C" uint32 sub_8009ECE4(uint32 controller)
{
    uint32 count, kind, data, value, result = 1;
    FUNCTION_MARKER(0x8009ECE4, "SLUS_003.73");
    for (uint32 index = 0; index < 6; ++index)
        w_u8(controller + 87 + index, 0);
    if (r_u16(controller + 230) && r_u32(controller + 40))
    {
        count = r_u8(controller + 52);
        if (count >= 7)
            count = 6;
        result = r_u8(controller + 233);
        for (uint32 group = 0; group < r_u8(controller + 233); ++group)
        {
            uint32 row = r_u32(controller + 4) + group * 5;
            uint32 mask = r_u8(row + 2) ? 255 : 1;
            uint32 enabled = 0;
            data = r_u32(controller + 40);
            for (uint32 index = 0; index < count; ++index)
            {
                if (r_u8(controller + 93 + index) == group && (r_u8(data + index) & mask))
                {
                    enabled = 1;
                    break;
                }
            }
            if (enabled)
            {
                value = r_u32(0x800FEE3C) + r_u8(row + 3);
                if ((sint32)value < 61)
                {
                    w_u32(0x800FEE3C, value);
                    for (uint32 index = 0; index < count; ++index)
                        if (r_u8(controller + 93 + index) == group)
                            w_u8(controller + 87 + index, 1);
                }
            }
            result = group + 1 < r_u8(controller + 233);
        }
        return result;
    }
    kind = r_u8(controller + 232);
    if ((kind == 4 || kind == 5 || kind == 7) && !r_u16(controller + 230) && r_u8(controller + 52) >= 2)
    {
        data = r_u32(controller + 40);
        result = r_u8(data) & 192;
        if (result == 64)
        {
            result = r_u8(data + 1) & 1;
            if (result && (sint32)(r_u32(0x800FEE3C) + 10) < 61)
            {
                w_u8(controller + 88, 1);
                w_u8(controller + 87, 1);
                result = r_u32(0x800FEE3C) + 10;
                w_u32(0x800FEE3C, result);
            }
        }
        return result;
    }
    if (kind == 3)
        w_u8(controller + 87, 1);
    else if (!r_u16(controller + 230))
        for (sint32 index = 5; index >= 0; --index)
            w_u8(controller + 87 + (uint32)index, 1);
    return 1;
}

extern "C" uint32 sub_8009F92C(uint32 duration)
{
    uint32 counter = r_u16(0x1F801120);
    FUNCTION_MARKER(0x8009F92C, "SLUS_003.73");
    w_u32(0x801086EC, duration);
    w_u32(0x801086E8, counter);
    return counter;
}

extern "C" uint32 sub_8009F94C(void)
{
    uint32 counter = r_u16(0x1F801120), start = r_u32(0x801086E8);
    FUNCTION_MARKER(0x8009F94C, "SLUS_003.73");
    if (counter < start)
    {
        uint32 target = r_u16(0x1F801128);
        counter += target ? r_u16(0x1F801128) : 65536;
    }
    uint32 mode = r_u16(0x1F801124) & 512;
    uint32 duration = r_u32(0x801086EC);
    uint32 elapsed = counter - r_u32(0x801086E8);
    return (mode ? elapsed : elapsed >> 3) >= duration;
}

static uint32 PadCapabilityStorageSize(uint32 controller)
{
    FUNCTION_MARKER(0x8009DA48, "SLUS_003.73");
    uint32 models = r_u8(controller + 227), actuators = r_u8(controller + 233);
    uint32 tail = r_u32(controller + 236);
    return ((models + 1) >> 1) * 4 + ((actuators * 5 + 3) & 0xFFC) + 4 + tail;
}

static uint32 BindPadCapabilityStorage(uint32 controller, uint32 storage)
{
    FUNCTION_MARKER(0x8009DA80, "SLUS_003.73");
    if (!storage || r_u32(controller + 4))
        return 0;
    uint32 target = r_u32(0x800FEE10);
    if (target != 0x8009F87C)
        return FailControllerCallback(target, controller, 1);
    if (sub_8009F87C(controller))
        return 0;
    storage = (uint32)((sint32)(storage + 3) >> 2) * 4;
    w_u8(controller + 73, 4);
    w_u8(controller + 70, 1);
    w_u32(controller + 20, 0x8009DB54);
    uint32 models = r_u8(controller + 227);
    w_u32(controller + 24, 0x8009DBFC);
    uint32 actuators = r_u8(controller + 233);
    w_u32(controller, storage);
    w_u8(controller + 71, 0);
    storage += ((models + 1) >> 1) * 4;
    w_u32(controller + 4, storage);
    w_u32(controller + 8, storage + ((actuators * 5 + 3) & 0xFFC));
    return 1;
}

static uint32 ReadPadModelResponse(uint32 controller)
{
    FUNCTION_MARKER(0x8009D8FC, "SLUS_003.73");
    uint32 stage = r_u8(controller + 70), packet;
    if (stage == 2)
    {
        packet = r_u32(controller + 60);
        w_u8(controller + 227, r_u8(packet + 3));
        uint32 value = r_u8(r_u32(controller + 60) + 4);
        w_u16(controller + 230, 0);
        w_u8(controller + 228, value);
        w_u8(controller + 233, r_u8(r_u32(controller + 60) + 5));
        value = r_u8(r_u32(controller + 60) + 6);
        w_u32(controller + 236, 0);
        w_u8(controller + 234, value);
        return 1;
    }
    if (stage == 3)
    {
        packet = r_u32(controller + 60);
        uint32 value = r_u8(packet + 4) * 256 + r_u8(packet + 5);
        w_u8(controller + 71, 0);
        w_u16(controller + 230, value);
        return 1;
    }
    if (stage != 4)
        return 1;
    uint32 total = r_u32(controller + 236);
    uint32 index = r_u8(controller + 71) + 1;
    packet = r_u32(controller + 60);
    uint32 size = r_u8(packet + 4);
    w_u8(controller + 71, index);
    total += 8 + ((size + 3) & 0x1FC);
    w_u32(controller + 236, total);
    if ((index & 255) < r_u8(controller + 234))
        return 0;
    if ((sint32)PadCapabilityStorageSize(controller) >= 129)
    {
        w_u8(controller + 70, 254);
        w_u8(controller + 73, 2);
    }
    else
    {
        w_u8(controller + 70, 255);
        BindPadCapabilityStorage(controller, controller + 99);
        w_u8(controller + 70, 2);
    }
    return 0;
}

static uint32 ResetPadQuery(uint32 controller)
{
    FUNCTION_MARKER(0x8009E9BC, "SLUS_003.73");
    if (!r_u8(controller + 73))
        return 8;
    if (r_u8(controller + 232) == 8)
    {
        uint32 child = r_u32(controller + 12);
        for (uint32 index = 0; index < 4; ++index)
            ResetPadQuery(child + index * 240);
    }
    w_u8(controller + 73, 0);
    w_u8(controller + 70, 0);
    w_u16(controller + 230, 0);
    w_u32(controller + 20, 0);
    w_u32(controller + 24, 0);
    w_u8(controller + 227, 0);
    w_u8(controller + 228, 0);
    w_u16(controller + 230, 0);
    w_u8(controller + 233, 0);
    w_u8(controller + 234, 0);
    w_u32(controller, 0);
    w_u32(controller + 4, 0);
    w_u32(controller + 8, 0);
    for (uint32 index = 0; index < 6; ++index)
        w_u8(controller + 93 + index, 255);
    return controller + 99;
}

static uint32 ReadPadCapabilities(uint32 controller)
{
    FUNCTION_MARKER(0x8009DBFC, "SLUS_003.73");
    uint32 stage = r_u8(controller + 70), index, packet, row, value;
    if (stage == 2)
    {
        index = r_u8(controller + 71);
        row = r_u32(controller) + index * 2;
        packet = r_u32(controller + 60);
        w_u16(row, r_u8(packet + 4) * 256 + r_u8(packet + 5));
        index = (r_u8(controller + 71) + 1) & 255;
        w_u8(controller + 71, index);
        if (index < r_u8(controller + 227))
            return 0;
        w_u8(controller + 71, 0);
        return 1;
    }
    if (stage == 3)
    {
        index = r_u8(controller + 71);
        row = r_u32(controller + 4) + index * 5;
        w_u8(row, r_u8(r_u32(controller + 60) + 4));
        w_u8(row + 1, r_u8(r_u32(controller + 60) + 5) & 127);
        w_u8(row + 2, r_u8(r_u32(controller + 60) + 6));
        w_u8(row + 3, r_u8(r_u32(controller + 60) + 7));
        w_u8(row + 4, r_u8(r_u32(controller + 60) + 5) >> 7);
        index = (r_u8(controller + 71) + 1) & 255;
        w_u8(controller + 71, index);
        if (index < r_u8(controller + 233))
            return 0;
        w_u8(controller + 71, 0);
        w_u8(controller + 72, 0);
        return 1;
    }
    if (stage != 4)
        return 1;
    index = r_u8(controller + 71);
    row = r_u32(controller + 8) + index * 8;
    uint32 bytes;
    if (!r_u8(controller + 72))
    {
        value = r_u8(r_u32(controller + 60) + 4);
        w_u8(controller + 72, value);
        w_u8(row, value);
        packet = r_u32(controller + 60) + 5;
        if (!r_u8(controller + 71))
            value = r_u32(controller + 8) + r_u8(controller + 234) * 8;
        else
            value = r_u32(row - 4) + ((r_u8(row - 8) + 3) & 0x1FC);
        w_u32(row + 4, value);
        w_u32(0x80107CE8, value);
        bytes = 3;
    }
    else
    {
        packet = r_u32(controller + 60) + 2;
        bytes = 6;
    }
    while (bytes-- && r_u8(controller + 72))
    {
        uint32 destination = r_u32(0x80107CE8);
        w_u8(destination, r_u8(packet++));
        w_u32(0x80107CE8, destination + 1);
        w_u8(controller + 72, r_u8(controller + 72) - 1);
    }
    if (r_u8(controller + 72))
        return 0;
    index = (r_u8(controller + 71) + 1) & 255;
    w_u8(controller + 71, index);
    if (index >= r_u8(controller + 234))
    {
        w_u8(controller + 73, 6);
        w_u8(controller + 70, 254);
    }
    else
        w_u8(controller + 72, 0);
    return 0;
}

#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>

/* Unverified draft C; integration remains TODO */

uint32 sub_8009F05C(void)
{
    FUNCTION_MARKER(0x8009F05Cu, "SLUS_003.73");
    w_u32(0x800FEE0Cu, 0x8009F090u);
    w_u32(0x800FEE10u, 0x8009F87Cu);
    w_u32(0x800FEE14u, 0x8009F240u);
    return 0x8009F240u;
}

uint32 sub_8009F090(uint32 controller)
{
    uint32 child;
    uint32 parent;
    uint32 entry;
    uint32 pointer;
    uint32 index;
    uint32 state;
    uint32 target;
    FUNCTION_MARKER(0x8009F090u, "SLUS_003.73");
    child = r_u32(controller + 12u);
    if (child == 0u)
    {
        if (r_u8(controller + 55u) != 0u)
            return 0u;
        parent = r_u32(controller + 16u);
        entry = r_u32(parent + 12u) + 44u;
        for (index = 0u; index < 4u; ++index)
        {
            if (r_u8(entry + 10u) == 0x43u || r_u8(entry + 11u) == 0x43u)
            {
                pointer = r_u32(entry);
                if (r_u8(pointer) == 1u)
                    return 0u;
            }
            entry += 240u;
        }
    }
    pointer = r_u32(controller + 60u);
    if (r_u8(pointer) == 0xF3u && r_u8(controller + 232u) == 0u)
    {
        sub_8009E108(controller, 0u);
        return 0u;
    }
    state = r_u8(controller + 70u);
    if (state == 1u)
    {
        sub_8009E108(controller, 1u);
        return 0u;
    }
    if (state == 0u)
        return 0u;
    if (state == 0xFEu)
    {
        sub_8009E108(controller, 0u);
        return 0u;
    }
    if (state == 0xFFu)
    {
        if (r_u8(controller + 232u) != 8u)
            return 0u;
        child = r_u32(controller + 12u);
        if (child == 0u)
            return 0u;
        for (index = 0u; index < 4u; ++index)
        {
            if (r_u8(child + 70u) == 1u)
            {
                sub_8009F090(child);
                return 0u;
            }
            child += 240u;
        }
        return 1u;
    }
    target = r_u32(controller + 20u);
    if (target != 0u)
    {
        switch (target)
        {
        case 0x8009DB54u:
            sub_8009DB54(controller);
            break;
        case 0x8009DED8u:
            sub_8009DED8(controller);
            break;
        case 0x8009E054u:
            sub_8009E054(controller);
            break;
        default:
            /* TODO Resolve unsupported callback target or adapter */
        abort();
        }
        return 0u;
    }
    sub_8009D878(controller);
    return 0u;
}

uint32 sub_8009E1FC(uint32 controller)
{
    uint32 first;
    uint32 selected;
    uint32 target;
    uint32 argument;
    uint32 command;
    FUNCTION_MARKER(0x8009E1FCu, "SLUS_003.73");
    first = r_u32(0x800FEE34u);
    selected = r_u32(0x800FEE44u);
    if (first == selected && r_u32(0x800FEE30u) != 0u)
    {
        target = r_u32(0x800FEE24u);
        /* TODO Dispatch FEE24 then FEE20 and recover carried callback argument */
        (void)target;
        /* TODO Resolve unsupported callback target or adapter */
        abort();
    }
    if (r_u32(0x800FEE74u) != 0u)
    {
        argument = r_u32(controller + 12u);
        target = r_u32(0x800FEE0Cu);
        switch (target)
        {
        case 0x8009F090u:
            sub_8009F090(argument);
            break;
        default:
            /* TODO Resolve unsupported callback target or adapter */
        abort();
        }
        argument = r_u32(controller + 12u);
        target = r_u32(0x800FEE0Cu);
        switch (target)
        {
        case 0x8009F090u:
            sub_8009F090(argument + 240u);
            break;
        default:
            /* TODO Resolve unsupported callback target or adapter */
        abort();
        }
    }
    command = r_u8(controller + 54u);
    if (command == 0u)
        command = 0x42u;
    else
        command = r_u8(controller + 54u);
    return sub_8009D54C(controller, command);
}

uint32 sub_8009E394(uint32 controller)
{
    uint32 pointer;
    uint32 mode = 0u;
    uint32 target;
    uint32 result;
    FUNCTION_MARKER(0x8009E394u, "SLUS_003.73");
    pointer = r_u32(controller + 60u);
    if ((r_u8(pointer) >> 4) == 8u)
        mode = r_u8(controller + 54u) == 0u;
    target = r_u32(0x800FEDFCu);
    switch (target)
    {
    case 0x8009EBD4u:
        result = sub_8009EBD4(controller, mode);
        break;
    default:
        /* TODO Resolve unsupported callback target or adapter */
        abort();
    }
    result = sub_8009D54C(controller, result & 0xFFu);
    if (result == 0x5Au || result == 0u || (sint32)result < 0)
        return result;
    return 0xFFFFFFF7u;
}

uint32 sub_8009E420(uint32 controller)
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
    /* TODO Recover inherited controller value on ambiguous callback paths */
    uint32 previous_controller = 0u;
    uint32 previous_controller_valid = 0u;
    uint32 offset;
    uint32 source;
    uint32 buffer;
    FUNCTION_MARKER(0x8009E420u, "SLUS_003.73");
    target = r_u32(0x800FEE00u);
    if (target != 0x8009ECE4u)
        /* TODO Resolve unsupported callback target or adapter */
        abort();
    sub_8009ECE4(controller);
    mode = 0u;
    if (r_u32(0x800FEE40u) != 0u)
    {
        pointer = r_u32(controller + 60u);
        if ((r_u8(pointer) >> 4) == 8u)
            mode = r_u8(controller + 54u) == 0u;
    }
    if (mode != 0u)
    {
        index = 0xFFFFFFFFu;
        stride = 0xFFFFFF10u;
        do
        {
            count = r_u32(0x800FEE6Cu) - 1u;
            w_u32(0x800FEE6Cu, count);
            if ((sint32)count <= 0)
                break;
            if ((sint32)index >= 0)
            {
                argument = r_u32(controller + 12u);
                target = r_u32(0x800FEE00u);
                if (target != 0x8009ECE4u)
                    /* TODO Resolve unsupported callback target or adapter */
        abort();
                sub_8009ECE4(argument + stride);
            }
            target = r_u32(0x800FEDFCu);
            if (target != 0x8009EBD4u)
                /* TODO Resolve unsupported callback target or adapter */
        abort();
            result = sub_8009EBD4(controller, 1u);
            result = sub_8009D54C(controller, result & 0xFFu);
            if ((sint32)result < 0)
                return result;
            sub_8009F92C(60u);
            result = sub_8009D780();
            index += 1u;
            if (result == 0u)
                return 0xFFFFFFFDu;
            stride += 240u;
        } while ((sint32)index < 4);
    }
    index = r_u32(0x800FEE34u);
    count = r_u32(0x800FEE6Cu);
    if ((sint32)count >= 2)
    {
        index = index == 0u;
        counter_address = 0x800FEE4Cu + index * 4u;
        stride = index * 240u;
        do
        {
            count = r_u32(counter_address);
            if ((sint32)count < 0)
                break;
            if ((sint32)count > 0)
            {
                pointer = r_u32(0x800FEE28u);
                pointer = r_u32(pointer + stride + 12u);
                previous_controller = pointer + count * 240u - 240u;
                previous_controller_valid = 1u;
                target = r_u32(0x800FEE14u);
                if (target != 0x8009F240u)
                    /* TODO Resolve unsupported callback target or adapter */
        abort();
                sub_8009F240(previous_controller);
            }
            count = r_u32(counter_address);
            if (count == 3u)
            {
                target = r_u32(0x800FEE14u);
                if (target != 0x8009F240u || previous_controller_valid == 0u)
                    /* TODO Resolve unsupported callback target or adapter */
        abort();
                sub_8009F240(previous_controller - 240u);
                w_u32(counter_address, 1u);
            }
            else if (count == 4u)
                w_u32(counter_address, 3u);
            else if ((sint32)count >= 0 && (sint32)count < 2)
            {
                pointer = r_u32(0x800FEE28u);
                previous_controller = pointer + stride;
                previous_controller_valid = 1u;
                target = r_u32(0x800FEE14u);
                if (target != 0x8009F240u)
                    /* TODO Resolve unsupported callback target or adapter */
        abort();
                sub_8009F240(previous_controller);
                target = r_u32(0x800FEE18u);
                if (target != 0x8009EB90u)
                    /* TODO Resolve unsupported callback target or adapter */
        abort();
                sub_8009EB90(previous_controller);
                w_u32(counter_address, 0xFFFFFFFFu);
            }
            target = r_u32(0x800FEDFCu);
            if (target != 0x8009EBD4u)
                /* TODO Resolve unsupported callback target or adapter */
        abort();
            result = sub_8009EBD4(controller, mode);
            result = sub_8009D374(controller, result & 0xFFu);
            if ((sint32)result < 0)
                return result;
            sub_8009F92C(60u);
            if (sub_8009D780() == 0u)
                return 0xFFFFFFFDu;
            count = r_u32(0x800FEE6Cu) - 1u;
            w_u32(0x800FEE6Cu, count);
        } while ((sint32)count >= 2);
    }
    for (;;)
    {
        count = r_u32(0x800FEE6Cu) - 1u;
        w_u32(0x800FEE6Cu, count);
        if ((sint32)count <= 0)
            break;
        target = r_u32(0x800FEDFCu);
        if (target != 0x8009EBD4u)
            /* TODO Resolve unsupported callback target or adapter */
        abort();
        result = sub_8009EBD4(controller, mode);
        result = sub_8009D374(controller, result & 0xFFu);
        if ((sint32)result < 0)
            return result;
        sub_8009F92C(60u);
        if (sub_8009D780() == 0u)
            return 0xFFFFFFFDu;
    }
    sub_8009D810();
    offset = r_u8(controller + 68u);
    w_u8(controller + 68u, offset + 1u);
    source = r_u32(0x800FEE70u);
    buffer = r_u32(controller + 60u);
    result = r_u8(source);
    w_u8(buffer + offset, result);
    target = r_u32(0x800FEDF4u);
    if (target != 0x8009EA80u)
        /* TODO Resolve unsupported callback target or adapter */
        abort();
    sub_8009EA80(0u);
    return 0u;
}


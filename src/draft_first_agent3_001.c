#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>

static void controller_global_callback(uint32 target)
{
    /* TODO Supply the controller global callback adapter */
    fprintf(stderr, "Missing controller global callback target=%08X\n", target);
    abort();
}

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
                xport_draft_guest_call1(target, controller);
                break;
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
        controller_global_callback(target);
        controller_global_callback(r_u32(0x800FEE20u));
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
                xport_draft_guest_call1(target, argument);
                break;
        }
        argument = r_u32(controller + 12u);
        target = r_u32(0x800FEE0Cu);
        switch (target)
        {
            case 0x8009F090u:
                sub_8009F090(argument + 240u);
                break;
            default:
                xport_draft_guest_call1(target, argument + 240u);
                break;
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
            result = xport_draft_guest_call2(target, controller, mode);
            break;
    }
    result = sub_8009D54C(controller, result & 0xFFu);
    if (result == 0x5Au || result == 0u || (sint32)result < 0)
        return result;
    return 0xFFFFFFF7u;
}

static void controller_transfer_missing(uint32 slot, uint32 target, uint32 argument0, uint32 argument1)
{
    /* TODO Supply the exact controller callback target */
    fprintf(stderr, "Controller callback missing slot=%08X target=%08X a0=%08X a1=%08X\n", slot, target, argument0, argument1);
    abort();
}
static void controller_carried_missing(uint32 target, uint32 counter, uint32 index)
{
    /* TODO Resolve an asynchronous transition without a carried controller */
    fprintf(stderr, "Controller carry missing slot=800FEE14 target=%08X counter=%08X index=%08X\n", target, counter, index);
    abort();
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
    uint32 offset;
    uint32 source;
    uint32 buffer;
    FUNCTION_MARKER(0x8009E420u, "SLUS_003.73");
    target = r_u32(0x800FEE00u);
    if (target != 0x8009ECE4u) controller_transfer_missing(0x800FEE00u, target, controller, 0u);
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
                if (target != 0x8009ECE4u) controller_transfer_missing(0x800FEE00u, target, argument + stride, 0u);
                sub_8009ECE4(argument + stride);
            }
            target = r_u32(0x800FEDFCu);
            if (target != 0x8009EBD4u) controller_transfer_missing(0x800FEDFCu, target, controller, 1u);
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
                uint32 previous_controller;
                pointer = r_u32(0x800FEE28u);
                pointer = r_u32(pointer + stride + 12u);
                previous_controller = pointer + count * 240u - 240u;
                target = r_u32(0x800FEE14u);
                if (target != 0x8009F240u)
                    controller_transfer_missing(0x800FEE14u, target, previous_controller, 0u);
                sub_8009F240(previous_controller);
                count = r_u32(counter_address);
                if (count == 3u)
                {
                    target = r_u32(0x800FEE14u);
                    if (target != 0x8009F240u)
                        controller_transfer_missing(0x800FEE14u, target, previous_controller - 240u, 0u);
                    sub_8009F240(previous_controller - 240u);
                    w_u32(counter_address, 1u);
                    goto controller_receive;
                }
            }
            else
                count = r_u32(counter_address);
            if (count == 3u)
            {
                /* TODO Define asynchronous zero-to-three counter transition */
                controller_carried_missing(r_u32(0x800FEE14u), counter_address, count);
            }
            else if (count == 4u)
                w_u32(counter_address, 3u);
            else if ((sint32)count >= 0 && (sint32)count < 2)
            {
                pointer = r_u32(0x800FEE28u);
                pointer += stride;
                target = r_u32(0x800FEE14u);
                if (target != 0x8009F240u) controller_transfer_missing(0x800FEE14u, target, pointer, 0u);
                sub_8009F240(pointer);
                target = r_u32(0x800FEE18u);
                if (target != 0x8009EB90u) controller_transfer_missing(0x800FEE18u, target, pointer, 0u);
                sub_8009EB90(pointer);
                w_u32(counter_address, 0xFFFFFFFFu);
            }
controller_receive:
            target = r_u32(0x800FEDFCu);
            if (target != 0x8009EBD4u) controller_transfer_missing(0x800FEDFCu, target, controller, mode);
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
        if (target != 0x8009EBD4u) controller_transfer_missing(0x800FEDFCu, target, controller, mode);
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
    if (target != 0x8009EA80u) controller_transfer_missing(0x800FEDF4u, target, 0u, 0u);
    sub_8009EA80(0u);
    return 0u;
}


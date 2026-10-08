#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
/* Unverified drafts; TODO Resolve adapter and ABI details during integration */

uint32 sub_80096E14(void)
{
    return r_u32(0x800FD270u) != 1u;
}

uint32 sub_80096C4C(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 size = a2 > 0x7EFF0u ? 0x7EFF0u : a2;
    sub_80094F40(a1, size, a3);
    if (r_u32(0x800FD274u) == 0u)
        w_u32(0x800FD270u, 0u);
    return size;
}

void sub_8008E9FC(uint32 a1)
{
    uint32 source = 0xF2000002u;
    uint32 rate = 0x44E8u;
    uint32 setting, callback;
    w_u8(0x800FD06Eu, 6u);
    w_u8(0x800FD06Cu, 0u);
    w_u8(0x800FD06Du, 0u);
    w_u32(0x800FD068u, 0u);
    setting = r_u32(0x800FD05Cu);
    if (setting == 0u)
    {
        w_u8(0x800FD06Eu, 127u);
        return;
    }
    if (setting == 3u)
        rate = 0x89D0u;
    else if (setting == 5u)
    {
        w_u8(0x800FD06Eu, 0u);
        if (a1 != 0u)
        {
            source = 0xF2000003u;
            rate = 1u;
        }
        else
            w_u8(0x800FD06Cu, 1u);
    }
    else if (setting != 2u)
    {
        if (r_u32(0x800FD060u) != 0u)
            return;
        setting = r_u32(0x800FD05Cu);
        if (setting == 0u)
        {
            /* TODO BIOS BREAK 7 adapter */
            abort();
        }
        if ((sint32)setting < 70)
        {
            rate = (uint32)(0x204CC0 / (sint32)setting) & 0xFFFFu;
            w_u8(0x800FD06Du, r_u8(0x800FD06Du) + 1u);
        }
        else
            rate = (uint32)(0x409980 / (sint32)setting) & 0xFFFFu;
    }
    xport_bios_enter_critical();
    if (r_u8(0x800FD06Cu) != 0u)
        sub_800865EC(r_u32(0x800FD064u));
    else
    {
        sub_8008EE44(source);
        sub_8008ED0C(source, rate, 4096u);
        if (r_u8(0x800FD06Eu) == 0u)
        {
            w_u32(0x800FD068u, sub_8008658C(0u, 0u));
            callback = 0x8008EC6Cu;
        }
        else
            callback = r_u8(0x800FD06Du) != 0u ? 0x8008ECB8u : r_u32(0x800FD064u);
        sub_8008658C((uint32)(sint32)(sint8)r_u8(0x800FD06Eu), callback);
    }
    xport_bios_exit_critical();
}

uint32 sub_8006F7D8(void)
{
    for (;;)
    {
        if (r_u32(0x800FF700u) == 0u && r_u8(0x800EADC5u + 16u * r_u32(0x800FF79Cu)) == 0u)
            return 0u;
        sub_8006F500();
        sub_8006B2A8();
    }
}

uint32 sub_8006E360(uint32 a1)
{
    uint32 offset = r_u32(a1 + 4u);
    uint32 pointer;
    for (;;)
    {
        pointer = a1 + offset;
        if (r_u32(pointer) == 0xFFFFFFFFu)
            return pointer + 4u;
        offset = r_u32(pointer + 4u);
        a1 = pointer + 8u;
    }
}

uint32 sub_8006E178(uint32 a1)
{
    uint32 result = sub_8006B864(40u, 0, 1);
    uint32 bucket = 0x800EB8F8u + (a1 & 0x1FFu) * 4u;
    uint32 previous = r_u32(bucket);
    w_u32(result + 36u, 0u);
    w_u32(result + 32u, previous);
    w_u32(bucket, result);
    previous = r_u32(result + 32u);
    if (previous != 0u)
        w_u32(previous + 36u, result);
    w_u32(result + 20u, a1);
    w_u8(result + 18u, 0u);
    w_u16(result + 16u, 0u);
    return result;
}

uint32 sub_8006E2DC(void)
{
    uint32 result, index;
    if ((sint32)r_u32(0x800FFC60u) >= 512)
        return 0u;
    result = r_u32(0x800FFC64u);
    while (result == 0u)
    {
        index = r_u32(0x800FFC60u) + 1u;
        w_u32(0x800FFC60u, index);
        result = r_u32(0x800EB8F8u + index * 4u);
        w_u32(0x800FFC64u, result);
        if ((sint32)r_u32(0x800FFC60u) >= 512)
            return 0u;
    }
    w_u32(0x800FFC64u, r_u32(result + 32u));
    return result;
}

uint32 sub_800695B4(uint32 a1)
{
    uint32 item = r_u32(0x800FF680u);
    while (item != 0u && r_u32(item + 8u) != a1)
        item = r_u32(item + 16u);
    return item;
}

uint32 sub_80069758(uint32 a1, uint32 a2, uint32 a3)
{
    sint16 rectangle[4];
    uint32 result = sub_800694EC(a1);
    uint32 size = (a3 & 1u) != 0u ? 16u : 256u;
    uint32 found = 0u, index, pointer, value, x, y;
    w_u8(result + 3u, a3);
    rectangle[3] = 1;
    for (index = 0u; index < size; ++index)
    {
        pointer = a2 + index * 2u;
        value = r_u16(pointer);
        if ((value & 0x7FFFu) == 31775u)
        {
            found = 1u;
            w_u16(pointer, 0u);
        }
        else if (found != 0u || r_u16(pointer) == 0u)
            w_u16(pointer, value | 0x8000u);
    }
    if (found == 0u)
        for (index = 0u; index < size; ++index)
        {
            pointer = a2 + index * 2u;
            w_u16(pointer, r_u16(pointer) | 0x8000u);
        }
    if (size == 16u)
    {
        w_u8(result + 2u, sub_800696A8());
        x = r_u32(0x800FF68Cu);
        y = r_u16(0x800FF690u) + r_u8(result + 2u);
    }
    else
    {
        w_u8(result + 2u, sub_80069700());
        x = r_u32(0x800FF694u);
        y = r_u32(0x800FF698u) + r_u8(result + 2u);
    }
    rectangle[0] = (sint16)x;
    rectangle[1] = (sint16)y;
    rectangle[2] = (sint16)size;
    w_u16(result, (y << 6) | ((x >> 4) & 0x3Fu));
    xport_draft_host_sub_800885A4_p1(rectangle, a2);
    w_u8(result + 6u, 1u);
    return result;
}

uint32 sub_800694EC(uint32 a1)
{
    uint32 result = sub_8006B864(24u, 0, 1);
    uint32 previous = r_u32(0x800FF680u);
    w_u32(result + 20u, 0u);
    w_u32(0x800FF680u, result);
    w_u32(result + 16u, previous);
    if (previous != 0u)
        w_u32(previous + 20u, result);
    w_u32(result + 8u, a1);
    w_u8(result + 6u, 0u);
    w_u16(result + 4u, 0u);
    return result;
}

uint32 sub_8006F164(uint32 a1)
{
    uint32 node = r_u32(0x800FF7A4u);
    uint32 entry, index, count, length, left, right;
    while (node != 0u)
    {
        entry = r_u32(node) + 4u;
        count = r_u32(r_u32(node));
        for (index = 0u; index < count; ++index)
        {
            length = 0u;
            for (;;)
            {
                left = r_u8(a1 + length) & 0xDFu;
                right = r_u8(entry + length) & 0xDFu;
                if (left != right || left == 0u || length >= 8u)
                    break;
                ++length;
            }
            if ((left == 0u && right == 0u) || length == 8u)
                return entry + 12u;
            entry += 8u * r_u32(entry + 8u) + 12u;
        }
        node = r_u32(node + 4u);
    }
    return 0u;
}

uint32 sub_8001A760(void)
{
    uint32 result = sub_8006E278(951706923u);
    w_u32(0x800FF1D0u, result);
    return result;
}

uint32 sub_80066570(uint32 a1)
{
    uint32 state = r_u32(0x800FFC14u);
    uint32 multiplier = r_u32(0x800FFC18u);
    uint32 increment = r_u32(0x800FFC1Cu);
    uint32 next = state * multiplier + increment;
    uint32 product = (next & 0xFFFFu) * a1;
    multiplier = (multiplier ^ next) + (uint32)((sint32)next >> 4);
    increment += 0xEFEFEFF0u + (uint32)((sint32)next >> 3);
    w_u32(0x800FFC14u, next);
    w_u32(0x800FFC18u, multiplier);
    w_u32(0x800FFC1Cu, increment);
    return (uint32)((sint32)product >> 16);
}

uint32 sub_8006A3A4(void)
{
    return sub_8006A334(0u);
}

uint32 sub_8006F004(void)
{
    uint32 result;
    sub_8006EE94(r_u32(0x800FF778u), 0u);
    sub_8006EE94(r_u32(0x800FF77Cu), 0u);
    w_u32(0x800FF778u, 0xFFFFFFFFu);
    w_u32(0x800FF77Cu, 0xFFFFFFFFu);
    result = sub_8006E414();
    w_u32(0x800FF798u, 0u);
    return result;
}

uint32 sub_8007DCDC(void)
{
    sub_8007DC5C(0u);
    return sub_8007DC5C(1u);
}

void sub_8001BE54(void)
{
    w_u32(0x800FF1E4u, 0u);
    w_u32(0x800FF1ECu, 0u);
    w_u32(0x800FF1E8u, 0u);
    w_u8(0x800FF1DCu, 0u);
}

uint32 sub_8006BF04(uint32 a1)
{
    sint32 input[3];
    uint32 output[3];
    input[0] = (sint32)r_u32(a1) >> 12;
    input[1] = (sint32)r_u32(a1 + 4u) >> 12;
    input[2] = (sint32)r_u32(a1 + 8u) >> 12;
    /* TODO GTE adapter must preserve IR and MAC state */
    xport_draft_gte_square_vector(input, output);
    return sub_80085B54(output[0] + output[1] + output[2]);
}

uint32 sub_80085B54(uint32 A0)
{
    uint32 shift = xport_draft_gte_leading_sign_count(A0) & 0xFEu;
    uint32 index;
    if ((sint32)A0 < 0)
        return r_u16(0x800F5130u + 2u * (A0 >> 20));
    index = (A0 << (shift & 31u)) >> 20;
    return r_u16(0x800F5130u + 2u * index) >> ((shift >> 1) & 31u);
}

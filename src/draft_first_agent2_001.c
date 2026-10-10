#include "draft_first_signatures.h"
#include <stdlib.h>
#include <stdio.h>

/* Unverified draft C; integration remains TODO */
static uint32 apocalypse_agent2_controller_callback(uint32 target, uint32 argument, uint32 argument_count)
{
    /* TODO Controller callback needs its actual native contract */
    fprintf(stderr, "Missing controller callback target=%08X argument=%08X argc=%u\n", target, argument, (unsigned)argument_count);
    abort();
}

uint32 sub_8009CDA0(void)
{
    uint32 port = r_u32(0x800FEE54u), target;
    if (!(r_u32(port + 4u) & 1u)) return 0u;
    if (!(r_u32(port) & 1u)) return 0u;
    target = r_u32(0x800FEE1Cu);
    if (target)
        (void)apocalypse_agent2_controller_callback(target, 0u, 0u);
    return 1u;
}

uint32 sub_8009E1BC(uint32 controller)
{
    uint32 target;
    uint32 result;
    target = r_u32(0x800FEE0Cu);
    switch (target)
    {
        case 0x8009F090u:
            result = sub_8009F090(controller);
            break;
        default:
            result = apocalypse_agent2_controller_callback(target, controller, 1u);
    }
    w_u32(0x800FEE74u, result);
    return sub_8009D374(controller, 0xFFFFFFFEu);
}

uint32 sub_8009D374(uint32 controller, uint32 command)
{
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
        port = r_u32(0x800FEE58u);
        buffer = r_u32(controller + 64u);
        received = r_u8(port);
        w_u8(controller + 68u, 0xFFu);
        w_u8(controller + 69u, 1u);
        w_u8(buffer, ~command);
        port = r_u32(0x800FEE58u);
        while ((r_u16(port + 4u) & 1u) == 0u)
        {
        }
        while (sub_8009F94C() == 0u)
        {
        }
        port = r_u32(0x800FEE58u);
        w_u8(port, ~command);
        return received;
    }
    buffer = r_u32(controller + 60u);
    mode = 0x88u;
    if ((r_u8(buffer) >> 4) == 8u && r_u8(controller + 68u) >= 9u)
        mode = 0x22u;
    port = r_u32(0x800FEE58u);
    timer = r_u16(0x1F801120u);
    status = r_u16(port + 4u);
    w_u32(0x801086ECu, 0x1AEu);
    w_u32(0x801086E8u, timer);
    if ((status & 2u) == 0u)
    {
        while ((r_u16(port + 4u) & 2u) == 0u)
        {
        }
    }
    port = r_u32(0x800FEE58u);
    mask_port = r_u32(0x800FEE54u);
    received = r_u8(port);
    w_u16(port + 14u, mode);
    status = r_u32(mask_port);
    if ((status & 0x80u) == 0u)
    {
        do
        {
            if (sub_8009F94C() != 0u)
                return 0xFFFFFFECu;
            mask_port = r_u32(0x800FEE54u);
            status = r_u32(mask_port);
        } while ((status & 0x80u) == 0u);
    }
    port = r_u32(0x800FEE58u);
    w_u8(port, command);
    counter = r_u8(controller + 69u);
    offset = r_u8(controller + 68u);
    buffer = r_u32(controller + 60u);
    w_u8(controller + 69u, counter + 1u);
    w_u8(buffer + offset, received);
    counter = r_u8(controller + 68u);
    w_u8(controller + 68u, counter + 1u);
    return received;
}

uint32 sub_8009E2D4(uint32 controller)
{
    uint32 argument;
    uint32 target;
    uint32 mode = 0u;
    uint32 result;
    uint32 length;
    if (r_u32(0x800FEE74u) != 0u)
    {
        argument = r_u32(controller + 12u);
        target = r_u32(0x800FEE0Cu);
        switch (target)
        {
            case 0x8009F090u:
                sub_8009F090(argument + 0x1E0u);
                break;
            default:
                (void)apocalypse_agent2_controller_callback(target, argument + 0x1E0u, 1u);
        }
        argument = r_u32(controller + 12u);
        target = r_u32(0x800FEE0Cu);
        switch (target)
        {
            case 0x8009F090u:
                sub_8009F090(argument + 0x2D0u);
                break;
            default:
                (void)apocalypse_agent2_controller_callback(target, argument + 0x2D0u, 1u);
        }
    }
    if (r_u8(controller + 54u) == 0u)
        mode = r_u32(0x800FEE40u);
    result = sub_8009D54C(controller, mode);
    if ((sint32)result < 0)
        return result;
    if ((result & 0xF0u) == 0u)
        return 0xFFFFFFF7u;
    length = (result & 0xFu) << 1;
    w_u32(0x800FEE6Cu, length);
    if (length == 0u)
        w_u32(0x800FEE6Cu, 0x20u);
    return 0u;
}

uint32 sub_8009D810(void)
{
    uint32 result;
    do
    {
        result = r_u16(r_u32(0x800FEE58u) + 4u) & 2u;
    } while (!result);
    return result;
}

uint32 sub_8009835C(uint32 a1)
{
    uint32 x = r_u8(a1), y = r_u8(a1 + 1u), z = r_u8(a1 + 2u);
    return 75u * (60u * (10u * (x >> 4) + (x & 15u)) + 10u * (y >> 4) + (y & 15u)) + 10u * (z >> 4) + (z & 15u) - 150u;
}

uint32 sub_8002FED8(uint32 a1)
{
    uint32 result = sub_8006B864(a1, 0u, 1u);
    uint32 i, count = (a1 + 3u) >> 2;
    for (i = 0; i < count; ++i)
        w_u32(result + 4u * i, 0u);
    return result;
}

uint32 sub_800153D8(uint32 a1)
{
    return sub_80015374(a1, 0u);
}

uint32 sub_80015474(uint32 a1, uint32 a2)
{
    uint32 result;
    w_u32(a1 + 28u * r_u8(a1 + 10u) + 24u, a2);
    w_u8(a1 + 28u * r_u8(a1 + 10u) + 37u, 1u);
    w_u8(a1 + 28u * r_u8(a1 + 10u) + 36u, 0u);
    result = r_u8(a1 + 10u) + 1u;
    w_u8(a1 + 10u, result);
    return result;
}

uint32 sub_80068404(void)
{
    w_u32(0x800FF670u, 0u);
    w_u32(0x800FF67Cu, 0u);
    w_u32(0x800FF678u, 0u);
    sub_800682CC(512u, 256u, 512u, 256u);
    return sub_800682CC(512u, 0u, 512u, 256u);
}

uint32 sub_80068240(void)
{
    uint32 result = sub_8006B864(20u, 0u, 1u);
    w_u32(0x800FF670u, r_u32(0x800FF670u) + 1u);
    return result;
}

uint32 sub_8006A868(void)
{
    w_u32(0x800FF6F0u, 0x800E5C08u);
    w_u32(0x800E5C08u, 1u);
    w_u32(0x800FF6F4u, 0x800E6A08u);
    return 0x800E6A08u;
}

void sub_8007D8C0(uint32 a1, uint32 a2, uint32 a3)
{
    w_u32(0x800FFADCu, a1);
    w_u32(0x800FFAE0u, a2);
    w_u32(0x800FFAE4u, a3);
    w_u32(0x800FFAFCu, 0u);
}

void sub_8006AED8(uint32 a1)
{
    w_u32(0x800FF70Cu, a1);
}

uint32 sub_8007113C(void)
{
    return sub_80071054();
}

uint32 sub_8006E278(uint32 a1)
{
    uint32 i;
    for (i = r_u32(0x800EB8F8u + 4u * (a1 & 511u)); i; i = r_u32(i + 32u))
        if (r_u32(i + 20u) == a1)
            break;
    return i;
}

uint32 sub_8006654C(uint32 a1)
{
    w_u32(0x800FFC14u, a1);
    w_u32(0x800FFC18u, 314159265u);
    w_u32(0x800FFC1Cu, 178453311u);
    return 178453311u;
}

uint32 sub_80010138(uint32 name_address)
{
    uint32 size = sub_8006B04C(name_address);
    uint32 result = sub_8006B864(size, 1u, 1u);
    w_u32(0x800FEEC0u, result);
    if (result)
    {
        sub_8006B234(result);
        return sub_8006B44C();
    }
    return result;
}

uint32 sub_8002FF54(void)
{
    uint32 result = (r_u32(0x800FF660u) + 126832u) & 0x7FFFFFFFu;
    w_u32(0x800FF374u, result);
    return result;
}

void sub_8001A7B0(uint32 a1)
{
    w_u8(0x800FF1B0u, a1);
}

uint32 sub_8001A7D4(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    uint32 color = ((a3 & 255u) << 16) | ((a2 & 255u) << 8) | (a1 & 255u);
    uint32 result;
    w_u32(0x800FF1B4u, color);
    result = color | (a4 ? 771751936u : 738197504u);
    w_u32(0x800FF1B4u, result);
    return result;
}

uint32 sub_8001A784(uint32 a1)
{
    uint32 value = r_u8(0x800E6908u + (a1 & 255u));
    return value == 255u ? 0xFFFFFFFFu : value;
}

uint32 sub_800653B8(uint32 a1)
{
    uint32 base = sub_8006B864(a1, 1u, 1u);
    w_u32(0x800FF63Cu, base);
    sub_8006B70C(base, base + a1);
    return base;
}

void nullsub_16(void)
{
    /* Original empty routine */
}

uint32 sub_800878AC(uint32 a1, uint32 a2)
{
    uint32 y = r_u32(a2 + 4u), z = r_u32(a2 + 8u);
    w_u32(a1 + 20u, r_u32(a2));
    w_u32(a1 + 24u, y);
    w_u32(a1 + 28u, z);
    return a1;
}

uint32 sub_8007EBC4(void)
{
    uint32 result = r_u32(0x800FFAA8u) + 1u;
    w_u32(0x800FFAA8u, result);
    return result;
}

void sub_8001A7C8(uint32 a1)
{
    w_u16(0x800FF1BAu, a1);
}

uint32 sub_80015F48(uint32 a1)
{
    sub_8006A3F0();
    sub_80069A94();
    return sub_80069BC4(a1);
}

uint32 sub_800151D0(void)
{
    return sub_8001A7D4(255u, 0u, 0u, 0u);
}

uint32 sub_800117D8(uint32 a1)
{
    if (a1 == 2u)
        return r_u32(0x800A55C0u);
    if (a1 == 4u)
        return r_u32(0x800A55C4u);
    if (a1 == 8u)
        return r_u32(0x800A55C8u);
    if (a1 == 1u)
        return r_u32(0x800A55BCu);
    return 0x800FEEFCu;
}

uint32 sub_800152C4(uint32 a1, uint32 a2)
{
    w_u32(a1, 0x800A05D4u);
    if (a2 & 1u)
        return sub_8002FF34(a1);
    return 0x800A05D4u;
}

uint32 sub_8002FF34(uint32 a1)
{
    return sub_8006BC20(a1);
}

uint32 sub_800650B4(uint32 a1)
{
    if (r_u8(a1))
    {
        do
        {
            ++a1;
        } while (r_u8(a1));
    }
    return a1 + ((a1 + 1u) & 1u) + 1u;
}

uint32 sub_800626C8(uint32 a1, uint32 a2)
{
    uint32 result = sub_8006ED70(a2);
    w_u8(a1 + 27u, result);
    w_u16(a1 + 22u, 0u);
    return result;
}

uint32 sub_80022B8C(uint32 a1, uint32 a2)
{
    sub_80022A24(a1, a2, 0u);
    w_u32(a1, 0x800A1548u);
    w_u32(a1 + 4u, 8u);
    sub_80022C58(a1, 10u, 32u, 10u, 8000u, 1u, 1u, 11u, 256u);
    return a1;
}

uint32 sub_80062D70(uint32 a1)
{
    uint32 result = r_u16(a1 + 78u) | 8u;
    w_u16(a1 + 78u, result);
    return result;
}

void sub_8005171C(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6)
{
    uint32 base = a1 + 16u * a2;
    w_u32(base + 512u, a3);
    w_u32(base + 520u, a4);
    w_u32(base + 516u, a5);
    w_u32(base + 524u, a6);
}

uint32 sub_8006CDC4(void)
{
    w_u32(0x800FF758u, 150u);
    return 150u;
}

uint32 sub_80018B44(uint32 a1)
{
    uint32 i, value = 0u;
    for (i = 1u; i < 34u; ++i)
        value ^= r_u32(a1 + 4u * i);
    return value | 1u;
}

void sub_8006737C(void)
{
    w_u32(0x800FF648u, 0u);
}

uint32 sub_8006C730(uint32 a1, uint32 a2)
{
    w_u16(a1, r_u16(a1) + r_u16(a2));
    w_u16(a1 + 2u, r_u16(a1 + 2u) + r_u16(a2 + 2u));
    w_u16(a1 + 4u, r_u16(a1 + 4u) + r_u16(a2 + 4u));
    return a1;
}

uint32 sub_8006C5C4(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 y = r_u32(a2 + 4u) << (r_u32(a3) & 31u), z = r_u32(a2 + 8u) << (r_u32(a3) & 31u);
    w_u32(a1, r_u32(a2) << (r_u32(a3) & 31u));
    w_u32(a1 + 4u, y);
    w_u32(a1 + 8u, z);
    return a1;
}

uint32 sub_8006CB28(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 x = (r_u16(a2) - r_u16(a3)) & 65535u, y = (r_u16(a2 + 2u) - r_u16(a3 + 2u)) & 65535u, z = r_u16(a2 + 4u) - r_u16(a3 + 4u);
    w_u32(a1, x | (y << 16));
    w_u16(a1 + 4u, z);
    return a1;
}

void nullsub_34(void)
{
    /* Original empty routine */
}

uint32 sub_80032F7C(uint32 a1)
{
    w_u32(a1 + 68u, 0x800A1DB8u);
    w_u8(a1 + 60u, 1u);
    w_u8(a1 + 61u, 1u);
    w_u8(a1 + 62u, 1u);
    w_u32(0x800FF3A8u, r_u32(0x800FF3A8u) + 1u);
    return a1;
}

uint32 sub_800338A0(uint32 a1, uint32 a2)
{
    uint32 color = a2 & 255u;
    uint32 result = (r_u32(a1 + 144u) & 0xFF000000u) | (color << 16) | (color << 8) | color;
    w_u32(a1 + 144u, result);
    return result;
}

uint32 sub_80022860(void)
{
    uint32 result = r_u32(0x800FF214u);
    if (result)
    {
        result = sub_80069EF4(3u, 0x800FFD48u, 0u);
    }
    w_u32(0x800FF214u, 0u);
    return result;
}

uint32 sub_80062F48(uint32 a1)
{
    return (r_u16(a1 + 78u) & 64u) != 0u;
}

void sub_80085C04(uint32 a1, uint32 a2)
{
    uint32 b = r_u32(a1 + 4u), c = r_u32(a1 + 8u), d = r_u32(a1 + 12u), e = r_u16(a1 + 16u);
    w_u32(a2, r_u32(a1));
    w_u32(a2 + 4u, b);
    w_u32(a2 + 8u, c);
    w_u32(a2 + 12u, d);
    w_u16(a2 + 16u, e);
}

void sub_80085BE8(uint32 a1)
{
    w_u32(a1, 0u);
    w_u32(a1 + 4u, 0u);
    w_u32(a1 + 8u, 0u);
    w_u32(a1 + 12u, 0u);
    w_u16(a1 + 16u, 0u);
}

uint32 sub_80034F9C(uint32 a1)
{
    uint32 result = r_u32(a1), y = r_u16(a1 + 4u);
    w_u32(0x800FF414u, r_u32(a1));
    w_u16(0x800FF418u, y);
    return result;
}

void sub_800350FC(uint32 a1, uint32 a2, uint32 a3)
{
    w_u8(0x800FF42Cu, a1);
    w_u8(0x800FF42Du, a2);
    w_u8(0x800FF42Eu, a3);
}

void sub_800774A4(uint32 a1, uint32 a2)
{
    w_u32(a1 + 540u, a2);
}

uint32 sub_80062608(uint32 a1)
{
    return sub_8006BC20(a1);
}

uint32 sub_800333F0(uint32 a1)
{
    uint32 high = (uint32)(sint32)(signed char)r_u8(a1 + 90u);
    uint32 sum = (high << 8) | r_u8(a1 + 91u);
    uint32 result;
    sum += r_u16(a1 + 92u);
    w_u8(a1 + 90u, sum >> 8);
    result = (uint32)(sint32)(signed char)r_u8(a1 + 90u) * 8u;
    w_u8(a1 + 91u, sum);
    w_u32(a1 + 84u, r_u32(a1 + 80u) + result);
    return result;
}

void sub_80033288(uint32 a1, uint32 a2)
{
    w_u16(a1 + 92u, a2);
}

uint32 sub_80033240(uint32 a1, uint32 a2)
{
    uint32 result = r_u8(a1 + 88u) | a2;
    w_u8(a1 + 88u, result);
    return result;
}

uint32 sub_80036614(uint32 a1)
{
    return sub_80033430(a1);
}

uint32 sub_80035758(uint32 a1)
{
    uint32 result;
    sub_8006C0B8(a1 + 24u, a1 + 36u);
    result = sub_80033530(a1, 1u);
    if (!result)
    {
        result = sub_80033590(a1, 1u);
        if (!result)
            return sub_80033430(a1);
    }
    return result;
}

uint32 sub_8006C1E8(uint32 a1, uint32 a2)
{
    uint32 shift = r_u32(a2) & 31u;
    w_u32(a1, (uint32)((sint32)r_u32(a1) >> shift));
    shift = r_u32(a2) & 31u;
    w_u32(a1 + 4u, (uint32)((sint32)r_u32(a1 + 4u) >> shift));
    shift = r_u32(a2) & 31u;
    w_u32(a1 + 8u, (uint32)((sint32)r_u32(a1 + 8u) >> shift));
    return a1;
}

uint32 sub_8005CEE0(uint32 a1, uint32 a2, uint32 a3)
{
    return sub_8007CC10(a2, a1, a3 & 255u);
}

uint32 sub_8006A4A0(uint32 a1, uint32 a2)
{
    return sub_800900AC(a1 & 255u, a2 & 255u);
}

uint32 sub_80062F0C(uint32 a1)
{
    uint32 result = sub_80062F48(a1);
    if (!result)
    {
        result = r_u16(a1 + 78u) | 64u;
        w_u16(a1 + 78u, result);
    }
    return result;
}

void nullsub_24(void)
{
    /* Original empty routine */
}

uint32 sub_8003188C(uint32 a1)
{
    sub_800330F4(a1);
    w_u32(a1 + 68u, 0x800A1A68u);
    return a1;
}

uint32 sub_8001B788(uint32 a1, uint32 a2)
{
    uint32 result = sub_8001B654();
    w_u32(result, a1);
    w_u32(result + 8u, a2);
    w_u16(result + 4u, 0u);
    return result;
}

uint32 sub_800695EC(void)
{
    uint32 node = r_u32(0x800FF680u), result = 0x80100000u, next;
    while (node)
    {
        result = r_u16(node + 4u);
        next = r_u32(node + 16u);
        if (!result)
        {
            if (r_u8(node + 3u) & 1u)
                w_u8(0x80100590u + r_u8(node + 2u), 1u);
            if (r_u8(node + 3u) & 2u)
                w_u8(0x80100630u + r_u8(node + 2u), 1u);
            /* The downstream allocator overwrites the unused A1 carrier */
            result = sub_80069540(node, 0u);
        }
        node = next;
    }
    return result;
}

uint32 sub_80069700(void)
{
    uint32 i = 0u;
    if (!r_u8(0x80100630u))
    {
        i = 1u;
        while (i < 82u)
        {
            if (r_u8(0x80100630u + i++))
            {
                --i;
                break;
            }
        }
    }
    w_u8(0x80100630u + i, 0u);
    return i;
}

uint32 sub_80063BF0(void)
{
    uint32 i, result = 0x800FFF8Cu, next;
    for (i = 0u; i < 256u; ++i)
        w_u32(0x8010038Cu - 4u * i, 0u);
    for (i = r_u32(0x800FF630u); i; i = next)
    {
        next = r_u32(i + 20u);
        result = sub_8006BC20(i);
    }
    w_u32(0x800FF630u, 0u);
    return result;
}

uint32 sub_8006EE2C(void)
{
    uint32 i;
    sub_8006F7D8();
    sub_8006F004();
    for (i = 0u; i < 40u; ++i)
        if (!r_u8(0x800EAEF8u + 64u * i + 11u))
            sub_8006EE94(i, 1u);
    return sub_8006E414();
}

/* Cleanup has no native result contract */
void sub_8006EE94(uint32 a1, uint32 a2)
{
    uint32 object, index;
    if (a1 == 0xFFFFFFFFu)
        return;
    object = 0x800EAEF8u + 64u * a1;
    if (!r_u8(object))
        return;
    sub_8006E394(a1);
    w_u32(object + 24u, 0u);
    w_u32(object + 40u, 0u);
    w_u32(object + 36u, 0u);
    w_u8(object + 11u, 0u);
    w_u8(object + 10u, 0u);
    w_u32(object + 28u, 0u);
    w_u32(object + 16u, 0u);
    w_u8(object, 0u);
    index = sub_8006F4D0(a1);
    if (index != 0xFFFFFFFFu)
    {
        sub_8007DC5C(index);
        sub_8006ECAC(index & 0xFFu);
        w_u32(0x800FF778u + 4u * index, 0xFFFFFFFFu);
    }
    sub_8006BC20(r_u32(object + 20u));
    w_u32(object + 20u, 0u);
    if (a2)
        sub_8006E414();
}

uint32 sub_8007DC5C(uint32 a1)
{
    uint32 i, j, base;
    if ((sint32)a1 >= 2)
        return 0u;
    base = 0x800EDA30u + 1632u * a1;
    w_u32(base, 0u);
    for (i = 0u; i < 20u; ++i)
        for (j = 0u; j < 20u; ++j)
            w_u32(base + 32u + 80u * i + 4u * j, 0u);
    return 0u;
}

uint32 sub_8006EBF4(uint32 a1)
{
    uint32 object = 0x800EAEF8u + 64u * a1, node, old, result;
    node = r_u32(object + 20u) + 12u;
    if (r_u32(0x800FF794u))
    {
        if (r_u32(r_u32(object + 20u) + 40u))
        {
            do
            {
                node = r_u32(node + 28u);
            } while (r_u32(node + 28u));
        }
        old = r_u32(0x800FF794u);
        w_u32(0x800FF794u, r_u32(object + 20u) + 12u);
        w_u32(node + 28u, old);
    }
    else
        w_u32(0x800FF794u, r_u32(object + 20u) + 12u);
    result = 0x800FF78Cu + 4u * sub_8006F4D0(a1);
    w_u32(result, r_u32(0x800FF788u));
    return result;
}

uint32 sub_80011904(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    uint32 z;
    w_u32(a1, 0x800A0380u);
    w_u32(a1 + 4u, r_u32(a2) << 12);
    w_u32(a1 + 8u, r_u32(a2 + 4u) << 12);
    z = r_u32(a2 + 8u);
    w_u32(a1 + 16u, a3);
    w_u32(a1 + 20u, a4);
    w_u32(a1 + 12u, z << 12);
    sub_800119F0(a1);
    w_u32(0x800ED51Cu, 0u);
    w_u16(0x800FFA98u, 128u);
    w_u16(0x800FFA96u, 128u);
    w_u16(0x800FFA94u, 128u);
    return a1;
}

uint32 sub_800152F8(uint32 a1)
{
    uint32 count = 0u, sum = 0u, i, result;
    for (i = 0u; i < r_u8(a1 + 10u); ++i)
        if (r_u8(a1 + 28u * i + 37u))
        {
            ++count;
            sum += r_u8(a1 + 28u * i + 36u);
        }
    result = (uint32)((sint32)(240u - ((count - 1u) * r_u32(a1 + 20u) + sum)) / 2);
    w_u32(a1 + 16u, result);
    return result;
}

uint32 sub_8006D028(uint32 a1)
{
    uint32 packet = r_u32(0x800FF668u), data, x, y, z, tail, buffer;
    if (r_u32(0x800FF374u) < packet + 40u)
        return 0u;
    w_u32(0x800FF668u, packet + 40u);
    w_u32(packet, 150994944u);
    w_u32(packet + 4u, 746619008u);
    data = r_u32(a1 + 4u);
    x = r_u32(data);
    y = r_u32(data + 4u);
    z = r_u32(data + 8u);
    tail = r_u16(data + 10u);
    w_u32(packet + 20u, y);
    w_u32(packet + 12u, x);
    w_u32(packet + 28u, z);
    w_u16(packet + 36u, tail);
    buffer = r_u32(0x800FF660u);
    w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(buffer + 112u) & 0xFFFFFFu));
    w_u32(buffer + 112u, (r_u32(buffer + 112u) & 0xFF000000u) | (packet & 0xFFFFFFu));
    return packet;
}

uint32 sub_8001173C(void)
{
    uint32 direction = 0u, counter;
    if (r_u8(0x800EC178u) || r_u8(0x800EC188u))
    {
        counter = r_u32(0x800FF018u);
        if (!counter || ((sint32)counter >= 11 && !(counter & 1u)))
        {
            if (r_u8(0x800EC188u))
                direction = 1u;
            if (r_u8(0x800EC178u))
                direction = 0xFFFFFFFFu;
        }
        w_u32(0x800FF018u, r_u32(0x800FF018u) + 1u);
        return direction;
    }
    w_u32(0x800FF018u, 0u);
    return 0u;
}

uint32 sub_8006ED70(uint32 a1)
{
    uint32 i;
    for (i = 0u; i < 40u; ++i)
        if (sub_80067724(a1, 0x800EAEF8u + 64u * i))
            return i;
    return 0xFFFFFFFFu;
}

uint32 sub_80063B5C(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 result = sub_8006B864(24u, 0u, 1u), slot = 0x800FFF90u + 4u * (a1 & 255u), old;
    w_u32(result + 20u, r_u32(0x800FF630u));
    old = r_u32(slot);
    w_u32(0x800FF630u, result);
    w_u32(result + 16u, old);
    w_u32(slot, result);
    w_u8(result + 4u, 0u);
    w_u8(result + 5u, 0u);
    w_u16(result + 10u, a2);
    w_u32(result, a3);
    w_u32(result + 12u, a1);
    w_u8(result + 7u, 0u);
    w_u8(result + 6u, 0u);
    w_u16(result + 8u, 0u);
    return result;
}

uint32 sub_80022C58(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9)
{
    w_u32(a1 + 60u, a2);
    w_u32(a1 + 64u, a3);
    w_u8(a1 + 70u, a4);
    w_u32(a1 + 72u, a5);
    w_u8(a1 + 68u, a6);
    w_u8(a1 + 69u, a7);
    w_u32(a1 + 76u, a8);
    w_u32(a1 + 80u, a9);
    return a5;
}

uint32 sub_80018B70(uint32 a1)
{
    uint32 owner = r_u32(0x800FF5A0u), i, result;
    if ((sint32)r_u32(a1 + 104u) < 3)
        w_u32(a1 + 104u, 3u);
    w_u32(owner + 532u, r_u32(a1 + 104u));
    for (i = 1u; i < 8u; ++i)
        if ((r_u8(a1 + 66u) >> i) & 1u)
        {
            sub_8005E260(owner, i);
            w_u32(r_u32(owner + 612u + 4u * i) + 56u, r_u32(a1 + 68u + 4u * i));
        }
    result = r_u8(a1 + 67u);
    w_u32(owner + 644u, result);
    return result;
}

uint32 sub_80078618(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 value = (uint32)(sint32)(short)a2, count = a3 & 65535u, result;
    if (count)
    {
        result = (uint32)((sint32)(value - r_u32(0x800FF910u)) / (sint32)count);
        w_u32(0x800FF914u, count);
        w_u32(0x800FFD10u, result);
    }
    else
    {
        result = value;
        w_u32(0x800FF910u, value);
        w_u32(0x800FF914u, 0u);
    }
    return result;
}

uint32 sub_800786E4(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 value = (uint32)(sint32)(short)a2, count = a3 & 65535u, result;
    if (count)
    {
        result = (uint32)((sint32)(value - r_u32(0x800FF928u)) / (sint32)count);
        w_u32(0x800FF92Cu, count);
        w_u32(0x800FFD28u, result);
    }
    else
    {
        result = value;
        w_u32(0x800FF928u, value);
        w_u32(0x800FF92Cu, 0u);
    }
    return result;
}

void sub_80076BC4(uint32 a1)
{
    uint32 node = r_u32(0x800FF8A0u), flags;
    while (node)
    {
        if (r_u16(node + 296u) == (a1 & 65535u))
        {
            flags = r_u16(node) | 1u;
            w_u16(node, flags);
            return;
        }
        node = r_u32(node + 28u);
    }
}

uint32 sub_80033398(uint32 a1, uint32 a2)
{
    uint32 data = r_u32(0x800A6808u + 4u * a2), result;
    w_u32(a1 + 80u, data);
    result = r_u8(data - 4u);
    w_u8(a1 + 90u, 0u);
    w_u8(a1 + 91u, 0u);
    w_u8(a1 + 89u, result);
    w_u32(a1 + 84u, r_u32(a1 + 80u));
    return result;
}

uint32 sub_80032D3C(uint32 a1, uint32 a2)
{
    uint32 red = r_u8(a1), green = (r_u32(a1) >> 8) & 255u, blue = (r_u32(a1) >> 16) & 255u, result;
    result = (sint32)red >= (sint32)a2 ? (red - a2) & 255u : 0u;
    green = (sint32)green >= (sint32)a2 ? (green - a2) & 255u : 0u;
    blue = (sint32)blue >= (sint32)a2 ? (blue - a2) & 255u : 0u;
    w_u32(a1, (r_u32(a1) & 0xFF000000u) | (blue << 16) | (green << 8) | result);
    return result;
}

uint32 sub_8003C684(uint32 a1, uint32 a2)
{
    uint32 result = sub_8006C304(a1 + 484u, a2), y, z;
    if (result)
    {
        y = r_u32(a2 + 4u);
        z = r_u32(a2 + 8u);
        w_u32(a1 + 484u, r_u32(a2));
        w_u32(a1 + 488u, y);
        w_u32(a1 + 492u, z);
        result = r_u32(a1 + 396u) | 1u;
        w_u32(a1 + 396u, result);
    }
    return result;
}

uint32 sub_80036988(uint32 a1, uint32 a2)
{
    uint32 i = 0u, result = r_u32(a1 + 72u);
    if ((sint32)result > 0)
    {
        do
        {
            sub_80033290(r_u32(r_u32(a1 + 92u) + 4u * i++), (uint32)(sint32)(short)a2);
            result = (sint32)i < (sint32)r_u32(a1 + 72u);
        } while (result);
    }
    return result;
}

uint32 sub_80063D3C(uint32 a1)
{
    uint32 node = r_u32(0x800FFF90u + 4u * (a1 & 255u));
    while (node)
    {
        if (r_u32(node + 12u) == a1)
        {
            uint32 active = r_u8(node + 5u);
            w_u8(node + 4u, 1u);
            if (!active)
            {
                sub_8006541C(r_u32(node), r_u16(node + 10u), 0u);
                w_u8(node + 5u, 1u);
                return node;
            }
        }
        node = r_u32(node + 16u);
    }
    return 0u;
}

uint32 sub_8001E4C8(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
    uint32 result = r_u32(0x800FF738u), i = 0u, node;
    if (!result)
    {
        while ((sint32)i < (sint32)a2)
        {
            node = sub_80032DC0(112u);
            if (node)
                sub_800358B4(node, a1, 128u, 128u, 128u, a3, a4 & 65535u, 1u, a5, 0x3000u, 30u);
            ++i;
            result = (sint32)i < (sint32)a2;
        }
    }
    return result;
}

uint32 sub_80036CB0(uint32 a1)
{
    uint32 result = r_u32(a1 + 96u), all = 1u, i = 0u;
    if (result)
    {
        result = r_u32(a1 + 72u);
        if ((sint32)result > 0)
        {
            do
            {
                if (!sub_80033590(r_u32(r_u32(a1 + 92u) + 4u * i), 0u))
                    all = 0u;
                ++i;
                result = (sint32)i < (sint32)r_u32(a1 + 72u);
            } while (result);
        }
        if (all)
            return sub_80032ED8(a1);
    }
    return result;
}

uint32 sub_80033530(uint32 a1, uint32 a2)
{
    uint32 value = r_u16(a1 + 94u);
    if (value)
    {
        value = (value - r_u16(a1 + 74u)) & 65535u;
        w_u16(a1 + 94u, value);
        if (value & 32768u)
            w_u16(a1 + 94u, 0u);
        return 0u;
    }
    if (a2)
        sub_80032ED8(a1);
    return 1u;
}

uint32 sub_80062D24(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 flags = r_u8(a1 + 172u);
    if (a2 == 1u)
    {
        if (flags & a3)
            return 0u;
        w_u8(a1 + 172u, flags | a3);
        return 1u;
    }
    w_u8(a1 + 172u, flags & ~a3);
    return 0u;
}

uint32 sub_80064E50(uint32 a1)
{
    uint32 i, count = r_u16(a1), id, kind;
    for (i = 0u; i < count; ++i)
    {
        id = r_u16(a1 + 2u + 2u * i);
        kind = (uint32)(sint32)(short)r_u16(r_u32(r_u32(0x800FF624u) + 4u * id));
        if (kind == 1u || kind == 7u)
        {
            sub_80064E10(r_u32(0x800FF4E8u), r_u16(a1 + 2u + 2u * i));
            sub_80064E10(r_u32(0x800FF5DCu), id);
        }
    }
    return 0u;
}

uint32 sub_8002FC64(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    uint32 result = r_u32(0x800FF700u);
    if (!result)
    {
        result = r_u8(0x800EADC5u + 16u * r_u32(0x800FF79Cu));
        if (!result)
        {
            result = r_u32(0x800FF250u);
            if (!result)
            {
                result = r_u32(0x800FF2BCu);
                if (result)
                {
                    result = r_u32(0x800FF2B0u);
                    if (!result)
                    {
                        sub_8002FD2C(a1, a2);
                        sub_8002FB34(a3);
                        result = r_u32(0x800FF64Cu);
                        w_u32(0x800FF2C4u, a1);
                        w_u32(0x800FF2C8u, a2);
                        w_u32(0x800FF2CCu, r_u32(0x800FF64Cu));
                    }
                }
            }
        }
    }
    return result;
}

uint32 sub_80020264(uint32 a1)
{
    uint32 result = sub_80062F48(a1), node, flags;
    if (!result)
    {
        if (r_u8(a1 + 304u))
        {
            node = sub_80066088(r_u16(a1 + 310u));
            sub_80064A08(node);
        }
        result = r_u16(a1 + 78u) | 64u;
        flags = r_u16(a1) | 1u;
        w_u16(a1 + 78u, result);
        w_u16(a1, flags);
    }
    return result;
}

uint32 sub_80034B08(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
    uint32 index = 0u, offset = 8u * a2, color = ((a5 & 255u) << 16) | ((a4 & 255u) << 8) | 0x3A000000u | (a3 & 255u), result, ptr;
    w_u32(offset + r_u32(a1 + 72u) + 4u, color);
    result = r_u32(a1 + 76u);
    ptr = result + offset;
    while (index < r_u32(a1 + 84u))
    {
        ++index;
        w_u32(ptr + 4u, color);
        result = index < r_u32(a1 + 84u);
        ptr += 8u * r_u32(a1 + 80u);
    }
    return result;
}

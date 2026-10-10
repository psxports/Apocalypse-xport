#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>

/* Unverified draft C; integration remains TODO */
uint32 sub_8001610C(void)
{
    uint32 size, p, base, i, menu, result;
    size = sub_8006B04C(0x800A05B4u);
    base = sub_8006B864(size, 1, 1);
    w_u32(0x800FF004u, base);
    sub_8006B234(base);
    sub_8006B44C();
    p = r_u32(0x800FF004u);
    while (r_u8(p))
    {
        do
        {
            ++p;
        } while (r_u8(p));
        p = (p + 3u) & 0xFFFFFFFCu;
        while (r_u32(p) != 0xFFFFFFFFu)
            p += 4u;
        p += 4u;
    }
    sub_8006BD14(r_u32(0x800FF004u), (p - r_u32(0x800FF004u) + 4u) & 0xFFFFFFFCu);
    for (i = 0; i < 136u; i += 4u)
        w_u32(0x800A545Cu + i, r_u32(0x800A0514u + i));
    for (i = 0; i < 136u; i += 4u)
        w_u32(0x800A53D4u + i, r_u32(0x800A0514u + i));
    menu = sub_8002FED8(444u);
    if (menu)
        menu = sub_80015228(menu, 256, 0, 0, 320, 256, 26);
    w_u32(0x800FF024u, menu);
    sub_80015474(menu, r_u32(0x800A5598u));
    sub_80015474(r_u32(0x800FF024u), r_u32(0x800A5594u));
    w_u32(r_u32(0x800FF024u) + 20u, 30u);
    for (i = 0; i < 11u; ++i)
        w_u32(0x800A5254u + 24u * i, r_u32(0x800A54F0u + 4u * i));
    result = 0x800A5254u;
    return result;
}

uint32 sub_80062DD8(uint32 a1)
{
    uint32 object, result = 0, numerator;
    sint32 current, total, alpha;
    sint16 direction[3] = {0, -4096, 0};
    if (r_u16(a1 + 78u) & 8u)
    {
        object = r_u32(a1 + 196u);
        if (!object)
        {
            w_u32(0x800FF3ACu, 0);
            object = sub_80032DC0(156u);
            if (object)
                object = sub_80033688(object);
            w_u32(a1 + 196u, object);
            w_u32(0x800FF3ACu, 1);
            sub_80033764(object, 0x800FF5F0u, 0);
            sub_8003384C(r_u32(a1 + 196u), 4);
            sub_80033874(r_u32(a1 + 196u));
            w_u16(r_u32(a1 + 196u) + 64u, 32);
            w_u8(r_u32(a1 + 196u) + 66u, 1);
        }
        /* The seventh scalar aliases the first two direction halfwords */
        xport_draft_host_sub_80033900_p3(r_u32(a1 + 196u), a1 + 184u, direction, r_u16(a1 + 200u), r_u16(a1 + 200u), 0, 0xF0000000u);
        total = (sint16)r_u16(a1 + 204u);
        current = (sint16)r_u16(a1 + 202u);
        numerator = ((uint32)total - (uint32)current) << 7;
        alpha = total ? (sint32)numerator / total : ((sint32)numerator < 0 ? 1 : -1);
        if (alpha < 0)
            alpha = 0;
        result = sub_800338A0(r_u32(a1 + 196u), (uint32)alpha & 255u);
    }
    else
        result = sub_80062D84(a1);
    return result;
}

uint32 sub_80033900(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7)
{
    sint32 first[3], second[3], out[3], length1, length2;
    uint32 input1[3] = {a4, a5, a6}, input2[3] = {a5, a6, a7};
    sint32 buffer1[4], buffer2[4];
    uint32 result;
    sint32 x = (sint16)r_u16(a3), y = (sint16)r_u16(a3 + 2u), z = (sint16)r_u16(a3 + 4u);
    first[0] = 0;
    first[1] = (sint32)(0u - (uint32)z);
    first[2] = y;
    second[0] = (sint32)((uint32)y * (uint32)y + (uint32)z * (uint32)z);
    second[1] = (sint32)((0u - (uint32)x) * (uint32)y);
    second[2] = (sint32)((0u - (uint32)z) * (uint32)x);
    length1 = xport_draft_host_sub_8006BF04_p1(first);
    length2 = xport_draft_host_sub_8006BF04_p1(second);
    xport_draft_host_sub_8006C40C_p123(buffer1, first, input1);
    xport_draft_host_sub_8006C4EC_p123(out, buffer1, &length1);
    first[0] = out[0];
    first[1] = out[1];
    first[2] = out[2];
    xport_draft_host_sub_8006C40C_p123(buffer2, second, input2);
    xport_draft_host_sub_8006C4EC_p123(out, buffer2, &length2);
    second[0] = out[0];
    second[1] = out[1];
    second[2] = out[2];
    xport_draft_host_sub_8006C3AC_p13(buffer1, a2, first);
    xport_draft_host_sub_8006C3AC_p123(out, buffer1, second);
    w_u32(a1 + 24u, out[0]);
    w_u32(a1 + 28u, out[1]);
    w_u32(a1 + 32u, out[2]);
    xport_draft_host_sub_8006C34C_p13(buffer1, a2, first);
    xport_draft_host_sub_8006C3AC_p123(out, buffer1, second);
    w_u32(a1 + 72u, out[0]);
    w_u32(a1 + 76u, out[1]);
    w_u32(a1 + 80u, out[2]);
    xport_draft_host_sub_8006C3AC_p13(buffer1, a2, first);
    xport_draft_host_sub_8006C34C_p123(out, buffer1, second);
    w_u32(a1 + 84u, out[0]);
    w_u32(a1 + 88u, out[1]);
    w_u32(a1 + 92u, out[2]);
    xport_draft_host_sub_8006C34C_p13(buffer1, a2, first);
    result = xport_draft_host_sub_8006C34C_p123(out, buffer1, second);
    w_u32(a1 + 96u, out[0]);
    w_u32(a1 + 100u, out[1]);
    w_u32(a1 + 104u, out[2]);
    return result;
}

void sub_8006D704(uint32 a1)
{
    static const uint32 xs[8] = {0, 414, 424, 439, 449, 454, 454, 449};
    static const uint32 ys[8] = {0, 37, 32, 32, 37, 43, 49, 55};
    uint32 owner = r_u32(0x800FF5A0u), item, packet, bar, head, x, y, link;
    sint32 extent;
    long long product;
    if (!owner)
        return;
    item = r_u32(owner + a1 * 4u + 612u);
    if (!item || a1 >= 8u || !a1)
        return;
    x = xs[a1];
    y = ys[a1];
    packet = r_u32(0x800FF668u);
    bar = packet + 16u;
    if (r_u32(0x800FF374u) < packet + 32u)
        return;
    product = (long long)(sint32)r_u32(item + 56u) * (sint32)0x10624DD3u;
    extent = (sint32)((sint32)(product >> 32) >> 3) - ((sint32)r_u32(item + 56u) >> 31);
    w_u32(0x800FF668u, packet + 32u);
    if (extent)
    {
        w_u8(bar + 3u, 3);
        w_u8(bar + 7u, 64);
        w_u8(bar + 5u, 255);
        w_u8(bar + 4u, 0);
        w_u8(bar + 6u, 0);
        head = r_u32(0x800FF660u);
        w_u16(bar + 8u, x);
        w_u16(bar + 10u, y);
        w_u16(bar + 12u, x + (uint32)extent - 1u);
        w_u16(bar + 14u, y);
        w_u32(bar, (r_u32(bar) & 0xFF000000u) | (r_u32(head + 112u) & 0xFFFFFFu));
        w_u32(head + 112u, (r_u32(head + 112u) & 0xFF000000u) | (bar & 0xFFFFFFu));
    }
    w_u8(packet + 3u, 3);
    w_u8(packet + 7u, 64);
    w_u8(packet + 5u, 64);
    w_u8(packet + 4u, 0);
    w_u8(packet + 6u, 0);
    head = r_u32(0x800FF660u);
    w_u16(packet + 8u, x);
    w_u16(packet + 10u, y);
    w_u16(packet + 12u, x + 7u);
    w_u16(packet + 14u, y);
    w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(head + 112u) & 0xFFFFFFu));
    link = (r_u32(head + 112u) & 0xFF000000u) | (packet & 0xFFFFFFu);
    w_u32(head + 112u, link);
    if (r_u32(owner + 644u) == a1)
    {
        sub_8006D1C0(x - 3u, y - 1u, 13, 4, 128, 128, 128, 0);
    }
}

static sint32 draft_agent3_lerp(uint32 first, uint32 last, uint32 count, uint32 total)
{
    uint32 product = (last - first) * count;
    if (!total)
        return (sint32)product < 0 ? 1 : -1;
    if (product == 0x80000000u && total == 0xFFFFFFFFu)
        return (sint32)product;
    return (sint32)product / (sint32)total;
}

void sub_8001B8BC(void)
{
    uint32 node = r_u32(0x800FF1ACu), data, total, count, period, rgb, x, y, size;
    while (node)
    {
        data = r_u32(node + 8u);
        period = r_u8(data + 26u);
        count = r_u16(node + 4u);
        if (period && !((count / period) & 1u))
        {
            node = r_u32(node + 12u);
            continue;
        }
        total = r_u16(data);
        size = r_u16(data + 4u) + (uint32)draft_agent3_lerp(r_u16(data + 4u), r_u16(data + 6u), count, total);
        w_u8(0x800FF1B0u, r_u8(data + 3u));
        w_u16(0x800FF1B8u, size);
        rgb = ((r_u8(data + 8u) + (uint32)draft_agent3_lerp(r_u8(data + 8u), r_u8(data + 11u), count, total)) & 255u);
        rgb |= ((r_u8(data + 9u) + (uint32)draft_agent3_lerp(r_u8(data + 9u), r_u8(data + 12u), count, total)) & 255u) << 8;
        rgb |= ((r_u8(data + 10u) + (uint32)draft_agent3_lerp(r_u8(data + 10u), r_u8(data + 13u), count, total)) & 255u) << 16;
        w_u32(0x800FF1B4u, rgb);
        w_u32(0x800FF1B4u, rgb | (r_u8(data + 2u) ? 0x2E000000u : 0x2C000000u));
        x = (uint32)(sint32)(sint16)r_u16(data + 18u);
        x += (uint32)draft_agent3_lerp(x, (uint32)(sint32)(sint16)r_u16(data + 22u), count, total);
        y = (uint32)(sint32)(sint16)r_u16(data + 20u);
        y += (uint32)draft_agent3_lerp(y, (uint32)(sint32)(sint16)r_u16(data + 24u), count, total);
        size = r_u16(data + 14u) + (uint32)draft_agent3_lerp(r_u16(data + 14u), r_u16(data + 16u), count, total);
        sub_8001AA28(x, y, r_u32(node), size, 0, 256);
        node = r_u32(node + 12u);
    }
}

uint32 sub_80020518(uint32 a1)
{
    uint32 count = r_u16(a1 + 336u), value, ptr, target, result = 0;
    long long product;
    sint32 high;
    if (count == 65535u)
        return 65535u;
    if (count)
        w_u16(a1 + 336u, count - 1u);
    count = r_u16(a1 + 336u);
    if (count < 60u)
    {
        value = r_u16(a1 + 330u) * r_u16(a1 + 336u);
        product = (long long)(sint32)value * (sint32)0x88888889u;
        high = (sint32)(product >> 32);
        value = (uint32)(((sint32)((uint32)high + value) >> 5) - ((sint32)value >> 31));
        w_u16(a1 + 332u, value);
    }
    count = r_u16(a1 + 336u);
    if (count - 31u < 29u)
        w_u8(a1 + 306u, (r_u32(0x800FF2F0u) & 2u) != 0);
    if (r_u16(a1 + 336u) < 31u)
        w_u8(a1 + 306u, (r_u32(0x800FF2F0u) & 1u) != 0);
    if (r_u8(a1 + 305u))
        w_u16(a1, r_u8(a1 + 306u) ? r_u16(a1) & 65534u : r_u16(a1) | 1u);
    result = r_u16(a1 + 336u);
    if (!result)
    {
        if (r_u32(a1 + 220u) < 8001u)
        {
            sub_8001D320(a1 + 4u, 20, 255, 255, 255, 4, 1, 20);
        }
        ptr = r_u32(a1 + 68u);
        value = a1 + (uint32)(sint32)(sint16)r_u16(ptr + 16u);
        target = r_u32(ptr + 20u);
        result = xport_draft_guest_call1(target, value);
    }
    return result;
}

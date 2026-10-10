#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>

void apocalypse_rotation_matrix(uint32 input, void *output)
{
    MATRIX previous;
    uint32 angle_xy = r_u32(input);
    uint32 angle_z, x, y, z, x_cos, packed, ir_x, ir_z, yz, saved0, saved3;
    uint8 *bytes = (uint8 *)output;
    ReadRotMatrix(&previous);
    saved0 = xport_load_le32((const uint8 *)previous.m);
    saved3 = xport_load_le32((const uint8 *)previous.m + 12u);
    xport_gte_write_data(1u, 0u);
    angle_z = r_u32(input + 4u);
    x = r_u32(0x800F863Cu + 4u * (angle_xy & 0xFFFu));
    y = r_u32(0x800F863Cu + 4u * (angle_z & 0xFFFu));
    xport_gte_write_control(0u, x);
    x_cos = x >> 16;
    xport_gte_write_control(3u, x_cos);
    xport_gte_write_data(0u, y & 0xFFFFu);
    xport_gte_execute(0x486012u);
    z = r_u32(0x800F863Cu + ((angle_xy >> 14) & 0x3FFCu));
    xport_gte_write_data(0u, y >> 16);
    packed = (z >> 16) - (z << 16);
    ir_x = xport_gte_read_data(9u);
    yz = xport_gte_read_data(11u);
    xport_gte_execute(0x486012u);
    xport_gte_write_control(0u, z);
    xport_gte_write_control(3u, packed);
    ir_x = ((ir_x ^ y) & 0xFFFFu) ^ y;
    xport_gte_write_data(0u, ir_x);
    ir_z = xport_gte_read_data(11u);
    ir_x = xport_gte_read_data(9u);
    xport_gte_execute(0x486012u);
    ir_x = (ir_x & 0xFFFFu) - (y << 16);
    xport_gte_write_data(0u, ir_x);
    ir_x = xport_gte_read_data(9u);
    packed = xport_gte_read_data(11u);
    xport_gte_execute(0x486012u);
    xport_store_le16(bytes, (uint16)ir_x);
    xport_store_le16(bytes + 12u, (uint16)packed);
    xport_store_le16(bytes + 6u, (uint16)yz);
    xport_gte_write_data(0u, x_cos);
    ir_x = xport_gte_read_data(9u);
    packed = xport_gte_read_data(11u);
    xport_gte_execute(0x486012u);
    xport_store_le16(bytes + 2u, (uint16)ir_x);
    xport_store_le16(bytes + 14u, (uint16)packed);
    xport_store_le16(bytes + 10u, (uint16)(0u - x));
    xport_store_le16(bytes + 8u, (uint16)ir_z);
    xport_gte_write_control(0u, saved0);
    xport_gte_write_control(3u, saved3);
    ir_x = xport_gte_read_data(9u);
    packed = xport_gte_read_data(11u);
    xport_store_le16(bytes + 4u, (uint16)ir_x);
    xport_store_le16(bytes + 16u, (uint16)packed);
}

void sub_800858FC(uint32 input, uint32 output)
{
    uint8 matrix[18];
    static const uint8 offsets[9] = {0u, 12u, 6u, 2u, 14u, 10u, 8u, 4u, 16u};
    uint32 index;
    apocalypse_rotation_matrix(input, matrix);
    for (index = 0u; index < 9u; ++index)
        w_u16(output + offsets[index], xport_load_le16(matrix + offsets[index]));
}

static uint32 menu_signed_quotient(uint32 numerator, uint32 denominator)
{
    sint32 left = (sint32)numerator, right = (sint32)denominator;
    if (!right)
        return left < 0 ? 1u : 0xFFFFFFFFu;
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
        return numerator;
    return (uint32)(left / right);
}

static void menu_rotate_normal(uint32 matrix, uint32 input, uint32 output)
{
    uint32 words[5], axis, result[3];
    for (axis = 0u; axis < 5u; ++axis)
        words[axis] = r_u32(matrix + 4u * axis);
    for (axis = 0u; axis < 5u; ++axis)
        xport_gte_write_control(axis, words[axis]);
    xport_gte_write_data(0u, r_u32(input));
    xport_gte_write_data(1u, r_u32(input + 4u));
    xport_gte_execute(0x486012u);
    for (axis = 0u; axis < 3u; ++axis)
        result[axis] = xport_gte_read_data(9u + axis);
    for (axis = 0u; axis < 3u; ++axis)
        w_u16(output + 2u * axis, result[axis]);
}

static void menu_transpose_matrix(uint32 input, uint32 output)
{
    uint32 first = r_u32(input), second = r_u32(input + 4u), third, fourth;
    w_u32(output + 4u, first);
    w_u32(output, second);
    w_u16(output, first);
    third = r_u32(input + 8u);
    fourth = r_u32(input + 12u);
    w_u32(output + 12u, third);
    w_u32(output + 8u, fourth);
    w_u16(output + 12u, second);
    w_u16(output + 8u, third);
    second = (uint32)(sint32)(sint16)r_u16(input + 16u);
    w_u16(output + 4u, fourth);
    w_u16(output + 16u, second);
}

uint32 sub_8007E63C(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 depth, difference, length, axis, resource, owner, result;
    uint32 first_normals = 0x800F25D0u, second_normals = 0x800F25E8u;
    w_u16(0x800F25D0u, 0u);
    w_u16(0x800F25D2u, 0u);
    w_u16(0x800F25D4u, 0xF000u);
    result = r_u16(a2 + 10u);
    w_u16(0x800F25D8u, 0u);
    w_u16(0x800F25DAu, 0u);
    w_u16(0x800F25DCu, 0x1000u);
    w_u16(0x800F25D6u, result);
    w_u16(0x800F25DEu, 0u - r_u16(a2 + 8u));

    difference = r_u16(a2 + 14u) - r_u16(a2 + 6u);
    depth = r_u16(a2 + 16u);
    length = sub_80085B54(difference * difference + depth * depth);
    w_u16(0x800F25E0u, 0u);
    w_u16(0x800F25E2u, menu_signed_quotient(r_u16(a2 + 16u) << 12, length));
    difference = r_u16(a2 + 6u) - r_u16(a2 + 14u);
    w_u16(0x800F25E6u, 0u);
    w_u16(0x800F25E4u, menu_signed_quotient(0u - (difference << 12), length));

    difference = r_u16(a2 + 2u) - r_u16(a2 + 14u);
    depth = r_u16(a2 + 16u);
    length = sub_80085B54(difference * difference + depth * depth);
    w_u16(0x800F25E8u, 0u);
    w_u16(0x800F25EAu, menu_signed_quotient(0u - (r_u16(a2 + 16u) << 12), length));
    difference = r_u16(a2 + 14u) - r_u16(a2 + 2u);
    w_u16(0x800F25EEu, 0u);
    w_u16(0x800F25ECu, menu_signed_quotient(0u - (difference << 12), length));

    difference = r_u16(a2 + 12u) - r_u16(a2 + 4u);
    depth = r_u16(a2 + 16u);
    length = sub_80085B54(difference * 25u * difference + depth * (depth << 6));
    w_u16(0x800F25F2u, 0u);
    w_u16(0x800F25F0u, menu_signed_quotient(r_u16(a2 + 16u) << 15, length));
    difference = r_u16(a2 + 4u) - r_u16(a2 + 12u);
    w_u16(0x800F25F6u, 0u);
    w_u16(0x800F25F4u, menu_signed_quotient(0u - ((difference * 5u) << 12), length));

    difference = r_u16(a2) - r_u16(a2 + 12u);
    depth = r_u16(a2 + 16u);
    length = sub_80085B54(difference * 25u * difference + depth * (depth << 6));
    w_u16(0x800F25FAu, 0u);
    w_u16(0x800F25F8u, menu_signed_quotient(0u - (r_u16(a2 + 16u) << 15), length));
    difference = r_u16(a2 + 12u) - r_u16(a2);
    w_u16(0x800F25FEu, 0u);
    w_u16(0x800F25FCu, menu_signed_quotient(0u - ((difference * 5u) << 12), length));

    difference = r_u16(a2 + 12u) - r_u16(a2 + 4u);
    depth = r_u16(a2 + 16u);
    length = sub_80085B54(difference * difference + depth * depth);
    w_u16(0x800F2602u, 0u);
    w_u16(0x800F2600u, menu_signed_quotient(r_u16(a2 + 16u) << 12, length));
    difference = r_u16(a2 + 4u) - r_u16(a2 + 12u);
    w_u16(0x800F2606u, 0u);
    w_u16(0x800F2604u, menu_signed_quotient(0u - (difference << 12), length));

    difference = r_u16(a2) - r_u16(a2 + 12u);
    depth = r_u16(a2 + 16u);
    length = sub_80085B54(difference * difference + depth * depth);
    w_u16(0x800F260Au, 0u);
    w_u16(0x800F2608u, menu_signed_quotient(0u - (r_u16(a2 + 16u) << 12), length));
    difference = r_u16(a2 + 12u) - r_u16(a2);
    w_u16(0x800F260Eu, 0u);
    w_u16(0x800F260Cu, menu_signed_quotient(0u - (difference << 12), length));
    for (axis = 0u; axis < 3u; ++axis)
    {
        menu_rotate_normal(a1 + 52u, first_normals, 0x800F3E30u + axis * 6u);
        menu_rotate_normal(a1 + 52u, second_normals, 0x800F3E50u + axis * 6u);
        first_normals += 8u;
        second_normals += 8u;
    }
    sub_800879EC(r_u16(a2 + 16u));
    sub_800879CC(r_u16(a2 + 12u), r_u16(a2 + 14u));
    sub_8007E57C(a2, a3);
    result = r_u16(0x800FFAE8u);
    depth = r_u16(0x800FFAECu);
    w_u32(0x800FFB0Cu, a1);
    w_u32(0x800FFB08u, a2);
    w_u32(0x800FFB30u, a3);
    w_u16(a2 + 8u, result);
    w_u16(a2 + 10u, depth);
    menu_transpose_matrix(a1 + 52u, a1 + 84u);
    for (axis = 0u; axis < 3u; ++axis)
    {
        sint32 scaled = 8 * (sint32)(sint16)r_u16(a1 + 84u + axis * 2u);
        uint32 middle = r_u16(a1 + 90u + axis * 2u);
        uint32 last = r_u16(a1 + 96u + axis * 2u);
        w_u16(a1 + 122u + axis * 2u, middle);
        w_u16(a1 + 128u + axis * 2u, last);
        w_u16(a1 + 116u + axis * 2u, (uint32)(scaled / 5));
    }
    resource = sub_8006E278(0xCCE0187Cu);
    result = r_u16(resource + 2u) << 16;
    depth = r_u16(resource + 6u) << 16;
    difference = r_u32(0x800FFB40u);
    w_u32(0x800FFB24u, result);
    w_u32(0x800FFB28u, depth);
    result = r_u8(resource) | ((uint32)r_u8(resource + 1u) << 8);
    w_u32(0x800FFAB4u, difference);
    w_u32(0x800FFB40u, r_u32(0x800FF650u));
    w_u32(0x800FFB2Cu, result);
    sub_80080398(r_u32(0x800FF778u));
    sub_80080398(r_u32(0x800FF77Cu));
    sub_80080398(r_u8(0x800FF644u));
    if (!r_u32(0x800FFAB0u))
        sub_8007FE68();
    sub_8007FF70(r_u8(0x800FF644u));
    owner = r_u32(0x800FFB0Cu);
    w_u16(0x800FFB10u, 0u - r_u16(owner + 58u));
    w_u16(0x800FFB12u, 0u - r_u16(owner + 60u));
    w_u16(0x800FFB14u, 0u - r_u16(owner + 62u));
    result = (uint32)((sint32)r_u32(0x800FF37Cu) >> 12) - r_u16(owner + 8u);
    w_u16(0x800FFB16u, result);
    return result;
}

uint32 sub_8007E57C(uint32 a1, uint32 a2)
{
    sint16 rectangle[4];
    uint32 packet, next_packet, tag, result;
    rectangle[0] = (sint16)r_u16(a1 + 4u);
    rectangle[1] = (sint16)(r_u16(a1 + 6u) + r_u16(r_u32(0x800FF660u) + 2u));
    rectangle[2] = (sint16)(r_u16(a1) - r_u16(a1 + 4u));
    packet = r_u32(0x800FF668u);
    rectangle[3] = (sint16)(r_u16(a1 + 2u) - r_u16(a1 + 6u));
    xport_draft_host_sub_800890BC_p2(packet, rectangle);
    tag = r_u32(packet);
    result = r_u32(a2 + 0x3FFCu);
    w_u32(packet, (tag & 0xFF000000u) | (result & 0x00FFFFFFu));
    next_packet = packet + 12u;
    result = (r_u32(a2 + 0x3FFCu) & 0xFF000000u) | (packet & 0x00FFFFFFu);
    w_u32(a2 + 0x3FFCu, result);
    w_u32(0x800FF668u, next_packet);
    return result;
}

uint32 sub_80080398(uint32 a1)
{
    uint32 model, current, end, step = 1u, old_tick, tick;
    if (a1 == 0xFFFFFFFFu)
        return 0xFFFFFFFFu;
    model = 0x800EAEF8u + (a1 << 6);
    current = r_u32(model + 40u);
    if (!current)
        return 0x800EAEF8u;
    tick = r_u32(0x800FFB40u);
    w_u32(0x800FFB04u, r_u32(model + 32u));
    end = current + r_u32(current - 4u);
    old_tick = r_u32(0x800FFAB4u);
    if (tick >= old_tick)
        step = tick - old_tick;
    while (current < end)
    {
        uint32 keys = current + 4u;
        uint32 elapsed = r_u8(current + 3u);
        uint32 key = r_u8(current + 2u);
        uint32 count = r_u8(current + 1u);
        uint32 active = keys + (key << 2);
        uint32 duration = r_u8(active + 3u);
        uint32 next, base[3], axis, scale;
        SVECTOR delta = {0};
        VECTOR interpolated;
        elapsed += step;
        if ((sint32)elapsed >= (sint32)duration)
        {
            do
            {
                elapsed -= r_u8(active + 3u);
                active += 4u;
                ++key;
                if (key == count)
                {
                    active = keys;
                    key = 0u;
                }
            } while ((sint32)elapsed >= (sint32)r_u8(active + 3u));
        }
        w_u8(current + 2u, key);
        active = keys + (key << 2);
        w_u8(current + 3u, elapsed);
        for (axis = 0u; axis < 3u; ++axis)
            base[axis] = (uint32)r_u8(active + axis) << 4;
        next = key + 1u;
        if (next == count)
            next = 0u;
        next = keys + (next << 2);
        delta.vx = (sint16)(((uint32)r_u8(next) << 4) - base[0]);
        delta.vy = (sint16)(((uint32)r_u8(next + 1u) << 4) - base[1]);
        delta.vz = (sint16)(((uint32)r_u8(next + 2u) << 4) - base[2]);
        xport_gte_write_data(6u, 0u);
        scale = menu_signed_quotient(elapsed << 12, r_u8(active + 3u));
        xport_gte_write_data(8u, scale);
        for (axis = 0u; axis < 3u; ++axis)
            xport_gte_write_data(25u + axis, base[axis]);
        /* Existing GPL12 updates MAC, IR, RGB FIFO and FLAG */
        gte_gpl12(&delta, (sint32)scale, &interpolated);
        w_u32(r_u32(0x800FFB04u) + ((uint32)r_u8(current) << 2), xport_gte_read_data(22u));
        current += (count << 2) + 4u;
    }
    return 0u;
}

static void menu_wrap_uv(uint32 values[4], uint32 adjustment)
{
    while (((values[0] & values[1] & values[2] & values[3]) ^ (values[0] | values[1] | values[2] | values[3])) & 0x10000u)
    {
        uint32 vertex;
        for (vertex = 0u; vertex < 4u; ++vertex)
            values[vertex] += adjustment;
    }
}

uint32 sub_8007FF70(uint32 a1)
{
    uint32 model, records, object_base, result;
    if (a1 == 0xFFFFFFFFu)
        return 0xFFFFFFFFu;
    result = a1 << 6;
    model = 0x800EAEF8u + result;
    records = r_u32(model + 36u);
    if (!records)
        return result;
    object_base = r_u32(model + 20u);
    result = r_u32(records);
    while (result)
    {
        uint32 object = object_base + r_u32(records);
        uint32 coordinates = records + 16u;
        uint32 count = r_u32(records + 12u);
        if (r_u16(object) & 0x8000u)
            records = coordinates + (count << 4);
        else
        {
            uint32 tick = r_u32(0x800FFB40u);
            uint32 scroll_x = (uint32)((sint32)(tick * (uint32)(sint32)(sint16)r_u16(records + 4u)) >> 4);
            uint32 scroll_y, phase_product = tick * r_u32(records + 8u);
            uint32 geometry_table = r_u32(0x800EAEF8u + ((uint32)r_u8(object + 27u) << 6) + 16u);
            uint32 geometry = r_u32(geometry_table + ((uint32)r_u16(object + 22u) << 2));
            uint32 packet = geometry + (r_u32(geometry + 4u) << 3) + 32u;
            uint32 item = 0u;
            uint32 y_product = tick * (uint32)(sint32)(sint16)r_u16(records + 6u);
            packet += r_u32(geometry + 8u) << 3;
            w_u32(0x800FFB48u, (uint32)((sint32)phase_product >> 10));
            scroll_y = (uint32)((sint32)y_product >> 4);
            while ((sint32)item < (sint32)count)
            {
                uint32 x[4], y[4], vertex, texture, wrap_x, wrap_y, stride;
                for (vertex = 0u; vertex < 4u; ++vertex)
                {
                    x[vertex] = ((uint32)r_u8(coordinates + vertex * 4u) << 8) + scroll_x;
                    y[vertex] = ((uint32)r_u8(coordinates + vertex * 4u + 1u) << 8) + scroll_y;
                }
                texture = r_u32(packet + 32u);
                wrap_x = (0u - ((texture & 0x1Fu) << 11)) & 0xF800u;
                wrap_y = (0u - ((texture & 0x3E0u) << 6)) & 0xFF00u;
                for (vertex = 0u; vertex < 4u; ++vertex)
                {
                    x[vertex] += sub_8007FED0(coordinates + vertex * 4u);
                    y[vertex] += sub_8007FF20(coordinates + vertex * 4u);
                }
                menu_wrap_uv(x, wrap_x);
                menu_wrap_uv(y, wrap_y);
                ++item;
                coordinates += 16u;
                w_u16(packet + 20u, ((x[0] & 0xFF00u) >> 8) | (y[0] & 0xFF00u));
                w_u16(packet + 24u, ((x[1] & 0xFF00u) >> 8) | (y[1] & 0xFF00u));
                stride = r_u16(packet + 2u);
                w_u32(packet + 28u, ((x[2] & 0xFF00u) >> 8) | (y[2] & 0xFF00u) | ((x[3] & 0xFF00u) << 8) | ((y[3] & 0xFF00u) << 16));
                packet += stride & 0xFFFCu;
            }
            records = coordinates;
        }
        result = r_u32(records);
    }
    return result;
}

/* TODO Missing call adapter JUMPOUT */
/* TODO Missing call adapter sub_800821B8 */
/* TODO Missing call adapter sub_80082224 */
/* TODO Missing call adapter sub_80083BF4 */
/* TODO Missing call adapter sub_80083C0C */
/* TODO Missing call adapter sub_80083C24 */
/* TODO Missing call adapter sub_80083C3C */
/* TODO Missing call adapter sub_80083C54 */
/* TODO Missing call adapter sub_80083E54 */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* TODO Postincrement memory expressions may require ordering refinement */
static void draft_817FC_normal(uint32 geometry, uint32 normal[3])
{
    normal[0] = r_u32(geometry);
    normal[1] = (uint32)((sint32)normal[0] >> 16);
    normal[2] = r_u32(geometry + 4);
}

/* Excluded helpers cannot supply carried interpolation outputs until implemented */
#if defined(_MSC_VER)
__declspec(noreturn)
#elif defined(__GNUC__)
__attribute__((noreturn))
#endif
static void
apocalypse_agent2_renderer_boundary(uint32 target, uint32 descriptor, uint32 packet, uint32 scratch, const uint32 offsets[4])
{
    /* TODO Native renderer helper context */
    fprintf(stderr, "Missing renderer helper target=%08X descriptor=%08X packet=%08X scratch=%08X vertices=%04X/%04X/%04X/%04X\n", target, descriptor, packet, scratch, offsets[0], offsets[1], offsets[2], offsets[3]);
    abort();
}

static void apocalypse_agent2_nearplane_queue(uint32 descriptor, uint32 packet, uint32 scratch, uint32 projection, uint32 flags, uint32 offsets[4], uint32 depths[4])
{
    uint32 queue_geometry, queued, temporary, edge_value;
    uint32 value0 = depths[0], value1 = depths[1], value2 = depths[2], value3 = depths[3];
    uint32 outside0, outside1, outside2;
    /* TODO Excluded edge helpers carry edge_value and queue_geometry through their native context */
near_800837AC:;
    queue_geometry = r_u32((scratch + 468u));
near_800837B0:;
    queued = r_u32((scratch + 484u));
near_800837B4:;
    edge_value = 128u << 16;
near_800837B8:;
    outside0 = value0 & edge_value;
near_800837BC:;
    outside1 = value1 & edge_value;
near_800837C0:;
    outside2 = value2 & edge_value;
near_800837C4:;
    temporary = flags & 16u;
near_800837C8:;
    if (temporary == 0u)
    {
        goto near_800838A8;
    }
near_800837CC:;

near_800837D0:;
    if (outside0 != 0u)
    {
        goto near_80083838;
    }
near_800837D4:;

near_800837D8:;
    if (outside1 != 0u)
    {
        goto near_80083808;
    }
near_800837DC:;

near_800837E0:;

    apocalypse_agent2_renderer_boundary(0x80083C24u, descriptor, packet, scratch, offsets);
near_800837E4:;

near_800837E8:;
    edge_value = edge_value >> 16u;
near_800837EC:;
    w_u16((queued + 20u), edge_value);
near_800837F0:;

    apocalypse_agent2_renderer_boundary(0x80083C0Cu, descriptor, packet, scratch, offsets);
near_800837F4:;

near_800837F8:;
    w_u16((queued + 24u), edge_value);
near_800837FC:;
    value0 = offsets[0];
near_80083800:;
    value1 = offsets[1];
    goto near_80083EA0;
near_80083804:;
    value1 = offsets[1];
near_80083808:;
    if (outside2 != 0u)
    {
        goto near_80083A94;
    }
near_8008380C:;

near_80083810:;

    apocalypse_agent2_renderer_boundary(0x80083C0Cu, descriptor, packet, scratch, offsets);
near_80083814:;

near_80083818:;
    edge_value = edge_value >> 16u;
near_8008381C:;
    w_u16((queued + 20u), edge_value);
near_80083820:;

    apocalypse_agent2_renderer_boundary(0x80083BF4u, descriptor, packet, scratch, offsets);
near_80083824:;

near_80083828:;
    w_u16((queued + 24u), edge_value);
near_8008382C:;
    value0 = offsets[2];
near_80083830:;
    value1 = offsets[0];
    goto near_80083EA0;
near_80083834:;
    value1 = offsets[0];
near_80083838:;
    if (outside1 != 0u)
    {
        goto near_8008388C;
    }
near_8008383C:;

near_80083840:;
    if (outside2 != 0u)
    {
        goto near_80083870;
    }
near_80083844:;

near_80083848:;

    apocalypse_agent2_renderer_boundary(0x80083BF4u, descriptor, packet, scratch, offsets);
near_8008384C:;

near_80083850:;
    edge_value = edge_value >> 16u;
near_80083854:;
    w_u16((queued + 20u), edge_value);
near_80083858:;

    apocalypse_agent2_renderer_boundary(0x80083C24u, descriptor, packet, scratch, offsets);
near_8008385C:;

near_80083860:;
    w_u16((queued + 24u), edge_value);
near_80083864:;
    value0 = offsets[1];
near_80083868:;
    value1 = offsets[2];
    goto near_80083EA0;
near_8008386C:;
    value1 = offsets[2];
near_80083870:;

    apocalypse_agent2_renderer_boundary(0x80083C0Cu, descriptor, packet, scratch, offsets);
near_80083874:;

near_80083878:;
    w_u16((queued + 20u), edge_value);
near_8008387C:;

    apocalypse_agent2_renderer_boundary(0x80083BF4u, descriptor, packet, scratch, offsets);
near_80083880:;

near_80083884:;
    value0 = offsets[1];
    goto near_80083DE4;
near_80083888:;
    value0 = offsets[1];
near_8008388C:;

    apocalypse_agent2_renderer_boundary(0x80083C24u, descriptor, packet, scratch, offsets);
near_80083890:;

near_80083894:;
    w_u16((queued + 20u), edge_value);
near_80083898:;

    apocalypse_agent2_renderer_boundary(0x80083C0Cu, descriptor, packet, scratch, offsets);
near_8008389C:;

near_800838A0:;
    value0 = offsets[2];
    goto near_80083DE4;
near_800838A4:;
    value0 = offsets[2];
near_800838A8:;
    edge_value = value3 & edge_value;
near_800838AC:;
    if (outside0 != 0u)
    {
        goto near_80083AB0;
    }
near_800838B0:;

near_800838B4:;
    if (outside1 != 0u)
    {
        goto near_800839E4;
    }
near_800838B8:;

near_800838BC:;
    if (outside2 != 0u)
    {
        goto near_8008393C;
    }
near_800838C0:;

near_800838C4:;
    value0 = offsets[0] + projection;
near_800838C8:;
    value1 = r_u16((value0 + 6u));
near_800838CC:;

near_800838D0:;
    value1 = value1 & 65407u;
near_800838D4:;
    value1 = value1 | 32768u;
near_800838D8:;
    w_u16((value0 + 6u), value1);
near_800838DC:;

    apocalypse_agent2_renderer_boundary(0x80083E54u, descriptor, packet, scratch, offsets);
near_800838E0:;

near_800838E4:;
    w_u16((queued + 4u), offsets[2]);
near_800838E8:;
    w_u16((queued + 6u), offsets[0]);
near_800838EC:;
    w_u16((queued + 8u), offsets[1]);
near_800838F0:;
    w_u16((queued + 10u), offsets[1]);
near_800838F4:;
    value0 = r_u16((descriptor + 28u));
near_800838F8:;
    value1 = r_u16((descriptor + 20u));
near_800838FC:;
    value2 = r_u16((descriptor + 24u));
near_80083900:;
    w_u16((queued + 20u), value0);
near_80083904:;
    w_u16((queued + 24u), value1);
near_80083908:;
    w_u16((queued + 28u), value2);
near_8008390C:;
    value0 = flags >> 16u;
near_80083910:;
    queued = queued + value0;
near_80083914:;

    apocalypse_agent2_renderer_boundary(0x80083C54u, descriptor, packet, scratch, offsets);
near_80083918:;

near_8008391C:;
    edge_value = edge_value >> 16u;
near_80083920:;
    w_u16((queued + 20u), edge_value);
near_80083924:;

    apocalypse_agent2_renderer_boundary(0x80083C3Cu, descriptor, packet, scratch, offsets);
near_80083928:;

near_8008392C:;
    w_u16((queued + 24u), edge_value);
near_80083930:;
    value0 = offsets[2];
near_80083934:;
    value1 = offsets[1];
    goto near_80083EA0;
near_80083938:;
    value1 = offsets[1];
near_8008393C:;
    if (edge_value != 0u)
    {
        goto near_800839BC;
    }
near_80083940:;

near_80083944:;
    value0 = offsets[1] + projection;
near_80083948:;
    value1 = r_u16((value0 + 6u));
near_8008394C:;

near_80083950:;
    value1 = value1 & 65407u;
near_80083954:;
    value1 = value1 | 32768u;
near_80083958:;
    w_u16((value0 + 6u), value1);
near_8008395C:;

    apocalypse_agent2_renderer_boundary(0x80083E54u, descriptor, packet, scratch, offsets);
near_80083960:;

near_80083964:;
    w_u16((queued + 4u), offsets[0]);
near_80083968:;
    w_u16((queued + 6u), offsets[1]);
near_8008396C:;
    w_u16((queued + 8u), offsets[3]);
near_80083970:;
    w_u16((queued + 10u), offsets[3]);
near_80083974:;
    value0 = r_u16((descriptor + 20u));
near_80083978:;
    value1 = r_u16((descriptor + 24u));
near_8008397C:;
    value2 = r_u16((descriptor + 30u));
near_80083980:;
    w_u16((queued + 20u), value0);
near_80083984:;
    w_u16((queued + 24u), value1);
near_80083988:;
    w_u16((queued + 28u), value2);
near_8008398C:;
    value0 = flags >> 16u;
near_80083990:;
    queued = queued + value0;
near_80083994:;

    apocalypse_agent2_renderer_boundary(0x80083C24u, descriptor, packet, scratch, offsets);
near_80083998:;

near_8008399C:;
    edge_value = edge_value >> 16u;
near_800839A0:;
    w_u16((queued + 20u), edge_value);
near_800839A4:;

    apocalypse_agent2_renderer_boundary(0x80083C54u, descriptor, packet, scratch, offsets);
near_800839A8:;

near_800839AC:;
    w_u16((queued + 24u), edge_value);
near_800839B0:;
    value0 = offsets[0];
near_800839B4:;
    value1 = offsets[3];
    goto near_80083EA0;
near_800839B8:;
    value1 = offsets[3];
near_800839BC:;

    apocalypse_agent2_renderer_boundary(0x80083C24u, descriptor, packet, scratch, offsets);
near_800839C0:;

near_800839C4:;
    edge_value = edge_value >> 16u;
near_800839C8:;
    w_u16((queued + 20u), edge_value);
near_800839CC:;

    apocalypse_agent2_renderer_boundary(0x80083C3Cu, descriptor, packet, scratch, offsets);
near_800839D0:;

near_800839D4:;
    w_u16((queued + 24u), edge_value);
near_800839D8:;
    value0 = offsets[0];
near_800839DC:;
    value1 = offsets[1];
    goto near_80083EA0;
near_800839E0:;
    value1 = offsets[1];
near_800839E4:;
    if (outside2 != 0u)
    {
        goto near_80083A94;
    }
near_800839E8:;

near_800839EC:;
    if (edge_value != 0u)
    {
        goto near_80083A6C;
    }
near_800839F0:;

near_800839F4:;
    value0 = offsets[2] + projection;
near_800839F8:;
    value1 = r_u16((value0 + 6u));
near_800839FC:;

near_80083A00:;
    value1 = value1 & 65407u;
near_80083A04:;
    value1 = value1 | 32768u;
near_80083A08:;
    w_u16((value0 + 6u), value1);
near_80083A0C:;

    apocalypse_agent2_renderer_boundary(0x80083E54u, descriptor, packet, scratch, offsets);
near_80083A10:;

near_80083A14:;
    w_u16((queued + 4u), offsets[3]);
near_80083A18:;
    w_u16((queued + 6u), offsets[2]);
near_80083A1C:;
    w_u16((queued + 8u), offsets[0]);
near_80083A20:;
    w_u16((queued + 10u), offsets[0]);
near_80083A24:;
    value0 = r_u16((descriptor + 30u));
near_80083A28:;
    value1 = r_u16((descriptor + 28u));
near_80083A2C:;
    value2 = r_u16((descriptor + 20u));
near_80083A30:;
    w_u16((queued + 20u), value0);
near_80083A34:;
    w_u16((queued + 24u), value1);
near_80083A38:;
    w_u16((queued + 28u), value2);
near_80083A3C:;
    value0 = flags >> 16u;
near_80083A40:;
    queued = queued + value0;
near_80083A44:;

    apocalypse_agent2_renderer_boundary(0x80083C3Cu, descriptor, packet, scratch, offsets);
near_80083A48:;

near_80083A4C:;
    edge_value = edge_value >> 16u;
near_80083A50:;
    w_u16((queued + 20u), edge_value);
near_80083A54:;

    apocalypse_agent2_renderer_boundary(0x80083BF4u, descriptor, packet, scratch, offsets);
near_80083A58:;

near_80083A5C:;
    w_u16((queued + 24u), edge_value);
near_80083A60:;
    value0 = offsets[3];
near_80083A64:;
    value1 = offsets[0];
    goto near_80083EA0;
near_80083A68:;
    value1 = offsets[0];
near_80083A6C:;

    apocalypse_agent2_renderer_boundary(0x80083C54u, descriptor, packet, scratch, offsets);
near_80083A70:;

near_80083A74:;
    edge_value = edge_value >> 16u;
near_80083A78:;
    w_u16((queued + 20u), edge_value);
near_80083A7C:;

    apocalypse_agent2_renderer_boundary(0x80083BF4u, descriptor, packet, scratch, offsets);
near_80083A80:;

near_80083A84:;
    w_u16((queued + 24u), edge_value);
near_80083A88:;
    value0 = offsets[2];
near_80083A8C:;
    value1 = offsets[0];
    goto near_80083EA0;
near_80083A90:;
    value1 = offsets[0];
near_80083A94:;

    apocalypse_agent2_renderer_boundary(0x80083BF4u, descriptor, packet, scratch, offsets);
near_80083A98:;

near_80083A9C:;
    w_u16((queued + 20u), edge_value);
near_80083AA0:;

    apocalypse_agent2_renderer_boundary(0x80083C24u, descriptor, packet, scratch, offsets);
near_80083AA4:;

near_80083AA8:;
    value0 = offsets[0];
    goto near_80083DE4;
near_80083AAC:;
    value0 = offsets[0];
near_80083AB0:;
    if (outside1 != 0u)
    {
        goto near_80083B84;
    }
near_80083AB4:;

near_80083AB8:;
    if (outside2 != 0u)
    {
        goto near_80083B38;
    }
near_80083ABC:;

near_80083AC0:;
    value0 = offsets[3] + projection;
near_80083AC4:;
    value1 = r_u16((value0 + 6u));
near_80083AC8:;

near_80083ACC:;
    value1 = value1 & 65407u;
near_80083AD0:;
    value1 = value1 | 32768u;
near_80083AD4:;
    w_u16((value0 + 6u), value1);
near_80083AD8:;

    apocalypse_agent2_renderer_boundary(0x80083E54u, descriptor, packet, scratch, offsets);
near_80083ADC:;

near_80083AE0:;
    w_u16((queued + 4u), offsets[1]);
near_80083AE4:;
    w_u16((queued + 6u), offsets[3]);
near_80083AE8:;
    w_u16((queued + 8u), offsets[2]);
near_80083AEC:;
    w_u16((queued + 10u), offsets[2]);
near_80083AF0:;
    value0 = r_u16((descriptor + 24u));
near_80083AF4:;
    value1 = r_u16((descriptor + 30u));
near_80083AF8:;
    value2 = r_u16((descriptor + 28u));
near_80083AFC:;
    w_u16((queued + 20u), value0);
near_80083B00:;
    w_u16((queued + 24u), value1);
near_80083B04:;
    w_u16((queued + 28u), value2);
near_80083B08:;
    value0 = flags >> 16u;
near_80083B0C:;
    queued = queued + value0;
near_80083B10:;

    apocalypse_agent2_renderer_boundary(0x80083BF4u, descriptor, packet, scratch, offsets);
near_80083B14:;

near_80083B18:;
    edge_value = edge_value >> 16u;
near_80083B1C:;
    w_u16((queued + 20u), edge_value);
near_80083B20:;

    apocalypse_agent2_renderer_boundary(0x80083C24u, descriptor, packet, scratch, offsets);
near_80083B24:;

near_80083B28:;
    w_u16((queued + 24u), edge_value);
near_80083B2C:;
    value0 = offsets[1];
near_80083B30:;
    value1 = offsets[2];
    goto near_80083EA0;
near_80083B34:;
    value1 = offsets[2];
near_80083B38:;
    if (edge_value != 0u)
    {
        goto near_80083B68;
    }
near_80083B3C:;

near_80083B40:;

    apocalypse_agent2_renderer_boundary(0x80083BF4u, descriptor, packet, scratch, offsets);
near_80083B44:;

near_80083B48:;
    edge_value = edge_value >> 16u;
near_80083B4C:;
    w_u16((queued + 20u), edge_value);
near_80083B50:;

    apocalypse_agent2_renderer_boundary(0x80083C54u, descriptor, packet, scratch, offsets);
near_80083B54:;

near_80083B58:;
    w_u16((queued + 24u), edge_value);
near_80083B5C:;
    value0 = offsets[1];
near_80083B60:;
    value1 = offsets[3];
    goto near_80083EA0;
near_80083B64:;
    value1 = offsets[3];
near_80083B68:;

    apocalypse_agent2_renderer_boundary(0x80083C3Cu, descriptor, packet, scratch, offsets);
near_80083B6C:;

near_80083B70:;
    w_u16((queued + 20u), edge_value);
near_80083B74:;

    apocalypse_agent2_renderer_boundary(0x80083BF4u, descriptor, packet, scratch, offsets);
near_80083B78:;

near_80083B7C:;
    value0 = offsets[1];
    goto near_80083DE4;
near_80083B80:;
    value0 = offsets[1];
near_80083B84:;
    if (outside2 != 0u)
    {
        goto near_80083BD8;
    }
near_80083B88:;

near_80083B8C:;
    if (edge_value != 0u)
    {
        goto near_80083BBC;
    }
near_80083B90:;

near_80083B94:;

    apocalypse_agent2_renderer_boundary(0x80083C3Cu, descriptor, packet, scratch, offsets);
near_80083B98:;

near_80083B9C:;
    edge_value = edge_value >> 16u;
near_80083BA0:;
    w_u16((queued + 20u), edge_value);
near_80083BA4:;

    apocalypse_agent2_renderer_boundary(0x80083C24u, descriptor, packet, scratch, offsets);
near_80083BA8:;

near_80083BAC:;
    w_u16((queued + 24u), edge_value);
near_80083BB0:;
    value0 = offsets[3];
near_80083BB4:;
    value1 = offsets[2];
    goto near_80083EA0;
near_80083BB8:;
    value1 = offsets[2];
near_80083BBC:;

    apocalypse_agent2_renderer_boundary(0x80083C24u, descriptor, packet, scratch, offsets);
near_80083BC0:;

near_80083BC4:;
    w_u16((queued + 20u), edge_value);
near_80083BC8:;

    apocalypse_agent2_renderer_boundary(0x80083C54u, descriptor, packet, scratch, offsets);
near_80083BCC:;

near_80083BD0:;
    value0 = offsets[2];
    goto near_80083DE4;
near_80083BD4:;
    value0 = offsets[2];
near_80083BD8:;

    apocalypse_agent2_renderer_boundary(0x80083C54u, descriptor, packet, scratch, offsets);
near_80083BDC:;

near_80083BE0:;
    w_u16((queued + 20u), edge_value);
near_80083BE4:;

    apocalypse_agent2_renderer_boundary(0x80083C3Cu, descriptor, packet, scratch, offsets);
near_80083BE8:;

near_80083BEC:;
    value0 = offsets[3];
    goto near_80083DE4;
near_80083BF0:;
    value0 = offsets[3];
near_80083DE4:;
    temporary = r_u32(0x800FFAC0u);
near_80083DEC:;
    w_u16((queued + 4u), value0);
near_80083DF0:;
    temporary = queue_geometry - temporary;
near_80083DF4:;
    temporary = temporary + (0u - 16u);
near_80083DF8:;
    w_u16((queued + 6u), temporary);
near_80083DFC:;
    temporary = temporary + 8u;
near_80083E00:;
    w_u16((queued + 8u), temporary);
near_80083E04:;
    w_u16((queued + 10u), temporary);
near_80083E08:;
    value1 = xport_draft_gte_data_read(20);
near_80083E0C:;
    value2 = xport_draft_gte_data_read(22);
near_80083E10:;
    w_u16((queued + 24u), value1);
near_80083E14:;
    w_u16((queued + 28u), value2);
near_80083E18:;
    value0 = r_u32((descriptor + 0u));
near_80083E1C:;
    value1 = r_u32((descriptor + 12u));
near_80083E20:;
    value2 = r_u32((descriptor + 16u));
near_80083E24:;
    value3 = (uint32)(sint32)(sint16)r_u16((descriptor + 22u));
near_80083E28:;
    outside0 = (uint32)(sint32)(sint16)r_u16((descriptor + 26u));
near_80083E2C:;
    outside1 = r_u32((descriptor + 32u));
near_80083E30:;
    value0 = value0 | 16u;
near_80083E34:;
    w_u32((queued + 0u), value0);
near_80083E38:;
    w_u32((queued + 12u), value1);
near_80083E3C:;
    w_u32((queued + 16u), value2);
near_80083E40:;
    w_u16((queued + 22u), value3);
near_80083E44:;
    w_u16((queued + 26u), outside0);
near_80083E48:;
    w_u32((queued + 32u), outside1);
near_80083E4C:;

    goto near_80083F10;
near_80083E50:;

near_80083EA0:;
    temporary = r_u32(0x800FFAC0u);
near_80083EA8:;
    w_u16((queued + 4u), value0);
near_80083EAC:;
    w_u16((queued + 6u), value1);
near_80083EB0:;
    temporary = queue_geometry - temporary;
near_80083EB4:;
    temporary = temporary + (0u - 16u);
near_80083EB8:;
    w_u16((queued + 8u), temporary);
near_80083EBC:;
    temporary = temporary + 8u;
near_80083EC0:;
    w_u16((queued + 10u), temporary);
near_80083EC4:;
    value1 = xport_draft_gte_data_read(20);
near_80083EC8:;
    value2 = xport_draft_gte_data_read(22);
near_80083ECC:;
    w_u16((queued + 28u), value1);
near_80083ED0:;
    w_u16((queued + 30u), value2);
near_80083ED4:;
    value0 = r_u32((descriptor + 0u));
near_80083ED8:;
    value1 = r_u32((descriptor + 12u));
near_80083EDC:;
    value2 = r_u32((descriptor + 16u));
near_80083EE0:;
    value3 = (uint32)(sint32)(sint16)r_u16((descriptor + 22u));
near_80083EE4:;
    outside0 = (uint32)(sint32)(sint16)r_u16((descriptor + 26u));
near_80083EE8:;
    outside1 = r_u32((descriptor + 32u));
near_80083EEC:;
    temporary = 16u;
near_80083EF0:;
    temporary = ~(temporary | 0u);
near_80083EF4:;
    value0 = value0 & temporary;
near_80083EF8:;
    w_u32((queued + 0u), value0);
near_80083EFC:;
    w_u32((queued + 12u), value1);
near_80083F00:;
    w_u32((queued + 16u), value2);
near_80083F04:;
    w_u16((queued + 22u), value3);
near_80083F08:;
    w_u16((queued + 26u), outside0);
near_80083F0C:;
    w_u32((queued + 32u), outside1);
near_80083F10:;
    temporary = r_u32((scratch + 500u));
near_80083F14:;

near_80083F18:;
    temporary = temporary + 1u;
near_80083F1C:;
    w_u32((scratch + 500u), temporary);
near_80083F20:;
    value0 = flags >> 16u;
near_80083F24:;
    queued = queued + value0;
near_80083F28:;
    w_u32((scratch + 468u), queue_geometry);
near_80083F2C:;
    w_u32((scratch + 484u), queued);
near_80083F30:;

    return;
near_80083F34:;
}

static uint32 apocalypse_agent2_subdivide(uint32 descriptor, uint32 packet, uint32 scratch, uint32 limit, uint32 flags, uint32 offsets[4], uint32 source_count, uint32 area, const uint32 geometry[3])
{
    uint32 work = descriptor, work_end, parts = source_count, command, step;
    uint32 value0, value1, value2, value3, value4, value5;
    uint32 texture0, texture1, remaining, temporary, available, packet_link, clipped, clip_mask;
    /* TODO Excluded setup helpers supply subdivision parts and interpolation context */
    if (flags & 1u)
    {
        if (flags & 0x10u)
            goto subdivision_80082B90;
        goto subdivision_80082C7C;
    }
    if ((flags & 0x800u) || ((flags & 12u) == 12u))
    {
        if (flags & 0x10u)
            goto subdivision_800829A0;
        goto subdivision_80082A7C;
    }
    if (flags & 0x10u)
        goto subdivision_80082800;
    goto subdivision_800828B4;
subdivision_80082800:;
    value0 = 8064u << 16;
    packet = sub_80082394(packet, area, &step, &parts);
subdivision_80082804:;
    value0 = 8064u << 16;
subdivision_80082808:;
    clip_mask = 0u;
    sub_80082638(packet, value0, clip_mask, geometry, parts, step);
subdivision_8008280C:;
    clip_mask = 0u;
subdivision_80082810:;
    value0 = r_u32((packet + 8u));
subdivision_80082814:;
    value1 = r_u32((packet + 12u));
subdivision_80082818:;
    value2 = r_u32((packet + 16u));
    apocalypse_agent2_renderer_boundary(0x8008270Cu, work, packet, scratch, offsets);
subdivision_8008281C:;
    value2 = r_u32((packet + 16u));
subdivision_80082820:;
    work = 8064u << 16;
subdivision_80082824:;
    work_end = parts + 1u;
subdivision_80082828:;
    work_end = work_end << 4u;
subdivision_8008282C:;
    work_end = work + work_end;
subdivision_80082830:;
    remaining = parts + (0u - 1u);
subdivision_80082834:;
    value3 = r_u32((packet + 4u));
subdivision_80082838:;
    parts = 32u;
subdivision_8008283C:;
    available = packet < limit;
subdivision_80082840:;
    if (available == 0u)
    {
        value1 = r_u32((work + 0u));
        apocalypse_agent2_renderer_boundary(0x80082F60u, work, packet, scratch, offsets);
        return packet;
    }
subdivision_80082844:;
    value1 = r_u32((work + 0u));
subdivision_80082848:;
    value4 = r_u32((work + 128u));
subdivision_8008284C:;
    temporary = 2048u << 16;
subdivision_80082850:;
    value3 = value3 ^ temporary;
subdivision_80082854:;
    temporary = remaining + (0u - 1u);
subdivision_80082858:;
    clipped = value1 & value4;
subdivision_8008285C:;
    if ((sint32)temporary >= 0)
    {
        clipped = clipped & clip_mask;
        apocalypse_agent2_renderer_boundary(0x800827A4u, work, packet, scratch, offsets);
    }
subdivision_80082860:;
    clipped = clipped & clip_mask;
subdivision_80082864:;
    temporary = 2048u << 16;
subdivision_80082868:;
    value3 = value3 ^ temporary;
subdivision_8008286C:;
    w_u32((packet + 8u), value1);
subdivision_80082870:;
    value1 = r_u32((work + 16u));
subdivision_80082874:;
    w_u32((packet + 12u), value4);
subdivision_80082878:;
    clipped = clipped & value1;
subdivision_8008287C:;
    if (clipped != 0u)
    {
        w_u32((packet + 4u), value3);
        goto subdivision_8008289C;
    }
subdivision_80082880:;
    w_u32((packet + 4u), value3);
subdivision_80082884:;
    packet = packet + 20u;
subdivision_80082888:;
    packet_link = 1024u << 16;
subdivision_8008288C:;
    packet_link = packet_link + packet;
subdivision_80082890:;
    w_u32((packet + (0u - 20u)), packet_link);
subdivision_80082894:;
    w_u32((packet + (0u - 4u)), value1);
subdivision_80082898:;
    packet_link = 20u;
subdivision_8008289C:;
    work = work | 127u;
subdivision_800828A0:;
    work = work + 1u;
subdivision_800828A4:;
    if (remaining != 0u)
    {
        remaining = remaining + (0u - 1u);
        goto subdivision_8008283C;
    }
subdivision_800828A8:;
    remaining = remaining + (0u - 1u);
subdivision_800828AC:;

    apocalypse_agent2_renderer_boundary(0x80082F60u, work, packet, scratch, offsets);
    return packet;
subdivision_800828B0:;

subdivision_800828B4:;
    value0 = 8064u << 16;
    packet = sub_80082394(packet, area, &step, &parts);
subdivision_800828B8:;
    value0 = 8064u << 16;
subdivision_800828BC:;
    clip_mask = 0u;
    sub_80082638(packet, value0, clip_mask, geometry, parts, step);
subdivision_800828C0:;
    clip_mask = 0u;
subdivision_800828C4:;

    apocalypse_agent2_renderer_boundary(0x8008267Cu, work, packet, scratch, offsets);
subdivision_800828C8:;

subdivision_800828CC:;
    value0 = r_u32((packet + 8u));
subdivision_800828D0:;
    value1 = r_u32((packet + 12u));
subdivision_800828D4:;
    value2 = r_u32((packet + 16u));
subdivision_800828D8:;
    value3 = r_u32((packet + 20u));
    apocalypse_agent2_renderer_boundary(0x80082750u, work, packet, scratch, offsets);
subdivision_800828DC:;
    value3 = r_u32((packet + 20u));
subdivision_800828E0:;
    work = 8064u << 16;
subdivision_800828E4:;
    work_end = parts + 1u;
subdivision_800828E8:;
    work_end = work_end << 4u;
subdivision_800828EC:;
    work_end = work + work_end;
subdivision_800828F0:;
    remaining = parts + (0u - 1u);
subdivision_800828F4:;
    value3 = r_u32((packet + 4u));
subdivision_800828F8:;
    available = packet < limit;
subdivision_800828FC:;
    if (available == 0u)
    {
        value1 = r_u32((work + 0u));
        apocalypse_agent2_renderer_boundary(0x80082F60u, work, packet, scratch, offsets);
        return packet;
    }
subdivision_80082900:;
    value1 = r_u32((work + 0u));
subdivision_80082904:;
    value4 = r_u32((work + 128u));
subdivision_80082908:;
    temporary = parts + (0u - 1u);
subdivision_8008290C:;
    clipped = value2 & value5;
subdivision_80082910:;
    clipped = clipped & clip_mask;
    apocalypse_agent2_renderer_boundary(0x800827A4u, work, packet, scratch, offsets);
subdivision_80082914:;
    clipped = clipped & clip_mask;
subdivision_80082918:;
    work = work | 127u;
subdivision_8008291C:;
    work = work + 1u;
subdivision_80082920:;
    if (remaining != 0u)
    {
        remaining = remaining + (0u - 1u);
        goto subdivision_800828F8;
    }
subdivision_80082924:;
    remaining = remaining + (0u - 1u);
subdivision_80082928:;

    apocalypse_agent2_renderer_boundary(0x80082F60u, work, packet, scratch, offsets);
    return packet;
subdivision_8008292C:;

subdivision_800829A0:;
    value0 = 8064u << 16;
    packet = sub_80082394(packet, area, &step, &parts);
subdivision_800829A4:;
    value0 = 8064u << 16;
subdivision_800829A8:;
    clip_mask = 32768u << 16;
subdivision_800829AC:;
    temporary = packet;
subdivision_800829B0:;
    value0 = 8u;
    sub_80082638(packet, value0, clip_mask, geometry, parts, step);
subdivision_800829B4:;
    value0 = 8u;
subdivision_800829B8:;
    value0 = r_u32((packet + 8u));
subdivision_800829BC:;
    value1 = r_u32((packet + 16u));
subdivision_800829C0:;
    value2 = r_u32((packet + 24u));
    apocalypse_agent2_renderer_boundary(0x8008270Cu, work, packet, scratch, offsets);
subdivision_800829C4:;
    value2 = r_u32((packet + 24u));
subdivision_800829C8:;
    work = 8064u << 16;
subdivision_800829CC:;
    work_end = parts + 1u;
subdivision_800829D0:;
    work_end = work_end << 4u;
subdivision_800829D4:;
    work_end = work + work_end;
subdivision_800829D8:;
    command = r_u32((packet + 4u));
subdivision_800829DC:;
    remaining = parts + (0u - 1u);
subdivision_800829E0:;
    command = command >> 24u;
subdivision_800829E4:;
    command = command << 24u;
subdivision_800829E8:;
    parts = 32u;
subdivision_800829EC:;
    temporary = 2048u << 16;
subdivision_800829F0:;
    command = command ^ temporary;
subdivision_800829F4:;
    available = packet < limit;
subdivision_800829F8:;
    if (available == 0u)
    {
        value0 = r_u32((work + 12u));
        apocalypse_agent2_renderer_boundary(0x80082F60u, work, packet, scratch, offsets);
        return packet;
    }
subdivision_800829FC:;
    value0 = r_u32((work + 12u));
subdivision_80082A00:;
    value1 = r_u32((work + 0u));
subdivision_80082A04:;
    value3 = r_u32((work + 140u));
subdivision_80082A08:;
    value4 = r_u32((work + 128u));
subdivision_80082A0C:;
    temporary = remaining + (0u - 1u);
subdivision_80082A10:;
    clipped = value1 & value4;
subdivision_80082A14:;
    if ((sint32)temporary >= 0)
    {
        clipped = clipped & clip_mask;
        apocalypse_agent2_renderer_boundary(0x80082930u, work, packet, scratch, offsets);
    }
subdivision_80082A18:;
    clipped = clipped & clip_mask;
subdivision_80082A1C:;
    value0 = value0 | command;
subdivision_80082A20:;
    temporary = 2048u << 16;
subdivision_80082A24:;
    value0 = value0 ^ temporary;
subdivision_80082A28:;
    w_u32((packet + 4u), value0);
subdivision_80082A2C:;
    w_u32((packet + 8u), value1);
subdivision_80082A30:;
    w_u32((packet + 12u), value3);
subdivision_80082A34:;
    value1 = r_u32((work + 16u));
subdivision_80082A38:;
    value0 = r_u32((work + 28u));
subdivision_80082A3C:;
    clipped = clipped & value1;
subdivision_80082A40:;
    if (clipped != 0u)
    {
        w_u32((packet + 16u), value4);
        goto subdivision_80082A64;
    }
subdivision_80082A44:;
    w_u32((packet + 16u), value4);
subdivision_80082A48:;
    packet = packet + 28u;
subdivision_80082A4C:;
    packet_link = 1536u << 16;
subdivision_80082A50:;
    packet_link = packet_link + packet;
subdivision_80082A54:;
    w_u32((packet + (0u - 28u)), packet_link);
subdivision_80082A58:;
    w_u32((packet + (0u - 8u)), value0);
subdivision_80082A5C:;
    w_u32((packet + (0u - 4u)), value1);
subdivision_80082A60:;
    packet_link = 28u;
subdivision_80082A64:;
    work = work | 127u;
subdivision_80082A68:;
    work = work + 1u;
subdivision_80082A6C:;
    if (remaining != 0u)
    {
        remaining = remaining + (0u - 1u);
        goto subdivision_800829F4;
    }
subdivision_80082A70:;
    remaining = remaining + (0u - 1u);
subdivision_80082A74:;

    apocalypse_agent2_renderer_boundary(0x80082F60u, work, packet, scratch, offsets);
    return packet;
subdivision_80082A78:;

subdivision_80082A7C:;
    value0 = 8064u << 16;
    packet = sub_80082394(packet, area, &step, &parts);
subdivision_80082A80:;
    value0 = 8064u << 16;
subdivision_80082A84:;
    clip_mask = 32768u << 16;
subdivision_80082A88:;
    temporary = packet;
subdivision_80082A8C:;
    value0 = 8u;
    sub_80082638(packet, value0, clip_mask, geometry, parts, step);
subdivision_80082A90:;
    value0 = 8u;
subdivision_80082A94:;
    temporary = packet + 24u;
subdivision_80082A98:;
    value0 = 4294967288u;
    apocalypse_agent2_renderer_boundary(0x8008267Cu, work, packet, scratch, offsets);
subdivision_80082A9C:;
    value0 = 4294967288u;
subdivision_80082AA0:;
    value0 = r_u32((packet + 8u));
subdivision_80082AA4:;
    value1 = r_u32((packet + 16u));
subdivision_80082AA8:;
    value2 = r_u32((packet + 24u));
subdivision_80082AAC:;
    value3 = r_u32((packet + 32u));
    apocalypse_agent2_renderer_boundary(0x80082750u, work, packet, scratch, offsets);
subdivision_80082AB0:;
    value3 = r_u32((packet + 32u));
subdivision_80082AB4:;
    work = 8064u << 16;
subdivision_80082AB8:;
    work_end = parts + 1u;
subdivision_80082ABC:;
    work_end = work_end << 4u;
subdivision_80082AC0:;
    work_end = work + work_end;
subdivision_80082AC4:;
    command = r_u32((packet + 4u));
subdivision_80082AC8:;
    remaining = parts + (0u - 1u);
subdivision_80082ACC:;
    command = command >> 24u;
subdivision_80082AD0:;
    command = command << 24u;
subdivision_80082AD4:;
    available = packet < limit;
subdivision_80082AD8:;
    if (available == 0u)
    {
        value0 = r_u32((work + 12u));
        apocalypse_agent2_renderer_boundary(0x80082F60u, work, packet, scratch, offsets);
        return packet;
    }
subdivision_80082ADC:;
    value0 = r_u32((work + 12u));
subdivision_80082AE0:;
    value1 = r_u32((work + 0u));
subdivision_80082AE4:;
    value3 = r_u32((work + 140u));
subdivision_80082AE8:;
    value4 = r_u32((work + 128u));
subdivision_80082AEC:;
    temporary = parts + (0u - 1u);
subdivision_80082AF0:;
    clipped = value1 & value4;
subdivision_80082AF4:;
    clipped = clipped & clip_mask;
    apocalypse_agent2_renderer_boundary(0x80082930u, work, packet, scratch, offsets);
subdivision_80082AF8:;
    clipped = clipped & clip_mask;
subdivision_80082AFC:;
    work = work | 127u;
subdivision_80082B00:;
    work = work + 1u;
subdivision_80082B04:;
    if (remaining != 0u)
    {
        remaining = remaining + (0u - 1u);
        goto subdivision_80082AD4;
    }
subdivision_80082B08:;
    remaining = remaining + (0u - 1u);
subdivision_80082B0C:;

    apocalypse_agent2_renderer_boundary(0x80082F60u, work, packet, scratch, offsets);
    return packet;
subdivision_80082B10:;

subdivision_80082B90:;
    value0 = 8064u << 16;
    packet = sub_80082394(packet, area, &step, &parts);
subdivision_80082B94:;
    value0 = 8064u << 16;
subdivision_80082B98:;
    clip_mask = 16384u << 16;
subdivision_80082B9C:;
    temporary = packet;
subdivision_80082BA0:;
    value0 = 8u;
    sub_80082638(packet, value0, clip_mask, geometry, parts, step);
subdivision_80082BA4:;
    value0 = 8u;
subdivision_80082BA8:;
    value0 = r_u32((packet + 8u));
subdivision_80082BAC:;
    value1 = r_u32((packet + 16u));
subdivision_80082BB0:;
    value2 = r_u32((packet + 24u));
    apocalypse_agent2_renderer_boundary(0x8008270Cu, work, packet, scratch, offsets);
subdivision_80082BB4:;
    value2 = r_u32((packet + 24u));
subdivision_80082BB8:;
    work = 8064u << 16;
subdivision_80082BBC:;
    work_end = parts + 1u;
subdivision_80082BC0:;
    work_end = work_end << 4u;
subdivision_80082BC4:;
    work_end = work + work_end;
subdivision_80082BC8:;
    remaining = parts + (0u - 1u);
subdivision_80082BCC:;
    texture0 = (uint32)(sint32)(sint16)r_u16((packet + 14u));
subdivision_80082BD0:;
    texture1 = (uint32)(sint32)(sint16)r_u16((packet + 22u));
subdivision_80082BD4:;
    value3 = r_u32((packet + 4u));
subdivision_80082BD8:;
    texture0 = texture0 << 16u;
subdivision_80082BDC:;
    texture1 = texture1 << 16u;
subdivision_80082BE0:;
    parts = 32u;
subdivision_80082BE4:;
    available = packet < limit;
subdivision_80082BE8:;
    if (available == 0u)
    {
        value1 = r_u32((work + 0u));
        apocalypse_agent2_renderer_boundary(0x80082F60u, work, packet, scratch, offsets);
        return packet;
    }
subdivision_80082BEC:;
    value1 = r_u32((work + 0u));
subdivision_80082BF0:;
    value2 = r_u16((work + 8u));
subdivision_80082BF4:;
    value4 = r_u32((work + 128u));
subdivision_80082BF8:;
    value5 = r_u16((work + 136u));
subdivision_80082BFC:;
    temporary = 2048u << 16;
subdivision_80082C00:;
    value3 = value3 ^ temporary;
subdivision_80082C04:;
    temporary = remaining + (0u - 1u);
subdivision_80082C08:;
    clipped = value1 & value4;
subdivision_80082C0C:;
    if ((sint32)temporary >= 0)
    {
        clipped = clipped & clip_mask;
        apocalypse_agent2_renderer_boundary(0x80082B14u, work, packet, scratch, offsets);
    }
subdivision_80082C10:;
    clipped = clipped & clip_mask;
subdivision_80082C14:;
    temporary = 2048u << 16;
subdivision_80082C18:;
    value3 = value3 ^ temporary;
subdivision_80082C1C:;
    value2 = value2 | texture0;
subdivision_80082C20:;
    value5 = value5 | texture1;
subdivision_80082C24:;
    w_u32((packet + 4u), value3);
subdivision_80082C28:;
    w_u32((packet + 8u), value1);
subdivision_80082C2C:;
    w_u32((packet + 12u), value2);
subdivision_80082C30:;
    w_u32((packet + 16u), value4);
subdivision_80082C34:;
    value1 = r_u32((work + 16u));
subdivision_80082C38:;
    value2 = r_u16((work + 24u));
subdivision_80082C3C:;
    clipped = clipped & value1;
subdivision_80082C40:;
    if (clipped != 0u)
    {
        w_u32((packet + 20u), value5);
        goto subdivision_80082C64;
    }
subdivision_80082C44:;
    w_u32((packet + 20u), value5);
subdivision_80082C48:;
    packet = packet + 32u;
subdivision_80082C4C:;
    packet_link = 1792u << 16;
subdivision_80082C50:;
    packet_link = packet_link + packet;
subdivision_80082C54:;
    w_u32((packet + (0u - 32u)), packet_link);
subdivision_80082C58:;
    w_u32((packet + (0u - 8u)), value1);
subdivision_80082C5C:;
    w_u32((packet + (0u - 4u)), value2);
subdivision_80082C60:;
    packet_link = 32u;
subdivision_80082C64:;
    work = work | 127u;
subdivision_80082C68:;
    work = work + 1u;
subdivision_80082C6C:;
    if (remaining != 0u)
    {
        remaining = remaining + (0u - 1u);
        goto subdivision_80082BE4;
    }
subdivision_80082C70:;
    remaining = remaining + (0u - 1u);
subdivision_80082C74:;

    apocalypse_agent2_renderer_boundary(0x80082F60u, work, packet, scratch, offsets);
    return packet;
subdivision_80082C78:;

subdivision_80082C7C:;
    value0 = 8064u << 16;
    packet = sub_80082394(packet, area, &step, &parts);
subdivision_80082C80:;
    value0 = 8064u << 16;
subdivision_80082C84:;
    clip_mask = 16384u << 16;
subdivision_80082C88:;
    temporary = packet;
subdivision_80082C8C:;
    value0 = 8u;
    sub_80082638(packet, value0, clip_mask, geometry, parts, step);
subdivision_80082C90:;
    value0 = 8u;
subdivision_80082C94:;
    temporary = packet + 24u;
subdivision_80082C98:;
    value0 = 4294967288u;
    apocalypse_agent2_renderer_boundary(0x8008267Cu, work, packet, scratch, offsets);
subdivision_80082C9C:;
    value0 = 4294967288u;
subdivision_80082CA0:;
    value0 = r_u32((packet + 8u));
subdivision_80082CA4:;
    value1 = r_u32((packet + 16u));
subdivision_80082CA8:;
    value2 = r_u32((packet + 24u));
subdivision_80082CAC:;
    value3 = r_u32((packet + 32u));
    apocalypse_agent2_renderer_boundary(0x80082750u, work, packet, scratch, offsets);
subdivision_80082CB0:;
    value3 = r_u32((packet + 32u));
subdivision_80082CB4:;
    work = 8064u << 16;
subdivision_80082CB8:;
    work_end = parts + 1u;
subdivision_80082CBC:;
    work_end = work_end << 4u;
subdivision_80082CC0:;
    work_end = work + work_end;
subdivision_80082CC4:;
    remaining = parts + (0u - 1u);
subdivision_80082CC8:;
    texture0 = (uint32)(sint32)(sint16)r_u16((packet + 14u));
subdivision_80082CCC:;
    texture1 = (uint32)(sint32)(sint16)r_u16((packet + 22u));
subdivision_80082CD0:;
    value3 = r_u32((packet + 4u));
subdivision_80082CD4:;
    texture0 = texture0 << 16u;
subdivision_80082CD8:;
    texture1 = texture1 << 16u;
subdivision_80082CDC:;
    available = packet < limit;
subdivision_80082CE0:;
    if (available == 0u)
    {
        value1 = r_u32((work + 0u));
        apocalypse_agent2_renderer_boundary(0x80082F60u, work, packet, scratch, offsets);
        return packet;
    }
subdivision_80082CE4:;
    value1 = r_u32((work + 0u));
subdivision_80082CE8:;
    value2 = r_u16((work + 8u));
subdivision_80082CEC:;
    value4 = r_u32((work + 128u));
subdivision_80082CF0:;
    value5 = r_u16((work + 136u));
subdivision_80082CF4:;
    temporary = parts + (0u - 1u);
subdivision_80082CF8:;
    clipped = value1 & value4;
subdivision_80082CFC:;
    clipped = clipped & clip_mask;
    apocalypse_agent2_renderer_boundary(0x80082B14u, work, packet, scratch, offsets);
subdivision_80082D00:;
    clipped = clipped & clip_mask;
subdivision_80082D04:;
    work = work | 127u;
subdivision_80082D08:;
    work = work + 1u;
subdivision_80082D0C:;
    if (remaining != 0u)
    {
        remaining = remaining + (0u - 1u);
        goto subdivision_80082CDC;
    }
subdivision_80082D10:;
    remaining = remaining + (0u - 1u);
subdivision_80082D14:;

    apocalypse_agent2_renderer_boundary(0x80082F60u, work, packet, scratch, offsets);
    return packet;
subdivision_80082D18:;

    return packet;
}

static uint32 apocalypse_agent2_subdivide_textured(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 quad, uint32 *last_stride)
{
    uint32 parts, step, work = scratch, row, before, command;
    uint32 geometry[3] = {points[0], points[1], points[2]};
    xport_draft_polygon_strip_context strip;
    sub_80082394(packet, area, &step, &parts);
    sub_80082638(packet, 12u, 0xC0000000u, geometry, parts, step);
    if (quad)
    {
        geometry[0] = points[3];
        geometry[1] = points[2];
        geometry[2] = points[1];
        sub_8008267C(packet + 36u, 0xFFFFFFF4u, 0xC0000000u, geometry, parts, step, scratch + (parts + 1u) * 128u);
        sub_80082750(parts, r_u32(packet + 8u), r_u32(packet + 20u), r_u32(packet + 32u), r_u32(packet + 44u));
    }
    else
        sub_8008270C(parts, r_u32(packet + 8u), r_u32(packet + 20u), r_u32(packet + 32u));
    command = (r_u32(packet + 4u) & 0xFF000000u) ^ 0x08000000u;
    strip.texture_high0 = (uint32)(sint32)(sint16)r_u16(packet + 14u) << 16;
    strip.texture_high1 = (uint32)(sint32)(sint16)r_u16(packet + 26u) << 16;
    strip.packet_limit = limit;
    strip.frustum_mask = 0xC0000000u;
    /* The original constructed packet supplies the initial native stride */
    strip.packet_stride = quad ? 52u : 40u;
    strip.disposition = 0u;
    row = parts - 1u;
    do
    {
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[0] = r_u32(work + 12u);
        strip.packet_words[1] = r_u32(work);
        strip.packet_words[2] = r_u16(work + 8u);
        strip.packet_words[3] = r_u32(work + 140u);
        strip.packet_words[4] = r_u32(work + 128u);
        strip.packet_words[5] = r_u16(work + 136u);
        strip.clipping_mask = strip.packet_words[1] & strip.packet_words[4] & strip.frustum_mask;
        before = packet;
        if (quad || (sint32)(row - 1u) >= 0)
        {
            strip.remaining_segments = quad ? parts - 1u : row - 1u;
            packet = sub_80082D1C(work, &strip);
            work += (quad ? parts : row) * 16u;
            if (packet != before)
                *last_stride = strip.packet_stride;
            if (strip.disposition == 1u)
                break;
        }
        if (!quad)
        {
            w_u32(packet + 4u, (strip.packet_words[0] | command) ^ 0x08000000u);
            w_u32(packet + 8u, strip.packet_words[1]);
            w_u32(packet + 12u, strip.packet_words[2] | strip.texture_high0);
            w_u32(packet + 16u, strip.packet_words[3]);
            w_u32(packet + 20u, strip.packet_words[4]);
            before = r_u32(work + 16u);
            if (!(strip.clipping_mask & before))
            {
                w_u32(packet + 24u, strip.packet_words[5] | strip.texture_high1);
                packet += 40u;
                w_u32(packet - 40u, packet + 0x09000000u);
                w_u32(packet - 12u, r_u32(work + 28u));
                w_u32(packet - 8u, before);
                w_u32(packet - 4u, r_u16(work + 24u));
                *last_stride = 40u;
                strip.packet_stride = 40u;
            }
            else
                w_u32(packet + 24u, strip.packet_words[5] | strip.texture_high1);
        }
        work = (work | 127u) + 1u;
    } while (row-- != 0u);
    /* The common continuation restores semantic caller state through native locals */
    xport_draft_gte_data_write(6u, 0u);
    return packet;
}

static uint32 apocalypse_agent2_subdivide_flat_textured_quad(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 *last_stride)
{
    uint32 parts, step, row, work = scratch, command, before;
    uint32 geometry[3] = {points[0], points[1], points[2]};
    xport_draft_polygon_strip_context strip;
    packet = sub_80082394(packet, area, &step, &parts);
    sub_80082638(packet, 8u, 0x40000000u, geometry, parts, step);
    geometry[0] = points[3];
    geometry[1] = points[2];
    geometry[2] = points[1];
    sub_8008267C(packet + 24u, 0xFFFFFFF8u, 0x40000000u, geometry, parts, step, scratch + (parts + 1u) * 128u);
    sub_80082750(parts, r_u32(packet + 8u), r_u32(packet + 16u), r_u32(packet + 24u), r_u32(packet + 32u));
    command = r_u32(packet + 4u);
    strip.texture_high0 = (uint32)(sint32)(sint16)r_u16(packet + 14u) << 16;
    strip.texture_high1 = (uint32)(sint32)(sint16)r_u16(packet + 22u) << 16;
    strip.packet_limit = limit;
    strip.frustum_mask = 0x40000000u;
    strip.packet_stride = 40u;
    strip.disposition = 0u;
    row = parts - 1u;
    do
    {
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[0] = command;
        strip.packet_words[1] = r_u32(work);
        strip.packet_words[2] = r_u16(work + 8u);
        strip.packet_words[3] = r_u32(work + 128u);
        strip.packet_words[4] = r_u16(work + 136u);
        strip.clipping_mask = strip.packet_words[1] & strip.packet_words[3] & strip.frustum_mask;
        strip.remaining_segments = parts - 1u;
        before = packet;
        packet = sub_80082B14(work, &strip);
        if (packet != before)
            *last_stride = strip.packet_stride;
        if (strip.disposition == 1u)
            break;
        work = (work | 127u) + 1u;
    } while (row-- != 0u);
    /* The common continuation retains caller state through native locals */
    xport_draft_gte_data_write(6u, 0u);
    return packet;
}

static uint32 apocalypse_agent2_subdivide_flat_quad(uint32 packet, uint32 limit, uint32 scratch, const uint32 points[4], uint32 area, uint32 *last_stride)
{
    uint32 parts, step, row, work = scratch, command, before;
    uint32 geometry[3] = {points[0], points[1], points[2]};
    xport_draft_polygon_strip_context strip;
    packet = sub_80082394(packet, area, &step, &parts);
    sub_80082638(packet, scratch, 0u, geometry, parts, step);
    geometry[0] = points[3];
    geometry[1] = points[2];
    geometry[2] = points[1];
    sub_8008267C(packet, scratch, 0u, geometry, parts, step, scratch + (parts + 1u) * 128u);
    sub_80082750(parts, r_u32(packet + 8u), r_u32(packet + 12u), r_u32(packet + 16u), r_u32(packet + 20u));
    command = r_u32(packet + 4u);
    strip.packet_limit = limit;
    strip.frustum_mask = 0u;
    strip.packet_stride = 24u;
    strip.disposition = 0u;
    row = parts - 1u;
    do
    {
        if (packet >= limit)
            break;
        strip.packet_cursor = packet;
        strip.packet_words[1] = command;
        strip.packet_words[2] = r_u32(work);
        strip.packet_words[3] = r_u32(work + 128u);
        strip.clipping_mask = 0u;
        strip.remaining_segments = parts - 1u;
        before = packet;
        packet = sub_800827A4(work, &strip);
        if (packet != before)
            *last_stride = strip.packet_stride;
        if (strip.disposition == 1u)
            break;
        work = (work | 127u) + 1u;
    } while (row-- != 0u);
    /* The common continuation retains caller state through native locals */
    xport_draft_gte_data_write(6u, 0u);
    return packet;
}

void sub_800817FC(uint32 descriptor, uint32 unused, uint32 count)
{
    const uint32 scratch = 0x1F800000u;
    const uint32 normals = 0x800F3E70u;
    uint32 packet, projection, table, ordering, limit;
    if (!count)
        return;
    packet = r_u32(0x800FF668u) & 0xFFFFFFu;
    projection = r_u32(0x800FFAC0u);
    table = r_u32(0x800FFB04u);
    ordering = r_u32(0x800FFB30u);
    limit = r_u32(0x800FF374u) - 100u;
    xport_draft_gte_control_write(5, 0);
    xport_draft_gte_control_write(6, 0);
    xport_draft_gte_control_write(7, 0);
    xport_draft_gte_control_write(28, 0x10000000u);
    xport_draft_gte_control_write(27, 0);
    w_u32(scratch + 0x1E4, 0x80082FACu);
    w_u32(scratch + 0x1F4, 0);
    for (;;)
    {
        uint32 offsets[4], points[4], depths[4], flags, clipping, area;
        uint32 packed = r_u32(descriptor + 4);
        uint32 packed2 = r_u32(descriptor + 8);
        uint32 mask = r_u32(scratch + 0x144);
        uint32 depth, fog = 0, vertices, colored, textured, tag, command;
        uint32 color[4], uv[4], normal[3], index[4];
        uint32 i, bucket, previous, bytes, chain_head = packet;
        offsets[0] = packed & 0xFFFFu;
        offsets[1] = packed >> 16;
        offsets[2] = packed2 & 0xFFFFu;
        offsets[3] = packed2 >> 16;
        flags = (r_u32(descriptor) & (uint32)((sint32)mask >> 16)) | (mask & 0xFFFFu);
        for (i = 0; i < 4; ++i)
            points[i] = projection + offsets[i];
        for (i = 0; i < 3; ++i)
            xport_draft_gte_data_write(12 + i, r_u32(points[i]));
        if (!(flags & 0xC0u))
            goto next_descriptor;
        xport_draft_gte_execute(0x1400006u);
        for (i = 0; i < 4; ++i)
            depths[i] = r_u32(points[i] + 4);
        w_u32(packet + 8, xport_draft_gte_data_read(12));
        clipping = ((depths[0] & depths[1] & depths[2] & depths[3]) >> 16) ^ 0xFF00u;
        xport_draft_gte_data_write(15, r_u32(points[3]));
        area = xport_draft_gte_data_read(24);
        xport_draft_gte_execute(0x1400006u);
        if (clipping & 0x20FFu)
            goto next_descriptor;
        if (clipping & 0x5000u)
        {
            uint32 geometry0 = points[0] + 0x1F40u;
            uint32 geometry1 = points[1] + 0x1F40u;
            uint32 geometry2 = points[2] + 0x1F40u;
            uint32 cross[3], magnitude = 0;
            if (!(flags & 0x1000u))
                goto next_descriptor;
            packed = r_u32(geometry0);
            xport_draft_gte_control_write(0, packed);
            xport_draft_gte_control_write(2, packed >> 16);
            xport_draft_gte_control_write(4, r_u32(points[0] + 4));
            packed = r_u32(geometry1);
            xport_draft_gte_data_write(9, packed);
            xport_draft_gte_data_write(10, packed >> 16);
            xport_draft_gte_data_write(11, r_u32(points[1] + 4));
            xport_draft_gte_execute(0x170000Cu);
            if ((sint32)xport_draft_gte_control_read(31) < 0)
            {
                uint32 shift;
                for (i = 0; i < 3; ++i)
                {
                    cross[i] = xport_draft_gte_data_read(25 + i);
                    magnitude |= (sint32)cross[i] < 0 ? 0u - cross[i] : cross[i];
                }
                /* LZCS/LZCR supplies a local signed leading-bit count */
                shift = (17u - (uint32)Lzc((sint32)magnitude)) & 31u;
                for (i = 0; i < 3; ++i)
                    xport_draft_gte_data_write(9 + i, (uint32)((sint32)cross[i] >> shift));
            }
            xport_draft_gte_control_write(0, r_u32(geometry2));
            xport_draft_gte_control_write(1, r_u32(points[2] + 4));
            xport_draft_gte_execute(0x41E012u);
            if ((sint32)xport_draft_gte_data_read(25) < 0)
                goto next_descriptor;
            area = 0x7FFF0000u;
        }
        else if ((sint32)area <= 0)
        {
            area = xport_draft_gte_data_read(24);
            if ((sint32)area >= 0)
                goto next_descriptor;
            area = 0u - area;
        }
        xport_draft_gte_data_write(24, area);
        if (packet >= limit)
            break;
        if (clipping & 0x8000u)
        {
            apocalypse_agent2_nearplane_queue(descriptor, packet, scratch, projection, flags, offsets, depths);
            goto next_descriptor;
        }
        depth = depths[0] << 16;
        for (i = 1; i < 4; ++i)
        {
            uint32 candidate = depths[i] << 16;
            if ((sint32)(depth - candidate) < 0)
                depth = candidate;
        }
        depth = (uint32)((sint32)depth >> 16);
        packed = depth - r_u32(scratch + 0x164);
        if ((sint32)packed > 0)
        {
            uint32 shift = r_u32(scratch + 0x174);
            fog = (sint32)shift >= 0 ? packed << (shift & 31u) : (uint32)((sint32)packed >> ((0u - shift) & 31u));
        }
        xport_draft_gte_data_write(8, fog);
        vertices = flags & 0x10u ? 3u : 4u;
        colored = (flags & 0x800u) || ((flags & 12u) == 12u);
        textured = colored ? (flags & 1u) : (flags & 3u);
        if (flags & 0x800u)
        {
            packed = r_u32(descriptor + 12);
            index[0] = packed << 2;
            index[1] = packed >> 6;
            index[2] = packed >> 14;
            index[3] = packed >> 22;
            if ((flags & 12u) == 0)
            {
                if (vertices == 4)
                {
                    xport_draft_gte_data_write(6, r_u32(table + (index[0] & 0x3FCu)));
                    xport_draft_gte_execute(0x780010u);
                    color[0] = xport_draft_gte_data_read(22);
                    for (i = 1; i < 4; ++i)
                        xport_draft_gte_data_write(19 + i, r_u32(table + (index[i] & 0x3FCu)));
                }
                else
                {
                    for (i = 0; i < 3; ++i)
                        xport_draft_gte_data_write(20 + i, r_u32(table + (index[i] & 0x3FCu)));
                }
                xport_draft_gte_execute(0xF8002Au);
            }
            else
            {
                if (!(flags & 4u))
                {
                    normal[0] = r_u32(scratch + 0x1A4);
                    normal[1] = r_u32(scratch + 0x1B4);
                    normal[2] = r_u32(scratch + 0x1C4);
                }
                else if (!(flags & 8u))
                    draft_817FC_normal(normals + r_u32(descriptor + 16), normal);
                for (i = 0; i < vertices; ++i)
                {
                    if ((flags & 12u) == 12u)
                        draft_817FC_normal(normals + offsets[i], normal);
                    sub_80082200(index[i], table, normal[0], normal[1], normal[2]);
                    if (i == 3)
                        color[0] = xport_draft_gte_data_read(20);
                    xport_draft_gte_execute(0x680029u);
                }
            }
        }
        else
        {
            xport_draft_gte_data_write(6, r_u32(descriptor + 12));
            if ((flags & 12u) == 12u)
            {
                for (i = 0; i < vertices; ++i)
                {
                    sub_800821E0(normals + offsets[i]);
                    if (i == 3)
                        color[0] = xport_draft_gte_data_read(20);
                    xport_draft_gte_execute(0x680029u);
                }
            }
            else
            {
                if (flags & 4u)
                    sub_800821E0(normals + r_u32(descriptor + 16));
                else if (flags & 8u)
                    sub_80082254(scratch);
                xport_draft_gte_execute(flags & 12u ? 0x680029u : 0x780010u);
            }
        }
        if (colored)
        {
            if (vertices == 3)
                color[0] = xport_draft_gte_data_read(20);
            for (i = 1; i < vertices; ++i)
                color[i] = xport_draft_gte_data_read((vertices == 3 ? 20u : 19u) + i);
        }
        else
            color[0] = xport_draft_gte_data_read(22);
        if (textured)
        {
            if (flags & 1u)
            {
                uv[0] = r_u32(descriptor + 20);
                uv[1] = r_u32(descriptor + 24);
                uv[2] = r_u32(descriptor + 28);
                uv[3] = uv[2] >> 16;
            }
            else
            {
                for (i = 0; i < vertices; ++i)
                    uv[i] = r_u32(normals + offsets[i] + 4);
                uv[0] |= r_u32(scratch + 0x184);
                uv[1] |= r_u32(scratch + 0x194);
                if (vertices == 4)
                {
                    uv[2] |= uv[3] << 16;
                    uv[3] = uv[2] >> 16;
                }
            }
            uv[1] |= (flags & 0x180u) << 14;
        }
        command = 0x20000000u | (vertices == 4 ? 0x08000000u : 0) | (colored ? 0x10000000u : 0) | (textured ? 0x04000000u : 0);
        sub_80082188(packet, color[0], command, flags);
        if (colored)
        {
            uint32 stride = textured ? 12u : 8u;
            for (i = 0; i < vertices; ++i)
            {
                if (i)
                {
                    w_u32(packet + 4 + i * stride, color[i]);
                    w_u32(packet + 8 + i * stride, xport_draft_gte_data_read(11 + i));
                }
                if (textured)
                    w_u32(packet + 12 + i * stride, uv[i]);
            }
            tag = ((vertices * (textured ? 3u : 2u)) << 24);
        }
        else
        {
            for (i = 0; i < vertices; ++i)
            {
                if (i)
                    w_u32(packet + 8 + i * (textured ? 8u : 4u), xport_draft_gte_data_read(11 + i));
                if (textured)
                    w_u32(packet + 12 + i * 8u, uv[i]);
            }
            tag = (1u + vertices * (textured ? 2u : 1u)) << 24;
        }
        if ((flags & 0x1000u) && (sint32)(0x800u - xport_draft_gte_data_read(24)) < 0)
        {
            if ((flags & 1u) && ((flags & 0x800u) || ((flags & 12u) == 12u)))
            {
                uint32 last_stride, end = apocalypse_agent2_subdivide_textured(packet, limit, scratch, points, xport_draft_gte_data_read(24), !(flags & 0x10u), &last_stride);
                if (end == packet)
                    goto next_descriptor;
                packet = end - last_stride;
                tag = (last_stride - 4u) << 22;
            }
            else if ((flags & 1u) && !(flags & 0x10u))
            {
                uint32 last_stride = 40u, end = apocalypse_agent2_subdivide_flat_textured_quad(packet, limit, scratch, points, xport_draft_gte_data_read(24u), &last_stride);
                if (end == packet)
                    goto next_descriptor;
                packet = end - last_stride;
                tag = (last_stride - 4u) << 22;
            }
            else if (!(flags & (0x10u | 0x800u)) && ((flags & 12u) != 12u) && !(flags & 1u))
            {
                uint32 last_stride = 24u, end = apocalypse_agent2_subdivide_flat_quad(packet, limit, scratch, points, xport_draft_gte_data_read(24u), &last_stride);
                if (end == packet)
                    goto next_descriptor;
                packet = end - last_stride;
                tag = (last_stride - 4u) << 22;
            }
            else
            {
                packet = apocalypse_agent2_subdivide(descriptor, packet, scratch, limit, flags, offsets, count, xport_draft_gte_data_read(24u), points);
                goto next_descriptor;
            }
        }
        if (flags & 0x6000u)
            depth += r_u32(scratch + (flags & 0x2000u ? 0x204u : 0x214u));
        bucket = ordering + (depth < 0x4000u ? depth & 0xFFFCu : 0x3FFCu);
        previous = r_u32(bucket);
        if (flags & 0x20u)
        {
            uint32 end;
            tag += 0x03000000u;
            w_u32(packet, previous | tag);
            end = packet + (tag >> 22) + 4;
            w_u32(bucket, end);
            w_u32(end - 12, 0);
            w_u32(end - 8, 0xE2000000u);
            w_u32(end - 4, 0);
            w_u32(end, chain_head | 0x02000000u);
            w_u32(end + 4, r_u32(descriptor + 32));
            w_u32(end + 8, 0);
            packet = end + 12;
        }
        else
        {
            w_u32(bucket, chain_head);
            w_u32(packet, previous | tag);
            bytes = (tag >> 22) + 4;
            if ((flags & 0x41u) == 0x40u)
            {
                uint32 end = packet + bytes;
                w_u32(end + 4, 0xE1000200u | ((flags & 0x180u) >> 2));
                w_u32(end, packet | 0x01000000u);
                w_u32(bucket, end);
                packet = end + 8;
            }
            else
                packet += bytes;
        }
    next_descriptor:
        descriptor += flags >> 16;
        if (--count)
            continue;
        count = r_u32(scratch + 0x1F4);
        if (!count)
            break;
        descriptor = 0x80082FACu;
        w_u32(scratch + 0x1F4, 0);
    }
    w_u32(0x800FF668u, packet);
    return;
}

uint32 sub_80016800(void)
{
    sint32 result;
    sub_8001A7B0(1);
    sub_8001A7BC(256);
    sub_8001A7D4(149, 20, 20, 0);
    sub_8001AA28(227, 213, r_u32((0x800A5578u + (0) * 4u)), 0, 0x0u, 0x100u);
    result = sub_8006D028(r_u32(0x800FF01Cu));
    if (result)
        return sub_8006D0D4(199, 214, result, r_u32(0x800FF01Cu));
    return result;
}

uint32 sub_80017364(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    sint32 v4;
    sint32 v5;
    sint32 result;
    v4 = (r_u8(0x800EC1D9u) == 0);
    w_u32(a4, ((unsigned char)(r_u8(0x800EC1D9u))));
    if (!v4)
        w_u8(0x800EC1D9u, 0);
    v5 = 0;
    if ((r_u8(0x800EC129u) || r_u32(a4)))
        v5 = 1;
    w_u32(a1, v5);
    if (v5)
        w_u8(0x800EC129u, 0);
    v4 = (r_u8(0x800EC0F9u) == 0);
    w_u32(a2, ((unsigned char)(r_u8(0x800EC0F9u))));
    if (!v4)
        w_u8(0x800EC0F9u, 0);
    w_u32(a3, (r_u32(a2) | ((sint32)(r_u32(a1)))));
    if (r_u8(0x800EC109u))
    {
        w_u8(0x800EC109u, 0);
        w_u32(a3, 1);
    }
    if (r_u8(0x800EC119u))
    {
        w_u8(0x800EC119u, 0);
        w_u32(a3, 1);
    }
    if (r_u8(0x800EC1E9u))
    {
        w_u8(0x800EC1E9u, 0);
        w_u32(a3, 1);
    }
    if (r_u8(0x800EC139u))
    {
        w_u8(0x800EC139u, 0);
        w_u32(a3, 1);
    }
    if (r_u8(0x800EC149u))
    {
        w_u8(0x800EC149u, 0);
        w_u32(a3, 1);
    }
    if (r_u8(0x800EC159u))
    {
        w_u8(0x800EC159u, 0);
        w_u32(a3, 1);
    }
    if (r_u8(0x800EC169u))
    {
        w_u8(0x800EC169u, 0);
        w_u32(a3, 1);
    }
    result = ((sint32)(r_u32(a1)));
    if (((sint32)(r_u32(a1))))
        w_u32(a2, 0);
    return result;
}

uint32 sub_80010938(void)
{
    TestEvent(r_u32(0x800FEED4u));
    TestEvent(r_u32(0x800FEED8u));
    TestEvent(r_u32(0x800FEEDCu));
    return TestEvent(r_u32(0x800FEEE0u));
}

uint32 sub_800166AC(void)
{
    sint32 v0;
    sub_8001A7B0(1);
    sub_8001A7BC(192);
    sub_8001A7D4(149, 20, 20, 0);
    sub_8001AA28(92, 210, r_u32(0x800A5574u), 0, 0, 256);
    sub_8001A7B0(0);
    v0 = sub_8006D028(((uint32)(r_u32(0x800FF01Cu)) + (uint32)(8)));
    if (v0)
        v0 = sub_8006D0D4(64, 211, v0, r_u32(0x800FF01Cu) + 8u);
    sub_8001A7BC(256);
    return v0;
}

uint32 sub_80069BC4(uint32 name)
{
    char filename[24];
    uint32 index = 0u, byte, bank, filesize, sequence, result;
    FUNCTION_MARKER(0x80069BC4u, "SLUS_003.73");
    while ((byte = r_u8(name)) != 0u)
    {
        /* TODO Original filenames must fit the 24-byte addressed local buffer */
        if (index >= sizeof(filename) - 5u)
        {
            fprintf(stderr, "Filename exceeds sub_80069BC4 local buffer\n");
            abort();
        }
        filename[index++] = (char)byte;
        ++name;
    }
    filename[index] = '.';
    filename[index + 1u] = 'V';
    filename[index + 2u] = 'A';
    filename[index + 3u] = 'B';
    filename[index + 4u] = '\0';
    bank = xport_draft_host_sub_80069D2C_p1(filename, 0x800FF6D8u);
    w_u32(0x800FF6A4u, bank);
    filename[index + 1u] = 'S';
    filename[index + 2u] = 'E';
    filename[index + 3u] = 'Q';
    filesize = xport_draft_host_sub_8006B04C_p1(filename);
    sequence = sub_8006B864(filesize, 1u, 1u);
    w_u32(0x800FF6A8u, sequence);
    sub_8006B234(sequence);
    sub_8006B44C();
    result = (uint32)(sint32)(sint16)apocalypse_sequence_open(r_u32(0x800FF6A8u), (uint32)(sint32)(sint16)r_u16(0x800FF6A4u));
    w_u32(0x800FF6ACu, result);
    sequence = r_u32(0x800FF6A8u);
    result = r_u8(sequence + 10u);
    w_u32(0x800FF6C8u, result);
    result += r_u8(sequence + 11u) << 8;
    w_u32(0x800FF6C8u, result);
    byte = r_u8(sequence + 12u) << 16;
    w_u32(0x800FF6C8u, result + byte);
    return byte;
}

extern uint32 sub_80088B28(uint32 environment);

uint32 sub_80016354(uint32 name, uint32 image)
{
    sint16 rectangle[4];
    uint32 index, cursor, words, result;
    sub_80088B28(0x800C636Cu);
    for (index = 0u; index < 4u; ++index)
        rectangle[index] = (sint16)r_u16(0x800FF05Cu + index * 2u);
    xport_draft_host_sub_8008847C_p1(rectangle, 0u, 0u, 0u);
    sub_8001024C(0u, image, 240u);
    for (index = 0u; index < 4u; ++index)
        rectangle[index] = (sint16)r_u16(0x800FF064u + index * 2u);
    xport_draft_host_sub_8008847C_p1(rectangle, 100u, 0u, 0u);
    for (index = 0u; index < 4u; ++index)
        rectangle[index] = (sint16)r_u16(0x800FF06Cu + index * 2u);
    xport_draft_host_sub_8008847C_p1(rectangle, 0u, 0u, 0u);
    cursor = r_u32(0x800FF004u);
    w_u32(0x800FF050u, 1u);
    w_u32(0x800FF058u, 0u);
    for (;;)
    {
        result = r_u8(cursor);
        if (!result)
            break;
        if (sub_80067724(cursor, name))
        {
            while (r_u8(cursor))
                ++cursor;
            words = (cursor + 3u) & 0xFFFFFFFCu;
            result = r_u32(words);
            while (result != 0xFFFFFFFFu)
            {
                w_u32(0x800FF058u, r_u32(0x800FF058u) + r_u32(words));
                words += 4u;
                result = r_u32(words);
            }
            break;
        }
        while (r_u8(cursor))
            ++cursor;
        words = (cursor + 3u) & 0xFFFFFFFCu;
        while (r_u32(words) != 0xFFFFFFFFu)
            words += 4u;
        cursor = words + 4u;
    }
    w_u32(0x800FF054u, 0u);
    return result;
}

/* TODO Missing host buffer adapter xport_draft_host_sub_8001C8E8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006613C_p1 */
uint32 sub_80065DE8(void)
{
    uint32 index = 0u, descriptor, payload, cursor, object, position[3];
    sint32 type;
    FUNCTION_MARKER(0x80065DE8u, "SLUS_003.73");
    while ((sint32)index < (sint32)r_u32(0x800FF628u))
    {
        descriptor = r_u32(r_u32(0x800FF624u) + index * 4u);
        type = (sint16)r_u16(descriptor);
        if (type == 6 || type == 2 || type == 9)
        {
            payload = descriptor + 4u + ((uint32)(sint32)(sint16)r_u16(descriptor + 2u) << 1);
            if (payload & 2u)
                payload += 2u;
            sub_80063B5C(r_u32(payload), index & 0xFFFFu, type == 6 ? payload + 4u : 0x800FF640u);
        }
        else if (type == 1 || type == 7)
        {
            payload = descriptor + 8u + ((uint32)(sint32)(sint16)r_u16(descriptor + 6u) << 1);
            if (sub_80064180(1u, payload) & 0xFFu)
            {
                if (type == 1)
                    sub_800641E8(index);
                else
                {
                    object = sub_8002FED8(24u);
                    if (object)
                        sub_8006FBB0(object, index & 0xFFFFu, 0xFFFFFFFFu);
                }
            }
        }
        else if (type == 5)
        {
            cursor = xport_draft_host_sub_8006613C_p1(position, index);
            if (r_u16(cursor) == 1u)
            {
                object = sub_8002FED8(24u);
                if (object)
                    sub_8006FBB0(object, index & 0xFFFFu, 0xFFFFFFFFu);
            }
        }
        else if (type == 500)
        {
            cursor = xport_draft_host_sub_8006613C_p1(position, index);
            w_u32(0x800FF3ACu, 0u);
            object = sub_80032DC0(100u);
            if (object)
                xport_draft_host_sub_8001C8E8_p2(object, position, index, r_u16(cursor), r_u16(cursor + 2u), r_u16(cursor + 4u), r_u16(cursor + 6u), r_u8(cursor + 8u), r_u8(cursor + 10u), r_u8(cursor + 12u), r_u8(cursor + 14u), r_u8(cursor + 16u), r_u8(cursor + 18u));
            w_u32(0x800FF3ACu, 1u);
        }
        ++index;
    }
    return 0u;
}

uint32 sub_800901EC(void)
{
    uint32 result = (uint32)SpuSetReverb(1);
    w_u32(0x800FD1D0u, result);
    return result;
}

/* TODO Missing call adapter sub_8001F6A0 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8001C8E8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006613C_p1 */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_80064874(uint32 index)
{
    sint32 type = (sint16)r_u16(r_u32(r_u32(0x800FF624u) + index * 4u));
    uint32 record, value, child, position[3];
    uint32 count;
    if (type == 6)
    {
        record = sub_80063C54(index);
        count = r_u8(record + 7u);
        value = r_u32(record);
        w_u8(record + 7u, count + 1u);
        return sub_8006541C(value, index, 0u);
    }
    if (type < 7)
    {
        if (type == 1 || type == 5)
            return sub_800641E8(index);
        return 5u;
    }
    if (type == 10)
    {
        /* TODO Excluded trigger service 8001F6A0 */
        fprintf(stderr, "Missing trigger service 8001F6A0 index=%08X\n", index);
        abort();
    }
    if (type < 11)
    {
        if (type == 7)
            return sub_800641E8(index);
        return 7u;
    }
    if (type != 501)
        return 501u;
    record = xport_draft_host_sub_8006613C_p1(position, index);
    w_u32(0x800FF3ACu, 0u);
    child = sub_80032DC0(100u);
    if (child)
        xport_draft_host_sub_8001C8E8_p2(child, position, index, r_u16(record), r_u16(record + 2u), r_u16(record + 4u), r_u16(record + 6u), r_u8(record + 8u), r_u8(record + 10u), r_u8(record + 12u), r_u8(record + 14u), r_u8(record + 16u), r_u8(record + 18u));
    w_u32(0x800FF3ACu, 1u);
    return 1u;
}

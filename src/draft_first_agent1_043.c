#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Existing named missing renderer boundaries */
uint32 sub_8007FA7C(uint32 geometry);
uint32 sub_8007FD3C(uint32 geometry);
void sub_8008128C(uint32 count);

static void missing_80076310_native(uint32 quaternion, MATRIX *matrix)
{
    uint32 w = r_u32(quaternion + 12u), x = r_u32(quaternion);
    uint32 wx = w * x, y = r_u32(quaternion + 4u);
    uint32 wy = w * y, z = r_u32(quaternion + 8u);
    uint32 wz = w * z, xx = x * x, xy = x * y, yy = y * y, zz = z * z;
    uint32 xz = x * z, yz;
    uint32 xx_shift, xy_shift, yy_shift, zz_shift, wx_shift, wy_shift, wz_shift, xz_shift, yz_shift;
    matrix->t[0] = 0;
    matrix->t[1] = 0;
    matrix->t[2] = 0;
    wz_shift = (uint32)((sint32)wz >> 11);
    yy_shift = (uint32)((sint32)yy >> 11);
    zz_shift = (uint32)((sint32)zz >> 11);
    xy_shift = (uint32)((sint32)xy >> 11);
    matrix->m[0][0] = (sint16)(4096u - (yy_shift + zz_shift));
    matrix->m[0][1] = (sint16)(xy_shift + wz_shift);
    matrix->m[1][0] = (sint16)(xy_shift - wz_shift);
    wy_shift = (uint32)((sint32)wy >> 11);
    xz_shift = (uint32)((sint32)xz >> 11);
    xx_shift = (uint32)((sint32)xx >> 11);
    matrix->m[1][1] = (sint16)(4096u - (xx_shift + zz_shift));
    wx_shift = (uint32)((sint32)wx >> 11);
    matrix->m[0][2] = (sint16)(xz_shift - wy_shift);
    matrix->m[2][0] = (sint16)(xz_shift + wy_shift);
    matrix->m[2][2] = (sint16)(4096u - (xx_shift + yy_shift));
    yz = y * z;
    yz_shift = (uint32)((sint32)yz >> 11);
    matrix->m[1][2] = (sint16)(yz_shift + wx_shift);
    matrix->m[2][1] = (sint16)(yz_shift - wx_shift);
}

static void missing_800875DC_native(MATRIX *matrix, const uint32 input[3], uint32 output[3])
{
    uint32 high[3], low[3], high_mac[3], low_mac[3], packed[5];
    uint32 axis, value, magnitude;
    for (axis = 0u; axis < 4u; ++axis)
    {
        uint32 first = axis * 2u, second = first + 1u;
        packed[axis] = (uint16)matrix->m[first / 3u][first % 3u] | ((uint32)(uint16)matrix->m[second / 3u][second % 3u] << 16);
    }
    packed[4] = (uint16)matrix->m[2][2];
    for (axis = 0u; axis < 5u; ++axis)
        xport_draft_gte_control_write(axis, packed[axis]);
    for (axis = 0u; axis < 3u; ++axis)
    {
        value = input[axis];
        if ((sint32)value < 0)
        {
            magnitude = 0u - value;
            high[axis] = 0u - (uint32)((sint32)magnitude >> 15);
            low[axis] = 0u - (magnitude & 0x7FFFu);
        }
        else
        {
            high[axis] = (uint32)((sint32)value >> 15);
            low[axis] = value & 0x7FFFu;
        }
    }
    for (axis = 0u; axis < 3u; ++axis)
        xport_draft_gte_data_write(9u + axis, high[axis]);
    xport_draft_gte_execute(0x41E012u);
    for (axis = 0u; axis < 3u; ++axis)
        high_mac[axis] = xport_draft_gte_data_read(25u + axis);
    for (axis = 0u; axis < 3u; ++axis)
        xport_draft_gte_data_write(9u + axis, low[axis]);
    xport_draft_gte_execute(0x49E012u);
    for (axis = 0u; axis < 3u; ++axis)
        low_mac[axis] = xport_draft_gte_data_read(25u + axis);
    for (axis = 0u; axis < 3u; ++axis)
        output[axis] = low_mac[axis] + (high_mac[axis] << 3);
}

/* TODO Connect the native quaternion interpolation boundary */
static uint32 missing_80076800_native(const uint32 from[4], const uint32 to[4], uint32 amount, uint32 output)
{
    fprintf(stderr, "Missing native adapter 80076800 quaternion16 interpolation\n");
    abort();
    return 0u;
}

uint32 sub_80077274(sint16 first, sint16 second);

static uint32 camera_77748_collision_angle(uint32 record[35], sint16 strength, uint32 reverse)
{
    uint32 numerator, ratio, product;
    sint32 divisor = (sint32)record[17];
    if (!divisor)
        record[17] = 1u;
    divisor = (sint32)record[17];
    numerator = (record[17] - record[16]) << 12;
    if (numerator == 0x80000000u && divisor == -1)
        ratio = numerator;
    else
        ratio = (uint32)((sint32)numerator / divisor);
    product = ratio * ratio;
    product = (uint32)(sint32)strength * (uint32)((sint32)product >> 12);
    if (reverse)
        return (uint32)(sint32)(sint16)(0u - (uint32)((sint32)product >> 12));
    return (uint32)((sint32)(product << 4) >> 16);
}

static void camera_77748_apply_quaternion(uint32 object, const uint32 q[4])
{
    uint32 x = r_u32(object + 444u), y = r_u32(object + 448u);
    uint32 z = r_u32(object + 452u), w = r_u32(object + 456u);
    uint32 out_x = w * q[0] + x * q[3] + y * q[2] - z * q[1];
    uint32 out_y = w * q[1] + y * q[3] + z * q[0] - x * q[2];
    uint32 out_z = w * q[2] + z * q[3] + x * q[1] - y * q[0];
    uint32 out_w = w * q[3] - x * q[0] - y * q[1] - z * q[2];
    w_u32(object + 444u, (uint32)((sint32)out_x >> 12));
    w_u32(object + 448u, (uint32)((sint32)out_y >> 12));
    w_u32(object + 452u, (uint32)((sint32)out_z >> 12));
    w_u32(object + 456u, (uint32)((sint32)out_w >> 12));
}

uint32 sub_80077748(uint32 object)
{
    MATRIX matrix;
    uint32 previous[3], input[3], transformed[3], lateral[3], opposite[3], forward[3];
    uint32 target[3], difference[3], collision[35];
    uint32 q[4], rotated[4], yaw_q[4], pitch_q[4], combined[4], old_q[4], desired_q[4];
    sint16 first_angles[4], second_angles[4];
    sint32 distance;
    uint32 result, angle, index;
    previous[0] = r_u32(object + 4u);
    previous[1] = r_u32(object + 8u);
    previous[2] = r_u32(object + 12u);
    if (!r_u32(0x800FF008u))
    {
        for (index = 0u; index < 4u; ++index)
            q[index] = r_u32(object + 428u + 4u * index);
        for (index = 0u; index < 4u; ++index)
            w_u32(object + 460u + 4u * index, q[index]);
        input[0] = 0u;
        input[1] = 0u;
        input[2] = 0u - (r_u32(0x800FF8FCu) << 12);
        missing_80076310_native(object + 444u, &matrix);
        missing_800875DC_native(&matrix, input, transformed);
        distance = (sint32)r_u32(object + 540u);
        /* Invalid negative distances select the default camera distance */
        if (distance < -1)
            distance = -1;
        if (distance == -1)
        {
            forward[0] = transformed[0];
            forward[1] = 0u;
            forward[2] = transformed[2];
        }
        else if (distance > 0)
        {
            input[0] = 0u;
            input[1] = 0u;
            input[2] = 0u - ((uint32)distance << 12);
            missing_800875DC_native(&matrix, input, forward);
        }
        distance = (sint32)r_u32(object + 536u);
        /* Invalid negative distances select the default camera distance */
        if (distance < -1)
            distance = -1;
        if (distance == -1 || distance > 0)
        {
            input[0] = 0u - ((distance == -1 ? r_u32(0x800FF8FCu) : (uint32)distance) << 12);
            input[1] = 0u;
            input[2] = 0u;
            missing_800875DC_native(&matrix, input, lateral);
            opposite[0] = 0u - lateral[0];
            opposite[1] = 0u - lateral[1];
            opposite[2] = 0u - lateral[2];
        }
        target[0] = r_u32(object + 308u) + transformed[0];
        target[1] = r_u32(object + 312u) + transformed[1];
        target[2] = r_u32(object + 316u) + transformed[2];
        w_u32(0x801028C8u, target[0]);
        w_u32(0x801028CCu, target[1]);
        w_u32(0x801028D0u, target[2]);
        xport_draft_host_sub_8006C3AC_p123(difference, target, previous);
        if (r_u32(object + 536u))
        {
            for (index = 0u; index < 2u; ++index)
            {
                uint32 *offset = index ? opposite : lateral;
                collision[0] = r_u32(object + 308u);
                collision[1] = r_u32(object + 312u);
                collision[2] = r_u32(object + 316u);
                collision[3] = r_u32(object + 308u) + offset[0];
                collision[4] = r_u32(object + 312u);
                collision[5] = r_u32(object + 316u) + offset[2];
                apocalypse_collision_init(collision);
                ((uint8 *)collision)[136] = 0u;
                w_u32(0x800FF96Cu, 1u);
                apocalypse_collision_query(collision, 1u);
                w_u32(0x800FF96Cu, 0u);
                if (collision[26])
                {
                    angle = camera_77748_collision_angle(collision, (sint16)r_u16(object + 544u), index);
                    xport_draft_host_sub_800762A8_p1(q, angle);
                    camera_77748_apply_quaternion(object, q);
                }
            }
        }
        if (r_u32(object + 540u))
        {
            collision[0] = r_u32(object + 308u);
            collision[1] = r_u32(object + 312u);
            collision[2] = r_u32(object + 316u);
            collision[3] = r_u32(object + 308u) + (forward[0] << 1);
            collision[4] = r_u32(object + 312u);
            collision[5] = r_u32(object + 316u) + (forward[2] << 1);
            apocalypse_collision_init(collision);
            ((uint8 *)collision)[136] = 0u;
            w_u32(0x800FF96Cu, 1u);
            apocalypse_collision_query(collision, 1u);
            w_u32(0x800FF96Cu, 0u);
            if (collision[26])
            {
                angle = camera_77748_collision_angle(collision, (sint16)r_u16(object + 546u), 0u);
                xport_draft_host_sub_80076274_p1(q, angle);
                xport_draft_host_sub_80075F80_p123(rotated, psx_addr(object + 444u, 16u), q);
                for (index = 0u; index < 4u; ++index)
                    w_u32(object + 444u + 4u * index, rotated[index]);
            }
        }
    }
    sub_80076800(object + 460u, object + 444u, 1023u, object + 428u);
    input[0] = 0u;
    input[1] = 0u;
    input[2] = 0u - (r_u32(0x800FF8FCu) << 12);
    missing_80076310_native(object + 428u, &matrix);
    missing_800875DC_native(&matrix, input, transformed);
    w_u32(object + 4u, r_u32(object + 308u) + transformed[0]);
    w_u32(object + 8u, r_u32(object + 312u) + transformed[1]);
    w_u32(object + 12u, r_u32(object + 316u) + transformed[2]);
    xport_draft_host_sub_8006C3AC_p123(difference, psx_addr(object + 4u, 12u), previous);
    result = r_u32(0x800FF008u);
    w_u32(object + 104u, difference[0]);
    w_u32(object + 108u, difference[1]);
    w_u32(object + 112u, difference[2]);
    if (!result)
    {
        result = r_u32(object + 336u);
        if (r_u32(object + 296u) != result)
        {
            apocalypse_rotation_between(first_angles, psx_addr(object + 308u, 12u), target);
            apocalypse_rotation_between(second_angles, psx_addr(r_u32(0x800FF4ECu) + 4u, 12u), target);
            angle = sub_80077274(first_angles[1], second_angles[1]);
            desired_q[0] = desired_q[1] = desired_q[2] = 0u;
            desired_q[3] = 4096u;
            for (index = 0u; index < 4u; ++index)
                q[index] = r_u32(object + 428u + 4u * index);
            for (index = 0u; index < 4u; ++index)
                w_u32(object + 476u + 4u * index, q[index]);
            apocalypse_rotation_between(first_angles, psx_addr(object + 4u, 12u), psx_addr(object + 308u, 12u));
            apocalypse_rotation_between(second_angles, psx_addr(object + 4u, 12u), psx_addr(r_u32(0x800FF4ECu) + 4u, 12u));
            xport_draft_host_sub_800762A8_p1(pitch_q, (uint32)(sint32)(sint16)(4096u - angle));
            xport_draft_host_sub_80075F80_p123(combined, pitch_q, psx_addr(object + 428u, 16u));
            xport_draft_host_sub_80076274_p1(yaw_q, ((uint32)(uint16)second_angles[0] - (uint32)(uint16)first_angles[0]) & 0xFFFu);
            xport_draft_host_sub_80075F80_p123(rotated, combined, yaw_q);
            for (index = 0u; index < 4u; ++index)
                desired_q[index] = rotated[index];
            for (index = 0u; index < 4u; ++index)
                old_q[index] = r_u32(object + 428u + 4u * index);
            return missing_80076800_native(old_q, desired_q, 1023u, object + 428u);
        }
    }
    return result;
}

void sub_80082750(uint32 count, uint32 corner0, uint32 corner1, uint32 corner2, uint32 corner3)
{
    uint32 position = 0x1F800000u;
    sub_800826C4(position, count, 16u, corner0, corner1);
    position += count * 16u;
    sub_800826C4(position, count, 128u, corner1, corner3);
    position += count * 128u;
    sub_800826C4(position, count, 0xFFFFFFF0u, corner3, corner2);
    position -= count * 16u;
    sub_800826C4(position, count, 0xFFFFFF80u, corner2, corner0);
}

static uint32 weapon_light_2349C_native(uint32 receiver, uint8 *red, uint8 *green, uint8 *blue)
{
    if (!r_u32(receiver + 84u))
        return 0u;
    *red = 255u;
    *green = 0u;
    *blue = 0u;
    w_u32(receiver + 84u, 0u);
    return 1u;
}

static uint32 missing_weapon_light_colors(uint32 weapon, uint8 *red, uint8 *green, uint8 *blue)
{
    uint32 table = r_u32(weapon);
    uint32 receiver = weapon + (uint32)(sint32)(sint16)r_u16(table + 32u);
    uint32 target = r_u32(table + 36u);
    if (target == 0x8002349Cu)
        return weapon_light_2349C_native(receiver, red, green, blue);
    /* TODO Supply other observed slot-36 native three-byte output callbacks */
    fprintf(stderr, "Weapon light callback missing target=%08X receiver=%08X red=%p green=%p blue=%p\n", target, receiver, (void *)red, (void *)green, (void *)blue);
    abort();
}

uint32 sub_8005CF0C(uint32 a1)
{
    uint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    signed int v6;
    sint32 v7;
    uint32 v8;
    short v9;
    sint32 v10;
    uint32 v11;
    short v12;
    short v13;
    sint32 result;
    uint32 v15[4];

    union
    {
        uint32 words[4];
        sint16 halves[8];
    } vector;

    unsigned char v17;
    unsigned char v18;
    char v19[2];
    sint32 v20;
    w_u16(0x800FFBB4u, 0);
    w_u16(0x800FFBB6u, -4096);
    w_u16(0x800FFBB8u, 0);
    w_u16(0x800FFBBCu, 0);
    w_u16(0x800FFBBEu, 4096);
    w_u16(0x800FFBC0u, 0);
    w_u16(0x800FFBC4u, 0);
    w_u16(0x800FFBC6u, 0);
    w_u16(0x800FFBC8u, 0);
    w_u16(0x800FFBD0u, 4096);
    w_u16(0x800FFBCEu, 4096);
    w_u16(0x800FFBCCu, 4096);
    v2 = r_u32(((uint32)((a1 + 32))));
    v3 = ((unsigned char)(v2));
    v4 = ((v2 >> 8) & 255u);
    v5 = ((v2 >> 16) & 255u);
    v6 = (v2 >> 24);
    if ((((uint32)((v6 - 1))) < 0x7F))
    {
        v3 = ((v3 * (0x80000 / v6)) >> 12);
        v4 = ((v4 * (0x80000 / v6)) >> 12);
        v5 = ((v5 * (0x80000 / v6)) >> 12);
    }
    w_u16(0x800FFBD4u, 16u * (uint32)v3);
    w_u16(0x800FFBD6u, 16u * (uint32)v4);
    w_u16(0x800FFBD8u, 16u * (uint32)v5);
    w_u32(0x800FFBDCu, r_u32(0x800FFBD4u));
    w_u32(0x800FFBE0u, r_u32(0x800FFBD8u));
    v7 = r_u32(((uint32)(((a1 + (4 * r_u32(((uint32)((a1 + 644)))))) + 612))));
    if (missing_weapon_light_colors((uint32)v7, &v17, &v18, (uint8 *)v19))
    {
        xport_draft_host_sub_8006C3AC_p1(vector.words, a1 + 416u, a1 + 4u);
        v20 = 12;
        xport_draft_host_sub_8006C564_p123(v15, vector.words, &v20);
        xport_draft_host_sub_800666DC_p12(vector.words, v15);
        w_u16(0x800FFBCCu, (16 * v17));
        w_u16(0x800FFBB4u, vector.halves[0]);
        w_u16(0x800FFBB6u, 0);
        w_u16(0x800FFBB8u, vector.halves[4]);
        w_u16(0x800FFBCEu, (16 * v18));
        w_u16(0x800FFBD0u, (16 * ((unsigned char)(v19[0]))));
    }
    else
    {
        w_u16(0x800FFBB8u, 0);
        w_u16(0x800FFBB4u, 0);
        w_u16(0x800FFBB6u, -4096);
        v8 = r_u8(((uint32)((a1 + 35))));
        v9 = (16 * v8);
        if ((v8 < 0x40))
            v9 = 1024;
        w_u16(0x800FFBD0u, v9);
        w_u16(0x800FFBCEu, v9);
        w_u16(0x800FFBCCu, v9);
    }
    if (r_u32(0x800FF904u))
        v10 = (r_u16(((uint32)((r_u32(0x800FF904u) + 494)))) & 0xFFF);
    else
        v10 = 0;
    v11 = (0x800F863Cu + (v10) * 4u);
    v12 = r_u16(((uint32)(v11)));
    w_u16(0x800FFBC6u, 0);
    w_u16(0x800FFBC4u, v12);
    v13 = r_u16((((uint32)(v11)) + (1) * 2u));
    w_u16(0x800EE6F0u, r_u16(0x800FFBB4u));
    w_u16(0x800EE6F4u, r_u16(0x800FFBB8u));
    w_u16(0x800EE6F6u, r_u16(0x800FFBBCu));
    w_u16(0x800EE6F8u, r_u16(0x800FFBBEu));
    w_u16(0x800EE6FAu, r_u16(0x800FFBC0u));
    w_u16(0x800EE6FCu, v12);
    w_u16(0x800EE6FEu, 0);
    w_u16(0x800EE6F2u, r_u16(0x800FFBB6u));
    w_u16(0x800EE710u, r_u16(0x800FFBCCu));
    w_u16(0x800EE714u, r_u16(0x800FFBDCu));
    w_u16(0x800EE716u, r_u16(0x800FFBCEu));
    w_u16(0x800EE718u, r_u16(0x800FFBD6u));
    w_u16(0x800EE71Au, r_u16(0x800FFBDEu));
    w_u16(0x800EE712u, r_u16(0x800FFBD4u));
    w_u16(0x800FFBC8u, v13);
    w_u16(0x800EE700u, v13);
    w_u16(0x800EE71Cu, r_u16(0x800FFBD0u));
    result = 64;
    w_u16(0x800FFA98u, 64);
    w_u16(0x800FFA96u, 64);
    w_u16(0x800FFA94u, 64);
    w_u16(0x800EE71Eu, r_u32(0x800FFBD8u));
    w_u16(0x800EE720u, r_u32(0x800FFBE0u));
    return result;
}

uint32 sub_8002349C(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    sint32 result;
    result = 1;
    if (!(r_u32(((uint32)((a1 + 84))))))
        return 0;
    w_u8(a2, -1);
    w_u8(a3, 0);
    w_u8(a4, 0);
    w_u32(((uint32)((a1 + 84))), 0);
    return result;
}

/* TODO Missing call adapter sub_8008128C */
void sub_8007FB34(uint32 geometry)
{
    uint32 count = r_u32(geometry + 4u), vertices = geometry + 32u;
    uint32 flags, result, faces;
    w_u32(0x800FFA9Cu, 1u);
    flags = sub_80080EF4(vertices, count);
    /* TODO Existing missing renderer boundary 8128C retains its real count argument */
    if (r_u32(0x800FFB3Cu))
        sub_8008128C(count);
    result = count << 3;
    faces = vertices + result;
    if (!r_u32(0x800FFB38u))
    {
        result = flags & 0xBFu;
        if (!result)
        {
            (void)(sub_800817FC(faces + (r_u32(geometry + 8u) << 3), faces, r_u32(geometry + 12u)));
            return;
        }
    }
    return;
}

void sub_80085C34(uint32 a1, uint32 a2)
{
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    short v6;
    v2 = r_u32(a1);
    v3 = r_u32((a1 + (1) * 4u));
    v4 = r_u32((a1 + (2) * 4u));
    v5 = r_u32((a1 + (3) * 4u));
    v6 = r_u16((((uint32)(a1)) + (8) * 2u));
    w_u32(((uint32)(a2)), (((unsigned short)((r_u32(a1) ^ v3))) ^ v3));
    w_u32(((uint32)((a2 + 4))), (((unsigned short)((v5 ^ v2))) ^ v2));
    w_u32(((uint32)((a2 + 8))), (((unsigned short)((v4 ^ v5))) ^ v5));
    w_u32(((uint32)((a2 + 12))), (((unsigned short)((v3 ^ v4))) ^ v4));
    w_u16(((uint32)((a2 + 16))), v6);
}

/* TODO Missing call adapter sub_8007FA7C */
/* TODO Missing call adapter sub_8007FD3C */
/* TODO Missing call adapter sub_800878DC */
void sub_8007F138(uint32 object)
{
    uint32 entry, flags, color, render_flags, geometry;
    MATRIX rotation, combined;
    if (!object)
        return;
    w_u32(0x1F800164u, 0x7FFFFFFFu);
    do
    {
        flags = r_u16(object);
        entry = 0x800EAEF8u + 64u * r_u8(object + 27u);
        if (!(flags & 0x8001u) && r_u8(entry + 10u))
        {
            render_flags = r_u32(0x800FFAA0u);
            if (flags & 0x400u)
            {
                render_flags = (render_flags & 0xFFFB0000u) | 8u;
                color = r_u32(object + 32u);
                w_u32(0x800FFB20u, color & 0xFFFFFFu);
                w_u32(0x1F800154u, color & 0xFFFFFFu);
                w_u32(0x1F8001A4u, (color & 255u) << 6);
                w_u32(0x1F8001B4u, (color & 0xFF00u) >> 2);
                w_u32(0x1F8001C4u, ((color & 0xFFFFFFu) >> 10) & 0x3FC0u);
            }
            if (r_u16(object) & 0x800u)
                render_flags = (render_flags & 0xFFFFFE7Fu) | (r_u32(object + 44u) & 0x180u) | 0x40u;
            w_u32(0x1F800144u, render_flags);
            SetRotMatrix((MATRIX *)psx_addr(r_u32(0x800FFB0Cu) + 116u, 20u));
            if (r_u32(object + 16u) || r_u16(object + 20u))
            {
                (void)xport_draft_host_sub_800858FC_p2(object + 16u, &rotation);
                (void)xport_draft_host_sub_800854F4_p12(&rotation, &combined);
                SetRotMatrix(&combined);
            }
            else
                (void)xport_draft_host_sub_800854D8_p1(&rotation);
            entry = 0x800EAEF8u + 64u * r_u8(object + 27u);
            w_u32(0x800FFB04u, r_u32(entry + 32u));
            geometry = r_u32(r_u32(entry + 16u) + 4u * r_u16(object + 22u));
            /* TODO Existing missing renderer dependencies retain their geometry inputs */
            if (r_u32(object + 16u) || r_u16(object + 20u))
                sub_8007FA7C(geometry);
            else
                sub_8007FD3C(geometry);
        }
        object = r_u32(object + 28u);
    } while (object);
    geometry = r_u32(0x800FFACCu);
    w_u32(0x1F800164u, geometry);
    return;
}

uint32 sub_8001C004(void)
{
    sint32 result;
    sint32 v1;
    sint32 v2;
    uint32 v3;
    sint32 v4;
    sint8 v5;
    sint8 v6;
    sint8 v7;
    sint32 v8;
    sint32 v9;
    if ((r_u32(0x800FF1E4u) || (result = r_u32(0x800FF1E8u) != 0)))
    {
        v1 = r_u32(0x800FF668u);
        v2 = ((r_u32(0x800FF660u) + (4 * r_u32(0x800FF1E0u))) + 112);
        v3 = ((uint32)((v2 + 4)));
        result = (r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 32))));
        v4 = (r_u32(0x800FF668u) + 24);
        if ((r_u32(0x800FF374u) >= ((uint32)((r_u32(0x800FF668u) + 32)))))
        {
            w_u32(0x800FF668u, (r_u32(0x800FF668u) + (32)));
            w_u8(((uint32)((v1 + 3))), 5);
            w_u8(((uint32)((v1 + 7))), 42);
            v5 = ((r_u32(0x800FFB60u) >> 16) & 255u);
            w_u16(((uint32)((v1 + 8))), 0);
            w_u16(((uint32)((v1 + 10))), 0);
            w_u16(((uint32)((v1 + 14))), 0);
            w_u16(((uint32)((v1 + 16))), 0);
            w_u8(((uint32)((v1 + 4))), v5);
            v6 = ((r_u32(0x800FFB64u) >> 16) & 255u);
            w_u16(((uint32)((v1 + 12))), 512);
            w_u16(((uint32)((v1 + 20))), 512);
            w_u8(((uint32)((v1 + 5))), v6);
            v7 = ((r_u32(0x800FFB68u) >> 16) & 255u);
            w_u16(((uint32)((v1 + 18))), 240);
            w_u16(((uint32)((v1 + 22))), 240);
            w_u8(((uint32)((v1 + 6))), v7);
            w_u32(((uint32)(v1)), ((r_u32(((uint32)(v1))) & 0xFF000000) | (r_u32(((uint32)((v2 + 4)))) & 0xFFFFFF)));
            v8 = r_u32(0x800FF1E8u);
            w_u32(((uint32)((v2 + 4))), ((r_u32(((uint32)((v2 + 4)))) & 0xFF000000) | (v1 & 0xFFFFFF)));
            w_u32(((uint32)((v1 + 24))), 0x1000000);
            if (v8)
                v9 = -520092096;
            else
                v9 = -520092128;
            w_u32(((uint32)((v4 + 4))), v9);
            w_u32(((uint32)(v4)), ((r_u32(((uint32)(v4))) & 0xFF000000) | (r_u32(v3) & 0xFFFFFF)));
            result = ((r_u32(v3) & 0xFF000000) | (v4 & 0xFFFFFF));
            w_u32(v3, result);
        }
    }
    return result;
}

/* TODO Missing call adapter sub_8009C858 */
/* TODO Missing call adapter sub_8009CB8C */
/* TODO Missing call adapter sub_8009CC0C */
uint32 sub_8007011C(uint32 controller, uint32 value, uint32 actuator, uint32 enabled)
{
    uint32 result = r_u32(0x800FF818u), index, port, owner, state;
    if (result)
        return result;
    index = controller & 255u;
    port = index ? 16u : 0u;
    state = (uint32)PadGetState((sint32)port);
    if (!state)
        return state;
    owner = 0x800EC0F8u + 380u * index;
    if (state == 1u)
        w_u8(owner + 375u, 0u);
    if (!r_u8(owner + 375u))
    {
        PadSetActPSX(port, owner + 372u, 2u);
        if (state == 2u || (state == 6u && PadSetActAlignPSX(port, 0x800FF824u)))
            w_u8(owner + 375u, 1u);
    }
    result = 0x800EC270u + 380u * index;
    actuator &= 255u;
    w_u8(0x800EC26Cu + 380u * index + actuator, enabled);
    w_u16(result + 2u * actuator, value);
    return result;
}

uint32 sub_8005ED30(uint32 a1)
{
    sint32 result;
    short v3;
    sint32 v4;
    sint32 v5;
    short v6;
    short v7;
    sint32 v8;
    uint32 v9;
    sint32 v10;
    sint32 v11;
    short v12;
    sint32 v13;
    short v14;
    sint32 v15;
    sint32 v16;
    if (!(r_u8(((uint32)((r_u32((((uint32)(a1)) + (111) * 4u)) + 256))))))
        return 0;
    if ((r_u32((((uint32)(a1)) + (115) * 4u)) == 256))
    {
        result = 0;
        if (!((r_u8((((uint32)(a1)) + (468) * 1u)) | r_u8((((uint32)(a1)) + (469) * 1u)))))
            return result;
        v3 = r_u16((a1 + (237) * 2u));
        v4 = r_u32(0x800FF904u);
        v5 = (r_u32(0x800FF904u) == 0);
        w_u16((a1 + (303) * 2u), v3);
        if (v5)
            v6 = (v3 & 0xFFF);
        else
            v6 = ((r_u16(((uint32)((v4 + 494)))) + v3) & 0xFFF);
        w_u16((a1 + (9) * 2u), v6);
        v7 = r_u16(a1);
        w_u32((((uint32)(a1)) + (115) * 4u), 2048);
        v8 = 0;
        if (((v7 & 8) != 0))
            v8 = 55;
        sub_80069DF0(v8, 0x2000, 0);
        v9 = a1;
        v10 = 12;
        v11 = 6;
        goto LABEL_21;
    }
    if (((r_u32((((uint32)(a1)) + (115) * 4u)) & 0x630) == 0))
    {
        v16 = (r_u16(a1) & 8);
        w_u32((((uint32)(a1)) + (115) * 4u), 256);
        if (v16)
            sub_80069DF0(72, 0x2000, 0);
        v9 = a1;
        v10 = 5;
        v11 = 0;
    LABEL_21:
        sub_80063038(v9, v10, v11, -1);

        return 1;
    }
    result = 0;
    if ((r_u8((((uint32)(a1)) + (468) * 1u)) | r_u8((((uint32)(a1)) + (469) * 1u))))
    {
        v12 = r_u16((a1 + (237) * 2u));
        v13 = r_u32(0x800FF904u);
        v5 = (r_u32(0x800FF904u) == 0);
        w_u16((a1 + (303) * 2u), v12);
        if (v5)
            v14 = (v12 & 0xFFF);
        else
            v14 = ((r_u16(((uint32)((v13 + 494)))) + v12) & 0xFFF);
        w_u16((a1 + (9) * 2u), v14);
        w_u32((((uint32)(a1)) + (115) * 4u), 2048);
        sub_80063038(a1, 12, 0, -1);
        v15 = 0;
        if (((r_u16(a1) & 8) != 0))
            v15 = 55;
        sub_80069DF0(v15, 0x2000, 0);
        return 1;
    }
    return result;
}

uint32 sub_8005ED28(void)
{
    return 0;
}

uint32 sub_8006C47C(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 result;
    sint32 v4;
    sint32 v5;
    result = a1;
    v4 = (r_u32((a3 + (1) * 4u)) * r_u32(a2));
    v5 = (r_u32((a3 + (2) * 4u)) * r_u32(a2));
    w_u32(a1, (r_u32(a3) * r_u32(a2)));
    w_u32((a1 + (1) * 4u), v4);
    w_u32((a1 + (2) * 4u), v5);
    return result;
}

uint32 sub_80030764(void)
{
    uint32 result;
    sint32 v1;
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    result = ((uint32)(r_u32(0x800FF444u)));
    if (r_u32(0x800FF444u))
    {
        do
        {
            v1 = r_u32((result + (4) * 4u));
            v2 = r_u32((result + (5) * 4u));
            w_u32((result + (6) * 4u), r_u32((result + (3) * 4u)));
            w_u32((result + (7) * 4u), v1);
            w_u32((result + (8) * 4u), v2);
            v3 = r_u32((result + (28) * 4u));
            v4 = r_u32((result + (29) * 4u));
            w_u32((result + (18) * 4u), r_u32((result + (27) * 4u)));
            w_u32((result + (19) * 4u), v3);
            w_u32((result + (20) * 4u), v4);
            v5 = r_u32((result + (31) * 4u));
            v6 = r_u32((result + (32) * 4u));
            w_u32((result + (21) * 4u), r_u32((result + (30) * 4u)));
            w_u32((result + (22) * 4u), v5);
            w_u32((result + (23) * 4u), v6);
            v7 = r_u32((result + (34) * 4u));
            v8 = r_u32((result + (35) * 4u));
            w_u32((result + (24) * 4u), r_u32((result + (33) * 4u)));
            w_u32((result + (25) * 4u), v7);
            w_u32((result + (26) * 4u), v8);
            result = ((uint32)(r_u32((result + (1) * 4u))));
        } while (result);
    }
    return result;
}

uint32 sub_80070288(uint32 a1, uint32 a2)
{
    uint32 result;
    result = ((uint32)(r_u32(0x800FF818u)));
    if (!r_u32(0x800FF818u))
    {
        result = (((uint32)((0x800EC270u + ((95 * a1)) * 4u))) + (a2) * 2u);
        w_u8((0x800EC26Cu + (((380 * a1) + a2)) * 1u), 0);
        w_u16(result, 0);
    }
    return result;
}

uint32 sub_800350E8(uint32 a1)
{
    sint32 result;
    result = ((a1 << 16) | a1);
    w_u32(0x800FF424u, result);
    return result;
}

uint32 sub_80034EF4(uint32 a1)
{
    sub_80032F7C(((sint32)(a1)));
    w_u32((a1 + (17) * 4u), 0x800A1CA8u);
    sub_80032E50(a1, 0x800FF440u);
    return a1;
}

uint32 sub_800667CC(uint32 output, uint32 scale, uint32 angles)
{
    return xport_draft_host_sub_800667CC_p3(output, scale, psx_addr(angles, 4u));
}

uint32 sub_800352B0(uint32 object)
{
    uint32 px = r_u32(object + 24u) + r_u32(object + 36u);
    uint32 py = r_u32(object + 28u) + r_u32(object + 40u);
    uint32 vx = r_u32(object + 36u);
    uint32 pz = r_u32(object + 32u) + r_u32(object + 44u);
    uint32 ax = r_u32(object + 48u);
    uint32 vy, ay, vz, az, age, lifetime, fade_y, fade_z, result;
    w_u32(object + 24u, px);
    vx += ax;
    vy = r_u32(object + 40u);
    ay = r_u32(object + 52u);
    vz = r_u32(object + 44u);
    az = r_u32(object + 56u);
    w_u32(object + 28u, py);
    w_u32(object + 32u, pz);
    vy += ay;
    vz += az;
    age = r_u16(object + 8u);
    lifetime = r_u16(object + 10u);
    w_u32(object + 36u, vx - (uint32)((sint32)vx >> 3));
    w_u32(object + 40u, vy - (uint32)((sint32)vy >> 3));
    w_u32(object + 44u, vz - (uint32)((sint32)vz >> 3));
    age = (age + 1u) & 0xFFFFu;
    w_u16(object + 8u, age);
    if ((sint32)(sint16)age >= (sint32)lifetime)
        sub_80032ED8(object);
    fade_y = r_u8(object + 85u);
    fade_z = r_u8(object + 86u);
    w_u8(object + 76u, r_u8(object + 76u) - r_u8(object + 84u));
    result = r_u8(object + 77u) - fade_y;
    w_u8(object + 77u, result);
    w_u8(object + 78u, r_u8(object + 78u) - fade_z);
    return result;
}

uint32 sub_80032ED8(uint32 a1)
{
    sint32 result;
    result = 1;
    w_u8(((uint32)((a1 + 63))), 1);
    return result;
}

uint32 sub_80034F38(uint32 a1, uint32 a2)
{
    uint32 result;
    sint32 v5;
    w_u32((a1 + (17) * 4u), 0x800A1CA8u);
    sub_80032E7C(a1, 0x800FF440u);
    result = sub_80032FB8(((sint32)(a1)), 0);
    if (((a2 & 1) != 0))
        return ((uint32)(sub_80032E30(((sint32)(a1)))));
    return result;
}

uint32 sub_80032FB8(uint32 a1, uint32 a2)
{
    uint32 result;
    sint32 v3;
    sint32 v4;
    result = 0x800A1DB8u;
    v3 = r_u32(0x800FF3A8u);
    v4 = (a2 & 1);
    w_u32(((uint32)((a1 + 68))), 0x800A1DB8u);
    w_u32(0x800FF3A8u, (v3 - 1));
    if (v4)
        return ((uint32)(sub_80032E30(a1)));
    return result;
}

uint32 sub_80032E30(uint32 a1)
{
    return sub_8006BC20(a1);
}

uint32 sub_8004B948(uint32 a1)
{
    sint32 result;
    sint32 v2;
    result = 0xFFFF;
    if (!(r_u8(((uint32)((a1 + 385))))))
    {
        v2 = r_u16(((uint32)((a1 + 214))));
        result = 1;
        if ((v2 != 0xFFFF))
        {
            w_u8(((uint32)((a1 + 385))), 1);
            return sub_80064A08((r_u32(((uint32)(((4 * v2) + r_u32(0x800FF624u))))) + 6));
        }
    }
    return result;
}

uint32 sub_80062A64(uint32 a1, uint32 a2)
{
    sint32 v4;
    sint32 v5;
    sint32 result;
    if ((((r_u16(((uint32)((a1 + 78)))) & 1) != 0) && (a2 != 0x800FF5E0u)))
        sub_80062BF0(a1);
    v4 = r_u32(((uint32)((a1 + 28))));
    if (v4)
        w_u32(((uint32)((v4 + 48))), r_u32(((uint32)((a1 + 48)))));
    v5 = r_u32(((uint32)((a1 + 48))));
    if (v5)
        w_u32(((uint32)((v5 + 28))), r_u32(((uint32)((a1 + 28)))));
    result = r_u32(a2);
    if ((r_u32(a2) == a1))
    {
        result = r_u32(((uint32)((a1 + 28))));
        w_u32(a2, result);
    }
    return result;
}

/* TODO Missing call adapter indirect */
uint32 sub_800629BC(uint32 object, uint32 reason)
{
    uint32 child = r_u32(object + 196u), table;
    w_u32(object + 68u, 0x800A3380u);
    if (child)
    {
        table = r_u32(child + 68u);
        (void)apocalypse_object_cleanup(r_u32(table + 12u), child + (uint32)(sint32)(short)r_u16(table + 8u), 3u);
    }
    sub_80062650(object, 0u);
    if (reason & 1u)
        return sub_80062608(object);
    return reason & 1u;
}

/* TODO Missing call adapter indirect */
/* TODO Selected host-buffer callees and unknown class callbacks retain explicit boundaries */
static sint32 projectile_divide(uint32 n, uint32 d)
{
    if (!d)
        return (sint32)n < 0 ? 1 : -1;
    return (sint32)((long long)(sint32)n / (long long)(sint32)d);
}

static void projectile_rotation_prepare(sint16 rotation[3])
{
    uint16 first, third, second;
    memcpy(&first, rotation, 2u);
    memcpy(&third, rotation + 2, 2u);
    first &= 4095u;
    memcpy(rotation, &first, 2u);
    memcpy(&second, rotation + 1, 2u);
    third &= 4095u;
    memcpy(rotation + 2, &third, 2u);
    second &= 4095u;
    memcpy(rotation + 1, &second, 2u);
}

static void projectile_virtual52(uint32 object, uint32 damage, const sint32 vector[3], uint32 reason)
{
    uint32 table = r_u32(object + 68u);
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 48u);
    apocalypse_object_virtual52_native(r_u32(table + 52u), receiver, damage, vector, reason);
}

static void projectile_spawn_host(uint32 object, const sint32 start[3], const sint32 end[3])
{
    apocalypse_projectile_spawn_234CC(object, start, end);
}

static void projectile_impact_host(uint32 effect, const sint32 position[3])
{
    apocalypse_projectile_impact_2289C(effect, position);
}

/* TODO Selected host-buffer callees and unknown class callbacks retain explicit boundaries */

uint32 sub_80022C90(uint32 a1, uint32 a2, uint32 a3)
{
    return apocalypse_weapon_fire_22C90(a1, (const sint32 *)psx_addr(a2, 12u), (const sint16 *)psx_addr(a3, 6u));
}

static void projectile_2374C_native_rotation(uint32 object, uint32 position, const sint16 rotation[3])
{
    apocalypse_projectile_construct_2374C(object, position, rotation);
}

uint32 apocalypse_weapon_fire_22C90(uint32 a1, const sint32 position[3], const sint16 input_rotation[3])
{
    sint32 v5;
    sint32 v6;
    uint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    short v12;
    sint32 v13;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    uint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    sint32 result;
    sint32 v28;

    union
    {
        uint32 words[2];
        sint16 halves[4];
    } rotation;

    sint32 endpoint[3];
    uint32 v34[4];
    int v35[4];
    int v36[4];
    int v37[3];
    uint32 v39[4];
    uint32 v40[4];
    uint32 v41[4];
    sint32 v42;
    sint32 v43;
    uint32 position_x;
    memcpy(&v5, position + 1, 4u);
    memcpy(&v6, position + 2, 4u);
    memcpy(&position_x, position, 4u);
    w_u32(((uint32)((a1 + 8))), position_x);
    w_u32(((uint32)((a1 + 12))), v5);
    w_u32(((uint32)((a1 + 16))), v6);
    v7 = ((uint32)((a1 + 8)));
    if (!(r_u32(((uint32)((a1 + 20))))))
    {
        sub_80022ABC(a1);
        v8 = r_u32(((uint32)((a1 + 76))));
        w_u32(((uint32)((a1 + 28))), r_u32(0x800FF2F0u));
        w_u32(((uint32)((a1 + 52))), sub_80069DF0(v8, 0x2000, 0));
    }
    if (r_u32(((uint32)((a1 + 80)))))
    {
        v9 = sub_80032DC0(124);
        if (v9)
            sub_8001CF9C(v9, v7, 5, 255, 235, 0, 2, 90, 10, 0, 20, (sint32)(20u * r_u32(a1 + 80)) / 256, 0, 0, (sint32)(14u * r_u32(a1 + 80)) / 256, 0, (sint32)(10u * r_u32(a1 + 80)) / 256, (sint32)(20u * r_u32(a1 + 80)) / 256, 0, 1);
    }
    if ((r_u32(((uint32)((a1 + 20)))) && ((sint32)(r_u32(0x800FF2F0u) - r_u32(((uint32)((a1 + 24))))) < projectile_divide(30u, r_u8(a1 + 70u)))))
    {
        w_u32(((uint32)((a1 + 20))), 1);
        return 0;
    }
    v10 = r_u8(((uint32)((a1 + 68))));
    w_u32(((uint32)((a1 + 84))), 1);
    if (v10)
    {
        w_u32(0x800FF3ACu, 0);
        v11 = sub_80032DC0(116);
        if (v11)
            projectile_2374C_native_rotation(v11, a1 + 8u, input_rotation);
        w_u32(0x800FF3ACu, 1);
    }
    memcpy(&v12, input_rotation + 2, 2u);
    memcpy(&rotation.words[0], input_rotation, 4u);
    rotation.halves[2] = v12;
    rotation.words[0] = ((rotation.words[0] & 0xFFFF0000u) | (((((rotation.words[0] + sub_80066570(((2 * r_u32(((uint32)((a1 + 60))))) | 1))) - r_u16(((uint32)((a1 + 60)))))) & 0xFFFFu) << 0));
    rotation.halves[1] = (sint16)((uint32)(uint16)rotation.halves[1] + sub_80066570((2u * r_u32(a1 + 64u)) | 1u) - r_u16(a1 + 64u));
    projectile_rotation_prepare(rotation.halves);
    xport_draft_host_sub_800667CC_p13(endpoint, r_u32(a1 + 72u), &rotation.words[0]);
    xport_draft_host_sub_8006C4EC_p12(v34, endpoint, a1 + 72u);
    xport_draft_host_sub_8006C0B8_p1(endpoint, a1 + 8u);
    rotation.halves[2] = 0;
    rotation.words[0] = ((rotation.words[0] & 0xFFFF0000u) | (((0) & 0xFFFFu) << 0));
    xport_draft_host_sub_800667CC_p13(v36, (uint32)-150, &rotation.words[0]);
    xport_draft_host_sub_8006C0B8_p1(v36, a1 + 8u);
    w_u32(0x800ED638u, v36[0]);
    w_u32(0x800ED640u, v36[2]);
    w_u32(0x800ED63Cu, v36[1]);
    v13 = 0;
    w_u32(0x800ED644u, r_u32(((uint32)((a1 + 8)))));
    v14 = 0;
    w_u32(0x800ED648u, r_u32(((uint32)((a1 + 12)))));
    w_u32(0x800ED64Cu, r_u32(((uint32)((a1 + 16)))));
    sub_8007BB24(0x800ED638u);
    w_u32(0x800FF974u, 1);
    sub_8007DD04(0x800ED638u, 1);
    w_u32(0x800FF974u, 0);
    v15 = 0;
    if (r_u32(0x800ED6A0u))
        goto LABEL_13;
    w_u32(0x800ED638u, r_u32(((uint32)((a1 + 8)))));
    w_u32(0x800ED63Cu, r_u32(((uint32)((a1 + 12)))));
    v16 = r_u32(((uint32)((a1 + 16))));
    w_u32(0x800ED644u, endpoint[0]);
    w_u32(0x800ED648u, endpoint[1]);
    w_u32(0x800ED64Cu, endpoint[2]);
    w_u32(0x800ED640u, v16);
    sub_8007BB24(0x800ED638u);
    w_u32(0x800FF974u, 1);
    sub_8007DD04(0x800ED638u, 1);
    v17 = r_u32(((uint32)((a1 + 44))));
    w_u32(0x800FF974u, 0);
    v18 = xport_draft_host_sub_8007C398_p23(a1 + 8u, endpoint, v35, r_u32(v17), 0);
    v15 = v18;
    if (!r_u32(0x800ED6A0u))
    {
        if (!v18)
            goto LABEL_20;
        goto LABEL_19;
    }
    if (v18)
    {
        if ((sub_8006689C((v18 + 4), (a1 + 8)) >= ((uint32)(r_u32(0x800ED678u)))))
        {
            v13 = 1;
            goto LABEL_20;
        }
    LABEL_19:
        v14 = 1;

        goto LABEL_20;
    }
LABEL_13:
    v13 = 1;

LABEL_20:
    v19 = 0;

    if (!v13)
        goto LABEL_37;
    sub_8001E740(r_u32(0x800ED6A0u), r_u32(0x800ED6B8u), r_u32(((uint32)((a1 + 40)))));
    if (((r_u16(((uint32)(r_u32(0x800ED6A0u)))) & 0x10) != 0))
        projectile_virtual52(r_u32(0x800ED6A0u), r_u32(a1 + 40u), (const sint32 *)v34, 28u);
    v19 = 12;
    endpoint[0] = r_u32(0x800ED6A4u);
    endpoint[1] = r_u32(0x800ED6A8u);
    endpoint[2] = r_u32(0x800ED6ACu);
    if (!sub_80066570(4))
    {
        xport_draft_host_sub_8006C3AC_p123(v37, endpoint, psx_addr(a1 + 8u, 12));
        v20 = (sint32)((uint32)(v37[0] >> 6) * (uint32)(sint32)(sint16)r_u16(0x800ED6B0u) + (uint32)(v37[1] >> 6) * (uint32)(sint32)(sint16)r_u16(0x800ED6B2u) + (uint32)(v37[2] >> 6) * (uint32)(sint32)(sint16)r_u16(0x800ED6B4u));
        v21 = (v20 > 0);
        v22 = (v20 >> 12);
        if (!v21)
        {
            v37[0] -= ((sint32)((uint32)v22 * (uint32)(sint32)(sint16)r_u16(0x800ED6B0u)) >> 6);
            v37[2] -= ((sint32)((uint32)v22 * (uint32)(sint32)(sint16)r_u16(0x800ED6B4u)) >> 6);
            v43 = xport_draft_host_sub_8006BF04_p1(v37);
            if ((v43 >= 201))
            {
                v42 = 400;
                xport_draft_host_sub_8006C40C_p123(v41, v37, &v42);
                xport_draft_host_sub_8006C4EC_p123(v40, v41, &v43);
                xport_draft_host_sub_8006C34C_p123(v39, endpoint, v40);
                v23 = sub_80032DC0(108);
                if (v23)
                    projectile_spawn_host(v23, endpoint, (const sint32 *)v39);
            }
        }
    }
    if (sub_80066570(4))
        goto LABEL_37;
    v24 = sub_80066570(3);
    if ((v24 == 1))
    {
        v25 = 28;
    }
    else if ((v24 >= 2))
    {
        v25 = 29;
        if ((v24 != 2))
            goto LABEL_37;
    }
    else
    {
        v25 = 27;
        if (v24)
            goto LABEL_37;
    }
    xport_draft_host_sub_80069EF4_p2(v25, endpoint, 0u);
LABEL_37:
    if (v14)
    {
        v19 = 4;
        projectile_virtual52((uint32)v15, r_u32(a1 + 40u), (const sint32 *)v34, 28u);
        if (sub_80066570(2))
            v19 = 20;
        endpoint[0] = v35[0];
        endpoint[1] = v35[1];
        endpoint[2] = v35[2];
    }

    if (v19)
        projectile_impact_host(v19, endpoint);
    v26 = sub_80032DC0(108);
    if (v26)
        projectile_spawn_host(v26, (const sint32 *)psx_addr(a1 + 8u, 12), endpoint);
    result = 1;
    v28 = r_u32(0x800FF2F0u);
    w_u32(((uint32)((a1 + 20))), 1);
    w_u32(((uint32)((a1 + 24))), v28);
    return result;
}

uint32 sub_8001CF9C(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13, uint32 a14, uint32 a15, uint32 a16, uint32 a17, uint32 a18, uint32 a19, uint32 a20)
{
    return xport_draft_host_sub_8001CF9C_p2(a1, psx_addr(a2, 12u), a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20);
}

uint32 xport_draft_host_sub_8001CF9C_p2(uint32 a1, const void *position, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13, uint32 a14, uint32 a15, uint32 a16, uint32 a17, uint32 a18, uint32 a19, uint32 a20)
{
    uint32 first;
    sint32 v27;
    sint32 v28;
    sub_80034598(a1, a3, 1);
    w_u32(((uint32)((a1 + 68))), 0x800A1154u);
    memcpy(&first, position, 4u);
    memcpy(&v27, (const uint8 *)position + 4u, 4u);
    memcpy(&v28, (const uint8 *)position + 8u, 4u);
    w_u32(((uint32)((a1 + 24))), first);
    w_u32(((uint32)((a1 + 28))), v27);
    w_u32(((uint32)((a1 + 32))), v28);
    sub_80034A18(a1, a4 & 255u, a5 & 255u, a6 & 255u);
    w_u16(((uint32)((a1 + 104))), a7);
    sub_80034A44(a1, a8 & 255u, a9 & 255u, a10 & 255u);
    w_u16(((uint32)((a1 + 106))), a11);
    sub_80034A9C(a1, 0, 0, 0, 0);
    sub_800349D0(a1, 0, a12);
    w_u16(((uint32)((a1 + 108))), a13);
    w_u8(((uint32)((a1 + 120))), a14);
    w_u16(((uint32)((a1 + 110))), a15);
    w_u16(((uint32)((a1 + 112))), a16);
    w_u16(((uint32)((a1 + 114))), a17);
    w_u16(((uint32)((a1 + 116))), a18);
    w_u16(((uint32)((a1 + 118))), a19);
    w_u16(((uint32)((a1 + 10))), a20);
    sub_8001D114(a1);
    return a1;
}

uint32 sub_80034598(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 v6;
    sint32 v7;
    uint32 v8;
    sub_80032F7C(a1);
    a2 *= 2;
    w_u32(((uint32)((a1 + 80))), a2);
    w_u32(((uint32)((a1 + 68))), 0x800A1D08u);
    w_u32(((uint32)((a1 + 84))), a3);
    w_u16(((uint32)((a1 + 92))), (a2 ? 0x1000u / a2 : 0xFFFFFFFFu));
    v6 = sub_8006B864((8 * (a2 + (a2 * a3))), 0, 1);
    v7 = (r_u32(((uint32)((a1 + 80)))) * r_u32(((uint32)((a1 + 84)))));
    w_u32(((uint32)((a1 + 72))), v6);
    v8 = 0;
    w_u32(((uint32)((a1 + 76))), (v6 + (8 * r_u32(((uint32)((a1 + 80)))))));
    if (v7)
    {
        do
            w_u32(((uint32)((((8 * v8++) + r_u32(((uint32)((a1 + 76))))) + 4))), 973078528);
        while ((v8 < (r_u32(((uint32)((a1 + 80)))) * r_u32(((uint32)((a1 + 84)))))));
    }
    w_u32(((uint32)((a1 + 88))), 838860800);
    w_u32(((uint32)((a1 + 100))), -1);
    sub_80032E50(((uint32)(a1)), 0x800FF450u);
    return a1;
}

uint32 sub_8001BCDC(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 duration)
{
    uint32 packet = sub_8001BC58(a1);
    if (packet)
    {
        uint32 red = a2 & 255u, green = a3 & 255u, blue = a4 & 255u;
        w_u32(packet + 16u, a1);
        w_u32(packet, (blue << 16) | (green << 8) | red);
        w_u8(packet + 12u, 0u);
        w_u8(packet + 9u, 0u);
        w_u8(packet + 10u, 0u);
        w_u8(packet + 13u, duration);
        w_u16(packet + 4u, (sint32)(0x8000u - (red << 8)) / (sint32)duration);
        w_u16(packet + 6u, (sint32)(0x8000u - (green << 8)) / (sint32)duration);
        w_u16(packet + 8u, (sint32)(0x8000u - (blue << 8)) / (sint32)duration);
        w_u16(a1, r_u16(a1) | 0x400u);
        w_u32(a1 + 32u, r_u32(packet));
        return r_u32(packet);
    }
    return packet;
}

uint32 sub_8001BC58(uint32 a1)
{
    uint32 v1;
    uint32 v3;
    v1 = 0;
    if (!r_u32(0x800FF1D8u))
        return 0;
    if (((r_u16(a1) & 0x400) != 0))
    {
        v1 = ((uint32)(r_u32(0x800FF1D4u)));
        if (!r_u32(0x800FF1D4u))
        {
        LABEL_8:
            v1 = r_u32(0x800FF1D8u);

            v3 = r_u32(((uint32)(((((uint32)(0x800F2624u)) + (((uint32)(r_u32(0x800FF1D8u)))) * 1u) + (2146490864) * 1u))));
            w_u32(((uint32)(((((uint32)(0x800F2624u)) + (((uint32)(r_u32(0x800FF1D8u)))) * 1u) + (2146490864) * 1u))), r_u32(0x800FF1D4u));
            w_u32(0x800FF1D4u, ((sint32)(v1)));
            w_u32(0x800FF1D8u, v3);
            return v1;
        }
        do
        {
            if ((((uint32)(r_u32((r_u32(v1) + (4) * 4u)))) == a1))
                break;
            v1 = ((uint32)(r_u32((r_u32(v1) + (5) * 4u))));
        } while (v1);
    }
    if (!v1)
        goto LABEL_8;
    return v1;
}

uint32 sub_800234CC(uint32 a1, uint32 a2, uint32 a3)
{
    return apocalypse_projectile_spawn_234CC(a1, (const sint32 *)psx_addr(a2, 12u), (const sint32 *)psx_addr(a3, 12u));
}

uint32 apocalypse_projectile_spawn_234CC(uint32 a1, const sint32 start[3], const sint32 end[3])
{
    sint32 v6;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    uint32 result;
    sint32 v13;
    sint32 v14;
    sint32 vector15[3];
    char v18[16];
    sint32 vector19[3];
    sint32 vector22[3];
    char v25[16];
    sint32 v26;
    sint32 v27;
    sint32 v28;
    sint32 v29;
    sub_80032824(a1);
    w_u32((a1 + (17) * 4u), 0x800A1530u);
    memcpy(&vector15[0], end, 4u);
    memcpy(&v6, end + 1, 4u);
    memcpy(&v7, end + 2, 4u);
    vector15[1] = v6;
    vector15[2] = v7;
    xport_draft_host_sub_8006C3AC_p123(v18, vector15, start);
    v26 = xport_draft_host_sub_8006BF04_p1(v18);
    if (v26)
        xport_draft_host_sub_8006C4EC_p123(v18, v18, &v26);
    else
        sub_80032ED8(a1);
    if ((v26 >= 3001))
    {
        v26 = 3000;
        xport_draft_host_sub_8006C47C_p123(vector22, &v26, v18);
        xport_draft_host_sub_8006C34C_p123(vector19, start, vector22);
        vector15[0] = vector19[0];
        vector15[1] = vector19[1];
        vector15[2] = vector19[2];
    }
    v27 = sub_80066570((v26 / 3));
    xport_draft_host_sub_8006C40C_p123(v25, v18, &v27);
    xport_draft_host_sub_8006C34C_p123(vector19, start, v25);
    v8 = vector19[1];
    v9 = vector19[2];
    w_u32((a1 + (20) * 4u), vector19[0]);
    w_u32((a1 + (21) * 4u), v8);
    w_u32((a1 + (22) * 4u), v9);
    xport_draft_host_sub_8006C3AC_p123(vector22, vector15, psx_addr(a1 + 80u, 12u));
    v28 = 5;
    xport_draft_host_sub_8006C4EC_p123(vector19, vector22, &v28);
    xport_draft_host_sub_8006C34C_p13(vector22, a1 + 80u, vector19);
    v10 = vector22[1];
    v11 = vector22[2];
    w_u32((a1 + (23) * 4u), vector22[0]);
    w_u32((a1 + (24) * 4u), v10);
    w_u32((a1 + (25) * 4u), v11);
    v29 = 1;
    xport_draft_host_sub_8006C5C4_p123(vector22, vector19, &v29);
    result = a1;
    v13 = vector22[1];
    v14 = vector22[2];
    w_u32((a1 + (9) * 4u), vector22[0]);
    w_u32((a1 + (10) * 4u), v13);
    w_u32((a1 + (11) * 4u), v14);
    w_u32((a1 + (18) * 4u), ((r_u32((a1 + (18) * 4u)) & 0xFF000000) | 0x2000000));
    return result;
}

uint32 sub_80032824(uint32 a1)
{
    sub_80032F7C(a1);
    w_u32((a1 + (18) * 4u), 1350598784);
    w_u32((a1 + (17) * 4u), 0x800A1B08u);
    w_u32((a1 + (19) * 4u), 1434484864);
    sub_80032E50(a1, 0x800FF464u);
    return a1;
}

uint32 sub_8002289C(uint32 flags, uint32 position)
{
    return apocalypse_projectile_impact_2289C(flags, (const sint32 *)psx_addr(position, 12u));
}

static uint32 impact_particles_host(uint32 object, const sint32 position[3], uint32 red, uint32 green, uint32 blue, uint32 kind, uint32 speed, uint32 mode, uint32 count, uint32 scale, uint32 lifetime)
{
    return apocalypse_particle_construct_358B4(object, position, red, green, blue, kind, speed, mode, count, scale, lifetime);
}

static uint32 impact_325B0_native_position_missing(uint32 object, const sint32 position[3], uint32 mode, uint32 kind, uint32 red, uint32 green, uint32 blue, uint32 count, uint32 speed, uint32 scale, uint32 lifetime)
{
    fprintf(stderr, "TODO 800325B0 native position constructor\n");
    abort();
}

uint32 apocalypse_projectile_impact_2289C(uint32 a1, const sint32 position[3])
{
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 result;
    sint32 v9;
    sint32 v10;
    if (((a1 & 4) != 0))
    {
        v4 = sub_80032DC0(120);
        v5 = v4;
        if (v4)
            v5 = xport_draft_host_sub_80035478_p2(v4, position, 0, 744, 1, 1, 0xFFFFFFFFu);
        w_u8(((uint32)((v5 + 90))), sub_80066570((r_u8(((uint32)((v5 + 89)))) - 1)));
        w_u16(((uint32)((v5 + 96))), sub_80066570(4096));
        w_u16(((uint32)((v5 + 64))), 100);
    }
    v6 = 0;
    if (((a1 & 8) != 0))
    {
        do
        {
            v7 = sub_80032DC0(112);
            if (v7)
                impact_particles_host(v7, position, 128, 128, 128, 6, 1000, 1, 40, 0x3000, 20);
            ++v6;
        } while ((v6 < 4));
    }
    result = (a1 & 0x10);
    if (((a1 & 0x10) != 0))
    {
        result = sub_80032DC0(96);
        v9 = result;
        if (result)
        {
            v10 = sub_80066570(3);
            return impact_325B0_native_position_missing(v9, position, 1, v10 + 1, 128, 128, 128, 8, 5, 700, 40);
        }
    }
    return result;
}

uint32 sub_800332A4(uint32 a1)
{
    sint32 result;
    result = (r_u32(((uint32)((a1 + 76)))) | 0x2000000);
    w_u32(((uint32)((a1 + 76))), result);
    return result;
}

uint32 sub_800355A0(uint32 a1)
{
    sint32 v2;
    sint32 result;
    sint32 v4;
    sint8 v5;
    sub_800333F0(a1);
    if (r_u32(((uint32)((a1 + 112)))))
    {
        v2 = r_u32(((uint32)((a1 + 116))));
        result = ((sint8)r_u8(a1 + 90u) < v2);
        if ((sint8)r_u8(a1 + 90u) >= v2)
            return sub_80032ED8(a1);
    }
    else
    {
        v4 = r_u8(((uint32)((a1 + 89))));
        result = -2;
        if ((sint8)r_u8(a1 + 90u) >= v4)
        {
            if ((r_u32(((uint32)((a1 + 116)))) == -2))
                v5 = (v4 - 1);
            else
                v5 = 0;
            return sub_800333D0(a1, v5);
        }
    }
    return result;
}

uint32 sub_8001D240(uint32 a1)
{
    uint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 result;
    v2 = 0;
    if (r_u32(((uint32)((a1 + 80)))))
    {
        v3 = 0;
        do
        {
            sub_80032D3C(((r_u32(((uint32)((a1 + 72)))) + v3) + 4), (sint16)r_u16(a1 + 106u));
            sub_80032D3C((a1 + 88), (sint16)r_u16(a1 + 104u));
            ++v2;
            v3 = (8 * v2);
        } while ((v2 < r_u32(((uint32)((a1 + 80))))));
    }
    if (r_u8(((uint32)((a1 + 120)))))
        sub_8001D114(a1);
    if (r_u16(((uint32)((a1 + 10)))))
    {
        v4 = r_u16(((uint32)((a1 + 10))));
        result = (sint16)(uint16)(r_u16(a1 + 8u) + 1u);
        w_u16(((uint32)((a1 + 8))), result);
        result = ((short)(result));
        if ((v4 >= ((short)(result))))
            return result;
    }
    else
    {
        result = (r_u32(((uint32)((a1 + 88)))) & 0xFFFFFF);
        if (result)
            return result;
    }
    return sub_80032ED8(a1);
}

uint32 sub_800236D4(uint32 a1)
{
    sint32 v2;
    sint32 v3;
    sint32 v4;
    v2 = r_u32(((uint32)((a1 + 104))));
    v3 = (a1 + 80);
    if (v2)
    {
        sub_8006C0B8(v3, (a1 + 36));
        sub_8006C0B8((a1 + 92), (a1 + 36));
        v2 = r_u32(((uint32)((a1 + 104))));
    }
    v4 = (sint32)((uint32)v2 + 1u);
    w_u32(((uint32)((a1 + 104))), v4);
    if ((v4 == 4))
        sub_80032ED8(a1);
    return sub_80032D3C((a1 + 76), 16);
}

uint32 sub_8001FCD4(uint32 a1, uint32 a2)
{
    sint32 result;
    w_u32(((uint32)((a1 + 68))), 0x800A1154u);
    result = sub_800348A8(a1, 0);
    if (((a2 & 1) != 0))
        return sub_80032E30(a1);
    return result;
}

uint32 sub_80034DE8(uint32 a1, uint32 a2)
{
    uint32 result;
    sint32 v5;
    w_u32((a1 + (17) * 4u), 0x800A1CD8u);
    sub_80032E7C(a1, 0x800FF438u);
    result = sub_800331EC(((sint32)(a1)), 0);
    if (((a2 & 1) != 0))
        return ((uint32)(sub_80032E30(((sint32)(a1)))));
    return result;
}

uint32 sub_8002BC44(uint32 a1, uint32 a2)
{
    sint32 result;
    w_u32(((uint32)((a1 + 68))), 0x800A1530u);
    result = sub_80032880(a1, 0);
    if (((a2 & 1) != 0))
        return sub_80032E30(a1);
    return result;
}

uint32 sub_8001CC1C(uint32 a1)
{
    sint32 v1;
    sint32 result;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    uint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 i;
    uint32 v14;
    sint32 v15;
    sint32 v16;
    int v17[4];
    v1 = (sint32)r_u32(0x800FF1F0u);
    w_u32(0x800FF1F0u, (uint32)v1 + 1u);
    result = ((sint32)r_u32(0x800FF1F0u) < 10);
    if (((sint32)r_u32(0x800FF1F0u) >= 10))
    {
        w_u32(0x800FF1F0u, v1);
        return result;
    }
    if (sub_80022318(a1, 1, 1))
    {
        v5 = 0;
        v4 = sub_8006F4D0(r_u8(((uint32)((a1 + 27)))));
        v6 = sub_80063D3C(r_u32(((uint32)(((4 * r_u16(((uint32)((a1 + 22))))) + r_u32((0x800FF7B4u + (v4) * 4u)))))));
        if (v6)
            v5 = (r_u16(r_u32(((uint32)(((4 * r_u16(((uint32)((v6 + 10))))) + r_u32(0x800FF624u)))))) == 9);
        v7 = 512;
        if (r_u32(0x800FF738u))
            goto LABEL_18;
        v8 = r_u32(((uint32)((a1 + 8))));
        v9 = r_u32(((uint32)((a1 + 12))));
        v17[0] = r_u32(((uint32)((a1 + 4))));
        v17[1] = v8;
        v17[2] = v9;
        if (v5)
        {
            xport_draft_host_sub_80067388_p1(v17, 0u, (sint16)r_u16(0x800EC51Cu), (sint16)r_u16(0x800EC51Au), 1u);
            v10 = 2;
            if (((r_u32(0x800FF2F0u) & 1) != 0))
                v10 = 1;
            xport_draft_host_sub_80069EF4_p2(v10, v17, 0u);
            v11 = sub_80032DC0(124);
            if (v11)
                xport_draft_host_sub_8001EB64_p2(v11, v17, 64u, 128u, 0u, 512u, 10u);
            xport_draft_host_sub_8001E4C8_p1(v17, 20u, 10u, 750u, 80u);
            v12 = sub_80032DC0(124);
            if (v12)
            {
                xport_draft_host_sub_8001CF9C_p2(v12, v17, 5u, 255u, 255u, 255u, 1u, 16u, 64u, 0u, 1u, 60u, 0u, 1u, 100u, 120u, 80u, 100u, 10u, 4u);
                v7 = 512;
            LABEL_18:
                if (v5)
                    v7 = 1024;

                for (i = r_u32(0x800FF794u); i; i = r_u32(((uint32)((i + 28)))))
                {
                    if ((i != a1))
                    {
                        v14 = r_u32(r_u32(((uint32)(((4 * r_u16(((uint32)((i + 22))))) + r_u32((0x800EAEF8u + (((16 * r_u8(((uint32)((i + 27))))) + 4)) * 4u)))))));
                        if (((v14 & 2) != 0))
                        {
                            v15 = 0;
                            if ((v14 >> 24))
                            {
                                if (v5)
                                {
                                    v15 = (sub_8006696C((a1 + 4), (i + 4)) < v7);
                                }
                                else if ((sub_8006696C((a1 + 4), (i + 4)) < v7))
                                {
                                    v16 = (r_u32(((uint32)((a1 + 8)))) - r_u32(((uint32)((i + 8)))));
                                    if (((v16 > 409600) && (v16 <= 1638399)))
                                        v15 = 1;
                                }
                                if (v15)
                                    sub_8001CC1C(i);
                            }
                        }
                    }
                }

                return (w_u32(0x800FF1F0u, r_u32(0x800FF1F0u) - 1u), r_u32(0x800FF1F0u));
            }
        }
        else
        {
            if (!sub_80066570(5))
            {
                apocalypse_explosion_native(v17, 1u);
                v7 = 512;
                goto LABEL_18;
            }
            apocalypse_explosion_native(v17, 0u);
        }
        v7 = 512;
        goto LABEL_18;
    }
    return (w_u32(0x800FF1F0u, r_u32(0x800FF1F0u) - 1u), r_u32(0x800FF1F0u));
}

uint32 sub_80021B3C(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    sint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 result;
    if (a1)
    {
        v7 = r_u32(((uint32)(((4 * ((unsigned char)(a2))) + a4))));
        v8 = r_u32(((uint32)((((a2 >> 6) & 0x3FC) + a4))));
        v9 = r_u32(((uint32)((((a2 >> 14) & 0x3FC) + a4))));
        v10 = ((((unsigned char)(r_u32(((uint32)(((4 * ((unsigned char)(a2))) + a4)))))) + ((unsigned char)(r_u32(((uint32)((((a2 >> 6) & 0x3FC) + a4))))))) + ((unsigned char)(r_u32(((uint32)((((a2 >> 14) & 0x3FC) + a4)))))));
        v11 = ((((v7 >> 8) & 255u) + ((v8 >> 8) & 255u)) + ((v9 >> 8) & 255u));
        v12 = ((((v7 >> 16) & 255u) + ((v8 >> 16) & 255u)) + ((v9 >> 16) & 255u));
        if ((a3 == 4))
        {
            v13 = r_u32(((uint32)(((4 * ((a2 >> 8) & 255u)) + a4))));
            w_u16(0x800FFB80u, ((r_u16(0x800FFB80u) & 0xFFFFFF00u) | (((((v10 + ((unsigned char)(v13))) / 4)) & 0xFFu) << 0)));
            w_u16(0x800FFB80u, ((r_u16(0x800FFB80u) & 0xFFFF00FFu) | (((((v11 + ((v13 >> 8) & 255u)) / 4)) & 0xFFu) << 8)));
            result = ((v12 + ((v13 >> 16) & 255u)) / 4);
        }
        else
        {
            w_u16(0x800FFB80u, ((r_u16(0x800FFB80u) & 0xFFFFFF00u) | ((((((unsigned long long)((1431655766LL * v10))) >> 32)) & 0xFFu) << 0)));
            w_u16(0x800FFB80u, ((r_u16(0x800FFB80u) & 0xFFFF00FFu) | ((((((unsigned long long)((1431655766LL * v11))) >> 32)) & 0xFFu) << 8)));
            result = (((unsigned long long)((1431655766LL * v12))) >> 32);
        }
    }
    else
    {
        result = ((a2 >> 16) & 65535u);
        w_u16(0x800FFB80u, a2);
    }
    w_u8(0x800FFB82u, result);
    return result;
}

uint32 sub_8006CBF8(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 result;
    short v4;
    sint32 v5;
    result = a1;
    v5 = ((v5 & 0xFFFF0000u) | ((((r_u16(a3) * r_u32(a2))) & 0xFFFFu) << 0));
    v5 = ((v5 & 0x0000FFFFu) | ((((r_u16((a3 + (1) * 2u)) * r_u32(a2))) & 0xFFFFu) << 16));
    v4 = (r_u16((a3 + (2) * 2u)) * r_u16(((uint32)(a2))));
    w_u32(((uint32)(a1)), v5);
    w_u16(((uint32)((a1 + 4))), v4);
    return result;
}

uint32 sub_8006CCE0(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 result;
    short v4;
    sint32 v5;
    result = a1;
    v5 = ((v5 & 0xFFFF0000u) | ((((r_u16(a2) >> r_u32(a3))) & 0xFFFFu) << 0));
    v5 = ((v5 & 0x0000FFFFu) | ((((r_u16((a2 + (1) * 2u)) >> r_u32(a3))) & 0xFFFFu) << 16));
    v4 = (r_u16((a2 + (2) * 2u)) >> r_u32(a3));
    w_u32(((uint32)(a1)), v5);
    w_u16(((uint32)((a1 + 4))), v4);
    return result;
}

uint32 sub_80021154(uint32 a1, uint32 a2)
{
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 result;
    sint32 v9;
    char v10[16];
    int v11[4];
    char v12[16];
    sint32 v13;
    sint32 v14;
    sint32 v15;
    sub_80032EE4(a1, a2);
    sub_8006C3AC(v10, a2, 0x800FFD58u);
    v14 = sub_8006BF04(v10);
    if (v14)
    {
        v13 = sub_80066570(48);
        sub_8006C47C(v12, &v13, v10);
        sub_8006C4EC(v11, v12, &v14);
        v4 = v11[1];
        v5 = v11[2];
        w_u32((a1 + (9) * 4u), v11[0]);
        w_u32((a1 + (10) * 4u), v4);
        w_u32((a1 + (11) * 4u), v5);
    }
    else
    {
        sub_80032ED8(a1);
    }
    v6 = sub_80066570(45);
    v7 = r_u32(0x800FF210u);
    w_u32((a1 + (9) * 4u), (r_u32((a1 + (9) * 4u)) + ((v6 * r_u16(((uint32)(r_u32(0x800FF210u))))))));
    w_u32((a1 + (10) * 4u), (r_u32((a1 + (10) * 4u)) + ((v6 * r_u16(((uint32)((v7 + 2))))))));
    w_u32((a1 + (11) * 4u), (r_u32((a1 + (11) * 4u)) + ((v6 * r_u16(((uint32)((v7 + 4))))))));
    w_u32((a1 + (10) * 4u), (r_u32((a1 + (10) * 4u)) - ((sub_80066570(175) << 12))));
    if (!sub_80066570(20))
    {
        v15 = 1;
        sub_8006C22C((a1 + (9) * 4u), &v15);
    }
    result = r_u32((a1 + (10) * 4u));
    if ((result > 0))
        w_u32((a1 + (10) * 4u), 0);
    v9 = r_u32((a1 + (50) * 4u));
    if (v9)
        return sub_800369F8(v9, (a1 + (6) * 4u));
    return result;
}

uint32 sub_80032EE4(uint32 a1, uint32 a2)
{
    sint32 result;
    sint32 v3;
    sint32 v4;
    result = r_u32(a2);
    v3 = r_u32((a2 + (1) * 4u));
    v4 = r_u32((a2 + (2) * 4u));
    w_u32((a1 + (6) * 4u), r_u32(a2));
    w_u32((a1 + (7) * 4u), v3);
    w_u32((a1 + (8) * 4u), v4);
    return result;
}

uint32 sub_8003445C(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9)
{
    sint32 result;
    w_u32(((uint32)((a1 + 160))), (((a2 << 16) | (a5 << 8)) | a4));
    w_u32(((uint32)((a1 + 164))), (((a3 << 16) | (a7 << 8)) | a6));
    result = (a8 | (a9 << 8));
    w_u16(((uint32)((a1 + 168))), result);
    return result;
}

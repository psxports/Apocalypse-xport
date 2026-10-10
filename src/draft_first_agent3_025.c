#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "game_mdec.h"

static uint32 vector_signed_quotient(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1u : 0xFFFFFFFFu;
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

/* Unverified native projection draft */
uint32 sub_80080EF4(uint32 input, uint32 count)
{
    uint32 bounds, bx, by, bz, output, vectors, reference, cache, packed, tagged;
    uint32 common = 0xFFFFu;
    if (!count) return 255u;
    bounds = r_u32(0x800FFB08u);
    bx = r_u32(bounds); by = r_u32(bounds + 4u); bz = r_u32(bounds + 8u);
    output = r_u32(0x800FFAC0u); vectors = r_u32(0x800FFAC4u);
    reference = output + 7992u; cache = r_u32(0x800FFB18u);
    packed = r_u32(input); tagged = r_u32(input + 4u);
    do
    {
        uint32 tag = tagged >> 16;
        xport_gte_write_data(0u, packed); xport_gte_write_data(1u, tagged);
        input += 8u; --count;
        xport_gte_execute(0x180001u);
        if (tag & 2u)
        {
            uint32 previous = reference - packed;
            uint32 xy = r_u32(previous), depth_flags = r_u32(previous + 4u);
            uint32 irxy = r_u32(previous + vectors - output);
            w_u32(output, xy); w_u32(output + 4u, depth_flags); w_u32(vectors, irxy);
            common &= depth_flags >> 16;
            packed = r_u32(input); tagged = r_u32(input + 4u);
        }
        else
        {
            uint32 irxy, xy, flags, depth_flags;
            sint32 x, y, depth;
            packed = r_u32(input); tagged = r_u32(input + 4u);
            irxy = (xport_gte_read_data(9u) & 0xFFFFu) | (xport_gte_read_data(10u) << 16);
            depth = (sint32)xport_gte_read_data(11u); xy = xport_gte_read_data(14u);
            x = (sint16)xy; y = (sint32)xy >> 16;
            flags = ((sint32)(bx & 0xFFFFu) < x)
                  | ((uint32)(x < (sint32)(by & 0xFFFFu)) << 1)
                  | ((uint32)((sint32)(bx >> 16) < y) << 2)
                  | ((uint32)(y < (sint32)(by >> 16)) << 3)
                  | ((uint32)(depth < (sint32)(bz & 0xFFFFu)) << 4)
                  | ((uint32)((sint32)(bz >> 16) < depth) << 5);
            if (depth < 0) flags ^= 15u;
            flags |= (flags << 8) ^ 0xFF00u; common &= flags;
            depth_flags = (flags << 16) | ((uint32)depth & 0xFFFFu);
            w_u32(output, xy); w_u32(output + 4u, depth_flags); w_u32(vectors, irxy);
            if (tag & 1u)
            {
                w_u32(cache, xy); w_u32(cache + 4u, depth_flags);
                w_u32(cache + vectors - output, irxy); cache -= 8u;
            }
        }
        output += 8u; vectors += 8u;
    } while (count);
    w_u32(0x800FFB18u, cache); w_u32(0x1F8001D4u, output);
    return common;
}

/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_800821E0(uint32 geometry)
{
    uint32 packed = r_u32(geometry);
    uint32 depth = r_u32(geometry + 4);
    xport_draft_gte_data_write(9, packed);
    xport_draft_gte_data_write(10, (uint32)((sint32)packed >> 16));
    xport_draft_gte_data_write(11, depth);
}

/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* Native semantic operands for the selected projection pipeline */
static void apocalypse_projection_ir_load(const sint16 v[3])
{
    uint32 i;
    for (i = 0u; i < 3u; ++i) xport_gte_write_data(9u + i, (uint32)(sint32)v[i]);
}
static void apocalypse_projection_ir_store(sint16 v[3])
{
    uint32 i;
    for (i = 0u; i < 3u; ++i) v[i] = (sint16)xport_gte_read_data(9u + i);
}
static void apocalypse_projection_matrix(const MATRIX *m, uint32 opcode)
{
    uint32 i;
    const sint16 *v = &m->m[0][0];
    (void)xport_gte_read_data(9u);
    for (i = 0u; i < 4u; ++i)
        xport_gte_write_control(i, (uint16)v[2u * i] | ((uint32)(uint16)v[2u * i + 1u] << 16));
    xport_gte_write_control(4u, (uint16)v[8]);
    xport_gte_execute(opcode);
}
static void apocalypse_projection_cross(const sint16 v[3])
{
    (void)xport_gte_read_data(9u);
    xport_gte_write_control(0u, (uint16)v[0]);
    xport_gte_write_control(2u, (uint16)v[1]);
    xport_gte_write_control(4u, (uint16)v[2]);
    xport_gte_execute(0x178000Cu);
}
static sint32 apocalypse_projection_div(sint32 a, sint32 b)
{
    /* Raw MIPS DIV has defined zero and overflow results */
    if (!b) return a < 0 ? 1 : -1;
    if ((uint32)a == 0x80000000u && b == -1) return a;
    return a / b;
}
static void apocalypse_projection_scale(const sint16 v[3], uint32 factor)
{
    uint32 i;
    (void)xport_gte_read_data(9u);
    xport_gte_write_data(8u, factor);
    for (i = 0u; i < 3u; ++i) xport_gte_write_data(25u + i, (uint32)(sint32)v[i]);
    xport_draft_gte_execute(0x1A8003Eu);
}
static void apocalypse_projection_vertex(uint32 slot, const sint16 c[3], const sint16 a[3], const sint16 b[3], sint32 sa, sint32 sb)
{
    sint16 x = (sint16)((uint32)(sint32)c[0] + (uint32)(sa * a[0]) + (uint32)(sb * b[0]));
    sint16 y = (sint16)((uint32)(sint32)c[1] + (uint32)(sa * a[1]) + (uint32)(sb * b[1]));
    sint16 z = (sint16)((uint32)(sint32)c[2] + (uint32)(sa * a[2]) + (uint32)(sb * b[2]));
    xport_gte_write_data(slot, (uint16)x | ((uint32)(uint16)y << 16));
    xport_gte_write_data(slot + 1u, (uint16)z);
}
void apocalypse_model_effect_native(uint32 model, const void *matrix32, uint32 vector, uint32 position, uint32 direction, uint32 color)
{
    MATRIX forward, inverse, diagonal, projection;
    sint16 center[3], extent[3], normal[3], cross[3], axis[3] = {0, 0, 0};
    sint16 basis1[3], basis2[3], coefficients[3], projected[3], edge1[3], edge2[3], dir[3];
    uint32 i, j, packed, squares[3], scale, packet, depth, screens, texture, bucket, header;
    sint32 average, numerator, distance;
    sint16 length;
    for (i = 0u; i < 3u; ++i)
    {
        sint32 low, high, half;
        packed = r_u32(model + 20u + 4u * i);
        low = (sint32)(packed & 0xFFFFu); high = (sint16)(packed >> 16);
        center[i] = (sint16)((low + high) >> 1);
        half = (low - high) >> 1;
        extent[i] = (sint16)((half * (i == 1u ? 11 : 5)) >> (i == 1u ? 3 : 2));
    }
    apocalypse_projection_ir_load(center);
    {
        uint32 words[5];
        memcpy(words, matrix32, sizeof(words));
        (void)xport_gte_read_data(9u);
        for (i = 0u; i < 5u; ++i) xport_gte_write_control(i, words[i]);
        xport_gte_execute(0x49E012u);
    }
    apocalypse_projection_ir_store(center);
    for (i = 0u; i < 3u; ++i)
    {
        uint16 offset;
        memcpy(&offset, (const uint8 *)matrix32 + 20u + i * 4u, sizeof(offset));
        center[i] = (sint16)((uint32)(sint32)center[i] + offset);
    }
    average = ((sint32)extent[0] + extent[1] + extent[2]) / 3;
    numerator = (sint32)((uint32)average << 16) >> 4;
    /* Original 85C04 copies only the nine rotation halfwords */
    memcpy(&forward.m[0][0], matrix32, 18u);
    for (i = 0u; i < 3u; ++i) for (j = 0u; j < 3u; ++j)
    {
        inverse.m[i][j] = forward.m[j][i];
        diagonal.m[i][j] = i == j ? (sint16)apocalypse_projection_div(numerator, extent[i]) : 0;
    }
    for (i = 0u; i < 3u; ++i) dir[i] = (sint16)r_u16(direction + 2u * i);
    apocalypse_projection_ir_load(dir);
    apocalypse_projection_matrix(&inverse, 0x49E012u);
    (void)sub_80085D64();
    apocalypse_projection_matrix(&diagonal, 0x49E012u);
    apocalypse_projection_ir_store(normal);
    length = (sint16)sub_80085D14();
    for (i = 0u; i < 3u; ++i) squares[i] = xport_gte_read_data(25u + i);
    if (squares[1] >= squares[0] && squares[2] >= squares[0]) axis[0] = 4096;
    else if (squares[0] >= squares[1] && squares[2] >= squares[1]) axis[1] = 4096;
    else axis[2] = 4096;
    apocalypse_projection_ir_load(normal);
    apocalypse_projection_cross(axis);
    apocalypse_projection_ir_store(cross);
    (void)sub_80085D64();
    scale = (uint32)((sint32)extent[0] * extent[1]);
    scale = (uint32)apocalypse_projection_div((sint32)scale, (sint16)average);
    scale *= (uint32)(sint32)extent[2]; scale *= (uint32)(sint32)length;
    scale = (uint32)(apocalypse_projection_div((sint32)scale, (sint16)average) >> 12);
    apocalypse_projection_matrix(&diagonal, 0x49E012u);
    apocalypse_projection_matrix(&forward, 0x49E012u);
    sub_80085D30(direction); sub_80085DBC(scale); apocalypse_projection_ir_store(basis1);
    apocalypse_projection_ir_load(normal); apocalypse_projection_cross(cross); (void)sub_80085D64();
    apocalypse_projection_matrix(&diagonal, 0x49E012u);
    apocalypse_projection_matrix(&forward, 0x49E012u);
    sub_80085D30(direction); sub_80085DBC(scale); apocalypse_projection_ir_store(basis2);
    xport_gte_write_data(0u, r_u32(vector)); xport_gte_write_data(1u, r_u32(vector + 4u));
    sub_80085CD4(direction);
    distance = (sint32)xport_gte_read_data(25u);
    if (distance < (sint32)r_u32(0x800FF964u)) return;
    for (i = 0u; i < 3u; ++i)
    {
        projection.m[0][i] = (sint16)((uint32)((sint32)r_u32(position + 4u * i) >> 12) - (uint32)(sint32)center[i]);
        projection.m[1][i] = basis1[i]; projection.m[2][i] = basis2[i];
    }
    apocalypse_projection_matrix(&projection, 0x486012u);
    apocalypse_projection_ir_store(coefficients);
    if (coefficients[0] > 0) return;
    for (i = 0u; i < 3u; ++i) center[i] = (sint16)((uint32)(sint32)center[i] - r_u16(r_u32(0x800FFB0Cu) + 4u + 4u * i));
    apocalypse_projection_ir_load(dir);
    apocalypse_projection_scale(center, (uint32)apocalypse_projection_div((sint32)((uint32)(sint32)coefficients[0] << 12), distance));
    apocalypse_projection_ir_store(projected);
    apocalypse_projection_ir_load(dir);
    apocalypse_projection_scale(basis1, (uint32)apocalypse_projection_div((sint32)(0u - ((uint32)(sint32)coefficients[1] << 12)), distance));
    apocalypse_projection_ir_store(edge1);
    apocalypse_projection_ir_load(dir);
    apocalypse_projection_scale(basis2, (uint32)apocalypse_projection_div((sint32)(0u - ((uint32)(sint32)coefficients[2] << 12)), distance));
    apocalypse_projection_ir_store(edge2);
    SetRotMatrix((MATRIX *)psx_addr(r_u32(0x800FFB0Cu) + 116u, 20u));
    for (i = 0u; i < 3u; ++i) xport_gte_write_control(5u + i, 0u);
    apocalypse_projection_vertex(0u, projected, edge1, edge2, -1, -1);
    apocalypse_projection_vertex(2u, projected, edge1, edge2, 1, -1);
    apocalypse_projection_vertex(4u, projected, edge1, edge2, -1, 1);
    xport_gte_execute(0x280030u);
    screens = r_u32(0x800FFAC0u);
    for (i = 0u; i < 3u; ++i) w_u32(screens + 8u * i, xport_gte_read_data(12u + i));
    apocalypse_projection_vertex(0u, projected, edge1, edge2, 1, 1);
    xport_gte_execute(0x180001u); w_u32(screens + 24u, xport_gte_read_data(14u));
    xport_gte_execute(0x168002Eu); depth = xport_gte_read_data(7u);
    if (depth >= 4096u) depth = 4095u;
    packet = r_u32(0x800FF668u); w_u32(0x800FF668u, packet + 40u);
    if (r_u32(0x800FF374u) < packet + 40u) { w_u32(0x800FF668u, packet); return; }
    (void)sub_8008AFFC(packet); w_u8(packet + 7u, r_u8(packet + 7u) | 2u);
    screens = r_u32(0x800FFAC0u);
    for (i = 0u; i < 4u; ++i) { w_u16(packet + 8u + i * 8u, r_u16(screens + i * 8u)); w_u16(packet + 10u + i * 8u, r_u16(screens + i * 8u + 2u)); }
    w_u8(packet + 4u, color); w_u8(packet + 5u, color >> 8); w_u8(packet + 6u, color >> 16);
    texture = r_u32(0x800FF408u); w_u8(packet + 12u, r_u8(texture));
    texture = r_u32(0x800FF408u); w_u8(packet + 13u, r_u8(texture + 1u));
    texture = r_u32(0x800FF408u); w_u8(packet + 20u, r_u8(texture + 4u) - 1u);
    texture = r_u32(0x800FF408u); w_u8(packet + 21u, r_u8(texture + 5u));
    texture = r_u32(0x800FF408u); w_u8(packet + 28u, r_u8(texture + 8u));
    texture = r_u32(0x800FF408u); w_u8(packet + 29u, r_u8(texture + 9u) - 1u);
    texture = r_u32(0x800FF408u); w_u8(packet + 36u, r_u8(texture + 10u) - 1u);
    texture = r_u32(0x800FF408u); w_u8(packet + 37u, r_u8(texture + 11u) - 1u);
    texture = r_u32(0x800FF408u); w_u16(packet + 14u, r_u16(texture + 2u));
    w_u16(packet + 22u, (r_u16(texture + 6u) & 0xFF9Fu) | 0x40u);
    bucket = r_u32(0x800FF660u) + 4u * depth + 112u;
    header = r_u32(bucket); w_u32(packet, (r_u32(packet) & 0xFF000000u) | (header & 0xFFFFFFu));
    w_u32(bucket, (r_u32(bucket) & 0xFF000000u) | (packet & 0xFFFFFFu));
}

void sub_8007A8F8(uint32 model, uint32 matrix, uint32 vector, uint32 position, uint32 direction, uint32 color)
{
    apocalypse_model_effect_native(model, psx_addr(matrix, 32u), vector, position, direction, color);
}
uint32 sub_80085D64(void)
{
    uint32 saved[3], result, divided[3], axis;
    for (axis = 0u; axis < 3u; ++axis)
        saved[axis] = xport_gte_read_data(9u + axis);
    result = sub_80085D14();
    for (axis = 0u; axis < 3u; ++axis)
        divided[axis] = vector_signed_quotient(saved[axis] << 12, result);
    for (axis = 0u; axis < 3u; ++axis)
        xport_gte_write_data(9u + axis, divided[axis]);
    return result;
}

uint32 sub_80085D14(void)
{
    sint32 input[3];
    uint32 squared[3], axis;
    for (axis = 0u; axis < 3u; ++axis)
        input[axis] = (sint32)xport_gte_read_data(9u + axis);
    xport_draft_gte_square_vector(input, squared);
    return sub_80085B54(squared[0] + squared[1] + squared[2]);
}

void sub_80085D30(uint32 input)
{
    uint32 packed = r_u32(input), last = r_u32(input + 4u);
    xport_gte_read_data(9u);
    xport_gte_write_control(0u, packed & 0xFFFFu);
    xport_gte_write_control(2u, packed >> 16);
    xport_gte_write_control(4u, last);
    xport_gte_execute(0x178000Cu);
}

void sub_80085CD4(uint32 input)
{
    uint32 words[5], index;
    for (index = 0u; index < 5u; ++index)
        words[index] = r_u32(input + index * 4u);
    xport_gte_read_data(9u);
    for (index = 0u; index < 5u; ++index)
        xport_gte_write_control(index, words[index]);
    xport_gte_execute(0x486012u);
}
uint32 sub_8008AFFC(uint32 a1)
{
    sint32 result;
    w_u8(((uint32)((a1 + 3))), 9);
    result = 44;
    w_u8(((uint32)((a1 + 7))), 44);
    return result;
}

uint32 sub_8006D164(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    short v4;
    short v5;
    short v6;
    sint32 v7;
    sint32 result;
    short v9;
    short v10;
    v4 = ((unsigned char)(((sint8)(r_u8((a4 + (2) * 1u))))));
    v5 = ((unsigned char)(((sint8)(r_u8((a4 + (3) * 1u))))));
    v6 = (a1 + ((sint8)(r_u8(a4))));
    v7 = (a2 + ((sint8)(r_u8((a4 + (1) * 1u)))));
    w_u16((a3 + (4) * 2u), v6);
    w_u16((a3 + (5) * 2u), v7);
    w_u16((a3 + (8) * 2u), (v6 + v4));
    result = v7;
    w_u16((a3 + (13) * 2u), (v7 + v5));
    v9 = v6;
    v10 = r_u16((a3 + (8) * 2u));
    v7 = ((v7 & 0xFFFF0000u) | (((r_u16((a3 + (13) * 2u))) & 0xFFFFu) << 0));
    w_u16((a3 + (9) * 2u), result);
    w_u16((a3 + (12) * 2u), v9);
    w_u16((a3 + (16) * 2u), v10);
    w_u16((a3 + (17) * 2u), v7);
    return result;
}

uint32 sub_8006D294(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11)
{
    sint32 v15;
    uint32 result;
    sint32 v17;
    sint32 v18;
    v15 = r_u32(0x800FF668u);
    result = (r_u32(0x800FF374u) < ((uint32)((r_u32(0x800FF668u) + 36))));
    if ((r_u32(0x800FF374u) >= ((uint32)((r_u32(0x800FF668u) + 36)))))
    {
        w_u32(0x800FF668u, (r_u32(0x800FF668u) + (36)));
        w_u8(((uint32)((v15 + 3))), 8);
        w_u8(((uint32)((v15 + 7))), 56);
        w_u16(((uint32)((v15 + 16))), (a1 + a3));
        w_u16(((uint32)((v15 + 32))), (a1 + a3));
        w_u8(((uint32)((v15 + 4))), a8);
        w_u8(((uint32)((v15 + 12))), a5);
        w_u8(((uint32)((v15 + 5))), a9);
        w_u8(((uint32)((v15 + 13))), a6);
        w_u8(((uint32)((v15 + 6))), a10);
        w_u8(((uint32)((v15 + 14))), a7);
        w_u8(((uint32)((v15 + 20))), a8);
        w_u8(((uint32)((v15 + 28))), a5);
        w_u8(((uint32)((v15 + 21))), a9);
        w_u8(((uint32)((v15 + 29))), a6);
        w_u8(((uint32)((v15 + 22))), a10);
        w_u8(((uint32)((v15 + 30))), a7);
        w_u16(((uint32)((v15 + 26))), (a2 + a4));
        w_u16(((uint32)((v15 + 34))), (a2 + a4));
        v17 = r_u32(0x800FF660u);
        w_u16(((uint32)((v15 + 10))), a2);
        w_u16(((uint32)((v15 + 18))), a2);
        w_u16(((uint32)((v15 + 8))), a1);
        w_u16(((uint32)((v15 + 24))), a1);
        v18 = ((4 * a11) + v17);
        w_u32(((uint32)(v15)), ((r_u32(((uint32)(v15))) & 0xFF000000) | (r_u32(((uint32)((v18 + 112)))) & 0xFFFFFF)));
        result = ((r_u32(((uint32)((v18 + 112)))) & 0xFF000000) | (v15 & 0xFFFFFF));
        w_u32(((uint32)((v18 + 112))), result);
    }
    return result;
}

/* TODO Missing call adapter SHIWORD */
/* TODO Missing call adapter sub_80098068 */
/* TODO Missing call adapter sub_8009AC6C */
/* TODO Missing call adapter sub_8009BDA0 */
/* TODO Missing call adapter sub_8009BE1C */
/* TODO Postincrement memory expressions may require ordering refinement */
static uint32 playback_pause_missing(uint32 command, uint32 parameter, uint32 result)
{
    /* TODO Connect 80098068 pause command */
    fprintf(stderr, "80098068 missing command=%08X parameter=%08X result=%08X\n", command, parameter, result);
    abort();
}
static uint32 playback_resume_missing(uint32 delay)
{
    /* TODO Connect 8009AC6C resume delay */
    fprintf(stderr, "8009AC6C missing delay=%08X\n", delay);
    abort();
}
uint32 sub_8002F284(void)
{
    uint32 result = r_u32(0x800FF250u);
    if (!result) return result;
    if (!r_u32(0x800FF26Cu))
    {
        if (r_u32(0x800FF24Cu))
        {
            w_u32(0x800FF24Cu, 0u);
            if (!r_u32(0x800FF248u) && !r_u32(0x800FF378u))
            {
                uint32 selected = sub_80066570(4u) == 1u ? 31u : sub_80066570(11u) + 4u;
                uint32 entry;
                w_u8(0x800FF299u, selected);
                entry = 0x800A5B8Cu + 28u * r_u8(0x800FF299u);
                w_u32(0x800FF27Cu, r_u16(entry + 8u));
                w_u32(0x800FF278u, r_u32(entry + 24u));
            }
            sub_8002FA48();
        }
        if (r_u32(0x800FF260u))
        {
            sint32 words;
            uint32 output;
            sub_8009BDA0(r_u32(0x800FF2A4u + r_u32(0x800FF25Cu) * 4u), 0u);
            words = (sint32)(sint16)r_u16(0x800FF2A0u) * (sint32)(sint16)r_u16(0x800FF2A2u);
            output = r_u32(0x800A5F68u + r_u32(0x800FF258u) * 4u);
            w_u32(0x800FF25Cu, 1u - r_u32(0x800FF25Cu));
            sub_8009BE1C(output, (uint32)(words / 2));
        }
        w_u32(0x800FF260u, sub_8002F9E4());
        if (r_u32(0x800FF270u))
        {
            uint32 next = r_u32(0x800FF270u) - 1u;
            w_u32(0x800FF270u, next);
            if (!next)
            {
                uint32 index = 0u, selected = 0u, point = 0x800A5B2Cu;
                sint32 best = 0xFFFFFF;
                while ((sint32)index < (sint32)r_u32(0x800FF294u))
                {
                    sint32 distance = (sint32)sub_8006696C(r_u32(0x800FF904u) + 4u, point);
                    if (distance < best) { best = distance; selected = index; }
                    point += 12u;
                    ++index;
                }
                sub_8002FB34(0x800A5B2Cu + 12u * selected);
                w_u32(0x800FF270u, 4u);
            }
        }
    }
    if (r_u32(0x800FF008u) || r_u32(0x800FF300u))
    {
        if (r_u32(0x800FF26Cu)) return 1u;
        w_u32(0x800FF26Cu, 1u);
        sub_8009B12C(1u, 0u, 0xFFFFFFFFu);
        return playback_pause_missing(9u, 0u, 0u);
    }
    result = r_u32(0x800FF26Cu);
    if (result)
    {
        w_u32(0x800FF26Cu, 0u);
        sub_8009B12C(0u, 0u, 0xFFFFFFFFu);
        return playback_resume_missing(480u);
    }
    return result;
}
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter indirect */
uint32 sub_8005F1B0(uint32 object)
{
    uint32 threshold, distance, numerator, denominator, damage, table, input, sound;
    if (!(r_u16(object + 216u) & 2u)) return 0u;
    threshold = r_u32(object + 484u);
    w_u8(object + 578u, 4u);
    distance = 0u;
    if (threshold)
        distance = (uint32)((sint32)(r_u32(object + 8u) - r_u32(object + 480u)) >> 12);
    if (threshold && (sint32)threshold < (sint32)distance)
    {
        numerator = (distance - threshold) * (uint32)(sint32)(sint16)r_u16(0x800EC526u);
        denominator = r_u32(object + 488u) - threshold;
        if (!denominator) damage = (sint32)numerator < 0 ? 1u : 0xFFFFFFFFu;
        else if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu) damage = numerator;
        else damage = (uint32)((sint32)numerator / (sint32)denominator);
        table = r_u32(object + 68u);
        apocalypse_object_virtual52(r_u32(table + 52u), object + (uint32)(sint32)(sint16)r_u16(table + 48u), damage, 0x800A71CCu, 0u);
        if ((sint16)r_u16(object + 218u) <= 0) return 1u;
    }
    else if (r_u32(0x800FF2FCu)) sub_8007011C(0u, 4u, 0u, 1u);
    sub_80063038(object, 2u, (uint32)(sint32)(sint16)r_u16(0x800EC4D6u) + 1u, 0xFFFFFFFFu);
    sound = (r_u16(object) & 8u) ? 56u : 19u;
    sub_80069DF0(sound, 0x2000u, 0u);
    input = r_u32(object + 444u);
    w_u32(object + 460u, 8u);
    w_u8(input + 273u, 0u);
    w_u8(object + 538u, 0u);
    w_u8(object + 537u, 0u);
    w_u32(0x800FF5A4u, 0u);
    return 1u;
}

void nullsub_25(void)
{
    ;
}

uint32 sub_80030168(uint32 a1)
{
    sint32 position_vector0[3], position_vector1[3];
    sint32 result;
    uint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    char v21[16];
    char v22[16];
    char v23[16];
    char v24[16];
    sint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    result = r_u32(0x800FF5A0u);
    v2 = a1;
    if ((a1 == ((uint32)(r_u32(0x800FF5A0u)))))
    {
        v3 = r_u32((a1 + (47) * 4u));
        v4 = r_u32((a1 + (48) * 4u));
        position_vector0[0] = r_u32((a1 + (46) * 4u));
        position_vector0[1] = v3;
        position_vector0[2] = v4;
        v25 = 2;
        xport_draft_host_sub_8006C564_p13(v21, (a1 + (147) * 4u), &v25);
        v26 = 2;
        xport_draft_host_sub_8006C564_p123(v23, &position_vector0[0], &v26);
        v27 = 3;
        xport_draft_host_sub_8006C47C_p123(v22, &v27, v23);
        result = xport_draft_host_sub_8006C34C_p123(&position_vector1[0], v21, v22);
        v5 = position_vector1[1];
        v6 = position_vector1[2];
        w_u32((v2 + (46) * 4u), position_vector1[0]);
        w_u32((v2 + (47) * 4u), v5);
        w_u32((v2 + (48) * 4u), v6);
        v7 = position_vector0[1];
        v8 = position_vector0[2];
        w_u32((v2 + (147) * 4u), position_vector0[0]);
        w_u32((v2 + (148) * 4u), v7);
        w_u32((v2 + (149) * 4u), v8);
    }
    while (v2)
    {
        v9 = r_u32((v2 + (2) * 4u));
        v10 = r_u32((v2 + (3) * 4u));
        position_vector0[0] = r_u32((v2 + (1) * 4u));
        position_vector0[1] = v9;
        position_vector0[2] = v10;
        v28 = 2;
        xport_draft_host_sub_8006C564_p13(v21, (v2 + (60) * 4u), &v28);
        v29 = 2;
        xport_draft_host_sub_8006C564_p123(v24, &position_vector0[0], &v29);
        v30 = 3;
        xport_draft_host_sub_8006C47C_p123(v22, &v30, v24);
        result = xport_draft_host_sub_8006C34C_p123(&position_vector1[0], v21, v22);
        v11 = position_vector1[1];
        v12 = position_vector1[2];
        w_u32((v2 + (1) * 4u), position_vector1[0]);
        w_u32((v2 + (2) * 4u), v11);
        w_u32((v2 + (3) * 4u), v12);
        v13 = position_vector0[1];
        v14 = position_vector0[2];
        w_u32((v2 + (60) * 4u), position_vector0[0]);
        w_u32((v2 + (61) * 4u), v13);
        w_u32((v2 + (62) * 4u), v14);
        v2 = ((uint32)(r_u32((v2 + (7) * 4u))));
    }

    return result;
}

void sub_800303F4(void)
{
    sint32 position_vector0[3], position_vector1[3];
    uint32 i;
    sint32 v1;
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    sint32 v22;
    sint32 v24;
    sint32 v25;
    char v32[16];
    char v33[16];
    char v34[16];
    char v35[16];
    sint32 v36;
    sint32 v37;
    sint32 v38;
    sint32 v39;
    sint32 v40;
    sint32 v41;
    sint32 v42;
    sint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    for (i = ((uint32)(r_u32(0x800FF444u))); i; i = ((uint32)(r_u32((i + (1) * 4u)))))
    {
        v1 = r_u32((i + (7) * 4u));
        v2 = r_u32((i + (8) * 4u));
        position_vector0[0] = r_u32((i + (6) * 4u));
        position_vector0[1] = v1;
        position_vector0[2] = v2;
        v36 = 2;
        xport_draft_host_sub_8006C564_p13(v32, (i + (3) * 4u), &v36);
        v37 = 2;
        xport_draft_host_sub_8006C564_p123(v34, &position_vector0[0], &v37);
        v38 = 3;
        xport_draft_host_sub_8006C47C_p123(v33, &v38, v34);
        xport_draft_host_sub_8006C34C_p123(&position_vector1[0], v32, v33);
        v3 = position_vector1[1];
        v4 = position_vector1[2];
        w_u32((i + (6) * 4u), position_vector1[0]);
        w_u32((i + (7) * 4u), v3);
        w_u32((i + (8) * 4u), v4);
        v5 = position_vector0[1];
        v6 = position_vector0[2];
        w_u32((i + (3) * 4u), position_vector0[0]);
        w_u32((i + (4) * 4u), v5);
        w_u32((i + (5) * 4u), v6);
        v7 = r_u32((i + (19) * 4u));
        v8 = r_u32((i + (20) * 4u));
        position_vector0[0] = r_u32((i + (18) * 4u));
        position_vector0[1] = v7;
        position_vector0[2] = v8;
        v39 = 2;
        xport_draft_host_sub_8006C564_p13(v32, (i + (27) * 4u), &v39);
        v40 = 2;
        xport_draft_host_sub_8006C564_p123(v35, &position_vector0[0], &v40);
        v41 = 3;
        xport_draft_host_sub_8006C47C_p123(v33, &v41, v35);
        xport_draft_host_sub_8006C34C_p123(&position_vector1[0], v32, v33);
        v9 = position_vector1[1];
        v10 = position_vector1[2];
        w_u32((i + (18) * 4u), position_vector1[0]);
        w_u32((i + (19) * 4u), v9);
        w_u32((i + (20) * 4u), v10);
        v11 = position_vector0[1];
        v12 = position_vector0[2];
        w_u32((i + (27) * 4u), position_vector0[0]);
        w_u32((i + (28) * 4u), v11);
        w_u32((i + (29) * 4u), v12);
        v13 = r_u32((i + (22) * 4u));
        v14 = r_u32((i + (23) * 4u));
        position_vector0[0] = r_u32((i + (21) * 4u));
        position_vector0[1] = v13;
        position_vector0[2] = v14;
        v42 = 2;
        xport_draft_host_sub_8006C564_p13(v32, (i + (30) * 4u), &v42);
        v43 = 2;
        xport_draft_host_sub_8006C564_p123(v34, &position_vector0[0], &v43);
        v44 = 3;
        xport_draft_host_sub_8006C47C_p123(v33, &v44, v34);
        xport_draft_host_sub_8006C34C_p123(&position_vector1[0], v32, v33);
        v15 = position_vector1[1];
        v16 = position_vector1[2];
        w_u32((i + (21) * 4u), position_vector1[0]);
        w_u32((i + (22) * 4u), v15);
        w_u32((i + (23) * 4u), v16);
        v17 = position_vector0[1];
        v18 = position_vector0[2];
        w_u32((i + (30) * 4u), position_vector0[0]);
        w_u32((i + (31) * 4u), v17);
        w_u32((i + (32) * 4u), v18);
        v19 = r_u32((i + (25) * 4u));
        v20 = r_u32((i + (26) * 4u));
        position_vector0[0] = r_u32((i + (24) * 4u));
        position_vector0[1] = v19;
        position_vector0[2] = v20;
        v45 = 2;
        xport_draft_host_sub_8006C564_p13(v32, (i + (33) * 4u), &v45);
        v46 = 2;
        xport_draft_host_sub_8006C564_p123(v34, &position_vector0[0], &v46);
        v47 = 3;
        xport_draft_host_sub_8006C47C_p123(v33, &v47, v34);
        xport_draft_host_sub_8006C34C_p123(&position_vector1[0], v32, v33);
        v21 = position_vector1[1];
        v22 = position_vector1[2];
        w_u32((i + (24) * 4u), position_vector1[0]);
        w_u32((i + (25) * 4u), v21);
        w_u32((i + (26) * 4u), v22);
        v24 = position_vector0[1];
        v25 = position_vector0[2];
        w_u32((i + (33) * 4u), position_vector0[0]);
        w_u32((i + (34) * 4u), v24);
        w_u32((i + (35) * 4u), v25);
    }

}

uint32 sub_80034FC4(uint32 a1)
{
    sint32 result;
    short v2;
    result = r_u32(((uint32)(a1)));
    v2 = r_u16(((uint32)((a1 + 4))));
    w_u32(0x800FF41Cu, r_u32(((uint32)(a1))));
    w_u16(0x800FF420u, v2);
    return result;
}

void sub_80035110(uint32 a1, uint32 a2, uint32 a3)
{
    w_u8(0x800FF430u, a1);
    w_u8(0x800FF431u, a2);
    w_u8(0x800FF432u, a3);
}

uint32 sub_80035124(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
    sint8 v13;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    short v18;
    sint32 v19;
    sub_80034EF4(((uint32)(a1)));
    v13 = r_u32(0x800FF424u);
    w_u32(((uint32)((a1 + 68))), 0x800A1C90u);
    v14 = (v13 & 0xF);
    if ((v14 == 1))
    {
        w_u8(((uint32)((a1 + 79))), 104);
        v15 = 0x2000000;
    }
    else
    {
        w_u8(((uint32)((a1 + 79))), 96);
        v14 = r_u32(0x800FF424u);
        v15 = 50331648;
    }
    w_u32(((uint32)((a1 + 72))), v15);
    w_u32(((uint32)((a1 + 80))), v14);
    w_u8(((uint32)((a1 + 76))), r_u8(0x800FF42Cu));
    w_u8(((uint32)((a1 + 77))), r_u8(0x800FF42Du));
    w_u8(((uint32)((a1 + 78))), r_u8(0x800FF42Eu));
    w_u8(((uint32)((a1 + 84))), r_u8(0x800FF430u));
    w_u8(((uint32)((a1 + 85))), r_u8(0x800FF431u));
    w_u8(((uint32)((a1 + 86))), r_u8(0x800FF432u));
    v16 = r_u32((a2 + (1) * 4u));
    v17 = r_u32((a2 + (2) * 4u));
    w_u32(((uint32)((a1 + 24))), r_u32(a2));
    w_u32(((uint32)((a1 + 28))), v16);
    w_u32(((uint32)((a1 + 32))), v17);
    sub_80034FEC((a1 + 36), a3);
    w_u32(((uint32)((a1 + 52))), a4);
    w_u8(((uint32)((a1 + 60))), 3);
    w_u8(((uint32)((a1 + 61))), 3);
    w_u8(((uint32)((a1 + 62))), 3);
    v18 = sub_80066570(a5);
    v19 = (r_u32(0x800FF428u) == 0);
    w_u16(((uint32)((a1 + 10))), v18);
    if (!v19)
        w_u8(((uint32)((a1 + 79))), (r_u8(((uint32)((a1 + 79)))) | (2u)));
    return a1;
}

/* TODO Missing call adapter SHIWORD */
uint32 sub_80034FEC(uint32 output, uint32 scale)
{
    sint16 angles[3];
    uint32 axis, random, range;
    angles[0] = (sint16)r_u16(0x800FF414u);
    angles[1] = (sint16)r_u16(0x800FF416u);
    angles[2] = (sint16)r_u16(0x800FF418u);
    for (axis = 0u; axis < 3u; ++axis)
    {
        range = (uint32)(sint32)(sint16)r_u16(0x800FF41Cu + axis * 2u);
        if (range)
        {
            random = sub_80066570(range);
            range = (uint32)(sint32)(sint16)r_u16(0x800FF41Cu + axis * 2u);
            angles[axis] = (sint16)((uint32)(uint16)angles[axis] + random - (uint32)((sint32)range >> 1));
        }
    }
    return xport_draft_host_sub_800667CC_p3(output, scale, angles);
}

uint32 sub_8004B8C8(uint32 a1)
{
    sint32 result;
    short v3;
    result = sub_80062F48(a1);
    if (!result)
    {
        sub_8004B948(a1);
        result = (((unsigned short)(r_u16((a1 + (39) * 2u)))) | 0x40);
        v3 = (r_u16(a1) | 1);
        w_u16((a1 + (39) * 2u), result);
        w_u16(a1, v3);
    }
    return result;
}

/* TODO Missing call adapter indirect */
uint32 sub_8004D6B4(uint32 object, uint32 reason)
{
    uint32 child, table, index = 0u;
    w_u32(object + 68u, 0x800A2AC0u);
    sub_80062A64(object, 0x800FF4E8u);
    child = r_u32(object + 512u);
    if (child) sub_8006A294(child);
    child = r_u32(object + 496u);
    if (child)
    {
        table = r_u32(child);
        (void)apocalypse_object_cleanup(r_u32(table + 12u), child + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
    }
    child = r_u32(object + 508u);
    if (child)
    {
        table = r_u32(child + 68u);
        (void)apocalypse_object_cleanup(r_u32(table + 12u), child + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
    }
    if (r_u32(object + 504u))
    {
        while ((sint32)index < (sint32)r_u16(object + 500u))
        {
            child = r_u32(r_u32(object + 504u) + index * 16u + 12u);
            if (child)
            {
                table = r_u32(child + 68u);
                (void)apocalypse_object_cleanup(r_u32(table + 12u), child + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
            }
            ++index;
        }
        sub_8006BC20(r_u32(object + 504u));
    }
    sub_8004B868(object, 0u);
    return (reason & 1u) ? sub_80062608(object) : 0u;
}

uint32 sub_8004B868(uint32 a1, uint32 a2)
{
    sint32 v4;
    sint32 result;
    v4 = r_u32(0x800FF4D8u);
    w_u32(((uint32)((a1 + 68))), 0x800A2B18u);
    w_u32(0x800FF4D8u, (v4 - 1));
    result = sub_80062FB8(a1, 0);
    if (((a2 & 1) != 0))
        return sub_80062608(a1);
    return result;
}

uint32 sub_80062650(uint32 a1, uint32 a2)
{
    sint32 v4;
    sint32 result;
    v4 = (r_u16(((uint32)(a1))) & 0x400);
    w_u32(((uint32)((a1 + 68))), 0x800A33C0u);
    if (v4)
        sub_8001BB14(((sint32)((0x800F2610u + ((6 * r_u8(((uint32)((a1 + 25)))))) * 4u))));
    result = (a2 & 1);
    if (((a2 & 1) != 0))
        return sub_80062608(a1);
    return result;
}

uint32 sub_8005EC7C(uint32 a1)
{
    sint32 result;
    sint32 v3;
    short v4;
    sint32 v5;
    result = 0;
    if (r_u32(((uint32)((a1 + 520)))))
    {
        if ((r_u8(((uint32)((a1 + 471)))) | r_u8(((uint32)((a1 + 472))))))
        {
            w_u32(((uint32)((a1 + 112))), 0);
            w_u32(((uint32)((a1 + 108))), 0);
            w_u32(((uint32)((a1 + 104))), 0);
            w_u32(((uint32)((a1 + 124))), 0);
            w_u32(((uint32)((a1 + 120))), 0);
            v3 = r_u32(((uint32)((a1 + 520))));
            w_u32(((uint32)((a1 + 116))), 0);
            v4 = r_u16(((uint32)((v3 + 20))));
            w_u32(((uint32)((a1 + 16))), r_u32(((uint32)((v3 + 16)))));
            w_u16(((uint32)((a1 + 20))), v4);
            sub_80063038(a1, 18, 0, -1);
            v5 = r_u32(((uint32)((a1 + 520))));
            w_u32(((uint32)((a1 + 460))), 0x20000);
            sub_80062034(v5);
            return 1;
        }
        else
        {
            return 0;
        }
    }
    return result;
}

/* TODO Missing call adapter indirect */
void sub_80022ABC(uint32 object)
{
    uint32 child = r_u32(object + 32u), table;
    if (child)
    {
        table = r_u32(child + 68u);
        (void)apocalypse_object_cleanup(r_u32(table + 12u), child + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
    }
    w_u32(object + 32u, 0u);
}

uint32 sub_80034A18(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    sint32 result;
    result = 838860800;
    w_u32(((uint32)((a1 + 88))), ((((a4 << 16) | (a3 << 8)) | 0x32000000) | a2));
    return result;
}

uint32 sub_8001D114(uint32 a1)
{
    uint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 result;
    v2 = 0;
    if (r_u32(((uint32)((a1 + 80)))))
    {
        v3 = 0;
        do
        {
            if (((v2 & 1) != 0))
            {
                v4 = sub_80066570(((sint16)(r_u16(((uint32)((a1 + 112)))))));
                v5 = ((sint16)(r_u16(((uint32)((a1 + 110))))));
            }
            else
            {
                v4 = sub_80066570(((sint16)(r_u16(((uint32)((a1 + 116)))))));
                v5 = ((sint16)(r_u16(((uint32)((a1 + 114))))));
            }
            w_u32(((uint32)((v3 + r_u32(((uint32)((a1 + 72))))))), (v5 + v4));
            ++v2;
            v3 += 8;
        } while ((v2 < r_u32(((uint32)((a1 + 80))))));
    }
    if ((((sint16)(r_u16(((uint32)((a1 + 110)))))) >= ((sint16)(r_u16(((uint32)((a1 + 118))))))))
        w_u16(((uint32)((a1 + 110))), (r_u16(((uint32)((a1 + 110)))) - (r_u16(((uint32)((a1 + 118)))))));
    if ((((sint16)(r_u16(((uint32)((a1 + 112)))))) >= ((sint16)(r_u16(((uint32)((a1 + 118))))))))
        w_u16(((uint32)((a1 + 112))), (r_u16(((uint32)((a1 + 112)))) - (r_u16(((uint32)((a1 + 118)))))));
    if ((((sint16)(r_u16(((uint32)((a1 + 114)))))) >= ((sint16)(r_u16(((uint32)((a1 + 118))))))))
        w_u16(((uint32)((a1 + 114))), (r_u16(((uint32)((a1 + 114)))) - (r_u16(((uint32)((a1 + 118)))))));
    if ((((sint16)(r_u16(((uint32)((a1 + 116)))))) >= ((sint16)(r_u16(((uint32)((a1 + 118))))))))
        w_u16(((uint32)((a1 + 116))), (r_u16(((uint32)((a1 + 116)))) - (r_u16(((uint32)((a1 + 118)))))));
    result = sub_80066570(1024);
    w_u16(((uint32)((a1 + 96))), result);
    return result;
}

uint32 sub_8002374C(uint32 a1, uint32 a2, uint32 a3)
{
    return apocalypse_projectile_construct_2374C(a1, a2, (const sint16 *)psx_addr(a3, 6u));
}

uint32 apocalypse_projectile_construct_2374C(uint32 a1, uint32 a2, const sint16 input_rotation[3])
{
    sint32 position_vector0[3], position_vector1[3];
    sint16 angles[3];
    sint32 v6;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    uint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    uint32 result;
    char v15[16];
    sub_80032068(a1, 1);
    w_u32((a1 + (17) * 4u), 0x800A1518u);
    w_u8(((uint32)((r_u32((a1 + (20) * 4u)) + 12))), 64);
    w_u8(((uint32)((r_u32((a1 + (20) * 4u)) + 13))), 64);
    w_u8(((uint32)((r_u32((a1 + (20) * 4u)) + 14))), 30);
    xport_draft_host_sub_800667CC_p3((a1 + (24) * 4u), 3, (void *)input_rotation);
    xport_draft_host_sub_800667CC_p13(v15, (uint32)-45, input_rotation);
    xport_draft_host_sub_8006C34C_p13(position_vector0, a2, v15);
    v6 = position_vector0[1];
    v7 = position_vector0[2];
    w_u32((a1 + (6) * 4u), position_vector0[0]);
    w_u32((a1 + (7) * 4u), v6);
    w_u32((a1 + (8) * 4u), v7);
    xport_draft_host_sub_8006C34C_p1(position_vector0, (a1 + (6) * 4u), (a1 + (24) * 4u));
    v8 = position_vector0[1];
    v9 = position_vector0[2];
    w_u32((a1 + (21) * 4u), position_vector0[0]);
    w_u32((a1 + (22) * 4u), v8);
    w_u32((a1 + (23) * 4u), v9);
    v10 = ((uint32)(r_u32((a1 + (20) * 4u))));
    xport_draft_host_sub_8006C3AC_p1(position_vector0, (a1 + (6) * 4u), (a1 + (24) * 4u));
    v11 = position_vector0[1];
    v12 = position_vector0[2];
    w_u32(v10, position_vector0[0]);
    w_u32((v10 + (1) * 4u), v11);
    w_u32((v10 + (2) * 4u), v12);
    memcpy(angles, input_rotation, 4u);
    memcpy(angles + 2, input_rotation + 2, 2u);
    angles[1] = (sint16)((uint16)angles[1] + 1024u);
    v13 = sub_80066570(4);
    xport_draft_host_sub_800667CC_p3(a1 + 36, v13 + 10, angles);
    w_u32((a1 + (10) * 4u), (r_u32((a1 + (10) * 4u)) - (40960)));
    xport_draft_host_sub_800667CC_p13(position_vector1, 145, angles);
    xport_draft_host_sub_8006C0B8_p1(position_vector1, a2);
    w_u32(0x800ED638u, position_vector1[0]);
    w_u32(0x800ED63Cu, position_vector1[1]);
    w_u32(0x800ED640u, position_vector1[2]);
    w_u32(0x800ED644u, position_vector1[0]);
    w_u32(0x800ED64Cu, position_vector1[2]);
    w_u32(0x800ED648u, (position_vector1[1] + 4096000));
    sub_8007BB24(0x800ED638u);
    sub_8007DD04(0x800ED638u, 1);
    if (r_u32(0x800ED6A0u))
        w_u32((a1 + (27) * 4u), r_u32(0x800ED6A8u));
    else
        w_u32((a1 + (27) * 4u), 0x7FFFFFFF);
    result = a1;
    w_u32((a1 + (28) * 4u), r_u32(0x800FF650u));
    return result;
}

/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_800855B4(uint32 input, uint32 output)
{
    uint32 first = r_u32(input), second = r_u32(input + 4u);
    uint32 fourth = r_u32(input + 12u), third, fifth, values[3][3], axis;
    xport_gte_write_data(9u, first);
    xport_gte_write_data(10u, second >> 16);
    xport_gte_write_data(11u, fourth);
    xport_gte_execute(0x4DE012u);
    third = r_u32(input + 8u);
    for (axis = 0u; axis < 3u; ++axis)
        values[0][axis] = xport_gte_read_data(25u + axis);
    xport_gte_write_data(9u, first >> 16);
    xport_gte_write_data(10u, third);
    xport_gte_write_data(11u, fourth >> 16);
    xport_gte_execute(0x4DE012u);
    fifth = r_u32(input + 16u);
    for (axis = 0u; axis < 3u; ++axis)
        values[1][axis] = xport_gte_read_data(25u + axis);
    xport_gte_write_data(9u, second);
    xport_gte_write_data(10u, third >> 16);
    xport_gte_write_data(11u, fifth);
    xport_gte_execute(0x4DE012u);
    w_u32(output, (values[0][0] & 0xFFFFu) | (values[1][0] << 16));
    w_u32(output + 12u, (values[0][2] & 0xFFFFu) | (values[1][2] << 16));
    for (axis = 0u; axis < 3u; ++axis)
        values[2][axis] = xport_gte_read_data(25u + axis);
    w_u32(output + 4u, (values[2][0] & 0xFFFFu) | (values[0][1] << 16));
    w_u32(output + 8u, (values[1][1] & 0xFFFFu) | (values[2][1] << 16));
    w_u32(output + 16u, values[2][2]);
}

uint32 sub_8006C190(uint32 a1, uint32 a2)
{
    uint32 result;
    result = a1;
    w_u32(a1, (r_u32(a1) / (((sint32)(r_u32(a2))))));
    w_u32((a1 + (1) * 4u), (r_u32((a1 + (1) * 4u)) / (((sint32)(r_u32(a2))))));
    w_u32((a1 + (2) * 4u), (r_u32((a1 + (2) * 4u)) / (((sint32)(r_u32(a2))))));
    return result;
}

uint32 sub_80035478(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7)
{
    return apocalypse_effect_construct_35478(a1, psx_addr(a2, 12u), a3, a4, a5, a6, a7);
}

uint32 apocalypse_effect_construct_35478(uint32 a1, const void *position, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7)
{
    uint32 first;
    sint32 v15;
    sint32 v16;
    sub_80034D88(a1);
    w_u32(((uint32)((a1 + 68))), 0x800A1C78u);
    memcpy(&first, position, 4u);
    memcpy(&v15, (const uint8 *)position + 4u, 4u);
    memcpy(&v16, (const uint8 *)position + 8u, 4u);
    w_u32(((uint32)((a1 + 24))), first);
    w_u32(((uint32)((a1 + 28))), v15);
    w_u32(((uint32)((a1 + 32))), v16);
    sub_80033398(a1, a3);
    w_u16(((uint32)((a1 + 94))), a4);
    if (a5)
        sub_800332A4(a1);
    w_u32(((uint32)((a1 + 112))), a6);
    if ((a7 == 0xFFFFFFFFu))
        w_u32(((uint32)((a1 + 116))), (r_u8(((uint32)((a1 + 89)))) - 1));
    else
        w_u32(((uint32)((a1 + 116))), a7);
    return a1;
}

uint32 sub_8003319C(uint32 a1)
{
    sint32 result;
    sub_80032F7C(a1);
    result = a1;
    w_u32(((uint32)((a1 + 68))), 0x800A1D68u);
    w_u16(((uint32)((a1 + 92))), 128);
    w_u16(((uint32)((a1 + 94))), 400);
    w_u32(((uint32)((a1 + 76))), 746619008);
    return result;
}

uint32 sub_800332FC(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    uint32 result;
    result = ((((r_u32(((uint32)((a1 + 76)))) & 0xFF000000) | (a4 << 16)) | (a3 << 8)) | a2);
    w_u32(((uint32)((a1 + 76))), result);
    return result;
}

uint32 sub_80035A54(uint32 a1)
{
    sint32 v2;
    sint32 v3;
    sint8 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    short v17;
    sint32 result;
    v2 = r_u32(((uint32)((a1 + 28))));
    v3 = r_u32(((uint32)((a1 + 40))));
    v4 = r_u8(((uint32)((a1 + 61))));
    w_u32(((uint32)((a1 + 24))), (r_u32(((uint32)((a1 + 24)))) + (r_u32(((uint32)((a1 + 36)))))));
    v5 = r_u32(((uint32)((a1 + 32))));
    v6 = r_u32(((uint32)((a1 + 44))));
    w_u32(((uint32)((a1 + 28))), ((uint32)v2 + (uint32)v3));
    v7 = r_u32(((uint32)((a1 + 36))));
    v8 = r_u32(((uint32)((a1 + 48))));
    w_u32(((uint32)((a1 + 32))), ((uint32)v5 + (uint32)v6));
    v9 = r_u32(((uint32)((a1 + 40))));
    v10 = r_u32(((uint32)((a1 + 52))));
    w_u32(((uint32)((a1 + 36))), ((uint32)v7 + (uint32)v8));
    v11 = (sint32)((uint32)v9 + (uint32)v10);
    w_u32(((uint32)((a1 + 44))), (r_u32(((uint32)((a1 + 44)))) + (r_u32(((uint32)((a1 + 56)))))));
    v7 = ((v7 & 0xFFFFFF00u) | (((r_u8(((uint32)((a1 + 60))))) & 0xFFu) << 0));
    v10 = ((v10 & 0xFFFFFF00u) | (((r_u8(((uint32)((a1 + 90))))) & 0xFFu) << 0));
    w_u32(((uint32)((a1 + 40))), v11);
    v12 = r_u32(((uint32)((a1 + 40))));
    v10 = ((v10 & 0xFFFFFF00u) | ((((v10 + 1)) & 0xFFu) << 0));
    v13 = (sint32)(r_u32(a1 + 36u) - (uint32)((sint32)r_u32(a1 + 36u) >> ((uint32)v7 & 31u)));
    v7 = ((v7 & 0xFFFFFF00u) | (((r_u8(((uint32)((a1 + 62))))) & 0xFFu) << 0));
    w_u32(((uint32)((a1 + 36))), v13);
    v14 = r_u32(((uint32)((a1 + 44))));
    w_u8(((uint32)((a1 + 90))), v10);
    w_u32(a1 + 44u, (uint32)v14 - (uint32)(v14 >> ((uint32)v7 & 31u)));
    v15 = r_u8(((uint32)((a1 + 89))));
    w_u32(a1 + 40u, (uint32)v12 - (uint32)(v12 >> ((uint32)v4 & 31u)));
    if ((((sint8)(v10)) >= v15))
        w_u8(((uint32)((a1 + 90))), (v15 - 1));
    v16 = r_u16(((uint32)((a1 + 10))));
    v17 = (r_u16(((uint32)((a1 + 8)))) + 1);
    w_u16(((uint32)((a1 + 8))), v17);
    result = (v17 < v16);
    if (!result)
        return sub_80032ED8(a1);
    return result;
}

/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_80082254(uint32 geometry)
{
    uint32 x = r_u32(geometry + 0x1A4u);
    uint32 y = r_u32(geometry + 0x1B4u);
    uint32 z = r_u32(geometry + 0x1C4u);
    xport_draft_gte_data_write(9, x);
    xport_draft_gte_data_write(10, y);
    xport_draft_gte_data_write(11, z);
}

/* TODO 64-bit guest field width remains TODO */
/* TODO Missing call adapter SHIDWORD */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* TODO 64-bit guest field width remains TODO */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_800850A4(uint32 line)
{
    uint32 camera = r_u32(0x800FFB0Cu), camera_position[3], first[3], second[3];
    uint32 first_xy, second_xy, first_flags, second_flags, packed, z, carrier, result, axis;
    uint32 planes[6], rotation[5], low = 0u, high = 4096u, mask, cursor;
    sint32 dx, dy;
    camera_position[0] = r_u32(camera + 4u);
    camera_position[1] = r_u32(camera + 8u);
    camera_position[2] = r_u32(camera + 12u);
    packed = ((uint32)((sint32)r_u32(line) >> 12) - camera_position[0]) & 0xFFFFu;
    packed |= ((uint32)((sint32)r_u32(line + 4u) >> 12) - camera_position[1]) << 16;
    z = (uint32)((sint32)r_u32(line + 8u) >> 12) - camera_position[2];
    xport_gte_write_data(0u, packed); xport_gte_write_data(1u, z);
    xport_gte_execute(0x180001u);
    packed = ((uint32)((sint32)r_u32(line + 12u) >> 12) - camera_position[0]) & 0xFFFFu;
    packed |= ((uint32)((sint32)r_u32(line + 16u) >> 12) - camera_position[1]) << 16;
    z = (uint32)((sint32)r_u32(line + 20u) >> 12) - camera_position[2];
    first_flags = xport_draft_gte_control_read(31u); first_xy = xport_gte_read_data(14u);
    for (axis = 0u; axis < 3u; ++axis) first[axis] = xport_gte_read_data(25u + axis);
    xport_gte_write_data(0u, packed); xport_gte_write_data(1u, z);
    xport_gte_execute(0x180001u);
    w_u32(line + 24u, first_xy); w_u32(line + 40u, first[2]);
    w_u32(line + 32u, 0u); w_u32(line + 36u, 4096u);
    second_flags = xport_draft_gte_control_read(31u); second_xy = xport_gte_read_data(14u);
    for (axis = 0u; axis < 3u; ++axis) second[axis] = xport_gte_read_data(25u + axis);
    carrier = first_flags | second_flags;
    if ((sint32)carrier >= 0)
    {
        dx = (sint16)(second_xy - first_xy); carrier = (uint32)dx;
        if (dx < 1024 && dx >= -1023)
        {
            dy = ((sint32)second_xy >> 16) - ((sint32)first_xy >> 16);
            carrier = (uint32)dy;
            if (dy < 512 && dy >= -511)
            {
                w_u32(line + 28u, second_xy); w_u32(line + 44u, second[2]);
                result = (sint32)first[2] < (sint32)second[2] ? first[2] >> 2 : second[2] >> 2;
                return (sint32)(result - 4096u) < 0 ? result : 4095u;
            }
        }
    }
    if (((carrier >> 22) & 7u) || (sint32)(first[2] & second[2]) < 0) return 0xFFFFFFFFu;
    xport_gte_write_data(0u, (first[0] & 0xFFFFu) | (first[1] << 16));
    xport_gte_write_data(1u, first[2]);
    xport_gte_write_data(2u, (second[0] & 0xFFFFu) | (second[1] << 16));
    xport_gte_write_data(3u, second[2]);
    for (axis = 0u; axis < 5u; ++axis) planes[axis] = r_u32(0x800F25D0u + axis * 4u);
    planes[5] = r_u16(0x800F25E4u);
    xport_gte_write_control(8u, planes[0]);
    xport_gte_write_control(9u, (planes[2] << 16) | (planes[1] & 0xFFFFu));
    xport_gte_write_control(10u, (planes[2] >> 16) | (planes[3] << 16));
    xport_gte_write_control(11u, planes[4]); xport_gte_write_control(12u, planes[5]);
    xport_gte_write_control(13u, (uint32)((sint32)planes[1] >> 16));
    xport_gte_write_control(14u, (uint32)((sint32)planes[3] >> 16)); xport_gte_write_control(15u, 0u);
    planes[0] = r_u32(0x800F25E8u); planes[1] = r_u16(0x800F25ECu);
    planes[2] = r_u32(0x800F2600u); planes[3] = r_u16(0x800F2604u);
    planes[4] = r_u32(0x800F2608u); planes[5] = r_u16(0x800F260Cu);
    xport_gte_write_control(16u, planes[0]);
    xport_gte_write_control(17u, (planes[2] << 16) | planes[1]);
    xport_gte_write_control(18u, (planes[2] >> 16) | (planes[3] << 16));
    xport_gte_write_control(19u, planes[4]); xport_gte_write_control(20u, planes[5]);
    xport_gte_execute(0x4A2412u); first_flags = (xport_draft_gte_control_read(31u) >> 19) & 0x38u;
    for (axis = 0u; axis < 3u; ++axis) w_u32(line + 48u + axis * 4u, xport_gte_read_data(25u + axis));
    xport_gte_execute(0x4C6412u); first_flags |= (xport_draft_gte_control_read(31u) >> 22) & 7u;
    for (axis = 0u; axis < 3u; ++axis) w_u32(line + 60u + axis * 4u, xport_gte_read_data(25u + axis));
    xport_gte_execute(0x4AA412u); second_flags = (xport_draft_gte_control_read(31u) >> 19) & 0x38u;
    for (axis = 0u; axis < 3u; ++axis) w_u32(line + 72u + axis * 4u, xport_gte_read_data(25u + axis));
    xport_gte_execute(0x4CE412u); second_flags |= (xport_draft_gte_control_read(31u) >> 22) & 7u;
    for (axis = 0u; axis < 3u; ++axis) w_u32(line + 84u + axis * 4u, xport_gte_read_data(25u + axis));
    if ((first_flags & second_flags) || ((first_flags | second_flags) & 0x20u)) return 0xFFFFFFFFu;
    cursor = line;
    for (mask = 32u; mask; mask >>= 1, cursor += 4u)
    {
        if ((first_flags ^ second_flags) & mask)
        {
            uint32 distance = r_u32(cursor + 48u), other = r_u32(cursor + 72u);
            uint32 parameter = vector_signed_quotient(distance << 12, distance - other);
            if (first_flags & mask)
            {
                if ((sint32)(parameter - low) > 0) low = parameter;
            }
            else if ((sint32)(parameter - high) < 0) high = parameter;
        }
    }
    if ((sint32)(low - high) >= 0) return 0xFFFFFFFFu;
    w_u32(line + 32u, low); w_u32(line + 36u, high);
    for (axis = 0u; axis < 5u; ++axis) rotation[axis] = xport_draft_gte_control_read(axis);
    packed = (xport_gte_read_data(0u) << 3) & 0xFFF8FFF8u;
    first[2] = xport_gte_read_data(1u) << 3;
    z = (xport_gte_read_data(2u) << 3) & 0xFFF8FFF8u;
    second[2] = xport_gte_read_data(3u) << 3;
    xport_gte_write_control(0u, packed); xport_gte_write_control(2u, z);
    xport_gte_write_control(1u, packed ^ ((packed ^ z) & 0xFFFFu));
    xport_gte_write_control(3u, first[2]); xport_gte_write_control(4u, second[2]);
    xport_gte_write_data(0u, 4096u - low); xport_gte_write_data(1u, low);
    xport_gte_execute(0x180001u);
    first_xy = xport_gte_read_data(14u); first[2] = xport_gte_read_data(11u);
    xport_gte_write_data(0u, 4096u - high); xport_gte_write_data(1u, high);
    first[2] >>= 3;
    xport_gte_execute(0x180001u);
    w_u32(line + 24u, first_xy); w_u32(line + 40u, first[2]);
    second_xy = xport_gte_read_data(14u); second[2] = xport_gte_read_data(11u);
    w_u32(line + 28u, second_xy); second[2] >>= 3; w_u32(line + 44u, second[2]);
    for (axis = 0u; axis < 5u; ++axis) xport_gte_write_control(axis, rotation[axis]);
    result = (sint32)first[2] < (sint32)second[2] ? first[2] >> 2 : second[2] >> 2;
    return (sint32)(result - 4096u) < 0 ? result : 4095u;
}

uint32 sub_800348A8(uint32 a1, uint32 a2)
{
    sint8 v3;
    sint32 v4;
    uint32 result;
    sint32 v6;
    v3 = a2;
    v4 = r_u32((a1 + (18) * 4u));
    w_u32((a1 + (17) * 4u), 0x800A1D08u);
    sub_8006BC20(v4);
    sub_80032E7C(a1, 0x800FF450u);
    result = sub_80032FB8(((sint32)(a1)), 0);
    if (((v3 & 1) != 0))
        return ((uint32)(sub_80032E30(((sint32)(a1)))));
    return result;
}

uint32 sub_800331EC(uint32 a1, uint32 a2)
{
    uint32 result;
    sint32 v5;
    w_u32(((uint32)((a1 + 68))), 0x800A1D68u);
    result = sub_80032FB8(a1, 0);
    if (((a2 & 1) != 0))
        return ((uint32)(sub_80032E30(a1)));
    return result;
}

/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter sub_800878DC */
static void polygon_vertex(uint32 output[3], uint32 geometry, uint32 indices, uint32 index)
{
    uint32 input = geometry + (r_u16(indices + index * 2u) & 0xFFF8u);
    uint32 axis;
    sint32 shift = 12;
    xport_draft_gte_data_write(0u, r_u32(input));
    xport_draft_gte_data_write(1u, r_u32(input + 4u));
    xport_draft_gte_execute(0x480012u);
    for (axis = 0u; axis < 3u; ++axis)
        output[axis] = xport_draft_gte_data_read(25u + axis);
    xport_draft_host_sub_8006C22C_p12(output, &shift);
}
uint32 sub_80021CA8(uint32 object, uint32 polygon, uint32 parameter, uint32 effect, uint32 test, uint32 mark, uint32 render)
{
    uint32 table = 0x800EAEF8u + ((uint32)r_u8(object + 27u) << 6);
    uint32 model = r_u32(r_u32(table + 16u) + (uint32)r_u16(object + 22u) * 4u);
    uint32 geometry = model + 32u;
    uint32 normals = geometry + r_u32(model + 4u) * 8u;
    uint32 payload = 0u, flags, count, axis, vertex, result = 1u;
    uint32 matrix[8], vertices[4][3];
    if (test)
    {
        uint32 touched = r_u16(polygon + 18u);
        if (!(touched & 8u) || (touched & 1u)) return 0u;
        if (mark)
        {
            flags = r_u16(polygon);
            w_u16(polygon + 18u, touched | 1u);
            w_u16(polygon, flags & 0xFE7Fu);
            if (!render && ((flags & 0x40u) || !(flags & 1u)))
                w_u16(polygon, flags & 0xFE3Fu);
        }
    }
    if (!render) return 1u;
    flags = r_u16(polygon);
    if (flags & 1u)
    {
        payload = flags & 2u ? polygon + 20u : r_u32(polygon + 20u);
        w_u32(0x800FFB78u, r_u16(payload + 2u));
        w_u32(0x800FFB7Cu, r_u16(payload + 6u));
    }
    w_u32(0x800FF210u, normals + (r_u16(polygon + 16u) & 0xFFF8u));
    xport_draft_host_sub_800858FC_p2(object + 16u, matrix);
    xport_draft_host_sub_800878DC_p1(matrix);
    sub_80084504(object + 4u);
    for (vertex = 0u; vertex < 3u; ++vertex)
        polygon_vertex(vertices[vertex], geometry, polygon + 4u, vertex);
    flags = r_u16(polygon);
    count = flags & 0x10u ? 3u : 4u;
    if (count == 4u) polygon_vertex(vertices[3], geometry, polygon + 4u, 3u);
    table = 0x800EAEF8u + ((uint32)r_u8(object + 27u) << 6);
    sub_80021B3C(r_u16(polygon) & 0x800u, r_u32(polygon + 12u), count, r_u32(table + 32u));
    for (axis = 0u; axis < 3u; ++axis)
    {
        uint32 sum = 0u;
        sint32 center;
        for (vertex = 0u; vertex < count; ++vertex)
            sum += (uint32)((sint32)vertices[vertex][axis] >> 12);
        center = count == 3u ? (sint32)sum / 3 : (sint32)sum >> 2;
        w_u32(0x800FFD58u + axis * 4u, (uint32)center << 12);
    }
    flags = r_u16(polygon);
    if (!(flags & 0x40u) && (flags & 1u))
    {
        uint32 texture = flags & 0x20u ? r_u32(payload + 12u) : 0u;
        if (!r_u32(0x800FF738u))
        {
            xport_draft_host_sub_80021358_p123(vertices[0], vertices[1], vertices[2], r_u8(payload), r_u8(payload + 1u), r_u8(payload + 4u), r_u8(payload + 5u), r_u8(payload + 8u), r_u8(payload + 9u), texture, parameter);
            if (count == 4u)
                xport_draft_host_sub_80021358_p123(vertices[1], vertices[2], vertices[3], r_u8(payload + 4u), r_u8(payload + 5u), r_u8(payload + 8u), r_u8(payload + 9u), r_u8(payload + 10u), r_u8(payload + 11u), texture, parameter);
        }
    }
    else
    {
        if (!r_u32(0x800FF738u))
        {
            uint32 offset[3], address = r_u32(0x800FF210u);
            uint32 red, green, blue;
            for (axis = 0u; axis < 3u; ++axis)
                offset[axis] = (uint32)(sint32)(sint16)r_u16(address + axis * 2u);
            result = 2u;
            red = r_u8(0x800FFB80u);
            blue = r_u8(0x800FFB82u);
            green = count == 3u ? r_u8(0x800FFB81u) : blue;
            xport_draft_host_sub_80022454_p2345(count == 3u ? 15u : 30u, vertices[0], vertices[1], vertices[2], offset, red, green, blue);
        }
        if (mark) w_u16(polygon, r_u16(polygon) & 0xFFBFu);
    }
    if (effect && !r_u32(0x800FF738u))
        sub_8001D320(0x800FFD58u, 100u, r_u8(0x800FFB80u), r_u8(0x800FFB81u), r_u8(0x800FFB82u), 4u, 1u, 100u);
    return result;
}
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_80021ADC(uint32 output, uint32 geometry, uint32 indices, uint32 index)
{
    uint32 input = geometry + (r_u16(indices + index * 2u) & 0xFFF8u);
    uint32 axis;
    sint32 shift = 12;
    xport_draft_gte_data_write(0u, r_u32(input));
    xport_draft_gte_data_write(1u, r_u32(input + 4u));
    xport_draft_gte_execute(0x480012u);
    for (axis = 0u; axis < 3u; ++axis)
        w_u32(output + axis * 4u, xport_draft_gte_data_read(25u + axis));
    return xport_draft_host_sub_8006C22C_p2(output, &shift);
}
/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_800344BC(uint32 object)
{
    uint32 matrix[8], vertex, axis;
    xport_draft_host_sub_800858FC_p2(object + 152u, matrix);
    xport_draft_host_sub_800878DC_p1(matrix);
    sub_80084504(object + 24u);
    for (vertex = 0u; vertex < 4u; ++vertex)
    {
        uint32 input = object + 72u + 8u * vertex;
        uint32 output = object + 104u + 12u * vertex;
        xport_draft_gte_data_write(0u, r_u32(input));
        xport_draft_gte_data_write(1u, r_u32(input + 4u));
        xport_draft_gte_execute(0x480012u);
        for (axis = 0u; axis < 3u; ++axis)
            w_u32(output + axis * 4u, xport_draft_gte_data_read(25u + axis));
    }
    return object + 96u;
}
uint32 sub_80034314(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 result;
    v5 = a4;
    v6 = a3;
    v7 = a2;
    w_u32((a1 + (43) * 4u), (((a4 << 16) | (a3 << 8)) | a2));
    v8 = sub_80066570(4096);
    w_u32((a1 + (44) * 4u), (((((v5 * v8) >> 12) << 16) | (((v6 * v8) >> 12) << 8)) | ((v7 * v8) >> 12)));
    v9 = sub_80066570(4096);
    w_u32((a1 + (45) * 4u), (((((v5 * v9) >> 12) << 16) | (((v6 * v9) >> 12) << 8)) | ((v7 * v9) >> 12)));
    v10 = sub_80066570(4096);
    v11 = ((((v5 * v10) >> 12) << 16) | (((v6 * v10) >> 12) << 8));
    result = ((v7 * v10) >> 12);
    w_u32((a1 + (46) * 4u), (v11 | result));
    return result;
}

/* TODO Resolve original data label 0x800FF3F8u */
uint32 sub_80036B00(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6)
{
    sint32 v12;
    sint8 v13;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 vars0;
    sint32 vars4;
    short vars8;
    sint32 varsC;
    sub_80036634(((sint32)(a1)), a2, a3, 2, ((sint32)(0x800FF3F8u)), ((sint32)(0x800FF3F8u)), 400, 1);
    v12 = r_u32((a1 + (18) * 4u));
    v13 = a4;
    v14 = (a4 / v12);
    v15 = (a5 / v12);
    v16 = 0;
    w_u32((a1 + (17) * 4u), 0x800A1BB0u);
    v17 = (a6 / v12);
    while (1)
    {
        v18 = r_u32((a1 + (18) * 4u));
        v19 = (v16 + 1);
        if ((v16 >= v18))
            break;
        v20 = (4 * v16);
        sub_800332FC(r_u32(((uint32)((v20 + r_u32((a1 + (23) * 4u)))))), (v13 - ((v18 - v19) * v14)), (a5 - ((v18 - v19) * v15)), (a6 - ((v18 - v19) * v17)));
        sub_8003334C(r_u32(((uint32)((v20 + r_u32((a1 + (23) * 4u)))))), 8);
        v16 = v19;
    }

    return a1;
}

uint32 sub_8003658C(uint32 a1)
{
    sub_80034E4C(a1);
    w_u32((a1 + (17) * 4u), 0x800A1BE0u);
    return a1;
}

uint32 sub_80034E4C(uint32 a1)
{
    sub_8003319C(((sint32)(a1)));
    w_u32((a1 + (17) * 4u), 0x800A1CC0u);
    sub_80032E50(a1, 0x800FF43Cu);
    return a1;
}

uint32 sub_80033354(uint32 a1, uint32 a2)
{
    sint32 v3;
    sint32 result;
    sint32 v5;
    v3 = sub_8006F164(a2);
    w_u32(((uint32)((a1 + 80))), v3);
    result = r_u8(((uint32)((v3 - 4))));
    v5 = r_u32(((uint32)((a1 + 80))));
    w_u8(((uint32)((a1 + 90))), 0);
    w_u8(((uint32)((a1 + 91))), 0);
    w_u8(((uint32)((a1 + 89))), result);
    w_u32(((uint32)((a1 + 84))), v5);
    return result;
}

void sub_80033290(uint32 a1, uint32 a2)
{
    w_u16(((uint32)((a1 + 94))), a2);
}





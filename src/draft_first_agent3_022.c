#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Native collision records retain the original 140-byte field layout */
static uint32 collision_read32(const void *record, uint32 offset)
{
    uint32 value;
    memcpy(&value, (const uint8 *)record + offset, 4u);
    return value;
}

static void collision_write32(void *record, uint32 offset, uint32 value)
{
    memcpy((uint8 *)record + offset, &value, 4u);
}

static void collision_write16(void *record, uint32 offset, uint16 value)
{
    memcpy((uint8 *)record + offset, &value, 2u);
}

static uint32 collision_leading_bits(uint32 value)
{
    uint32 count = 0u;
    uint32 test = (value & 0x80000000u) ? ~value : value;
    if (!test)
        return 32u;
    while (!(test & 0x80000000u))
    {
        ++count;
        test <<= 1;
    }
    return count;
}

static sint32 collision_divide(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator >= 0 ? -1 : 1;
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
        return (sint32)numerator;
    return (sint32)numerator / (sint32)denominator;
}

static uint32 collision_missing(const char *operation, uint32 object, void *record)
{
    fprintf(stderr, "TODO Collision %s object %08X record %p\n", operation, object, record);
    abort();
}

static uint32 collision_init_record(void *record, uint32 guest_address)
{
    sint16 dx, dy, dz;
    uint32 horizontal, shift, normalization;
    sint32 horizontal_length, length, sine, cosine;
    MATRIX rotation;
    uint32 axis;
    collision_write32(record, 64u, 0x7FFFFFFFu);
    collision_write32(record, 104u, 0u);
    collision_write32(record, 132u, 0xFFFFFFFFu);
    ((uint8 *)record)[136] = 0;
    ((uint8 *)record)[137] = 0;
    dx = (sint16)((sint32)(collision_read32(record, 12u) - collision_read32(record, 0u)) >> 12);
    dy = (sint16)((sint32)(collision_read32(record, 16u) - collision_read32(record, 4u)) >> 12);
    dz = (sint16)((sint32)(collision_read32(record, 20u) - collision_read32(record, 8u)) >> 12);
    w_u16(0x800FFA3Cu, dx);
    w_u16(0x800FFA3Eu, dy);
    w_u16(0x800FFA40u, dz);
    horizontal = (uint32)((sint32)dx * dx) + (uint32)((sint32)dz * dz);
    if (horizontal)
    {
        normalization = collision_leading_bits(horizontal);
        shift = (uint32)((sint32)(normalization - 1u) >> 1);
        horizontal_length = (sint32)sub_80085B54(horizontal << ((normalization - 1u) & 30u));
        sine = collision_divide((uint32)(sint32)dx << ((shift + 12u) & 31u), horizontal_length);
        cosine = collision_divide((uint32)(sint32)dz << ((shift + 12u) & 31u), horizontal_length);
        w_u16(0x800FFA2Eu, sine);
        w_u16(0x800FFA2Cu, cosine);
        w_u16(0x800ED740u, cosine);
        w_u16(0x800ED742u, 0u);
        w_u16(0x800ED744u, 0u - sine);
        w_u16(0x800ED746u, 0u);
        w_u16(0x800ED748u, 4096u);
        w_u16(0x800ED74Au, 0u);
        w_u16(0x800ED74Cu, sine);
        w_u16(0x800ED74Eu, 0u);
        w_u16(0x800ED750u, cosine);
        horizontal += (uint32)((sint32)dy * dy);
        normalization = collision_leading_bits(horizontal);
        length = (sint32)sub_80085B54(horizontal << ((normalization - 1u) & 30u));
        cosine = collision_divide((uint32)horizontal_length << 12, length);
        sine = collision_divide((uint32)(sint32)dy << ((shift + 12u) & 31u), length);
        collision_write32(record, 68u, (uint32)(length >> (shift & 31u)));
        w_u16(0x800FFA2Cu, cosine);
        w_u16(0x800FFA2Eu, sine);
        rotation.m[0][0] = 4096; rotation.m[0][1] = 0; rotation.m[0][2] = 0;
        rotation.m[1][0] = 0; rotation.m[1][1] = (sint16)cosine; rotation.m[1][2] = (sint16)(0u - sine);
        rotation.m[2][0] = 0; rotation.m[2][1] = (sint16)sine; rotation.m[2][2] = (sint16)cosine;
        MulMatrix(&rotation, (MATRIX *)psx_addr(0x800ED740u, sizeof(MATRIX)));
    }
    else
    {
        sine = dy >= 0 ? 4096 : -4096;
        w_u16(0x800FFA2Eu, sine);
        collision_write32(record, 68u, dy >= 0 ? (uint32)(sint32)dy : 0u - (uint32)(sint32)dy);
        if (dy >= 0)
            ((uint8 *)record)[137] = 1;
        rotation.m[0][0] = 4096; rotation.m[0][1] = 0; rotation.m[0][2] = 0;
        rotation.m[1][0] = 0; rotation.m[1][1] = 0; rotation.m[1][2] = (sint16)(0u - sine);
        rotation.m[2][0] = 0; rotation.m[2][1] = (sint16)sine; rotation.m[2][2] = 0;
    }
    memcpy((uint8 *)record + 72u, rotation.m, 18u);
    SetRotMatrix(&rotation);
    if (guest_address)
    {
        w_u32(0x800FFA68u, guest_address);
        w_u32(0x800FFA6Cu, guest_address + 12u);
    }
    for (axis = 0; axis < 3u; ++axis)
    {
        sint32 first = (sint32)collision_read32(record, axis * 4u);
        sint32 second = (sint32)collision_read32(record, axis * 4u + 12u);
        collision_write32(record, 24u + axis * 4u, first < second ? first : second);
        collision_write32(record, 36u + axis * 4u, first < second ? second : first);
    }
    sub_8007BAB0();
    collision_write16(record, 138u, r_u16(0x800FF968u));
    return r_u16(0x800FF968u);
}

uint32 apocalypse_collision_init(void *record)
{
    return collision_init_record(record, 0u);
}

uint32 sub_8007BB24(uint32 record)
{
    return collision_init_record(psx_addr(record, 140u), record);
}

uint32 sub_8007DD04(uint32 record, uint32 include_objects)
{
    return apocalypse_collision_query(psx_addr(record, 140u), include_objects);
}

uint32 apocalypse_collision_query(void *a1, uint32 a2)
{
    uint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    uint32 v8;
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
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    uint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    sint32 v35;
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
    sint32 v49;
    sint32 v50;
    uint32 v51;
    sint32 v52;
    sint32 v53;
    sint32 v54;
    sint32 v55;
    sint32 v56;
    sint32 v57;
    sint32 v58;
    w_u32(0x800FF988u, 0);
    w_u32(0x800FF984u, -1);
    if (r_u32(0x800FF974u))
        w_u32(0x800FF988u, 0x400000);
    if (!r_u32(0x800FF970u))
        w_u32(0x800FF988u, (r_u32(0x800FF988u) ^ (0x200000u)));
    if (r_u32(0x800FF96Cu))
        w_u32(0x800FF984u, -1048577);
    if (r_u32(0x800FF978u))
        w_u32(0x800FF984u, (r_u32(0x800FF984u) ^ (0x20000u)));
    if (a2)
        apocalypse_collision_objects(r_u32(0x800FF5DCu), a1);
    v51 = 0x800EDA30u;
    v52 = 0;
    do
    {
        v2 = (v51 + (408) * 4u);
        if (!(((sint32)(r_u32(v51)))))
            goto LABEL_106;
        v3 = ((sint32)(collision_read32(a1, 0u)));
        v4 = ((sint32)(collision_read32(a1, 12u)));
        v5 = ((sint32)(collision_read32(a1, 8u)));
        v6 = ((sint32)(r_u32((v51 + (1) * 4u))));
        v7 = ((sint32)(collision_read32(a1, 20u)));
        v8 = v51;
        v9 = ((sint32)(r_u32((v51 + (3) * 4u))));
        v10 = ((sint32)(r_u32((v51 + (2) * 4u))));
        v11 = ((sint32)(r_u32((v51 + (4) * 4u))));
        v12 = ((sint32)(r_u32((v51 + (5) * 4u))));
        if (((((sint32)(collision_read32(a1, 0u))) < v6) && (v4 < v6)))
            goto LABEL_105;
        if (((((v9 < v3) && (v9 < v4)) || ((v5 < v10) && (v7 < v10))) || ((v11 < v5) && (v11 < v7))))
            goto LABEL_104;
        if (((v3 == v4) && (v5 == v7)))
        {
            v13 = ((v3 - v6) / v12);
            v14 = ((v5 - v10) / v12);
            if ((v13 == ((sint16)(r_u16((((uint32)(v51)) + (14) * 2u))))))
                --v13;
            v15 = v14;
            if ((v14 != ((sint16)(r_u16((((uint32)(v51)) + (15) * 2u))))))
                goto LABEL_103;
            v16 = (v14 - 1);
        }
        else
        {
            if ((v3 < v6))
            {
                if ((v5 >= v7))
                    v17 = (v7 + sub_80085BA4((v4 - v6), (v5 - v7), (v4 - v3)));
                else
                    v17 = (v5 + sub_80085BA4((v6 - v3), (v7 - v5), (v4 - v3)));
                v5 = v17;
                v3 = v6;
            }
            if ((v4 < v6))
            {
                if ((v7 >= v5))
                    v18 = (v5 + sub_80085BA4((v3 - v6), (v7 - v5), (v3 - v4)));
                else
                    v18 = (v7 + sub_80085BA4((v6 - v4), (v5 - v7), (v3 - v4)));
                v7 = v18;
                v4 = v6;
            }
            if ((v9 < v3))
            {
                if ((v7 >= v5))
                    v19 = (v5 + sub_80085BA4((v3 - v9), (v7 - v5), (v3 - v4)));
                else
                    v19 = (v7 + sub_80085BA4((v9 - v4), (v5 - v7), (v3 - v4)));
                v5 = v19;
                v3 = v9;
            }
            if ((v9 < v4))
            {
                if ((v5 >= v7))
                    v20 = (v7 + sub_80085BA4((v4 - v9), (v5 - v7), (v4 - v3)));
                else
                    v20 = (v5 + sub_80085BA4((v9 - v3), (v7 - v5), (v4 - v3)));
                v7 = v20;
                v4 = v9;
            }
            if ((v5 < v10))
            {
                if ((v3 >= v4))
                    v21 = (v4 + sub_80085BA4((v7 - v10), (v3 - v4), (v7 - v5)));
                else
                    v21 = (v3 + sub_80085BA4((v10 - v5), (v4 - v3), (v7 - v5)));
                v3 = v21;
                v5 = v10;
            }
            if ((v7 < v10))
            {
                if ((v4 >= v3))
                    v22 = (v3 + sub_80085BA4((v5 - v10), (v4 - v3), (v5 - v7)));
                else
                    v22 = (v4 + sub_80085BA4((v10 - v7), (v3 - v4), (v5 - v7)));
                v4 = v22;
                v7 = v10;
            }
            if ((v11 < v5))
            {
                if ((v4 >= v3))
                    v23 = (v3 + sub_80085BA4((v5 - v11), (v4 - v3), (v5 - v7)));
                else
                    v23 = (v4 + sub_80085BA4((v11 - v7), (v3 - v4), (v5 - v7)));
                v3 = v23;
                v5 = v11;
            }
            if ((v11 < v7))
            {
                if ((v3 >= v4))
                    v24 = (v4 + sub_80085BA4((v7 - v11), (v3 - v4), (v7 - v5)));
                else
                    v24 = (v3 + sub_80085BA4((v11 - v5), (v4 - v3), (v7 - v5)));
                v4 = v24;
                v7 = v11;
            }
            v25 = (v4 - v3);
            if (((v4 - v3) >= 0))
            {
                v49 = 1;
            }
            else
            {
                v49 = -1;
                v25 = (v3 - v4);
            }
            v26 = (v7 - v5);
            if (((v7 - v5) >= 0))
            {
                v50 = 1;
            }
            else
            {
                v50 = -1;
                v26 = (v5 - v7);
            }
            v27 = ((v4 - v6) / v12);
            v28 = ((v3 - v6) / v12);
            v29 = ((v3 - v6) % v12);
            v16 = ((v5 - v10) / v12);
            v30 = ((v5 - v10) % v12);
            v31 = v51;
            v32 = ((v7 - v10) / v12);
            v33 = v29;
            v34 = ((sint16)(r_u16((((uint32)(v51)) + (14) * 2u))));
            v35 = v30;
            if ((v28 == v34))
            {
                --v28;
                v33 += v12;
                v31 = v51;
            }
            v36 = ((sint16)(r_u16((((uint32)(v31)) + (15) * 2u))));
            if ((v16 == v36))
            {
                --v16;
                v35 = (v30 + v12);
            }
            if ((v27 == v34))
                --v27;
            if ((v32 == v36))
                --v32;
            v53 = v26;
            v55 = v27;
            v57 = v25;
            v37 = sub_80085BA4(v25, v35, v12);
            v38 = v12;
            v39 = v37;
            v40 = sub_80085BA4(v53, v33, v38);
            v41 = v53;
            v42 = v55;
            v43 = v57;
            v13 = v28;
            if ((v3 >= v4))
                v44 = -v40;
            else
                v44 = (v40 - v53);
            if ((v5 >= v7))
                v45 = (v44 + v39);
            else
                v45 = ((v44 + v57) - v39);
            v46 = v45;
            while (((v13 != v42) || (v16 != v32)))
            {
                if ((v13 < 0))
                    goto LABEL_104;
                v8 = v51;
                if (((v13 >= ((sint16)(r_u16((((uint32)(v51)) + (14) * 2u))))) || (v16 < 0)))
                    break;
                if ((v16 >= ((sint16)(r_u16((((uint32)(v51)) + (15) * 2u))))))
                    goto LABEL_99;
                v54 = v41;
                v56 = v42;
                v58 = v43;
                apocalypse_collision_cells(((uint32)(r_u32((0x800EDA30u + (((((20 * v13) + 8) + v16) + v52)) * 4u)))), a1);
                v41 = v54;
                v42 = v56;
                v43 = v58;
                if ((v46 < 0))
                {
                    v46 += v58;
                    v16 += v50;
                }
                else
                {
                    v46 -= v54;
                    v13 += v49;
                }
            }

            if ((v13 < 0))
                goto LABEL_104;
            v8 = v51;
        LABEL_99:
            if (((v13 >= ((sint16)(r_u16((((uint32)(v8)) + (14) * 2u))))) || (v16 < 0)))
                goto LABEL_105;

            v47 = (v16 >= ((sint16)(r_u16((((uint32)(v8)) + (15) * 2u)))));
            v2 = (v8 + (408) * 4u);
            if (v47)
                goto LABEL_106;
        }
        v15 = v16;
    LABEL_103:
        apocalypse_collision_cells(((uint32)(r_u32((0x800EDA30u + (((((20 * v13) + 8) + v15) + v52)) * 4u)))), a1);

    LABEL_104:
        v8 = v51;

    LABEL_105:
        v2 = (v8 + (408) * 4u);

    LABEL_106:
        v51 = v2;

        v52 += 408;
    } while ((((sint32)(v2)) < ((sint32)(0x800EE6F0u))));
    return apocalypse_collision_finish(a1);
}

/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* Unverified native translation of the original pipelined collision vertex transform */
uint32 apocalypse_collision_transform(uint32 geometry, uint32 output, uint32 depth_limit, const sint32 translation[3])
{
    uint32 remaining = r_u32(geometry + 4u), vertex = geometry + 32u;
    uint32 packed = r_u32(vertex), zword = r_u32(vertex + 4u);
    uint32 x = packed + (uint32)translation[0];
    uint32 y = (uint32)((sint32)packed >> 16) + (uint32)translation[1];
    uint32 z = zword + (uint32)translation[2];
    uint32 result = 0x60Fu, mask;
    sint32 transformed_x, transformed_y, transformed_z;
    do
    {
        xport_draft_gte_data_write(9u, x);
        xport_draft_gte_data_write(10u, y);
        xport_draft_gte_data_write(11u, z);
        vertex += 8u;
        --remaining;
        xport_draft_gte_execute(0x4D8012u);
        /* The original prefetches the next vertex before storing this result */
        packed = r_u32(vertex);
        zword = r_u32(vertex + 4u);
        x = packed + (uint32)translation[0];
        y = (uint32)((sint32)packed >> 16) + (uint32)translation[1];
        z = zword + (uint32)translation[2];
        transformed_x = (sint32)xport_draft_gte_data_read(25u);
        transformed_y = (sint32)xport_draft_gte_data_read(26u);
        transformed_z = (sint32)xport_draft_gte_data_read(27u);
        mask = ((uint32)(transformed_x < 0) << 2)
             | ((uint32)(transformed_y < 0) << 1)
             | (uint32)(transformed_z < 0)
             | ((uint32)((sint32)depth_limit < transformed_z) << 3)
             | ((uint32)(transformed_x > 0) << 10)
             | ((uint32)(transformed_y > 0) << 9);
        w_u32(output, ((uint32)transformed_x & 0xFFFFu) | ((uint32)transformed_y << 16));
        w_u32(output + 4u, (uint32)transformed_z);
        output += 8u;
        result &= mask;
        w_u16(output - 2u, mask);
    } while (remaining);
    return result;
}

uint32 sub_80084D4C(uint32 geometry, uint32 output, uint32 depth_limit, uint32 translation)
{
    sint32 vector[3];
    vector[0] = (sint32)r_u32(translation);
    vector[1] = (sint32)r_u32(translation + 4u);
    vector[2] = (sint32)r_u32(translation + 8u);
    return apocalypse_collision_transform(geometry, output, depth_limit, vector);
}

uint32 apocalypse_collision_polygons(uint32 geometry, uint32 output, void *record, uint32 object)
{
    uint32 required, rejected, count, normals, polygon, header, flags, stride;
    uint32 vertices[4], packed[3], y[3], cross0, cross1, cross2;
    uint32 normal, start, numerator, endpoint, denominator, distance, direction, axis;
    xport_draft_gte_control_write(11u, 0u);
    xport_draft_gte_control_write(12u, collision_read32(record, 68u));
    required = r_u32(0x800FF984u);
    rejected = r_u32(0x800FF988u);
    count = r_u32(geometry + 12u);
    normals = geometry + 32u + (r_u32(geometry + 4u) << 3);
    polygon = normals + (r_u32(geometry + 8u) << 3);
    do
    {
        flags = r_u32(polygon + 16u);
        header = r_u32(polygon);
        if (!(flags & rejected) && (flags | required) == 0xFFFFFFFFu && ((flags >> 16) & 3u) != 1u)
        {
            packed[0] = r_u32(polygon + 4u);
            packed[1] = r_u32(polygon + 8u);
            vertices[0] = output + (packed[0] & 0xFFFFu);
            vertices[1] = output + (packed[0] >> 16);
            vertices[2] = output + (packed[1] & 0xFFFFu);
            vertices[3] = output + (packed[1] >> 16);
            if (!(r_u16(vertices[0] + 6u) & r_u16(vertices[1] + 6u) & r_u16(vertices[2] + 6u) & r_u16(vertices[3] + 6u) & 0x60Fu))
            {
                for (axis = 0u; axis < 3u; ++axis)
                {
                    packed[axis] = r_u32(vertices[axis]);
                    y[axis] = (uint32)((sint32)packed[axis] >> 16);
                }
                for (axis = 0u; axis < 3u; ++axis)
                    xport_draft_gte_control_write(axis * 2u, packed[axis]);
                for (axis = 0u; axis < 3u; ++axis)
                    xport_draft_gte_data_write(9u + axis, y[axis]);
                xport_draft_gte_execute(0x170000Cu);
                cross0 = xport_draft_gte_data_read(25u);
                cross1 = xport_draft_gte_data_read(26u);
                cross2 = xport_draft_gte_data_read(27u);
                if ((sint32)cross0 < 0)
                {
                    if (header & 0x10u)
                        goto next_polygon;
                    packed[0] = r_u32(vertices[3]);
                    xport_draft_gte_data_write(10u, y[1]);
                    xport_draft_gte_data_write(11u, y[2]);
                    xport_draft_gte_control_write(0u, packed[0]);
                    xport_draft_gte_data_write(9u, (uint32)((sint32)packed[0] >> 16));
                    xport_draft_gte_execute(0x170000Cu);
                    cross1 = 0u - xport_draft_gte_data_read(26u);
                    cross2 = 0u - xport_draft_gte_data_read(27u);
                }
                if ((sint32)(cross1 | cross2) >= 0)
                {
                    normal = normals + (flags & 0xFFFFu);
                    xport_draft_gte_data_write(0u, r_u32(normal));
                    xport_draft_gte_data_write(1u, r_u32(normal + 4u));
                    start = r_u32(vertices[0]);
                    endpoint = r_u16(vertices[0] + 4u);
                    xport_draft_gte_execute(0x4C6012u);
                    xport_draft_gte_control_write(8u, start);
                    xport_draft_gte_control_write(9u, endpoint);
                    denominator = xport_draft_gte_data_read(11u);
                    xport_draft_gte_execute(0x43E012u);
                    numerator = xport_draft_gte_data_read(25u);
                    endpoint = xport_draft_gte_data_read(27u);
                    if ((sint32)numerator <= 0 && (sint32)numerator >= (sint32)endpoint)
                    {
                        distance = (uint32)collision_divide(numerator, denominator);
                        if (flags & 0x20000u)
                        {
                            if (*((const uint8 *)record + 136u))
                                w_u32(0x800FFA88u, r_u16(object + 22u));
                        }
                        else if ((sint32)(collision_read32(record, 64u) - distance) > 0)
                        {
                            collision_write32(record, 104u, object);
                            collision_write32(record, 64u, distance);
                            collision_write32(record, 128u, polygon);
                            collision_write32(record, 132u, endpoint);
                            for (axis = 0u; axis < 3u; ++axis)
                                xport_draft_gte_data_write(25u + axis, collision_read32(record, axis * 4u));
                            direction = collision_read32(record, 84u);
                            endpoint = collision_read32(record, 88u);
                            xport_draft_gte_data_write(9u, direction);
                            xport_draft_gte_data_write(10u, direction >> 16);
                            xport_draft_gte_data_write(11u, endpoint);
                            xport_draft_gte_data_write(8u, distance);
                            xport_draft_gte_execute(0x1A0003Eu);
                            for (axis = 0u; axis < 3u; ++axis)
                                collision_write32(record, 108u + axis * 4u, xport_draft_gte_data_read(25u + axis));
                            w_u32(0x800ED61Cu, normal);
                            w_u32(0x800ED618u, polygon);
                        }
                    }
                }
            }
        }
    next_polygon:
        stride = header >> 16;
        polygon += stride;
        --count;
    } while (count);
    return stride;
}

uint32 sub_80084E24(uint32 geometry, uint32 output, uint32 guest_record, uint32 object)
{
    FUNCTION_MARKER(0x80084E24u, "SLUS_003.73");
    return apocalypse_collision_polygons(geometry, output, psx_addr(guest_record, 140u), object);
}

/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 apocalypse_collision_finish(void *record)
{
    uint32 object = collision_read32(record, 104u);
    uint32 normal;
    if (!object)
        return 0u;
    RotMatrix((SVECTOR *)psx_addr(object + 16u, 6u), (MATRIX *)psx_addr(0x800ED720u, sizeof(MATRIX)));
    SetRotMatrix((MATRIX *)psx_addr(0x800ED720u, sizeof(MATRIX)));
    normal = r_u32(0x800ED61Cu);
    xport_gte_write_data(0u, r_u32(normal));
    xport_gte_write_data(1u, r_u32(normal + 4u));
    xport_gte_execute(0x486012u);
    collision_write16(record, 120u, (uint16)xport_gte_read_data(9u));
    collision_write16(record, 122u, (uint16)xport_gte_read_data(10u));
    collision_write16(record, 124u, (uint16)xport_gte_read_data(11u));
    return 1u;
}

uint32 sub_8007BFD4(uint32 record)
{
    return apocalypse_collision_finish(psx_addr(record, 140u));
}

uint32 sub_8004CFDC(uint32 a1, uint32 a2)
{
    uint32 p, value, result, index;
    a2 &= 0xFFFFu;
    if (a2 - 8448u >= 67u)
        return 0u;
    switch (a2)
    {
        case 8501:
            p = (r_u32(a1 + 400u) + 3u) & 0xFFFFFFFCu;
            value = r_u32(p);
            w_u32(a1 + 400u, p + 4u);
            w_u32(a1 + 480u, value);
            return p + 4u;
        case 8495:
            p = (r_u32(a1 + 400u) + 3u) & 0xFFFFFFFCu;
            result = sub_8006E080(r_u32(p), r_u8(a1 + 27u));
            w_u16(a1 + 22u, result);
            w_u32(a1 + 400u, p + 4u);
            return result;
        case 8512:
        case 8513:
        case 8514:
            result = (uint32)((sint32)(sub_8004CF78(a1) << 16) >> 4);
            w_u32(a1 + 4u + (a2 - 8512u) * 4u, result);
            return result;
        case 8497:
            p = r_u32(a1 + 400u);
            value = r_u16(p);
            w_u32(a1 + 400u, p + 2u);
            result = r_u16(a1);
            result = value ? result | 8u : result & 0xFFF7u;
            w_u16(a1, result);
            return result;
        case 8480:
            p = r_u32(a1 + 400u);
            index = r_u16(p) & 0xFFu;
            w_u32(a1 + 400u, p + 2u);
            result = sub_8004CF78(a1);
            w_u16(a1 + 460u + index * 2u, result);
            return result;
        case 8485:
            p = r_u32(a1 + 400u);
            index = r_u16(p);
            p += 2u;
            w_u32(a1 + 400u, p);
            value = r_u16(p);
            w_u16(a1 + 436u + index * 2u, value);
            w_u32(a1 + 400u, p + 2u);
            return p + 2u;
        case 8486:
            p = r_u32(a1 + 400u);
            index = r_u16(p) & 0xFFu;
            p += 2u;
            w_u32(a1 + 400u, p);
            value = r_u16(p) & 0xFFu;
            w_u32(a1 + 400u, p + 2u);
            result = r_u16(p + 2u);
            w_u32(a1 + 400u, p + 4u);
            return sub_8004BDCC(a1, index, value, result);
        case 8448:
        case 8449:
        case 8468:
        case 8469:
        case 8481:
        case 8482:
        case 8492:
        case 8493:
        case 8494:
            p = r_u32(a1 + 400u);
            value = (a2 == 8492u || a2 == 8493u) ? r_u8(p) : r_u16(p);
            w_u32(a1 + 400u, p + 2u);
            switch (a2)
            {
                case 8448:
                    w_u16(a1 + 218u, value);
                    break;
                case 8449:
                    w_u16(a1 + 212u, value);
                    break;
                case 8468:
                    w_u16(a1 + 472u, value);
                    break;
                case 8469:
                    w_u16(a1 + 474u, value);
                    break;
                case 8481:
                    w_u32(a1 + 304u, (uint32)(sint32)(sint16)value);
                    break;
                case 8482:
                    w_u16(a1 + 456u, value);
                    break;
                case 8492:
                    w_u8(a1 + 384u, value);
                    break;
                case 8493:
                    w_u8(a1 + 383u, value);
                    break;
                case 8494:
                    w_u16(a1 + 392u, value);
                    break;
            }
            return p + 2u;
        default:
            return 0x8004D2A0u;
    }
}

/* TODO Missing call adapter indirect */
uint32 sub_8004CF78(uint32 object)
{
    uint32 stream = r_u32(object + 400u);
    uint32 command = r_u16(stream);
    w_u32(object + 400u, stream + 2u);
    if ((command & 0x8000u) == 0u && (command & 0x2000u) != 0u)
    {
        uint32 table = r_u32(object + 68u);
        uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80u);
        command = xport_draft_guest_call2(r_u32(table + 84u), receiver, command);
    }
    return command & 0xFFFFu;
}

/* TODO Missing call adapter HIDWORD */
/* TODO Missing call adapter LODWORD */
/* TODO Missing call adapter SHIDWORD */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter indirect */
/* TODO Connect native host-buffer boundaries when exercised */
static void command_4BF3C_missing(const char *name)
{
    fprintf(stderr, "TODO 8004BF3C boundary: %s\n", name);
    abort();
}

static void command_4BF3C_writer(uint32 target, uint32 receiver, uint32 command, const uint16 words[8], uint32 count)
{
    (void)target; (void)receiver; (void)command; (void)words; (void)count;
    command_4BF3C_missing("Virtual writer with native command words");
}

static void command_4BF3C_angles(const sint16 angles[3])
{
    (void)angles;
    command_4BF3C_missing("80034FC4 native angles");
}

static void command_4BF3C_spawn(uint32 object, const uint32 position[3], uint16 value, uint32 enabled, uint16 argument5)
{
    (void)object; (void)position; (void)value; (void)enabled; (void)argument5;
    command_4BF3C_missing("8001DB68 native position");
}

uint32 sub_8002005C(uint32 object, uint32 velocity, uint32 mode);
void sub_8002EF58(void);

static uint32 command_4BF3C_word(uint32 object)
{
    uint32 cursor = r_u32(object + 400u);
    uint32 value = r_u16(cursor);
    w_u32(object + 400u, cursor + 2u);
    return value;
}

static uint32 command_4BF3C_get(uint32 object, uint32 value)
{
    uint32 table = r_u32(object + 68u);
    return xport_draft_guest_call2(r_u32(table + 84u), object + (uint32)(sint32)(sint16)r_u16(table + 80u), value);
}

static uint32 command_4BF3C_value(uint32 object)
{
    uint32 value = command_4BF3C_word(object);
    return (value & 0x2000u) ? command_4BF3C_get(object, value) & 0xFFFFu : value;
}

static sint32 command_4BF3C_signed_value(uint32 object)
{
    uint32 value = command_4BF3C_word(object);
    return (value & 0x2000u) ? (sint16)command_4BF3C_get(object, value) : (sint32)value;
}

uint32 sub_8004BF3C(uint32 object, uint32 opcode)
{
    uint32 value, second, third, cursor, saved, table, created, count, i;
    uint32 position[3];
    uint16 words[8];
    sint16 angles[3];
    sint32 left, right;
    opcode &= 0xFFFFu;
    switch (opcode)
    {
    case 0x4101: case 0x4102:
        value = command_4BF3C_word(object);
        w_u32(object + 400u, r_u32(object + value * 4u + 404u));
        return opcode == 0x4101;
    case 0x4104:
        value = command_4BF3C_word(object);
        w_u32(object + value * 4u + 404u, r_u32(object + 400u));
        return 1u;
    case 0x4105:
        saved = r_u32(object + 400u);
        while (r_u16(r_u32(object + 400u)) != 0x4100u)
        {
            value = command_4BF3C_word(object);
            if (value == 0x4104u)
            {
                value = command_4BF3C_word(object);
                w_u32(object + value * 4u + 404u, r_u32(object + 400u));
            }
        }
        w_u32(object + 400u, saved);
        return 1u;
    case 0x4106:
        value = command_4BF3C_value(object);
        cursor = xport_draft_host_sub_8006613C_p1(position, value) + 6u;
        w_u32(object + 400u, cursor);
        return 1u;
    case 0x4107: return 0u;
    case 0x4110: case 0x4111:
        value = command_4BF3C_word(object);
        cursor = r_u32(object + 400u);
        left = (sint16)command_4BF3C_get(object, value);
        count = 0u;
        while (cursor != r_u32(object + 400u))
        {
            if (count >= 7u) command_4BF3C_missing("TODO Command words exceed original 16-byte local");
            words[count++] = r_u16(cursor);
            cursor += 2u;
        }
        right = command_4BF3C_signed_value(object);
        words[count] = (uint16)(opcode == 0x4110u ? left + right : left - right);
        saved = r_u32(object + 400u);
        table = r_u32(object + 68u);
        command_4BF3C_writer(r_u32(table + 76u), object + (uint32)(sint32)(sint16)r_u16(table + 72u), value, words, count + 1u);
        w_u32(object + 400u, saved);
        return 1u;
    case 0x4112: case 0x4113: case 0x4114:
        left = command_4BF3C_signed_value(object);
        right = command_4BF3C_signed_value(object);
        if ((opcode == 0x4112u && !(right < left)) || (opcode == 0x4113u && !(left < right)) || (opcode == 0x4114u && left != right))
            sub_8004BE30(object);
        return 1u;
    case 0x4115: case 0x4116:
        value = command_4BF3C_word(object);
        second = r_u32(object + 396u) & value;
        if ((opcode == 0x4115u && second != value) || (opcode == 0x4116u && second != 0u)) sub_8004BE30(object);
        return 1u;
    case 0x4200:
        sub_800626C8(object, r_u32(object + 400u));
        goto skip_string;
    case 0x4201:
        value = command_4BF3C_word(object); second = command_4BF3C_word(object);
        sub_80063118(object, value, (uint32)(sint32)(sint8)second);
        return 1u;
    case 0x4202:
        value = command_4BF3C_word(object); sub_80063038(object, value, 0u, 0xFFFFFFFFu); return 1u;
    case 0x4203: w_u16(object, r_u16(object) & 0xFFFEu); return 1u;
    case 0x4204: w_u16(object, r_u16(object) | 1u); return 1u;
    case 0x4205:
        table = r_u32(object + 68u);
        apocalypse_object_virtual20(r_u32(table + 20u), object + (uint32)(sint32)(sint16)r_u16(table + 16u)); return 1u;
    case 0x4226:
        w_u32(object + 112u, 0u); w_u32(object + 108u, 0u); w_u32(object + 104u, 0u); return 1u;
    case 0x4227:
        value = command_4BF3C_word(object);
        if (value) { w_u8(object + 386u, value); w_u8(object + 387u, 0u); w_u32(object + 396u, r_u32(object + 396u) | 2u); }
        else w_u32(object + 396u, r_u32(object + 396u) & ~2u);
        return 1u;
    case 0x4240: w_u8(object + 380u, 0u); return 0u;
    case 0x4260: case 0x4261:
        value = command_4BF3C_word(object); sub_8004BDCC(object, value & 0xFFu, 0u, opcode == 0x4260u); return 1u;
    case 0x4280:
        value = command_4BF3C_word(object); w_u16(object + 476u, value);
        if (value & 0x2000u) w_u16(object + 476u, command_4BF3C_get(object, r_u16(object + 476u)));
        return 0u;
    case 0x4281:
        if (r_u16(object + 388u) & 1u) return 1u;
        w_u32(object + 400u, r_u32(object + 400u) - 2u); return 0u;
    case 0x4290:
        value = command_4BF3C_word(object); sub_80069DF0((uint32)(sint32)(sint16)value, 0x2000u, 0u); return 1u;
    case 0x4291:
        value = command_4BF3C_word(object); sub_80069EF4((uint32)(sint32)(sint16)value, object + 4u, 0u); return 1u;
    case 0x4292:
        count = command_4BF3C_word(object);
        if ((sint32)r_u32(0x800FF3A8u) >= 200) return 1u;
        sub_80034F9C(object + 16u);
        angles[0] = 512; angles[1] = 4096; angles[2] = 0;
        command_4BF3C_angles(angles);
        sub_800350E8(1u); sub_800350FC(128u, 128u, 128u); sub_80035110(4u, 4u, 4u);
        w_u32(0x800FF3ACu, 0u);
        for (i = 0u; i < count; ++i) { created = sub_80032DC0(88u); if (created) sub_80035124(created, object + 4u, 32u, 0x2000u, 32u); }
        w_u32(0x800FF3ACu, 1u); return 1u;
    case 0x4293:
        cursor = (r_u32(object + 400u) + 3u) & ~3u;
        sub_8002E600(r_u32(cursor)); w_u32(object + 400u, cursor + 4u); return 1u;
    case 0x4294:
        sub_8002E6D8(object + 4u); table = sub_80066088(r_u16(object + 214u)); cursor = table;
        for (i = 0u; i < r_u16(table); ++i) { value = r_u16(cursor + 2u); cursor += 2u; xport_draft_host_sub_8006613C_p1(position, value); sub_8002E6D8(object + 4u); }
        sub_8002EF68(); return 1u;
    case 0x4295: sub_8002F130(); return 1u;
    case 0x4296: value = command_4BF3C_word(object); sub_8006A428(); return 1u;
    case 0x4297: value = command_4BF3C_word(object); sub_8006A3C4(); return 1u;
    case 0x4298:
        value = command_4BF3C_value(object);
        if (r_u32(0x800FF904u)) sub_800774EC(r_u32(0x800FF904u), object + 4u, value == 0u ? 2u : value == 1u ? 1u : 0u);
        return 1u;
    case 0x4299: value = command_4BF3C_word(object); sub_8002EE7C(value & 0xFFu); return 1u;
    case 0x429A: case 0x42A6:
        if (opcode == 0x429Au) { value = command_4BF3C_value(object); xport_draft_host_sub_8006613C_p1(position, value); }
        else { position[0] = r_u32(object + 4u); position[1] = r_u32(object + 8u); position[2] = r_u32(object + 12u); }
        value = command_4BF3C_word(object);
        if (value < 2u) sub_8001E1B8(object + 4u, value);
        else if (value == 2u)
        {
            second = sub_8004CF78(object);
            cursor = r_u32(object + 400u) + 2u; w_u32(object + 400u, cursor);
            third = r_u16(cursor); w_u32(object + 400u, cursor + 2u);
            created = sub_80032DC0(156u);
            if (created) command_4BF3C_spawn(created, position, (uint16)second, third != 0u, (uint16)third);
        }
        return 1u;
    case 0x429C:
        cursor = r_u32(object + 400u);
        sub_8001BE78(r_u8(cursor), r_u8(cursor + 2u), r_u8(cursor + 4u), r_u16(cursor + 6u), 0u, r_u16(cursor + 8u));
        w_u32(object + 400u, r_u32(object + 400u) + 10u); return 1u;
    case 0x429D: sub_8002E6D8(object + 4u); sub_8002EF58(); return 1u;
    case 0x429E: w_u32(0x800FF37Cu, r_u32(object + 8u)); return 1u;
    case 0x42A0:
        value = command_4BF3C_value(object); second = command_4BF3C_word(object); third = command_4BF3C_word(object); count = command_4BF3C_word(object);
        created = sub_80020EF8(value, object + 4u, second, third, 0x800A71CCu);
        if (created) sub_8002005C(created, count << 12, 5u);
        return 1u;
    case 0x42A2: value = command_4BF3C_word(object); w_u32(0x800FF380u, value); return 1u;
    case 0x42A3: value = command_4BF3C_word(object); second = command_4BF3C_word(object); sub_8002FD2C(value, second); return 1u;
    case 0x42A4:
        cursor = (r_u32(object + 400u) + 3u) & ~3u;
        w_u32(object + 400u, cursor + 4u); value = r_u16(cursor + 4u);
        w_u32(object + 400u, cursor + 6u); second = r_u16(cursor + 6u);
        w_u32(object + 400u, cursor + 8u); third = r_u16(cursor + 8u);
        w_u32(object + 400u, cursor + 10u); sub_8002E148(r_u32(cursor), value, second, third & 0xFFu); return 1u;
    case 0x42A5: sub_8002E2B8(); return 1u;
    case 0x42B0:
    skip_string:
        cursor = r_u32(object + 400u); while (r_u8(cursor)) ++cursor;
        w_u32(object + 400u, cursor + ((cursor & 1u) ? 1u : 2u)); return 1u;
    case 0x42B1: case 0x42B2:
        value = command_4BF3C_value(object); cursor = sub_80066088(value);
        if (opcode == 0x42B1u) sub_80064E50(cursor); else sub_80064A08(cursor);
        return 1u;
    case 0x450A:
        value = command_4BF3C_word(object); second = command_4BF3C_word(object); sub_8002FC64(value, second, object + 4u, 0u); return 1u;
    default: return 1u;
    }
}


uint32 sub_8004D604(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 v6;
    short v7;
    short v8;
    sint32 result;
    sub_8004B800(a1);
    w_u32(((uint32)((a1 + 68))), 0x800A2AC0u);
    v6 = sub_80062B0C(a1, a2);
    w_u32(((uint32)((a1 + 400))), sub_80062B50(a1, v6));
    w_u8(((uint32)((a1 + 380))), 1);
    v7 = r_u16(((uint32)(a1)));
    w_u32(((uint32)((a1 + 516))), -1);
    v8 = r_u16(((uint32)((a1 + 78))));
    w_u16(((uint32)((a1 + 214))), a3);
    w_u16(((uint32)(a1)), (v7 | 1));
    w_u16(((uint32)((a1 + 78))), (v8 & 0xFFEF));
    sub_80062A38(a1, 0x800FF4E8u);
    result = a1;
    w_u16(((uint32)((a1 + 212))), 0);
    w_u16(((uint32)((a1 + 58))), 203);
    return result;
}

/* TODO Missing call adapter sub_80087A3C */
uint32 sub_800620B8(uint32 object, uint32 animation, uint32 model)
{
    uint32 data, words, descriptor, delta_z, delta_x, value;
    sint32 heading, current;
    sub_80062924(object);
    w_u32(object + 68u, 0x800A31E4u);
    value = r_u16(object + 78u);
    current = r_u16(object);
    w_u16(object + 212u, 0u);
    w_u16(object + 214u, model);
    w_u16(object + 78u, value & 0xFFEFu);
    w_u16(object, (uint32)current | 1u);
    sub_80062A38(object, 0x800FF4E8u);
    data = sub_80062B0C(object, animation);
    w_u16(object + 16u, r_u16(data));
    w_u16(object + 18u, r_u16(data + 2u));
    w_u16(object + 20u, r_u16(data + 4u));
    descriptor = sub_80066088(model);
    sub_8006613C(object + 324u, r_u16(descriptor + 2u));
    words = (data + 9u) & 0xFFFFFFFCu;
    value = r_u32(words);
    delta_z = r_u32(object + 332u) - r_u32(object + 12u);
    delta_x = r_u32(object + 324u);
    w_u32(object + 296u, value << 12);
    value = r_u32(words + 4u);
    delta_x -= r_u32(object + 4u);
    w_u32(object + 300u, value << 12);
    w_u32(object + 304u, r_u32(words + 8u) << 12);
    w_u32(object + 308u, r_u32(words + 12u) << 12);
    w_u32(object + 312u, r_u32(words + 16u) << 12);
    w_u32(object + 316u, r_u32(words + 20u) << 12);
    heading = (3072u - (uint32)ratan2((sint32)delta_z >> 12, (sint32)delta_x >> 12)) & 0xFFFu;
    current = (sint16)r_u16(object + 18u);
    if (current < heading)
        w_u8(object + 320u, heading - current < 2048 ? 1u : 0u);
    else if (heading < current)
        w_u8(object + 320u, current - heading >= 2048 ? 1u : 0u);
    return object;
}

uint32 sub_8005C580(void)
{
    sint32 result;
    sint32 v1;
    sint32 v2;
    result = r_u32(0x800FF5A0u);
    v1 = r_u32(((uint32)((r_u32(0x800FF5A0u) + 8))));
    v2 = r_u32(((uint32)((r_u32(0x800FF5A0u) + 12))));
    w_u32(((uint32)((r_u32(0x800FF5A0u) + 392))), r_u32(((uint32)((r_u32(0x800FF5A0u) + 4)))));
    w_u32(((uint32)((result + 396))), v1);
    w_u32(((uint32)((result + 400))), v2);
    v1 = ((v1 & 0xFFFF0000u) | (((r_u16(((uint32)((result + 20))))) & 0xFFFFu) << 0));
    w_u32(((uint32)((result + 410))), r_u32(((uint32)((result + 16)))));
    w_u16(((uint32)((result + 414))), v1);
    return result;
}

/* TODO Missing call adapter nullsub_17 */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8002FF90(void)
{
    sint32 v0;
    sint32 result;
    sint32 v2;
    if (((r_u32(0x800FF818u) == 1) && r_u8(0x800EC1E8u)))
    {
        w_u32(0x800FF818u, 0);
        w_u32(0x800FF2ECu, 7);
        /* Original 8002FF88 is empty */
    }
    v0 = (r_u32(0x800FF2F4u) & 1);
    result = (r_u32(0x800FF2F4u) + 1);
    w_u32(0x800FF38Cu, r_u32(0x800FF38Cu) + 1u);
    w_u32(0x800FF2F4u, r_u32(0x800FF2F4u) + 1u);
    if (!v0)
    {
        w_u32(0x800FF2F0u, r_u32(0x800FF2F0u) + 1u);
        sub_8001BB68();
        sub_8006A868();
        sub_80070748();
        if ((r_u32(0x800FF008u) || r_u32(0x800FF300u)))
        {
            sub_80063770(0x800FF904u);
        }
        else
        {
            sub_8001BF34();
            sub_8006737C();
            sub_80063738(r_u32(0x800FF4E8u));
            sub_80063738(r_u32(0x800FF5DCu));
            sub_80063CC4();
            sub_80063770(0x800FF5A0u);
            sub_80063CF0();
            sub_800372B8();
            sub_80063770(0x800FF204u);
            sub_80063770(0x800FF220u);
            sub_80063770(0x800FF5DCu);
            sub_80063770(0x800FF8A0u);
            sub_80063770(0x800FF4E8u);
            sub_8006FD18();
            sub_800638A4();
            sub_80032C88();
        }
        sub_80022860();
        sub_8006CDD4();
        sub_80017914();
        sub_8001B7C4();
        result = r_u32(0x800FF340u);
        if (!r_u32(0x800FF340u))
        {
            sub_8006F500();
            return sub_8006B2A8();
        }
    }
    return result;
}

uint32 sub_80063CC4(void)
{
    sint32 result;
    for (result = r_u32(0x800FF630u); result; result = r_u32(((uint32)((result + 20)))))
        w_u8(((uint32)((result + 4))), 0);

    return result;
}

/* Update the object list */
void sub_80063770(uint32 a1)
{
    sint32 v1;
    sint32 v3;
    short v4;
    uint32 v5;
    sint32 v6;
    v3 = ((sint32)(r_u32(a1)));
    if (((sint32)(r_u32(a1))))
        v1 = r_u32(((uint32)((v3 + 28))));
    while (v3)
    {
        sub_80062858(v3);
        v4 = r_u16(((uint32)((v3 + 78))));
        if (((v4 & 0x40) != 0))
        {
            if (((v4 & 0x80) != 0))
            {
                uint32 table = r_u32((uint32)v3 + 68u);
                apocalypse_object_cleanup(r_u32(table + 12u), (uint32)v3 + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
                v3 = v1;
                goto LABEL_14;
            }
            w_u16(((uint32)((v3 + 78))), (v4 | 0x80));
        }
        else
        {
            v5 = sub_8006696C((r_u32(0x800FF5A0u) + 4), (v3 + 4));
            if (!sub_8006FC84(v3, v5))
            {
                v6 = (r_u16(((uint32)((v3 + 78)))) & 2);
                w_u32(((uint32)((v3 + 220))), v5);
                if ((v6 && (r_u32(0x800FF5ECu) < v5)))
                {
                    sub_80062B7C(v3, a1);
                    v3 = v1;
                    goto LABEL_14;
                }
                sub_8006338C(v3);
                sub_80062DD8(v3);
            }
        }
        v3 = v1;
    LABEL_14:
        if (!v1)
            return;

        v1 = r_u32(((uint32)((v1 + 28))));
    }
}

uint32 sub_80062858(uint32 a1)
{
    sint32 result;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    short v6;
    short v7;
    short v8;
    result = r_u16(((uint32)((a1 + 60))));
    if (r_u16(((uint32)((a1 + 60)))))
    {
        v3 = r_u32(((uint32)((a1 + 32))));
        v4 = (((unsigned char)((((v3 >> 8) & 255u) + r_u16(((uint32)((a1 + 64))))))) << 8);
        v5 = ((unsigned char)((r_u32(((uint32)((a1 + 32)))) + r_u16(((uint32)((a1 + 62)))))));
        v6 = r_u16(((uint32)((a1 + 66))));
        w_u16(((uint32)(a1)), (r_u16(((uint32)(a1))) | (0x400u)));
        v7 = r_u16(((uint32)((a1 + 60))));
        result = (((((unsigned char)((((v3 >> 16) & 255u) + v6))) << 16) | v4) | v5);
        w_u32(((uint32)((a1 + 32))), result);
        w_u16(((uint32)((a1 + 60))), --v7);
        if (!v7)
        {
            if (((r_u32(((uint32)((a1 + 52)))) & 0x2000000) != 0))
                v8 = (r_u16(((uint32)(a1))) | 0x400);
            else
                v8 = (r_u16(((uint32)(a1))) & 0xFBFF);
            w_u16(((uint32)(a1)), v8);
            result = r_u32(((uint32)((a1 + 52))));
            w_u32(((uint32)((a1 + 52))), 0);
            w_u32(((uint32)((a1 + 32))), result);
        }
    }
    return result;
}


/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8007BAB0(void)
{
  sint32 result;
  sint32 v1;
  sint32 i;
  (w_u16(0x800FF968u,(r_u16(0x800FF968u)+1u)),(r_u16(0x800FF968u)+1u));
  result = 1;
  if (!r_u16(0x800FF968u))
  {
    v1 = r_u32(0x800FF794u);
    for (w_u16(0x800FF968u,1); v1; v1 = r_u32(((uint32)((v1 + 28)))))
      w_u16(((uint32)((v1 + 2))),0);

    for (i = r_u32(0x800FF5DCu); i; i = r_u32(((uint32)((i + 28)))))
      w_u16(((uint32)((i + 2))),0);

  }
  return result;
}

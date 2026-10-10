#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* TODO Missing call adapter sub_80018E74 */
/* TODO Missing call adapter sub_800196D4 */
/* TODO Missing call adapter sub_80019DA0 */
/* TODO Missing call adapter sub_8001A2E8 */
/* TODO Missing call adapter sub_8003D41C */
/* TODO Missing call adapter sub_800408A0 */
/* TODO Missing call adapter sub_8004509C */
/* TODO Missing call adapter sub_80047A9C */
/* TODO Missing call adapter sub_80049F24 */
/* TODO Missing call adapter sub_80053AB4 */
/* TODO Missing call adapter sub_80055F14 */
/* TODO Missing call adapter sub_80057FAC */
/* TODO Missing call adapter sub_800590DC */
/* TODO Missing call adapter sub_8005A630 */
/* TODO Missing call adapter sub_8005B3E0 */
/* TODO Missing call adapter sub_80061BD4 */
/* TODO Missing call adapter sub_80071CEC */
/* TODO Missing call adapter sub_800737A0 */
/* TODO Missing call adapter sub_8007488C */
/* TODO Missing host buffer adapter xport_draft_host_sub_80020EF8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006613C_p1 */
static void apocalypse_missing_record_constructor(uint32 target, uint32 object, uint32 data, uint32 index, uint32 mode)
{
    /* TODO Implement only if this unrecorded constructor becomes an authorized runtime dependency */
    fprintf(stderr, "Unimplemented record constructor 0x%08X object=%08X data=%08X index=%u mode=%u\n", target, object, data, index, mode);
    abort();
}

uint32 sub_800641E8(uint32 index)
{
    uint32 record, cursor, data, object = 0u, list_head = 0u;
    uint32 preserve_list = 0u, clear_flag = 0u, special_flag = 0u;
    uint16 tag = 0xFFFFu;
    sint32 type, kind;
    sint32 position[4];
    FUNCTION_MARKER(0x800641E8u, "SLUS_003.73");
    record = r_u32(r_u32(0x800FF624u) + index * 4u);
    type = (sint16)r_u16(record);
    cursor = record + 2u;
    if (type == 5)
    {
        uint32 resource = r_u16(cursor);
        uint32 info = xport_draft_host_sub_8006613C_p1(position, index);
        preserve_list = 1u;
        special_flag = 1u;
        object = xport_draft_host_sub_80020EF8_p2(resource, position, r_u16(info + 2u) == 0u ? 4u : 0u, r_u16(info + 4u), 0x800A71CCu);
        goto finish;
    }
    if (type == 7)
        special_flag = 1u;
    else if (type != 1)
        return 0u;
    kind = (sint16)r_u16(cursor);
    cursor += 2u;
    tag = r_u16(cursor);
    cursor += 2u;
    cursor += 2u + (uint32)(sint32)(sint16)r_u16(cursor) * 2u;
    preserve_list = (uint8)sub_80064180(2u, cursor);
    clear_flag = (uint8)sub_80064180(4u, cursor);
    data = (sub_800641B8(cursor) + 3u) & 0xFFFFFFFCu;
    switch (kind)
    {
        case 200:
            object = sub_800625AC(624u);
            if (!object)
                return 0u;
            object = sub_8004E35C(object, data, index);
            break;
        case 201:
            object = sub_800625AC(572u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x800590DCu, object, data, index, 0u);
            break;
        case 202:
            object = sub_800625AC(544u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x80053AB4u, object, data, index, 0u);
            break;
        case 203:
            object = sub_800625AC(520u);
            if (!object)
                return 0u;
            object = sub_8004D604(object, data, index);
            break;
        case 204:
            object = sub_800625AC(328u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x8005B3E0u, object, data, index, 0u);
            break;
        case 205:
            object = sub_800625AC(576u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x80055F14u, object, data, index, 0u);
            break;
        case 206:
            object = sub_800625AC(556u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x80057FACu, object, data, index, 0u);
            break;
        case 207:
            object = sub_800625AC(528u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x8007488Cu, object, data, index, 0u);
            break;
        case 208:
            object = sub_800625AC(496u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x800737A0u, object, data, index, 1u);
            break;
        case 209:
            object = sub_800625AC(504u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x80049F24u, object, data, index, 0u);
            break;
        case 210:
            object = sub_800625AC(496u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x800737A0u, object, data, index, 0u);
            break;
        case 212:
            object = sub_800625AC(548u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x80071CECu, object, data, index, 0u);
            break;
        case 220:
            object = sub_800625AC(576u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x800408A0u, object, data, index, 0u);
            break;
        case 221:
            object = sub_800625AC(568u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x8003D41Cu, object, data, index, 0u);
            break;
        case 222:
            object = sub_800625AC(684u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x80047A9Cu, object, data, index, 0u);
            break;
        case 223:
            object = sub_800625AC(656u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x8004509Cu, object, data, index, 0u);
            break;
        case 400:
            w_u32(0x800FF3ACu, 0u);
            object = sub_800625AC(348u);
            if (object)
            {
                apocalypse_missing_record_constructor(0x80018E74u, object, data, index, 0u);
            }
            w_u32(0x800FF3ACu, 1u);
            break;
        case 401:
            w_u32(0x800FF3ACu, 0u);
            object = sub_800625AC(644u);
            if (object)
            {
                apocalypse_missing_record_constructor(0x800196D4u, object, data, index, 0u);
            }
            w_u32(0x800FF3ACu, 1u);
            break;
        case 402:
            object = sub_800625AC(540u);
            if (!object)
                return 0u;
            object = sub_8003B964(object, data, index);
            break;
        case 403:
            object = sub_800625AC(312u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x80061BD4u, object, data, index, 0u);
            break;
        case 404:
            w_u32(0x800FF3ACu, 0u);
            object = sub_800625AC(332u);
            if (object)
            {
                apocalypse_missing_record_constructor(0x80019DA0u, object, data, index, 1u);
            }
            w_u32(0x800FF3ACu, 1u);
            break;
        case 405:
            w_u32(0x800FF3ACu, 0u);
            object = sub_800625AC(324u);
            if (object)
            {
                apocalypse_missing_record_constructor(0x8001A2E8u, object, data, index & 0xFFFFu, 0u);
            }
            w_u32(0x800FF3ACu, 1u);
            break;
        case 406:
            object = sub_800625AC(336u);
            if (!object)
                return 0u;
            object = sub_800620B8(object, data, index);
            break;
        case 407:
            object = sub_800625AC(336u);
            if (!object)
                return 0u;
            object = sub_80061DC8(object, data, index);
            break;
        case 408:
            w_u32(0x800FF3ACu, 0u);
            object = sub_800625AC(332u);
            if (object)
            {
                apocalypse_missing_record_constructor(0x80019DA0u, object, data, index, 0u);
            }
            w_u32(0x800FF3ACu, 1u);
            break;
        case 409:
            object = sub_800625AC(504u);
            if (!object)
                return 0u;
            apocalypse_missing_record_constructor(0x8005A630u, object, data, index, 0u);
            break;
        default:
            break;
    }
finish:
    if (!object)
        return 0u;
    w_u16(object + 214u, index);
    w_u16(object + 170u, tag);
    if (clear_flag)
        w_u16(object + 78u, r_u16(object + 78u) & 0xFFFDu);
    if (special_flag)
        w_u16(object + 78u, r_u16(object + 78u) | 4u);
    if (!preserve_list)
    {
        if (object == r_u32(0x800FF4E8u))
            list_head = 0x800FF4E8u;
        if (object == r_u32(0x800FF5DCu))
            list_head = 0x800FF5DCu;
        if (object == r_u32(0x800FF204u))
            list_head = 0x800FF204u;
        sub_80062B7C(object, list_head);
    }
    sub_80029CC4(object);
    return object;
}

uint32 sub_8003B964(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 v6;
    short v7;
    sint32 v8;
    uint32 result;
    sub_8004B800(a1);
    v6 = r_u32(0x800FF62Cu);
    w_u32((((uint32)(a1)) + (17) * 4u), 0x800A1DF0u);
    sub_800626C8(a1, v6);
    sub_80062A38(a1, 0x800FF5DCu);
    v7 = ((sint16)(r_u16(a1)));
    w_u16((a1 + (29) * 2u), 402);
    w_u16(a1, ((v7 & 0xFEEC) | 0x111));
    v8 = sub_80062B0C(a1, a2);
    w_u32((((uint32)(a1)) + (100) * 4u), sub_80062B50(a1, v8));
    result = a1;
    w_u32((((uint32)(a1)) + (131) * 4u), -1);
    w_u16((a1 + (218) * 2u), 32);
    w_u16((a1 + (107) * 2u), a3);
    w_u8((((uint32)(a1)) + (380) * 1u), 1);
    return result;
}

/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8004E48C(uint32 a1)
{
    sint32 v2;
    sint32 result;
    (w_u32(0x800FF308u, (r_u32(0x800FF308u) + 1u)), r_u32(0x800FF308u));
    w_u8(((uint32)(((uint32)(a1) + (uint32)(129)))), 31);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(131)))), 31);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(130)))), 3);
    sub_80062A38(a1, 0x800FF4E8u);
    v2 = r_u32(0x800FF4DCu);
    result = 200;
    w_u16(((uint32)(((uint32)(a1) + (uint32)(58)))), 200);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(472)))), 0);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(474)))), 0);
    w_u32(0x800FF4DCu, ((uint32)(v2) + (uint32)(1)));
    w_u8(((uint32)(((uint32)(a1) + (uint32)(381)))), 0);
    return result;
}

uint32 sub_8004BD04(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 v7;
    sint32 result;
    v7 = r_u32(((uint32)(((uint32)(a1) + (uint32)(448)))));
    if (v7)
        ((void)(a2), sub_8006BC20(v7));
    result = sub_8006B864(((uint32)(((uint32)(2) * (uint32)(a2))) * (uint32)(a3)), 0, 1);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(448)))), result);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(452)))), a2);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(453)))), a3);
    return result;
}

/* TODO Missing call adapter sub_800878DC */
void apocalypse_collision_cells(uint32 list, void *record)
{
    uint32 length;
    sint32 bounds[6];
    uint16 generation;
    if (!list)
        return;
    memcpy(&length, (uint8 *)record + 68u, 4u);
    if (!length)
        return;
    memcpy(&generation, (uint8 *)record + 138u, 2u);
    SetRotMatrix((MATRIX *)((uint8 *)record + 72u));
    memcpy(bounds, record, sizeof(bounds));
    apocalypse_mark_array_collision_candidates(list, bounds, generation);
    while (r_u32(list))
    {
        uint32 object = r_u32(list);
        if (r_u16(object + 2u) != generation)
        {
            w_u16(object + 2u, generation);
            (void)apocalypse_collision_object(object, record);
        }
        list += 4u;
    }
}

void sub_8007BF2C(uint32 list, uint32 record)
{
    apocalypse_collision_cells(list, psx_addr(record, 140u));
}

uint32 sub_800848D0(const sint32 query_low[3], const sint32 query_high[3], const sint32 object_low[3], const sint32 object_high[3])
{
    sint32 lower_delta[3], upper_delta[3], cross[3];
    uint32 axis;
    /* Preserve the packed-low T3 and packed-high T0 carriers */
    for (axis = 0u; axis < 3u; ++axis)
        if ((sint32)((uint32)query_high[axis] - (uint32)object_high[axis]) < 0)
            return 0u;
    for (axis = 0u; axis < 3u; ++axis)
    {
        lower_delta[axis] = (sint32)((uint32)object_low[axis] - (uint32)query_low[axis]);
        if (lower_delta[axis] < 0)
            return 0u;
    }
    for (axis = 0u; axis < 3u; ++axis)
        xport_draft_gte_data_write(9u + axis, (uint32)lower_delta[axis]);
    xport_draft_gte_execute(0x170000Cu);
    for (axis = 0u; axis < 3u; ++axis)
        upper_delta[axis] = (sint32)((uint32)object_high[axis] - (uint32)query_low[axis]);
    for (axis = 0u; axis < 3u; ++axis)
        cross[axis] = (sint32)xport_draft_gte_data_read(25u + axis);
    if (cross[0] < 0)
    {
        if (cross[1] < 0)
            goto CheckX;
        xport_draft_gte_data_write(9u, (uint32)upper_delta[0]);
        xport_draft_gte_data_write(10u, (uint32)upper_delta[1]);
        xport_draft_gte_data_write(11u, (uint32)lower_delta[2]);
        xport_draft_gte_execute(0x170000Cu);
        if ((sint32)xport_draft_gte_data_read(9u) < 0)
            return 0u;
        if ((sint32)xport_draft_gte_data_read(10u) > 0)
            return 0u;
        return 1u;
    }
    if (cross[2] < 0)
    {
        xport_draft_gte_data_write(9u, (uint32)upper_delta[0]);
        xport_draft_gte_data_write(10u, (uint32)lower_delta[1]);
        xport_draft_gte_data_write(11u, (uint32)upper_delta[2]);
        xport_draft_gte_execute(0x170000Cu);
        if ((sint32)xport_draft_gte_data_read(9u) > 0)
            return 0u;
        if ((sint32)xport_draft_gte_data_read(11u) < 0)
            return 0u;
        return 1u;
    }
CheckX:
    xport_draft_gte_data_write(9u, (uint32)lower_delta[0]);
    xport_draft_gte_data_write(10u, (uint32)upper_delta[1]);
    xport_draft_gte_data_write(11u, (uint32)upper_delta[2]);
    xport_draft_gte_execute(0x170000Cu);
    if ((sint32)xport_draft_gte_data_read(10u) < 0)
        return 0u;
    if ((sint32)xport_draft_gte_data_read(11u) > 0)
        return 0u;
    return 1u;
}

uint32 sub_8004D2B4(uint32 object, uint32 opcode)
{
    uint32 cursor, value, player, result;
    opcode &= 65535u;
    switch (opcode)
    {
        case 8448u:
            return (uint32)(sint32)(sint16)r_u16(object + 218u);
        case 8480u:
            cursor = r_u32(object + 400u);
            value = r_u8(cursor);
            w_u32(object + 400u, cursor + 2u);
            return (uint32)(sint32)(sint16)r_u16(object + 460u + value * 2u);
        case 8489u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            return sub_80066570(value);
        case 8490u:
            return r_u16(object + 214u);
        case 8491u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            if (value & 0x2000u)
            {
                uint32 table = r_u32(object + 68u);
                uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80u);
                uint32 target = r_u32(table + 84u);
                value = xport_draft_guest_call2(target, receiver, value);
            }
            cursor = sub_80066088(value & 65535u);
            return r_u16(cursor) ? r_u16(cursor + 2u) : 0u;
        case 8492u:
            return r_u8(object + 384u);
        case 8493u:
            return r_u8(object + 383u);
        case 8494u:
            return r_u16(object + 392u);
        case 8498u:
            player = r_u32(0x800FF5A0u);
            if (!player)
                return 8191u;
            result = sub_80066918(object + 4u, player + 4u);
            return result < 8192u ? result : 8191u;
        case 8499u:
            player = r_u32(0x800FF5A0u);
            if (!player)
                return 0u;
            {
                uint32 position[3];
                position[0] = r_u32(object + 4u);
                position[1] = r_u32(object + 8u);
                position[2] = r_u32(object + 12u);
                position[1] -= (uint32)(sint32)(sint16)r_u16(object + 458u) << 12;
                return xport_draft_host_sub_800679A4_p1(position, player + 4u);
            }
        case 8502u:
            return r_u32(0x800FF274u);
        case 8512u:
        case 8513u:
        case 8514u:
            return (uint32)((sint32)r_u32(object + 4u + (opcode - 8512u) * 4u) >> 12);
        case 8528u:
        case 8529u:
        case 8530u:
            player = r_u32(0x800FF5A0u);
            return player ? (uint32)((sint32)r_u32(player + 4u + (opcode - 8528u) * 4u) >> 12) : 0u;
        default:
            return 0u;
    }
}

uint32 apocalypse_move_target_native(uint32 object, const void *position)
{
    uint32 values[3], result;
    if (r_u8(object + 26u) != 2u)
        sub_80063118(object, 2u, 1u);
    if (r_u16(object + 390u) == 128u && (r_u32(object + 372u) & 9u) && r_u8(object + 26u) != 1u)
        sub_80063118(object, 1u, 1u);
    memcpy(values, position, sizeof(values));
    w_u32(object + 484u, values[0]);
    w_u32(object + 488u, values[1]);
    w_u32(object + 492u, values[2]);
    result = (r_u32(object + 396u) | 1u) & 0xFFFFFFC7u;
    w_u32(object + 396u, result);
    return result;
}

uint32 sub_80053784(uint32 object, uint32 position)
{
    return apocalypse_move_target_native(object, psx_addr(position, 12u));
}

uint32 apocalypse_jump_target_native(uint32 object, const void *position)
{
    uint32 values[3], result, state;
    sub_80063038(object, 4u, 0u, 0xFFFFFFFFu);
    memcpy(values, position, sizeof(values));
    w_u32(object + 484u, values[0]);
    w_u32(object + 488u, values[1]);
    w_u32(object + 492u, values[2]);
    result = r_u32(object + 396u) | 9u;
    state = r_u8(object + 438u);
    w_u8(object + 612u, state);
    w_u32(object + 396u, result);
    return result;
}

uint32 sub_80053834(uint32 object, uint32 position)
{
    return apocalypse_jump_target_native(object, psx_addr(position, 12u));
}

uint32 apocalypse_special_target_native(uint32 object, const void *position)
{
    uint32 values[3], result;
    sub_80063038(object, 4u, 0u, 0xFFFFFFFFu);
    memcpy(values, position, sizeof(values));
    w_u32(object + 484u, values[0]);
    w_u32(object + 488u, values[1]);
    w_u32(object + 492u, values[2]);
    result = r_u32(object + 396u);
    w_u8(object + 382u, 1u);
    result |= 0x11u;
    w_u32(object + 396u, result);
    return result;
}

uint32 sub_8005389C(uint32 object, uint32 position)
{
    return apocalypse_special_target_native(object, psx_addr(position, 12u));
}

uint32 sub_8004BE30(uint32 a1)
{
    sint32 v1;
    uint32 v2;
    sint32 v3;
    sint32 result;
    v1 = 0;
    do
    {
        v2 = r_u32(((uint32)(((uint32)(a1) + (uint32)(400)))));
        v3 = r_u16(v2);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(400)))), (v2 + (1) * 2u));
        result = ((uint32)(v3) - (uint32)(16658));
        if ((((unsigned short)(v3)) == 16672))
        {
            if ((v1-- == 0))
                return result;
            result = ((unsigned short)(v3));
        }
        else
        {
            result = ((unsigned short)(v3));
            if ((((unsigned short)(((uint32)(v3) - (uint32)(16658)))) < 5u))
                ++v1;
        }
    } while ((result != 16640));
    return result;
}

static void apocalypse_missing_level_cleanup(void)
{
    /* TODO Excluded sub_80063A38 cleanup service has no native implementation */
    fprintf(stderr, "Missing level cleanup service 0x80063A38\n");
    abort();
}

uint32 sub_80030AD4(void)
{
    uint32 frame_start;
    sint32 elapsed;
    sint16 rectangle[4];
    uint32 packed;
    FUNCTION_MARKER(0x80030AD4u, "SLUS_003.73");
    sub_8005C580();
    w_u32(0x800FF64Cu, 0);
    w_u32(0x800FF2F0u, 0);
    w_u32(0x800FF2F4u, 0);
    w_u8(0x800FF8F1u, 0);
    sub_8007001C();
    w_u32(0x800FF7BCu, 0);
    w_u32(0x800FF7C0u, 0);
    w_u32(0x800FF38Cu, 0);
    w_u32(0x800FF390u, 0);
    w_u32(0x800FF2ECu, 0);
    sub_8006654C(312921176);
    sub_800664E4(1);
    while (!r_u32(0x800FF2ECu))
    {
        nullsub_16();
        frame_start = r_u32(0x800FF64Cu);
        sub_80068160();
        sub_8002FF54();
        sub_8002FF90();
        if (r_u32(0x800FF2ECu))
            break;
        sub_8002FF90();
        if (r_u32(0x800FF2ECu))
            break;
        sub_800307F0();
        sub_8002F284();
        if ((r_u32(0x800FF64Cu) == frame_start))
            sub_800664E4(1);
        w_u32(0x800FF65Cu, 0);
        while (1)
        {
            sub_800664E4(1);
            if (!DrawSync(1))
                break;
            sub_800662A0();
        }

        sub_800681B8();
        if (!r_u32(0x800FF65Cu))
        {
            sub_800662A0();
            w_u32(0x800FF65Cu, 1);
        }
        elapsed = (sint32)(r_u32(0x800FF64Cu) - 2u - frame_start);
        if ((((!r_u32(0x800FF304u) && (r_u32(0x800FF818u) != 1)) && (elapsed > 0)) && !r_u8(0x800FF8F1u)))
        {
            sub_8002FF90();
            if (r_u32(0x800FF2ECu))
                break;
            if ((elapsed >= 2))
            {
                sub_8002FF90();
                if (r_u32(0x800FF2ECu))
                    break;
            }
        }
        if (sub_80062F48(r_u32(0x800FF5A0u)))
            w_u32(0x800FF2ECu, 2);
    }

    sub_8006F7D8();
    w_u16(0x800FFAACu, 1);
    sub_8002E2B8();
    if (r_u32(0x800FF304u))
        apocalypse_missing_level_cleanup();
    packed = r_u32(0x800FF368u);
    rectangle[0] = (sint16)packed;
    rectangle[1] = (sint16)(packed >> 16);
    packed = r_u32(0x800FF36Cu);
    rectangle[2] = (sint16)packed;
    rectangle[3] = (sint16)(packed >> 16);
    xport_draft_host_sub_8008847C_p1(rectangle, 0, 0, 0);
    rectangle[1] = 511;
    rectangle[3] = 1;
    return xport_draft_host_sub_8008847C_p1(rectangle, 0, 0, 0);
}

/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8001BF34(void)
{
    sint32 result;
    sint32 v1;
    if (r_u32(0x800FF1E8u))
    {
        result = ((uint32)(r_u32(0x800FF1ECu)) - (uint32)(1));
        if (r_u32(0x800FF1ECu))
        {
            (w_u32(0x800FF1ECu, (r_u32(0x800FF1ECu) - 1u)), r_u32(0x800FF1ECu));
            v1 = (result != 0);
            result = 16711680;
            if (v1)
            {
                w_u32(0x800FFB60u, (r_u32(0x800FFB60u) + (r_u32(0x800FFB6Cu))));
                w_u32(0x800FFB68u, (r_u32(0x800FFB68u) + (r_u32(0x800FFB6Cu))));
                result = ((uint32)(r_u32(0x800FFB64u)) + (uint32)(r_u32(0x800FFB6Cu)));
                w_u32(0x800FFB64u, (r_u32(0x800FFB64u) + (r_u32(0x800FFB6Cu))));
            }
            else
            {
                w_u32(0x800FFB68u, 16711680);
                w_u32(0x800FFB64u, 16711680);
                w_u32(0x800FFB60u, 16711680);
            }
        }
    }
    else
    {
        result = ((uint32)(r_u32(0x800FF1E4u)) - (uint32)(1));
        if (r_u32(0x800FF1E4u))
        {
            (w_u32(0x800FF1E4u, (r_u32(0x800FF1E4u) - 1u)), r_u32(0x800FF1E4u));
            if (result)
            {
                w_u32(0x800FFB60u, (r_u32(0x800FFB60u) - (r_u32(0x800FFB6Cu))));
                result = ((uint32)(r_u32(0x800FFB64u)) - (uint32)(r_u32(0x800FFB70u)));
                w_u32(0x800FFB64u, (r_u32(0x800FFB64u) - (r_u32(0x800FFB70u))));
                w_u32(0x800FFB68u, (r_u32(0x800FFB68u) - (r_u32(0x800FFB74u))));
            }
            else
            {
                w_u8(0x800FF1DCu, 0);
            }
        }
    }
    return result;
}

/* TODO Missing call adapter indirect */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C3AC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C4EC_p123 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C808_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006CB28_p1 */
static uint32 apocalypse_object_dispatch(uint32 target, uint32 object)
{
    switch (target)
    {
        case 0x800209ECu:
            return sub_800209EC(object);
        case 0x80026488u:
            return sub_80026488(object);
        case 0x8002948Cu:
            return sub_8002948C(object);
        case 0x8003BBD8u:
            return sub_8003BBD8(object);
        case 0x8004D800u:
            return sub_8004D800(object);
        case 0x8004E71Cu:
            return sub_8004E71C(object);
        case 0x8005BD48u:
            return sub_8005BD48(object);
        case 0x8005F688u:
            return sub_8005F688(object);
        case 0x80061F54u:
            return sub_80061F54(object);
        case 0x800622D8u:
            nullsub_34();
            /* The empty callee preserves the original JALR target as its carried result */
            return target;
        case 0x8007876Cu:
            return sub_8007876C(object);
        default:
            /* TODO Resolve targets whose semantic input ABI is not established */
            fprintf(stderr, "Unimplemented object callback 0x%08X\n", target);
            abort();
    }
}

static void apocalypse_object_update_callback(uint32 object)
{
    uint32 table = r_u32(object + 68u);
    uint32 adjusted = object + (uint32)(sint32)(sint16)r_u16(table + 32u);
    uint32 target = r_u32(table + 36u);
    if (target == 0x800639A4u)
        nullsub_22();
    else
        (void)apocalypse_object_dispatch(target, adjusted);
}

static uint32 apocalypse_object_behavior_callback(uint32 object)
{
    uint32 table = r_u32(object + 68u);
    uint32 adjusted = object + (uint32)(sint32)(sint16)r_u16(table + 24u);
    uint32 target = r_u32(table + 28u);
    return apocalypse_object_dispatch(target, adjusted);
}

uint32 sub_8006338C(uint32 a1)
{
    sint32 v2;
    sint8 v3;
    sint8 v4;
    sint32 result;
    uint32 v6;
    unsigned char v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    sint8 v13;
    sint8 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    short v21;
    sint32 result_vector[3];
    char v25[16];
    sint32 v26;
    sint32 v27;
    FUNCTION_MARKER(0x8006338Cu, "SLUS_003.73");
    if (((r_u16(((uint32)(a1))) & 2) != 0))
    {
        sub_80063164(a1);
        apocalypse_object_update_callback(a1);
        if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(225))))) < 2u))
        {
            v2 = r_u32(((uint32)(((uint32)(a1) + (uint32)(68)))));
            w_u16(((uint32)(((uint32)(a1) + (uint32)(310)))), ((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(24))))))));
            (void)apocalypse_object_behavior_callback(a1);
            v3 = r_u8(((uint32)(((uint32)(a1) + (uint32)(26)))));
            v4 = r_u8(((uint32)(((uint32)(a1) + (uint32)(297)))));
            result = ((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(24)))))));
            w_u16(((uint32)(((uint32)(a1) + (uint32)(308)))), result);
            w_u8(((uint32)(((uint32)(a1) + (uint32)(312)))), v3);
            w_u8(((uint32)(((uint32)(a1) + (uint32)(313)))), v4);
            return result;
        }
    }
    else
    {
        apocalypse_object_update_callback(a1);
        if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(225))))) < 2u))
            return apocalypse_object_behavior_callback(a1);
    }
    v6 = r_u8(((uint32)(((uint32)(a1) + (uint32)(225)))));
    v7 = ((uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(226)))))) + (uint32)(1));
    w_u8(((uint32)(((uint32)(a1) + (uint32)(226)))), v7);
    if ((v7 < v6))
    {
        sub_8006C0B8(((uint32)(a1) + (uint32)(4)), ((uint32)(a1) + (uint32)(264)));
        sub_8006C730(((uint32)(a1) + (uint32)(16)), ((uint32)(a1) + (uint32)(288)));
        return sub_8006C624(((uint32)(a1) + (uint32)(16)));
    }
    else
    {
        w_u8(((uint32)(((uint32)(a1) + (uint32)(226)))), 0);
        v8 = r_u32(((uint32)(((uint32)(a1) + (uint32)(256)))));
        v9 = r_u32(((uint32)(((uint32)(a1) + (uint32)(260)))));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(4)))), r_u32(((uint32)(((uint32)(a1) + (uint32)(252))))));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(8)))), v8);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(12)))), v9);
        v10 = r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))));
        v11 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(228)))), r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(232)))), v10);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(236)))), v11);
        v10 = ((v10 & 0xFFFF0000u) | (((r_u16(((uint32)(((uint32)(a1) + (uint32)(286)))))) & 0xFFFFu) << 0));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(16)))), r_u32(((uint32)(((uint32)(a1) + (uint32)(282))))));
        w_u16(((uint32)(((uint32)(a1) + (uint32)(20)))), v10);
        v10 = ((v10 & 0xFFFF0000u) | (((r_u16(((uint32)(((uint32)(a1) + (uint32)(20)))))) & 0xFFFFu) << 0));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(276)))), r_u32(((uint32)(((uint32)(a1) + (uint32)(16))))));
        w_u16(((uint32)(((uint32)(a1) + (uint32)(280)))), v10);
        if (((r_u16(((uint32)(a1))) & 2) != 0))
        {
            v12 = r_u32(((uint32)(((uint32)(a1) + (uint32)(68)))));
            w_u16(((uint32)(((uint32)(a1) + (uint32)(310)))), ((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(24))))))));
            (void)apocalypse_object_behavior_callback(a1);
            v13 = r_u8(((uint32)(((uint32)(a1) + (uint32)(26)))));
            v14 = r_u8(((uint32)(((uint32)(a1) + (uint32)(297)))));
            w_u16(((uint32)(((uint32)(a1) + (uint32)(308)))), ((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(24))))))));
            w_u8(((uint32)(((uint32)(a1) + (uint32)(312)))), v13);
            w_u8(((uint32)(((uint32)(a1) + (uint32)(313)))), v14);
        }
        else
        {
            (void)apocalypse_object_behavior_callback(a1);
        }
        v15 = r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))));
        v16 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(252)))), r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(256)))), v15);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(260)))), v16);
        v17 = r_u32(((uint32)(((uint32)(a1) + (uint32)(232)))));
        v18 = r_u32(((uint32)(((uint32)(a1) + (uint32)(236)))));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(4)))), r_u32(((uint32)(((uint32)(a1) + (uint32)(228))))));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(8)))), v17);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(12)))), v18);
        xport_draft_host_sub_8006C3AC_p1(v25, ((uint32)(a1) + (uint32)(252)), ((uint32)(a1) + (uint32)(228)));
        v26 = r_u8(((uint32)(((uint32)(a1) + (uint32)(225)))));
        xport_draft_host_sub_8006C4EC_p123(&result_vector[0], v25, &v26);
        v19 = result_vector[1];
        v20 = result_vector[2];
        w_u32(((uint32)(((uint32)(a1) + (uint32)(264)))), result_vector[0]);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(268)))), v19);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(272)))), v20);
        v19 = ((v19 & 0xFFFF0000u) | (((r_u16(((uint32)(((uint32)(a1) + (uint32)(20)))))) & 0xFFFFu) << 0));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(282)))), r_u32(((uint32)(((uint32)(a1) + (uint32)(16))))));
        w_u16(((uint32)(((uint32)(a1) + (uint32)(286)))), v19);
        v19 = ((v19 & 0xFFFF0000u) | (((r_u16(((uint32)(((uint32)(a1) + (uint32)(280)))))) & 0xFFFFu) << 0));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(16)))), r_u32(((uint32)(((uint32)(a1) + (uint32)(276))))));
        w_u16(((uint32)(((uint32)(a1) + (uint32)(20)))), v19);
        xport_draft_host_sub_8006CB28_p1(&result_vector[0], ((uint32)(a1) + (uint32)(282)), ((uint32)(a1) + (uint32)(276)));
        v21 = result_vector[1];
        w_u32(((uint32)(((uint32)(a1) + (uint32)(288)))), result_vector[0]);
        w_u16(((uint32)(((uint32)(a1) + (uint32)(292)))), v21);
        if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(288))))))))) < -2048))
            w_u16(((uint32)(((uint32)(a1) + (uint32)(288)))), (r_u16(((uint32)(((uint32)(a1) + (uint32)(288))))) + (4096)));
        if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(288))))))))) >= 2049))
            w_u16(((uint32)(((uint32)(a1) + (uint32)(288)))), (r_u16(((uint32)(((uint32)(a1) + (uint32)(288))))) - (4096)));
        if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(290))))))))) < -2048))
            w_u16(((uint32)(((uint32)(a1) + (uint32)(290)))), (r_u16(((uint32)(((uint32)(a1) + (uint32)(290))))) + (4096)));
        if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(290))))))))) >= 2049))
            w_u16(((uint32)(((uint32)(a1) + (uint32)(290)))), (r_u16(((uint32)(((uint32)(a1) + (uint32)(290))))) - (4096)));
        if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(292))))))))) < -2048))
            w_u16(((uint32)(((uint32)(a1) + (uint32)(292)))), (r_u16(((uint32)(((uint32)(a1) + (uint32)(292))))) + (4096)));
        if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(292))))))))) >= 2049))
            w_u16(((uint32)(((uint32)(a1) + (uint32)(292)))), (r_u16(((uint32)(((uint32)(a1) + (uint32)(292))))) - (4096)));
        v27 = r_u8(((uint32)(((uint32)(a1) + (uint32)(225)))));
        return xport_draft_host_sub_8006C808_p2(((uint32)(a1) + (uint32)(288)), &v27);
    }
}

uint32 sub_8006C8E8(uint32 a1, uint32 a2)
{
    uint32 result;
    short v3;
    sint32 v4;
    short v5;
    sint32 v6;
    result = a1;
    v3 = ((uint32)(((sint16)(r_u16(a1)))) - (uint32)((((sint32)(((sint16)(r_u16(a1))))) >> r_u8(a2))));
    v4 = ((sint16)(r_u16((a1 + (1) * 2u))));
    w_u16(result, v3);
    v5 = ((uint32)(r_u16((result + (1) * 2u))) - (uint32)((((sint32)(v4)) >> r_u8((a2 + (1) * 1u)))));
    v6 = ((short)(r_u16((result + (2) * 2u))));
    w_u16((result + (1) * 2u), v5);
    w_u16((result + (2) * 2u), (r_u16((result + (2) * 2u)) - ((((sint32)(v6)) >> r_u8((a2 + (2) * 1u))))));
    return result;
}

/* TODO Missing call adapter HIDWORD */
/* TODO Missing call adapter LODWORD */
/* TODO Missing call adapter SBYTE4 */
uint32 sub_8005CB1C(uint32 a1)
{
    sint32 v2;
    sint32 result;
    sint32 v4;
    sint32 v5;
    uint32 v6;
    uint32 v7;
    sint8 v8;
    sint8 v9;
    uint32 v10;
    sint8 v11;
    uint32 v12;
    sint8 v13;
    unsigned char v14;
    sint32 v15;
    sint32 v16;
    long long v17;
    sint32 v18;
    long long v19;
    v2 = r_u32(((uint32)(((uint32)(a1) + (uint32)(452)))));
    result = 1;
    w_u8(((uint32)(((uint32)(a1) + (uint32)(468)))), 0);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(469)))), 0);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(471)))), 0);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(472)))), 0);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(470)))), 1);
    if (v2)
    {
        w_u8(((uint32)(((uint32)(a1) + (uint32)(473)))), 0);
    }
    else
    {
        v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
        v5 = r_u32(((uint32)(((uint32)(v4) + (uint32)(364)))));
        if (((v5 == 115) || (v5 == 83)))
        {
            v6 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
            w_u8(((uint32)(((uint32)(a1) + (uint32)(468)))), r_u8(((uint32)(((uint32)(v4) + (uint32)(360))))));
            w_u8(((uint32)(((uint32)(a1) + (uint32)(469)))), r_u8((v6 + (361) * 1u)));
            w_u8(((uint32)(((uint32)(a1) + (uint32)(471)))), r_u8((v6 + (362) * 1u)));
            w_u8(((uint32)(((uint32)(a1) + (uint32)(472)))), r_u8((v6 + (363) * 1u)));
        }
        if (!((r_u8(((uint32)(((uint32)(a1) + (uint32)(468))))) | r_u8(((uint32)(((uint32)(a1) + (uint32)(469))))))))
        {
            v7 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
            v8 = 0x80;
            if (r_u8(v7 + 160u) || (v8 = 127, r_u8(v7 + 176u)))
            {
                w_u8(((uint32)(((uint32)(a1) + (uint32)(468)))), v8);
                v7 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
            }
            v9 = 127;
            if (r_u8(v7 + 144u) || (v9 = 128, r_u8(v7 + 128u)))
                w_u8(((uint32)(((uint32)(a1) + (uint32)(469)))), v9);
        }
        if (r_u16(((uint32)(((uint32)(a1) + (uint32)(574))))))
        {
            w_u8(((uint32)(((uint32)(a1) + (uint32)(468)))), (r_u8(((uint32)(((uint32)(a1) + (uint32)(468))))) - ((((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(574)))))) * (uint32)(((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(468))))))))) / r_u16(((uint32)(((uint32)(a1) + (uint32)(576)))))))));
            w_u8(((uint32)(((uint32)(a1) + (uint32)(469)))), (r_u8(((uint32)(((uint32)(a1) + (uint32)(469))))) - ((((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(574)))))) * (uint32)(((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(469))))))))) / r_u16(((uint32)(((uint32)(a1) + (uint32)(576)))))))));
        }
        if ((r_u8(((uint32)(((uint32)(a1) + (uint32)(471))))) | r_u8(((uint32)(((uint32)(a1) + (uint32)(472)))))))
            goto LABEL_28;
        v10 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
        w_u8(((uint32)(((uint32)(a1) + (uint32)(470)))), 0);
        v11 = 0x80;
        if (r_u8(v10) || (v11 = 127, r_u8(v10 + 48u)))
            w_u8(((uint32)(((uint32)(a1) + (uint32)(471)))), v11);
        v12 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
        v13 = 127;
        if (r_u8(v12 + 32u) || (v13 = 128, r_u8(v12 + 16u)))
        {
            w_u8(((uint32)(((uint32)(a1) + (uint32)(472)))), v13);
            v12 = r_u32(((uint32)(((uint32)(a1) + (uint32)(444)))));
        }
        if ((((r_u8(v12) || r_u8((v12 + (48) * 1u))) || r_u8((v12 + (32) * 1u))) || r_u8((v12 + (16) * 1u))))
        {
            v14 = ((uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(473)))))) + (uint32)(1));
            w_u8(((uint32)(((uint32)(a1) + (uint32)(473)))), v14);
            if ((v14 >= 2u))
                w_u8(((uint32)(((uint32)(a1) + (uint32)(473)))), 2);
        }
        else
        {
        LABEL_28:
            w_u8(((uint32)(((uint32)(a1) + (uint32)(473)))), 0);
        }
        v15 = r_u8(a1 + 468u);
        v17 = r_u8(a1 + 469u);
        result = v15 | (uint32)v17;
        if (result)
        {
            result = ratan2(-(sint32)(sint8)v15, (sint8)v17);
            w_u16(a1 + 474u, (1024u - result) & 4095u);
        }
        v18 = r_u8(a1 + 471u);
        v19 = r_u8(a1 + 472u);
        result = v18 | (uint32)v19;
        if (result)
        {
            result = ratan2(-(sint32)(sint8)v18, (sint8)v19);
            w_u16(a1 + 476u, (1024u - result) & 4095u);
        }
    }
    return result;
}

uint32 sub_8005EB60(uint32 a1)
{
    sint32 result;
    sint32 v2;
    sint32 v3;
    result = 0;
    if ((((sint32)(((sint8)(r_u8(((uint32)(((uint32)(a1) + (uint32)(469))))))))) > 0))
    {
        v2 = ((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(474)))))));
        result = 0;
        if ((((sint32)(v2)) >= 768))
        {
            if ((((sint32)(v2)) >= 1281))
            {
                return 0;
            }
            else
            {
                v3 = r_u8(((uint32)(((uint32)(a1) + (uint32)(26)))));
                w_u32(((uint32)(((uint32)(a1) + (uint32)(460)))), 1024);
                result = 1;
                if ((v3 != 1))
                {
                    sub_80063118(a1, 1, 1);
                    return 1;
                }
            }
        }
    }
    return result;
}

uint32 sub_8005F124(uint32 a1)
{
    sint32 v2;
    sint32 v3;
    sint8 v4;
    sint32 result;
    if (((r_u16(((uint32)(((uint32)(a1) + (uint32)(216))))) & 2) != 0))
    {
    LABEL_4:
        result = 0;

        if (r_u8(((uint32)(((uint32)(a1) + (uint32)(578))))))
            return result;
        goto LABEL_5;
    }
    v2 = r_u8(((uint32)(((uint32)(a1) + (uint32)(578)))));
    v3 = (v2 == 0);
    v4 = ((uint32)(v2) - (uint32)(1));
    if (!v3)
    {
        w_u8(((uint32)(((uint32)(a1) + (uint32)(578)))), v4);
        goto LABEL_4;
    }
LABEL_5:
    w_u32(((uint32)(((uint32)(a1) + (uint32)(480)))), r_u32(((uint32)(((uint32)(a1) + (uint32)(8))))));

    sub_80063038(a1, 2, ((uint32)(r_u16(0x800EC4D4u)) + (uint32)(1)), r_u16(0x800EC4D6u));
    result = 1;
    w_u32(((uint32)(((uint32)(a1) + (uint32)(460)))), 4);
    return result;
}

/* TODO Missing call adapter abs32 */
uint32 sub_80061A78(uint32 object, uint32 target_angle, uint32 heading, uint32 immediate)
{
    uint32 target = target_angle & 4095u, desired = heading & 4095u;
    uint32 link, yaw_distance, angle_distance;
    sint32 old_heading, old_angle, delta;
    if (r_u8(object + 470u) || r_u8(object + 473u) == 1u)
        immediate = 1u;
    if (immediate)
    {
        link = r_u32(object + 360u);
        w_u16(object + 18u, desired);
        w_u16(link + 86u, target);
        w_u32(object + 436u, 0u);
        w_u32(object + 440u, 0u);
        return link;
    }
    old_heading = (sint16)r_u16(object + 18u);
    delta = (sint32)desired - old_heading;
    yaw_distance = delta < 0 ? 0u - (uint32)delta : (uint32)delta;
    if (yaw_distance - 1025u < 2047u)
    {
        link = r_u32(object + 360u);
        w_u16(object + 18u, desired);
        w_u16(link + 86u, target);
        w_u32(object + 436u, 0u);
        w_u32(object + 440u, 0u);
        return link;
    }
    link = r_u32(object + 360u);
    old_angle = (sint16)r_u16(link + 86u);
    delta = (sint32)target - old_angle;
    angle_distance = delta < 0 ? 0u - (uint32)delta : (uint32)delta;
    if (angle_distance - 1025u < 2047u)
    {
        w_u16(object + 18u, desired);
        w_u16(link + 86u, target);
        w_u32(object + 436u, 0u);
        w_u32(object + 440u, 0u);
        return link;
    }
    if (yaw_distance && angle_distance)
    {
        uint32 angle = r_u16(link + 86u);
        angle = old_heading < (sint32)desired ? angle - yaw_distance : angle + yaw_distance;
        w_u16(link + 86u, angle & 4095u);
    }
    w_u32(object + 440u, 8u);
    w_u16(object + 18u, desired);
    link = r_u32(object + 360u);
    w_u32(object + 432u, target);
    old_angle = (sint16)r_u16(link + 86u);
    if (old_angle < (sint32)target)
        delta = (sint32)target - old_angle < 1024 ? (sint32)target - old_angle : (sint16)(target - 4096u) - old_angle;
    else
        delta = old_angle - (sint32)target < 1024 ? (sint32)target - old_angle : (sint32)target - (sint16)(r_u16(link + 86u) - 4096u);
    delta /= 8;
    w_u32(object + 436u, (uint32)delta);
    return (uint32)delta;
}

/* TODO Missing call adapter xport_draft_missing_gte_adapter */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C22C_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C3AC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C4EC_p12 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C564_p123 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C5C4_p13 */
uint32 sub_8007C398(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5)
{
    return apocalypse_collision_sphere_segment(a1, a2, psx_addr(a3, 12u), a4, a5);
}

uint32 apocalypse_collision_sphere_segment(uint32 a1, uint32 a2, void *a3, uint32 a4, uint32 a5)
{
    return apocalypse_collision_sphere_segment_native_end(a1, psx_addr(a2, 12u), a3, a4, a5);
}

static uint32 collision_endpoint_word(const void *endpoint, unsigned axis)
{
    uint32 value;
    memcpy(&value, (const uint8 *)endpoint + axis * 4u, 4u);
    return value;
}

uint32 apocalypse_collision_sphere_segment_native_end(uint32 a1, const void *endpoint, void *a3, uint32 a4, uint32 a5)
{
    sint32 result;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 vector[3];
    int v35[4];
    uint32 v36[4];
    sint32 v37;
    sint32 v38;
    sint32 v39;
    sint32 v40;
    sint32 v41;
    int *v42;
    FUNCTION_MARKER(0x8007C398u, "SLUS_003.73");
    w_u32(0x800FFA84u, 0);
    w_u32(0x800ED634u, 0x7FFFFFFF);
    for (unsigned axis = 0u; axis < 3u; ++axis)
    {
        uint32 end_component = collision_endpoint_word(endpoint, axis);
        v35[axis] = (sint32)(end_component - r_u32(a1 + axis * 4u));
    }
    v37 = 12;
    xport_draft_host_sub_8006C564_p123(vector, v35, &v37);
    w_u32(0x800ED5C0u, vector[0]);
    w_u32(0x800ED5C4u, vector[1]);
    w_u32(0x800ED5C8u, vector[2]);
    xport_draft_gte_data_write(9u, r_u32(0x800ED5C0u));
    xport_draft_gte_data_write(10u, r_u32(0x800ED5C4u));
    xport_draft_gte_data_write(11u, r_u32(0x800ED5C8u));
    xport_draft_gte_execute(0xA00428u);
    w_u32(0x800ED5E4u, xport_draft_gte_data_read(25u));
    w_u32(0x800ED5E8u, xport_draft_gte_data_read(26u));
    w_u32(0x800ED5ECu, xport_draft_gte_data_read(27u));
    w_u32(0x800FFA14u, sub_80085B54(((uint32)(((uint32)(r_u32(0x800ED5E4u)) + (uint32)(r_u32(0x800ED5E8u)))) + (uint32)(r_u32(0x800ED5ECu)))));
    if (!r_u32(0x800FFA14u))
        return 0;
    v38 = 12;
    xport_draft_host_sub_8006C5C4_p13(v36, 0x800ED5C0u, &v38);
    xport_draft_host_sub_8006C4EC_p12(vector, v36, 0x800FFA14u);
    w_u32(0x800ED5C0u, vector[0]);
    w_u32(0x800ED5C4u, vector[1]);
    w_u32(0x800ED5C8u, vector[2]);
    xport_draft_gte_control_write(0u, r_u32(0x800ED5C0u));
    xport_draft_gte_control_write(2u, r_u32(0x800ED5C4u));
    xport_draft_gte_control_write(4u, r_u32(0x800ED5C8u));
    w_u16(0x800FFA44u, vector[0]);
    w_u16(0x800FFA46u, vector[1]);
    w_u16(0x800FFA48u, vector[2]);
    xport_draft_gte_control_write(8u, r_u32(0x800FFA44u));
    xport_draft_gte_control_write(9u, r_u32(0x800FFA48u));
    if ((((sint32)(((sint32)(r_u32(a1))))) >= ((sint32)(((sint32)(collision_endpoint_word(endpoint, 0u)))))))
    {
        w_u32(0x800ED5CCu, ((sint32)(collision_endpoint_word(endpoint, 0u))));
        v19 = ((sint32)(r_u32(a1)));
    }
    else
    {
        w_u32(0x800ED5CCu, ((sint32)(r_u32(a1))));
        v19 = ((sint32)(collision_endpoint_word(endpoint, 0u)));
    }
    w_u32(0x800ED5D8u, v19);
    if ((((sint32)(((sint32)(r_u32((a1 + (1) * 4u)))))) >= ((sint32)(((sint32)(collision_endpoint_word(endpoint, 1u)))))))
    {
        w_u32(0x800ED5D0u, ((sint32)(collision_endpoint_word(endpoint, 1u))));
        v20 = ((sint32)(r_u32((a1 + (1) * 4u))));
    }
    else
    {
        w_u32(0x800ED5D0u, ((sint32)(r_u32((a1 + (1) * 4u)))));
        v20 = ((sint32)(collision_endpoint_word(endpoint, 1u)));
    }
    w_u32(0x800ED5DCu, v20);
    if ((((sint32)(((sint32)(r_u32((a1 + (2) * 4u)))))) >= ((sint32)(((sint32)(collision_endpoint_word(endpoint, 2u)))))))
    {
        w_u32(0x800ED5D4u, ((sint32)(collision_endpoint_word(endpoint, 2u))));
        v21 = ((sint32)(r_u32((a1 + (2) * 4u))));
    }
    else
    {
        w_u32(0x800ED5D4u, ((sint32)(r_u32((a1 + (2) * 4u)))));
        v21 = ((sint32)(collision_endpoint_word(endpoint, 2u)));
    }
    w_u32(0x800ED5E0u, v21);
    if (a4)
    {
        v42 = vector;
        do
        {
            if (((r_u16(((uint32)(((uint32)(a4) + (uint32)(78))))) & 0x40) != 0))
                goto LABEL_30;
            if ((a4 == a5))
                goto LABEL_30;
            v22 = r_u32(((uint32)(((uint32)(a4) + (uint32)(4)))));
            w_u32(0x800FF9F0u, ((uint32)(r_u16(((uint32)(((uint32)(a4) + (uint32)(212)))))) << (uint32)(12)));
            if ((((sint32)(((uint32)(v22) + (uint32)(r_u32(0x800FF9F0u))))) < (sint32)r_u32(0x800ED5CCu)))
                goto LABEL_30;
            if (((sint32)r_u32(0x800ED5D8u) < ((sint32)(((uint32)(v22) - (uint32)(r_u32(0x800FF9F0u)))))))
                goto LABEL_30;
            v23 = r_u32(((uint32)(((uint32)(a4) + (uint32)(8)))));
            if ((((sint32)(((uint32)(v23) + (uint32)(r_u32(0x800FF9F0u))))) < (sint32)r_u32(0x800ED5D0u)))
                goto LABEL_30;
            if (((sint32)r_u32(0x800ED5DCu) < ((sint32)(((uint32)(v23) - (uint32)(r_u32(0x800FF9F0u)))))))
                goto LABEL_30;
            v24 = r_u32(((uint32)(((uint32)(a4) + (uint32)(12)))));
            if ((((sint32)(((uint32)(v24) + (uint32)(r_u32(0x800FF9F0u))))) < (sint32)r_u32(0x800ED5D4u)))
                goto LABEL_30;
            if (((sint32)r_u32(0x800ED5E0u) < ((sint32)(((uint32)(v24) - (uint32)(r_u32(0x800FF9F0u)))))))
                goto LABEL_30;
            xport_draft_host_sub_8006C3AC_p1(v35, ((uint32)(((uint32)(a4) + (uint32)(4)))), a1);
            v39 = 12;
            xport_draft_host_sub_8006C564_p123(vector, v35, &v39);
            w_u32(0x800ED5F0u, vector[0]);
            w_u32(0x800ED5F4u, vector[1]);
            w_u32(0x800ED5F8u, vector[2]);
            xport_draft_gte_data_write(11u, r_u32(0x800ED5F8u));
            xport_draft_gte_data_write(9u, r_u32(0x800ED5F0u));
            xport_draft_gte_data_write(10u, r_u32(0x800ED5F4u));
            xport_draft_gte_execute(0x178000Cu);
            xport_draft_gte_execute(0xA00428u);
            w_u32(0x800ED5E4u, xport_draft_gte_data_read(25u));
            w_u32(0x800ED5E8u, xport_draft_gte_data_read(26u));
            w_u32(0x800ED5ECu, xport_draft_gte_data_read(27u));
            w_u32(0x800FFA10u, ((uint32)(r_u16(((uint32)(((uint32)(a4) + (uint32)(212)))))) * (uint32)(r_u16(((uint32)(((uint32)(a4) + (uint32)(212))))))));
            if ((sint32)(r_u32(0x800ED5E4u) + r_u32(0x800ED5E8u) + r_u32(0x800ED5ECu)) >= (sint32)r_u32(0x800FFA10u))
                goto LABEL_30;
            xport_draft_gte_data_write(0u, r_u16(0x800ED5F0u) | ((uint32)r_u16(0x800ED5F4u) << 16));
            xport_draft_gte_data_write(1u, r_u32(0x800ED5F8u));
            xport_draft_gte_execute(0x4A6012u);
            w_u32(0x800FFA18u, xport_draft_gte_data_read(25u));
            if ((sint32)r_u32(0x800FFA18u) >= (sint32)r_u32(0x800ED634u))
                goto LABEL_30;
            if ((sint32)r_u32(0x800FFA18u) >= 0)
            {
                if ((sint32)r_u32(0x800FFA14u) >= (sint32)r_u32(0x800FFA18u))
                    goto LABEL_29;
                for (unsigned axis = 0u; axis < 3u; ++axis)
                {
                    uint32 object_component = r_u32(a4 + 4u + axis * 4u);
                    v35[axis] = (sint32)(object_component - collision_endpoint_word(endpoint, axis));
                }
                v40 = 12;
                xport_draft_host_sub_8006C564_p123(v42, v35, &v40);
                w_u32(0x800ED5FCu, vector[0]);
                w_u32(0x800ED600u, vector[1]);
                w_u32(0x800ED604u, vector[2]);
                xport_draft_gte_data_write(9u, r_u32(0x800ED5FCu));
                xport_draft_gte_data_write(10u, r_u32(0x800ED600u));
                xport_draft_gte_data_write(11u, r_u32(0x800ED604u));
                xport_draft_gte_execute(0xA00428u);
                w_u32(0x800ED5E4u, xport_draft_gte_data_read(25u));
                w_u32(0x800ED5E8u, xport_draft_gte_data_read(26u));
                w_u32(0x800ED5ECu, xport_draft_gte_data_read(27u));
            }
            else
            {
                xport_draft_gte_data_write(9u, r_u32(0x800ED5F0u));
                xport_draft_gte_data_write(10u, r_u32(0x800ED5F4u));
                xport_draft_gte_data_write(11u, r_u32(0x800ED5F8u));
                xport_draft_gte_execute(0xA00428u);
                w_u32(0x800ED5E4u, xport_draft_gte_data_read(25u));
                w_u32(0x800ED5E8u, xport_draft_gte_data_read(26u));
                w_u32(0x800ED5ECu, xport_draft_gte_data_read(27u));
            }
            if ((sint32)(r_u32(0x800ED5E4u) + r_u32(0x800ED5E8u) + r_u32(0x800ED5ECu)) < (sint32)r_u32(0x800FFA10u))
            {
            LABEL_29:
                w_u32(0x800FFA84u, a4);

                w_u32(0x800ED634u, r_u32(0x800FFA18u));
            }
        LABEL_30:
            a4 = r_u32(((uint32)(((uint32)(a4) + (uint32)(28)))));

        } while (a4);
    }
    result = r_u32(0x800FFA84u);
    if (r_u32(0x800FFA84u))
    {
        xport_draft_gte_data_write(9u, r_u32(0x800ED5C0u));
        xport_draft_gte_data_write(10u, r_u32(0x800ED5C4u));
        xport_draft_gte_data_write(11u, r_u32(0x800ED5C8u));
        xport_draft_gte_data_write(8u, r_u32(0x800ED634u));
        xport_draft_gte_execute(0x198003Du);
        uint32 component;
        component = xport_draft_gte_data_read(25u);
        memcpy((uint8 *)a3, &component, sizeof(component));
        component = xport_draft_gte_data_read(26u);
        memcpy((uint8 *)a3 + 4u, &component, sizeof(component));
        component = xport_draft_gte_data_read(27u);
        memcpy((uint8 *)a3 + 8u, &component, sizeof(component));
        v41 = 12;
        xport_draft_host_sub_8006C22C_p12(a3, &v41);
        xport_draft_host_sub_8006C0B8_p1(a3, a1);
        return r_u32(0x800FFA84u);
    }
    return result;
}

/* TODO Missing call adapter xport_draft_missing_gte_adapter */

uint32 sub_800666DC(uint32 output, uint32 input)
{
    uint32 x = r_u32(input), y = r_u32(input + 4u), z = r_u32(input + 8u);
    sint32 divisor;
    uint32 result;
    xport_draft_gte_data_write(9u, (uint32)((sint32)x >> 12));
    xport_draft_gte_data_write(10u, (uint32)((sint32)y >> 12));
    xport_draft_gte_data_write(11u, (uint32)((sint32)z >> 12));
    xport_draft_gte_execute(0xA00428u);
    x = xport_draft_gte_data_read(25u);
    y = xport_draft_gte_data_read(26u);
    z = xport_draft_gte_data_read(27u);
    divisor = (sint32)sub_80085B54(x + y + z);
    if (!divisor)
    {
        w_u32(output, 0u);
        w_u32(output + 4u, 0u);
        w_u32(output + 8u, 0u);
        return 0u;
    }
    x = r_u32(input);
    result = x == 0x80000000u && divisor == -1 ? x : (uint32)((sint32)x / divisor);
    w_u32(output, result);
    y = r_u32(input + 4u);
    result = y == 0x80000000u && divisor == -1 ? y : (uint32)((sint32)y / divisor);
    w_u32(output + 4u, result);
    z = r_u32(input + 8u);
    result = z == 0x80000000u && divisor == -1 ? z : (uint32)((sint32)z / divisor);
    w_u32(output + 8u, result);
    return result;
}

uint32 sub_8007CAC8(uint32 a1, uint32 a2)
{
    sint32 v3;
    sint32 v4;
    sint32 result;
    sint32 v6;
    sint32 v7;
    uint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    v3 = r_u16(((uint32)(((uint32)(a2) + (uint32)(2)))));
    v4 = r_u8(((uint32)(((uint32)(a1) + (uint32)(27)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(364)))), ((uint32)(a2) + (uint32)(4)));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(356)))), sub_8006B864(((uint32)(24) * (uint32)(r_u32(((uint32)(((uint32)(r_u32((0x800EAEF8u + (((uint32)(((uint32)(16) * (uint32)(v4))) + (uint32)(4))) * 4u))) - (uint32)(4))))))), 0, 1));
    result = sub_8006B864(((uint32)(12) * (uint32)(v3)), 0, 1);
    v6 = 0;
    w_u32(((uint32)(((uint32)(a1) + (uint32)(360)))), result);
    if (v3)
    {
        v7 = 0;
        do
        {
            ++v6;
            v8 = ((uint32)(((uint32)(v7) + (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(360)))))))));
            w_u16((v8 + (5) * 2u), 0);
            w_u16((v8 + (4) * 2u), 0);
            w_u16((v8 + (3) * 2u), 0);
            w_u16((v8 + (2) * 2u), 0);
            w_u16((v8 + (1) * 2u), 0);
            w_u16(v8, 0);
            result = (((sint32)(v6)) < ((sint32)(v3)));
            v7 += 12;
        } while ((((sint32)(v6)) < ((sint32)(v3))));
    }
    v9 = 0;
    if (v3)
    {
        v10 = 0;
        do
        {
            v11 = 0;
            v12 = 0;
            while (1)
            {
                v13 = r_u32(((uint32)(((uint32)(a1) + (uint32)(364)))));
                if ((r_u16(((uint32)(((uint32)(((sint32)(((uint32)(v10) + (uint32)(v13))))) + (uint32)(2))))) == r_u16(((uint32)(((sint32)(((uint32)(v12) + (uint32)(v13)))))))))
                    break;
                ++v11;
                v12 += 12;
                if ((((sint32)(v11)) >= ((sint32)(v3))))
                    goto LABEL_10;
            }

            w_u16(((uint32)(((uint32)(((sint32)(((uint32)(v10) + (uint32)(v13))))) + (uint32)(10)))), v11);
        LABEL_10:
            if ((v11 == v3))
                w_u16(((uint32)(((uint32)(((uint32)(v10) + (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(364)))))))) + (uint32)(10)))), -1);

            result = (++v9 < ((sint32)(v3)));
            v10 += 12;
        } while ((((sint32)(v9)) < ((sint32)(v3))));
    }
    return result;
}

/* TODO Missing host buffer adapter xport_draft_host_sub_8007BB24_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8007DD04_p1 */
uint32 sub_8005F470(uint32 a1)
{
    sint32 result;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;

    union
    {
        uint32 words[35];

        struct
        {
            sint32 origin[3], target[3];
            uint8 workspace[0x68 - 6 * sizeof(sint32)];
            uint32 hit;
            sint32 position[3];
            uint16 rotation[3];
        } fields;
    } ray;

    result = (r_u32(((uint32)(((uint32)(a1) + (uint32)(460))))) & 0x3000);
    if (result)
        goto LABEL_8;
    if (((r_u16(((uint32)(a1))) & 8) != 0))
    {
        v3 = r_u32(((uint32)(((uint32)(a1) + (uint32)(4)))));
        w_u8(((uint32)(((uint32)(a1) + (uint32)(314)))), 1);
        v4 = r_u32(0x800FF37Cu);
        v5 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
        result = -4096;
        w_u16(((uint32)(((uint32)(a1) + (uint32)(164)))), 0);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(166)))), 61440);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(184)))), v3);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(188)))), v4);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(192)))), v5);
        return result;
    }
    if (((r_u16(((uint32)(((uint32)(a1) + (uint32)(216))))) & 2) != 0))
    {
        w_u8(((uint32)(((uint32)(a1) + (uint32)(314)))), 1);
        result = ((uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(582)))))) << (uint32)(12));
        w_u32(((uint32)(((uint32)(a1) + (uint32)(188)))), ((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))))) + (uint32)(result)));
        return result;
    }
    ray.fields.origin[0] = r_u32(((uint32)(((uint32)(a1) + (uint32)(4)))));
    ray.fields.origin[1] = ((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))))) - (uint32)(0x40000));
    v6 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
    ray.fields.target[0] = ray.fields.origin[0];
    ray.fields.target[1] = ((uint32)(ray.fields.origin[1]) + (uint32)(0x800000));
    ray.fields.origin[2] = v6;
    ray.fields.target[2] = v6;
    xport_draft_host_sub_8007BB24_p1(&ray);
    xport_draft_host_sub_8007DD04_p1(&ray, 1);
    result = 1;
    if (!ray.fields.hit)
    {
    LABEL_8:
        w_u8(((uint32)(((uint32)(a1) + (uint32)(314)))), 0);
    }
    else
    {
        w_u8(((uint32)(((uint32)(a1) + (uint32)(314)))), 1);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(184)))), ray.fields.position[0]);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(188)))), ray.fields.position[1]);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(192)))), ray.fields.position[2]);
        w_u16(((uint32)(((uint32)(a1) + (uint32)(164)))), ray.fields.rotation[0]);
        w_u16(((uint32)(((uint32)(a1) + (uint32)(166)))), ray.fields.rotation[1]);
        result = ray.fields.rotation[2];
        w_u16(((uint32)(((uint32)(a1) + (uint32)(168)))), ray.fields.rotation[2]);
    }
    return result;
}

uint32 sub_80063CF0(void)
{
    sint32 i;
    sint32 result;
    for (i = r_u32(0x800FF630u); i; i = r_u32(((uint32)(((uint32)(i) + (uint32)(20))))))
    {
        result = r_u8(((uint32)(((uint32)(i) + (uint32)(5)))));
        if (r_u8(((uint32)(((uint32)(i) + (uint32)(5))))))
        {
            result = r_u8(((uint32)(((uint32)(i) + (uint32)(4)))));
            if (!(r_u8(((uint32)(((uint32)(i) + (uint32)(4)))))))
                w_u8(((uint32)(((uint32)(i) + (uint32)(5)))), 0);
        }
    }

    return result;
}

uint32 sub_80069EF4(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 v4;
    uint32 v5;
    if ((((sint32)(a1)) < 0))
        return 0;
    v4 = sub_80067E9C(a2, 256, 12000);
    v5 = (((uint32)((v4 & 0xFFFu)) * (uint32)(r_u16(0x800ECC7Au))) >> 12);
    if ((((sint32)(v4)) < 0))
        v5 = 0u - v5;
    return sub_8006A4C4(r_u32((0x800E53A8u + (((uint32)(2) * (uint32)(a1))) * 4u)), r_u8((((uint32)(0x800E53A8u)) + ((((uint32)(8) * (uint32)(a1)) | 4)) * 1u)), ((short)(v5)), ((short)((((uint32)(((uint32)((((v4 >> 16) & 65535u) & 0xFFF)) * (uint32)(r_u16(0x800ECC7Au))))) >> 12))), a3);
}

void apocalypse_mark_array_collision_candidates(uint32 list, const sint32 bounds[6], uint32 mark)
{
    sint32 lower[3], upper[3], first[3], second[3];
    uint32 object = r_u32(list), flip_bits, table, geometry, axis;
    uint32 translation[3];
    xport_draft_bounds reference, result;
    if (!object)
        return;
    for (axis = 0u; axis < 3u; ++axis)
    {
        lower[axis] = bounds[axis] >> 12;
        upper[axis] = bounds[axis + 3u] >> 12;
    }
    flip_bits = 0u;
    sub_800847AC(lower, upper, &flip_bits);
    table = r_u32(0x800EAEF8u + 64u * r_u8(object + 27u) + 16u);
    do
    {
        uint32 flags = r_u32(object);
        if (flags & 0x21u)
            w_u16(object + 2u, mark);
        else if ((flags >> 16) != mark)
        {
            geometry = r_u32(table + 4u * r_u16(object + 22u));
            for (axis = 0u; axis < 3u; ++axis)
            {
                translation[axis] = (uint32)((sint32)r_u32(object + 4u + axis * 4u) >> 12);
                reference.minimum[axis] = (uint32)lower[axis];
                reference.maximum[axis] = (uint32)upper[axis];
            }
            sub_80084814(geometry, flip_bits, &reference, translation, &result);
            for (axis = 0u; axis < 3u; ++axis)
            {
                first[axis] = (sint32)result.minimum[axis];
                second[axis] = (sint32)result.maximum[axis];
            }
            if (!sub_800848D0(lower, upper, first, second))
                w_u16(object + 2u, mark);
        }
        list += 4u;
        object = r_u32(list);
    } while (object);
}

void sub_80084C50(uint32 list, uint32 unused, uint32 guest_bounds, uint32 mark)
{
    sint32 bounds[6];
    uint32 axis;
    FUNCTION_MARKER(0x80084C50u, "SLUS_003.73");
    if (!r_u32(list))
        return;
    for (axis = 0u; axis < 6u; ++axis)
        bounds[axis] = (sint32)r_u32(guest_bounds + axis * 4u);
    apocalypse_mark_array_collision_candidates(list, bounds, mark);
}

/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_800847AC(sint32 low[3], sint32 high[3], uint32 *flip_bits)
{
    uint32 axis;
    for (axis = 0; axis < 3u; ++axis)
    {
        uint32 delta = (uint32)high[axis] - (uint32)low[axis];
        if ((sint32)delta < 0)
        {
            sint32 temporary = low[axis];
            low[axis] = high[axis];
            high[axis] = temporary;
            delta = (uint32)high[axis] - (uint32)low[axis];
            *flip_bits ^= 1u << axis;
        }
        xport_draft_gte_control_write(axis * 2u, delta);
    }
}

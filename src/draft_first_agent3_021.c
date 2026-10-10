#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <string.h>

uint32 sub_800115C0(uint32 a1)
{
    sint32 v1;
    sint32 result;
    result = sub_800155D4(a1, r_u32((0x800A55A4u + (0) * 4u)));
    if (!result)
    {
        if (sub_800155D4(a1, r_u32((0x800A55A8u + (0) * 4u))))
            v1 = ((((sint32)(((((unsigned long long)(((296 * r_u16(0x800ECC7Au)) * ((long long)(((sint32)0x80020009u)))))) >> 32) + (296 * r_u16(0x800ECC7Au))))) >> 13) - ((296 * r_u16(0x800ECC7Au)) >> 31));
        if (sub_800155D4(a1, r_u32((0x800A55ACu + (0) * 4u))))
            v1 = ((296 * r_u16(0x800ECC78u)) / 127);
        if (sub_800155D4(a1, r_u32((0x800A55B0u + (0) * 4u))))
            v1 = ((296 * r_u16(0x800ECC7Cu)) / 255);
        sub_8006D1C0((v1 + 108), 41, (296 - v1), 10, 0u, 0u, 0u, 0u);
        return sub_8006D1C0(106, 40, 300, 12, 100u, 0u, 0u, 0u);
    }
    return result;
}

void sub_800905AC(void)
{
    w_u16(0x801055C0u, 0);
}

uint32 sub_8007FED0(uint32 a1)
{
    uint32 v1;
    uint32 v2;
    sint32 v3;
    sint8 v4;
    v1 = r_u8(((uint32)((a1 + 2))));
    v2 = (v1 >> 4);
    v3 = ((v1 >> 4) != 0);
    v4 = (v1 & 0xF);
    if (v3)
        return ((sint16)(r_u16((((uint32)((0x800F5130u + ((32 * v2)) * 4u))) + (((((uint8)(r_u32(0x800FFB48u))) + (4 * v4)) & 0x3F)) * 2u))));
    else
        return 0;
}

uint32 sub_8006A428(void)
{
    uint32 result, volume;
    if (r_u32(0x800FF6B4u))
    {
        apocalypse_sequence_replay((uint32)(sint32)(sint16)r_u16(0x800FF6ACu));
    }
    else if (r_u32(0x800FF6B8u))
    {
        apocalypse_sequence_play((uint32)(sint32)(sint16)r_u16(0x800FF6ACu), 1u, 0u);
    }
    volume = (uint32)(sint32)(sint16)r_u16(0x800ECC78u);
    result = apocalypse_sequence_volume((uint32)(sint32)(sint16)r_u16(0x800FF6ACu), volume, volume);
    w_u32(0x800FF6B4u, 0);
    w_u32(0x800FF6B8u, 0);
    return result;
}

uint32 sub_8008D51C(uint32 a1, uint32 a2)
{
    return apocalypse_sequence_tick((uint32)(sint32)(sint16)a1, (uint32)(sint32)(sint16)a2);
}

/* TODO Missing call adapter sub_80097DF8 */
/* TODO Missing call adapter sub_80098258 */
static void cd_2FD2C_control_97F34(uint32 command, uint32 parameter)
{
    (void)sub_80097F34(command, parameter);
}

uint32 sub_8002FD2C(uint32 a1, uint32 a2)
{
    sint8 v3;
    sint32 result;
    uint32 v5;
    sint32 v6;
    char v7[8];
    char v8[8];
    char v9[8];
    v3 = a2;
    result = (((sint32)(a1)) < 22);
    if ((((sint32)(a1)) < 22))
    {
        v5 = (0x800A5FD8u + (((16 * a1) + a2)) * 4u);
        result = -1;
        if ((((sint32)(r_u32(v5))) != -1))
        {
            result = -2146500608;
            if (!r_u32(0x800FF700u))
            {
                result = ((unsigned char)(r_u8((0x800EADC5u + ((16 * r_u32(0x800FF79Cu))) * 1u))));
                if (!r_u8((0x800EADC5u + ((16 * r_u32(0x800FF79Cu))) * 1u)))
                {
                    result = r_u32(0x800FF250u);
                    if (!r_u32(0x800FF250u))
                    {
                        result = r_u32(0x800FF2B0u);
                        if (!r_u32(0x800FF2B0u))
                        {
                            result = r_u32(0x800FF2BCu);
                            if (r_u32(0x800FF2BCu))
                            {
                                if ((r_u32(0x800FF2C0u) || sub_8002FAD0(), (result = r_u32(0x800FF2C0u) != 0)))
                                {
                                    v7[0] = 1;
                                    v7[1] = v3;
                                    xport_draft_host_sub_80097DF8_p2(13u, v7, 0x800FF2D8u);
                                    v6 = ((sint32)(r_u32(v5)));
                                    w_u32(0x800FF2D4u, (r_u32(0x800FF2ACu) + (16 * r_u32((0x800A5F80u + (a1) * 4u)))));
                                    w_u32(0x800FF2D0u, (r_u32(0x800FF2D4u) + (16 * v6)));
                                    xport_draft_host_sub_80098258_p2(r_u32(0x800FF2D4u), v8);
                                    v9[2] = 0;
                                    v9[3] = 0;
                                    v9[0] = r_u8(0x800ECC7Cu);
                                    v9[1] = r_u8(0x800ECC7Cu);
                                    xport_draft_host_sub_800981B4_p1(v9);
                                    cd_2FD2C_control_97F34(12u, 0u);
                                    xport_draft_host_sub_80097DF8_p2(27u, v8, 0x800FF2D8u);
                                    result = 1;
                                    if (r_u8(0x800FF2D8u))
                                    {
                                        w_u32(0x800FF2B8u, 1);
                                        w_u32(0x800FF2BCu, 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

uint32 sub_8008DF18(uint32 a1)
{
    return sub_8008DD9C(a1);
}

/* TODO Missing call adapter sub_8009282C */
/* TODO Missing call adapter sub_80092EA4 */
static void sequence_8DD9C_volume_9282C(sint16 sequence, uint32 left, uint32 right, uint32 mode)
{
    /* TODO PsyQ sequence volume service */
    abort();
}

static void sequence_8DD9C_stop_92EA4(sint16 sequence)
{
    /* TODO PsyQ sequence stop service */
    abort();
}

uint32 sub_8008DD9C(uint32 a1)
{
    sint32 v1;
    sint32 result;
    sint32 v3;
    uint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    v1 = (sint16)a1;
    sequence_8DD9C_volume_9282C((sint16)a1, 0, 0, 1);
    sequence_8DD9C_stop_92EA4((sint16)v1);
    result = (r_u32(0x80104588u) & ~((1u << ((uint32)v1 & 31u))));
    w_u32(0x80104588u, result);
    v3 = 0;
    if (((sint16)r_u16(0x80104E12u) > 0))
    {
        v4 = (0x80104590u + (v1) * 4u);
        v5 = 0;
        do
        {
            w_u32(((uint32)(((v5 + ((sint32)(r_u32(v4)))) + 152))), 0);
            w_u8((uint32)v5 + r_u32(v4) + 34u, 255u);
            w_u8((uint32)v5 + r_u32(v4) + 35u, 0u);
            w_u16(((uint32)(((v5 + ((sint32)(r_u32(v4)))) + 72))), 0);
            w_u16(((uint32)(((v5 + ((sint32)(r_u32(v4)))) + 74))), 0);
            w_u32(((uint32)(((v5 + ((sint32)(r_u32(v4)))) + 156))), 0);
            w_u32(((uint32)(((v5 + ((sint32)(r_u32(v4)))) + 160))), 0);
            w_u16(((uint32)(((v5 + ((sint32)(r_u32(v4)))) + 76))), 0);
            w_u32(((uint32)(((v5 + ((sint32)(r_u32(v4)))) + 172))), 0);
            w_u32(((uint32)(((v5 + ((sint32)(r_u32(v4)))) + 168))), 0);
            w_u32(((uint32)(((v5 + ((sint32)(r_u32(v4)))) + 164))), 0);
            w_u16(((uint32)(((v5 + ((sint32)(r_u32(v4)))) + 78))), 0);
            ++v3;
            w_u16(((uint32)(((v5 + ((sint32)(r_u32(v4)))) + 88))), 127);
            v6 = (v5 + ((sint32)(r_u32(v4))));
            v5 += 176;
            result = (v3 < (sint16)r_u16(0x80104E12u));
            v7 = (v3 < (sint16)r_u16(0x80104E12u));
            w_u16(((uint32)((v6 + 90))), 127);
        } while (v7);
    }
    return result;
}

/* TODO Missing call adapter sub_800936DC */
static void soundbank_69CF8_close_936DC(sint16 bank)
{
    /* TODO PsyQ sound bank close service */
    abort();
}

uint32 sub_80069CF8(uint32 a1, uint32 a2)
{
    soundbank_69CF8_close_936DC((sint16)a1);
    return sub_8006BC20(a2);
}

uint32 sub_80011990(uint32 a1, uint32 a2)
{
    uint32 result;
    result = 0x800A0390u;
    w_u32(a1, 0x800A0390u);
    if (((a2 & 1) != 0))
        return ((uint32)(sub_8002FF34(a1)));
    return result;
}

/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8006E394(uint32 a1)
{
    uint32 v1;
    sint32 v2;
    uint32 result;
    uint32 v4;
    uint32 v5;
    uint32 v6;
    v1 = (0x800EAEF8u + ((16 * a1)) * 4u);
    v2 = r_u32(((uint32)((((sint32)(r_u32((v1 + (4) * 4u)))) - 4))));
    result = (sub_8006E360(((uint32)(((sint32)(r_u32((v1 + (5) * 4u))))))) + ((v2 + 1)) * 4u);
    v4 = r_u32((result - (1) * 4u));
    v5 = 0;
    if (v4)
    {
        v6 = result;
        do
        {
            ++v5;
            (w_u16(((uint32)((r_u32(v6) + 16))), (r_u16(((uint32)((r_u32(v6) + 16)))) - 1u)), (r_u16(((uint32)((r_u32(v6) + 16)))) - 1u));
            result = ((uint32)((v5 < v4)));
            (v6 += 4u);
        } while ((v5 < v4));
    }
    return result;
}

uint32 sub_8006ECAC(uint32 a1)
{
    sint32 v1;
    sint32 v2;
    sint32 v3;
    sint32 result;
    sint32 v5;
    v1 = a1;
    w_u32((0x800FF78Cu + (v1) * 4u), 0);
    v2 = r_u32((0x800FF778u + ((a1 ^ 1)) * 4u));
    v3 = (r_u32((0x800EAEF8u + (((16 * r_u32((0x800FF778u + (v1) * 4u))) + 5)) * 4u)) + 12);
    result = -1;
    v5 = 0;
    if ((v2 != -1))
    {
        result = r_u32((0x800EAEF8u + (((16 * v2) + 5)) * 4u));
        v5 = (result + 12);
    }
    if (v5)
    {
        result = r_u32(0x800FF794u);
        if ((v3 == r_u32(0x800FF794u)))
        {
            w_u32(0x800FF794u, v5);
        }
        else
        {
            while (1)
            {
                result = r_u32(((uint32)((v5 + 28))));
                if ((result == v3))
                    break;
                v5 = r_u32(((uint32)((v5 + 28))));
            }

            w_u32(((uint32)((v5 + 28))), 0);
        }
    }
    else
    {
        w_u32(0x800FF794u, 0);
    }
    return result;
}

uint32 sub_80068DC4(uint32 a1, uint32 a2)
{
    uint32 result;
    sint32 v3;
    sint32 v4;
    uint16 v5;
    uint16 v6;
    uint32 v7;
    a1 = (uint32)(sint32)(sint16)a1;
    a2 = (uint32)(sint32)(sint16)a2;
    for (result = r_u32(0x800FF678u); result; result = ((uint32)(r_u32((((uint32)(result)) + (2) * 4u)))))
    {
        v3 = r_u16(result);
        if ((v3 == a1))
        {
            v4 = r_u16((result + (1) * 2u));
            if ((v4 == a2))
            {
                v5 = r_u16((result + (2) * 2u));
                v6 = r_u16((result + (3) * 2u));
                sub_8006838C(0x800FF678u, ((sint32)(result)));
                v7 = sub_800682CC(v3, v4, v5, v6);
                return ((uint32)(sub_800690A0(v7)));
            }
        }
    }

    return result;
}

uint32 sub_8006E1EC(uint32 a1)
{
    sint32 v1;
    sint32 v2;
    uint32 v3;
    v1 = r_u32((a1 + (8) * 4u));
    if (v1)
        w_u32(((uint32)((v1 + 36))), r_u32((a1 + (9) * 4u)));
    v2 = r_u32((a1 + (9) * 4u));
    if (v2)
        w_u32(((uint32)((v2 + 32))), r_u32((a1 + (8) * 4u)));
    v3 = (0x800EB8F8u + ((r_u32((a1 + (5) * 4u)) & 0x1FF)) * 4u);
    if ((a1 == ((uint32)(((sint32)(r_u32(v3)))))))
        w_u32(v3, r_u32((a1 + (8) * 4u)));
    return sub_8006BC20(((sint32)(a1)));
}

/* TODO Resolve original data label 0x800FF634u */
static uint32 trigger_position_cursor(uint32 index)
{
    uint32 record = r_u32(r_u32(0x800FF624u) + index * 4u);
    uint32 cursor, type = r_u16(record);
    switch (type)
    {
        case 1u:
        case 7u:
            cursor = record + 8u + r_u16(record + 6u) * 2u;
            while (r_u8(cursor) != 255u)
                ++cursor;
            return (cursor + 4u) & 0xFFFFFFFCu;
        case 3u:
        case 8u:
        case 10u:
        case 1000u:
        case 1001u:
            return (record + r_u16(record + 2u) * 2u + 7u) & 0xFFFFFFFCu;
        case 5u:
            return (record + r_u16(record + 4u) * 2u + 9u) & 0xFFFFFFFCu;
        case 500u:
        case 501u:
            return (record + 5u) & 0xFFFFFFFCu;
        default:
            /* TODO Unsupported record types return inherited guest V1 without output */
            abort();
    }
}

uint32 apocalypse_trigger_position(uint32 output[3], uint32 index)
{
    uint32 cursor = trigger_position_cursor(index), axis;
    for (axis = 0u; axis < 3u; ++axis)
        output[axis] = r_u32(cursor + axis * 4u) << 12;
    return cursor + 12u;
}

uint32 sub_8006613C(uint32 destination, uint32 index)
{
    uint32 cursor = trigger_position_cursor(index), axis;
    for (axis = 0u; axis < 3u; ++axis)
        w_u32(destination + axis * 4u, r_u32(cursor + axis * 4u) << 12);
    return cursor + 12u;
}

uint32 sub_80063F4C(uint32 basename)
{
    char filename[32], *destination = filename;
    uint32 suffix = 0x800FF634u, value, file_size, header, cursor, index, record;
    uint32 position[4], end, position_end, count;
    w_u32(0x800FF308u, 0u);
    w_u32(0x800FF30Cu, 0u);
    while ((value = r_u8(basename)) != 0u)
    {
        ++basename;
        *destination++ = (char)value;
    }
    while ((value = r_u8(suffix)) != 0u)
    {
        ++suffix;
        *destination++ = (char)value;
    }
    *destination = '\0';
    file_size = xport_draft_host_sub_8006B04C_p1(filename);
    header = sub_8006B864(file_size, 0u, 1u);
    w_u32(0x800FF61Cu, header);
    sub_8006B234(header);
    sub_8006B44C();
    header = r_u32(0x800FF61Cu);
    count = r_u16(header + 8u);
    cursor = header + 12u;
    w_u32(0x800FF624u, cursor);
    w_u32(0x800FF628u, count);
    if (count)
    {
        index = 0u;
        do
        {
            w_u32(cursor, r_u32(cursor) + r_u32(0x800FF61Cu));
            ++index;
            count = r_u32(0x800FF628u);
            cursor += 4u;
        } while ((sint32)index < (sint32)count);
    }
    end = r_u32(r_u32(0x800FF624u) + r_u32(0x800FF628u) * 4u - 4u);
    if ((sint16)r_u16(end) == 255)
    {
        header = r_u32(0x800FF61Cu);
        sub_8006BD14(header, (end - header + 5u) & 0xFFFFFFFCu);
    }
    sub_8006EE2C();
    w_u32(0x800FF618u, 0u);
    for (index = 0u; (sint32)index < (sint32)r_u32(0x800FF628u); ++index)
    {
        record = r_u32(r_u32(0x800FF624u) + index * 4u);
        if ((sint16)r_u16(record) == 8)
        {
            position_end = apocalypse_trigger_position(position, index);
            count = r_u32(0x800FF618u);
            w_u32(0x800A71FCu + count * 4u, position_end + 6u);
            w_u32(0x800FF618u, count + 1u);
        }
    }
    w_u32(0x800FF620u, 65535u);
    for (index = 0u; (sint32)index < (sint32)r_u32(0x800FF628u); ++index)
    {
        record = r_u32(r_u32(0x800FF624u) + index * 4u);
        if ((sint16)r_u16(record) == 4)
            sub_8006541C(record + 2u, index, 1u);
    }
    return sub_80065DE8();
}

/* TODO Missing call adapter sub_800174A8 */
/* TODO Missing call adapter sub_8002FB18 */
/* TODO Missing call adapter sub_8002FBE4 */
/* TODO Missing call adapter sub_8005F62C */
/* TODO Missing call adapter sub_80064B24 */
/* TODO Missing call adapter sub_80064FC8 */
/* TODO Missing call adapter sub_8006A490 */
/* TODO Missing call adapter sub_80076B78 */
/* TODO Missing call adapter sub_80076C10 */
/* TODO Missing call adapter sub_800774AC */
/* TODO Missing call adapter sub_80077624 */
/* TODO Missing call adapter sub_80078728 */
uint32 sub_80064FC8(uint32 argument1, uint32 argument2);
uint32 sub_80064B24(uint32 argument1, uint32 argument2);
uint32 sub_80076B78(uint32 argument1);
uint32 sub_8002FB18(uint32 argument1);
uint32 sub_8002FBE4(uint32 argument1);
uint32 sub_8006A490(uint32 argument1);
uint32 sub_800174A8(uint32 argument1);
uint32 sub_80077624(uint32 argument1, uint32 argument2);
uint32 sub_800774AC(uint32 argument1, uint32 argument2);
uint32 sub_80078728(uint32 argument1, uint32 argument2, uint32 argument3);
uint32 sub_8005F62C(uint32 argument1, uint32 argument2);

static void initialize_6541C_region_67C20(const uint32 origin[3], const uint32 extent[3])
{
    /* TODO Native region boundary 67C20 */
    abort();
}

static void initialize_6541C_region_67CCC(const uint32 origin[3], const uint32 extent[3], uint32 enabled)
{
    uint32 node = r_u32(0x800FF794u);
    while (node)
    {
        uint32 resource = 0x800EAEF8u + 64u * r_u8(node + 27u);
        if (r_u8(resource + 10u))
        {
            sint32 x = (sint32)r_u32(node + 4u);
            sint32 y = (sint32)r_u32(node + 8u);
            sint32 z = (sint32)r_u32(node + 12u);
            /* Extent carries the inclusive upper corner */
            if (x >= (sint32)origin[0] && (sint32)extent[0] >= x && z >= (sint32)origin[2] && (sint32)extent[2] >= z && y >= (sint32)origin[1] && (sint32)extent[1] >= y)
            {
                uint32 flags = r_u16(node);
                w_u16(node, enabled ? flags & 0xFFFEu : flags | 1u);
            }
        }
        node = r_u32(node + 28u);
    }
}

static void initialize_6541C_data_76C10(uint32 object, uint32 resource, const uint16 prefix[3])
{
    /* TODO Native six-byte data boundary 76C10 */
    abort();
}

uint32 sub_8006541C(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 v3;
    sint32 v4;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 result;
    unsigned short v13;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    uint32 v18;
    sint32 v19;
    uint32 v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    short v27;
    uint32 v28;
    sint32 v29;
    sint32 v30;
    sint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    uint32 v35;
    sint32 v36;
    sint32 v37;
    sint32 v38;
    uint32 v39;
    sint32 v40;
    sint32 v41;
    sint32 v42;
    sint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    short v50;
    uint32 v51;
    sint32 v52;
    uint32 v53;
    uint32 v54;
    uint32 v55;
    sint32 v56;
    sint32 v57;
    sint32 v58;
    sint32 v59;
    sint32 v60;
    sint32 v61;
    sint32 v62;
    sint32 v63;
    sint32 v64;
    sint32 v65;
    sint32 v66;
    sint32 v67;
    sint32 v68;
    sint32 v69;
    sint32 v70;
    sint32 v71;
    sint32 v72;
    sint32 v73;
    sint32 v74;
    sint32 v75;
    sint32 v76;
    uint32 origin[3];
    uint16 data_prefix[3];
    int v80[4];
    int v81[4];
    sint32 v82;
    v8 = 0;
    v10 = sub_80063C54(a2);
    while (1)
    {
        v11 = ((unsigned short)(((sint16)(r_u16(a1)))));
        result = 0xFFFF;
        (a1 += 2u);
        if ((v11 == 0xFFFF))
            break;
        v13 = v11;
        switch (v11)
        {
            case 2:
                w_u32(0x800FF618u, 0);
                while (1)
                {
                    v9 = ((sint32)(a1));
                    if (!(r_u8(((uint32)(a1)))))
                        break;
                    v14 = (r_u32(0x800FF618u) + 1);
                    w_u32((0x800A71FCu + (r_u32(0x800FF618u)) * 4u), ((sint32)(a1)));
                    w_u32(0x800FF618u, v14);
                    a1 = ((uint32)(sub_800650B4(((uint32)(a1)))));
                }

                goto LABEL_60;

            case 3:
                if (v10)
                {
                    if (r_u16(((uint32)((v10 + 8)))))
                    {
                        v51 = ((uint32)(sub_80066088(a2)));
                        sub_80064A08(v51);
                        v52 = r_u16(((uint32)((v10 + 8))));
                        if ((v52 != 0xFFFF))
                            w_u16(((uint32)((v10 + 8))), (v52 - 1));
                    }
                }
                else
                {
                    v53 = ((uint32)(sub_80066088(a2)));
                    sub_80064A08(v53);
                }
                continue;

            case 4:

            case 5:
                v54 = ((uint32)(sub_80066088(a2)));
                sub_80064FC8(v54, v13);
                continue;

            case 10:
                v55 = ((uint32)(sub_80066088(a2)));
                sub_80064E50(v55);
                continue;

            case 11:
                sub_80064C1C(a2, 0);
                continue;

            case 12:
                sub_80064C1C(a2, 1);
                continue;

            case 13:
                sub_80064B24(a2, r_u16(a1));
                goto LABEL_60;

            case 102:
                w_u32(0x800FF7BCu, 1);
                continue;

            case 103:
                w_u32(0x800FF7BCu, 0);
                continue;

            case 104:
                v3 = ((unsigned short)(((sint16)(r_u16(a1)))));
                v39 = ((uint32)((a1 + (1) * 2u)));
                v4 = r_u16((v39 += 2u, v39 - 2u));
                v40 = r_u16(v39);
                a1 = ((uint32)((v39 + (1) * 2u)));
                v8 = 1;
                v82 = v40;
                continue;

            case 105:
                v41 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                sub_80069DF0(v41, 0x2000, 0);
                continue;

            case 106:
                v42 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                sub_8006A294(v42);
                continue;

            case 119:
                goto LABEL_63;

            case 126:
                v18 = a1;
                v19 = 0;
                goto LABEL_14;

            case 127:
                sub_8006EDD4(a1);
                goto LABEL_63;

            case 128:
                v18 = a1;
                v19 = 1;
            LABEL_14:
                sub_8006F29C(v18, v19);

                if (a3)
                    sub_8006F7D8();
                goto LABEL_63;

            case 129:
                sub_8006F7D8();
                continue;

            case 130:
                v9 = r_u32(0x800FF904u);
                if (!r_u32(0x800FF904u))
                    goto LABEL_104;
                v58 = ((sint16)(r_u16(a1)));
                v59 = ((unsigned short)(((sint16)(r_u16((a1 + (1) * 2u))))));
                a1 += (2) * 2u;
                sub_8007851C(r_u32(0x800FF904u), v58, v59);
                continue;

            case 131:
                v37 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                sub_80076B78(v37);
                continue;

            case 132:
                v38 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                sub_80076BC4(v38);
                continue;

            case 133:
                while ((((sint16)(r_u16(a1))) != 255))
                {
                    v20 = ((uint32)(((((uint32)(a1)) + 3) & 0xFFFFFFFC)));
                    v21 = r_u32((v20 += 4u, v20 - 4u));
                    origin[0] = (uint32)v21 << 12;
                    v22 = r_u32((v20 += 4u, v20 - 4u));
                    origin[1] = (uint32)v22 << 12;
                    v23 = r_u32((v20 += 4u, v20 - 4u));
                    origin[2] = (uint32)v23 << 12;
                    v24 = r_u32((v20 += 4u, v20 - 4u));
                    v80[0] = (uint32)v24 << 12;
                    v25 = r_u32((v20 += 4u, v20 - 4u));
                    v80[1] = (uint32)v25 << 12;
                    v26 = ((sint32)(r_u32(v20)));
                    a1 = ((uint32)((v20 + (1) * 4u)));
                    v80[2] = (uint32)v26 << 12;
                    initialize_6541C_region_67C20(origin, (const uint32 *)v80);
                }

                goto LABEL_60;

            case 134:
                if (!(r_u8(((uint32)((v10 + 6))))))
                {
                    v50 = ((sint16)(r_u16(a1)));
                    w_u8(((uint32)((v10 + 6))), 1);
                    w_u16(((uint32)((v10 + 8))), v50);
                }
                goto LABEL_60;

            case 135:
                v9 = r_u32(0x800FF904u);
                if (!r_u32(0x800FF904u))
                    goto LABEL_104;
                v60 = ((unsigned short)(((sint16)(r_u16(a1)))));
                v61 = ((unsigned short)(((sint16)(r_u16((a1 + (1) * 2u))))));
                a1 += (2) * 2u;
                sub_800785D8(r_u32(0x800FF904u), v60, v61);
                continue;

            case 136:
                sub_8002FB18(1);
                continue;

            case 137:
                sub_8002FB18(0);
                continue;

            case 138:
                v75 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                sub_8002FBE4(v75);
                continue;

            case 139:
                sub_8002FD2C(r_u16(a1), r_u16(a1 + 2u));
                goto LABEL_104;

            case 140:

            case 176:
                v16 = r_u32(0x800FF620u);
                sub_80063DD4(((sint32)(a1)));
                a1 = ((uint32)(sub_800650B4(((uint32)(a1)))));
                if (((v16 != 0xFFFF) && (r_u32(0x800FF620u) != v16)))
                {
                    if ((v13 != 176))
                    {
                        sub_8001B708();
                        sub_8001B788(((sint32)(r_u32((0x800A5720u + (0) * 4u)))), ((sint32)(0x800A57A8u)));
                    }
                    sub_800189EC();
                }
                continue;

            case 141:
                v27 = r_u16((a1 += 2u, a1 - 2u));
                while ((((sint16)(r_u16(a1))) != 255))
                {
                    v28 = ((uint32)(((((uint32)(a1)) + 3) & 0xFFFFFFFC)));
                    v29 = r_u32((v28 += 4u, v28 - 4u));
                    origin[0] = (uint32)v29 << 12;
                    v30 = r_u32((v28 += 4u, v28 - 4u));
                    origin[1] = (uint32)v30 << 12;
                    v31 = r_u32((v28 += 4u, v28 - 4u));
                    origin[2] = (uint32)v31 << 12;
                    v32 = r_u32((v28 += 4u, v28 - 4u));
                    v81[0] = (uint32)v32 << 12;
                    v33 = r_u32((v28 += 4u, v28 - 4u));
                    v81[1] = (uint32)v33 << 12;
                    v34 = ((sint32)(r_u32(v28)));
                    a1 = ((uint32)((v28 + (1) * 4u)));
                    v81[2] = (uint32)v34 << 12;
                    initialize_6541C_region_67CCC(origin, (const uint32 *)v81, v27 != 0);
                }

                goto LABEL_60;

            case 142:
                w_u32(0x800FF62Cu, ((sint32)(a1)));
                a1 = ((uint32)(sub_800650B4(((uint32)(a1)))));
                w_u8(0x800FF644u, sub_8006ED70(r_u32(0x800FF62Cu)));
                continue;

            case 143:
                v9 = r_u32(0x800FF904u);
                if (!r_u32(0x800FF904u))
                    goto LABEL_104;
                v62 = ((sint16)(r_u16(a1)));
                v63 = ((unsigned short)(((sint16)(r_u16((a1 + (1) * 2u))))));
                a1 += (2) * 2u;
                sub_80078618(r_u32(0x800FF904u), v62, v63);
                continue;

            case 144:
                v9 = r_u32(0x800FF904u);
                if (!r_u32(0x800FF904u))
                    goto LABEL_104;
                v64 = ((sint16)(r_u16(a1)));
                v65 = ((unsigned short)(((sint16)(r_u16((a1 + (1) * 2u))))));
                a1 += (2) * 2u;
                sub_8007865C(r_u32(0x800FF904u), v64, v65);
                continue;

            case 145:
                v9 = r_u32(0x800FF904u);
                if (!r_u32(0x800FF904u))
                    goto LABEL_104;
                v66 = ((sint16)(r_u16(a1)));
                v67 = ((unsigned short)(((sint16)(r_u16((a1 + (1) * 2u))))));
                a1 += (2) * 2u;
                sub_800786A0(r_u32(0x800FF904u), v66, v67);
                continue;

            case 146:
                v9 = r_u32(0x800FF904u);
                if (!r_u32(0x800FF904u))
                    goto LABEL_104;
                v68 = ((sint16)(r_u16(a1)));
                v69 = ((unsigned short)(((sint16)(r_u16((a1 + (1) * 2u))))));
                a1 += (2) * 2u;
                sub_800786E4(r_u32(0x800FF904u), v68, v69);
                continue;

            case 147:
                w_u32(0x800FF378u, ((unsigned short)(r_u16((a1 += 2u, a1 - 2u)))));
                continue;

            case 148:
                v49 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                if ((r_u8(((uint32)((v10 + 7)))) == v49))
                    continue;
                if ((((sint16)(r_u16(a1))) == 149))
                    goto LABEL_60;
                do
                    a1 = ((uint32)(sub_800650EC(((uint32)(a1)))));
                while ((((sint16)(r_u16(a1))) != 149));
                (a1 += 2u);
                break;

            case 150:
                v43 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                sub_8006A490(v43);
                continue;

            case 151:
                sub_800653F4();
                v15 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                sub_800653B8((1000 * v15));
                continue;

            case 152:
                v9 = r_u32(0x800FF5A0u);
                if (r_u32(0x800FF5A0u))
                    sub_8005E708(r_u32(0x800FF5A0u));
                continue;

            case 153:
                v9 = r_u32(0x800FF904u);
                if (!r_u32(0x800FF904u))
                    goto LABEL_60;
                v70 = (uint32)(sint32)r_s16((a1 += 2u, a1 - 2u));
                sub_8007749C(r_u32(0x800FF904u), v70);
                continue;

            case 154:
                v9 = r_u32(0x800FF904u);
                if (!r_u32(0x800FF904u))
                    goto LABEL_60;
                v71 = (uint32)(sint32)r_s16((a1 += 2u, a1 - 2u));
                sub_800774A4(r_u32(0x800FF904u), v71);
                continue;

            case 155:
                v44 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                sub_8006A428();
                continue;

            case 156:
                v45 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                sub_8006A3C4();
                continue;

            case 157:
                v46 = r_u8(((uint32)((a1 += 2u, a1 - 2u))));
                sub_8006A334(v46);
                continue;

            case 158:
                sub_800174A8(1);
                continue;

            case 159:
                sub_80069BC4(a1);
            LABEL_63:
                a1 = ((uint32)(sub_800650B4(((uint32)(a1)))));

                continue;

            case 160:
                if (r_u32(0x800FF904u))
                    w_u32(((uint32)((r_u32(0x800FF904u) + 564))), ((unsigned short)(((sint16)(r_u16(a1))))));
                goto LABEL_60;

            case 161:
                v9 = r_u32(0x800FF904u);
                if (!r_u32(0x800FF904u))
                    goto LABEL_60;
                v74 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                sub_80077624(r_u32(0x800FF904u), v74);
                break;

            case 163:
                if (r_u32(0x800FF5A0u))
                {
                    v47 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                    w_u32(((uint32)((r_u32(0x800FF5A0u) + 452))), v47);
                }
                continue;

            case 164:
                v9 = r_u32(0x800FF904u);
                if (!r_u32(0x800FF904u))
                    goto LABEL_60;
                v72 = r_u16((a1 += 2u, a1 - 2u));
                sub_800774AC(r_u32(0x800FF904u), v72);
                break;

            case 165:
                v9 = r_u32(0x800FF904u);
                if (!r_u32(0x800FF904u))
                    goto LABEL_60;
                v73 = r_u16((a1 += 2u, a1 - 2u));
                sub_800774B4(r_u32(0x800FF904u), v73);
                break;

            case 166:
                w_u16(0x800FFAACu, r_u16((a1 += 2u, a1 - 2u)));
                continue;

            case 167:
                v9 = r_u32(0x800FF904u);
                if (!r_u32(0x800FF904u))
                    goto LABEL_104;
                v56 = ((unsigned short)(((sint16)(r_u16(a1)))));
                v57 = ((unsigned short)(((sint16)(r_u16((a1 + (1) * 2u))))));
                a1 += (2) * 2u;
                sub_80078728(r_u32(0x800FF904u), v56, v57);
                break;

            case 168:
                if (r_u32(0x800FF904u))
                    w_u16(((uint32)((r_u32(0x800FF904u) + 412))), ((sint16)(r_u16(a1))));
                goto LABEL_60;

            case 169:
                w_u16(0x800FFAAEu, r_u16((a1 += 2u, a1 - 2u)));
                continue;

            case 170:
                w_u32(0x800FF5ECu, ((unsigned short)(r_u16((a1 += 2u, a1 - 2u)))));
                continue;

            case 171:
                v35 = (a1 + 3u) & ~3u;
                v36 = r_u32(v35);
                data_prefix[0] = r_u16(v35 + 4u);
                data_prefix[1] = r_u16(v35 + 6u);
                data_prefix[2] = r_u16(v35 + 8u);
                a1 = v35 + 10u;
                v9 = sub_800625AC(300);
                if (v9)
                    initialize_6541C_data_76C10(v9, v36, data_prefix);
                continue;

            case 172:
                if (r_u32(0x800FF904u))
                    w_u16(((uint32)((r_u32(0x800FF904u) + 414))), ((sint16)(r_u16(a1))));
            LABEL_60:
                (a1 += 2u);

                break;

            case 173:
                if (r_u32(0x800FF904u))
                    w_u32(((uint32)((r_u32(0x800FF904u) + 336))), r_u32(((uint32)((r_u32(0x800FF904u) + 296)))));
                break;

            case 174:
                v9 = r_u32(0x800FF5A0u);
                if (r_u32(0x800FF5A0u))
                {
                    v76 = ((unsigned short)(((sint16)(r_u16((a1 + (1) * 2u))))));
                    w_u32(((uint32)((r_u32(0x800FF5A0u) + 484))), ((unsigned short)(((sint16)(r_u16(a1))))));
                    w_u32(((uint32)((v9 + 488))), v76);
                }
            LABEL_104:
                a1 += (2) * 2u;

                break;

            case 175:
                if (r_u32(0x800FF5A0u))
                    w_u32(((uint32)((r_u32(0x800FF5A0u) + 484))), 0);
                break;

            case 177:
                v9 = r_u32(0x800FF5A0u);
                if (r_u32(0x800FF5A0u))
                {
                    v48 = ((unsigned short)(r_u16((a1 += 2u, a1 - 2u))));
                    sub_8005F62C(r_u32(0x800FF5A0u), v48);
                }
                break;

            default:
                continue;
        }
    }

    if (v8)
        return sub_8007D76C(v3, v4, v82);
    return result;
}

uint32 sub_80064180(uint32 a1, uint32 a2)
{
    sint32 v2;
    v2 = r_u8(a2);
    if ((v2 == 255))
        return 0;
    while (1)
    {
        (a2 += 1u);
        if ((v2 == a1))
            break;
        v2 = r_u8(a2);
        if ((v2 == 255))
            return 0;
    }

    return 1;
}

uint32 sub_800625AC(uint32 a1)
{
    sint32 v2;
    uint32 v3;
    uint32 v4;
    uint32 i;
    v2 = sub_8006B864(a1, 0, 1);
    v3 = ((uint32)(v2));
    v4 = (((uint32)((a1 + 3))) >> 2);
    for (i = 0; (i < v4); (v3 += 4u))
    {
        w_u32(v3, 0);
        ++i;
    }

    return v2;
}

/* TODO Resolve original data label 0x800FF5B0u */
uint32 sub_8005C5C0(uint32 a1)
{
    sint32 v2;
    short v3;
    short v4;
    short v5;
    sint8 v6;
    short v7;
    sint32 v8;
    uint32 v9;
    uint32 v10;
    sint32 v11;
    sint32 v12;
    sub_80062F64(a1);
    w_u32((((uint32)(a1)) + (17) * 4u), 0x800A32A4u);
    w_u32(0x800FF5ACu, 0);
    w_u32(0x800FF5A4u, 0);
    w_u32(0x800FF5A8u, 0);
    w_u16((a1 + (74) * 2u), 2);
    sub_80062A38(a1, 0x800FF5A0u);
    v2 = r_u32(0x800FF59Cu);
    v3 = ((sint16)(r_u16((a1 + (39) * 2u))));
    w_u16((a1 + (106) * 2u), 100);
    w_u32(0x800FF59Cu, (v2 + 1));
    w_u16((a1 + (39) * 2u), (v3 & 0xFFFD));
    v4 = r_u16(0x800EC526u);
    v5 = ((sint16)(r_u16(a1)));
    w_u32((((uint32)(a1)) + (111) * 4u), 0x800EC0F8u);
    w_u16(a1, (v5 | 0x80));
    w_u16((a1 + (109) * 2u), v4);
    sub_800626C8(a1, 0x800FF5B0u);
    w_u16((a1 + (29) * 2u), 50);
    v6 = r_u8(0x800EC6D8u);
    w_u8((((uint32)(a1)) + (129) * 1u), 1);
    w_u8((((uint32)(a1)) + (131) * 1u), 1);
    w_u8((((uint32)(a1)) + (130) * 1u), v6);
    w_u8((((uint32)(a1)) + (144) * 1u), 5);
    w_u8((((uint32)(a1)) + (145) * 1u), 1);
    w_u8((((uint32)(a1)) + (146) * 1u), 5);
    w_u8((((uint32)(a1)) + (582) * 1u), r_u8(0x800EC4B4u));
    v7 = r_u16(0x800EC4B8u);
    w_u16((a1 + (106) * 2u), 100);
    w_u32((((uint32)(a1)) + (112) * 4u), 1);
    w_u8((((uint32)(a1)) + (314) * 1u), 1);
    w_u16((a1 + (292) * 2u), v7);
    w_u32((((uint32)(a1)) + (79) * 4u), sub_8006B864(32, 0, 1));
    v8 = sub_8006B864(16, 0, 1);
    v9 = ((uint32)(r_u32((((uint32)(a1)) + (79) * 4u))));
    w_u32((((uint32)(a1)) + (80) * 4u), v8);
    w_u16(v9, 0);
    w_u16(((uint32)((r_u32((((uint32)(a1)) + (79) * 4u)) + 2))), -4096);
    w_u16(((uint32)((r_u32((((uint32)(a1)) + (79) * 4u)) + 4))), 0);
    w_u8(r_u32((((uint32)(a1)) + (80) * 4u)), 0x80);
    w_u8(((uint32)((r_u32((((uint32)(a1)) + (80) * 4u)) + 1))), 0x80);
    w_u8(((uint32)((r_u32((((uint32)(a1)) + (80) * 4u)) + 2))), 0x80);
    sub_8005E9E4(a1);
    w_u32(0x800FF840u, 50);
    w_u32(((uint32)((r_u32(0x800FF904u) + 564))), 3);
    v10 = ((uint32)(sub_8002FED8(88)));
    if (v10)
        v10 = sub_80022B8C(v10, ((sint32)(0x800FF4E8u)));
    v11 = 5;
    v12 = (r_u32(0x800FF384u) != 0);
    w_u32((((uint32)(a1)) + (153) * 4u), v10);
    if (!v12)
        v11 = 7;
    w_u32((v10 + (10) * 4u), v11);
    w_u32((((uint32)(a1)) + (161) * 4u), 0);
    w_u32((((uint32)(a1)) + (162) * 4u), 1);
    w_u32((((uint32)(a1)) + (133) * 4u), 3);
    sub_8007CA50(a1, 0x800A6ECCu);
    return a1;
}

uint32 sub_80062F64(uint32 a1)
{
    sint32 result;
    short v3;
    sub_80062924(a1);
    result = a1;
    w_u32(((uint32)((a1 + 68))), 0x800A3340u);
    w_u32(((uint32)((a1 + 304))), 0x10000);
    v3 = r_u16(((uint32)(a1)));
    w_u8(((uint32)((a1 + 302))), 1);
    w_u8(((uint32)((a1 + 303))), 1);
    w_u16(((uint32)(a1)), (v3 | 2));
    return result;
}

uint32 sub_80062A38(uint32 a1, uint32 a2)
{
    sint32 v2;
    sint32 result;
    v2 = ((sint32)(r_u32(a2)));
    w_u32(((uint32)((a1 + 48))), 0);
    w_u32(((uint32)((a1 + 28))), v2);
    w_u32(a2, a1);
    result = r_u32(((uint32)((a1 + 28))));
    if (result)
        w_u32(((uint32)((result + 48))), a1);
    return result;
}

uint32 sub_80063038(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    sint32 v4;
    unsigned char v5;
    sint32 v6;
    sint8 v7;
    sint32 v8;
    sint32 v9;
    sint32 result;
    a2 &= 255u;
    (v4 = r_u8(((uint32)((a1 + 27)))));
    w_u8(((uint32)((a1 + 26))), a2);
    (v5 = r_u8(((uint32)((((8 * a2) + r_u32((0x800EAEF8u + (((16 * v4) + 6)) * 4u))) + 8)))));
    w_u8(((uint32)((a1 + 302))), v5);
    if ((a3 == -1))
        (a3 = (v5 - 1));
    if ((a4 == -1))
        (a4 = (r_u8(((uint32)((a1 + 302)))) - 1));
    if (((((sint32)(a3)) < 0) || (((sint32)(a3)) >= r_u8(((uint32)((a1 + 302)))))))
        (a3 = 0);
    if (((((sint32)(a4)) < 0) || ((v6 = (((sint32)(a3)) < ((sint32)(a4)))), (((sint32)(a4)) >= r_u8(((uint32)((a1 + 302))))))))
    {
        (a4 = 0);
        (v6 = (((sint32)(a3)) < 0));
    }
    w_u8(((uint32)((a1 + 296))), 0);
    if (v6)
    {
        (v7 = 1);
        goto LABEL_15;
    }
    (v7 = -1);
    if ((((sint32)(a4)) < ((sint32)(a3))))
    {
    LABEL_15:
        w_u8(((uint32)((a1 + 297))), v7);

        goto LABEL_16;
    }
    w_u8(((uint32)((a1 + 297))), 0);
LABEL_16:
    w_u8(((uint32)((a1 + 24))), a3);

    (v8 = ((sint8)(r_u8(((uint32)((a1 + 24)))))));
    w_u8(((uint32)((a1 + 298))), a4);
    (v9 = ((sint8)(r_u8(((uint32)((a1 + 298)))))));
    w_u16(((uint32)((a1 + 300))), 0);
    (result = (v8 == v9));
    w_u8(((uint32)((a1 + 303))), result);
    return result;
}

uint32 sub_80022A24(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 result;
    result = a1;
    w_u32(a1, 0x800A1570u);
    w_u32((a1 + (11) * 4u), a2);
    w_u32((a1 + (12) * 4u), a3);
    w_u32((a1 + (14) * 4u), 1000);
    return result;
}

uint32 sub_80077680(uint32 object)
{
    uint32 x = r_u32(object + 4u);
    uint32 offset_x = r_u32(0x800ED510u);
    uint32 y, z, offset_y, offset_z;
    w_u32(0x800ED520u, (uint32)((sint32)(x + offset_x) >> 12));
    y = r_u32(object + 8u);
    offset_y = r_u32(0x800ED514u);
    w_u32(0x800ED524u, (uint32)((sint32)(y + offset_y) >> 12));
    z = r_u32(object + 12u);
    offset_z = r_u32(0x800ED518u);
    w_u32(0x800ED528u, (uint32)((sint32)(z + offset_z) >> 12));
    w_u32(0x800ED530u, (uint32)((sint32)(r_u32(object + 344u) + offset_x) >> 12));
    w_u32(0x800ED534u, (uint32)((sint32)(r_u32(object + 348u) + offset_y) >> 12));
    w_u32(0x800ED538u, (uint32)((sint32)(r_u32(object + 352u) + offset_z) >> 12));
    sub_80076310(object + 428u, 0x800ED550u);
    return sub_800878AC(0x800ED550u, 0x800ED520u);
}

uint32 sub_80076310(uint32 a1, uint32 a2)
{
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
    sint32 result;
    v2 = ((sint32)(r_u32((a1 + (3) * 4u))));
    v3 = ((sint32)(r_u32(a1)));
    v4 = (v2 * ((sint32)(r_u32(a1))));
    v5 = ((sint32)(r_u32((a1 + (1) * 4u))));
    v6 = ((sint32)(r_u32((a1 + (2) * 4u))));
    v7 = (v3 * v3);
    w_u32(((uint32)((a2 + 20))), 0);
    w_u32(((uint32)((a2 + 24))), 0);
    v8 = (v3 * v6);
    w_u32(((uint32)((a2 + 28))), 0);
    v9 = ((v2 * v6) >> 11);
    v10 = ((v5 * v5) >> 11);
    v11 = ((v6 * v6) >> 11);
    v12 = ((v3 * v5) >> 11);
    w_u16(((uint32)(a2)), (4096 - (v10 + v11)));
    w_u16(((uint32)((a2 + 2))), (v12 + v9));
    w_u16(((uint32)((a2 + 6))), (v12 - v9));
    w_u16(((uint32)((a2 + 8))), (4096 - ((v7 >> 11) + v11)));
    w_u16(((uint32)((a2 + 4))), ((v8 >> 11) - ((v2 * v5) >> 11)));
    w_u16(((uint32)((a2 + 12))), ((v8 >> 11) + ((v2 * v5) >> 11)));
    w_u16(((uint32)((a2 + 16))), (4096 - ((v7 >> 11) + v10)));
    result = (((v5 * v6) >> 11) + (v4 >> 11));
    w_u16(((uint32)((a2 + 10))), result);
    w_u16(((uint32)((a2 + 14))), (((v5 * v6) >> 11) - (v4 >> 11)));
    return result;
}

uint32 sub_80063E7C(void)
{
    uint32 position[4], cursor, owner, rotation_word;
    cursor = xport_draft_host_sub_8006613C_p1(position, r_u32(0x800FF620u));
    owner = r_u32(0x800FF5A0u);
    w_u32(owner + 4u, position[0]);
    w_u32(owner + 8u, position[1]);
    w_u32(owner + 12u, position[2]);
    /* Original unaligned load/store transfers four rotation bytes */
    rotation_word = r_u8(cursor) | (r_u8(cursor + 1u) << 8) | (r_u8(cursor + 2u) << 16) | (r_u8(cursor + 3u) << 24);
    w_u8(owner + 16u, rotation_word);
    w_u8(owner + 17u, rotation_word >> 8);
    w_u8(owner + 18u, rotation_word >> 16);
    w_u8(owner + 19u, rotation_word >> 24);
    w_u16(owner + 20u, r_u16(cursor + 4u));
    cursor += 6u;
    while (r_u8(cursor))
        ++cursor;
    ++cursor;
    cursor += cursor & 1u;
    sub_8006541C(cursor, r_u32(0x800FF620u), 1u);
    return sub_8006CDC4();
}

uint32 sub_8007851C(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 result;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    result = (a2 & 0xFFF);
    if (a3)
    {
        v4 = ((sint16)(r_u16(((uint32)((a1 + 494))))));
        v5 = result;
        w_u32(0x800FFD1Cu, result);
        if ((v4 >= result))
        {
            result = (result < v4);
            if ((v5 < v4))
            {
                v8 = ((sint16)(r_u16(((uint32)((a1 + 494))))));
                w_u32(0x800FFD18u, a3);
                if (((v8 - v5) < 2049))
                    v7 = (v5 - v8);
                else
                    v7 = ((v5 + 4096) - v8);
                goto LABEL_4;
            }
            w_u32(0x800FFD14u, 0);
            w_u32(0x800FFD18u, 0);
        }
        else
        {
            w_u32(0x800FFD18u, a3);
            v6 = (result - v4);
            v7 = ((result - v4) - 4096);
            if ((v6 >= 2049))
            {
            LABEL_4:
                result = (v7 / a3);

                w_u32(0x800FFD14u, result);
                return result;
            }
            result = (v6 / a3);
            w_u32(0x800FFD14u, result);
        }
    }
    else
    {
        w_u16(((uint32)((a1 + 494))), result);
        w_u32(0x800FFD18u, 0);
    }
    return result;
}

uint32 sub_8007865C(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 result;
    if (a3)
    {
        result = ((a2 - r_u32(0x800FF918u)) / a3);
        w_u32(0x800FF91Cu, a3);
        w_u32(0x800FFD20u, result);
    }
    else
    {
        result = a2;
        w_u32(0x800FF918u, a2);
        w_u32(0x800FF91Cu, 0);
    }
    return result;
}

uint32 sub_80064A08(uint32 a1)
{
    uint32 v1;
    sint32 result;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    v1 = (a1 + (1) * 2u);
    result = ((unsigned short)(r_u16(a1)));
    if (r_u16(a1))
    {
        v3 = ((unsigned short)(r_u16(a1)));
        v4 = 1;
        do
        {
            v5 = r_u16((v1 += 2u, v1 - 2u));
            sub_80064874(v5);
            result = (v4 < v3);
        } while ((v4++ < v3));
    }
    return result;
}

uint32 sub_800641B8(uint32 a1)
{
    uint32 v1;
    sint32 v2;
    if ((r_u8(a1) != 255))
    {
        v1 = (a1 + (1) * 1u);
        do
            v2 = r_u8((v1 += 1u, v1 - 1u));
        while ((v2 != 255));
        a1 = (v1 - (1) * 1u);
    }
    return (a1 + (1) * 1u);
}

uint32 sub_80062B0C(uint32 a1, uint32 a2)
{
    sint32 v2;
    sint32 v3;
    sint32 v4;
    v2 = (r_u32(a2) << 12);
    w_u32((a1 + (60) * 4u), v2);
    w_u32((a1 + (1) * 4u), v2);
    v3 = (r_u32((a2 + (1) * 4u)) << 12);
    w_u32((a1 + (61) * 4u), v3);
    w_u32((a1 + (2) * 4u), v3);
    v4 = (r_u32((a2 + (2) * 4u)) << 12);
    w_u32((a1 + (62) * 4u), v4);
    w_u32((a1 + (3) * 4u), v4);
    return (a2 + (3) * 4u);
}

uint32 sub_8004E35C(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 v6;
    sub_8004B800(a1);
    w_u32(((uint32)((a1 + 68))), 0x800A2A68u);
    v6 = sub_80062B0C(a1, a2);
    w_u32(((uint32)((a1 + 400))), sub_80062B50(a1, v6));
    w_u8(((uint32)((a1 + 380))), 1);
    w_u16(((uint32)((a1 + 214))), a3);
    sub_8004E48C(a1);
    sub_8004BE90(a1, r_u32(((uint32)((a1 + 400)))));
    return a1;
}

/* TODO Missing call adapter indirect */
uint32 sub_8004BE90(uint32 object, uint32 cursor)
{
    w_u32(object + 400u, cursor);
    for (;;)
    {
        cursor = r_u32(object + 400u);
        uint32 command = r_u16(cursor), result = cursor + 2u;
        if (command == 16640u)
        {
            w_u8(object + 380u, 0);
            return result;
        }
        w_u32(object + 400u, result);
        if (command & 0x4000u)
        {
            uint32 table = r_u32(object + 68u);
            result = xport_draft_guest_call2(r_u32(table + 68u), object + (sint16)r_u16(table + 64u), command);
            if (!result)
                return result;
        }
        else if (command & 0x2000u)
        {
            uint32 table = r_u32(object + 68u);
            xport_draft_guest_call2(r_u32(table + 76u), object + (sint16)r_u16(table + 72u), command);
        }
    }
}

uint32 sub_800587E8(uint32 linked, uint32 object)
{
    uint32 flags;
    if (r_u32(linked + 544u))
        return 0u;
    flags = r_u32(linked + 396u);
    w_u32(linked + 544u, object);
    w_u32(linked + 396u, flags | 0x20u);
    return 1u;
}

uint32 sub_80052EE8(uint32 object, uint32 linked)
{
    w_u32(object + 616u, linked);
    if (r_u16(linked + 58u) == 206u)
    {
        sub_800587E8(linked, object);
        w_u16(object + 472u, 12u);
        w_u16(object + 474u, 0u);
        return 12u;
    }
    return 206u;
}

static uint32 command_529AC_word(uint32 object)
{
    uint32 cursor = r_u32(object + 400u);
    uint32 value = r_u16(cursor);
    w_u32(object + 400u, cursor + 2u);
    return value;
}

static uint32 command_529AC_resolve(uint32 object, uint32 value)
{
    if (value & 0x2000u)
    {
        uint32 table = r_u32(object + 68u);
        uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80u);
        uint32 target = r_u32(table + 84u);
        value = xport_draft_guest_call2(target, receiver, value);
    }
    return (uint16)value;
}

static sint32 command_529AC_ground(const uint32 position[3], uint32 below, uint32 above)
{
    return (sint32)apocalypse_ground_native(position, below, above);
}

static void command_529AC_position(uint32 target, uint32 object, const uint32 position[3])
{
    switch (target)
    {
        case 0x80053784u:
            apocalypse_move_target_native(object, position);
            return;
        case 0x80053834u:
            apocalypse_jump_target_native(object, position);
            return;
        case 0x8005389Cu:
            apocalypse_special_target_native(object, position);
            return;
        default:
            abort();
    }
}

uint32 sub_800529AC(uint32 object, uint32 command)
{
    uint32 position[3], shift = 12u, target;
    command = (uint16)command;
    switch (command)
    {
        case 16902:
            sub_80051740(object, command_529AC_word(object));
            return 1;
        case 16928:
        case 16931:
        case 16930:
        case 16933:
        {
            uint32 cursor = (r_u32(object + 400u) + 3u) & ~3u;
            for (uint32 i = 0; i < 3u; ++i)
                position[i] = r_u32(cursor + i * 4u);
            xport_draft_host_sub_8006C22C_p12(position, &shift);
            if (command == 16930u || command == 16933u)
                xport_draft_host_sub_8006C0B8_p1(position, object + 4u);
            w_u32(object + 400u, cursor + 12u);
            target = command == 16928u || command == 16930u ? 0x80053784u : 0x80053834u;
            command_529AC_position(target, object, position);
            return 1;
        }
        case 16929:
        case 16932:
        case 16936:
        {
            uint32 value = command_529AC_resolve(object, command_529AC_word(object));
            xport_draft_host_sub_8006613C_p1(position, value);
            sint32 height = command_529AC_ground(position, 0, 2048);
            if (height != -1)
                position[1] = (uint32)height - ((uint32)(sint16)r_u16(object + 456u) << 12);
            target = command == 16929u ? 0x80053784u : command == 16932u ? 0x80053834u : 0x8005389Cu;
            command_529AC_position(target, object, position);
            return 1;
        }
        case 16938:
        case 16939:
            sub_80063038(object, command == 16938u ? 7u : 8u, 0, 0xFFFFFFFFu);
            w_u16(object + 212u, command == 16938u ? 32u : 128u);
            {
                uint32 flags = r_u32(object + 396u);
                uint32 state = r_u16(object + 78u);
                w_u32(object + 396u, flags & ~1u);
                w_u16(object + 78u, command == 16938u ? state & ~0x10u : state | 0x10u);
            }
            w_u32(object + 112u, 0);
            w_u32(object + 108u, 0);
            w_u32(object + 104u, 0);
            w_u16(object + 476u, command == 16938u ? 12u : 14u);
            return 0;
        case 16937:
        {
            uint32 value = command_529AC_resolve(object, command_529AC_word(object));
            for (uint32 node = r_u32(0x800FF5DCu); node; node = r_u32(node + 28u))
                if (r_u16(node + 214u) == value)
                {
                    sub_80052EE8(object, node);
                    break;
                }
            return 1;
        }
        case 16994:
            if (command_529AC_word(object) == 1u)
                sub_80052F34(object);
            return 1;
        case 17415:
        {
            uint32 value = command_529AC_word(object);
            if (value)
                sub_80053950(object, command_529AC_resolve(object, value));
            else
                sub_8005398C(object);
            return 1;
        }
        case 18176:
            if (command_529AC_word(object))
                w_u32(object + 396u, r_u32(object + 396u) | 0x100u);
            else
                w_u32(object + 396u, r_u32(object + 396u) & ~0x100u);
            return 1;
        case 18177:
            w_u32(object + 396u, r_u32(object + 396u) | 0x200u);
            return 1;
        default:
            return sub_8004BF3C(object, command);
    }
}

uint32 sub_8006E080(uint32 a1, uint32 a2)
{
    uint32 v2;
    sint32 result;
    uint32 v4;
    uint32 v5;
    uint32 i;
    v2 = (0x800EAEF8u + ((16 * a2)) * 4u);
    result = ((sint32)(r_u32((v2 + (4) * 4u))));
    v4 = r_u32(((uint32)((result - 4))));
    v5 = ((uint32)(((sint32)(r_u32((v2 + (3) * 4u))))));
    for (i = 0; (i < v4); (v5 += 4u))
    {
        result = i;
        if ((r_u32(v5) == a1))
            break;
        result = (++i < v4);
    }

    return result;
}

uint32 apocalypse_ground_native(const void *position, uint32 below, uint32 above)
{
    uint32 collision[36], value;
    memcpy(&value, position, sizeof(value));
    collision[0] = value;
    memcpy(&value, (const uint8 *)position + 4u, sizeof(value));
    collision[1] = value - (below << 12);
    memcpy(&value, (const uint8 *)position + 8u, sizeof(value));
    collision[2] = value;
    memcpy(&value, position, sizeof(value));
    collision[3] = value;
    memcpy(&value, (const uint8 *)position + 4u, sizeof(value));
    collision[4] = value + (above << 12);
    memcpy(&value, (const uint8 *)position + 8u, sizeof(value));
    collision[5] = value;
    xport_draft_host_sub_8007BB24_p1(collision);
    ((uint8 *)collision)[136] = 0u;
    xport_draft_host_sub_8007DD04_p1(collision, 1u);
    if (collision[26] && (sint16)(collision[30] >> 16) < -3071)
        return collision[28];
    return 0xFFFFFFFFu;
}

uint32 sub_80067A18(uint32 position, uint32 below, uint32 above)
{
    return apocalypse_ground_native(psx_addr(position, 12u), below, above);
}

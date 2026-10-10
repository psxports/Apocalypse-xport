#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>

uint32 sub_800325B0(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11)
{
    sint32 result;
    sint32 v19;
    sint32 v20;
    sub_800330F4(a1);
    result = a1;
    w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))), 0x800A1B20u);
    v19 = r_u32((a2 + (1) * 4u));
    v20 = r_u32((a2 + (2) * 4u));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(24)))), r_u32(a2));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(28)))), v19);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(32)))), v20);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(10)))), a3);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(72)))), a4);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(76)))), a5);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(77)))), a6);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(78)))), a7);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(80)))), a8);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(84)))), a9);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(88)))), a10);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(92)))), a11);
    return result;
}

/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_80085674(uint32 output, uint32 matrix, uint32 local)
{
    uint32 words[5], translation[3], input[5], result[3], axis, column;
    const uint32 opcodes[3] = {0x486012u, 0x48E012u, 0x496012u};
    FUNCTION_MARKER(0x80085674u, "SLUS_003.73");
    for (axis = 0u; axis < 5u; ++axis)
        words[axis] = r_u32(matrix + axis * 4u);
    for (axis = 0u; axis < 5u; ++axis)
        xport_draft_gte_control_write(axis, words[axis]);
    for (axis = 0u; axis < 3u; ++axis)
        translation[axis] = (uint32)(sint32)(sint16)r_u16(matrix + 18u + axis * 2u);
    for (axis = 0u; axis < 3u; ++axis)
        xport_draft_gte_control_write(5u + axis, translation[axis]);
    for (axis = 0u; axis < 5u; ++axis)
        input[axis] = r_u32(local + axis * 4u);
    xport_draft_gte_data_write(0u, (input[0] & 0xFFFFu) | (input[1] & 0xFFFF0000u));
    xport_draft_gte_data_write(1u, input[3]);
    xport_draft_gte_data_write(2u, (input[0] >> 16) | (input[2] << 16));
    xport_draft_gte_data_write(3u, input[3] >> 16);
    xport_draft_gte_data_write(4u, (input[1] & 0xFFFFu) | (input[2] & 0xFFFF0000u));
    xport_draft_gte_data_write(5u, input[4]);
    for (column = 0u; column < 3u; ++column)
    {
        xport_draft_gte_execute(opcodes[column]);
        for (axis = 0u; axis < 3u; ++axis)
            result[axis] = xport_draft_gte_data_read(9u + axis);
        for (axis = 0u; axis < 3u; ++axis)
            w_u16(output + column * 2u + axis * 6u, result[axis]);
    }
    for (axis = 0u; axis < 3u; ++axis)
        translation[axis] = (uint32)(sint32)(sint16)r_u16(local + 18u + axis * 2u);
    for (axis = 0u; axis < 3u; ++axis)
        xport_draft_gte_data_write(9u + axis, translation[axis]);
    xport_draft_gte_execute(0x498012u);
    for (axis = 0u; axis < 3u; ++axis)
        result[axis] = xport_draft_gte_data_read(9u + axis);
    for (axis = 0u; axis < 3u; ++axis)
        w_u16(output + 18u + axis * 2u, result[axis]);
}

uint32 sub_800665CC(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 v3;
    sint32 result;
    v3 = (0x800F863Cu + ((a3 & 0xFFF)) * 4u);
    w_u32(a1, (((uint32)(((uint32)((((sint32)(((sint32)(r_u32(a2))))) >> 3)) * (uint32)(((sint16)(r_u16((((uint32)(v3)) + (1) * 2u))))))) + (uint32)(((uint32)((((sint32)(((sint32)(r_u32((a2 + (2) * 4u)))))) >> 3)) * (uint32)(((sint16)(r_u16(((uint32)(v3))))))))) >> 9));
    w_u32((a1 + (1) * 4u), ((sint32)(r_u32((a2 + (1) * 4u)))));
    result = (((uint32)(((uint32)((((sint32)(((sint32)(r_u32((a2 + (2) * 4u)))))) >> 3)) * (uint32)(((sint16)(r_u16((((uint32)(v3)) + (1) * 2u))))))) - (uint32)(((uint32)((((sint32)(((sint32)(r_u32(a2))))) >> 3)) * (uint32)(((sint16)(r_u16(((uint32)(v3))))))))) >> 9);
    w_u32((a1 + (2) * 4u), result);
    return result;
}

/* TODO Missing call adapter indirect */
uint32 sub_800210D8(uint32 object, uint32 flags)
{
    uint32 child = r_u32(object + 200u);
    w_u32(object + 68u, 0x800A12C8u);
    if (child)
    {
        uint32 table = r_u32(child + 68u);
        (void)apocalypse_object_cleanup(r_u32(table + 12u),
            child + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
    }
    sub_800342B0(object, 0u);
    if (flags & 1u) return sub_80032E30(object);
    return flags & 1u;
}

uint32 sub_80036C5C(uint32 a1, uint32 a2)
{
    sint32 result;
    sint32 v5;
    w_u32((a1 + (17) * 4u), 0x800A1BB0u);
    result = sub_80036828(a1, 0);
    if (((a2 & 1) != 0))
        return sub_80032E30(((sint32)(a1)));
    return result;
}

/* TODO Missing call adapter indirect */
uint32 sub_80036828(uint32 object, uint32 flags)
{
    sint32 index = 0;
    sint32 count = (sint32)r_u32(object + 72u);
    w_u32(object + 68u, 0x800A1BC8u);
    if (count > 0)
    {
        do
        {
            uint32 child = r_u32(r_u32(object + 92u) + (uint32)index * 4u);
            if (child)
            {
                uint32 table = r_u32(child + 68u);
                (void)apocalypse_object_cleanup(r_u32(table + 12u),
                    child + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
            }
            index = (sint32)((uint32)index + 1u);
        } while (index < (sint32)r_u32(object + 72u));
    }
    sub_8006BC20(r_u32(object + 92u));
    sub_8006BC20(r_u32(object + 88u));
    sub_80033138(object, 0u);
    if (flags & 1u) return sub_80032E30(object);
    return flags & 1u;
}

/* TODO Missing call adapter indirect */
/* TODO Missing host buffer adapter xport_draft_host_sub_8005BBB0_p3 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006696C_p2 */
void sub_8004FB00(uint32 object, uint32 mode)
{
    uint32 angle = sub_80066570(1024u);
    uint32 index;
    for (index = 0; index < 6u; ++index)
    {
        uint32 velocity[3];
        uint32 child;
        if (r_u32(object + 220u) >= 2048u || !r_u32(0x800FF904u) || sub_80066570(4u))
        {
            uint32 random = sub_80066570(16u);
            uint32 table = 0x800F863Cu + (angle & 4095u) * 4u;
            velocity[0] = (uint32)(sint32)(sint16)r_u16(table) * (random + 24u);
            velocity[1] = (0u - 32u - sub_80066570(16u)) << 12;
            random = sub_80066570(16u);
            velocity[2] = (uint32)(sint32)(sint16)r_u16(table + 2u) * (random + 24u);
        }
        else
        {
            uint32 target = r_u32(0x800FF904u);
            uint32 position[3];
            uint32 divisor;
            sint32 numerator;
            position[0] = r_u32(target + 4u);
            position[1] = r_u32(target + 8u);
            position[2] = r_u32(target + 12u);
            position[1] += 0x80000u;
            divisor = xport_draft_host_sub_8006696C_p2(object + 4u, position) >> 7;
            /* MIPS DIV has no BREAK guard and defines LO even for zero */
            numerator = (sint32)(position[0] - r_u32(object + 4u));
            velocity[0] = divisor ? (uint32)(numerator / (sint32)divisor) : numerator < 0 ? 1u : 0xFFFFFFFFu;
            numerator = (sint32)(position[2] - r_u32(object + 12u));
            velocity[2] = divisor ? (uint32)(numerator / (sint32)divisor) : numerator < 0 ? 1u : 0xFFFFFFFFu;
            numerator = (sint32)(position[1] - r_u32(object + 8u));
            velocity[1] = (divisor ? (uint32)(numerator / (sint32)divisor) : numerator < 0 ? 1u : 0xFFFFFFFFu) - (divisor << 13);
        }
        child = sub_800625AC(320u);
        if (child)
        {
            uint32 variation = sub_80066570(r_u8(object + 615u));
            uint32 kind = (r_u8(object + 614u) + variation) & 65535u;
            uint32 height = r_u32(object + 8u) + ((uint32)(sint32)(sint16)r_u16(object + 456u) << 12);
            uint32 result = xport_draft_host_sub_8005BBB0_p3(child, object + 4u,
                velocity, kind, height, mode == 5u ? 5u : (r_u16(object + 390u) & 0x180u) ? 9u : 3u,
                24u, r_u32(0x800FF62Cu));
            if (mode == 5u && result) sub_800626F8(object, 0u, 0u, 0u, 0u);
        }
        angle += 682u;
    }
    {
        uint32 table = r_u32(object + 68u);
        uint32 target = r_u32(table + 20u);
        uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16u);
        apocalypse_object_virtual20(target, receiver);
        return;
    }
}

uint32 sub_800650EC(uint32 a1)
{
    sint32 v1;
    uint32 v2;
    uint32 result;
    v1 = r_u16(a1);
    v2 = ((uint32)((a1 + (1) * 2u)));
    if ((((uint32)(v1)) >= 0x8A))
    {
        if ((v1 == 160))
            goto LABEL_70;
        if ((((sint32)(v1)) >= 161))
        {
            if ((v1 == 172))
                goto LABEL_70;
            if ((((sint32)(v1)) >= 173))
            {
                if ((v1 == 175))
                    return ((uint32)(v2));
                if ((((sint32)(v1)) >= 176))
                {
                    if ((v1 == 177))
                        goto LABEL_70;
                    if ((((sint32)(v1)) >= 177))
                    {
                        result = ((uint32)(v2));
                        if ((v1 == 0xFFFF))
                            return 0;
                        return result;
                    }
                    return sub_800650B4(v2);
                }
                if ((v1 == 173))
                    return ((uint32)(v2));
            }
            else if ((v1 != 167))
            {
                if ((((sint32)(v1)) < 168))
                {
                    result = ((uint32)(v2));
                    if ((((sint32)(v1)) < 163))
                        return result;
                    v2 += (2) * 1u;
                    return ((uint32)(v2));
                }
                if ((((sint32)(v1)) >= 171))
                    return ((uint32)((((uint32)((v2 + (3) * 1u))) & 0xFFFFFFFC)) + (uint32)(10));
                goto LABEL_70;
            }
        }
        else
        {
            if ((((sint32)(v1)) >= 147))
            {
                if ((v1 == 149))
                    return ((uint32)(v2));
                if ((((sint32)(v1)) >= 149))
                {
                    if ((((sint32)(v1)) < 157))
                    {
                        result = ((uint32)(v2));
                        if ((((sint32)(v1)) < 153))
                            return result;
                        v2 += (2) * 1u;
                    }
                    return ((uint32)(v2));
                }
                goto LABEL_70;
            }
            if ((((sint32)(v1)) < 143))
            {
                if ((v1 == 140))
                    return sub_800650B4(v2);
                if ((((sint32)(v1)) >= 141))
                {
                    if ((v1 != 141))
                        return sub_800650B4(v2);
                    v2 += (2) * 1u;
                    goto LABEL_68;
                }
                if ((v1 == 138))
                    goto LABEL_70;
                result = ((uint32)(v2));
                if ((v1 != 139))
                    return result;
            }
        }
    LABEL_61:
        v2 += (4) * 1u;

        return ((uint32)(v2));
    }
    if ((((sint32)(v1)) >= 136))
        return ((uint32)(v2));
    if ((v1 == 115))
        return sub_800650B4(v2);
    if ((((sint32)(v1)) >= 116))
    {
        if ((v1 == 129))
            return ((uint32)(v2));
        if ((((sint32)(v1)) < 130))
        {
            if ((((sint32)(v1)) >= 123))
            {
                result = ((uint32)(v2));
                if ((((sint32)(v1)) < 126))
                    return result;
            }
            else
            {
                if ((((sint32)(v1)) >= 121))
                    return ((uint32)(v2));
                result = ((uint32)(v2));
                if ((v1 != 119))
                    return result;
            }
            return sub_800650B4(v2);
        }
        if ((((sint32)(v1)) < 133))
        {
            if ((((sint32)(v1)) < 131))
            {
                v2 += (4) * 1u;
                return ((uint32)(v2));
            }
        LABEL_70:
            v2 += (2) * 1u;

            return ((uint32)(v2));
        }
        if ((v1 == 134))
            goto LABEL_70;
        if ((((sint32)(v1)) < 135))
        {
        LABEL_68:
            while ((r_u16(((uint32)(v2))) != 255))
                v2 = ((uint32)(((uint32)((((uint32)((v2 + (3) * 1u))) & 0xFFFFFFFC)) + (uint32)(24))));

            goto LABEL_70;
        }
        goto LABEL_61;
    }
    if ((v1 == 13))
        goto LABEL_70;
    if ((((sint32)(v1)) < 14))
    {
        if (((((sint32)(v1)) < 6) && (((sint32)(v1)) < 3)))
        {
            result = ((uint32)(v2));
            if ((v1 != 2))
                return result;
            while (r_u8(v2))
                v2 = ((uint32)(sub_800650B4(v2)));

            v2 += (2) * 1u;
        }
        return ((uint32)(v2));
    }
    if ((v1 == 104))
    {
        v2 += (6) * 1u;
        return ((uint32)(v2));
    }
    result = ((uint32)(v2));
    if ((((sint32)(v1)) >= 105))
    {
        result = ((uint32)(v2));
        if ((((sint32)(v1)) < 107))
        {
            v2 += (2) * 1u;
            return ((uint32)(v2));
        }
    }
    return result;
}

/* TODO Missing call adapter indirect */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8004E508(uint32 object, uint32 flags)
{
    uint32 child, kind;
    w_u32(object + 68u, 0x800A2A68u);
    sub_80062A64(object, 0x800FF4E8u);
    child = r_u32(object + 608u);
    if (child)
    {
        uint32 table = r_u32(child + 68u);
        (void)apocalypse_object_cleanup(r_u32(table + 12u),
            child + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
    }
    child = r_u32(object + 620u);
    if (child)
    {
        uint32 table = r_u32(child);
        uint32 target = r_u32(table + 12u);
        uint32 receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8u);
        /* TODO Bind the separate list-node destructor contract */
        fprintf(stderr, "Missing list-node cleanup 8004E508 target %08X receiver %08X reason 3\n", target, receiver);
        abort();
    }
    sub_8004BD94(object, 3u);
    kind = r_u16(object + 390u);
    w_u32(0x800FF4DCu, r_u32(0x800FF4DCu) - 1u);
    if (kind == 128u)
        w_u32(0x800FF4E4u, r_u32(0x800FF4E4u) - 1u);
    if (r_u16(object + 390u) == 256u)
    {
        child = r_u32(object + 480u);
        if (child)
        {
            sub_8006A294(child);
            w_u32(object + 480u, 0u);
        }
    }
    sub_8004B868(object, 0u);
    if (flags & 1u) return sub_80062608(object);
    return flags & 1u;
}

uint32 sub_800336D8(uint32 a1, uint32 a2)
{
    uint32 result;
    sint32 v5;
    w_u32((a1 + (17) * 4u), 0x800A1D50u);
    sub_80032E7C(a1, 0x800FF444u);
    result = sub_80032FB8(((sint32)(a1)), 0);
    if (((a2 & 1) != 0))
        return ((uint32)(sub_80032E30(((sint32)(a1)))));
    return result;
}

/* TODO Missing call adapter indirect */
/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8003C490(uint32 object, uint32 damage)
{
    uint32 count = r_u8(object + 383u);
    uint32 enabled = r_u16(object + 438u), remaining, table;
    w_u8(object + 383u, count + 1u);
    if (enabled && (sint16)r_u16(object + 218u) > 0)
    {
        remaining = r_u16(object + 218u) - damage;
        w_u16(object + 218u, remaining);
        if ((sint32)(remaining << 16) > 0)
            sub_800626F8(object, 3u, 255u, 255u, 255u);
        else
        {
            table = r_u32(object + 68u);
            apocalypse_object_virtual20(r_u32(table + 20u), object + (uint32)(sint32)(sint16)r_u16(table + 16u));
            sub_80069EF4(sub_80066570(2u) + 1u, object + 4u, 0u);
            sub_80022318(object, 0u, 1u);
            w_u16(object, r_u16(object) | 1u);
        }
    }
    return 1u;
}

/* TODO Missing call adapter indirect */
/* TODO Missing host buffer adapter xport_draft_host_sub_8001C158_p23 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8005CEE0_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80066B8C_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8007CC10_p1 */
uint32 sub_80053358(uint32 a1)
{
    sint32 result;
    sint32 v3;
    sint32 v4;
    unsigned short v5;
    sint32 v6;
    unsigned short v7;
    sint32 v8;
    sint32 v9;
    unsigned short v10;
    sint32 v11;
    unsigned short v12;
    sint32 v13;
    sint32 v14;
    unsigned short v15;
    sint32 v16;
    unsigned short v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    unsigned short v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v26[3];
    char v29[8];
    sint32 v30[3];
    if (((r_u32(((uint32)(((uint32)(a1) + (uint32)(396))))) & 0x40) != 0))
    {
        w_u16(((uint32)(((uint32)(a1) + (uint32)(18)))), ((uint32)(3072) - (uint32)(ratan2(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(504)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(12))))))), ((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(496)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(4)))))))))));
        sub_80063038(a1, 1, 0, -1);
        xport_draft_host_sub_8007CC10_p1(v26, a1, 0);
        xport_draft_host_sub_80066B8C_p12(v29, v26, a1 + 496u);
    }
    else
    {
        result = r_u32(0x800FF5A0u);
        if (!r_u32(0x800FF5A0u))
            return result;
        w_u16(((uint32)(((uint32)(a1) + (uint32)(18)))), ((uint32)(3072) - (uint32)(ratan2(((uint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(12)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(12))))))), ((uint32)(r_u32(((uint32)(((uint32)(r_u32(0x800FF5A0u)) + (uint32)(4)))))) - (uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(4)))))))))));
        sub_80063038(a1, 1, 0, -1);
        xport_draft_host_sub_8007CC10_p1(v26, a1, 0);
        xport_draft_host_sub_8005CEE0_p2(r_u32(0x800FF5A0u), v30, 9);
        v30[1] = (sint32)((uint32)v30[1] + 0x10000u);
        xport_draft_host_sub_80066B8C_p123(v29, v26, v30);
    }

    v3 = r_u16(((uint32)(((uint32)(a1) + (uint32)(390)))));
    if ((v3 == 4))
    {
        v4 = sub_80032DC0(108);
        if (v4)
        {
            v5 = sub_8004BDFC(a1, 1u, 1u);
            v6 = sub_8004B914(a1, v5);
            v8 = ((unsigned short)(sub_8004BDFC(a1, 1u, 4u)));
            v7 = sub_8004BDFC(a1, 1u, 2u);
            xport_draft_host_sub_8001C158_p23(v4, v26, v29, v6, v8, v7, 16, 240, 240, 240, 32, 0x80u, 0x40u);
        }
    }
    else if ((v3 == 256))
    {
        v9 = sub_80032DC0(108);
        if (v9)
        {
            v10 = sub_8004BDFC(a1, 1u, 1u);
            v11 = sub_8004B914(a1, v10);
            v13 = ((unsigned short)(sub_8004BDFC(a1, 1u, 4u)));
            v12 = sub_8004BDFC(a1, 1u, 2u);
            xport_draft_host_sub_8001C158_p23(v9, v26, v29, v11, v13, v12, 16, 240, 240, 240, 128, 0x20u, 0x20u);
        }
        xport_draft_host_sub_8007CC10_p1(v26, a1, 1);
        v14 = sub_80032DC0(108);
        if (v14)
        {
            v15 = sub_8004BDFC(a1, 1u, 1u);
            v16 = sub_8004B914(a1, v15);
            v18 = ((unsigned short)(sub_8004BDFC(a1, 1u, 4u)));
            v17 = sub_8004BDFC(a1, 1u, 2u);
            xport_draft_host_sub_8001C158_p23(v14, v26, v29, v16, v18, v17, 16, 240, 240, 240, 128, 0x20u, 0x20u);
        }
    }
    else
    {
        v19 = r_u32(((uint32)(((uint32)(a1) + (uint32)(620)))));
        if (v19)
        {
            uint32 table = r_u32((uint32)v19);
            uint32 receiver = (uint32)v19 + (uint32)(sint32)(sint16)r_u16(table + 16u);
            /* TODO Bind the list-node attack callback with native position and angles */
            fprintf(stderr, "Missing list-node attack 80053358 target %08X receiver %08X position %p angles %p\n",
                r_u32(table + 20u), receiver, (void *)v26, (void *)v29);
            abort();
        }
        else
        {
            v20 = sub_80032DC0(108);
            if (v20)
            {
                v21 = sub_8004BDFC(a1, 1u, 1u);
                v22 = sub_8004B914(a1, v21);
                v23 = ((unsigned short)(sub_8004BDFC(a1, 1u, 4u)));
                v24 = ((unsigned short)(sub_8004BDFC(a1, 1u, 2u)));
                xport_draft_host_sub_8001C158_p23(v20, v26, v29, v22, v23, v24, 16, 240, 240, 240, 64, 0x20u, 0x80u);
            }
        }
    }
    return sub_80069EF4(20, ((uint32)(a1) + (uint32)(4)), 2);
}

/* TODO Missing call adapter SLOWORD */
/* TODO Missing call adapter indirect */
/* TODO Missing host buffer adapter xport_draft_host_sub_800666DC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8007C398_p13 */
uint32 sub_8001C598(uint32 object)
{
    uint32 position = object + 24u, velocity = object + 36u;
    uint32 old_position[3], current_position[3], hit_record[4], hit_vector[4];
    uint32 child, base, phase, hit, trigger, result, table, receiver;
    sint32 amplitude, oscillation, scale, threshold;
    sint16 tick;
    old_position[0] = r_u32(position);
    old_position[1] = r_u32(position + 4u);
    old_position[2] = r_u32(position + 8u);
    sub_8006C0B8(position, velocity);
    sub_80032EE4(r_u32(object + 76u), position);
    child = r_u32(object + 76u);
    w_u16(child + 96u, r_u16(child + 96u) + 140u);
    base = r_u32(object + 84u);
    phase = r_u32(object + 92u) + r_u32(object + 88u);
    w_u32(object + 92u, phase);
    amplitude = (sint32)(6u * base) / 16;
    oscillation = (sint32)((uint32)amplitude * (uint32)(sint32)(sint16)r_u16(0x800F863Cu + (phase & 0xFFFu) * 4u)) / 4096;
    scale = (sint32)(base + (uint32)oscillation);
    sub_80034918(r_u32(object + 76u), scale);
    sub_80034958(r_u32(object + 76u), scale);
    sub_800349D0(r_u32(object + 76u), 0u, (sint32)(42u * (uint32)scale) / 16);
    current_position[0] = r_u32(position);
    current_position[1] = r_u32(position + 4u);
    current_position[2] = r_u32(position + 8u);
    hit = xport_draft_host_sub_8007C398_p123(old_position, current_position, hit_record, r_u32(0x800FF5A0u), 0u);
    if (hit)
    {
        xport_draft_host_sub_800666DC_p1(hit_vector, velocity);
        table = r_u32(hit + 68u);
        receiver = hit + (uint32)(sint32)(sint16)r_u16(table + 48u);
        /* TODO Native vector virtual52 adapter */
        fprintf(stderr, "Missing projectile virtual52 target=%08X receiver=%08X owner=%08X vector=%p reason=29\n", r_u32(table + 52u), receiver, r_u32(object + 72u), (void *)hit_vector);
        abort();
        child = sub_80032DC0(124u);
        if (child)
            sub_8001CF9C(child, hit + 4u, 4, 255, 100, 0, 2, 255, 100, 0, 20, 80, 0, 0, 30, 30, 15, 15, 0, 0);
        return sub_80032ED8(object);
    }
    threshold = (sint32)r_u32(object + 80u);
    tick = (sint16)(r_u16(object + 8u) + 1u);
    w_u16(object + 8u, tick);
    if (threshold < tick)
    {
        sub_80034B7C(r_u32(object + 76u), 10u);
        sub_80034B9C(r_u32(object + 76u), 10u);
    }
    if ((sint16)r_u16(object + 8u) >= (sint32)r_u16(object + 10u))
    {
        child = sub_80032DC0(124u);
        if (child)
            sub_8001CF9C(child, position, 3, r_u8(object + 104u), r_u8(object + 105u), r_u8(object + 106u), 2, r_u8(object + 104u), r_u8(object + 105u), r_u8(object + 106u), 20, 40, 0, 0, 50, 50, 25, 25, 0, 0);
        sub_80032ED8(object);
        sub_80069EF4(24u, position, 0u);
        trigger = r_u32(object + 96u);
        if (trigger && !(trigger & 3u))
            sub_8001E740(trigger, r_u32(object + 100u), r_u32(object + 72u));
    }
    result = r_u32(r_u32(object + 76u) + 88u) & 0xFFFFFFu;
    if (!result)
        return sub_80032ED8(object);
    return result;
}

uint32 sub_80064E10(uint32 a1, uint32 a2)
{
    sint32 result;
    for (; a1; a1 = r_u32(((uint32)(((uint32)(a1) + (uint32)(28))))))
    {
        result = r_u16(((uint32)(((uint32)(a1) + (uint32)(214)))));
        if ((result == a2))
        {
            result = (r_u16(((uint32)(((uint32)(a1) + (uint32)(76))))) | 1);
            w_u16(((uint32)(((uint32)(a1) + (uint32)(76)))), result);
        }
    }

    return result;
}

/* TODO Missing call adapter sub_8001F718 */
/* TODO Missing call adapter sub_8006E0D0 */
static uint32 apocalypse_missing_trigger_service(uint32 target, uint32 argument)
{
    /* TODO Excluded trigger lookup or activation needs a native project adapter */
    fprintf(stderr, "Missing trigger service %08X argument=%08X\n", target, argument);
    abort();
}

uint32 sub_80064C1C(uint32 resource, uint32 mode)
{
    uint32 list = sub_80066088(resource);
    uint32 count = r_u16(list), index, identifier, descriptor, pointer, object;
    sint32 type;
    FUNCTION_MARKER(0x80064C1Cu, "SLUS_003.73");
    list += 2u;
    for (index = 0u; index < count; ++index)
    {
        identifier = r_u16(list);
        descriptor = r_u32(r_u32(0x800FF624u) + identifier * 4u);
        type = (sint16)r_u16(descriptor);
        list += 2u;
        if (type == 1)
        {
            sub_80064A68(identifier, r_u32(0x800FF4E8u), mode);
            sub_80064A68(identifier, r_u32(0x800FF5DCu), mode);
        }
        else if (type == 2 || type == 9)
        {
            pointer = descriptor + 2u;
            pointer += ((uint32)(sint32)(sint16)r_u16(pointer) << 1) + 2u;
            if (pointer & 2u)
                pointer += 2u;
            object = apocalypse_missing_trigger_service(0x8006E0D0u, r_u32(pointer));
            if (object)
            {
                if (mode == 1u)
                    sub_8001E740(object, 0u, 0xFFFFu);
                else
                    w_u16(object, r_u16(object) | 1u);
            }
        }
        else if (type == 10)
            (void)apocalypse_missing_trigger_service(0x8001F718u, identifier);
        else if (type == 500 || type == 501)
        {
            object = r_u32(0x800FF434u);
            while (object && (r_u8(object + 67u) != 2u || r_u16(object + 10u) != identifier))
                object = r_u32(object + 4u);
            if (object)
                sub_80032ED8(object);
        }
    }
    return 0u;
}

/* TODO Missing call adapter indirect */
void sub_80064A68(uint32 identifier, uint32 object, uint32 mode)
{
    while (object)
    {
        if (r_u16(object + 214u) == identifier)
        {
            if (mode == 0u)
            {
                uint32 table = r_u32(object + 68u);
                apocalypse_object_virtual20(r_u32(table + 20u), object + (uint32)(sint32)(sint16)r_u16(table + 16u));
            }
            else if (mode == 1u)
            {
                uint32 table = r_u32(object + 68u);
                uint32 argument = (uint32)(sint32)(sint16)r_u16(object + 218u);
                apocalypse_object_virtual52(r_u32(table + 52u), object + (uint32)(sint32)(sint16)r_u16(table + 48u), argument, 0x800A71CCu, 0u);
            }
        }
        object = r_u32(object + 28u);
    }
}


uint32 sub_80061DC8(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 v6;
    uint32 v7;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    uint32 v11;
    sint32 v12;
    short v13;
    sint32 result;
    sub_80062924(a1);
    v6 = r_u32(0x800FF62Cu);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))), 0x800A3224u);
    sub_800626C8(a1, v6);
    sub_80062A38(a1, 0x800FF4E8u);
    v7 = ((uint32)(sub_80062B0C(a1, a2)));
    v8 = r_u32(((uint32)(((uint32)(a1) + (uint32)(8)))));
    v9 = r_u32(((uint32)(((uint32)(a1) + (uint32)(12)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(296)))), r_u32(((uint32)(((uint32)(a1) + (uint32)(4))))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(300)))), v8);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(304)))), v9);
    v10 = r_u8(((uint32)(((uint32)(a1) + (uint32)(27)))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(16)))), r_u16(v7));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(18)))), r_u16((v7 + (1) * 2u)));
    v11 = ((uint32)((((uint32)(((uint32)(v7))) + (uint32)(9)) & 0xFFFFFFFC)));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(20)))), r_u16((v7 + (2) * 2u)));
    v12 = ((sint32)(r_u32(v11)));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(332)))), ((sint32)(r_u32(v11))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(22)))), sub_8006E080(v12, v10));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(328)))), r_u32(((uint32)((((uint32)(((uint32)(v7))) + (uint32)(13)) & 0xFFFFFFFC)))));
    sub_800667CC(((uint32)(a1) + (uint32)(308)), -128, ((uint32)(a1) + (uint32)(16)));
    sub_8006C0B8(((uint32)(a1) + (uint32)(308)), ((uint32)(a1) + (uint32)(4)));
    v13 = r_u16(((uint32)(((uint32)(a1) + (uint32)(78)))));
    result = a1;
    w_u16(((uint32)(((uint32)(a1) + (uint32)(212)))), 0);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(320)))), 0);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(214)))), a3);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(78)))), (v13 & 0xFFEF));
    return result;
}

/* TODO Missing host buffer adapter xport_draft_host_sub_8006C0B8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C564_p13 */
uint32 sub_8005DDF0(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    sint32 result;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    int v13[2];
    unsigned char v14;
    sint32 v15;
    sint32 v16;
    if (r_u32(0x800FF31Cu))
        return 0;
    result = 0;
    if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(218))))))))) <= 0))
        return result;
    if (sub_80062F48(a1))
        return 0;
    result = 0;
    if (!(r_u32(((uint32)(((uint32)(a1) + (uint32)(452)))))))
    {
        result = 0;
        if (!(r_u32(((uint32)(((uint32)(a1) + (uint32)(456)))))))
        {
            v16 = 1;
            xport_draft_host_sub_8006C564_p13(v13, a3, &v16);
            xport_draft_host_sub_8006C0B8_p2(((uint32)(a1) + (uint32)(104)), v13);
            if ((((sint32)(a2)) >= 2))
            {
                if ((((sint32)(((sint16)(r_u16(((uint32)(((uint32)(a1) + (uint32)(218))))))))) < (r_u16(0x800EC526u) / 5)))
                    a2 >>= 1;
                if (!r_u32(0x800FF384u))
                    a2 >>= 1;
            }
            v9 = ((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(218)))))) - (uint32)(a2));
            w_u16(((uint32)(((uint32)(a1) + (uint32)(218)))), v9);
            if ((((uint32)(v9) << (uint32)(16)) > 0))
            {
                w_u8(((uint32)(((uint32)(a1) + (uint32)(465)))), 6);
                if (((a4 == 5) || (a4 == 31)))
                {
                    w_u8(((uint32)(((uint32)(a1) + (uint32)(466)))), 90);
                    /* MIPS 8005DF74 stores zero as the fifth argument */
                    sub_800626F8(a1, 90, 255, 0, 0);
                    w_u16(((uint32)(((uint32)(a1) + (uint32)(180)))), 10240);
                    v10 = r_u8(((uint32)(((uint32)(a1) + (uint32)(466)))));
                    w_u16(((uint32)(((uint32)(a1) + (uint32)(182)))), 10);
                    w_u32(((uint32)(((uint32)(a1) + (uint32)(176)))), (((((uint32)(2) * (uint32)(v10)) | ((uint32)(v10) << (uint32)(9))) | 0x2000000) | ((uint32)(v10) << (uint32)(17))));
                }
                if (r_u32(0x800FF2FCu))
                {
                    sub_8007011C(0, 4, 0, 1);
                    sub_8007011C(1, 4, 1, 255);
                }
                sub_8001BE78(0x80u, 0, 0, 8u, 0, 0);
                v11 = sub_80066570(4);
                sub_80069DF0(((uint32)(v11) + (uint32)(4)), 0x2000, 0);
                result = 1;
                if (!(r_u8(((uint32)(((uint32)(a1) + (uint32)(26)))))))
                {
                    v12 = sub_80066570(4);
                    sub_80063038(a1, 6, r_u8((((uint32)(0x800FF5CCu)) + (((uint32)(2) * (uint32)(v12))) * 1u)), r_u8(((((uint32)(0x800FF5CCu)) + (((uint32)(2) * (uint32)(v12))) * 1u) + (1) * 1u)));
                    return 1;
                }
            }
            else
            {
                w_u16(((uint32)(((uint32)(a1) + (uint32)(218)))), 0);
                sub_8005E708(a1);
                sub_80070288(0, 0);
                sub_80070288(0, 1);
                return 1;
            }
        }
    }
    return result;
}

/* TODO Missing host buffer adapter xport_draft_host_sub_8006BF04_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C0B8_p2 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C3AC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C47C_p13 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C4EC_p123 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C564_p123 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C5C4_p123 */
uint32 sub_8005E07C(uint32 a1, uint32 a2)
{
    char v5[4];
    sint32 v6;
    char v7[16];
    char v8[16];
    char v9[16];
    char v10[16];
    sint32 v11;
    sint32 v12;
    sint32 v13;
    xport_draft_host_sub_8006C3AC_p1(v5, ((uint32)(a1) + (uint32)(4)), ((uint32)(a2) + (uint32)(4)));
    v6 = 0;
    v12 = xport_draft_host_sub_8006BF04_p1(v5);
    if (!v12)
        v12 = 1;
    v11 = 12;
    xport_draft_host_sub_8006C564_p123(v10, v5, &v11);
    xport_draft_host_sub_8006C47C_p13(v9, ((uint32)(a2) + (uint32)(208)), v10);
    xport_draft_host_sub_8006C4EC_p123(v8, v9, &v12);
    v13 = 12;
    xport_draft_host_sub_8006C5C4_p123(v7, v8, &v13);
    return xport_draft_host_sub_8006C0B8_p2(((uint32)(a1) + (uint32)(104)), v7);
}

/* TODO Missing host buffer adapter xport_draft_host_sub_800667CC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C0B8_p2 */
uint32 sub_80062044(uint32 a1)
{
    sint32 v2;
    sint32 result;
    char v4[16];
    v2 = sub_80066088(r_u16(((uint32)(((uint32)(a1) + (uint32)(214))))));
    sub_80064A08(v2);
    sub_80069DF0(47, 0x2000, 0);
    xport_draft_host_sub_800667CC_p1(v4, 16, ((uint32)(a1) + (uint32)(16)));
    xport_draft_host_sub_8006C0B8_p2(((uint32)(a1) + (uint32)(4)), v4);
    result = sub_8006E080(r_u32(((uint32)(((uint32)(a1) + (uint32)(328))))), r_u8(((uint32)(((uint32)(a1) + (uint32)(27))))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(22)))), result);
    return result;
}

uint32 sub_80029D24(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    uint32 v8;
    sint32 v9;
    sint32 v10;
    uint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    sub_800330F4(a1);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(67)))), 3);
    v8 = ((uint32)(r_u32(0x800FF5A0u)));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))), 0x800A1380u);
    v9 = r_u32((v8 + (2) * 4u));
    v10 = r_u32((v8 + (3) * 4u));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(24)))), r_u32((v8 + (1) * 4u)));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(28)))), v9);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(32)))), v10);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(92)))), a2);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(96)))), a3);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(100)))), a4);
    if (!a4)
        w_u32(((uint32)(((uint32)(a1) + (uint32)(100)))), 1);
    v11 = ((uint32)(sub_80032DC0(116)));
    if (v11)
        v11 = sub_80028FB8(v11, 16, -857728900);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(72)))), v11);
    w_u8((((uint32)(v11)) + (66) * 1u), 1);
    v12 = 0;
    sub_80029198(r_u32(((uint32)(((uint32)(a1) + (uint32)(72))))), ((uint32)(((uint32)(a1) + (uint32)(24)))), 0, 0);
    v14 = sub_80066570(64);
    v13 = sub_80066570(64);
    sub_80029324(r_u32(((uint32)(((uint32)(a1) + (uint32)(72))))), v14, v13, 2);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(80)))), 255);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(84)))), 100);
    v15 = r_u32(((uint32)(((uint32)(a1) + (uint32)(72)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(88)))), 0);
    sub_800293D8(v15, 255, 100, 0);
    v16 = r_u32(0x800FF4E8u);
    w_u16(((uint32)(((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(72)))))) + (uint32)(114)))), 1);
    sub_80029F58(a1, v16);
    sub_80029F58(a1, r_u32(0x800FF5DCu));
    v17 = a1;
    do
    {
        w_u32(((uint32)(((uint32)(v17) + (uint32)(104)))), sub_80066570(4096));
        ++v12;
        v17 += 4;
    } while ((((sint32)(v12)) < 16));
    sub_800774EC(r_u32(0x800FF904u), ((uint32)(a1) + (uint32)(24)), 1);
    w_u32(0x800FF218u, 1);
    w_u32(0x800FF21Cu, a1);
    sub_80069DF0(10, 0x2000, 0);
    return a1;
}

/* TODO Missing host buffer adapter xport_draft_host_sub_800667CC_p13 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C34C_p13 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C40C_p12 */
uint32 sub_80029198(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    uint32 v9;
    sint32 i;
    sint32 result;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    short v19;
    char v20[16];
    int v21[4];
    int v22[4];
    int v23[4];
    v5 = 0;
    v6 = r_u32((a2 + (1) * 4u));
    v7 = r_u32((a2 + (2) * 4u));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(24)))), r_u32(a2));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(28)))), v6);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(32)))), v7);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(96)))), a4);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(92)))), a3);
    v6 = ((v6 & 0xFFFF0000u) | (((r_u16(((uint32)(((uint32)(a1) + (uint32)(88)))))) & 0xFFFFu) << 0));
    v18 = r_u32(((uint32)(((uint32)(a1) + (uint32)(84)))));
    v19 = v6;
    v8 = r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))));
    v9 = ((uint32)(((uint32)(v8) + (uint32)(128))));
    for (i = (4096 / r_u32(((uint32)(((uint32)(a1) + (uint32)(76))))));; v18 = ((v18 & 0x0000FFFFu) | (((i) & 0xFFFFu) << 16)))
    {
        result = (((sint32)(v5)) < r_u32(((uint32)(((uint32)(a1) + (uint32)(76))))));
        if ((((sint32)(v5)) >= r_u32(((uint32)(((uint32)(a1) + (uint32)(76)))))))
            break;
        xport_draft_host_sub_800667CC_p13(v20, 1, &v18);
        xport_draft_host_sub_8006C40C_p12(v22, v20, ((uint32)(a1) + (uint32)(92)));
        xport_draft_host_sub_8006C34C_p13(v21, ((uint32)(a1) + (uint32)(24)), v22);
        v12 = v21[1];
        v13 = v21[2];
        w_u32((v9 - (6) * 4u), v21[0]);
        w_u32((v9 - (5) * 4u), v12);
        w_u32((v9 - (4) * 4u), v13);
        xport_draft_host_sub_8006C40C_p12(v21, v20, ((uint32)(a1) + (uint32)(96)));
        xport_draft_host_sub_8006C34C_p13(v23, ((uint32)(v8) + (uint32)(104)), v21);
        v14 = v23[1];
        v15 = v23[2];
        w_u32((v9 - (3) * 4u), v23[0]);
        w_u32((v9 - (2) * 4u), v14);
        w_u32((v9 - (1) * 4u), v15);
        xport_draft_host_sub_8006C34C_p13(v22, ((uint32)(v8) + (uint32)(116)), v21);
        v16 = v22[1];
        v17 = v22[2];
        w_u32(v9, v22[0]);
        w_u32((v9 + (1) * 4u), v16);
        w_u32((v9 + (2) * 4u), v17);
        v9 += (35) * 4u;
        v8 += 140;
        ++v5;
    }

    return result;
}

uint32 sub_8001E8B0(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9)
{
    sint32 v17;
    sint32 v18;
    sub_80034598(a1, a9, 2);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))), 0x800A113Cu);
    v17 = r_u32((a2 + (1) * 4u));
    v18 = r_u32((a2 + (2) * 4u));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(24)))), r_u32(a2));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(28)))), v17);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(32)))), v18);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(104)))), a3);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(105)))), a4);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(106)))), a5);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(108)))), a7);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(112)))), 0);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(116)))), a6);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(94)))), 1);
    sub_80034A18(a1, 0, 0, 0);
    sub_80034A44(a1, 0, 0, 0);
    sub_80034994(a1, 0);
    sub_800349D0(a1, 0, a8);
    sub_800349D0(a1, 1, a8);
    sub_80034A9C(a1, 0, r_u8(a1 + 104u), r_u8(a1 + 105u), r_u8(a1 + 106u));
    sub_80034A9C(a1, 1, 0, 0, 0);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(96)))), sub_80066570(1024));
    return a1;
}

uint32 sub_8001EA60(uint32 a1)
{
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    v2 = r_u8(((uint32)(((uint32)(a1) + (uint32)(104)))));
    v3 = (((sint32)(v2)) < r_u32(((uint32)(((uint32)(a1) + (uint32)(108))))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(112)))), (r_u32(((uint32)(((uint32)(a1) + (uint32)(112))))) + (r_u32(((uint32)(((uint32)(a1) + (uint32)(116))))))));
    if (v3)
        w_u8(((uint32)(((uint32)(a1) + (uint32)(104)))), 0);
    else
        w_u8(((uint32)(((uint32)(a1) + (uint32)(104)))), ((uint32)(v2) - (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(108))))))));
    v4 = r_u8(((uint32)(((uint32)(a1) + (uint32)(105)))));
    if ((((sint32)(v4)) < r_u32(((uint32)(((uint32)(a1) + (uint32)(108)))))))
        w_u8(((uint32)(((uint32)(a1) + (uint32)(105)))), 0);
    else
        w_u8(((uint32)(((uint32)(a1) + (uint32)(105)))), ((uint32)(v4) - (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(108))))))));
    v5 = r_u8(((uint32)(((uint32)(a1) + (uint32)(106)))));
    if ((((sint32)(v5)) < r_u32(((uint32)(((uint32)(a1) + (uint32)(108)))))))
        w_u8(((uint32)(((uint32)(a1) + (uint32)(106)))), 0);
    else
        w_u8(((uint32)(((uint32)(a1) + (uint32)(106)))), ((uint32)(v5) - (uint32)(r_u8(((uint32)(((uint32)(a1) + (uint32)(108))))))));
    if (!(((r_u8(((uint32)(((uint32)(a1) + (uint32)(106))))) | r_u8(((uint32)(((uint32)(a1) + (uint32)(104)))))) | r_u8(((uint32)(((uint32)(a1) + (uint32)(105))))))))
        sub_80032ED8(a1);
    sub_80034994(a1, r_u32(((uint32)(((uint32)(a1) + (uint32)(112))))));
    return sub_80034A9C(a1, 0, r_u8(a1 + 104u), r_u8(a1 + 105u), r_u8(a1 + 106u));
}

uint32 sub_8001EA0C(uint32 a1, uint32 a2)
{
    sint32 result;
    w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))), 0x800A113Cu);
    result = sub_800348A8(a1, 0);
    if (((a2 & 1) != 0))
        return sub_80032E30(a1);
    return result;
}

uint32 sub_8001EB64(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7)
{
    uint32 v14;
    uint32 v15;
    sint32 v16;
    uint32 result;
    sint32 v18;
    sint32 vars0;
    sint32 vars4;
    sint32 vars8;
    v14 = a3;
    v15 = a4;
    sub_8001E8B0(a1, a2, a3, a4, a5, 0, 0, 0, 6);
    v16 = ((v16 & 0xFFFFFF00u) | (((a3) & 0xFFu) << 0));
    w_u32((a1 + (17) * 4u), 0x800A1124u);
    w_u32((a1 + (30) * 4u), (((sint32)(a6)) / ((sint32)(a7))));
    if ((v14 < a5))
        v16 = ((v16 & 0xFFFFFF00u) | (((a5) & 0xFFu) << 0));
    v16 = ((unsigned char)(v16));
    if ((((unsigned char)(v16)) < v15))
        v16 = a4;
    result = a1;
    w_u32((a1 + (27) * 4u), (((sint32)(v16)) / ((sint32)(a7))));
    return result;
}

/* TODO Missing call adapter sub_800314CC */
/* TODO Missing call adapter sub_80031BA8 */
/* TODO Missing call adapter sub_80031E10 */
uint32 sub_80020068(uint32 object)
{
    uint32 kind = r_u16(object + 58u);
    uint32 child;
    w_u32(0x800FF3ACu, 0u);
    switch (kind)
    {
        case 1u: case 2u: case 3u: case 7u:
            child = sub_80032DC0(kind == 2u || kind == 3u ? 80u : 76u);
            if (child)
            {
                if (kind == 2u) child = sub_8003188C(child);
                else
                {
                    uint32 target = kind == 1u ? 0x80031BA8u : kind == 3u ? 0x80031E10u : 0x800314CCu;
                    /* TODO Implement the unselected component constructors */
                    fprintf(stderr, "Missing component constructor %08X receiver %08X\n", target, child);
                    abort();
                }
            }
            w_u32(object + 340u, child);
            break;
        default: break;
    }
    child = r_u32(object + 340u);
    if (child) w_u8(child + 66u, 1u);
    w_u32(0x800FF3ACu, 1u);
    return 1u;
}

uint32 sub_800260FC(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 result;
    sub_8003304C(a1);
    w_u32((a1 + (17) * 4u), 0x800A15B8u);
    w_u32((a1 + (19) * 4u), a2);
    w_u32((a1 + (20) * 4u), sub_8006B864(((uint32)(36) * (uint32)(a2)), 0, 1));
    result = a1;
    w_u32((a1 + (18) * 4u), a3);
    return result;
}

/* TODO Missing call adapter xport_draft_missing_gte_adapter */
uint32 sub_80025248(uint32 position, uint32 screen_output)
{
    uint32 x = (uint32)((sint32)r_u32(position) >> 12) - r_u32(0x800ED520u);
    uint32 y = (uint32)((sint32)r_u32(position + 4u) >> 12) - r_u32(0x800ED524u);
    uint32 z = (uint32)((sint32)r_u32(position + 8u) >> 12) - r_u32(0x800ED528u);
    xport_draft_gte_data_write(0u, (x & 0xFFFFu) | (y << 16));
    xport_draft_gte_data_write(1u, z);
    xport_draft_gte_execute(0x180001u);
    w_u32(screen_output, xport_draft_gte_data_read(14u));
    return xport_draft_gte_data_read(27u);
}

uint32 sub_80062BF0(uint32 a1)
{
    sint32 result;
    result = (r_u16(((uint32)(((uint32)(a1) + (uint32)(78))))) & 1);
    if (result)
    {
        sub_80062A64(a1, 0x800FF5E0u);
        sub_80062A38(a1, ((sint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(72))))))));
        result = (r_u16(((uint32)(((uint32)(a1) + (uint32)(78))))) & 0xFFFE);
        w_u16(((uint32)(((uint32)(a1) + (uint32)(78)))), result);
    }
    return result;
}

uint32 sub_8003BAE0(uint32 a1, uint32 a2)
{
    sint32 v4;
    sint32 result;
    w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))), 0x800A1DF0u);
    sub_80062A64(a1, 0x800FF5DCu);
    v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(520)))));
    if (v4)
    {
        sub_8006A294(v4);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(520)))), 0);
    }
    sub_8004B868(a1, 0);
    result = (a2 & 1);
    if (((a2 & 1) != 0))
        return sub_80062608(a1);
    return result;
}

uint32 sub_8005CE70(uint32 a1, uint32 a2)
{
    short v3;
    sint32 v4;
    v3 = ((uint32)(r_u16(((uint32)(((uint32)(a1) + (uint32)(218)))))) + (uint32)(a2));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(218)))), v3);
    if ((r_u16(0x800EC526u) < ((sint32)(v3))))
        w_u16(((uint32)(((uint32)(a1) + (uint32)(218)))), r_u16(0x800EC526u));
    v4 = sub_80066570(2);
    sub_8002FC64(1, v4, ((uint32)(a1) + (uint32)(4)), 1);
    return 1;
}

uint32 sub_80027878(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11, uint32 a12, uint32 a13)
{
    sint32 result;
    result = a5;
    w_u8((a1 + (80) * 1u), a2);
    w_u8((a1 + (81) * 1u), a3);
    w_u8((a1 + (82) * 1u), a4);
    w_u8((a1 + (83) * 1u), a5);
    w_u8((a1 + (84) * 1u), a6);
    w_u8((a1 + (85) * 1u), a7);
    w_u8((a1 + (86) * 1u), a8);
    w_u8((a1 + (87) * 1u), a9);
    w_u8((a1 + (88) * 1u), a10);
    w_u8((a1 + (89) * 1u), a11);
    w_u8((a1 + (90) * 1u), a12);
    w_u8((a1 + (91) * 1u), a13);
    return result;
}

uint32 sub_80022B34(uint32 a1, uint32 a2)
{
    sint32 result;
    result = ((uint32)(r_u32(((uint32)(((uint32)(a1) + (uint32)(56)))))) - (uint32)(a2));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(56)))), result);
    if ((((sint32)(result)) < 0))
        w_u32(((uint32)(((uint32)(a1) + (uint32)(56)))), 0);
    return result;
}

/* TODO Missing host buffer adapter xport_draft_host_sub_800667CC_p1 */
/* TODO Missing host buffer adapter xport_draft_host_sub_8006C0FC_p2 */
uint32 sub_80022B54(uint32 a1, uint32 a2, uint32 a3)
{
    char v5[16];
    xport_draft_host_sub_800667CC_p1(v5, 100, a3);
    return xport_draft_host_sub_8006C0FC_p2(a2, v5);
}

/* TODO Missing call adapter sub_8008BF8C */
/* TODO Missing SDK scalar service 8008BF8C */
static sint32 apocalypse_missing_scalar_8008BF8C(void)
{
    fprintf(stderr, "Missing SDK scalar service 8008BF8C\n");
    abort();
}

uint32 sub_800284E8(uint32 object, uint32 red, uint32 green, uint32 blue)
{
    if (!r_u32(object + 60u)) return 0u;
    sint32 sample;
    sample = apocalypse_missing_scalar_8008BF8C();
    if (sample >= 0x4000) return 0u;
    w_u8(red, r_u8(object + 80u));
    w_u8(green, r_u8(object + 81u));
    w_u8(blue, r_u8(object + 82u));
    return 1u;
}

uint32 sub_8002616C(uint32 a1, uint32 a2)
{
    sint8 v3;
    sint32 v4;
    sint32 result;
    v3 = a2;
    v4 = r_u32(((uint32)(((uint32)(a1) + (uint32)(80)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))), 0x800A15B8u);
    ((void)(a2), sub_8006BC20(v4));
    result = sub_80033090(a1, 0);
    if (((v3 & 1) != 0))
        return sub_80032E30(a1);
    return result;
}

/* TODO Missing call adapter indirect */
void sub_800278D0(uint32 object)
{
    uint32 child, next, table;
    child = r_u32(object + 60u);
    if (child)
    {
        table = r_u32(child + 68u);
        (void)apocalypse_object_cleanup(r_u32(table + 12u), child + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
    }
    next = r_u32(object + 64u);
    w_u32(object + 60u, 0u);
    if (next)
    {
        table = r_u32(next + 68u);
        (void)apocalypse_object_cleanup(r_u32(table + 12u), next + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
    }
    child = r_u32(object + 92u);
    w_u32(object + 64u, 0u);
    if (child)
    {
        table = r_u32(child + 68u);
        (void)apocalypse_object_cleanup(r_u32(table + 12u), child + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
    }
    w_u32(object + 92u, 0u);
    return;
}

/* TODO Missing call adapter indirect */
uint32 sub_8001CA68(uint32 object)
{
    uint32 player = r_u32(0x800FF5A0u);
    sint32 x = (sint32)(r_u32(object + 24u) - r_u32(player + 4u)) >> 12;
    sint32 z = (sint32)(r_u32(object + 32u) - r_u32(player + 12u)) >> 12;
    sint32 distance;
    uint32 result;
    if (x < 0) x = (sint32)(0u - (uint32)x);
    if (z < 0) z = (sint32)(0u - (uint32)z);
    distance = x < z ? (sint32)((uint32)z + (uint32)(x / 2))
                     : (sint32)((uint32)x + (uint32)(z / 2));
    result = distance < (sint32)r_u32(object + 92u);
    if (!result)
    {
        uint32 child = r_u32(object + 96u);
        if (child)
        {
            uint32 table = r_u32(child + 68u);
            result = apocalypse_object_cleanup(r_u32(table + 12u),
                child + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
            w_u32(object + 96u, 0u);
        }
    }
    else
    {
        if (!r_u32(object + 96u))
        {
            uint32 child;
            w_u32(0x800FF3ACu, 0u);
            child = sub_80032DC0(176u);
            if (child)
                child = sub_80034C0C(child, object + 24u,
                    r_u32(object + 72u), r_u32(object + 76u), r_u32(object + 88u),
                    r_u8(object + 80u), r_u8(object + 81u), r_u8(object + 82u),
                    r_u8(object + 83u), r_u8(object + 84u), r_u8(object + 85u));
            w_u32(object + 96u, child);
            w_u32(0x800FF3ACu, 1u);
            /* The original also stores through a zero allocation result */
            w_u8(child + 66u, 1u);
        }
        result = (uint32)((sint32)(((uint32)distance * 4u + (uint32)distance) * 2u) / 16);
        w_u16(r_u32(object + 96u) + 96u, result);
    }
    return result;
}

uint32 sub_80034C0C(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10, uint32 a11)
{
    uint32 v17;
    sint32 v18;
    uint32 v19;
    unsigned char vars0;
    unsigned char vars4;
    unsigned char vars8;
    unsigned char varsC;
    sub_800346A8(((sint32)(a1)), a2, a3, a4, a6, a7, a8, a9, a10, a11);
    w_u32((a1 + (17) * 4u), 0x800A1CF0u);
    w_u32((a1 + (43) * 4u), a3);
    v17 = 0;
    v18 = r_u32((a1 + (20) * 4u));
    w_u32((a1 + (42) * 4u), (((sint32)(((uint32)(a3) * (uint32)(a5)))) / 256));
    if (v18)
    {
        v19 = a1;
        do
        {
            w_u32((v19 + (34) * 4u), sub_80066570(4096));
            w_u32((v19 + (26) * 4u), ((uint32)(sub_80066570(50)) + (uint32)(50)));
            ++v17;
            (v19 += 4u);
        } while ((v17 < r_u32((a1 + (20) * 4u))));
    }
    return a1;
}

/* TODO Missing call adapter SLOWORD */
uint32 sub_80034CF8(uint32 object)
{
    uint32 result = r_u32(object + 80u);
    uint32 index = 0u, cursor = object;
    if (result)
    {
        do
        {
            uint32 phase = r_u32(cursor + 136u) + r_u32(cursor + 104u);
            sint32 product;
            uint32 output;
            w_u32(cursor + 136u, phase);
            product = (sint32)(r_u32(object + 168u) *
                (uint32)(sint32)(sint16)r_u16(0x800F863Cu + (phase & 4095u) * 4u));
            output = r_u32(object + 72u) + index * 8u;
            w_u32(output, r_u32(object + 172u) + (uint32)(product / 4096));
            ++index;
            result = index < r_u32(object + 80u);
            cursor += 4u;
        } while (result);
    }
    return result;
}

/* The cleanup caller does not consume incidental V0 */
void sub_8006EDD4(uint32 a1)
{
    uint32 index = sub_8006ED70(a1);
    if (r_u8(0x800EAEF8u + 64u * index + 10u))
    {
        index = sub_8006ED70(a1);
        sub_8006EE94(index, 1u);
    }
}

uint32 sub_80036D48(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10)
{
    sint32 v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    short v24;
    sint32 result;
    sub_80032F7C(a1);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(68)))), 0x800A1B98u);
    sub_80032E50(((uint32)(a1)), 0x800FF454u);
    v16 = r_u32((a2 + (1) * 4u));
    v17 = r_u32((a2 + (2) * 4u));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(24)))), r_u32(a2));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(28)))), v16);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(32)))), v17);
    v18 = ((uint32)(((uint32)(2) * (uint32)(a10))) + (uint32)(1));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(72)))), ((uint32)(r_u32(a2)) + (uint32)(((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a8))) + (uint32)(1)))) - (uint32)(a8))) << (uint32)(12)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(76)))), ((uint32)(r_u32((a2 + (1) * 4u))) + (uint32)(((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a9))) + (uint32)(1)))) - (uint32)(a9))) << (uint32)(12)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(80)))), ((uint32)(r_u32((a2 + (2) * 4u))) + (uint32)(((uint32)(((uint32)(sub_80066570(v18)) - (uint32)(a10))) << (uint32)(12)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(84)))), ((uint32)(r_u32(a2)) + (uint32)(((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a8))) + (uint32)(1)))) - (uint32)(a8))) << (uint32)(12)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(88)))), ((uint32)(r_u32((a2 + (1) * 4u))) + (uint32)(((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a9))) + (uint32)(1)))) - (uint32)(a9))) << (uint32)(12)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(92)))), ((uint32)(r_u32((a2 + (2) * 4u))) + (uint32)(((uint32)(((uint32)(sub_80066570(v18)) - (uint32)(a10))) << (uint32)(12)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(96)))), ((uint32)(r_u32(a2)) + (uint32)(((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a8))) + (uint32)(1)))) - (uint32)(a8))) << (uint32)(12)))));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(100)))), ((uint32)(r_u32((a2 + (1) * 4u))) + (uint32)(((uint32)(((uint32)(sub_80066570(((uint32)(((uint32)(2) * (uint32)(a9))) + (uint32)(1)))) - (uint32)(a9))) << (uint32)(12)))));
    v19 = (a5 >> 2);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(104)))), ((uint32)(r_u32((a2 + (2) * 4u))) + (uint32)(((uint32)(((uint32)(sub_80066570(v18)) - (uint32)(a10))) << (uint32)(12)))));
    v20 = r_u32((a3 + (1) * 4u));
    v21 = r_u32((a3 + (2) * 4u));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(36)))), r_u32(a3));
    w_u32(((uint32)(((uint32)(a1) + (uint32)(40)))), v20);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(44)))), v21);
    v22 = (a6 >> 2);
    w_u32(((uint32)(((uint32)(a1) + (uint32)(108)))), a4);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(112)))), v19);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(113)))), v22);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(115)))), v19);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(116)))), v22);
    v23 = (a7 >> 2);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(114)))), v23);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(117)))), v23);
    v24 = ((uint32)(sub_80066570(30)) + (uint32)(30));
    result = a1;
    w_u16(((uint32)(((uint32)(a1) + (uint32)(10)))), v24);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(118)))), 4);
    return result;
}

/* TODO Missing call adapter indirect */
uint32 sub_8001C9EC(uint32 object, uint32 flags)
{
    uint32 child = r_u32(object + 96u);
    w_u32(object + 68u, 0x800A10B4u);
    if (child)
    {
        uint32 table = r_u32(child + 68u);
        (void)apocalypse_object_cleanup(r_u32(table + 12u),
            child + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
    }
    sub_80033138(object, 0u);
    if (flags & 1u) return sub_80032E30(object);
    return flags & 1u;
}

uint32 sub_8003A0C4(uint32 a1, uint32 a2)
{
    uint32 result;
    sint32 v5;
    w_u32((a1 + (17) * 4u), 0x800A1CF0u);
    result = sub_800348A8(a1, 0);
    if (((a2 & 1) != 0))
        return ((uint32)(sub_80032E30(((sint32)(a1)))));
    return result;
}

uint32 sub_8004FE20(uint32 a1)
{
    sint32 v1;
    sint32 result;
    v1 = r_u16(((uint32)(((uint32)(a1) + (uint32)(390)))));
    w_u16(((uint32)(((uint32)(a1) + (uint32)(472)))), 13);
    w_u8(((uint32)(((uint32)(a1) + (uint32)(380)))), 0);
    w_u16(((uint32)(((uint32)(a1) + (uint32)(474)))), 0);
    if ((v1 == 2))
    {
        result = (r_u32(((uint32)(((uint32)(a1) + (uint32)(372))))) | 0xFF80);
        w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))), result);
    }
    else
    {
        result = 1;
        if ((((sint32)(v1)) >= 3))
        {
            result = 8;
            if ((v1 == 4))
            {
                result = (r_u32(((uint32)(((uint32)(a1) + (uint32)(372))))) | 0x7D);
                w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))), result);
            }
            else if ((v1 == 8))
            {
                result = (r_u32(((uint32)(((uint32)(a1) + (uint32)(372))))) | 0x1FF80);
                w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))), result);
            }
        }
        else if ((v1 == 1))
        {
            result = (r_u32(((uint32)(((uint32)(a1) + (uint32)(372))))) | 0x1F80);
            w_u32(((uint32)(((uint32)(a1) + (uint32)(372)))), result);
        }
    }
    return result;
}

void sub_800774B4(uint32 a1, uint32 a2)
{
    w_u16(((uint32)(((uint32)(a1) + (uint32)(546)))), a2);
}

uint32 sub_8005DD40(uint32 a1)
{
    sint32 result;
    short v3;
    result = sub_80062F48(a1);
    if (!result)
    {
        result = (((unsigned short)(r_u16((a1 + (39) * 2u)))) | 0x40);
        v3 = (r_u16(a1) | 1);
        w_u16((a1 + (39) * 2u), result);
        w_u16(a1, v3);
    }
    return result;
}

/* TODO Missing call adapter sub_80097F34 */
/* TODO Missing host buffer adapter xport_draft_host_sub_80093ED8_p2 */
uint32 sub_8006A1DC(void)
{
    uint32 pending = r_u32(0x800FF6B0u), voice, left, right;
    uint16 attributes[8];
    if (!pending)
        return pending;
    w_u32(0x800FF6B0u, 0u);
    attributes[3] = 3u;
    for (voice = 0u; voice < 24u; ++voice)
    {
        left = r_u16(0x800E5BA8u + voice * 2u);
        right = r_u16(0x800E5BD8u + voice * 2u);
        if (left | right)
        {
            attributes[0] = (uint16)left;
            attributes[1] = r_u16(0x800E5BD8u + voice * 2u);
            /* Only volume fields and mask are defined by the original caller */
            xport_draft_host_sub_80093ED8_p2(voice, attributes);
        }
    }
    if (!r_u32(0x800FFC20u))
        sub_8006A428();
    return sub_80097F34(12u, 0u);
}

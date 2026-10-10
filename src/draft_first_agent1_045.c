#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>

/* Unverified draft; TODO Recover omitted call arguments, host-buffer adapters and signed field widths */

uint32 sub_8001B654(void)
{
    sint32 result;
    sint32 v1;
    result = sub_8006B864(20, 0, 1);
    v1 = r_u32(0x800FF1ACu);
    w_u32(((uint32)((result + 16))), 0);
    w_u32(0x800FF1ACu, result);
    w_u32(((uint32)((result + 12))), v1);
    if (v1)
        w_u32(((uint32)((v1 + 16))), result);
    return result;
}

uint32 sub_800201F8(uint32 a1, uint32 a2)
{
    sint32 result;
    w_u32(((uint32)((a1 + 68))), 0x800A11ACu);
    sub_80062A64(a1, 0x800FF204u);
    sub_80020168(a1);
    result = sub_800629BC(a1, 0);
    if (((a2 & 1) != 0))
        return sub_80062608(a1);
    return result;
}

uint32 sub_80027720(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6)
{
    sub_80022A24(a1, a2, 0);
    w_u32(a1, 0x800A1408u);
    w_u32((a1 + (1) * 4u), 3);
    w_u32((a1 + (19) * 4u), a4);
    w_u32((a1 + (25) * 4u), a6);
    w_u32((a1 + (24) * 4u), a5);
    w_u32((a1 + (18) * 4u), ((sint32)a3 / 9));
    sub_80027878(a1, 255, 128, 0, 185, 185, 185, 160, 80, 0, 255, 255, 255);
    return a1;
}

void sub_800284A4(uint32 a1)
{
    sint32 v2;
    v2 = r_u32((a1 + (13) * 4u));
    if (v2)
        sub_8006A294(v2);
    w_u32((a1 + (13) * 4u), 0);
    sub_800278D0(a1);
}

uint32 sub_80066918(uint32 left, uint32 right)
{
    uint32 x = r_u32(left);
    uint32 other_x = r_u32(right);
    uint32 z, other_z;
    x = (uint32)((sint32)(x - other_x) >> 12);
    z = r_u32(left + 8u);
    other_z = r_u32(right + 8u);
    z = (uint32)((sint32)(z - other_z) >> 12);
    return sub_80085B54(x * x + z * z);
}

uint32 sub_800346A8(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8, uint32 a9, uint32 a10)
{
    sint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    uint32 i;
    uint32 v23;
    uint32 v24;
    sint32 v25;
    sub_80032F7C(a1);
    w_u32(((uint32)((a1 + 68))), 0x800A1D08u);
    w_u32(((uint32)((a1 + 80))), 8);
    w_u16(((uint32)((a1 + 92))), 512);
    v17 = r_u32(((uint32)((a1 + 80))));
    w_u32(((uint32)((a1 + 84))), 1);
    v18 = sub_8006B864((16 * v17), 0, 1);
    v19 = r_u32(((uint32)((a1 + 80))));
    w_u32(((uint32)((a1 + 72))), v18);
    w_u32(((uint32)((a1 + 76))), (v18 + (8 * v19)));
    sub_80032E50(((uint32)(a1)), 0x800FF450u);
    v20 = r_u32((a2 + (1) * 4u));
    v21 = r_u32((a2 + (2) * 4u));
    w_u32(((uint32)((a1 + 24))), r_u32(a2));
    w_u32(((uint32)((a1 + 28))), v20);
    w_u32(((uint32)((a1 + 32))), v21);
    for (i = 0; (i < r_u32(((uint32)((a1 + 80))))); ++i)
        w_u32(((uint32)(((8 * i) + r_u32(((uint32)((a1 + 72))))))), a3);

    v23 = 0;
    for (w_u32(((uint32)((a1 + 88))), ((((a7 << 16) | (a6 << 8)) | 0x32000000) | a5)); (v23 < r_u32(((uint32)((a1 + 80))))); ++v23)
        w_u32(((uint32)((((8 * v23) + r_u32(((uint32)((a1 + 72))))) + 4))), (((a10 << 16) | (a9 << 8)) | a8));

    v24 = 0;
    if (r_u32(((uint32)((a1 + 80)))))
    {
        v25 = 0;
        do
        {
            w_u32(((uint32)((v25 + r_u32(((uint32)((a1 + 76))))))), a4);
            w_u32(((uint32)(((v25 + r_u32(((uint32)((a1 + 76))))) + 4))), 973078528);
            ++v24;
            v25 = (8 * v24);
        } while ((v24 < r_u32(((uint32)((a1 + 80))))));
    }
    w_u32(((uint32)((a1 + 100))), -1);
    return a1;
}

void sub_8007749C(uint32 a1, uint32 a2)
{
    w_u32(((uint32)((a1 + 536))), a2);
}

uint32 sub_8002F50C(uint32 a1, uint32 a2)
{
    sint32 i;
    sint32 result;
    sint32 v4;
    uint32 v5;
    sint32 v6;
    uint32 v7;
    uint32 v8;
    uint32 v9;
    sint32 v10;
    uint32 v11;
    uint32 v12;
    uint32 v13;
    uint32 v14;
    uint32 v15;
    uint32 v16;
    uint32 v17;
    uint32 v18;
    uint32 v19;
    uint32 v20;
    uint32 v21;
    uint32 v22;
    sint32 v23;
    sint32 v24;
    for (i = 0;; ++i)
    {
        result = i;
        if ((i >= 2))
            break;
        v4 = r_u32((0x800FF778u + (result) * 4u));
        if ((v4 != -1))
        {
            v5 = 0;
            v6 = r_u32((0x800EAEF8u + (((16 * v4) + 4)) * 4u));
            v7 = r_u32(((uint32)((v6 - 4))));
            while (1)
            {
                v8 = 0;
                if ((v5 >= v7))
                    break;
                v9 = r_u32(((uint32)((r_u32(((uint32)(v6))) + 12))));
                v10 = (((r_u32(((uint32)(v6))) + (8 * r_u32(((uint32)((r_u32(((uint32)(v6))) + 4)))))) + 32) + (8 * r_u32(((uint32)((r_u32(((uint32)(v6))) + 8))))));
                while ((v8 < v9))
                {
                    if ((((((r_u32(((uint32)(v10))) & 1) != 0) && ((r_u32(((uint32)(v10))) & 2) != 0)) && (r_u16(((uint32)((v10 + 26)))) == r_u16((((uint32)(a2)) + (3) * 2u)))) && (r_u16(((uint32)((v10 + 22)))) == r_u16((((uint32)(a2)) + (1) * 2u)))))
                    {
                        v11 = r_u8(((uint32)((v10 + 20))));
                        v12 = r_u8(a2);
                        if ((v11 >= v12))
                        {
                            v13 = r_u8((a2 + (4) * 1u));
                            if ((v13 >= v11))
                            {
                                v14 = r_u8(((uint32)((v10 + 24))));
                                if (((v14 >= v12) && (v13 >= v14)))
                                {
                                    v15 = r_u8(((uint32)((v10 + 28))));
                                    if (((v15 >= v12) && (v13 >= v15)))
                                    {
                                        v16 = r_u8(((uint32)((v10 + 30))));
                                        if (((v16 >= v12) && (v13 >= v16)))
                                        {
                                            v17 = r_u8(((uint32)((v10 + 21))));
                                            v18 = r_u8((a2 + (1) * 1u));
                                            if ((v17 >= v18))
                                            {
                                                v19 = r_u8((a2 + (9) * 1u));
                                                if ((v19 >= v17))
                                                {
                                                    v20 = r_u8(((uint32)((v10 + 25))));
                                                    if (((v20 >= v18) && (v19 >= v20)))
                                                    {
                                                        v21 = r_u8(((uint32)((v10 + 29))));
                                                        if (((v21 >= v18) && (v19 >= v21)))
                                                        {
                                                            v22 = r_u8(((uint32)((v10 + 31))));
                                                            if (((v22 >= v18) && (v19 >= v22)))
                                                            {
                                                                w_u16(((uint32)((v10 + 26))), a1);
                                                                w_u8(((uint32)((v10 + 20))), ((r_u8(((uint32)((v10 + 20)))) + 1) >> 1));
                                                                v23 = (r_u8(((uint32)((v10 + 28)))) + 1);
                                                                w_u8(((uint32)((v10 + 24))), ((r_u8(((uint32)((v10 + 24)))) + 1) >> 1));
                                                                v24 = r_u8(((uint32)((v10 + 30))));
                                                                w_u8(((uint32)((v10 + 28))), (v23 >> 1));
                                                                w_u8(((uint32)((v10 + 30))), ((v24 + 1) >> 1));
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    ++v8;
                    v10 += (r_u16(((uint32)((v10 + 2)))) & 0xFFFC);
                }

                v6 += 4;
                ++v5;
            }
        }
    }

    return (result * 4);
}

uint32 sub_8002EF68(void)
{
    uint32 result = r_u32(0x800A5B8Cu + 28u * r_u8(0x800FF299u) + 24u);
    uint32 remaining, base, cursor, size, stride, table, count, i;
    if (!result)
        return result;
    result = r_u32(0x800FF250u);
    if (result)
        return result;
    w_u16(0x800FF29Cu, r_u16(0x800FF28Au));
    w_u16(0x800FF29Eu, r_u16(0x800FF28Cu));
    w_u16(0x800FF2A0u, r_u8(0x800FF298u) ? 24u : 16u);
    w_u32(0x800FF26Cu, 0u);
    w_u32(0x800FF248u, 0u);
    w_u32(0x800FF258u, 0u);
    w_u32(0x800FF25Cu, 0u);
    w_u32(0x800FF260u, 0u);
    w_u16(0x800FF2A2u, r_u16(0x800FF292u));
    base = apocalypse_heap_available(&remaining);
    if (remaining <= 0x11000u)
        goto exhausted;
    w_u32(0x800FFB88u, base);
    cursor = base + 0x11000u;
    table = 0x800FF2A4u;
    size = r_u32(0x800FF280u);
    stride = size & 0xFFFFFFFCu;
    remaining -= 0x11000u;
    for (i = 0u; i < 2u; ++i)
    {
        if (size >= remaining)
            goto exhausted;
        w_u32(table, cursor);
        cursor += stride;
        table += 4u;
        remaining -= size;
    }
    size = r_u32(0x800FF284u) << 11;
    if (size >= remaining)
        goto exhausted;
    w_u32(0x800FFB84u, cursor);
    cursor += size;
    count = (r_u16(0x800FF290u) >> 4) & 255u;
    w_u8(0x800FF288u, count);
    remaining -= size;
    table = 0x800A5F68u;
    stride = r_u16(0x800FF292u) << 5;
    for (i = 0u; i < count; ++i)
    {
        if (stride >= remaining)
            goto exhausted;
        w_u32(table, cursor);
        cursor += stride;
        remaining -= stride;
        table += 4u;
    }
    sub_8009C65C(r_u32(0x800FFB88u));
    w_u32(0x800FF250u, 1u);
    w_u32(0x800FF24Cu, 0u);
    sub_8002F7C8();
    return sub_8002F9E4();
exhausted:
    w_u32(0x800FF254u, 1u);
    return 1u;
}

uint32 apocalypse_heap_available(uint32 *bytes)
{
    uint32 base = r_u32(0x800FF728u);
    uint32 total = r_u32(0x800FF72Cu) - base;
    uint32 first = r_u32(0x800FF730u), last = r_u32(0x800FF734u);
    uint32 first_size = first ? r_u32(first + 4u) >> 4 : 0u;
    uint32 last_size = last ? r_u32(last + 4u) >> 4 : 0u;
    if (first && last && first_size + last_size + 16u == total)
    {
        *bytes = 0u;
        return 0u;
    }
    if (first)
    {
        *bytes = total - first_size - (last ? last_size + 16u : 8u);
        return base + first_size + 8u;
    }
    *bytes = total - (last ? last_size + 8u : 0u);
    return base;
}

uint32 sub_8006B71C(uint32 bytes)
{
    uint32 available;
    uint32 result = apocalypse_heap_available(&available);
    w_u32(bytes, available);
    return result;
}

uint32 sub_80027804(uint32 a1, uint32 a2)
{
    sint32 v4;
    sint32 result;
    v4 = r_u32((a1 + (13) * 4u));
    w_u32(a1, 0x800A1408u);
    if (v4)
        sub_8006A294(v4);
    w_u32((a1 + (13) * 4u), 0);
    sub_800278D0(a1);
    sub_80022A48(a1, 0);
    result = (a2 & 1);
    if (((a2 & 1) != 0))
        return sub_8002FF34(a1);
    return result;
}

uint32 sub_80022A48(uint32 a1, uint32 a2)
{
    sint32 v4;
    sint32 result;
    w_u32(a1, 0x800A1570u);
    sub_80022ABC(a1);
    v4 = r_u32((a1 + (13) * 4u));
    if (v4)
        sub_8006A294(v4);
    w_u32(a1, 0x800A15D8u);
    result = (a2 & 1);
    if (((a2 & 1) != 0))
        return sub_8002FF34(a1);
    return result;
}

uint32 sub_8005398C(uint32 a1)
{
    uint32 result;
    result = (r_u32(((uint32)((a1 + 396)))) & 0xFFFFFF3F);
    w_u32(((uint32)((a1 + 396))), result);
    return result;
}

uint32 sub_80022454(uint32 a1, uint32 a2, uint32 a3, uint32 a4, uint32 a5, uint32 a6, uint32 a7, uint32 a8)
{
    sint32 v14;
    sint32 v15;
    sint32 i;
    sint32 v17;
    uint32 result;
    uint32 v19;
    sint32 transformed[3];
    char v23[16];
    sint32 collision[35];
    char v32[16];
    sint32 position[3];
    sint32 velocity[3];
    int v39[4];
    char v40[16];
    char v41[16];
    char v42[16];
    sint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    sint32 v52;
    sint8 v53;
    sint8 v54;
    sint8 v55;
    v53 = a6;
    v54 = a7;
    v55 = a8;
    sub_8006C34C(v23, a3, a4);
    v43 = 1;
    sub_8006C564(&transformed[0], v23, &v43);
    collision[0] = (transformed[0] + (500 * r_u32(a5)));
    collision[1] = (transformed[1] + (500 * r_u32((a5 + (1) * 4u))));
    v14 = r_u32((a5 + (2) * 4u));
    collision[4] = (collision[1] + 12288000);
    collision[3] = collision[0];
    collision[2] = (transformed[2] + (500 * v14));
    collision[5] = collision[2];
    xport_draft_host_sub_8007BB24_p1(collision);
    xport_draft_host_sub_8007DD04_p1(collision, 1);
    v15 = 0x7FFFFFFF;
    if (collision[26])
        v15 = collision[28];
    sub_8006C3AC(v32, a4, a2);
    v44 = 12;
    sub_8006C564(v23, v32, &v44);
    sub_8006C3AC(&position[0], a3, a2);
    v45 = 12;
    sub_8006C564(v32, &position[0], &v45);
    for (i = 0; (i < a1); ++i)
    {
        v46 = sub_80066570(4096);
        v47 = sub_80066570(4096);
        sub_8006C47C(v39, &v46, v23);
        sub_8006C47C(v40, &v47, v32);
        sub_8006C34C(&velocity[0], v39, v40);
        v48 = 12;
        sub_8006C564(&position[0], &velocity[0], &v48);
        v49 = 12;
        sub_8006C5C4(v39, &position[0], &v49);
        sub_8006C34C(&velocity[0], v39, a2);
        position[0] = velocity[0];
        position[1] = velocity[1];
        position[2] = velocity[2];
        sub_8006C3AC(&velocity[0], &position[0], &transformed[0]);
        sub_800666DC(&velocity[0], &velocity[0]);
        v50 = sub_80066570(40);
        sub_8006C40C(v41, &velocity[0], &v50);
        v51 = sub_80066570(40);
        sub_8006C40C(v42, a5, &v51);
        sub_8006C34C(v39, v41, v42);
        velocity[0] = v39[0];
        velocity[1] = v39[1];
        velocity[2] = v39[2];
        v17 = sub_80032DC0(120);
        if (v17)
            xport_draft_host_sub_80036D48_p23(v17, position, velocity, v15, (uint8)a6, (uint8)a7, (uint8)a8, 100, 100, 100);
    }

    if (r_u32(0x800FF214u))
    {
        v52 = 12;
        position[0] = r_u32(0x800ED520u);
        position[1] = r_u32(0x800ED524u);
        position[2] = r_u32(0x800ED528u);
        sub_8006C22C(&position[0], &v52);
        v19 = sub_8006696C(&position[0], &transformed[0]);
        result = sub_8006696C(&position[0], 0x800FFD48u);
        if ((v19 < result))
        {
            w_u32(0x800FFD48u, transformed[0]);
            w_u32(0x800FFD4Cu, transformed[1]);
            w_u32(0x800FFD50u, transformed[2]);
        }
    }
    else
    {
        w_u32(0x800FFD48u, transformed[0]);
        w_u32(0x800FFD4Cu, transformed[1]);
        w_u32(0x800FFD50u, transformed[2]);
        result = 1;
        w_u32(0x800FF214u, 1);
    }
    return result;
}

uint32 sub_80036FFC(uint32 a1)
{
    sint32 v2;
    sint32 v3;
    short v4;
    sint32 v5;
    sint32 v6;
    sint8 v7;
    sint8 v8;
    sint32 v9;
    short v10;
    uint32 v11;
    uint32 v12;
    sint8 v13;
    uint32 v14;
    uint32 v15;
    sint8 v16;
    uint32 v17;
    uint32 v18;
    sint8 v19;
    sint32 result;
    sub_8006C0B8((a1 + 72), (a1 + 36));
    sub_8006C0B8((a1 + 84), (a1 + 36));
    sub_8006C0B8((a1 + 96), (a1 + 36));
    v2 = (r_u32(((uint32)((a1 + 108)))) >= r_u32(((uint32)((a1 + 76)))));
    w_u32(((uint32)((a1 + 40))), (r_u32(((uint32)((a1 + 40)))) + (10000)));
    if (!v2)
    {
        if ((r_u16(((uint32)((a1 + 8)))) >= 2))
        {
            w_u32(((uint32)((a1 + 40))), 0);
        }
        else
        {
            w_u32(((uint32)((a1 + 76))), (r_u32(((uint32)((a1 + 108)))) - (sub_80066570(50) << 12)));
            w_u32(((uint32)((a1 + 88))), (r_u32(((uint32)((a1 + 108)))) - (sub_80066570(50) << 12)));
            v3 = (r_u32(((uint32)((a1 + 108)))) - (sub_80066570(50) << 12));
            v4 = r_u16(((uint32)((a1 + 8))));
            v5 = ((0u - (r_u32(((uint32)((a1 + 40)))))) >> 2);
            w_u32(((uint32)((a1 + 100))), v3);
            w_u32(((uint32)((a1 + 40))), v5);
            w_u16(((uint32)((a1 + 8))), (v4 + 1));
        }
        v6 = (r_u32(((uint32)((a1 + 44)))) >> 1);
        w_u32(((uint32)((a1 + 36))), (r_u32(((uint32)((a1 + 36)))) >> (1)));
        w_u32(((uint32)((a1 + 44))), v6);
    }
    if (((r_u16(((uint32)((a1 + 8)))) >= 2) || sub_80066570(5)))
    {
        v7 = r_u8(((uint32)((a1 + 113))));
        v8 = r_u8(((uint32)((a1 + 114))));
        w_u8(((uint32)((a1 + 115))), r_u8(((uint32)((a1 + 112)))));
        w_u8(((uint32)((a1 + 116))), v7);
        w_u8(((uint32)((a1 + 117))), v8);
    }
    else
    {
        w_u8(((uint32)((a1 + 115))), 48);
        w_u8(((uint32)((a1 + 116))), 48);
        w_u8(((uint32)((a1 + 117))), 64);
    }
    v9 = r_u16(((uint32)((a1 + 10))));
    v2 = (v9 == 0);
    v10 = (v9 - 1);
    if ((v2 || (w_u16(((uint32)((a1 + 10))), v10) == 0)))
    {
        v11 = r_u8(((uint32)((a1 + 112))));
        v12 = r_u8(((uint32)((a1 + 118))));
        v13 = (v11 - v12);
        if ((v12 >= v11))
            v13 = 0;
        v14 = r_u8(((uint32)((a1 + 113))));
        v15 = r_u8(((uint32)((a1 + 118))));
        w_u8(((uint32)((a1 + 112))), v13);
        v16 = (v14 - v15);
        if ((v15 >= v14))
            v16 = 0;
        v17 = r_u8(((uint32)((a1 + 114))));
        v18 = r_u8(((uint32)((a1 + 118))));
        w_u8(((uint32)((a1 + 113))), v16);
        v19 = (v17 - v18);
        if ((v18 >= v17))
            v19 = 0;
        w_u8(((uint32)((a1 + 114))), v19);
    }
    result = (r_u8(((uint32)((a1 + 112)))) | r_u8(((uint32)((a1 + 113)))));
    if (!((r_u8(((uint32)((a1 + 114)))) | result)))
        return sub_80032ED8(a1);
    return result;
}

uint32 sub_80036F98(uint32 a1, uint32 a2)
{
    uint32 result;
    sint32 v5;
    w_u32((a1 + (17) * 4u), 0x800A1B98u);
    sub_80032E7C(a1, 0x800FF454u);
    result = sub_80032FB8(((sint32)(a1)), 0);
    if (((a2 & 1) != 0))
        return ((uint32)(sub_80032E30(((sint32)(a1)))));
    return result;
}

uint32 sub_8006FC00(uint32 a1, uint32 a2)
{
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 result;
    v2 = 0;
    v3 = r_u32(0x800FF7DCu);
    v4 = (r_u32(0x800FF7DCu) == ((uint32)(a1)));
    w_u32(a1, 0x800A3ACCu);
    if (!v4)
    {
        do
        {
            v2 = v3;
            v3 = r_u32(((uint32)((v3 + 20))));
        } while ((((uint32)(v3)) != a1));
    }
    if (v2)
        w_u32(((uint32)((v2 + 20))), r_u32((a1 + (5) * 4u)));
    else
        w_u32(0x800FF7DCu, r_u32((a1 + (5) * 4u)));
    w_u32(a1, 0x800A3ADCu);
    result = (a2 & 1);
    if (((a2 & 1) != 0))
        return sub_8002FF34((sint32)a1);
    return result;
}

uint32 sub_8005E90C(uint32 a1)
{
    sint32 v2;
    sint32 result;
    sint32 v4;
    v2 = r_u32(0x800FF904u);
    if (r_u32(0x800FF904u))
    {
        result = r_u32(((uint32)((a1 + 448))));
        if (result)
        {
            w_u16(((uint32)((a1 + 652))), 0);
            sub_800785D8(v2, 64, 32);
            if ((r_u32(0x800FF378u) == 10))
                v4 = -1400;
            else
                v4 = -700;
            sub_80078618(r_u32(0x800FF904u), v4, 32);
            sub_8007865C(r_u32(0x800FF904u), 0, 32);
            sub_800786A0(r_u32(0x800FF904u), 0, 32);
            sub_800786E4(r_u32(0x800FF904u), 0, 32);
            return sub_8007851C(r_u32(0x800FF904u), ((r_u16(((uint32)((r_u32(0x800FF904u) + 494)))) + 1024) & 0xFFF), 34);
        }
    }
    return result;
}

uint32 sub_8001BEF8(uint32 a1, uint32 a2)
{
    sint32 result;
    w_u32(0x800FF1E8u, 1);
    result = 255;
    w_u32(0x800FF1ECu, a1);
    w_u8(0x800FF1DCu, -1);
    w_u32(0x800FFB68u, 0);
    w_u32(0x800FFB64u, 0);
    w_u32(0x800FFB60u, 0);
    w_u32(0x800FF1E0u, a2);
    w_u32(0x800FFB6Cu, (16711680 / a1));
    return result;
}

/* TODO Postincrement memory expressions may require ordering refinement */
uint32 sub_8007741C(uint32 a1, uint32 a2)
{
    sint32 result;
    sint32 v5;
    w_u32(((uint32)((a1 + 68))), 0x800A3DA4u);
    sub_80062A64(a1, 0x800FF904u);
    (w_u32(0x800FF900u, (r_u32(0x800FF900u) - 1u)), (r_u32(0x800FF900u) - 1u));
    result = sub_800629BC(a1, 0);
    if (((a2 & 1) != 0))
        return sub_80062608(a1);
    return result;
}

uint32 sub_8001664C(void)
{
    uint32 result;
    sub_8001A7BC(512u);
    sub_8001A7D4(149u, 20u, 20u, 0u);
    result = sub_8001AA28(256u, 34u, r_u32(0x800A5548u), 0u, 0u, 256u);
    sub_8001A7BC(256u);
    return result;
}

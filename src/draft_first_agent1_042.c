#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>

/* Unverified draft; TODO Recover omitted call arguments, host-buffer adapters and signed field widths */
/* TODO Missing call adapter abs32 */
/* TODO Missing call adapter indirect */
/* TODO Missing call adapter nullsub_20 */
/* TODO Missing call adapter sub_80069FAC */
/* TODO Postincrement memory expressions may require ordering refinement */
static uint32 collision_angle_divide(uint32 numerator, uint32 denominator);
static uint32 behavior_signed_remainder(uint32 numerator, uint32 denominator);

/* TODO Additional player classes require a reviewed slot-52 extent */
static void moving_platform_player_damage(uint32 player)
{
    uint32 table = r_u32(player + 68u);
    if (table != 0x800A32A4u)
        abort();
    apocalypse_object_virtual52(r_u32(table + 52u), player + (sint16)r_u16(table + 48u), 2u, 0x800A71CCu, 0u);
}

uint32 sub_8003BBD8(uint32 a1)
{
    sint32 v2;
    sint32 v3;
    short v4;
    short v5;
    short v6;
    sint32 v7;
    short v8;
    short v9;
    short v10;
    sint32 v11;
    short v12;
    short v13;
    short v14;
    sint32 v15;
    sint32 v16;
    short v17;
    sint8 v18;
    sint32 v19;
    short v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    unsigned short v27;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    sint32 v31;
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
    unsigned short v42;
    sint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    short v50;
    sint32 v51;
    uint32 v52;
    sint32 v53;
    sint32 v54;
    sint32 v55;
    sint32 v56;
    sint32 result;
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
    sint32 direction[3];
    uint32 angles[2];

    union
    {
        uint32 words[4];
        sint16 halves[8];
    } temporary;

    uint32 v79[4];
    v2 = r_u16(((uint32)((a1 + 496))));
    v3 = (v2 == 0);
    v4 = (v2 - 1);
    if (!v3)
    {
        v5 = r_u16(((uint32)((a1 + 36))));
        v6 = r_u16(((uint32)((a1 + 502))));
        w_u16(((uint32)((a1 + 496))), v4);
        w_u16(((uint32)((a1 + 36))), (v5 + v6));
    }
    v7 = r_u16(((uint32)((a1 + 498))));
    v3 = (v7 == 0);
    v8 = (v7 - 1);
    if (!v3)
    {
        v9 = r_u16(((uint32)((a1 + 38))));
        v10 = r_u16(((uint32)((a1 + 504))));
        w_u16(((uint32)((a1 + 498))), v8);
        w_u16(((uint32)((a1 + 38))), (v9 + v10));
    }
    v11 = r_u16(((uint32)((a1 + 500))));
    v3 = (v11 == 0);
    v12 = (v11 - 1);
    if (!v3)
    {
        v13 = r_u16(((uint32)((a1 + 40))));
        v14 = r_u16(((uint32)((a1 + 506))));
        w_u16(((uint32)((a1 + 500))), v12);
        w_u16(((uint32)((a1 + 40))), (v13 + v14));
    }
    v15 = r_u32(0x800A71D0u);
    v16 = r_u32(0x800A71D4u);
    w_u32(a1 + 116u, r_u32(0x800A71CCu));
    w_u32(((uint32)((a1 + 120))), v15);
    w_u32(((uint32)((a1 + 124))), v16);
    if (!(r_u32(((uint32)((a1 + 512))))))
        (w_u32(((uint32)((a1 + 516))), (r_u32(((uint32)((a1 + 516)))) + 1u)), (r_u32(((uint32)((a1 + 516)))) + 1u));
    v17 = r_u16(((uint32)((a1 + 76))));
    w_u16(((uint32)((a1 + 388))), v17);
    if (((v17 & 1) != 0))
    {
        v18 = r_u8(((uint32)((a1 + 384))));
        w_u16(((uint32)((a1 + 76))), (v17 & 0xFFFE));
        w_u8(((uint32)((a1 + 384))), (v18 + 1));
    }
    if (r_u8(((uint32)((a1 + 380)))))
    {
        v19 = r_u16(((uint32)((a1 + 476))));
        v3 = (v19 == 0);
        v20 = (v19 - 1);
        if (v3)
            sub_8004BE90(a1, r_u32(((uint32)((a1 + 400)))));
        else
            w_u16(((uint32)((a1 + 476))), v20);
    }
    else
    {
        /* Original nullsub_20 has no side effects */
    }
    if (((r_u32(((uint32)((a1 + 396)))) & 1) == 0))
    {
        if ((r_u16(((uint32)((a1 + 440)))) != 1))
            goto LABEL_64;
        v44 = -(r_u16(((uint32)((a1 + 20)))));
        if ((sint16)(0u - r_u16(a1 + 20u)) < -2048)
            v44 = (4096 - r_u16(((uint32)((a1 + 20)))));
        v45 = (sint32)((uint32)v44 << 16);
        if ((((short)(v44)) >= 2049))
            v45 = (sint32)(((uint32)v44 - 4096u) << 16);
        v46 = (v45 >> 16);
        v3 = (v46 != 0);
        v47 = (v46 >> 3);
        if (v3)
        {
            w_u16(((uint32)((a1 + 142))), v47);
            goto LABEL_64;
        }
    LABEL_62:
        w_u16(((uint32)((a1 + 136))), 0);

        goto LABEL_64;
    }
    xport_draft_host_sub_8006C3AC_p1(direction, a1 + 4u, a1 + 484u);
    if (((sint32)r_u16(((uint32)((a1 + 436)))) < (sint32)xport_draft_host_sub_8006BF04_p1(direction)))
    {
        v23 = r_u16(((uint32)((a1 + 440))));
        if (!(r_u16(((uint32)((a1 + 440))))))
        {
            xport_draft_host_sub_80066B8C_p1(angles, a1 + 4u, a1 + 484u);
            xport_draft_host_sub_800667CC_p3(a1 + 104u, r_u16(a1 + 436u), angles);
            goto LABEL_64;
        }
        if ((v23 == 1))
        {
            xport_draft_host_sub_80066B8C_p1(angles, a1 + 4u, a1 + 484u);
            xport_draft_host_sub_8006C3AC_p1(temporary.words, a1 + 484u, a1 + 4u);
            v24 = (sint32)collision_angle_divide(8160u, xport_draft_host_sub_8006BF04_p1(temporary.words));
            if ((((v24 >= -256) && (v24 < 257)) && (((uint32)((v24 - 1))) >= 3)))
            {
                v25 = (a1 + 132);
                if ((v24 >= 0))
                {
                LABEL_30:
                    v26 = (a1 + 138);

                LABEL_31:
                    v27 = r_u16(((uint32)((a1 + 20))));

                    temporary.words[0] = r_u32(((uint32)((a1 + 16))));
                    temporary.halves[2] = v27;
                    sub_80066CF0((uint32)temporary.words[0], v27, v25, v26, angles[0], angles[1] & 65535u, (uint32)(v24 < -256 ? -256 : v24 > 256 ? 256 : (uint32)(v24 - 1) < 3u ? 4 : v24 < 0 && v24 >= -3 ? -4 : v24));
                    v28 = r_u16(((uint32)((a1 + 436))));
                    w_u16(((uint32)((a1 + 16))), 0);
                    xport_draft_host_sub_800667CC_p3(a1 + 116u, v28, angles);
                    if (((r_u32(((uint32)((a1 + 396)))) & 0x20) != 0))
                        v29 = ((((angles[0] >> 16) & 65535u) - r_u16(((uint32)((a1 + 18))))) & 0xFFF);
                    else
                        v29 = 0;
                    v30 = (sint32)((uint32)v29 << 16);
                    if ((((short)(v29)) >= 2049))
                    {
                        v29 -= 4096;
                        v30 = (sint32)((uint32)v29 << 16);
                    }
                    v3 = ((v30 >> 16) < 1025);
                    v31 = ((v30 >> 16) < -1024);
                    if (v3)
                    {
                        v3 = !v31;
                        v33 = (sint32)((uint32)v29 << 16);
                        if (v3)
                            goto LABEL_41;
                        v32 = -1024;
                    }
                    else
                    {
                        v32 = 1024;
                    }
                    v33 = (sint32)((uint32)v32 << 16);
                LABEL_41:
                    v34 = (((-((v33 >> 16)) >> 1) & 0xFFF) - r_u16(((uint32)((a1 + 20)))));

                    v35 = v34;
                    if ((((short)(v34)) < -2048))
                        v35 = (v34 + 4096);
                    v36 = (sint32)((uint32)v35 << 16);
                    if ((((short)(v35)) >= 2049))
                        v36 = (sint32)(((uint32)v35 - 4096u) << 16);
                    v37 = (v36 >> 16);
                    v3 = (v37 == 0);
                    v38 = (v37 >> 3);
                    if (!v3)
                    {
                        w_u16(((uint32)((a1 + 142))), v38);
                        goto LABEL_64;
                    }
                    goto LABEL_62;
                }
                v26 = (a1 + 138);
                if ((v24 < -3))
                    goto LABEL_31;
            }
            v25 = (a1 + 132);
            goto LABEL_30;
        }
        if ((v23 != 2))
            goto LABEL_64;
        xport_draft_host_sub_80066B8C_p1(angles, a1 + 4u, a1 + 484u);
        xport_draft_host_sub_8006C3AC_p1(v79, a1 + 484u, a1 + 4u);
        v39 = (sint32)collision_angle_divide(8160u, xport_draft_host_sub_8006BF04_p1(v79));
        if ((((v39 >= -256) && (v39 < 257)) && (((uint32)((v39 - 1))) >= 3)))
        {
            v40 = (a1 + 132);
            if ((v39 >= 0))
            {
            LABEL_54:
                v41 = (a1 + 138);

                goto LABEL_55;
            }
            v41 = (a1 + 138);
            if ((v39 < -3))
            {
            LABEL_55:
                v42 = r_u16(((uint32)((a1 + 20))));

                temporary.words[0] = r_u32(((uint32)((a1 + 16))));
                temporary.halves[2] = v42;
                sub_80066CF0((uint32)temporary.words[0], v42, v40, v41, angles[0], angles[1] & 65535u, (uint32)(v39 < -256 ? -256 : v39 > 256 ? 256 : (uint32)(v39 - 1) < 3u ? 4 : v39 < 0 && v39 >= -3 ? -4 : v39));
                v43 = r_u16(((uint32)((a1 + 436))));
                w_u16(((uint32)((a1 + 16))), 0);
                xport_draft_host_sub_800667CC_p3(a1 + 116u, v43, angles);
                goto LABEL_64;
            }
        }
        v40 = (a1 + 132);
        goto LABEL_54;
    }
    v21 = r_u32(((uint32)((a1 + 488))));
    v22 = r_u32(((uint32)((a1 + 492))));
    w_u32(((uint32)((a1 + 4))), r_u32(((uint32)((a1 + 484)))));
    w_u32(((uint32)((a1 + 8))), v21);
    w_u32(((uint32)((a1 + 12))), v22);
    if ((r_u16(((uint32)((a1 + 440)))) < 2u))
    {
        w_u32(((uint32)((a1 + 112))), 0);
        w_u32(((uint32)((a1 + 108))), 0);
        w_u32(((uint32)((a1 + 104))), 0);
    }
    w_u32(((uint32)((a1 + 396))), (r_u32(((uint32)((a1 + 396)))) & (~1u)));
LABEL_64:
    if (r_u8(((uint32)((a1 + 382)))))
        sub_8006C0B8((a1 + 116), 0x800A6844u);

    sub_8003C558(a1);
    if (((r_u16(((uint32)((a1 + 440)))) == 2) && r_u32(0x800FF5A0u)))
    {
        if (((sint32)sub_80066918((a1 + 4), (r_u32(0x800FF5A0u) + 4)) >= r_u16(((uint32)((a1 + 442))))))
        {
            v55 = r_u32(((uint32)((a1 + 520))));
            w_u32(((uint32)((a1 + 396))), (r_u32(((uint32)((a1 + 396)))) & (~8u)));
            if (v55)
            {
                sub_8006A294(v55);
                w_u32(((uint32)((a1 + 520))), 0);
            }
        }
        else
        {
            if (((r_u32(((uint32)((a1 + 396)))) & 8) == 0))
                sub_80064A08((r_u32(((uint32)(((4 * r_u16(((uint32)((a1 + 214))))) + r_u32(0x800FF624u))))) + 6));
            v48 = r_u32(((uint32)((a1 + 520))));
            w_u32(((uint32)((a1 + 396))), (r_u32(((uint32)((a1 + 396)))) | (8u)));
            if (!v48)
                w_u32(((uint32)((a1 + 520))), sub_80069DF0(11, 0x2000, 0));
            v49 = 0;
            moving_platform_player_damage(r_u32(0x800FF5A0u));
            while ((v49 < sub_80066570(4)))
            {
                v50 = sub_80066570(4096);
                v51 = sub_80066570((r_u16(((uint32)((a1 + 442)))) >> 1));
                v52 = (0x800F863Cu + ((v50 & 0xFFF)) * 4u);
                v53 = (sint32)((uint32)(sint32)(sint16)r_u16(v52 + 2u) * (uint32)v51);
                direction[0] = (sint32)((uint32)(sint32)(sint16)r_u16(v52) * (uint32)v51);
                direction[1] = 0;
                direction[2] = v53;
                xport_draft_host_sub_8006C0B8_p1(direction, r_u32(0x800FF5A0u) + 4u);
                direction[1] = r_u32(((uint32)((a1 + 8))));
                v54 = sub_80032DC0(120);
                if (v54)
                    v54 = xport_draft_host_sub_80035478_p2(v54, &direction[0], 0, 512, 1, 1, 0xFFFFFFFFu);
                w_u16(((uint32)((v54 + 64))), 64);
                ++v49;
            }
        }
    }
    v56 = r_u32(((uint32)((a1 + 524))));
    if (((v56 != -1) && !behavior_signed_remainder(r_u32(0x800FF2F0u), (uint32)v56)))
        sub_80069FAC(r_u32(a1 + 520u), a1 + 4u, 0u);
    result = r_u32(a1 + 396u) & 4u;
    w_u32(a1 + 512u, 0u);
    if (result)
    {
        uint32 player = r_u32(0x800FF5A0u);
        result = player;
        if (player)
        {
            uint32 x = r_u32(a1 + 4u), y = r_u32(a1 + 8u), z = r_u32(a1 + 12u);
            uint32 rx = r_u32(a1 + 528u), ry = r_u32(a1 + 532u), rz = r_u32(a1 + 536u);
            uint32 px = r_u32(player + 4u), py = r_u32(player + 8u), pz = r_u32(player + 12u);
            result = (sint32)px < (sint32)(x + rx);
            if (!result)
                return result;
            result = (sint32)(x - rx) < (sint32)px;
            if (!result)
                return result;
            result = (sint32)pz < (sint32)(z + rz);
            if (!result)
                return result;
            result = (sint32)(z - rz) < (sint32)pz;
            if (!result)
                return result;
            result = (sint32)py < (sint32)(y + ry);
            if (!result)
                return result;
            result = (sint32)(y - ry) < (sint32)py;
            if (result)
            {
                uint32 vx = r_u32(a1 + 104u), vz = r_u32(a1 + 112u);
                uint32 ax = (sint32)vx < 0 ? 0u - vx : vx;
                uint32 az = (sint32)vz < 0 ? 0u - vz : vz;
                result = az;
                if ((sint32)az < (sint32)ax)
                {
                    result = (sint32)vx > 0 ? x + rx + 0x40000u : x - rx - 0x40000u;
                    w_u32(r_u32(0x800FF5A0u) + 4u, result);
                }
                else if ((sint32)az > 0)
                {
                    result = (sint32)vz > 0 ? z + rz + 0x40000u : z - rz - 0x40000u;
                    w_u32(r_u32(0x800FF5A0u) + 12u, result);
                }
            }
        }
    }
    return result;
}

uint32 sub_8003C558(uint32 a1)
{
    short v2;
    sint32 v3;
    sint32 v4;
    unsigned short v5;
    sint32 v6;
    sint32 result;
    v2 = r_u16(0x800FF5E8u);
    w_u32(((uint32)((a1 + 164))), 0x800FF5E4u);
    w_u16(((uint32)((a1 + 168))), v2);
    v3 = r_u16(((uint32)((a1 + 440))));
    v4 = (v3 != 0);
    v5 = (v3 - 1);
    if (v4)
    {
        result = (v5 < 2u);
        if (!result)
            return result;
        sub_8006C0B8((a1 + 104), (a1 + 116));
        sub_8006C270((a1 + 104), (a1 + 129));
        sub_8006C05C((a1 + 104));
        sub_8006C0B8((a1 + 4), (a1 + 104));
        v6 = (a1 + 132);
        sub_8006C730((a1 + 16), (a1 + 132));
        sub_8006C624((a1 + 16));
        sub_8006C730((a1 + 132), (a1 + 138));
        sub_8006C8E8((a1 + 132), (a1 + 144));
    }
    else
    {
        sub_8006C0B8((a1 + 104), (a1 + 116));
        sub_8006C05C((a1 + 104));
        sub_8006C0B8((a1 + 4), (a1 + 104));
        v6 = (a1 + 132);
        sub_8006C730((a1 + 16), (a1 + 132));
        sub_8006C624((a1 + 16));
        sub_8006C730((a1 + 132), (a1 + 138));
    }
    return sub_8006C64C(v6);
}

/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_8003A8B0 */
/* TODO Missing call adapter sub_8005C11C */
/* TODO Missing call adapter sub_80069FAC */
/* TODO Existing excluded callees remain named fail-fast adapters */
uint32 sub_8005C11C(uint32 object, uint32 position);
uint32 sub_80069FAC(uint32 sound, uint32 position, uint32 mode);
uint32 sub_8003A8B0(uint32 object);

static uint32 behavior_signed_remainder(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return numerator;
    return (uint32)((long long)(sint32)numerator % (long long)(sint32)denominator);
}

uint32 sub_8004D800(uint32 a1)
{
    short v2;
    sint8 v3;
    sint32 v4;
    sint32 v5;
    short v6;
    sint32 i;
    sint32 v8;
    sint32 v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 result;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    uint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    sint32 v22;
    uint32 v23;
    sint32 v24;
    sint32 v25;
    uint32 v26;
    sint32 v27;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    sint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    int v35[4];
    uint32 v36[4];
    uint32 v37[4];
    sint32 v38;
    uint32 local_position[3];
    uint32 local_offset[3];
    v2 = r_u16(((uint32)((a1 + 76))));
    w_u16(((uint32)((a1 + 388))), v2);
    if (((v2 & 1) != 0))
    {
        v3 = r_u8(((uint32)((a1 + 384))));
        w_u16(((uint32)((a1 + 76))), (v2 & 0xFFFE));
        w_u8(((uint32)((a1 + 384))), (v3 + 1));
    }
    if (r_u8(((uint32)((a1 + 380)))))
    {
        v4 = r_u16(((uint32)((a1 + 476))));
        v5 = (v4 == 0);
        v6 = (v4 - 1);
        if (v5)
            sub_8004BE90(a1, r_u32(((uint32)((a1 + 400)))));
        else
            w_u16(((uint32)((a1 + 476))), v6);
    }
    else
    {
        uint32 table = r_u32(a1 + 68u);
        apocalypse_object_virtual20(r_u32(table + 20u), a1 + (sint16)r_u16(table + 16u));
    }
    if ((((r_u32(((uint32)((a1 + 396)))) & 0x100) != 0) && !sub_80066570(4)))
    {
        w_u32(0x800FF3ACu, 0);
        for (i = (sub_80066570(3) + 2); i; --i)
        {
            v8 = r_u32(((uint32)((a1 + 8))));
            v9 = r_u32(((uint32)((a1 + 12))));
            v29 = r_u32(((uint32)((a1 + 4))));
            v30 = v8;
            v31 = v9;
            v29 += ((sub_80066570(128) - 64u) << 12);
            v30 += ((sub_80066570(128) - 64u) << 12);
            v31 += ((sub_80066570(128) - 64u) << 12);
            v10 = sub_80032DC0(116);
            if (v10)
                sub_8005C11C(v10, a1 + 4u);
        }

        w_u32(0x800FF3ACu, 1);
    }
    v11 = r_u32(((uint32)((a1 + 516))));
    if (((v11 != -1) && !behavior_signed_remainder(r_u32(0x800FF2F0u), (uint32)v11)))
        sub_80069FAC(r_u32(a1 + 512u), a1 + 4u, 0u);
    v12 = r_u32(((uint32)((a1 + 496))));
    if (v12)
        sub_8003A8B0(v12);
    result = r_u32(((uint32)((a1 + 504))));
    v14 = 0;
    if (result)
    {
        v15 = r_u32(((uint32)((r_u32(0x800FF5A0u) + 188))));
        while (1)
        {
            result = (v14 < r_u16(((uint32)((a1 + 500)))));
            if ((v14 >= r_u16(((uint32)((a1 + 500))))))
                break;
            v16 = (r_u32(((uint32)((a1 + 504)))) + (16 * v14));
            v17 = r_u32(((uint32)((v16 + 12))));
            v18 = r_u32((v17 + (21) * 4u));
            v19 = r_u32((v17 + (22) * 4u));
            v29 = r_u32((v17 + (20) * 4u));
            v30 = v18;
            v31 = v19;
            local_position[0] = v29;
            local_position[1] = v30;
            local_position[2] = v31;
            xport_draft_host_sub_8006C0B8_p1(local_position, v16);
            v29 = local_position[0];
            v30 = local_position[1];
            v31 = local_position[2];
            if ((v15 < v30))
            {
                v20 = ((sub_80066570(2400) - 1200u) << 12);
                v22 = ((sub_80066570(400) - 1200u) << 12);
                v21 = sub_80066570(2400);
                v32 = v20;
                v33 = v22;
                v34 = (((uint32)v21 - 1200u) << 12);
                local_offset[0] = v32;
                local_offset[1] = v33;
                local_offset[2] = v34;
                xport_draft_host_sub_8006C34C_p13(v35, r_u32(0x800FF5A0u) + 4u, local_offset);
                v29 = v35[0];
                v30 = v35[1];
                v31 = v35[2];
            }
            v23 = r_u32(((uint32)((v16 + 12))));
            v24 = v30;
            v25 = v31;
            w_u32((v23 + (20) * 4u), v29);
            w_u32((v23 + (21) * 4u), v24);
            w_u32((v23 + (22) * 4u), v25);
            v26 = r_u32(((uint32)((r_u32(((uint32)((v16 + 12)))) + 76))));
            ++v14;
            local_position[0] = v29;
            local_position[1] = v30;
            local_position[2] = v31;
            xport_draft_host_sub_8006C3AC_p123(v36, local_position, psx_addr(v16, 12));
            v38 = 1;
            xport_draft_host_sub_8006C564_p13(v37, r_u32(0x800FF904u) + 104u, &v38);
            xport_draft_host_sub_8006C34C_p123(local_offset, v36, v37);
            v32 = local_offset[0];
            v33 = local_offset[1];
            v34 = local_offset[2];
            v27 = v33;
            v28 = v34;
            w_u32(v26, v32);
            w_u32((v26 + (1) * 4u), v27);
            w_u32((v26 + (2) * 4u), v28);
        }
    }
    return result;
}

/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_80010D3C */
/* TODO Missing call adapter sub_800322F4 */
/* TODO Missing call adapter sub_8003243C */
/* TODO Missing call adapter sub_8003A154 */
/* TODO Missing call adapter sub_8003A214 */
/* TODO Missing call adapter sub_8003A228 */
/* TODO Missing call adapter sub_8005ABF8 */
/* TODO Existing excluded constructors remain named fail-fast dependencies */
uint32 sub_800322F4(uint32 object, uint32 enabled);
uint32 sub_8003243C(uint32 object);
uint32 sub_8003A154(uint32 object, uint32 mode, uint32 owner, uint32 target, uint32 flags);
uint32 sub_8003A214(uint32 object, uint32 position);
uint32 sub_8003A228(uint32 object, uint32 rotation);
uint32 sub_8005ABF8(uint32 object, uint32 position, uint32 kind, uint32 mode, uint32 auxiliary, sint32 angle);
uint32 sub_80010D3C(uint32 object, uint32 position, uint32 rotation, uint32 kind, uint32 mask, uint32 p5, uint32 p6, uint32 p7, uint32 p8, uint32 p9, uint32 identity);

uint32 sub_8004DC40(uint32 object, uint32 incoming_event)
{
    uint32 event = (uint16)incoming_event;
    uint32 pointer, allocated, value, sound, sound_id;
    if (event == 0x4503u)
    {
        uint32 count = (uint16)sub_8004CF78(object), index = 0u;
        uint32 origin[3], position[3], difference[4], scaled[4], output[4];
        uint32 player;
        sint32 shift;
        w_u16(object + 500u, count);
        allocated = sub_8006B864(count * 16u, 0, 1);
        player = r_u32(0x800FF5A0u);
        w_u32(object + 504u, allocated);
        w_u32(0x800FF3ACu, 0u);
        origin[0] = r_u32(player + 4u);
        origin[1] = r_u32(player + 8u);
        origin[2] = r_u32(player + 12u);
        while (index < r_u16(object + 500u))
        {
            uint32 entry = r_u32(object + 504u) + 16u * index;
            uint32 x = (sub_80066570(2400u) - 1200u) << 12;
            uint32 y = (sub_80066570(400u) - 1200u) << 12;
            uint32 z = (sub_80066570(2400u) - 1200u) << 12;
            position[0] = x;
            position[1] = y;
            position[2] = z;
            xport_draft_host_sub_8006C0B8_p12(position, origin);
            x = (sub_80066570(16u) - 8u) << 12;
            y = (sub_80066570(48u) + 48u) << 12;
            z = (sub_80066570(16u) - 8u) << 12;
            w_u32(entry, x);
            w_u32(entry + 4u, y);
            w_u32(entry + 8u, z);
            allocated = sub_80032DC0(100u);
            if (allocated)
                allocated = sub_800322F4(allocated, 1u);
            w_u32(entry + 12u, allocated);
            w_u8(allocated + 66u, 1u);
            w_u32(allocated + 80u, position[0]);
            w_u32(allocated + 84u, position[1]);
            w_u32(allocated + 88u, position[2]);
            pointer = r_u32(allocated + 76u);
            xport_draft_host_sub_8006C3AC_p123(difference, position, psx_addr(entry, 12));
            ++index;
            shift = 1;
            xport_draft_host_sub_8006C564_p13(scaled, r_u32(0x800FF904u) + 104u, &shift);
            xport_draft_host_sub_8006C34C_p123(output, difference, scaled);
            w_u32(pointer, output[0]);
            w_u32(pointer + 4u, output[1]);
            w_u32(pointer + 8u, output[2]);
            w_u8(allocated + 92u, 64u);
            w_u8(allocated + 93u, 64u);
            w_u8(allocated + 94u, 80u);
            w_u8(r_u32(allocated + 76u) + 12u, 16u);
            w_u8(r_u32(allocated + 76u) + 13u, 16u);
            w_u8(r_u32(allocated + 76u) + 14u, 32u);
            sub_8003243C(allocated);
        }
        w_u32(0x800FF3ACu, 1u);
        return 1u;
    }
    if (event == 0x4507u || event == 0x4508u || event == 0x4509u)
    {
        sound = r_u32(object + 512u);
        if (sound)
            sub_8006A294(sound);
        if (event == 0x4509u)
            w_u32(object + 512u, 0u);
        else
        {
            pointer = r_u32(object + 400u);
            value = r_u16(pointer);
            sound_id = value;
            w_u32(object + 400u, pointer + 2u);
            if (event == 0x4507u)
            {
                w_u32(object + 512u, sub_80069DF0(value, 0x2000u, 0u));
                w_u32(object + 516u, 0xFFFFFFFFu);
            }
            else
            {
                pointer = r_u32(object + 400u);
                value = r_u16(pointer);
                w_u32(object + 400u, pointer + 2u);
                w_u32(object + 516u, value);
                w_u32(object + 512u, sub_80069EF4(sound_id, object + 4u, 0u));
            }
        }
        return 1u;
    }
    if (event == 0x4700u)
    {
        pointer = r_u32(object + 400u);
        value = r_u16(pointer);
        w_u32(object + 400u, pointer + 2u);
        w_u32(object + 396u, value ? r_u32(object + 396u) | 0x100u : r_u32(object + 396u) & ~0x100u);
        return 1u;
    }
    if (event == 0x4506u)
    {
        uint32 player = r_u32(0x800FF5A0u);
        if (player)
        {
            uint32 x = r_u32(player + 4u), y = r_u32(player + 8u), z = r_u32(player + 12u);
            w_u32(object + 4u, x);
            w_u32(object + 8u, y);
            w_u32(object + 12u, z);
        }
        return 1u;
    }
    if (event == 0x4504u)
    {
        uint32 p0, p1, p2, p3, player;
        pointer = r_u32(object + 400u);
        player = r_u32(0x800FF5A0u);
        p0 = r_u16(pointer);
        w_u32(object + 400u, pointer + 2u);
        p1 = r_u16(pointer + 2u);
        w_u32(object + 400u, pointer + 4u);
        p2 = r_u16(pointer + 4u);
        w_u32(object + 400u, pointer + 6u);
        p3 = r_u16(pointer + 6u);
        w_u32(object + 400u, pointer + 8u);
        if (player && sub_80066918(object + 4u, player + 4u) < 4096u)
        {
            allocated = sub_800625AC(504u);
            if (allocated)
                sub_8005ABF8(allocated, object + 4u, p0, p1, p2, (sint16)p3);
        }
        return 1u;
    }
    if (event == 0x429Bu || event == 0x42A1u)
    {
        allocated = sub_80032DC0(364u);
        if (allocated)
        {
            pointer = r_u32(object + 400u);
            allocated = sub_80010D3C(allocated, object + 4u, object + 16u, r_u16(pointer), 0xFFFFu, r_u16(pointer + 2u), r_u16(pointer + 4u), r_u16(pointer + 6u), r_u16(pointer + 8u), r_u8(pointer + 10u), event == 0x429Bu ? 0xCCE0187Cu : 0x68B7B136u);
        }
        w_u32(object + 508u, allocated);
        if (event == 0x429Bu)
            w_u8(allocated + 66u, 1u);
        w_u32(object + 400u, r_u32(object + 400u) + 12u);
        return 1u;
    }
    if (event == 0x429Fu)
    {
        allocated = r_u32(object + 508u);
        if (allocated)
        {
            uint32 table = r_u32(allocated + 68u);
            apocalypse_object_cleanup(r_u32(table + 12u), allocated + (sint16)r_u16(table + 8u), 3u);
        }
        w_u32(object + 508u, 0u);
        return 1u;
    }
    if (event == 0x4500u || event == 0x4501u || event == 0x4502u || event == 0x4505u)
    {
        allocated = sub_8002FED8(40u);
        if (allocated)
            allocated = sub_8003A154(allocated, event == 0x4505u ? 3u : event - 0x4500u, object, 0x800FF5A0u, 0u);
        w_u32(object + 496u, allocated);
        sub_8003A214(allocated, object + 4u);
        sub_8003A228(r_u32(object + 496u), object + 16u);
        return 1u;
    }
    return sub_8004BF3C(object, event);
}

uint32 sub_80033688(uint32 a1)
{
    sub_80032F7C(((sint32)(a1)));
    w_u32((a1 + (17) * 4u), 0x800A1D50u);
    w_u32((a1 + (36) * 4u), 746619008);
    sub_80032E50(a1, 0x800FF444u);
    return a1;
}

uint32 sub_80032E50(uint32 a1, uint32 a2)
{
    sint32 v2;
    uint32 result;
    v2 = r_u32(a2);
    w_u32(a1, 0);
    w_u32((a1 + (1) * 4u), v2);
    w_u32(a2, ((sint32)(a1)));
    result = ((uint32)(r_u32((a1 + (1) * 4u))));
    if (result)
        w_u32(result, a1);
    return result;
}

uint32 sub_80033764(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 result;
    result = r_u32(((uint32)((((8 * a3) + sub_8006F164(a2)) + 4))));
    w_u32(((uint32)((a1 + 148))), result);
    return result;
}

uint32 sub_80033874(uint32 a1)
{
    sint32 result;
    result = (r_u32(((uint32)((a1 + 144)))) | 0x2000000);
    w_u32(((uint32)((a1 + 144))), result);
    return result;
}

uint32 sub_8006C34C(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 result;
    sint32 v4;
    sint32 v5;
    result = a1;
    v4 = (r_u32((a2 + (1) * 4u)) + r_u32((a3 + (1) * 4u)));
    v5 = (r_u32((a2 + (2) * 4u)) + r_u32((a3 + (2) * 4u)));
    w_u32(a1, (r_u32(a2) + r_u32(a3)));
    w_u32((a1 + (1) * 4u), v4);
    w_u32((a1 + (2) * 4u), v5);
    return result;
}

/* TODO Missing call adapter indirect */
void sub_8006FD18(void)
{
    uint32 node = r_u32(0x800FF7DCu), next, object, table;
    while (node)
    {
        next = r_u32(node + 20u);
        if (sub_8006696C(r_u32(0x800FF5A0u) + 4u, node + 8u) < 0x2328u)
        {
            object = sub_800641E8(r_u16(node + 4u));
            if (object)
            {
                if ((sint32)(short)r_u16(node + 6u) >= 0)
                    w_u16(object + 218u, r_u16(node + 6u));
                w_u16(object + 78u, r_u16(object + 78u) | 4u);
                table = r_u32(node);
                (void)apocalypse_object_cleanup(r_u32(table + 12u), node + (uint32)(sint32)(short)r_u16(table + 8u), 3u);
            }
        }
        node = next;
    }
}

void sub_800638A4(void)
{
    sint32 v0;
    sint32 i;
    v0 = r_u32(0x800FF5E0u);
    if (r_u32(0x800FF5E0u))
    {
        for (i = r_u32(((uint32)((r_u32(0x800FF5E0u) + 28))));; i = r_u32(((uint32)((i + 28)))))
        {
            if ((((r_u16(((uint32)((v0 + 78)))) & 2) != 0) && (r_u32(0x800FF5ECu) >= ((uint32)(sub_8006696C((r_u32(0x800FF5A0u) + 4), (v0 + 4)))))))
                sub_80062BF0(v0);
            v0 = i;
            if (!i)
                break;
        }
    }
}

void sub_80032C88(void)
{
    sub_80032C18(r_u32(0x800FF434u));
    sub_80032C18(r_u32(0x800FF438u));
    sub_80032C18(r_u32(0x800FF43Cu));
    sub_80032C18(r_u32(0x800FF440u));
    sub_80032C18(r_u32(0x800FF45Cu));
    sub_80032C18(r_u32(0x800FF460u));
    sub_80032C18(r_u32(0x800FF444u));
    sub_80032C18(r_u32(0x800FF448u));
    sub_80032C18(r_u32(0x800FF44Cu));
    sub_80032C18(r_u32(0x800FF450u));
    sub_80032C18(r_u32(0x800FF454u));
    sub_80032C18(r_u32(0x800FF464u));
    {
        (void)(sub_80032C18(r_u32(0x800FF458u)));
        return;
    }
}

/* TODO Missing call adapter indirect */
/* TODO Missing call adapter sub_800103E4 */
/* TODO Missing call adapter sub_80010530 */
/* TODO Missing call adapter sub_80010A38 */
/* TODO Missing call adapter sub_80010CBC */
/* TODO Missing call adapter sub_800154E0 */
/* TODO Missing call adapter sub_80017760 */
/* TODO Missing call adapter sub_8001779C */
/* TODO Missing call adapter sub_8001BE6C */
/* TODO Missing call adapter sub_80076D6C */
/* TODO Postincrement memory expressions may require ordering refinement */
/* TODO Unrecorded card and pause helpers remain named fail-fast boundaries */
uint32 sub_800103E4(void);
uint32 sub_80010530(void);
uint32 sub_80010A38(uint32 port, uint32 slot);
uint32 sub_80010CBC(uint32 port, uint32 slot);
uint32 sub_800154E0(uint32 object, uint32 label);
uint32 sub_80017760(void);
uint32 sub_8001779C(uint32 object);
uint32 sub_8001BE6C(void);
uint32 sub_80076D6C(void);

static uint32 missing_80067724_native(const uint8 prefix[4], uint32 limit)
{
    fprintf(stderr, "Missing native adapter 80067724 prefix4 guest limit\n");
    abort();
    return 0u;
}

static void cleanup_17914_menu(uint32 object)
{
    uint32 table = r_u32(object);
    apocalypse_object_cleanup(r_u32(table + 12u), object + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
}

/* Pause-menu update has no native return value */
void sub_80017914(void)
{
    sint32 v0;
    sint32 v1;
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    uint32 v9;
    sint8 v10;
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
    uint32 v21;
    uint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    uint32 v27;
    uint32 v28;
    sint32 v29;
    sint32 v30;
    sint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    sint32 result;
    sint32 v36;
    sint32 v37;
    sint32 v38;
    uint8 prefix[4];

    sint32 v43 = 0;
    sint32 v44 = 0;
    sint32 v45 = 0;
    sint32 v46 = 0;
    v0 = -1;
    v1 = 0;
    if ((!r_u8(0x800EC138u) || (r_u32(0x800FF080u) != 1)))
    {
        xport_draft_host_sub_80017364_p1234(&v43, &v44, &v45, &v46);
        sub_80076D44();
    }
    v2 = 0;
    if (r_u32(0x800FF080u))
        sub_80010AC8(0, 0);
    if ((r_u8(0x800EC1E8u) && r_u8(0x800EC1D8u)))
    {
        if (!r_u32(0x800FF098u))
        {
            w_u32(0x800FF098u, 1);
            w_u32(0x800FF09Cu, r_u32(0x800FF64Cu));
        }
        if ((((uint32)((r_u32(0x800FF64Cu) - r_u32(0x800FF09Cu)))) >= 0x79))
        {
            w_u32(0x800FF098u, 0);
            w_u32(0x800FF2ECu, 7);
            v1 = 1;
        }
    }
    else
    {
        w_u32(0x800FF098u, 0);
    }
    switch (r_u32(0x800FF080u))
    {
        case 0:
            if ((v46 || !r_u32(0x800EC264u)))
            {
                sub_8001B708();
                sub_80070100(0x800EC0F8u);
                w_u32(0x800FF00Cu, 0);
                sub_8006A114();
                sub_80069DF0(23, 0x2000, 0);
                sub_80070288(0, 0);
                sub_80070288(0, 1);
                w_u32(0x800FF008u, 1);
                v3 = sub_8002FED8(444);
                if (v3)
                    v3 = sub_80015228(v3, 256u, 0u, 0u, 320u, 256u, 26u);
                w_u32(0x800FF084u, v3);
                sub_80015474(v3, ((sint32)(r_u32((0x800A554Cu + (0) * 4u)))));
                sub_80015474(r_u32(0x800FF084u), ((sint32)(r_u32((0x800A5580u + (0) * 4u)))));
                if (!r_u32(0x800FF338u))
                    sub_8001551C(r_u32(0x800FF084u), ((sint32)(r_u32((0x800A5580u + (0) * 4u)))));
                sub_80015474(r_u32(0x800FF084u), ((sint32)(r_u32((0x800A555Cu + (0) * 4u)))));
                sub_800152F8(r_u32(0x800FF084u));
                w_u32(((uint32)((r_u32(0x800FF084u) + 16))), (r_u32(((uint32)((r_u32(0x800FF084u) + 16)))) + (5)));
                if (!r_u32(0x800EC264u))
                    goto LABEL_62;
                w_u32(0x800FF080u, 1);
            }
            goto LABEL_190;

        case 1:
            sub_8006CDC4();
            if (!r_u32(0x800FF008u))
                v1 = 1;
            if (!r_u32(0x800EC264u))
                w_u32(0x800FF080u, 4);
            if (r_u8(0x800EC138u))
            {
                if (sub_80076D6C())
                {
                    w_u32(0x800FF00Cu, 32);
                    w_u32(0x800FF010u, 0);
                }
                sub_80070100(0x800EC0F8u);
                goto LABEL_190;
            }
            if (r_u32(0x800FF338u))
                sub_800154E0(r_u32(0x800FF084u), r_u32(0x800A5580u));
            else
                sub_8001551C(r_u32(0x800FF084u), ((sint32)(r_u32((0x800A5580u + (0) * 4u)))));
            sub_80015BA4(r_u32(0x800FF084u));
            v4 = -1;
            if (!v43)
                goto LABEL_43;
            if (v46)
            {
                v5 = 19;
                v1 = 1;
            }
            else
            {
                if (sub_800155D4(r_u32(0x800FF084u), ((sint32)(r_u32((0x800A554Cu + (0) * 4u))))))
                {
                    v0 = 19;
                    v1 = 1;
                }
                if (sub_800155D4(r_u32(0x800FF084u), ((sint32)(r_u32((0x800A5580u + (0) * 4u))))))
                {
                    v0 = 23;
                    w_u32(0x800FF080u, 2);
                }
                v6 = sub_800155D4(r_u32(0x800FF084u), ((sint32)(r_u32((0x800A555Cu + (0) * 4u)))));
                v4 = v0;
                if (!v6)
                    goto LABEL_43;
                v5 = 23;
                sub_800153D8(r_u32(0x800FF024u));
                w_u32(0x800FF080u, 3);
            }
            v4 = v5;
        LABEL_43:
            sub_80069DF0(v4, 0x2000, 0);

            goto LABEL_190;

        case 2:
            if (!r_u32(0x800FF088u))
            {
                v7 = sub_8002FED8(444);
                if (v7)
                    v7 = sub_80015228(v7, 256u, 0u, 0u, 256u, 192u, 26u);
                v8 = 0;
                v9 = 0x800A71FCu;
                w_u32(0x800FF088u, v7);
                w_u32(((uint32)((v7 + 20))), 18);
                while (v8 < (sint32)r_u32(0x800FF618u))
                {
                    prefix[0] = r_u8(r_u32(v9));
                    prefix[1] = r_u8(r_u32(v9) + 1u);
                    prefix[2] = r_u8(r_u32(v9) + 2u);
                    prefix[3] = 0u;
                    if (!missing_80067724_native(prefix, 0x800FF0A8u))
                        sub_80015474(r_u32(0x800FF088u), r_u32(v9));
                    (v9 += 4u);
                    ++v8;
                }

                sub_800152F8(r_u32(0x800FF088u));
            }
            sub_80015BA4(r_u32(0x800FF088u));
            if (v43)
            {
                sub_80063DD4(r_u32(((uint32)(((r_u32(0x800FF088u) + (28 * r_u8(((uint32)((r_u32(0x800FF088u) + 6)))))) + 24)))));
                if (r_u32(0x800FF088u))
                    cleanup_17914_menu(r_u32(0x800FF088u));
                v1 = 1;
                w_u32(0x800FF088u, 0);
                w_u32(0x800FF2ECu, 6);
                sub_80069DF0(23, 0x2000, 0);
            }
            if (v44)
            {
                if (r_u32(0x800FF088u))
                    cleanup_17914_menu(r_u32(0x800FF088u));
                w_u32(0x800FF088u, 0);
                w_u32(0x800FF080u, 1);
                sub_80069DF0(19, 0x2000, 0);
            }
            goto LABEL_190;

        case 3:
            if (!r_u32(0x800EC264u))
            {
            LABEL_62:
                w_u32(0x800FF080u, 4);

                goto LABEL_190;
            }
            sub_80015BA4(r_u32(0x800FF024u));
            if (!v43)
                goto LABEL_71;
            if (v46)
            {
                v1 = 1;
            }
            else
            {
                if (sub_800155D4(r_u32(0x800FF024u), r_u32((0x800A5594u + (0) * 4u))))
                {
                    v1 = 1;
                    w_u32(0x800FF2ECu, 7);
                    v11 = 23;
                    goto LABEL_70;
                }
                w_u32(0x800FF080u, 1);
            }
            v11 = 19;
        LABEL_70:
            sub_80069DF0(v11, 0x2000, 0);

        LABEL_71:
            v2 = 19;

            if (v44)
            {
                sub_80069DF0(19, 0x2000, 0);
                goto LABEL_74;
            }
            goto LABEL_190;

        case 4:
            if (r_u32(0x800EC264u))
            LABEL_74:
                w_u32(0x800FF080u, 1);

            goto LABEL_190;

        case 5:
            if (r_u8(r_u32(0x800FF030u) + r_u32(0x800FF034u)))
            {
                v2 = 16;
                if ((((uint32)((r_u32(0x800FF64Cu) - r_u32(0x800FFB50u)))) >= 4))
                {
                    w_u32(0x800FF034u, r_u32(0x800FF034u) + 1u);
                    w_u32(0x800FFB50u, r_u32(0x800FF64Cu));
                    sub_80069DF0(16, 0x2000, 0);
                }
            }
            else
            {
                if (!r_u32(0x800FF040u))
                    goto LABEL_86;
                w_u32(0x800FFB5Cu, sub_80069DF0(11, 0x2000, 0));
                w_u32(0x800FF080u, 6);
            }
            goto LABEL_190;

        case 6:
            v12 = ((sint32)r_u32(0x800FFB54u) < 101);
            if (((sint32)r_u32(0x800FFB54u) < 0))
            {
                w_u32(0x800FFB54u, 0);
                v12 = 1;
            }
            if (!v12)
                w_u32(0x800FFB54u, 100);
            v13 = (sint32)(r_u32(0x800FFB58u) + 1u);
            w_u32(0x800FFB58u, (uint32)v13);
            if ((sint32)r_u32(0x800FFB54u) < v13)
            {
                w_u32(0x800FFB58u, r_u32(0x800FFB54u));
                sub_8006A294(r_u32(0x800FFB5Cu));
                goto LABEL_86;
            }
            goto LABEL_190;

        case 7:
            sub_8001BEF8(32, 1);
            w_u32(0x800FF080u, 8);
            goto LABEL_190;

        case 8:
            sub_8001BF34();
            if (sub_8001BE6C())
            {
                w_u32(0x800FF2ECu, 3);
                v1 = 1;
            }
            goto LABEL_190;

        case 9:
            sub_80015BA4(r_u32(0x800FF024u));
            v2 = 23;
            if (v43)
            {
                sub_80069DF0(23, 0x2000, 0);
                if (!sub_800155D4(r_u32(0x800FF024u), r_u32((0x800A5594u + (0) * 4u))))
                    goto LABEL_176;
                w_u32(0x800FF044u, 0);
                w_u32(0x800FF080u, 10);
            }
            goto LABEL_190;

        case 10:
            v13 = (r_u32(0x800FF02Cu) - 1);
            if (r_u32(0x800FF02Cu))
            {
                w_u32(0x800FF02Cu, (uint32)v13);
                if (!v13)
                    w_u32(0x800FF080u, 11);
            }
            else
            {
                w_u32(0x800FF02Cu, 10);
            }
            goto LABEL_190;

        case 11:
            if (((sint32)r_u32(0x800FEEC4u) == -1))
            {
                w_u32(0x800FF080u, 18);
            }
            else if (((sint32)r_u32(0x800FEEC4u) >= 0))
            {
                if (((sint32)r_u32(0x800FEEC4u) == 1))
                {
                    v14 = sub_80010530() == 0;
                    v15 = 12;
                    if (!v14)
                    {
                        sub_800664E4(1);
                        v14 = (sint32)sub_80010A38(0u, 0u) > 0;
                        v15 = 12;
                        if (!v14)
                            v15 = 17;
                    }
                    w_u32(0x800FF080u, v15);
                    if ((v15 == 12))
                    {
                        v16 = sub_8002FED8(444);
                        if (v16)
                            v16 = sub_80015228(v16, 256u, 55u, 0u, 256u, 192u, 18u);
                        w_u32(0x800FF028u, v16);
                        w_u8(((uint32)((v16 + 9))), 1);
                        sub_8001779C(r_u32(0x800FF028u));
                    }
                }
            }
            else if (((sint32)r_u32(0x800FEEC4u) == -2))
            {
                v17 = r_u32(0x800FF024u);
                v18 = 19;
                goto LABEL_127;
            }
            goto LABEL_190;

        case 12:
            sub_80015BA4(r_u32(0x800FF028u));
            if (((sint32)r_u32(0x800FEEC4u) == -1))
            {
                v20 = r_u32(0x800FF028u);
                w_u32(0x800FF080u, 18);
            LABEL_121:
                if (v20)
                    cleanup_17914_menu((uint32)v20);

                w_u32(0x800FF028u, 0);
                goto LABEL_190;
            }
            if (v44)
            {
                sub_80069DF0(19, 0x2000, 0);
                sub_80017760();
                v20 = r_u32(0x800FF028u);
                goto LABEL_121;
            }
            v2 = 23;
            if (v43)
            {
                sub_80069DF0(23, 0x2000, 0);
                if (r_u32(((uint32)((0x800A493Cu + (((136 * r_u8(((uint32)((r_u32(0x800FF028u) + 6))))) + 512)) * 1u)))))
                {
                    v17 = r_u32(0x800FF024u);
                    v18 = 13;
                LABEL_127:
                    w_u32(0x800FF080u, v18);

                    sub_800153D8(v17);
                }
                else
                {
                    v21 = 0x800A53D4u;
                    w_u32(0x800A53D4u, sub_80018B44(0x800A53D4u));
                    v2 = (sint32)0x800A5454u;
                    v22 = (0x800A493Cu + (((136 * r_u8(((uint32)((r_u32(0x800FF028u) + 6))))) + 512)) * 1u);
                    do
                    {
                        v23 = r_u32((v21 + (1) * 4u));
                        v24 = r_u32((v21 + (2) * 4u));
                        v25 = r_u32((v21 + (3) * 4u));
                        w_u32(((uint32)(v22)), r_u32(v21));
                        w_u32((((uint32)(v22)) + (1) * 4u), v23);
                        w_u32((((uint32)(v22)) + (2) * 4u), v24);
                        w_u32((((uint32)(v22)) + (3) * 4u), v25);
                        v21 += (4) * 4u;
                        v22 += (16) * 1u;
                    } while ((v21 != 0x800A5454u));
                    v26 = r_u32((v21 + (1) * 4u));
                    w_u32(((uint32)(v22)), r_u32(v21));
                    w_u32((((uint32)(v22)) + (1) * 4u), v26);
                    w_u32(0x800FF080u, 14);
                    w_u32(0x800FF0A0u, 4);
                    w_u32(0x800FF0A4u, 4);
                }
            }
            goto LABEL_190;

        case 13:
            sub_80015BA4(r_u32(0x800FF024u));
            if (v43)
            {
                if (sub_800155D4(r_u32(0x800FF024u), r_u32((0x800A5594u + (0) * 4u))))
                {
                    sub_80069DF0(23, 0x2000, 0);
                    v27 = 0x800A53D4u;
                    w_u32(0x800A53D4u, sub_80018B44(0x800A53D4u));
                    v28 = (0x800A493Cu + (((136 * r_u8(((uint32)((r_u32(0x800FF028u) + 6))))) + 512)) * 1u);
                    do
                    {
                        v29 = r_u32((v27 + (1) * 4u));
                        v30 = r_u32((v27 + (2) * 4u));
                        v31 = r_u32((v27 + (3) * 4u));
                        w_u32(((uint32)(v28)), r_u32(v27));
                        w_u32((((uint32)(v28)) + (1) * 4u), v29);
                        w_u32((((uint32)(v28)) + (2) * 4u), v30);
                        w_u32((((uint32)(v28)) + (3) * 4u), v31);
                        v27 += (4) * 4u;
                        v28 += (16) * 1u;
                    } while ((v27 != 0x800A5454u));
                    v32 = r_u32((v27 + (1) * 4u));
                    w_u32(((uint32)(v28)), r_u32(v27));
                    w_u32((((uint32)(v28)) + (1) * 4u), v32);
                    w_u32(0x800FF080u, 14);
                    w_u32(0x800FF0A0u, 4);
                    w_u32(0x800FF0A4u, 4);
                }
                else
                {
                    sub_80069DF0(19, 0x2000, 0);
                    w_u32(0x800FF080u, 12);
                }
            }
            v2 = 19;
            if (v44)
                goto LABEL_171;
            goto LABEL_190;

        case 14:
            if (((sint32)r_u32(0x800FEEC4u) == -1))
            {
                if (r_u32(0x800FF028u))
                    cleanup_17914_menu(r_u32(0x800FF028u));
                w_u32(0x800FF028u, 0);
                w_u32(0x800FF080u, 18);
            }
            v13 = (sint32)(r_u32(0x800FF0A0u) - 1u);
            w_u32(0x800FF0A0u, (uint32)v13);
            if (v13 < 0 && r_u32(0x800FEEC4u))
            {
                if (sub_800103E4())
                {
                    v34 = (r_u32(0x800FF0A4u) - 1);
                    w_u32(0x800FF0A4u, v34);
                    if ((v34 >= 0))
                        goto LABEL_189;
                    w_u32(0x800FF080u, 15);
                }
                else
                {
                    if (r_u32(0x800FF028u))
                        cleanup_17914_menu(r_u32(0x800FF028u));
                    w_u32(0x800FF028u, 0);
                    w_u32(0x800FF080u, 16);
                }
            }
            goto LABEL_190;

        case 15:
            v2 = 23;
            if (v45)
            {
            LABEL_171:
                sub_80069DF0(v2, 0x2000, 0);

                w_u32(0x800FF080u, 12);
            }
            goto LABEL_190;

        case 16:
            v2 = 23;
            if (v45)
            {
                sub_80069DF0(23, 0x2000, 0);
            LABEL_176:
                w_u32(0x800FF080u, 7);
            }
            goto LABEL_190;

        case 17:
            v19 = 18;
            if (((sint32)r_u32(0x800FEEC4u) == -1))
                goto LABEL_144;
            if (((sint32)r_u32(0x800FEEC4u) >= 0))
            {
                v19 = 10;
                if (((sint32)r_u32(0x800FEEC4u) == 2))
                    goto LABEL_144;
            }
            else
            {
                v19 = 10;
                if (((sint32)r_u32(0x800FEEC4u) == -2))
                    goto LABEL_144;
            }
            goto LABEL_145;

        case 18:
            if ((((sint32)r_u32(0x800FEEC4u) == -2) || ((((sint32)r_u32(0x800FEEC4u) >= -2) && ((sint32)r_u32(0x800FEEC4u) < 3)) && ((sint32)r_u32(0x800FEEC4u) > 0))))
            {
                v19 = 10;
            LABEL_144:
                w_u32(0x800FF080u, v19);
            }
        LABEL_145:
            if (v45)
                goto LABEL_86;

            goto LABEL_190;

        case 19:
            sub_80015BA4(r_u32(0x800FF024u));
            if (((sint32)r_u32(0x800FEEC4u) == -1))
            {
                v33 = 18;
            LABEL_153:
                w_u32(0x800FF080u, v33);

                goto LABEL_154;
            }
            if ((((sint32)r_u32(0x800FEEC4u) >= -1) && ((sint32)r_u32(0x800FEEC4u) < 3)))
            {
                v33 = 10;
                if (((sint32)r_u32(0x800FEEC4u) > 0))
                    goto LABEL_153;
            }
        LABEL_154:
            if (v43)
            {
                if (sub_800155D4(r_u32(0x800FF024u), r_u32((0x800A5594u + (0) * 4u))))
                {
                    sub_80069DF0(23, 0x2000, 0);
                    w_u32(0x800FF080u, 20);
                    w_u32(0x800FF0A0u, 4);
                    w_u32(0x800FF0A4u, 4);
                }
                else
                {
                    v44 = 1;
                }
            }

            v2 = 19;
            if (v44)
            {
            LABEL_169:
                sub_80069DF0(v2, 0x2000, 0);

            LABEL_86:
                sub_80017760();
            }
        LABEL_190:
            if (r_u32(0x800FF008u))
            {
                result = r_u32(0x800FF0ACu);
                v36 = 951648256;
                if (!r_u32(0x800FF0ACu))
                    result = sub_8002E148(951706923, 64, 64, 127);
            }
            else
            {
                result = sub_8002E13C();
                w_u32(0x800FF0ACu, result);
            }

            if (v1)
            {
                w_u32(0x800FF008u, 0);
                w_u32(0x800FF080u, 0);
                v37 = 3;
                if (r_u32(0x800FF084u))
                    cleanup_17914_menu(r_u32(0x800FF084u));
                w_u32(0x800FF084u, 0);
                if (((r_u32(0x800FF2ECu) != 7) && (r_u32(0x800FF2ECu) != 3)))
                    sub_8006A1DC();
                result = r_u32(0x800FF0ACu);
                if (!r_u32(0x800FF0ACu))
                {
                    sub_8002E2B8();
                    return;
                }
            }
            return;

        case 20:
            v13 = (sint32)(r_u32(0x800FF0A0u) - 1u);
            w_u32(0x800FF0A0u, (uint32)v13);
            if (v13 < 0)
            {
                v2 = 0;
                if ((sint32)r_u32(0x800FEEC4u))
                {
                    if (sub_80010CBC(0u, 0u) == 1u)
                    {
                        w_u32(0x800FF080u, 22);
                    }
                    else
                    {
                        v34 = (r_u32(0x800FF0A4u) - 1);
                        w_u32(0x800FF0A4u, v34);
                        if ((v34 >= 0))
                        LABEL_189:
                            w_u32(0x800FF0A0u, (60u * (4u - (uint32)v34)));

                        else
                            w_u32(0x800FF080u, 21);
                    }
                }
            }
            goto LABEL_190;

        case 21:
            if (((sint32)r_u32(0x800FEEC4u) == -1))
                w_u32(0x800FF080u, 18);
            v2 = 23;
            if (v45)
                goto LABEL_169;
            goto LABEL_190;

        case 22:
            v2 = 23;
            if (v45)
            {
                sub_80069DF0(23, 0x2000, 0);
                w_u32(0x800FF080u, 10);
            }
            goto LABEL_190;

        default:
            goto LABEL_190;
    }
}

uint32 sub_800307F0(void)
{
    sint32 v0;
    short v1;
    short v2;
    uint32 result;
    sint32 vars0;
    sint32 vars4;
    sint32 vars8;
    sint32 varsC;
    if (r_u32(0x800FF2E0u))
    {
        sub_8001A7D4(0xFFu, 0, 0, 0);
        sub_8001A7BC(256);
        sub_8001A7B0(0);
        sub_8001AA28(256u, 60u, r_u32(0x800FF2E0u), 0u, 0u, 256u);
    }
    v0 = 0;
    w_u16(0x800A67C8u, sub_80078760(r_u32(0x800FF904u)));
    sub_8002E2E8();
    if (!r_u32(0x800FF360u))
    {
        w_u32(0x800FF360u, 1);
        w_u32(0x800FFB8Cu, r_u32(0x800FF2F0u));
    }
    if ((((r_u32(0x800FF2F0u) - r_u32(0x800FFB8Cu)) >= 2) && r_u32(0x800FF35Cu)))
    {
        w_u32(0x800FF35Cu, 0);
        v0 = 1;
        sub_80030168(((uint32)(r_u32(0x800FF5A0u))));
        sub_80030168(((uint32)(r_u32(0x800FF4E8u))));
        sub_80030168(((uint32)(r_u32(0x800FF5DCu))));
        sub_800303F4();
    }
    else
    {
        w_u32(0x800FF35Cu, 1);
        sub_8003032C(((uint32)(r_u32(0x800FF5A0u))));
        sub_8003032C(((uint32)(r_u32(0x800FF4E8u))));
        sub_8003032C(((uint32)(r_u32(0x800FF5DCu))));
        sub_800306D8();
    }
    w_u32(0x800FFB8Cu, r_u32(0x800FF2F0u));
    sub_80063770(0x800FF904u);
    sub_8007E63C(0x800ED51Cu, 0x800A67B8u, (r_u32(0x800FF660u) + 112));
    sub_8007EBDC(r_u32(0x800FF794u));
    sub_8007EBDC(r_u32(0x800FF5DCu));
    v1 = r_u16(0x800FFAACu);
    v2 = r_u16(0x800FFAAEu);
    w_u16(0x800FFAACu, 8);
    w_u16(0x800FFAAEu, -8);
    sub_8007EBDC(r_u32(0x800FF5A0u));
    w_u16(0x800FFAACu, v1);
    w_u16(0x800FFAAEu, v2);
    sub_8007EBDC(r_u32(0x800FF4E8u));
    sub_8007EBDC(r_u32(0x800FF204u));
    sub_8007EBDC(r_u32(0x800FF220u));
    sub_8007F138(r_u32(0x800FF8A0u));
    sub_8007EBC4();
    sub_8003736C();
    sub_8006AAB8();
    sub_8006D91C();
    sub_80016884();
    sub_8001B8BC();
    sub_8001C004();
    result = ((uint32)(((r_u32(0x800FF660u) + 16496) & 0x7FFFFFFF)));
    w_u32(0x800FF34Cu, ((r_u32(0x800FF668u) & 0x7FFFFFFF) - ((uint32)(result))));
    if (v0)
    {
        sub_80030390(((uint32)(r_u32(0x800FF5A0u))));
        sub_80030390(((uint32)(r_u32(0x800FF4E8u))));
        sub_80030390(((uint32)(r_u32(0x800FF5DCu))));
        return sub_80030764();
    }
    return result;
}

/* Angle output uses the existing integer slope solver and square-root service */
static uint32 collision_angle_divide(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1u : 0xFFFFFFFFu;
    return (uint32)((long long)(sint32)numerator / (long long)(sint32)denominator);
}

uint32 apocalypse_rotation_between(sint16 rotation[3], const uint32 from[3], const uint32 to[3])
{
    sint32 x = (sint32)(to[0] - from[0]) >> 12;
    sint32 z = (sint32)(to[2] - from[2]) >> 12;
    sint32 y = (sint32)(to[1] - from[1]) >> 12;
    uint32 distance, angle;
    if (z > 0)
        angle = 2048u - apocalypse_angle_from_slope((sint32)collision_angle_divide((uint32)x << 12, 0u - (uint32)z));
    else if (z < 0)
        angle = apocalypse_angle_from_slope((sint32)collision_angle_divide((uint32)x << 12, (uint32)z));
    else
        angle = x > 0 ? (uint32)-1024 : 1024u;
    rotation[1] = (sint16)angle;
    distance = sub_80085B54((uint32)x * (uint32)x + (uint32)z * (uint32)z);
    if (!distance)
        angle = y > 0 ? 1024u : (uint32)-1024;
    else if (y > 0)
        angle = apocalypse_angle_from_slope((sint32)(((uint32)y << 12) / distance));
    else
        angle = 0u - apocalypse_angle_from_slope((sint32)(((0u - (uint32)y) << 12) / distance));
    rotation[0] = (sint16)angle;
    angle = (uint16)rotation[0];
    rotation[2] = 0;
    rotation[0] = (sint16)(angle & 0xFFFu);
    rotation[1] = (sint16)((uint16)rotation[1] & 0xFFFu);
    return distance;
}

uint32 sub_80066B8C(uint32 rotation, uint32 from, uint32 to)
{
    return apocalypse_rotation_between((sint16 *)psx_addr(rotation, 6u), (const uint32 *)psx_addr(from, 12u), (const uint32 *)psx_addr(to, 12u));
}

/* TODO Missing call adapter sub_8008718C */
/* Matrix elements and products retain signed halfword and low-word arithmetic */
static sint32 quaternion_element(uint32 matrix, uint32 index)
{
    return (sint32)(short)r_u16(matrix + 2u * index);
}

static uint32 quaternion_product(sint32 value, uint32 factor)
{
    return (uint32)((sint32)((uint32)value * factor) >> 12);
}

uint32 sub_80076420(uint32 matrix, uint32 output)
{
    sint32 diagonal[3], trace, selected, next, last;
    uint32 root, factor, result;
    diagonal[0] = quaternion_element(matrix, 0u);
    diagonal[1] = quaternion_element(matrix, 4u);
    diagonal[2] = quaternion_element(matrix, 8u);
    trace = diagonal[0] + diagonal[1] + diagonal[2];
    selected = diagonal[0] < diagonal[1];
    if (trace > 0)
    {
        root = SquareRoot0((sint32)(((uint32)trace + 4096u) << 12));
        w_u32(output + 12u, (uint32)((sint32)root >> 1));
        factor = collision_angle_divide(0x800000u, root);
        w_u32(output, quaternion_product(quaternion_element(matrix, 5u) - quaternion_element(matrix, 7u), factor));
        w_u32(output + 4u, quaternion_product(quaternion_element(matrix, 6u) - quaternion_element(matrix, 2u), factor));
        result = quaternion_product(quaternion_element(matrix, 1u) - quaternion_element(matrix, 3u), factor);
        w_u32(output + 8u, result);
        return result;
    }
    if (diagonal[selected] < diagonal[2])
        selected = 2;
    next = (sint32)r_u32(0x800ED4C8u + 4u * (uint32)selected);
    last = (sint32)r_u32(0x800ED4C8u + 4u * (uint32)next);
    root = SquareRoot0((sint32)(((uint32)quaternion_element(matrix, 4u * selected) - (uint32)quaternion_element(matrix, 4u * next) - (uint32)quaternion_element(matrix, 4u * last) + 4096u) << 12));
    w_u32(output + 4u * (uint32)selected, (uint32)((sint32)root >> 1));
    factor = collision_angle_divide(0x800000u, root);
    w_u32(output + 12u, quaternion_product(quaternion_element(matrix, 3u * next + last) - quaternion_element(matrix, 3u * last + next), factor));
    if (next >= 0 && next < 3)
        w_u32(output + 4u * (uint32)next, quaternion_product(quaternion_element(matrix, 3u * selected + next) + quaternion_element(matrix, 3u * next + selected), factor));
    result = (uint32)(last < 2);
    if (last == 0 || last == 1 || last == 2)
    {
        result = quaternion_product(quaternion_element(matrix, 3u * selected + last) + quaternion_element(matrix, 3u * last + selected), factor);
        w_u32(output + 4u * (uint32)last, result);
    }
    else if (last >= 2)
        result = 2u;
    return result;
}

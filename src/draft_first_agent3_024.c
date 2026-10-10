#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static uint32 quaternion_divide(uint32 numerator, sint32 denominator);

static uint32 collision_velocity_square(uint32 object, uint32 offset)
{
    uint32 component = (uint32)((sint32)r_u32(object + offset) >> 9);
    return component * component;
}

static void collision_link_missing(uint32 hit, uint32 object, uint32 point, uint32 normal)
{
    /* TODO Connect 8003C79C collision attachment */
    fprintf(stderr, "8003C79C missing hit=%08X object=%08X point=%08X normal=%08X\n", hit, object, point, normal);
    abort();
}

static void collision_owner_missing(uint32 hit)
{
    /* TODO Connect 8003BB60 collision owner update */
    fprintf(stderr, "8003BB60 missing hit=%08X\n", hit);
    abort();
}

uint32 sub_8003ACB8(uint32 a1)
{
    union
    {
        uint32 words[36];
        sint16 halves[72];
    } collision;

    uint32 vector0[3], vector1[3], vector2[3], vector3[3];
    sint8 v2;
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
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    sint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    sint32 v35;
    short v36;
    sint32 v37;
    sint32 v38;
    sint32 v39;
    short v40;
    sint32 v41;
    sint32 v42;
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
    sint32 v53;
    sint32 v54;
    sint32 v55;
    uint32 v57[4];
    uint32 v58[4];
    sint32 v85[3];
    uint32 v86[4];
    uint32 v87[4];
    sint32 v88;
    sint32 v89;
    sint32 v90;
    sint32 v91;
    sint32 v92;
    sint32 v93;
    sint32 v94;
    sint32 *v95;
    uint32 *v96;
    uint32 *v97;
    v2 = 15;
    v3 = r_u32(0x800FF378u);
    v4 = (r_u32(0x800FF378u) == 12);
    w_u16(((uint32)((a1 + 216))), 0);
    if (v4)
        goto LABEL_5;
    if ((v3 == 3))
    {
        v2 = 11;
    LABEL_6:
        v5 = (a1 + 104);

        goto LABEL_7;
    }
    v5 = (a1 + 104);
    if ((v3 == 6))
    {
    LABEL_5:
        v2 = 1;

        goto LABEL_6;
    }
LABEL_7:
    v6 = r_u32(((uint32)((a1 + 108))));

    v7 = r_u32(((uint32)((a1 + 112))));
    v57[0] = r_u32(((uint32)((a1 + 104))));
    v57[1] = v6;
    v57[2] = v7;
    sub_8006C0B8(v5, (a1 + 116));
    sub_8006C270(v5, (a1 + 129));
    sub_8006C05C(v5);
    v8 = -1;
    if ((((sint32)(r_u32(((uint32)((a1 + 108)))))) >= 0))
    {
        v8 = r_u32(((uint32)((a1 + 108))));
        w_u32(((uint32)((a1 + 108))), 0);
    }
    v9 = r_u32(((uint32)((a1 + 108))));
    v10 = r_u32(((uint32)((a1 + 112))));
    v58[0] = r_u32(((uint32)((a1 + 104))));
    v58[1] = v9;
    v58[2] = v10;
    v11 = (sint32)collision_velocity_square(a1, 104u);
    v12 = (sint32)collision_velocity_square(a1, 108u);
    v13 = (sint32)collision_velocity_square(a1, 112u);
    v14 = 0;
    v95 = v85;
    v96 = &vector1[0];
    v97 = &vector0[0];
    v93 = sub_80085B54((uint32)v11 + (uint32)v12 + (uint32)v13);
    do
    {
        v15 = (sint32)collision_velocity_square(a1, 104u);
        v16 = (sint32)collision_velocity_square(a1, 112u);
        v90 = sub_80085B54((uint32)v15 + collision_velocity_square(a1, 108u) + (uint32)v16);
        if (!v90)
            break;
        v17 = sub_80085B54((uint32)v15 + (uint32)v16);
        if (!v17)
            break;
        v18 = r_u16(((uint32)((a1 + 584))));
        v19 = (sint32)((uint32)(sint32)(sint16)r_u16(0x800EC4C0u) << 2);
        if ((v19 < v17))
            v18 = (sint32)((uint32)v18 + quaternion_divide((uint32)v18 * ((uint32)v17 - (uint32)v19), v19));
        v20 = r_u16(((uint32)((a1 + 584))));
        v21 = (sint32)quaternion_divide((uint32)v20 * (r_u32(a1 + 112u) << 2), v90);
        v22 = (sint32)quaternion_divide((0u - (uint32)v20) * (r_u32(a1 + 104u) << 2), v90);
        vector0[1] = 0;
        vector0[0] = v21;
        vector0[2] = v22;
        v23 = r_u32(((uint32)((a1 + 12))));
        v24 = (r_u32(((uint32)((a1 + 8)))) + 0x10000);
        vector1[0] = r_u32(((uint32)((a1 + 4))));
        vector1[1] = v24;
        vector1[2] = v23;
        vector2[0] = (sint32)quaternion_divide((uint32)v18 * (r_u32(a1 + 104u) << 3), v17);
        vector2[1] = (sint32)quaternion_divide((uint32)r_u16(a1 + 584u) * (r_u32(a1 + 108u) << 3), v90);
        v25 = (8 * r_u32(((uint32)((a1 + 112)))));
        collision.words[0] = (vector1[0] + v21);
        collision.words[1] = v24;
        collision.words[2] = (v23 + v22);
        collision.words[3] = ((vector1[0] + v21) + vector2[0]);
        collision.words[4] = (v24 + vector2[1]);
        vector2[2] = (sint32)quaternion_divide((uint32)v18 * (uint32)v25, v17);
        collision.words[5] = ((v23 + v22) + vector2[2]);
        xport_draft_host_sub_8007BB24_p1(&collision);
        xport_draft_host_sub_8007DD04_p1(&collision, 1);
        ++v14;
        if (collision.words[26])
            goto LABEL_21;
        collision.words[0] = (vector1[0] - vector0[0]);
        collision.words[3] = ((vector1[0] - vector0[0]) + vector2[0]);
        collision.words[1] = vector1[1];
        collision.words[2] = (vector1[2] - vector0[2]);
        collision.words[5] = ((vector1[2] - vector0[2]) + vector2[2]);
        collision.words[4] = (vector1[1] + vector2[1]);
        xport_draft_host_sub_8007BB24_p1(&collision);
        xport_draft_host_sub_8007DD04_p1(&collision, 1);
        if (collision.words[26])
            goto LABEL_21;
        v26 = (v2 & 4);
        if (((v2 & 2) == 0))
            goto LABEL_23;
        vector1[0] = r_u32(((uint32)((a1 + 4))));
        vector1[2] = r_u32(((uint32)((a1 + 12))));
        v27 = ((r_u8(((uint32)((a1 + 26)))) == 12)) ? ((r_u32(((uint32)((a1 + 8)))) - 0x10000)) : ((r_u32(((uint32)((a1 + 8)))) - 0x40000));
        vector1[1] = v27;
        v88 = 3;
        xport_draft_host_sub_8006C5C4_p13(v87, a1 + 104, &v88);
        v89 = (r_u16(((uint32)((a1 + 584)))) - 16);
        xport_draft_host_sub_8006C47C_p123(v86, &v89, v87);
        xport_draft_host_sub_8006C4EC_p123(v95, v86, &v90);
        xport_draft_host_sub_8006C34C_p123(vector3, vector1, v95);
        vector2[0] = vector3[0];
        vector2[1] = vector3[1];
        vector2[2] = vector3[2];
        collision.words[0] = vector1[0];
        collision.words[1] = vector1[1];
        collision.words[2] = vector1[2];
        collision.words[3] = vector3[0];
        collision.words[4] = vector3[1];
        collision.words[5] = vector3[2];
        xport_draft_host_sub_8007BB24_p1(&collision);
        xport_draft_host_sub_8007DD04_p1(&collision, 1);
        v26 = (v2 & 4);
        if (collision.words[26])
        {
        LABEL_21:
            v28 = r_u32(((uint32)((a1 + 104))));

            w_u16(((uint32)((a1 + 216))), (r_u16(((uint32)((a1 + 216)))) | (1u)));
            v29 = (sint32)((uint32)(v28 >> 6) * (uint32)(sint32)collision.halves[60] + (uint32)((sint32)r_u32(a1 + 112u) >> 6) * (uint32)(sint32)collision.halves[62]);
            v4 = (v29 > 0);
            v30 = (v29 >> 12);
            if (!v4)
            {
                w_u32(a1 + 104u, (uint32)v28 - (uint32)((sint32)((uint32)v30 * (uint32)(sint32)collision.halves[60]) >> 6));
                w_u32(a1 + 112u, r_u32(a1 + 112u) - (uint32)((sint32)((uint32)v30 * (uint32)(sint32)collision.halves[62]) >> 6));
                v31 = (((sint32)(r_u32(((uint32)((a1 + 112)))))) >> 2);
                w_u32(a1 + 104u, (uint32)((sint32)r_u32(a1 + 104u) >> 2));
                w_u32(((uint32)((a1 + 112))), v31);
            }
        }
        else
        {
        LABEL_23:
            if (v26)
            {
                vector1[0] = r_u32(((uint32)((a1 + 4))));
                vector1[1] = (r_u32(((uint32)((a1 + 8)))) + 0x10000);
                vector1[2] = r_u32(((uint32)((a1 + 12))));
                xport_draft_host_sub_8006C34C_p123(vector3, v96, v97);
                vector2[0] = vector3[0];
                vector2[1] = vector3[1];
                vector2[2] = vector3[2];
                collision.words[0] = vector1[0];
                collision.words[1] = vector1[1];
                collision.words[2] = vector1[2];
                collision.words[3] = vector3[0];
                collision.words[4] = vector3[1];
                collision.words[5] = vector3[2];
                xport_draft_host_sub_8007BB24_p1(&collision);
                xport_draft_host_sub_8007DD04_p1(&collision, 1);
                if (!collision.words[26])
                {
                    vector1[0] = r_u32(((uint32)((a1 + 4))));
                    vector1[1] = (r_u32(((uint32)((a1 + 8)))) + 0x10000);
                    vector1[2] = r_u32(((uint32)((a1 + 12))));
                    xport_draft_host_sub_8006C3AC_p123(vector3, v96, v97);
                    vector2[0] = vector3[0];
                    vector2[1] = vector3[1];
                    vector2[2] = vector3[2];
                    collision.words[0] = vector1[0];
                    collision.words[1] = vector1[1];
                    collision.words[2] = vector1[2];
                    collision.words[3] = vector3[0];
                    collision.words[4] = vector3[1];
                    collision.words[5] = vector3[2];
                    xport_draft_host_sub_8007BB24_p1(&collision);
                    xport_draft_host_sub_8007DD04_p1(&collision, 1);
                    v32 = (v14 < 3);
                    if (!collision.words[26])
                        goto LABEL_30;
                }
                w_u32(((uint32)((a1 + 104))), (r_u32(((uint32)((a1 + 104)))) + ((16 * collision.halves[60]))));
                w_u32(((uint32)((a1 + 112))), (r_u32(((uint32)((a1 + 112)))) + ((16 * collision.halves[62]))));
                collision.words[26] = 0;
            }
        }
    } while ((collision.words[26] && (v14 < 3)));
    v32 = (v14 < 3);
LABEL_30:
    if (v32)
        sub_8006C0B8((a1 + 4), (a1 + 104));
    else
        w_u32(((uint32)((a1 + 8))), (r_u32(((uint32)((a1 + 8)))) + (r_u32(((uint32)((a1 + 108)))))));

    v33 = 0;
    if (((((sint32)(r_u32(((uint32)((a1 + 108)))))) < 0) || ((v34 = r_u32(((uint32)((a1 + 428)))) != 0) && (((sint32)(r_u32(((uint32)((v34 + 108)))))) < 0))))
        v33 = 1;
    if (v33)
    {
        collision.words[0] = r_u32(((uint32)((a1 + 4))));
        collision.words[1] = r_u32(((uint32)((a1 + 8))));
        v35 = r_u32(((uint32)((a1 + 12))));
        collision.words[3] = collision.words[0];
        collision.words[4] = (collision.words[1] - 753664);
        collision.words[2] = v35;
        collision.words[5] = v35;
        xport_draft_host_sub_8007BB24_p1(&collision);
        xport_draft_host_sub_8007DD04_p1(&collision, 1);
        if (collision.words[26])
        {
            if ((sint32)(r_u32(a1 + 8u) - 491520u) < (sint32)collision.words[28])
            {
                v36 = r_u16(((uint32)((a1 + 216))));
                w_u32(((uint32)((a1 + 8))), (collision.words[28] + 491520));
                w_u32(((uint32)((a1 + 108))), 0);
                w_u16(((uint32)((a1 + 216))), (v36 | 0x100));
            }
        }
    }
    if ((v8 >= 0))
    {
        collision.words[0] = r_u32(((uint32)((a1 + 4))));
        collision.words[1] = (r_u32(((uint32)((a1 + 8)))) - (((uint32)(sint32)(sint16)r_u16(0x800EC4BAu) - 64u) << 12));
        v37 = r_u32(((uint32)((a1 + 12))));
        collision.words[3] = collision.words[0];
        collision.words[4] = ((collision.words[1] + v8) + ((uint32)(sint32)(sint16)r_u16(0x800EC4BCu) << 12));
        collision.words[2] = v37;
        collision.words[5] = v37;
        xport_draft_host_sub_8007BB24_p1(&collision);
        xport_draft_host_sub_8007DD04_p1(&collision, 1);
        if (collision.words[26])
        {
            sub_8005D20C(a1, collision.words[32], collision.words[26]);
            if ((collision.halves[61] >= -3072))
            {
                v41 = (v8 - (v8 >> (r_u8(a1 + 129u) & 31u)));
                w_u32(((uint32)((a1 + 8))), (collision.words[28] - (r_u8(((uint32)((a1 + 582)))) << 12)));
                v42 = (sint32)((uint32)(v41 >> 12) * (uint32)(sint32)collision.halves[60]);
                v43 = (v8 - (v8 >> (r_u8(a1 + 131u) & 31u)));
                w_u32(((uint32)((a1 + 4))), (r_u32(((uint32)((a1 + 4)))) + (v42)));
                v44 = (sint32)((uint32)(v43 >> 12) * (uint32)(sint32)collision.halves[62]);
                v45 = r_u32(((uint32)((a1 + 12))));
                w_u32(((uint32)((a1 + 108))), v8);
                w_u32(((uint32)((a1 + 8))), (r_u32(((uint32)((a1 + 8)))) + ((v8 - (v8 >> (r_u8(a1 + 130u) & 31u))))));
                w_u32(((uint32)((a1 + 12))), (v45 + v44));
            }
            else
            {
                v38 = r_u8(((uint32)((a1 + 582))));
                v39 = collision.words[28];
                v40 = r_u16(((uint32)((a1 + 216))));
                w_u32(((uint32)((a1 + 108))), 0);
                w_u32(((uint32)((a1 + 8))), (v39 - (v38 << 12)));
                w_u16(((uint32)((a1 + 216))), (v40 | 2));
            }
            w_u16(((uint32)((a1 + 164))), collision.halves[60]);
            w_u16(((uint32)((a1 + 166))), collision.halves[61]);
            w_u16(((uint32)((a1 + 168))), collision.halves[62]);
            w_u32(((uint32)((a1 + 184))), collision.words[27]);
            w_u32(((uint32)((a1 + 188))), collision.words[28]);
            w_u32(((uint32)((a1 + 192))), collision.words[29]);
            if (((r_u16(collision.words[26]) & 0x100) != 0))
            {
                collision_link_missing((uint32)collision.words[26], a1, a1 + 4u, a1 + 164u);
                collision_owner_missing((uint32)collision.words[26]);
                w_u32(((uint32)((a1 + 428))), collision.words[26]);
            }
            else
            {
                w_u32(((uint32)((a1 + 428))), 0);
            }
        }
        else
        {
            if (!v93)
                goto LABEL_61;
            v91 = 3;
            xport_draft_host_sub_8006C5C4_p123(vector2, v58, &v91);
            v92 = (r_u16(((uint32)((a1 + 584)))) + 8);
            xport_draft_host_sub_8006C47C_p123(vector1, &v92, vector2);
            xport_draft_host_sub_8006C4EC_p123(vector0, vector1, &v93);
            collision.words[0] = (r_u32(((uint32)((a1 + 4)))) + vector0[0]);
            collision.words[1] = (r_u32(((uint32)((a1 + 8)))) - ((uint32)(sint32)(sint16)r_u16(0x800EC4BAu) << 12));
            v46 = r_u32(((uint32)((a1 + 12))));
            collision.words[3] = collision.words[0];
            collision.words[2] = (v46 + vector0[2]);
            v47 = r_u32(((uint32)((a1 + 8))));
            collision.words[5] = (v46 + vector0[2]);
            collision.words[4] = (v47 + v8);
            xport_draft_host_sub_8007BB24_p1(&collision);
            xport_draft_host_sub_8007DD04_p1(&collision, 1);
            if (!collision.words[26])
            {
                v94 = 1;
                xport_draft_host_sub_8006C1E8_p12(vector0, &v94);
                collision.words[0] = (r_u32(((uint32)((a1 + 4)))) + vector0[0]);
                collision.words[1] = (r_u32(((uint32)((a1 + 8)))) - ((uint32)(sint32)(sint16)r_u16(0x800EC4BAu) << 12));
                v48 = r_u32(((uint32)((a1 + 12))));
                collision.words[3] = collision.words[0];
                collision.words[2] = (v48 + vector0[2]);
                v49 = r_u32(((uint32)((a1 + 8))));
                collision.words[5] = (v48 + vector0[2]);
                collision.words[4] = (v49 + v8);
                xport_draft_host_sub_8007BB24_p1(&collision);
                xport_draft_host_sub_8007DD04_p1(&collision, 1);
                if (!collision.words[26])
                    goto LABEL_61;
            }
            if (((r_u16(((uint32)((collision.words[32] + 18)))) & 4) != 0))
            {
                if (((r_u16(collision.words[26]) & 0x100) != 0))
                    w_u32(((uint32)((a1 + 428))), collision.words[26]);
                else
                    w_u32(((uint32)((a1 + 428))), 0);
                v50 = 1;
                w_u16(((uint32)((a1 + 216))), (r_u16(((uint32)((a1 + 216)))) | (0x40u)));
                w_u32(((uint32)((a1 + 544))), collision.words[28]);
                while (1)
                {
                    collision.words[0] = r_u32(((uint32)((a1 + 4))));
                    collision.words[1] = (r_u32(((uint32)((a1 + 544)))) + (v50 << 16));
                    collision.words[2] = r_u32(((uint32)((a1 + 12))));
                    v51 = r_u32(((uint32)((a1 + 4))));
                    collision.words[4] = collision.words[1];
                    collision.words[3] = (v51 + vector0[0]);
                    collision.words[5] = (r_u32(((uint32)((a1 + 12)))) + vector0[2]);
                    xport_draft_host_sub_8007BB24_p1(&collision);
                    xport_draft_host_sub_8007DD04_p1(&collision, 1);
                    ++v50;
                    if (collision.words[26])
                        break;
                    if ((v50 >= 7))
                    {
                        v52 = (a1 + 104);
                        goto LABEL_63;
                    }
                }

                w_u16(((uint32)((a1 + 216))), (r_u16(((uint32)((a1 + 216)))) | (0x80u)));
                w_u32(((uint32)((a1 + 540))), collision.words[27]);
                w_u32(((uint32)((a1 + 548))), collision.words[29]);
                w_u32(((uint32)((a1 + 552))), collision.halves[60]);
                w_u32(((uint32)((a1 + 556))), collision.halves[61]);
                w_u32(((uint32)((a1 + 560))), collision.halves[62]);
            }
            else
            {
            LABEL_61:
                v53 = r_u32(((uint32)((a1 + 8))));

                w_u32(((uint32)((a1 + 108))), v8);
                w_u32(((uint32)((a1 + 8))), (v53 + v8));
            }
        }
    }
    v52 = (a1 + 104);
LABEL_63:
    xport_draft_host_sub_8006C3AC_p13(vector0, v52, v57);

    v54 = vector0[1];
    v55 = vector0[2];
    w_u32(((uint32)((a1 + 152))), vector0[0]);
    w_u32(((uint32)((a1 + 156))), v54);
    w_u32(((uint32)((a1 + 160))), v55);
    sub_8006C730((a1 + 16), (a1 + 132));
    sub_8006C624((a1 + 16));
    sub_8006C730((a1 + 132), (a1 + 138));
    sub_8006C8E8((a1 + 132), (a1 + 144));
    return sub_8006C64C((a1 + 132));
}

uint32 sub_8006C3AC(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 result;
    sint32 v4;
    sint32 v5;
    result = a1;
    v4 = (r_u32((a2 + (1) * 4u)) - r_u32((a3 + (1) * 4u)));
    v5 = (r_u32((a2 + (2) * 4u)) - r_u32((a3 + (2) * 4u)));
    w_u32(a1, (r_u32(a2) - r_u32(a3)));
    w_u32((a1 + (1) * 4u), v4);
    w_u32((a1 + (2) * 4u), v5);
    return result;
}

uint32 sub_8006C64C(uint32 a1)
{
    sint32 result;
    if ((((unsigned short)((r_u16(a1) + 1))) < 3u))
        w_u16(a1, 0);
    if ((((unsigned short)((r_u16((a1 + (1) * 2u)) + 1))) < 3u))
        w_u16((a1 + (1) * 2u), 0);
    result = (((unsigned short)((r_u16((a1 + (2) * 2u)) + 1))) < 3u);
    if ((((unsigned short)((r_u16((a1 + (2) * 2u)) + 1))) < 3u))
        w_u16((a1 + (2) * 2u), 0);
    return result;
}

uint32 sub_8005EA80(uint32 a1)
{
    sint32 result;
    sint32 v2;
    sint32 v3;
    result = 0;
    if ((((sint8)(r_u8(((uint32)((a1 + 468)))))) > 0))
    {
        v2 = ((sint16)(r_u16(((uint32)((a1 + 474))))));
        result = 0;
        if ((v2 >= 1281))
        {
            if ((v2 >= 2816))
            {
                return 0;
            }
            else
            {
                v3 = r_u8(((uint32)((a1 + 26))));
                w_u32(((uint32)((a1 + 460))), 32);
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

void sub_80085B04(uint32 a1, uint32 a2)
{
    short v2;
    short v3;
    short v4;
    short v5;
    short v6;
    short v7;
    short v8;
    short v9;
    v2 = r_u16((a1 + (1) * 2u));
    v3 = r_u16((a1 + (2) * 2u));
    v4 = r_u16((a1 + (3) * 2u));
    v5 = r_u16((a1 + (4) * 2u));
    v6 = r_u16((a1 + (5) * 2u));
    v7 = r_u16((a1 + (6) * 2u));
    v8 = r_u16((a1 + (7) * 2u));
    v9 = r_u16((a1 + (8) * 2u));
    w_u16(a2, r_u16(a1));
    w_u16((a2 + (3) * 2u), v2);
    w_u16((a2 + (6) * 2u), v3);
    w_u16((a2 + (1) * 2u), v4);
    w_u16((a2 + (4) * 2u), v5);
    w_u16((a2 + (7) * 2u), v6);
    w_u16((a2 + (2) * 2u), v7);
    w_u16((a2 + (5) * 2u), v8);
    w_u16((a2 + (8) * 2u), v9);
}

uint32 sub_8006C22C(uint32 a1, uint32 a2)
{
    uint32 result;
    result = a1;
    w_u32(a1, (r_u32(a1) << (r_u32(a2))));
    w_u32((a1 + (1) * 4u), (r_u32((a1 + (1) * 4u)) << (r_u32(a2))));
    w_u32((a1 + (2) * 4u), (r_u32((a1 + (2) * 4u)) << (r_u32(a2))));
    return result;
}

uint32 apocalypse_vector_different_native(uint32 left, const void *right)
{
    uint32 axis, first, second;
    for (axis = 0u; axis < 3u; ++axis)
    {
        first = r_u32(left + axis * 4u);
        memcpy(&second, (const uint8 *)right + axis * 4u, sizeof(second));
        if (first != second)
            return 1u;
    }
    return 0u;
}

uint32 sub_8006C304(uint32 left, uint32 right)
{
    return apocalypse_vector_different_native(left, psx_addr(right, 12u));
}

uint32 sub_80067508(uint32 a1, uint32 a2, uint32 a3)
{
    sint32 result;
    char v6[16];
    result = xport_draft_host_sub_8007C398_p3(a1, a2, v6, a3, 0u);
    if (!result)
    {
        w_u32(0x800ED638u, ((sint32)(r_u32(a1))));
        w_u32(0x800ED63Cu, ((sint32)(r_u32((a1 + (1) * 4u)))));
        w_u32(0x800ED640u, ((sint32)(r_u32((a1 + (2) * 4u)))));
        w_u32(0x800ED644u, ((sint32)(r_u32(a2))));
        w_u32(0x800ED648u, ((sint32)(r_u32((a2 + (1) * 4u)))));
        w_u32(0x800ED64Cu, ((sint32)(r_u32((a2 + (2) * 4u)))));
        sub_8007BB24(0x800ED638u);
        w_u32(0x800FF974u, 1);
        sub_8007BEA0(r_u32(0x800FF5DCu), 0x800ED638u);
        result = r_u32(0x800ED6A0u);
        w_u32(0x800FF974u, 0);
    }
    return result;
}

uint32 sub_8006C40C(uint32 a1, uint32 a2, uint32 a3)
{
    uint32 result;
    sint32 v4;
    sint32 v5;
    result = a1;
    v4 = (r_u32((a2 + (1) * 4u)) * r_u32(a3));
    v5 = (r_u32((a2 + (2) * 4u)) * r_u32(a3));
    w_u32(a1, (r_u32(a2) * r_u32(a3)));
    w_u32((a1 + (1) * 4u), v4);
    w_u32((a1 + (2) * 4u), v5);
    return result;
}

/* TODO Missing call adapter sub_80085A08 */
uint32 sub_8007FDF0(uint32 object)
{
    uint32 z, y;
    sub_800858FC(object + 16u, object + 324u);
    if (r_u16(object) & 0x200u)
    {
        uint8 matrix[20];
        uint32 index, column, row;
        for (index = 0u; index < 5u; ++index)
            xport_store_le32(matrix + index * 4u, r_u32(object + 324u + index * 4u));
        xport_draft_host_sub_80085A08_p2(object, matrix);
        for (column = 0u; column < 3u; ++column)
            for (row = 0u; row < 3u; ++row)
                w_u16(object + 324u + row * 6u + column * 2u, xport_load_le16(matrix + row * 6u + column * 2u));
    }
    z = r_u32(object + 12u);
    w_u32(object + 344u, (uint32)((sint32)r_u32(object + 4u) >> 12));
    y = r_u32(object + 8u);
    w_u32(object + 352u, (uint32)((sint32)z >> 12));
    y = (uint32)((sint32)y >> 12);
    w_u32(object + 348u, y);
    return y;
}

static void pose_load_rotation(const uint8 *matrix)
{
    uint32 words[5], i;
    for (i = 0; i < 5u; ++i)
        words[i] = xport_load_le32(matrix + 4u * i);
    for (i = 0; i < 5u; ++i)
        xport_gte_write_control(i, words[i]);
}

static void pose_store_column(uint8 *output, uint32 column)
{
    uint32 values[3], row;
    for (row = 0; row < 3u; ++row)
        values[row] = xport_gte_read_data(9u + row);
    for (row = 0; row < 3u; ++row)
        xport_store_le16(output + 6u * row + 2u * column, (uint16)values[row]);
}

/* Compact pose matrices contain nine rotation and three translation halfwords */
static void pose_compose(uint8 *output, const uint8 *left, const uint8 *right)
{
    uint32 words[5], i;
    pose_load_rotation(left);
    for (i = 0; i < 3u; ++i)
        xport_gte_write_control(5u + i, (uint32)(sint32)(sint16)xport_load_le16(left + 18u + 2u * i));
    for (i = 0; i < 5u; ++i)
        words[i] = xport_load_le32(right + 4u * i);
    xport_gte_write_data(0u, (words[0] & 0xffffu) | (words[1] & 0xffff0000u));
    xport_gte_write_data(1u, words[3]);
    xport_gte_write_data(2u, (words[0] >> 16) | (words[2] << 16));
    xport_gte_write_data(3u, words[3] >> 16);
    xport_gte_write_data(4u, (words[1] & 0xffffu) | (words[2] & 0xffff0000u));
    xport_gte_write_data(5u, words[4]);
    xport_gte_execute(0x486012u);
    pose_store_column(output, 0u);
    xport_gte_execute(0x48E012u);
    pose_store_column(output, 1u);
    xport_gte_execute(0x496012u);
    pose_store_column(output, 2u);
    for (i = 0; i < 3u; ++i)
        xport_gte_write_data(9u + i, (uint32)(sint32)(sint16)xport_load_le16(right + 18u + 2u * i));
    xport_gte_execute(0x498012u);
    for (i = 0; i < 3u; ++i)
        words[i] = xport_gte_read_data(9u + i);
    for (i = 0; i < 3u; ++i)
        xport_store_le16(output + 18u + 2u * i, (uint16)words[i]);
}

static void pose_relative(uint8 *output, const uint8 *left, const uint8 *right)
{
    uint32 first, second, third, values[3], i;
    pose_load_rotation(left);
    first = xport_load_le32(right);
    second = xport_load_le32(right + 4u);
    xport_gte_write_data(0u, first);
    xport_gte_write_data(1u, second);
    xport_gte_execute(0x486012u);
    pose_store_column(output, 0u);
    third = xport_load_le32(right + 8u);
    xport_gte_write_data(0u, (third << 16) | (second >> 16));
    xport_gte_write_data(1u, third >> 16);
    xport_gte_execute(0x486012u);
    pose_store_column(output, 1u);
    first = xport_load_le32(right + 12u);
    second = xport_load_le32(right + 16u);
    xport_gte_write_data(0u, first);
    xport_gte_write_data(1u, second);
    xport_gte_execute(0x486012u);
    pose_store_column(output, 2u);
    pose_load_rotation(output);
    for (i = 0; i < 3u; ++i)
        xport_gte_write_data(9u + i, (uint32)(sint32)(sint16)xport_load_le16(right + 18u + 2u * i));
    xport_gte_execute(0x49E012u);
    for (i = 0; i < 3u; ++i)
        values[i] = (uint32)(sint32)(sint16)xport_load_le16(left + 18u + 2u * i);
    for (i = 0; i < 3u; ++i)
        values[i] -= xport_gte_read_data(9u + i);
    for (i = 0; i < 3u; ++i)
        xport_store_le16(output + 18u + 2u * i, (uint16)values[i]);
}

uint32 sub_8007D148(uint32 object)
{
    uint8 parents[144], matrix[24];
    sint32 top = -1, count, ordinal, total;
    uint32 output = r_u32(object + 356u), links = r_u32(object + 364u);
    uint32 angles = r_u32(object + 360u), table, models, poses, entry, source, joint, row, column;
    uint32 result = r_u16(object) & 4u;
    if (!result || !output || !angles)
        return result;
    table = 0x800EAEF8u + 64u * r_u8(object + 27u);
    models = r_u32(table + 16u);
    total = (sint32)r_u32(models - 4u);
    poses = r_u32(table + 24u);
    for (ordinal = 0; ordinal < total; ++ordinal)
        w_u16(output + 24u * ordinal, 0x8000u);
    entry = poses + 8u * r_u8(object + 26u);
    count = r_u16(links - 2u);
    source = r_u16(entry + 10u) ? 0x800ED760u : poses + r_u32(entry + 4u) + 24u * r_u8(object + 24u) * (uint32)total;
    for (ordinal = 0; ordinal < count; ++ordinal, links += 12u, angles += 12u)
    {
        const uint8 *base;
        joint = r_u16(links);
        while (top >= 0 && r_u16(links + 10u) < xport_load_le32(parents + 28u * top + 24u))
            --top;
        base = (const uint8 *)psx_addr(source + 24u * joint, 24u);
        if (r_u16(angles) || r_u16(angles + 2u) || r_u16(angles + 4u))
        {
            xport_draft_host_sub_800858FC_p2(angles, matrix);
            xport_store_le16(matrix + 18u, 0u);
            xport_store_le16(matrix + 20u, 0u);
            xport_store_le16(matrix + 22u, 0u);
            pose_compose(matrix, base, matrix);
            if (top >= 0)
                pose_compose(matrix, parents + 28u * top, matrix);
            ++top;
            if (28u * (uint32)top + 28u > sizeof(parents))
            {
                fprintf(stderr, "Pose hierarchy exceeds original local buffer\n");
                abort();
            }
            pose_relative(parents + 28u * top, matrix, base);
            xport_store_le32(parents + 28u * top + 24u, (uint32)ordinal);
            for (row = 0; row < 3u; ++row)
            {
                for (column = 0; column < 3u; ++column)
                    w_u16(output + 24u * joint + row * 6u + column * 2u, xport_load_le16(matrix + row * 6u + column * 2u));
                w_u16(output + 24u * joint + 18u + row * 2u, xport_load_le16(matrix + 18u + row * 2u));
            }
        }
        else if (top < 0)
        {
            for (row = 0; row < 3u; ++row)
            {
                for (column = 0; column < 3u; ++column)
                    w_u16(output + 24u * joint + row * 6u + column * 2u, r_u16(source + 24u * joint + row * 6u + column * 2u));
                w_u16(output + 24u * joint + 18u + row * 2u, r_u16(source + 24u * joint + 18u + row * 2u));
            }
        }
        else
            pose_compose((uint8 *)psx_addr(output + 24u * joint, 24u), parents + 28u * top, base);
    }
    result = r_u16(poses + 8u * r_u8(object + 26u) + 10u);
    if (!result)
        for (ordinal = 0; ordinal < total; ++ordinal)
        {
            uint32 destination = output + 24u * ordinal, from = source + 24u * ordinal, words[4];
            if ((sint16)r_u16(destination) == -32768)
            {
                for (column = 0; column < 4u; ++column)
                    words[column] = r_u32(from + column * 4u);
                for (column = 0; column < 4u; ++column)
                    w_u32(destination + column * 4u, words[column]);
                words[0] = r_u32(from + 16u);
                words[1] = r_u32(from + 20u);
                w_u32(destination + 16u, words[0]);
                w_u32(destination + 20u, words[1]);
            }
            result = ordinal + 1 < total;
        }
    return result;
}

static uint32 weapon_setup_325B0_native(uint32 object, const sint32 position[3], uint32 kind, uint32 enabled, uint32 red, uint32 green, uint32 blue, uint32 count, uint32 mode, uint32 extent0, uint32 extent1)
{
    uint32 values[3];
    sub_800330F4(object);
    w_u32(object + 68u, 0x800A1B20u);
    memcpy(values, position, sizeof(values));
    w_u32(object + 24u, values[0]);
    w_u32(object + 28u, values[1]);
    w_u32(object + 32u, values[2]);
    w_u16(object + 10u, kind);
    w_u32(object + 72u, enabled);
    w_u8(object + 76u, red);
    w_u8(object + 77u, green);
    w_u8(object + 78u, blue);
    w_u32(object + 80u, count);
    w_u32(object + 84u, mode);
    w_u32(object + 88u, extent0);
    w_u32(object + 92u, extent1);
    return object;
}

uint32 apocalypse_weapon_tick_23380(uint32 weapon, const sint32 position[3])
{
    uint32 child = r_u32(weapon + 32u), result, sound;
    if (child)
    {
        xport_draft_host_sub_80032EE4_p2(child, (void *)position);
        if (r_u32(0x800FF2F0u) - r_u32(weapon + 36u) >= 31u)
            sub_80022ABC(weapon);
    }
    result = r_u32(weapon + 20u);
    if (result)
    {
        sound = r_u32(weapon + 52u);
        w_u32(weapon + 20u, 0u);
        if (sound)
            sub_8006A294(sound);
        result = r_u32(0x800FF2F0u) - r_u32(weapon + 28u) < 46u;
        if (!result)
        {
            child = sub_80032DC0(96u);
            if (child)
                child = weapon_setup_325B0_native(child, position, 0xFFFFFFFFu, 1u, 100u, 100u, 100u, 3u, 8u, 188u, 188u);
            w_u32(weapon + 32u, child);
            w_u8(child + 66u, 1u);
            result = r_u32(0x800FF2F0u);
            w_u32(weapon + 36u, result);
        }
    }
    return result;
}

uint32 sub_80023380(uint32 weapon, uint32 position)
{
    return apocalypse_weapon_tick_23380(weapon, (const sint32 *)psx_addr(position, 12u));
}

uint32 sub_80062D84(uint32 a1)
{
    uint32 child = r_u32(a1 + 196u), table;
    uint32 result = r_u16(a1 + 78u) & 0xFFF7u;
    w_u16(a1 + 78u, result);
    if (child)
    {
        table = r_u32(child + 68u);
        result = apocalypse_object_cleanup(r_u32(table + 12u), child + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
        w_u32(a1 + 196u, 0u);
    }
    return result;
}

void sub_80037258(uint32 a1)
{
    uint32 node, table;
    for (node = a1; node; node = r_u32(node + 4u))
    {
        if (!r_u8(node + 63u))
        {
            table = r_u32(node + 68u);
            apocalypse_object_virtual20(r_u32(table + 20u), node + (uint32)(sint32)(sint16)r_u16(table + 16u));
        }
    }
}

/* TODO Missing call adapter indirect */
static uint32 behavior_stream_half(uint32 object)
{
    uint32 cursor = r_u32(object + 400u), value = r_u16(cursor);
    w_u32(object + 400u, cursor + 2u);
    return value;
}

static void behavior_position_native(uint32 object, const uint32 position[3])
{
    apocalypse_object_target_native(object, position);
}

uint32 sub_8003C7B4(uint32 object, uint32 command)
{
    uint32 value, duration, axis, step, cursor, position[3], shift = 12u;
    uint32 sound, table, player, receiver, target, x, y, z;
    command = (uint16)command;
    switch (command)
    {
        case 0x4303u:
            w_u16(object + 508u, behavior_stream_half(object));
            return 1u;
        case 0x4305u:
            value = behavior_stream_half(object);
            w_u16(object, value ? r_u16(object) | 8u : r_u16(object) & 0xFFF7u);
            return 1u;
        case 0x4304u:
        case 0x4306u:
        case 0x4307u:
        case 0x4308u:
            if (!(r_u16(object) & 0x200u))
            {
                w_u16(object, r_u16(object) | 0x200u);
                w_u16(object + 40u, 4096u);
                w_u16(object + 38u, 4096u);
                w_u16(object + 36u, 4096u);
            }
            value = behavior_stream_half(object);
            duration = behavior_stream_half(object);
            axis = command == 0x4304u ? 0u : command - 0x4306u;
            w_u16(object + 496u + axis * 2u, duration);
            if (command == 0x4304u)
            {
                w_u16(object + 500u, duration);
                w_u16(object + 498u, duration);
            }
            step = (uint32)(((sint32)(value << 16) >> 4) / (sint32)(duration ? 100u * r_u16(object + 496u + axis * 2u) : 100u));
            w_u16(object + 502u + axis * 2u, step);
            if (command == 0x4304u)
            {
                w_u16(object + 506u, step);
                w_u16(object + 504u, step);
                y = r_u16(object + 504u);
                z = r_u16(object + 506u);
                w_u16(object + 36u, r_u16(object + 36u) + r_u16(object + 502u));
                x = r_u16(object + 40u) + z;
                w_u16(object + 38u, r_u16(object + 38u) + y);
                w_u16(object + 40u, x);
            }
            else
                w_u16(object + 36u + axis * 2u, r_u16(object + 36u + axis * 2u) + r_u16(object + 502u + axis * 2u));
            return 1u;
        case 0x4507u:
            sound = r_u32(object + 520u);
            if (sound)
                sub_8006A294(sound);
            value = behavior_stream_half(object);
            w_u32(object + 520u, sub_80069DF0(value, 0x2000u, 0u));
            w_u32(object + 524u, 0xFFFFFFFFu);
            return 1u;
        case 0x4508u:
            sound = r_u32(object + 520u);
            if (sound)
                sub_8006A294(sound);
            value = behavior_stream_half(object);
            duration = behavior_stream_half(object);
            w_u32(object + 524u, duration);
            w_u32(object + 520u, sub_80069EF4(value, object + 4u, 0u));
            return 1u;
        case 0x4509u:
            sound = r_u32(object + 520u);
            if (sound)
                sub_8006A294(sound);
            w_u32(object + 520u, 0u);
            return 1u;
        case 0x4300u:
            if (r_u32(object + 512u))
                return 1u;
            w_u32(object + 400u, r_u32(object + 400u) - 2u);
            return 0u;
        case 0x4301u:
            player = r_u32(0x800FF5A0u);
            if (r_u32(player + 428u) == object)
                w_u32(player + 428u, 0u);
            value = r_u16(object);
            table = r_u32(object + 68u);
            w_u16(object, value | 1u);
            apocalypse_object_virtual20(r_u32(table + 20u), object + (uint32)(sint32)(sint16)r_u16(table + 16u));
            sub_80022318(object, 0u, 0u);
            return 0u;
        case 0x4302u:
            x = behavior_stream_half(object);
            y = behavior_stream_half(object);
            z = behavior_stream_half(object);
            w_u32(object + 396u, r_u32(object + 396u) | 4u);
            w_u32(object + 528u, x << 11);
            w_u32(object + 532u, y << 11);
            w_u32(object + 536u, z << 11);
            return 1u;
        case 0x4220u:
        case 0x4222u:
            cursor = (r_u32(object + 400u) + 3u) & 0xFFFFFFFCu;
            position[0] = r_u32(cursor);
            position[1] = r_u32(cursor + 4u);
            position[2] = r_u32(cursor + 8u);
            xport_draft_host_sub_8006C22C_p12(position, &shift);
            if (command == 0x4222u)
                xport_draft_host_sub_8006C0B8_p1(position, object + 4u);
            w_u32(object + 400u, cursor + 12u);
            behavior_position_native(object, position);
            return 1u;
        case 0x4221u:
            value = behavior_stream_half(object);
            if (value & 0x2000u)
            {
                table = r_u32(object + 68u);
                receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80u);
                target = r_u32(table + 84u);
                value = xport_draft_guest_call2(target, receiver, value);
            }
            apocalypse_trigger_position(position, (uint16)value);
            behavior_position_native(object, position);
            return 1u;
        case 0x4205u:
            table = r_u32(object + 68u);
            apocalypse_object_virtual20(r_u32(table + 20u), object + (uint32)(sint32)(sint16)r_u16(table + 16u));
            return 0u;
        default:
            return sub_8004BF3C(object, command);
    }
}

uint32 sub_8003CF3C(uint32 a1, uint32 a2)
{
    uint32 v3;
    sint8 v4;
    sint32 result;
    uint32 v6;
    short v7;
    uint32 v8;
    short v9;
    sint32 v10;
    sint32 v11;
    short v12;
    short v13;
    uint32 v14;
    short v15;
    short v16;
    a2 &= 0xFFFFu;
    switch (a2)
    {
        case 0x2123u:
            v3 = ((uint32)(r_u32((((uint32)(a1)) + (100) * 4u))));
            v4 = ((sint8)(r_u8(v3)));
            result = ((sint32)((v3 + (2) * 1u)));
            w_u32((((uint32)(a1)) + (100) * 4u), result);
            w_u8((((uint32)(a1)) + (382) * 1u), v4);
            return result;

        case 0x2124u:
            v6 = ((uint32)(r_u32((((uint32)(a1)) + (100) * 4u))));
            v7 = ((sint16)(r_u16(v6)));
            result = ((sint32)((v6 + (1) * 2u)));
            w_u32((((uint32)(a1)) + (100) * 4u), result);
            w_u16((a1 + (11) * 2u), v7);
            return result;

        case 0x2127u:
            v15 = sub_8004CF78(a1);
            v13 = sub_8004CF78(a1);
            result = sub_8004CF78(a1);
            v14 = (a1 + (66) * 2u);
            w_u16((a1 + (66) * 2u), v15);
            goto LABEL_9;

        case 0x2128u:
            v16 = sub_8004CF78(a1);
            v13 = sub_8004CF78(a1);
            result = sub_8004CF78(a1);
            v14 = (a1 + (69) * 2u);
            w_u16((a1 + (69) * 2u), v16);
            goto LABEL_9;

        case 0x212Fu:
            v8 = ((uint32)(((r_u32((((uint32)(a1)) + (100) * 4u)) + 3) & 0xFFFFFFFC)));
            w_u16((a1 + (11) * 2u), sub_8006E080(r_u32(v8), r_u8((((uint32)(a1)) + (27) * 1u))));
            v9 = ((sint16)(r_u16(a1)));
            w_u32((((uint32)(a1)) + (100) * 4u), (v8 + (1) * 4u));
            result = (v9 & 0xFFFE);
            w_u16(a1, result);
            return result;

        case 0x2134u:
            v10 = sub_8004CF78(a1);
            v11 = sub_8004CF78(a1);
            result = ((sint32)(sub_8004CF78(a1) << 16) >> 4);
            w_u32((((uint32)(a1)) + (26) * 4u), ((sint32)((uint32)v10 << 16) >> 4));
            w_u32((((uint32)(a1)) + (27) * 4u), ((sint32)((uint32)v11 << 16) >> 4));
            w_u32((((uint32)(a1)) + (28) * 4u), result);
            return result;

        case 0x2137u:
            v12 = sub_8004CF78(a1);
            v13 = sub_8004CF78(a1);
            result = sub_8004CF78(a1);
            v14 = (a1 + (8) * 2u);
            w_u16((a1 + (8) * 2u), v12);
        LABEL_9:
            w_u16((v14 + (1) * 2u), v13);

            w_u16((v14 + (2) * 2u), result);
            break;

        default:
            result = sub_8004CFDC(a1, a2);
            break;
    }

    return result;
}

uint32 sub_8003D0F0(uint32 a1, uint32 a2)
{
    a2 &= 0xFFFFu;
    if ((a2 == 8704))
        return r_u32(((uint32)((a1 + 516))));
    else
        return ((short)(sub_8004D2B4(a1, a2)));
}

/* Unverified native spatial volume draft */
uint32 apocalypse_sound_balance_native(const uint32 origin[3], uint32 minimum, uint32 maximum)
{
    uint32 object = r_u32(0x800FF904u), position[3], numerator, weight, angle, flag = 0u;
    uint32 left, right, delta, magnitude;
    sint32 distance, divisor = (sint32)maximum, product;
    if (!object)
        return 0u;
    position[0] = r_u32(object + 4u);
    position[1] = r_u32(object + 8u);
    position[2] = r_u32(object + 12u);
    distance = (sint32)xport_draft_host_sub_8006696C_p2(object + 4u, (void *)origin);
    if ((sint32)minimum >= distance)
        return 0x0FFF0FFFu;
    if (distance >= divisor)
        return 0u;
    delta = maximum - (uint32)distance;
    numerator = (delta << 12) - delta;
    if (!divisor)
        weight = (sint32)numerator < 0 ? 1u : 0xFFFFFFFFu;
    else if ((sint32)numerator == (-2147483647 - 1) && divisor == -1)
        weight = numerator;
    else
        weight = (uint32)((sint32)numerator / divisor);
    object = r_u32(0x800FF904u);
    delta = (uint32)(sint32)(sint16)r_u16(object + 16u) - 1024u;
    magnitude = (delta ^ (uint32)((sint32)delta >> 31)) - (uint32)((sint32)delta >> 31);
    if ((sint32)magnitude < 64)
        return weight | (weight << 16);
    angle = 1024u - (uint32)ratan2((sint32)(origin[2] - position[2]), (sint32)(origin[0] - position[0]));
    object = r_u32(0x800FF904u);
    angle = (angle - ((uint32)r_u16(object + 494u) - 2048u)) & 4095u;
    if (angle - 1025u < 2047u)
    {
        flag = 0x80000000u;
        weight -= (uint32)((sint32)weight >> 4);
    }
    product = (sint32)(weight * (uint32)(sint32)(sint16)r_u16(0x800F863Cu + angle * 4u)) >> 12;
    if (angle < 2048u)
    {
        left = weight;
        right = weight - (uint32)product;
    }
    else
    {
        right = weight;
        left = weight + (uint32)product;
    }
    return right | (left << 16) | flag;
}

uint32 sub_80067E9C(uint32 origin, uint32 minimum, uint32 maximum)
{
    uint32 position[3] = {r_u32(origin), r_u32(origin + 4u), r_u32(origin + 8u)};
    return apocalypse_sound_balance_native(position, minimum, maximum);
}

/* TODO Missing call adapter sub_8003A21C */
/* TODO Missing call adapter sub_8003A274 */
void sub_8004DB90(uint32 a1, uint32 a2)
{
    sint32 v3;
    unsigned short v4;
    uint32 v5;
    sint32 v6;
    short v7;
    sint8 v8;
    if (((uint16)a2 == 8960))
    {
        if (r_u32(((uint32)((a1 + 496)))))
        {
            v3 = r_u32(a1 + 496u);
            v4 = r_u16(r_u32(a1 + 400u));
            w_u32(a1 + 400u, r_u32(a1 + 400u) + 2u);
            /* TODO Implement excluded game function sub_8003A274 */
            fprintf(stderr, "Missing sub_8003A274 (%08X,%08X)\n", (uint32)v3, (uint32)v4);
            ((void)(v3), (void)(v4), abort(), 0u);
        }
    }
    else if (((uint16)a2 == 8961))
    {
        v5 = ((sint32)(r_u32(((uint32)((a1 + 400))))));
        v6 = r_u32(((uint32)((a1 + 496))));
        v7 = r_u16((v5 += 2u, v5 - 2u));
        w_u32(((uint32)((a1 + 400))), v5);
        v8 = r_u8(((uint32)(v5)));
        w_u32(((uint32)((a1 + 400))), (v5 + (1) * 2u));
        if (v6)
        {
            /* TODO Implement excluded game function sub_8003A21C */
            fprintf(stderr, "Missing sub_8003A21C (%08X,%08X,%08X)\n", (uint32)v6, (uint32)(sint32)(sint16)v7, (uint32)(uint8)v8);
            abort();
        }
    }
    else
    {
        sub_8004CFDC(a1, (uint16)a2);
    }
}

uint32 sub_8004B9A4(uint32 a1)
{
    sint32 v2;
    short v3;
    v2 = (a1 + 104);
    v3 = r_u16(0x800FF5E8u);
    w_u32(((uint32)((a1 + 164))), 0x800FF5E4u);
    w_u16(((uint32)((a1 + 168))), v3);
    w_u16(((uint32)((a1 + 216))), 0);
    sub_8006C0B8((a1 + 104), (a1 + 116));
    sub_8006C270(v2, (a1 + 129));
    sub_8006C05C(v2);
    sub_8006C0B8((a1 + 4), v2);
    sub_8006C730((a1 + 16), (a1 + 132));
    sub_8006C624((a1 + 16));
    sub_8006C730((a1 + 132), (a1 + 138));
    sub_8006C8E8((a1 + 132), (a1 + 144));
    return sub_8006C64C((a1 + 132));
}

uint32 sub_8003384C(uint32 a1, uint32 a2)
{
    sint32 result;
    result = (r_u8(((uint32)((a1 + 152)))) | a2);
    w_u8(((uint32)((a1 + 152))), result);
    return result;
}

void sub_80032C18(uint32 a1)
{
    uint32 node = a1, next, table;
    while (node)
    {
        next = r_u32(node + 4u);
        if (r_u8(node + 63u))
        {
            table = r_u32(node + 68u);
            (void)apocalypse_object_cleanup(r_u32(table + 12u), node + (uint32)(sint32)(sint16)r_u16(table + 8u), 3u);
        }
        node = next;
    }
}

uint32 sub_80076D44(void)
{
    sint32 v0;
    uint32 result;
    v0 = 15;
    result = (((uint32)((0x801028ACu + (2) * 4u))) + (3) * 1u);
    do
    {
        w_u8(result, 0);
        --v0;
        (result -= 1u);
    } while ((v0 >= 0));
    return result;
}

uint32 sub_80078760(uint32 a1)
{
    return r_u32(((uint32)((a1 + 372))));
}

uint32 sub_800306D8(void)
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
            v1 = r_u32((result + (7) * 4u));
            v2 = r_u32((result + (8) * 4u));
            w_u32((result + (3) * 4u), r_u32((result + (6) * 4u)));
            w_u32((result + (4) * 4u), v1);
            w_u32((result + (5) * 4u), v2);
            v3 = r_u32((result + (19) * 4u));
            v4 = r_u32((result + (20) * 4u));
            w_u32((result + (27) * 4u), r_u32((result + (18) * 4u)));
            w_u32((result + (28) * 4u), v3);
            w_u32((result + (29) * 4u), v4);
            v5 = r_u32((result + (22) * 4u));
            v6 = r_u32((result + (23) * 4u));
            w_u32((result + (30) * 4u), r_u32((result + (21) * 4u)));
            w_u32((result + (31) * 4u), v5);
            w_u32((result + (32) * 4u), v6);
            v7 = r_u32((result + (25) * 4u));
            v8 = r_u32((result + (26) * 4u));
            w_u32((result + (33) * 4u), r_u32((result + (24) * 4u)));
            w_u32((result + (34) * 4u), v7);
            w_u32((result + (35) * 4u), v8);
            result = ((uint32)(r_u32((result + (1) * 4u))));
        } while (result);
    }
    return result;
}

void sub_80085ACC(uint32 a1)
{
    short v1;
    short v2;
    short v3;
    short v4;
    short v5;
    v1 = r_u16((a1 + (2) * 2u));
    v2 = r_u16((a1 + (3) * 2u));
    v3 = r_u16((a1 + (5) * 2u));
    v4 = r_u16((a1 + (6) * 2u));
    v5 = r_u16((a1 + (7) * 2u));
    w_u16((a1 + (3) * 2u), r_u16((a1 + (1) * 2u)));
    w_u16((a1 + (6) * 2u), v1);
    w_u16((a1 + (1) * 2u), v2);
    w_u16((a1 + (7) * 2u), v3);
    w_u16((a1 + (2) * 2u), v4);
    w_u16((a1 + (5) * 2u), v5);
}

uint32 sub_80076274(uint32 a1, uint32 a2)
{
    uint32 result;
    uint32 v3;
    sint32 v4;
    sint32 v5;
    result = a1;
    v3 = ((uint32)((((uint32)(0x800F863Cu)) + (((2 * a2) & 0x3FFC)) * 1u)));
    v4 = ((sint16)(r_u16((v3 + (1) * 2u))));
    v5 = ((sint16)(r_u16(v3)));
    w_u32((result + (1) * 4u), 0);
    w_u32((result + (2) * 4u), 0);
    w_u32(result, v5);
    w_u32((result + (3) * 4u), v4);
    return result;
}

static uint32 quaternion_divide(uint32 numerator, sint32 denominator)
{
    sint32 value = (sint32)numerator;
    if (!denominator)
        return value < 0 ? 1u : 0xFFFFFFFFu;
    if (value == (-2147483647 - 1) && denominator == -1)
        return 0x80000000u;
    return (uint32)(value / denominator);
}

uint32 sub_80076800(uint32 a1, uint32 a2, uint32 a3, uint32 a4)
{
    uint32 first = r_u32(a1), sum, dot, weight1, weight2, angle, index, product, result;
    uint32 component1, component2, component3;
    sint32 denominator;
    sum = first * r_u32(a2);
    sum += r_u32(a1 + 4u) * r_u32(a2 + 4u);
    sum += r_u32(a1 + 8u) * r_u32(a2 + 8u);
    sum += r_u32(a1 + 12u) * r_u32(a2 + 12u);
    dot = (uint32)((sint32)sum >> 12);
    if ((sint32)dot < 0)
    {
        dot = 0u - dot;
        component1 = r_u32(a1 + 4u);
        w_u32(a1, 0u - first);
        component3 = r_u32(a1 + 12u);
        w_u32(a1 + 4u, 0u - component1);
        component2 = r_u32(a1 + 8u);
        w_u32(a1 + 12u, 0u - component3);
        w_u32(a1 + 8u, 0u - component2);
    }
    if ((sint32)(dot + 4096u) < 129)
    {
        w_u32(a4, 0u - r_u32(a1 + 4u));
        w_u32(a4 + 4u, 0u - r_u32(a1));
        w_u32(a4 + 8u, 0u - r_u32(a1 + 12u));
        w_u32(a4 + 12u, r_u32(a1 + 8u));
        weight1 = (uint32)(sint32)(sint16)r_u16(0x800F863Cu + (((3217u * (4096u - a3)) >> 9) & 0x3FFCu));
        weight2 = (uint32)(sint32)(sint16)r_u16(0x800F863Cu + (((3217u * a3) >> 9) & 0x3FFCu));
        for (index = 0u; index < 4u; ++index)
        {
            product = weight1 * r_u32(a1 + index * 4u) + weight2 * r_u32(a4 + index * 4u);
            result = (uint32)((sint32)product >> 12);
            w_u32(a4 + index * 4u, result);
        }
    }
    else
    {
        weight1 = 4096u - a3;
        weight2 = a3;
        if ((sint32)(4096u - dot) >= 129)
        {
            angle = sub_80067930(dot);
            denominator = (sint16)r_u16(0x800F863Cu + (angle & 0xFFFu) * 4u);
            product = (4096u - a3) * angle;
            product = ((uint32)((sint32)product >> 10)) & 0x3FFCu;
            weight1 = quaternion_divide((uint32)(sint32)(sint16)r_u16(0x800F863Cu + product) << 12, denominator);
            product = a3 * angle;
            product = ((uint32)((sint32)product >> 10)) & 0x3FFCu;
            weight2 = quaternion_divide((uint32)(sint32)(sint16)r_u16(0x800F863Cu + product) << 12, denominator);
        }
        for (index = 0u; index < 4u; ++index)
        {
            product = weight1 * r_u32(a1 + index * 4u) + weight2 * r_u32(a2 + index * 4u);
            result = (uint32)((sint32)product >> 12);
            w_u32(a4 + index * 4u, result);
        }
    }
    return result;
}

uint32 sub_80082B14(uint32 geometry, xport_draft_polygon_strip_context *context)
{
    uint32 packet = context->packet_cursor, clipping = context->clipping_mask;
    uint32 remaining = context->remaining_segments;
    uint32 word0 = context->packet_words[0], word1 = context->packet_words[1];
    uint32 word2 = context->packet_words[2], word3 = context->packet_words[3];
    uint32 word4 = context->packet_words[4];
    uint32 repeat;
    context->disposition = 0u;
    do
    {
        uint32 next0, next1, next2, next3, mask;
        if (packet >= context->packet_limit)
        {
            context->packet_cursor = packet;
            context->clipping_mask = clipping;
            context->remaining_segments = remaining;
            context->packet_words[0] = word0;
            context->packet_words[1] = word1;
            context->packet_words[2] = word2 | context->texture_high0;
            context->packet_words[3] = word3;
            context->packet_words[4] = word4;
            context->disposition = 1u;
            return packet;
        }
        w_u32(packet + 4, word0);
        w_u32(packet + 8, word1);
        w_u32(packet + 12, word2 | context->texture_high0);
        w_u32(packet + 16, word3);
        w_u32(packet + 20, word4 | context->texture_high1);
        next0 = r_u32(geometry + 16);
        next1 = r_u16(geometry + 24);
        next2 = r_u32(geometry + 144);
        next3 = r_u16(geometry + 152);
        geometry += 16;
        mask = next0 & next2;
        repeat = clipping & mask;
        clipping = mask & context->frustum_mask;
        if (!repeat)
        {
            packet += 40;
            w_u32(packet - 40, packet + 0x09000000u);
            w_u32(packet - 16, next0);
            w_u32(packet - 12, next1);
            w_u32(packet - 8, next2);
            w_u32(packet - 4, next3);
            context->packet_stride = 40u;
        }
        word1 = next0;
        word2 = next1;
        word3 = next2;
        word4 = next3;
        repeat = remaining != 0;
        --remaining;
    } while (repeat);
    context->packet_cursor = packet;
    context->clipping_mask = clipping;
    context->remaining_segments = remaining;
    context->packet_words[0] = word0;
    context->packet_words[1] = word1;
    context->packet_words[2] = word2;
    context->packet_words[3] = word3;
    context->packet_words[4] = word4;
    return packet;
}

/* TODO Missing call adapter sub_8007B2E4 */
/* TODO Missing call adapter sub_8007FBD4 */
/* TODO Missing call adapter sub_80081458 */
/* TODO Missing call adapter sub_800874CC */
/* TODO Missing call adapter sub_800878DC */
/* TODO Missing call adapter sub_8008793C */
/* TODO Missing call adapter sub_8008798C */
/* TODO Missing call adapter sub_800879AC */
/* TODO Missing call adapter xport_draft_missing_gte_adapter */
static void render_matrix_missing(uint32 left, uint32 right, void *output)
{
    MulMatrix0((MATRIX *)psx_addr(left, 20u), (MATRIX *)psx_addr(right, 20u), (MATRIX *)output);
}

static void render_setup_missing(const void *rotation, const void *light, uint32 pose)
{
    apocalypse_model_gte_setup_native(rotation, light, pose);
}

static void render_shadow_missing(uint32 model, const void *matrix, uint32 rgb, uint32 uv0, uint32 uv1, uint32 texture)
{
    /* TODO Connect native 8007B2E4 matrix input */
    fprintf(stderr, "8007B2E4 missing model=%08X matrix=%p rgb=%08X uv0=%08X uv1=%08X texture=%08X\n", model, matrix, rgb, uv0, uv1, texture);
    abort();
}

static void render_effect_missing(uint32 model, const void *matrix, uint32 normal, uint32 point, uint32 effect, uint32 color)
{
    apocalypse_model_effect_native(model, matrix, normal, point, effect, color);
}

static void render_packet_missing(uint32 model)
{
    /* TODO Connect 8007FBD4 packet renderer */
    fprintf(stderr, "8007FBD4 missing model=%08X\n", model);
    abort();
}

uint32 sub_8007F340(uint32 object)
{
    uint32 flags = r_u16(object), camera, table, models, count, pose;
    uint32 index, mode, animation = 0u, rgb = 0u, uv0 = 0u, uv1 = 0u;
    uint32 input_rotation[5], rotation[8], light[8], matrix[8], transformed[3];
    uint32 axis;
    w_u32(0x800FFB44u, 0u);
    camera = r_u32(0x800FFB0Cu);
    sub_800878DC(camera + 116u);
    for (axis = 0u; axis < 5u; ++axis)
        input_rotation[axis] = r_u32(object + 324u + axis * 4u);
    xport_draft_host_sub_800854F4_p12(input_rotation, rotation);
    for (axis = 0u; axis < 3u; ++axis)
        xport_draft_gte_data_write(9u + axis, (uint32)((sint32)r_u32(object + 4u + axis * 4u) >> 12) - r_u32(camera + 4u + axis * 4u));
    xport_draft_gte_execute(0x49E012u);
    table = 0x800EAEF8u + ((uint32)r_u8(object + 27u) << 6);
    models = r_u32(table + 16u);
    count = r_u32(models - 4u);
    for (axis = 0u; axis < 3u; ++axis)
    {
        transformed[axis] = xport_draft_gte_data_read(25u + axis);
        rotation[5u + axis] = transformed[axis];
    }
    if (flags & 4u)
        pose = r_u32(object + 356u);
    else
    {
        uint32 poses = r_u32(table + 24u);
        uint32 entry = poses + ((uint32)r_u8(object + 26u) << 3);
        if (r_u16(entry + 10u))
        {
            sub_8007CECC(object);
            pose = 0x800ED760u;
        }
        else
            pose = poses + r_u32(entry + 4u) + 24u * r_u8(object + 24u) * count;
    }
    if (flags & 0x80u)
    {
        sub_8008793C(0x800EE710u);
        sub_800879AC((uint32)(sint32)(sint16)r_u16(0x800FFA8Cu), (uint32)(sint32)(sint16)r_u16(0x800FFA8Eu), (uint32)(sint32)(sint16)r_u16(0x800FFA90u));
        sub_8008798C((uint32)(sint32)(sint16)r_u16(0x800FFA94u), (uint32)(sint32)(sint16)r_u16(0x800FFA96u), (uint32)(sint32)(sint16)r_u16(0x800FFA98u));
        render_matrix_missing(0x800EE6F0u, object + 324u, light);
    }
    mode = r_u8(object + 179u);
    if (mode & 0x7Fu)
    {
        sint32 divisor, dividend, remainder;
        uint32 animation_index = (mode - 1u) & 0x7Fu;
        if (animation_index >= 15u)
            animation_index = 14u;
        animation = r_u32(0x800A6808u + animation_index * 4u);
        if (r_u32(0x800FFAB8u) != r_u32(0x800FF650u))
        {
            w_u32(0x800FFABCu, r_u32(0x800FFAA8u));
            w_u32(0x800FFAB8u, r_u32(0x800FF650u));
        }
        divisor = (sint32)r_u32(animation - 4u);
        dividend = (sint32)r_u32(0x800FFABCu);
        remainder = divisor == 0 ? dividend : (dividend == (sint32)0x80000000u && divisor == -1 ? 0 : dividend % divisor);
        uv0 = r_u16(object + 180u);
        uv1 = r_u16(object + 182u);
        rgb = r_u32(object + 176u) & 0xFFFFFFu;
        animation += (uint32)remainder << 3;
    }
    w_u32(0x800FFB18u, r_u32(0x800FFB44u) ? 0x1F8003F8u : r_u32(0x800FFAC0u) + 7992u);
    table = 0x800EAEF8u + ((uint32)r_u8(object + 27u) << 6);
    w_u32(0x800FFB04u, r_u32(table + 32u));
    for (index = 0u; index < count; ++index, pose += 24u)
    {
        uint32 model, effects;
        w_u32(0x800FFB38u, (r_u32(object + 372u) & (1u << (index & 31u))) != 0u);
        model = r_u32(models + index * 4u);
        render_setup_missing(rotation, light, pose);
        if (r_u32(model) & 1u)
        {
            render_matrix_missing(object + 324u, pose, matrix);
            xport_draft_host_sub_80081458_p12(matrix, transformed, 0x800F3E70u, model);
        }
        else if (flags & 0x80u)
            sub_80081794(model + 32u + r_u32(model + 4u) * 8u, r_u32(model + 8u) == r_u32(model + 12u) ? r_u32(model + 8u) : r_u32(model + 4u));
        if (!(mode & 0x80u))
        {
            if (r_u32(0x800FFB44u))
                render_packet_missing(model);
            else
                sub_8007FB34(model);
        }
        if (r_u32(0x800FFB38u))
            continue;
        effects = r_u8(object + 314u) != 0u;
        if ((mode & 0x7Fu) || effects)
        {
            render_matrix_missing(object + 324u, pose, matrix);
            for (axis = 0u; axis < 3u; ++axis)
                xport_draft_gte_data_write(9u + axis, r_u16(pose + 18u + axis * 2u));
            sub_80085C94(object + 324u);
            for (axis = 0u; axis < 3u; ++axis)
                matrix[5u + axis] = xport_draft_gte_data_read(25u + axis) + (uint32)((sint32)r_u32(object + 4u + axis * 4u) >> 12);
        }
        if ((mode & 0x7Fu) && (!r_u32(0x800FFB3Cu) || (sint32)matrix[6] < ((sint32)r_u32(0x800FF37Cu) >> 12) - 40))
            render_shadow_missing(model, matrix, rgb, uv0, (uint32)(sint32)(sint16)uv1, r_u32(animation + 4u));
        if (effects)
        {
            uint32 effect_index = 0u;
            while (effect_index < r_u8(object + 314u))
            {
                render_effect_missing(model, matrix, object + 164u, object + 184u, r_u32(object + 316u) + 8u * effect_index, r_u32(r_u32(object + 320u) + effect_index * 4u));
                ++effect_index;
            }
        }
    }
    return 0u;
}

/* TODO Missing call adapter xport_draft_missing_gte_adapter */
void sub_80081794(uint32 vertices, uint32 count)
{
    uint32 output = 0x800F3E70u, first, second, x, y, z;
    xport_gte_write_data(6u, 0xFFFFFFu);
    first = r_u32(vertices);
    second = r_u32(vertices + 4u);
    do
    {
        xport_gte_write_data(0u, first);
        xport_gte_write_data(1u, second);
        vertices += 8u;
        --count;
        xport_gte_execute(0x108041Bu);
        first = r_u32(vertices);
        second = r_u32(vertices + 4u);
        x = xport_gte_read_data(9u);
        y = xport_gte_read_data(10u);
        z = xport_gte_read_data(11u);
        w_u32(output, (x & 0xFFFFu) | (y << 16));
        w_u32(output + 4u, z);
        output += 8u;
    } while ((sint32)count > 0);
}

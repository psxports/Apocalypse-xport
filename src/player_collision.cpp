extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "player.h"
#include "object_collision_events.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>

static uint32 ReadPlayerVelocitySquare(uint32 object, uint32 offset)
{
    uint32 component = (uint32)((sint32)r_u32(object + offset) >> 9);
    return component * component;
}

static uint32 DividePlayerCollisionScalar(uint32 numerator, sint32 denominator)
{
    sint32 value = (sint32)numerator;
    if (!denominator)
        return value < 0 ? 1 : 0xFFFFFFFF;
    if (value == (sint32)0x80000000 && denominator == -1)
        return 0x80000000;
    return (uint32)(value / denominator);
}

static void MarkPlayerCollisionLink(uint32 hit)
{
    // Original helper ignores the other prepared contact arguments
    sub_8003C79C(hit);
}

static void MarkPlayerCollisionOwner(uint32 hit)
{
    sub_8003BB60(hit);
}

uint32 ResolvePlayerCollisionMotion(uint32 a1)
{
    FUNCTION_MARKER(0x8003ACB8, "SLUS_003.73");
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
    uint32 vertical_velocity;
    v2 = 15;
    v3 = r_u32(0x800FF378);
    v4 = (v3 == 12);
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
    v57[0] = r_u32(a1 + 104);
    v6 = r_u32(a1 + 108);
    v7 = r_u32(a1 + 112);
    v57[1] = v6;
    v57[2] = v7;
    sub_8006C0B8(v5, (a1 + 116));
    sub_8006C270(v5, (a1 + 129));
    sub_8006C05C(v5);
    v8 = -1;
    vertical_velocity = r_u32(a1 + 108);
    if ((sint32)vertical_velocity >= 0)
    {
        v8 = (sint32)vertical_velocity;
        w_u32(((uint32)((a1 + 108))), 0);
    }
    v58[0] = r_u32(a1 + 104);
    v9 = r_u32(a1 + 108);
    v10 = r_u32(a1 + 112);
    v58[1] = v9;
    v58[2] = v10;
    v11 = (sint32)ReadPlayerVelocitySquare(a1, 104);
    v12 = (sint32)ReadPlayerVelocitySquare(a1, 108);
    v13 = (sint32)ReadPlayerVelocitySquare(a1, 112);
    v14 = 0;
    v95 = v85;
    v96 = &vector1[0];
    v97 = &vector0[0];
    v93 = sub_80085B54((uint32)v11 + (uint32)v12 + (uint32)v13);
    do
    {
        v15 = (sint32)ReadPlayerVelocitySquare(a1, 104);
        v12 = (sint32)ReadPlayerVelocitySquare(a1, 108);
        v16 = (sint32)ReadPlayerVelocitySquare(a1, 112);
        v90 = sub_80085B54((uint32)v15 + (uint32)v12 + (uint32)v16);
        if (!v90)
            break;
        v17 = sub_80085B54((uint32)v15 + (uint32)v16);
        if (!v17)
            break;
        v19 = (sint32)((uint32)(sint32)(sint16)r_u16(0x800EC4C0) << 2);
        v18 = r_u16(a1 + 584);
        if ((v19 < v17))
            v18 = (sint32)((uint32)v18 + DividePlayerCollisionScalar((uint32)v18 * ((uint32)v17 - (uint32)v19), v19));
        v20 = r_u16(((uint32)((a1 + 584))));
        v21 = (sint32)DividePlayerCollisionScalar((uint32)v20 * (r_u32(a1 + 112) << 2), v90);
        v22 = (sint32)DividePlayerCollisionScalar((0 - (uint32)v20) * (r_u32(a1 + 104) << 2), v90);
        vector0[1] = 0;
        vector0[0] = v21;
        vector0[2] = v22;
        vector1[0] = r_u32(a1 + 4);
        v24 = r_u32(a1 + 8) + 0x10000;
        v23 = r_u32(a1 + 12);
        vector1[1] = v24;
        vector1[2] = v23;
        vector2[0] = (sint32)DividePlayerCollisionScalar((uint32)v18 * (r_u32(a1 + 104) << 3), v17);
        vector2[1] = (sint32)DividePlayerCollisionScalar((uint32)r_u16(a1 + 584) * (r_u32(a1 + 108) << 3), v90);
        v25 = (8 * r_u32(((uint32)((a1 + 112)))));
        collision.words[0] = (vector1[0] + v21);
        collision.words[1] = v24;
        collision.words[2] = (v23 + v22);
        collision.words[3] = ((vector1[0] + v21) + vector2[0]);
        collision.words[4] = (v24 + vector2[1]);
        vector2[2] = (sint32)DividePlayerCollisionScalar((uint32)v18 * (uint32)v25, v17);
        collision.words[5] = ((v23 + v22) + vector2[2]);
        InitializeCollisionRay((uint8 *)collision.words);
        QueryCollisionRay((uint8 *)collision.words, 1);
        ++v14;
        if (collision.words[26])
            goto LABEL_21;
        collision.words[0] = (vector1[0] - vector0[0]);
        collision.words[3] = ((vector1[0] - vector0[0]) + vector2[0]);
        collision.words[1] = vector1[1];
        collision.words[2] = (vector1[2] - vector0[2]);
        collision.words[5] = ((vector1[2] - vector0[2]) + vector2[2]);
        collision.words[4] = (vector1[1] + vector2[1]);
        InitializeCollisionRay((uint8 *)collision.words);
        QueryCollisionRay((uint8 *)collision.words, 1);
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
        InitializeCollisionRay((uint8 *)collision.words);
        QueryCollisionRay((uint8 *)collision.words, 1);
        v26 = (v2 & 4);
        if (collision.words[26])
        {
        LABEL_21:
            uint32 contact_flags = r_u16(a1 + 216);
            v28 = r_u32(a1 + 104);
            w_u16(a1 + 216, contact_flags | 1);
            v29 = (sint32)((uint32)(v28 >> 6) * (uint32)(sint32)collision.halves[60] + (uint32)((sint32)r_u32(a1 + 112) >> 6) * (uint32)(sint32)collision.halves[62]);
            v4 = (v29 > 0);
            v30 = (v29 >> 12);
            if (!v4)
            {
                w_u32(a1 + 104, (uint32)v28 - (uint32)((sint32)((uint32)v30 * (uint32)(sint32)collision.halves[60]) >> 6));
                w_u32(a1 + 112, r_u32(a1 + 112) - (uint32)((sint32)((uint32)v30 * (uint32)(sint32)collision.halves[62]) >> 6));
                uint32 velocity_x = r_u32(a1 + 104);
                v31 = (sint32)r_u32(a1 + 112) >> 2;
                w_u32(a1 + 104, (uint32)((sint32)velocity_x >> 2));
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
                InitializeCollisionRay((uint8 *)collision.words);
                QueryCollisionRay((uint8 *)collision.words, 1);
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
                    InitializeCollisionRay((uint8 *)collision.words);
                    QueryCollisionRay((uint8 *)collision.words, 1);
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
        InitializeCollisionRay((uint8 *)collision.words);
        QueryCollisionRay((uint8 *)collision.words, 1);
        if (collision.words[26])
        {
            if ((sint32)(r_u32(a1 + 8) - 491520) < (sint32)collision.words[28])
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
        collision.words[1] = (r_u32(((uint32)((a1 + 8)))) - (((uint32)(sint32)(sint16)r_u16(0x800EC4BA) - 64) << 12));
        v37 = r_u32(((uint32)((a1 + 12))));
        collision.words[3] = collision.words[0];
        collision.words[4] = ((collision.words[1] + v8) + ((uint32)(sint32)(sint16)r_u16(0x800EC4BC) << 12));
        collision.words[2] = v37;
        collision.words[5] = v37;
        InitializeCollisionRay((uint8 *)collision.words);
        QueryCollisionRay((uint8 *)collision.words, 1);
        if (collision.words[26])
        {
            sub_8005D20C(a1, collision.words[32], collision.words[26]);
            if ((collision.halves[61] >= -3072))
            {
                v41 = (v8 - (v8 >> (r_u8(a1 + 129) & 31)));
                w_u32(((uint32)((a1 + 8))), (collision.words[28] - (r_u8(((uint32)((a1 + 582)))) << 12)));
                v42 = (sint32)((uint32)(v41 >> 12) * (uint32)(sint32)collision.halves[60]);
                v43 = (v8 - (v8 >> (r_u8(a1 + 131) & 31)));
                w_u32(((uint32)((a1 + 4))), (r_u32(((uint32)((a1 + 4)))) + (v42)));
                v44 = (sint32)((uint32)(v43 >> 12) * (uint32)(sint32)collision.halves[62]);
                v45 = r_u32(((uint32)((a1 + 12))));
                w_u32(((uint32)((a1 + 108))), v8);
                w_u32(((uint32)((a1 + 8))), (r_u32(((uint32)((a1 + 8)))) + ((v8 - (v8 >> (r_u8(a1 + 130) & 31))))));
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
                MarkPlayerCollisionLink((uint32)collision.words[26]);
                MarkPlayerCollisionOwner((uint32)collision.words[26]);
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
            collision.words[1] = (r_u32(((uint32)((a1 + 8)))) - ((uint32)(sint32)(sint16)r_u16(0x800EC4BA) << 12));
            v46 = r_u32(((uint32)((a1 + 12))));
            collision.words[3] = collision.words[0];
            collision.words[2] = (v46 + vector0[2]);
            v47 = r_u32(((uint32)((a1 + 8))));
            collision.words[5] = (v46 + vector0[2]);
            collision.words[4] = (v47 + v8);
            InitializeCollisionRay((uint8 *)collision.words);
            QueryCollisionRay((uint8 *)collision.words, 1);
            if (!collision.words[26])
            {
                v94 = 1;
                xport_draft_host_sub_8006C1E8_p12(vector0, &v94);
                collision.words[0] = (r_u32(((uint32)((a1 + 4)))) + vector0[0]);
                collision.words[1] = (r_u32(((uint32)((a1 + 8)))) - ((uint32)(sint32)(sint16)r_u16(0x800EC4BA) << 12));
                v48 = r_u32(((uint32)((a1 + 12))));
                collision.words[3] = collision.words[0];
                collision.words[2] = (v48 + vector0[2]);
                v49 = r_u32(((uint32)((a1 + 8))));
                collision.words[5] = (v48 + vector0[2]);
                collision.words[4] = (v49 + v8);
                InitializeCollisionRay((uint8 *)collision.words);
                QueryCollisionRay((uint8 *)collision.words, 1);
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
                w_u16(((uint32)((a1 + 216))), (r_u16(((uint32)((a1 + 216)))) | (0x40)));
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
                    InitializeCollisionRay((uint8 *)collision.words);
                    QueryCollisionRay((uint8 *)collision.words, 1);
                    ++v50;
                    if (collision.words[26])
                        break;
                    if ((v50 >= 7))
                    {
                        v52 = (a1 + 104);
                        goto LABEL_63;
                    }
                }

                w_u16(((uint32)((a1 + 216))), (r_u16(((uint32)((a1 + 216)))) | (0x80)));
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

extern "C" uint32 sub_8003ACB8(uint32 object)
{
    FUNCTION_MARKER(0x8003ACB8, "SLUS_003.73");
    return ResolvePlayerCollisionMotion(object);
}

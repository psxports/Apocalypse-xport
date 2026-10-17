#include "visual_object.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "player.h"
#include "scripted_level_object.h"
#include "object_collision_events.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "native_object_storage.h"
#include <stdio.h>
#include <stdlib.h>

static uint32 SquarePlayerVelocityComponent(uint32 velocity)
{
    uint32 component = (uint32)((sint32)velocity >> 9);
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

static void MarkPlayerCollisionLink(ModelInstance *hit)
{
    // Original helper ignores the other prepared contact arguments
    static_cast<ScriptedLevelObject *>(hit->owner)->ActivateContact();
}

static void MarkPlayerCollisionOwner(ModelInstance *hit)
{
    static_cast<ScriptedLevelObject *>(hit->owner)->ApplyPlayerDamage();
}

uint32 ResolvePlayerCollisionMotion(uint32 a1)
{
    FUNCTION_MARKER(0x8003ACB8, "SLUS_003.73");
    CollisionRay collision;
    Player &player = Player::Attach(a1);

    uint32 vector0[3], vector1[3], vector2[3], vector3[3];
    sint8 v2;
    sint32 v3;
    sint32 v4;
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
    ModelInstance *v34;
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

        goto LABEL_7;
    }
    if ((v3 == 6))
    {
    LABEL_5:
        v2 = 1;

        goto LABEL_6;
    }
LABEL_7:
    v57[0] = player.motion.velocity[0];
    v6 = player.motion.velocity[1];
    v7 = player.motion.velocity[2];
    v57[1] = v6;
    v57[2] = v7;
    AddVectors(player.motion.velocity, player.motion.acceleration);
    DampVector(player.motion.velocity, player.motion.drag);
    SnapSmallVector(player.motion.velocity);
    v8 = -1;
    vertical_velocity = player.motion.velocity[1];
    if ((sint32)vertical_velocity >= 0)
    {
        v8 = (sint32)vertical_velocity;
        player.motion.velocity[1] = 0;
    }
    v58[0] = player.motion.velocity[0];
    v9 = player.motion.velocity[1];
    v10 = player.motion.velocity[2];
    v58[1] = v9;
    v58[2] = v10;
    v11 = (sint32)SquarePlayerVelocityComponent(player.motion.velocity[0]);
    v12 = (sint32)SquarePlayerVelocityComponent(player.motion.velocity[1]);
    v13 = (sint32)SquarePlayerVelocityComponent(player.motion.velocity[2]);
    v14 = 0;
    v95 = v85;
    v96 = &vector1[0];
    v97 = &vector0[0];
    v93 = sub_80085B54((uint32)v11 + (uint32)v12 + (uint32)v13);
    do
    {
        v15 = (sint32)SquarePlayerVelocityComponent(player.motion.velocity[0]);
        v12 = (sint32)SquarePlayerVelocityComponent(player.motion.velocity[1]);
        v16 = (sint32)SquarePlayerVelocityComponent(player.motion.velocity[2]);
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
        v21 = (sint32)DividePlayerCollisionScalar((uint32)v20 * (player.motion.velocity[2] << 2), v90);
        v22 = (sint32)DividePlayerCollisionScalar((0 - (uint32)v20) * (player.motion.velocity[0] << 2), v90);
        vector0[1] = 0;
        vector0[0] = v21;
        vector0[2] = v22;
        vector1[0] = r_u32(a1 + 4);
        v24 = r_u32(a1 + 8) + 0x10000;
        v23 = r_u32(a1 + 12);
        vector1[1] = v24;
        vector1[2] = v23;
        vector2[0] = (sint32)DividePlayerCollisionScalar((uint32)v18 * (player.motion.velocity[0] << 3), v17);
        vector2[1] = (sint32)DividePlayerCollisionScalar((uint32)r_u16(a1 + 584) * (player.motion.velocity[1] << 3), v90);
        v25 = (8 * player.motion.velocity[2]);
        collision.pos[0][0] = (vector1[0] + v21);
        collision.pos[0][1] = v24;
        collision.pos[0][2] = (v23 + v22);
        collision.pos[1][0] = ((vector1[0] + v21) + vector2[0]);
        collision.pos[1][1] = (v24 + vector2[1]);
        vector2[2] = (sint32)DividePlayerCollisionScalar((uint32)v18 * (uint32)v25, v17);
        collision.pos[1][2] = ((v23 + v22) + vector2[2]);
        InitializeCollisionRay(collision);
        QueryCollisionRay(collision, 1);
        ++v14;
        if (collision.hit.GetObject())
            goto LABEL_21;
        collision.pos[0][0] = (vector1[0] - vector0[0]);
        collision.pos[1][0] = ((vector1[0] - vector0[0]) + vector2[0]);
        collision.pos[0][1] = vector1[1];
        collision.pos[0][2] = (vector1[2] - vector0[2]);
        collision.pos[1][2] = ((vector1[2] - vector0[2]) + vector2[2]);
        collision.pos[1][1] = (vector1[1] + vector2[1]);
        InitializeCollisionRay(collision);
        QueryCollisionRay(collision, 1);
        if (collision.hit.GetObject())
            goto LABEL_21;
        v26 = (v2 & 4);
        if (((v2 & 2) == 0))
            goto LABEL_23;
        vector1[0] = r_u32(((uint32)((a1 + 4))));
        vector1[2] = r_u32(((uint32)((a1 + 12))));
        v27 = ((r_u8(((uint32)((a1 + 26)))) == 12)) ? ((r_u32(((uint32)((a1 + 8)))) - 0x10000)) : ((r_u32(((uint32)((a1 + 8)))) - 0x40000));
        vector1[1] = v27;
        v88 = 3;
        ShiftVectorLeft((uint32 *)v87, player.motion.velocity, (const uint32 *)&v88);
        v89 = (r_u16(((uint32)((a1 + 584)))) - 16);
        xport_draft_host_sub_8006C47C_p123(v86, &v89, v87);
        xport_draft_host_sub_8006C4EC_p123(v95, v86, &v90);
        xport_draft_host_sub_8006C34C_p123(vector3, vector1, v95);
        vector2[0] = vector3[0];
        vector2[1] = vector3[1];
        vector2[2] = vector3[2];
        collision.pos[0][0] = vector1[0];
        collision.pos[0][1] = vector1[1];
        collision.pos[0][2] = vector1[2];
        collision.pos[1][0] = vector3[0];
        collision.pos[1][1] = vector3[1];
        collision.pos[1][2] = vector3[2];
        InitializeCollisionRay(collision);
        QueryCollisionRay(collision, 1);
        v26 = (v2 & 4);
        if (collision.hit.GetObject())
        {
        LABEL_21:
            uint32 contact_flags = r_u16(a1 + 216);
            v28 = player.motion.velocity[0];
            w_u16(a1 + 216, contact_flags | 1);
            v29 = (sint32)((uint32)(v28 >> 6) * (uint32)(sint32)collision.n.vx + (uint32)((sint32)player.motion.velocity[2] >> 6) * (uint32)(sint32)collision.n.vz);
            v4 = (v29 > 0);
            v30 = (v29 >> 12);
            if (!v4)
            {
                player.motion.velocity[0] = (uint32)v28 - (uint32)((sint32)((uint32)v30 * (uint32)(sint32)collision.n.vx) >> 6);
                player.motion.velocity[2] = player.motion.velocity[2] - (uint32)((sint32)((uint32)v30 * (uint32)(sint32)collision.n.vz) >> 6);
                uint32 velocity_x = player.motion.velocity[0];
                v31 = (sint32)player.motion.velocity[2] >> 2;
                player.motion.velocity[0] = (uint32)((sint32)velocity_x >> 2);
                player.motion.velocity[2] = v31;
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
                collision.pos[0][0] = vector1[0];
                collision.pos[0][1] = vector1[1];
                collision.pos[0][2] = vector1[2];
                collision.pos[1][0] = vector3[0];
                collision.pos[1][1] = vector3[1];
                collision.pos[1][2] = vector3[2];
                InitializeCollisionRay(collision);
                QueryCollisionRay(collision, 1);
                if (!collision.hit.GetObject())
                {
                    vector1[0] = r_u32(((uint32)((a1 + 4))));
                    vector1[1] = (r_u32(((uint32)((a1 + 8)))) + 0x10000);
                    vector1[2] = r_u32(((uint32)((a1 + 12))));
                    xport_draft_host_sub_8006C3AC_p123(vector3, v96, v97);
                    vector2[0] = vector3[0];
                    vector2[1] = vector3[1];
                    vector2[2] = vector3[2];
                    collision.pos[0][0] = vector1[0];
                    collision.pos[0][1] = vector1[1];
                    collision.pos[0][2] = vector1[2];
                    collision.pos[1][0] = vector3[0];
                    collision.pos[1][1] = vector3[1];
                    collision.pos[1][2] = vector3[2];
                    InitializeCollisionRay(collision);
                    QueryCollisionRay(collision, 1);
                    v32 = (v14 < 3);
                    if (!collision.hit.GetObject())
                        goto LABEL_30;
                }
                player.motion.velocity[0] = (player.motion.velocity[0] + ((16 * collision.n.vx)));
                player.motion.velocity[2] = (player.motion.velocity[2] + ((16 * collision.n.vz)));
                collision.hit.SetObject(0);
            }
        }
    } while ((collision.hit.GetObject() && (v14 < 3)));
    v32 = (v14 < 3);
LABEL_30:
    if (v32)
        AddVectors((uint32 *)psx_addr((a1 + 4), 12), player.motion.velocity);
    else
        w_u32(((uint32)((a1 + 8))), (r_u32(((uint32)((a1 + 8)))) + (player.motion.velocity[1])));

    v33 = 0;
    if (((((sint32)(player.motion.velocity[1])) < 0) || (((v34 = player.ground.GetObject()) != 0) && (((sint32)(static_cast<VisualObject *>(v34->owner)->motion.velocity[1])) < 0))))
        v33 = 1;
    if (v33)
    {
        collision.pos[0][0] = r_u32(((uint32)((a1 + 4))));
        collision.pos[0][1] = r_u32(((uint32)((a1 + 8))));
        v35 = r_u32(((uint32)((a1 + 12))));
        collision.pos[1][0] = collision.pos[0][0];
        collision.pos[1][1] = (collision.pos[0][1] - 753664);
        collision.pos[0][2] = v35;
        collision.pos[1][2] = v35;
        InitializeCollisionRay(collision);
        QueryCollisionRay(collision, 1);
        if (collision.hit.GetObject())
        {
            if ((sint32)(r_u32(a1 + 8) - 491520) < (sint32)collision.hit_pos[1])
            {
                v36 = r_u16(((uint32)((a1 + 216))));
                w_u32(((uint32)((a1 + 8))), (collision.hit_pos[1] + 491520));
                player.motion.velocity[1] = 0;
                w_u16(((uint32)((a1 + 216))), (v36 | 0x100));
            }
        }
    }
    if ((v8 >= 0))
    {
        collision.pos[0][0] = r_u32(((uint32)((a1 + 4))));
        collision.pos[0][1] = (r_u32(((uint32)((a1 + 8)))) - (((uint32)(sint32)(sint16)r_u16(0x800EC4BA) - 64) << 12));
        v37 = r_u32(((uint32)((a1 + 12))));
        collision.pos[1][0] = collision.pos[0][0];
        collision.pos[1][1] = ((collision.pos[0][1] + v8) + ((uint32)(sint32)(sint16)r_u16(0x800EC4BC) << 12));
        collision.pos[0][2] = v37;
        collision.pos[1][2] = v37;
        InitializeCollisionRay(collision);
        QueryCollisionRay(collision, 1);
        if (collision.hit.GetObject())
        {
            Player::Attach(a1).SetContactColor(*collision.hit.GetPolygon(), collision.hit.GetObject());
            if ((collision.n.vy >= -3072))
            {
                v41 = (v8 - (v8 >> (player.motion.drag[0] & 31)));
                w_u32(((uint32)((a1 + 8))), (collision.hit_pos[1] - (r_u8(((uint32)((a1 + 582)))) << 12)));
                v42 = (sint32)((uint32)(v41 >> 12) * (uint32)(sint32)collision.n.vx);
                v43 = (v8 - (v8 >> (player.motion.drag[2] & 31)));
                w_u32(((uint32)((a1 + 4))), (r_u32(((uint32)((a1 + 4)))) + (v42)));
                v44 = (sint32)((uint32)(v43 >> 12) * (uint32)(sint32)collision.n.vz);
                v45 = r_u32(((uint32)((a1 + 12))));
                player.motion.velocity[1] = v8;
                w_u32(((uint32)((a1 + 8))), (r_u32(((uint32)((a1 + 8)))) + ((v8 - (v8 >> (player.motion.drag[1] & 31))))));
                w_u32(((uint32)((a1 + 12))), (v45 + v44));
            }
            else
            {
                v38 = r_u8(((uint32)((a1 + 582))));
                v39 = collision.hit_pos[1];
                v40 = r_u16(((uint32)((a1 + 216))));
                player.motion.velocity[1] = 0;
                w_u32(((uint32)((a1 + 8))), (v39 - (v38 << 12)));
                w_u16(((uint32)((a1 + 216))), (v40 | 2));
            }
            w_u16(((uint32)((a1 + 164))), collision.n.vx);
            w_u16(((uint32)((a1 + 166))), collision.n.vy);
            w_u16(((uint32)((a1 + 168))), collision.n.vz);
            w_u32(((uint32)((a1 + 184))), collision.hit_pos[0]);
            w_u32(((uint32)((a1 + 188))), collision.hit_pos[1]);
            w_u32(((uint32)((a1 + 192))), collision.hit_pos[2]);
            if (((collision.hit.GetObject()->flags & 0x100) != 0))
            {
                MarkPlayerCollisionLink(collision.hit.GetObject());
                MarkPlayerCollisionOwner(collision.hit.GetObject());
                player.ground.SetObject(collision.hit.GetObject());
            }
            else
            {
                player.ground.Reset();
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
            collision.pos[0][0] = (r_u32(((uint32)((a1 + 4)))) + vector0[0]);
            collision.pos[0][1] = (r_u32(((uint32)((a1 + 8)))) - ((uint32)(sint32)(sint16)r_u16(0x800EC4BA) << 12));
            v46 = r_u32(((uint32)((a1 + 12))));
            collision.pos[1][0] = collision.pos[0][0];
            collision.pos[0][2] = (v46 + vector0[2]);
            v47 = r_u32(((uint32)((a1 + 8))));
            collision.pos[1][2] = (v46 + vector0[2]);
            collision.pos[1][1] = (v47 + v8);
            InitializeCollisionRay(collision);
            QueryCollisionRay(collision, 1);
            if (!collision.hit.GetObject())
            {
                v94 = 1;
                xport_draft_host_sub_8006C1E8_p12(vector0, &v94);
                collision.pos[0][0] = (r_u32(((uint32)((a1 + 4)))) + vector0[0]);
                collision.pos[0][1] = (r_u32(((uint32)((a1 + 8)))) - ((uint32)(sint32)(sint16)r_u16(0x800EC4BA) << 12));
                v48 = r_u32(((uint32)((a1 + 12))));
                collision.pos[1][0] = collision.pos[0][0];
                collision.pos[0][2] = (v48 + vector0[2]);
                v49 = r_u32(((uint32)((a1 + 8))));
                collision.pos[1][2] = (v48 + vector0[2]);
                collision.pos[1][1] = (v49 + v8);
                InitializeCollisionRay(collision);
                QueryCollisionRay(collision, 1);
                if (!collision.hit.GetObject())
                    goto LABEL_61;
            }
            if (((collision.hit.GetPolygon()->surface & 4) != 0))
            {
                if (((collision.hit.GetObject()->flags & 0x100) != 0))
                    player.ground.SetObject(collision.hit.GetObject());
                else
                    player.ground.Reset();
                v50 = 1;
                w_u16(((uint32)((a1 + 216))), (r_u16(((uint32)((a1 + 216)))) | (0x40)));
                w_u32(((uint32)((a1 + 544))), collision.hit_pos[1]);
                while (1)
                {
                    collision.pos[0][0] = r_u32(((uint32)((a1 + 4))));
                    collision.pos[0][1] = (r_u32(((uint32)((a1 + 544)))) + (v50 << 16));
                    collision.pos[0][2] = r_u32(((uint32)((a1 + 12))));
                    v51 = r_u32(((uint32)((a1 + 4))));
                    collision.pos[1][1] = collision.pos[0][1];
                    collision.pos[1][0] = (v51 + vector0[0]);
                    collision.pos[1][2] = (r_u32(((uint32)((a1 + 12)))) + vector0[2]);
                    InitializeCollisionRay(collision);
                    QueryCollisionRay(collision, 1);
                    ++v50;
                    if (collision.hit.GetObject())
                        break;
                    if ((v50 >= 7))
                    {
                        goto LABEL_63;
                    }
                }

                w_u16(((uint32)((a1 + 216))), (r_u16(((uint32)((a1 + 216)))) | (0x80)));
                w_u32(((uint32)((a1 + 540))), collision.hit_pos[0]);
                w_u32(((uint32)((a1 + 548))), collision.hit_pos[2]);
                w_u32(((uint32)((a1 + 552))), collision.n.vx);
                w_u32(((uint32)((a1 + 556))), collision.n.vy);
                w_u32(((uint32)((a1 + 560))), collision.n.vz);
            }
            else
            {
            LABEL_61:
                v53 = r_u32(((uint32)((a1 + 8))));

                player.motion.velocity[1] = v8;
                w_u32(((uint32)((a1 + 8))), (v53 + v8));
            }
        }
    }
LABEL_63:
    SubtractVectors(vector0, player.motion.velocity, (const uint32 *)v57);

    v54 = vector0[1];
    v55 = vector0[2];
    w_u32(((uint32)((a1 + 152))), vector0[0]);
    w_u32(((uint32)((a1 + 156))), v54);
    w_u32(((uint32)((a1 + 160))), v55);
    AddAngleVector((uint16 *)psx_addr((a1 + 16), 6), player.motion.angular_velocity);
    sub_8006C624((a1 + 16));
    AddAngleVector(player.motion.angular_velocity, player.motion.angular_acceleration);
    DampAngles(player.motion.angular_velocity, player.motion.angular_drag);
    return SnapSmallAngles(player.motion.angular_velocity);
}



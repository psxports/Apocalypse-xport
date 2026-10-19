#include <stdio.h>
#include <stdlib.h>

#include "level_session.h"
#include "game_tuning.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "player.h"
#include "scripted_level_object.h"
#include "collision.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"

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

uint32 ResolvePlayerCollisionMotion(Player &player)
{
    FUNCTION_MARKER(0x8003ACB8, "SLUS_003.73");
    CollisionRay collision;
    Vector3 lateral_offset, probe_origin, probe_step, probe_end;
    Vector3 ground_probe_velocity, ground_probe_scaled, ground_probe_offset;
    Vector3 initial_velocity, probe_velocity;
    Vector3 ledge_step_offset, ledge_scaled_velocity, ledge_velocity;

    sint8 probe_mask;
    sint32 area_kind;
    bool moving_outward;
    sint32 fall_velocity;
    uint32 velocity_sq[3];
    sint32 attempt;
    sint32 horizontal_speed;
    sint32 sweep_radius;
    sint32 move_speed_limit;
    sint32 probe_size;
    sint32 lateral_x;
    sint32 lateral_z;
    sint32 probe_origin_z;
    sint32 probe_origin_y;
    sint32 step_velocity_z;
    bool edge_probe_enabled;
    sint32 high_probe_y;
    sint32 velocity_x;
    sint32 normal_dot;
    sint32 normal_step;
    sint32 damped_velocity_z;
    bool allow_horizontal_motion;
    bool probe_ceiling;
    ModelInstance *ground_object;
    sint32 ceiling_probe_z;
    sint16 ceiling_contact_flags;
    sint32 ground_probe_z;
    sint32 ground_offset;
    sint32 ground_y;
    sint16 ground_contact_flags;
    sint32 slope_motion_x;
    sint32 slope_offset_x;
    sint32 slope_motion_z;
    sint32 slope_offset_z;
    sint32 cur_z;
    sint32 ledge_probe_z0;
    sint32 ledge_probe_y0;
    sint32 ledge_probe_z1;
    sint32 ledge_probe_y1;
    sint32 ledge_step;
    sint32 ledge_ray_x;
    sint32 unsupported_y;
    uint32 ledge_probe_radius;
    uint32 speed;
    uint32 ground_probe_radius;
    uint32 probe_speed;
    uint32 vertical_velocity;
    const uint32 probe_shift = 3;
    const uint32 half_shift = 1;
    probe_mask = 15;
    area_kind = level_environment.area_kind;
    player.contact_flags = 0;
    if (area_kind == 12 || area_kind == 6)
        probe_mask = 1;
    else if (area_kind == 3)
        probe_mask = 11;
    initial_velocity = player.motion.velocity;
    AddVectors(player.motion.velocity.words, player.motion.acceleration.words);
    DampVector(player.motion.velocity.words, player.motion.drag);
    SnapSmallVector(player.motion.velocity.words);
    fall_velocity = -1;
    vertical_velocity = player.motion.velocity.words[1];
    if ((sint32)vertical_velocity >= 0)
    {
        fall_velocity = (sint32)vertical_velocity;
        player.motion.velocity.words[1] = 0;
    }
    probe_velocity = player.motion.velocity;
    velocity_sq[0] = SquarePlayerVelocityComponent(player.motion.velocity.words[0]);
    velocity_sq[1] = SquarePlayerVelocityComponent(player.motion.velocity.words[1]);
    velocity_sq[2] = SquarePlayerVelocityComponent(player.motion.velocity.words[2]);
    attempt = 0;
    probe_speed = LookupDistanceRoot(velocity_sq[0] + velocity_sq[1] + velocity_sq[2]);
    do
    {
        velocity_sq[0] = SquarePlayerVelocityComponent(player.motion.velocity.words[0]);
        velocity_sq[1] = SquarePlayerVelocityComponent(player.motion.velocity.words[1]);
        velocity_sq[2] = SquarePlayerVelocityComponent(player.motion.velocity.words[2]);
        speed = LookupDistanceRoot(velocity_sq[0] + velocity_sq[1] + velocity_sq[2]);
        if (!speed)
            break;
        horizontal_speed = LookupDistanceRoot(velocity_sq[0] + velocity_sq[2]);
        if (!horizontal_speed)
            break;
        move_speed_limit = (sint32)((uint32)(sint32)(sint16)GetGameTuning().player.move_speed << 2);
        sweep_radius = player.collision_probe_size;
        if (move_speed_limit < horizontal_speed)
            sweep_radius = (sint32)((uint32)sweep_radius + DividePlayerCollisionScalar((uint32)sweep_radius * ((uint32)horizontal_speed - (uint32)move_speed_limit), move_speed_limit));
        probe_size = player.collision_probe_size;
        lateral_x = (sint32)DividePlayerCollisionScalar((uint32)probe_size * (player.motion.velocity.words[2] << 2), speed);
        lateral_z = (sint32)DividePlayerCollisionScalar((0 - (uint32)probe_size) * (player.motion.velocity.words[0] << 2), speed);
        lateral_offset.words[1] = 0;
        lateral_offset.words[0] = lateral_x;
        lateral_offset.words[2] = lateral_z;
        probe_origin.words[0] = player.model.pos.words[0];
        probe_origin_y = player.model.pos.words[1] + 0x10000;
        probe_origin_z = player.model.pos.words[2];
        probe_origin.words[1] = probe_origin_y;
        probe_origin.words[2] = probe_origin_z;
        probe_step.words[0] = (sint32)DividePlayerCollisionScalar((uint32)sweep_radius * (player.motion.velocity.words[0] << 3), horizontal_speed);
        probe_step.words[1] = (sint32)DividePlayerCollisionScalar((uint32)player.collision_probe_size * (player.motion.velocity.words[1] << 3), speed);
        step_velocity_z = (8 * player.motion.velocity.words[2]);
        collision.endpoints[0].words[0] = (probe_origin.words[0] + lateral_x);
        collision.endpoints[0].words[1] = probe_origin_y;
        collision.endpoints[0].words[2] = (probe_origin_z + lateral_z);
        collision.endpoints[1].words[0] = ((probe_origin.words[0] + lateral_x) + probe_step.words[0]);
        collision.endpoints[1].words[1] = (probe_origin_y + probe_step.words[1]);
        probe_step.words[2] = (sint32)DividePlayerCollisionScalar((uint32)sweep_radius * (uint32)step_velocity_z, horizontal_speed);
        collision.endpoints[1].words[2] = ((probe_origin_z + lateral_z) + probe_step.words[2]);
        InitializeCollisionRay(collision);
        QueryCollisionRay(collision, 1);
        ++attempt;
        if (collision.hit.GetObject())
            goto ResolveWallContact;
        collision.endpoints[0].words[0] = (probe_origin.words[0] - lateral_offset.words[0]);
        collision.endpoints[1].words[0] = ((probe_origin.words[0] - lateral_offset.words[0]) + probe_step.words[0]);
        collision.endpoints[0].words[1] = probe_origin.words[1];
        collision.endpoints[0].words[2] = (probe_origin.words[2] - lateral_offset.words[2]);
        collision.endpoints[1].words[2] = ((probe_origin.words[2] - lateral_offset.words[2]) + probe_step.words[2]);
        collision.endpoints[1].words[1] = (probe_origin.words[1] + probe_step.words[1]);
        InitializeCollisionRay(collision);
        QueryCollisionRay(collision, 1);
        if (collision.hit.GetObject())
            goto ResolveWallContact;
        edge_probe_enabled = (probe_mask & 4);
        if (!(probe_mask & 2))
            goto CheckEdgeProbes;
        probe_origin.words[0] = player.model.pos.words[0];
        probe_origin.words[2] = player.model.pos.words[2];
        high_probe_y = ((player.model.animation == 12)) ? ((player.model.pos.words[1] - 0x10000)) : ((player.model.pos.words[1] - 0x40000));
        probe_origin.words[1] = high_probe_y;

        ShiftVectorLeft(ledge_velocity.words, player.motion.velocity.words, probe_shift);
        ledge_probe_radius = (player.collision_probe_size - 16);
        MultiplyScalarVector(ledge_scaled_velocity.words, ledge_probe_radius, ledge_velocity.words);
        DivideVector(ledge_step_offset.words, ledge_scaled_velocity.words, speed);
        AddVectors(probe_end.words, probe_origin.words, ledge_step_offset.words);
        collision.endpoints[0] = probe_origin;
        collision.endpoints[1] = probe_end;
        InitializeCollisionRay(collision);
        QueryCollisionRay(collision, 1);
        edge_probe_enabled = (probe_mask & 4);
        if (collision.hit.GetObject())
        {
        ResolveWallContact:
            uint32 contact_flags = player.contact_flags;
            velocity_x = player.motion.velocity.words[0];
            player.contact_flags = contact_flags | 1;
            normal_dot = (sint32)((uint32)(velocity_x >> 6) * (uint32)(sint32)collision.n.vx + (uint32)((sint32)player.motion.velocity.words[2] >> 6) * (uint32)(sint32)collision.n.vz);
            moving_outward = (normal_dot > 0);
            normal_step = (normal_dot >> 12);
            if (!moving_outward)
            {
                player.motion.velocity.words[0] = (uint32)velocity_x - (uint32)((sint32)((uint32)normal_step * (uint32)(sint32)collision.n.vx) >> 6);
                player.motion.velocity.words[2] = player.motion.velocity.words[2] - (uint32)((sint32)((uint32)normal_step * (uint32)(sint32)collision.n.vz) >> 6);
                uint32 slide_velocity_x = player.motion.velocity.words[0];
                damped_velocity_z = (sint32)player.motion.velocity.words[2] >> 2;
                player.motion.velocity.words[0] = (uint32)((sint32)slide_velocity_x >> 2);
                player.motion.velocity.words[2] = damped_velocity_z;
            }
        }
        else
        {
        CheckEdgeProbes:
            if (edge_probe_enabled)
            {
                probe_origin.words[0] = player.model.pos.words[0];
                probe_origin.words[1] = (player.model.pos.words[1] + 0x10000);
                probe_origin.words[2] = player.model.pos.words[2];
                AddVectors(probe_end.words, probe_origin.words, lateral_offset.words);
                collision.endpoints[0] = probe_origin;
                collision.endpoints[1] = probe_end;
                InitializeCollisionRay(collision);
                QueryCollisionRay(collision, 1);
                if (!collision.hit.GetObject())
                {
                    probe_origin.words[0] = player.model.pos.words[0];
                    probe_origin.words[1] = (player.model.pos.words[1] + 0x10000);
                    probe_origin.words[2] = player.model.pos.words[2];
                    SubtractVectors(probe_end.words, probe_origin.words, lateral_offset.words);
                    collision.endpoints[0] = probe_origin;
                    collision.endpoints[1] = probe_end;
                    InitializeCollisionRay(collision);
                    QueryCollisionRay(collision, 1);
                    allow_horizontal_motion = (attempt < 3);
                    if (!collision.hit.GetObject())
                        goto ApplyResolvedMotion;
                }
                player.motion.velocity.words[0] = (player.motion.velocity.words[0] + ((16 * collision.n.vx)));
                player.motion.velocity.words[2] = (player.motion.velocity.words[2] + ((16 * collision.n.vz)));
                collision.hit.SetObject(0);
            }
        }
    } while (collision.hit.GetObject() && attempt < 3);
    allow_horizontal_motion = (attempt < 3);
ApplyResolvedMotion:
    if (allow_horizontal_motion)
        AddVectors(player.model.pos.words, player.motion.velocity.words);
    else
        player.model.pos.words[1] = (player.model.pos.words[1] + (player.motion.velocity.words[1]));

    probe_ceiling = false;
    if ((sint32)player.motion.velocity.words[1] < 0 || ((ground_object = player.ground.GetObject()) && (sint32) static_cast<VisualObject *>(ground_object->owner)->motion.velocity.words[1] < 0))
        probe_ceiling = true;
    if (probe_ceiling)
    {
        collision.endpoints[0].words[0] = player.model.pos.words[0];
        collision.endpoints[0].words[1] = player.model.pos.words[1];
        ceiling_probe_z = player.model.pos.words[2];
        collision.endpoints[1].words[0] = collision.endpoints[0].words[0];
        collision.endpoints[1].words[1] = (collision.endpoints[0].words[1] - 753664);
        collision.endpoints[0].words[2] = ceiling_probe_z;
        collision.endpoints[1].words[2] = ceiling_probe_z;
        InitializeCollisionRay(collision);
        QueryCollisionRay(collision, 1);
        if (collision.hit.GetObject())
        {
            if ((sint32)(player.model.pos.words[1] - 491520) < (sint32)collision.hit_pos.words[1])
            {
                ceiling_contact_flags = player.contact_flags;
                player.model.pos.words[1] = (collision.hit_pos.words[1] + 491520);
                player.motion.velocity.words[1] = 0;
                player.contact_flags = (ceiling_contact_flags | 0x100);
            }
        }
    }
    if (fall_velocity >= 0)
    {
        collision.endpoints[0].words[0] = player.model.pos.words[0];
        collision.endpoints[0].words[1] = (player.model.pos.words[1] - (((uint32)GetGameTuning().player.ground_probe_offset - 64) << 12));
        ground_probe_z = player.model.pos.words[2];
        collision.endpoints[1].words[0] = collision.endpoints[0].words[0];
        collision.endpoints[1].words[1] = ((collision.endpoints[0].words[1] + fall_velocity) + ((uint32)GetGameTuning().player.ground_probe_length << 12));
        collision.endpoints[0].words[2] = ground_probe_z;
        collision.endpoints[1].words[2] = ground_probe_z;
        InitializeCollisionRay(collision);
        QueryCollisionRay(collision, 1);
        if (collision.hit.GetObject())
        {
            player.SetContactColor(*collision.hit.GetPolygon(), *collision.hit.GetObject());
            if (collision.n.vy >= -3072)
            {
                slope_motion_x = (fall_velocity - (fall_velocity >> (player.motion.drag[0] & 31)));
                player.model.pos.words[1] = (collision.hit_pos.words[1] - (player.ground_offset << 12));
                slope_offset_x = (sint32)((uint32)(slope_motion_x >> 12) * (uint32)(sint32)collision.n.vx);
                slope_motion_z = (fall_velocity - (fall_velocity >> (player.motion.drag[2] & 31)));
                player.model.pos.words[0] = (player.model.pos.words[0] + (slope_offset_x));
                slope_offset_z = (sint32)((uint32)(slope_motion_z >> 12) * (uint32)(sint32)collision.n.vz);
                cur_z = player.model.pos.words[2];
                player.motion.velocity.words[1] = fall_velocity;
                player.model.pos.words[1] = (player.model.pos.words[1] + ((fall_velocity - (fall_velocity >> (player.motion.drag[1] & 31)))));
                player.model.pos.words[2] = (cur_z + slope_offset_z);
            }
            else
            {
                ground_offset = player.ground_offset;
                ground_y = collision.hit_pos.words[1];
                ground_contact_flags = player.contact_flags;
                player.motion.velocity.words[1] = 0;
                player.model.pos.words[1] = (ground_y - (ground_offset << 12));
                player.contact_flags = (ground_contact_flags | 2);
            }
            player.surface.n.vx = collision.n.vx;
            player.surface.n.vy = collision.n.vy;
            player.surface.n.vz = collision.n.vz;
            player.surface.pos = collision.hit_pos;
            if (collision.hit.GetObject()->flags & 0x100)
            {
                static_cast<ScriptedLevelObject *>(collision.hit.GetObject()->owner)->ActivateContact();
                static_cast<ScriptedLevelObject *>(collision.hit.GetObject()->owner)->ApplyPlayerDamage();
                player.ground.SetObject(collision.hit.GetObject());
            }
            else
            {
                player.ground.Reset();
            }
        }
        else
        {
            if (!probe_speed)
                goto ContinueFalling;

            ShiftVectorLeft(ground_probe_velocity.words, probe_velocity.words, probe_shift);
            ground_probe_radius = (player.collision_probe_size + 8);
            MultiplyScalarVector(ground_probe_scaled.words, ground_probe_radius, ground_probe_velocity.words);
            DivideVector(ground_probe_offset.words, ground_probe_scaled.words, probe_speed);
            collision.endpoints[0].words[0] = (player.model.pos.words[0] + ground_probe_offset.words[0]);
            collision.endpoints[0].words[1] = (player.model.pos.words[1] - ((uint32)GetGameTuning().player.ground_probe_offset << 12));
            ledge_probe_z0 = player.model.pos.words[2];
            collision.endpoints[1].words[0] = collision.endpoints[0].words[0];
            collision.endpoints[0].words[2] = (ledge_probe_z0 + ground_probe_offset.words[2]);
            ledge_probe_y0 = player.model.pos.words[1];
            collision.endpoints[1].words[2] = (ledge_probe_z0 + ground_probe_offset.words[2]);
            collision.endpoints[1].words[1] = (ledge_probe_y0 + fall_velocity);
            InitializeCollisionRay(collision);
            QueryCollisionRay(collision, 1);
            if (!collision.hit.GetObject())
            {
                ShiftVectorRight(ground_probe_offset.words, half_shift);
                collision.endpoints[0].words[0] = (player.model.pos.words[0] + ground_probe_offset.words[0]);
                collision.endpoints[0].words[1] = (player.model.pos.words[1] - ((uint32)GetGameTuning().player.ground_probe_offset << 12));
                ledge_probe_z1 = player.model.pos.words[2];
                collision.endpoints[1].words[0] = collision.endpoints[0].words[0];
                collision.endpoints[0].words[2] = (ledge_probe_z1 + ground_probe_offset.words[2]);
                ledge_probe_y1 = player.model.pos.words[1];
                collision.endpoints[1].words[2] = (ledge_probe_z1 + ground_probe_offset.words[2]);
                collision.endpoints[1].words[1] = (ledge_probe_y1 + fall_velocity);
                InitializeCollisionRay(collision);
                QueryCollisionRay(collision, 1);
                if (!collision.hit.GetObject())
                    goto ContinueFalling;
            }
            if (collision.hit.GetPolygon()->surface & 4)
            {
                if (collision.hit.GetObject()->flags & 0x100)
                    player.ground.SetObject(collision.hit.GetObject());
                else
                    player.ground.Reset();
                ledge_step = 1;
                player.contact_flags = (player.contact_flags | (0x40));
                player.ledge_pos[1] = collision.hit_pos.words[1];
                while (1)
                {
                    collision.endpoints[0].words[0] = player.model.pos.words[0];
                    collision.endpoints[0].words[1] = ((uint32)player.ledge_pos[1] + (ledge_step << 16));
                    collision.endpoints[0].words[2] = player.model.pos.words[2];
                    ledge_ray_x = player.model.pos.words[0];
                    collision.endpoints[1].words[1] = collision.endpoints[0].words[1];
                    collision.endpoints[1].words[0] = (ledge_ray_x + ground_probe_offset.words[0]);
                    collision.endpoints[1].words[2] = (player.model.pos.words[2] + ground_probe_offset.words[2]);
                    InitializeCollisionRay(collision);
                    QueryCollisionRay(collision, 1);
                    ++ledge_step;
                    if (collision.hit.GetObject())
                        break;
                    if (ledge_step >= 7)
                    {
                        goto FinalizeMotion;
                    }
                }

                player.contact_flags = (player.contact_flags | (0x80));
                player.ledge_pos[0] = collision.hit_pos.words[0];
                player.ledge_pos[2] = collision.hit_pos.words[2];
                player.ledge_normal[0] = collision.n.vx;
                player.ledge_normal[1] = collision.n.vy;
                player.ledge_normal[2] = collision.n.vz;
            }
            else
            {
            ContinueFalling:
                unsupported_y = player.model.pos.words[1];

                player.motion.velocity.words[1] = fall_velocity;
                player.model.pos.words[1] = (unsupported_y + fall_velocity);
            }
        }
    }
FinalizeMotion:
    SubtractVectors(player.motion.delta_velocity.words, player.motion.velocity.words, initial_velocity.words);
    AddAngleVector(player.model.angles.words, player.motion.angular_velocity.words);
    NormalizeAngles(player.model.angles.words);
    AddAngleVector(player.motion.angular_velocity.words, player.motion.angular_acceleration.words);
    DampAngles(player.motion.angular_velocity.words, player.motion.angular_drag);
    return SnapSmallAngles(player.motion.angular_velocity.words);
}

#include <new>
#include <stdlib.h>
#include <string.h>

#include "collision_ray.h"
#include "level_session.h"
#include "game_lighting.h"
#include "game_tuning.h"
#include "game_settings.h"
#include "game_tick.h"
#include "object_initializers.h"
#include "audio_voice.h"
#include "enemy_list.h"
#include "cheat_input.h"
#include "visual_object.h"
#include "object_damage.h"
#include "model_bank.h"
#include "game_diagnostics.h"
#include "player.h"
#include "object_force.h"
#include "ring_attack.h"
#include "beam_barrier.h"
#include "global.h"
#include "cd_voice_event.h"
#include "one_shot_activator.h"
#include "resetting_activator.h"
#include "expanding_ring.h"
#include "effect_lifecycle.h"
#include "pickup.h"
#include "game_pad_input.h"
#include "screen_flash.h"
#include "model_animation.h"
#include "model_attachment_data.h"
#include "model_matrix.h"
#include "machine_gun.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "audio_session.h"
#include "camera.h"
#include "traversal_rail.h"
#include "flexible_traversal_rail.h"
#include "weapon_type.h"
#include "weapon_node.h"
#include "homing_missiles.h"
#include "pulse_laser.h"
#include "particle_beam.h"
#include "rip_laser.h"
#include "grenades.h"
#include "rockets.h"
#include "flamethrower.h"
#include "spiral_ray_weapon.h"
#include "angle_math.h"
#include "model_attachment.h"
#include "collision.h"
#include "effect_object.h"
#include "scripted_level_object.h"

struct PlayerRuntime
{
    uint32 count;
    uint32 rail_entered;
    uint32 jump_released;
    uint32 kills_since_quote;
    uint8 quote_kind;
    uint8 quote_variant;
    uint32 last_footstep_sound;
    uint32 motion_update_seen;
    Vector3 trace_direction;
    uint32 death_wait;
    uint8 quote_history[5];
};

static PlayerRuntime runtime;
static const uint8 hurt_frames[4][2] = {{0, 22}, {23, 36}, {37, 52}, {53, 65}};

struct DirectionAnimation
{
    uint8 animation[8];
    sint8 dir[8];
    uint16 body_heading[8];
    sint16 facing[8];
};

static const DirectionAnimation dir_animations[10] = {
    {{1, 1, 3, 8, 8, 8, 4, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {0, 0, 1024, 2048, 2048, 2048, 64512, 0}, {0, 512, 0, -512, 0, 512, 0, -512}},         {{1, 1, 1, 3, 8, 8, 8, 4}, {1, 1, 1, 1, 1, 1, 1, 1}, {512, 512, 512, 1536, 64000, 64000, 64000, 65024}, {-512, 0, 512, 0, -512, 0, 512, 0}},
    {{4, 1, 1, 1, 3, 8, 8, 8}, {1, 1, 1, 1, 1, 1, 1, 1}, {0, 1024, 1024, 1024, 2048, 64512, 64512, 64512}, {0, -512, 0, 512, 0, -512, 0, 512}}, {{8, 3, 1, 1, 1, 4, 8, 8}, {1, -1, 1, 1, 1, -1, 1, 1}, {65024, 512, 1536, 1536, 1536, 64000, 65024, 65024}, {512, 0, -512, 0, 512, 0, -512, 0}},
    {{8, 8, 4, 1, 1, 1, 3, 8}, {1, 1, 1, 1, 1, 1, 1, 1}, {0, 0, 1024, 2048, 2048, 2048, 64512, 0}, {0, 512, 0, -512, 0, 512, 0, -512}},         {{8, 8, 8, 3, 1, 1, 1, 4}, {1, 1, 1, -1, 1, 1, 1, -1}, {512, 512, 512, 1536, 64000, 64000, 64000, 65024}, {-512, 0, 512, 0, -512, 0, 512, 0}},
    {{3, 8, 8, 8, 4, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {0, 1024, 1024, 1024, 2048, 64512, 64512, 64512}, {0, -512, 0, 512, 0, -512, 0, 512}}, {{1, 3, 8, 8, 8, 4, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}, {65024, 512, 1536, 1536, 1536, 64000, 65024, 65024}, {512, 0, -512, 0, 512, 0, -512, 0}},
    {{0, 0, 0, 0, 0, 0, 0, 0}, {1, 1, 1, 1, 1, 1, 1, 1}, {0, 0, 1024, 2048, 2048, 2048, 64512, 0}, {0, 512, 0, -512, 0, 512, 0, -512}},         {{5, 5, 5, 5, 5, 5, 5, 5}, {1, 1, 1, 1, 1, 1, 1, 1}, {0, 0, 1024, 2048, 2048, 2048, 64512, 0}, {0, 512, 0, -512, 0, 512, 0, -512}},
};

void Player::ResetRuntime()
{
    runtime = PlayerRuntime();
}

void Player::StopSelWeapon(uint32 sel)
{
    WeaponNode *weapon = weapons[sel];
    if (weapon)
        weapon->Update(&model.pos);
}

uint32 Player::ResolveColMotion()
{
    FUNCTION_MARKER(0x8003ACB8, "SLUS_003.73");
    CollisionRay col;
    Vector3 lateral_offset, probe_origin, probe_step, probe_end;
    Vector3 ground_probe_velocity, ground_probe_scaled, ground_probe_offset;
    Vector3 initial_velocity, probe_velocity;
    Vector3 ledge_step_offset, ledge_scaled_velocity, ledge_velocity;

    uint8 probe_mask;
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
    uint16 ceiling_contact_flags;
    sint32 ground_probe_z;
    sint32 ground_offset;
    uint32 ground_y;
    uint16 ground_contact_flags;
    sint32 slope_motion_x;
    sint32 slope_offset_x;
    sint32 slope_motion_z;
    sint32 slope_offset_z;
    uint32 cur_z;
    sint32 ledge_probe_z0;
    sint32 ledge_probe_y0;
    sint32 ledge_probe_z1;
    sint32 ledge_probe_y1;
    sint32 ledge_step;
    sint32 ledge_ray_x;
    uint32 unsupported_y;
    uint32 ledge_probe_radius;
    uint32 speed;
    uint32 ground_probe_radius;
    uint32 probe_speed;
    sint32 vertical_velocity;
    const uint32 probe_shift = 3;
    const uint32 half_shift = 1;
    probe_mask = 15;
    area_kind = level_environment.area_kind;
    this->contact_flags = 0;
    if (area_kind == 12 || area_kind == 6)
        probe_mask = 1;
    else if (area_kind == 3)
        probe_mask = 11;
    initial_velocity = motion.velocity;
    motion.velocity.Add(motion.acceleration);
    motion.velocity.Damp(motion.drag);
    motion.velocity.SnapSmall();
    fall_velocity = -1;
    vertical_velocity = motion.velocity.y;
    if (vertical_velocity >= 0)
    {
        fall_velocity = vertical_velocity;
        motion.velocity.y = 0;
    }
    probe_velocity = motion.velocity;
    velocity_sq[0] = Player::SqVelocityComponent(motion.velocity.comp[0]);
    velocity_sq[1] = Player::SqVelocityComponent(motion.velocity.comp[1]);
    velocity_sq[2] = Player::SqVelocityComponent(motion.velocity.comp[2]);
    attempt = 0;
    probe_speed = LookupDistanceRoot(velocity_sq[0] + velocity_sq[1] + velocity_sq[2]);
    do
    {
        velocity_sq[0] = Player::SqVelocityComponent(motion.velocity.comp[0]);
        velocity_sq[1] = Player::SqVelocityComponent(motion.velocity.comp[1]);
        velocity_sq[2] = Player::SqVelocityComponent(motion.velocity.comp[2]);
        speed = LookupDistanceRoot(velocity_sq[0] + velocity_sq[1] + velocity_sq[2]);
        if (!speed)
            break;
        horizontal_speed = LookupDistanceRoot(velocity_sq[0] + velocity_sq[2]);
        if (!horizontal_speed)
            break;
        sint16 configured_move_speed = GetGameTuning().player.move_speed;
        move_speed_limit = configured_move_speed * 4;
        sweep_radius = col_probe_size;
        if (move_speed_limit < horizontal_speed)
            sweep_radius = (uint32)sweep_radius + DivideSignedWord(sweep_radius * ((uint32)horizontal_speed - move_speed_limit), move_speed_limit);
        probe_size = col_probe_size;
        lateral_x = DivideSignedWord(probe_size * ((uint32)motion.velocity.z << 2), speed);
        lateral_z = DivideSignedWord((-probe_size) * ((uint32)motion.velocity.x << 2), speed);
        lateral_offset.y = 0;
        lateral_offset.x = lateral_x;
        lateral_offset.z = lateral_z;
        probe_origin.x = model.pos.x;
        probe_origin_y = (uint32)model.pos.y + 0x10000;
        probe_origin_z = model.pos.z;
        probe_origin.y = probe_origin_y;
        probe_origin.z = probe_origin_z;
        probe_step.x = DivideSignedWord((uint32)sweep_radius * ((uint32)motion.velocity.x << 3), horizontal_speed);
        probe_step.y = DivideSignedWord(col_probe_size * ((uint32)motion.velocity.y << 3), speed);
        step_velocity_z = 8 * (uint32)motion.velocity.z;
        col.endpoints[0].x = (uint32)probe_origin.x + lateral_x;
        col.endpoints[0].y = probe_origin_y;
        col.endpoints[0].z = probe_origin_z + lateral_z;
        col.endpoints[1].x = ((uint32)probe_origin.x + lateral_x) + (uint32)probe_step.x;
        col.endpoints[1].y = probe_origin_y + (uint32)probe_step.y;
        probe_step.z = DivideSignedWord((uint32)sweep_radius * step_velocity_z, horizontal_speed);
        col.endpoints[1].z = (probe_origin_z + lateral_z) + (uint32)probe_step.z;
        col.Initialize();
        col.Query(1);
        ++attempt;
        if (col.hit.GetObject())
            goto ResolveWallContact;
        col.endpoints[0].x = (uint32)probe_origin.x - lateral_offset.x;
        col.endpoints[1].x = ((uint32)probe_origin.x - lateral_offset.x) + (uint32)probe_step.x;
        col.endpoints[0].y = probe_origin.y;
        col.endpoints[0].z = (uint32)probe_origin.z - lateral_offset.z;
        col.endpoints[1].z = ((uint32)probe_origin.z - lateral_offset.z) + (uint32)probe_step.z;
        col.endpoints[1].y = (uint32)probe_origin.y + probe_step.y;
        col.Initialize();
        col.Query(1);
        if (col.hit.GetObject())
            goto ResolveWallContact;
        edge_probe_enabled = probe_mask & 4;
        if (!(probe_mask & 2))
            goto CheckEdgeProbes;
        probe_origin.x = model.pos.x;
        probe_origin.z = model.pos.z;
        high_probe_y = (model.animation == 12) ? ((uint32)model.pos.y - 0x10000) : ((uint32)model.pos.y - 0x40000);
        probe_origin.y = high_probe_y;

        ledge_velocity.ShiftLeft(motion.velocity, probe_shift);
        ledge_probe_radius = col_probe_size - 16;
        ledge_scaled_velocity.Scale(ledge_probe_radius, ledge_velocity);
        ledge_step_offset.Divide(ledge_scaled_velocity, speed);
        probe_end.Add(probe_origin, ledge_step_offset);
        col.endpoints[0] = probe_origin;
        col.endpoints[1] = probe_end;
        col.Initialize();
        col.Query(1);
        edge_probe_enabled = probe_mask & 4;
        if (col.hit.GetObject())
        {
        ResolveWallContact:
            uint16 contact_flags = this->contact_flags;
            velocity_x = motion.velocity.x;
            this->contact_flags = contact_flags | 1;
            normal_dot = (uint32)(velocity_x >> 6) * col.n.vx + (uint32)(motion.velocity.z >> 6) * col.n.vz;
            moving_outward = normal_dot > 0;
            normal_step = normal_dot >> 12;
            if (!moving_outward)
            {
                sint32 correction_x = (uint32)normal_step * col.n.vx;
                motion.velocity.x = (uint32)velocity_x - (correction_x >> 6);
                sint32 correction_z = (uint32)normal_step * col.n.vz;
                motion.velocity.z = (uint32)motion.velocity.z - (correction_z >> 6);
                sint32 slide_velocity_x = motion.velocity.x;
                damped_velocity_z = motion.velocity.z >> 2;
                motion.velocity.x = slide_velocity_x >> 2;
                motion.velocity.z = damped_velocity_z;
            }
        }
        else
        {
        CheckEdgeProbes:
            if (edge_probe_enabled)
            {
                probe_origin.x = model.pos.x;
                probe_origin.y = (uint32)model.pos.y + 0x10000;
                probe_origin.z = model.pos.z;
                probe_end.Add(probe_origin, lateral_offset);
                col.endpoints[0] = probe_origin;
                col.endpoints[1] = probe_end;
                col.Initialize();
                col.Query(1);
                if (!col.hit.GetObject())
                {
                    probe_origin.x = model.pos.x;
                    probe_origin.y = (uint32)model.pos.y + 0x10000;
                    probe_origin.z = model.pos.z;
                    probe_end.Subtract(probe_origin, lateral_offset);
                    col.endpoints[0] = probe_origin;
                    col.endpoints[1] = probe_end;
                    col.Initialize();
                    col.Query(1);
                    allow_horizontal_motion = attempt < 3;
                    if (!col.hit.GetObject())
                        goto ApplyResolvedMotion;
                }
                motion.velocity.x = (uint32)motion.velocity.x + (16 * col.n.vx);
                motion.velocity.z = (uint32)motion.velocity.z + (16 * col.n.vz);
                col.hit.SetObject(0);
            }
        }
    } while (col.hit.GetObject() && attempt < 3);
    allow_horizontal_motion = attempt < 3;
ApplyResolvedMotion:
    if (allow_horizontal_motion)
        model.pos.Add(motion.velocity);
    else
        model.pos.y = (uint32)model.pos.y + motion.velocity.y;

    probe_ceiling = false;
    if (motion.velocity.y < 0 || ((ground_object = ground.GetObject()) && static_cast<VisualObject *>(ground_object->owner)->motion.velocity.y < 0))
        probe_ceiling = true;
    if (probe_ceiling)
    {
        col.endpoints[0].x = model.pos.x;
        col.endpoints[0].y = model.pos.y;
        ceiling_probe_z = model.pos.z;
        col.endpoints[1].x = col.endpoints[0].x;
        col.endpoints[1].y = (uint32)col.endpoints[0].y - 753664;
        col.endpoints[0].z = ceiling_probe_z;
        col.endpoints[1].z = ceiling_probe_z;
        col.Initialize();
        col.Query(1);
        if (col.hit.GetObject())
        {
            if ((sint32)((uint32)model.pos.y - 491520) < col.hit_pos.y)
            {
                ceiling_contact_flags = this->contact_flags;
                model.pos.y = (uint32)col.hit_pos.y + 491520;
                motion.velocity.y = 0;
                this->contact_flags = ceiling_contact_flags | 0x100;
            }
        }
    }
    if (fall_velocity >= 0)
    {
        col.endpoints[0].x = model.pos.x;
        col.endpoints[0].y = (uint32)model.pos.y - (((uint32)GetGameTuning().player.ground_probe_offset - 64) << 12);
        ground_probe_z = model.pos.z;
        col.endpoints[1].x = col.endpoints[0].x;
        col.endpoints[1].y = ((uint32)col.endpoints[0].y + fall_velocity) + ((uint32)GetGameTuning().player.ground_probe_len << 12);
        col.endpoints[0].z = ground_probe_z;
        col.endpoints[1].z = ground_probe_z;
        col.Initialize();
        col.Query(1);
        if (col.hit.GetObject())
        {
            SetContactColor(*col.hit.GetPolygon(), *col.hit.GetObject());
            if (col.n.vy >= -3072)
            {
                slope_motion_x = fall_velocity - (fall_velocity >> (motion.drag[0] & 31));
                model.pos.y = (uint32)col.hit_pos.y - (this->ground_offset << 12);
                slope_offset_x = (uint32)(slope_motion_x >> 12) * col.n.vx;
                slope_motion_z = fall_velocity - (fall_velocity >> (motion.drag[2] & 31));
                model.pos.x = (uint32)model.pos.x + slope_offset_x;
                slope_offset_z = (uint32)(slope_motion_z >> 12) * col.n.vz;
                cur_z = model.pos.z;
                motion.velocity.y = fall_velocity;
                model.pos.y = (uint32)model.pos.y + (fall_velocity - (fall_velocity >> (motion.drag[1] & 31)));
                model.pos.z = cur_z + slope_offset_z;
            }
            else
            {
                ground_offset = this->ground_offset;
                ground_y = col.hit_pos.y;
                ground_contact_flags = this->contact_flags;
                motion.velocity.y = 0;
                model.pos.y = ground_y - (ground_offset << 12);
                this->contact_flags = ground_contact_flags | 2;
            }
            surface.n.vx = col.n.vx;
            surface.n.vy = col.n.vy;
            surface.n.vz = col.n.vz;
            surface.pos = col.hit_pos;
            if (col.hit.GetObject()->flags & 0x100)
            {
                static_cast<ScriptedLevelObject *>(col.hit.GetObject()->owner)->ActivateContact();
                static_cast<ScriptedLevelObject *>(col.hit.GetObject()->owner)->ApplyPlayerDamage();
                ground.SetObject(col.hit.GetObject());
            }
            else
            {
                ground.Reset();
            }
        }
        else
        {
            if (!probe_speed)
                goto ContinueFalling;

            ground_probe_velocity.ShiftLeft(probe_velocity, probe_shift);
            ground_probe_radius = col_probe_size + 8;
            ground_probe_scaled.Scale(ground_probe_radius, ground_probe_velocity);
            ground_probe_offset.Divide(ground_probe_scaled, probe_speed);
            col.endpoints[0].x = (uint32)model.pos.x + ground_probe_offset.x;
            col.endpoints[0].y = (uint32)model.pos.y - ((uint32)GetGameTuning().player.ground_probe_offset << 12);
            ledge_probe_z0 = model.pos.z;
            col.endpoints[1].x = col.endpoints[0].x;
            col.endpoints[0].z = ledge_probe_z0 + (uint32)ground_probe_offset.z;
            ledge_probe_y0 = model.pos.y;
            col.endpoints[1].z = ledge_probe_z0 + (uint32)ground_probe_offset.z;
            col.endpoints[1].y = ledge_probe_y0 + fall_velocity;
            col.Initialize();
            col.Query(1);
            if (!col.hit.GetObject())
            {
                ground_probe_offset.ShiftRight(half_shift);
                col.endpoints[0].x = (uint32)model.pos.x + ground_probe_offset.x;
                col.endpoints[0].y = (uint32)model.pos.y - ((uint32)GetGameTuning().player.ground_probe_offset << 12);
                ledge_probe_z1 = model.pos.z;
                col.endpoints[1].x = col.endpoints[0].x;
                col.endpoints[0].z = ledge_probe_z1 + (uint32)ground_probe_offset.z;
                ledge_probe_y1 = model.pos.y;
                col.endpoints[1].z = ledge_probe_z1 + (uint32)ground_probe_offset.z;
                col.endpoints[1].y = ledge_probe_y1 + fall_velocity;
                col.Initialize();
                col.Query(1);
                if (!col.hit.GetObject())
                    goto ContinueFalling;
            }
            if (col.hit.GetPolygon()->surface & 4)
            {
                if (col.hit.GetObject()->flags & 0x100)
                    ground.SetObject(col.hit.GetObject());
                else
                    ground.Reset();
                ledge_step = 1;
                this->contact_flags = this->contact_flags | 0x40;
                ledge_pos[1] = (uint32)col.hit_pos.y;
                while (1)
                {
                    col.endpoints[0].x = model.pos.x;
                    col.endpoints[0].y = (uint32)ledge_pos[1] + (ledge_step << 16);
                    col.endpoints[0].z = model.pos.z;
                    ledge_ray_x = model.pos.x;
                    col.endpoints[1].y = col.endpoints[0].y;
                    col.endpoints[1].x = ledge_ray_x + (uint32)ground_probe_offset.x;
                    col.endpoints[1].z = (uint32)model.pos.z + ground_probe_offset.z;
                    col.Initialize();
                    col.Query(1);
                    ++ledge_step;
                    if (col.hit.GetObject())
                        break;
                    if (ledge_step >= 7)
                    {
                        goto FinalizeMotion;
                    }
                }

                this->contact_flags = this->contact_flags | 0x80;
                ledge_pos[0] = (uint32)col.hit_pos.x;
                ledge_pos[2] = (uint32)col.hit_pos.z;
                ledge_normal[0] = col.n.vx;
                ledge_normal[1] = col.n.vy;
                ledge_normal[2] = col.n.vz;
            }
            else
            {
            ContinueFalling:
                unsupported_y = model.pos.y;

                motion.velocity.y = fall_velocity;
                model.pos.y = unsupported_y + fall_velocity;
            }
        }
    }
FinalizeMotion:
    motion.delta_velocity.Subtract(motion.velocity, initial_velocity);
    model.angles.AddAngles(motion.angular_velocity);
    model.angles.NormalizeAngles();
    motion.angular_velocity.AddAngles(motion.angular_acceleration);
    motion.angular_velocity.DampAngles(motion.angular_drag);
    return motion.angular_velocity.SnapAngles();
}

void Player::SaveTransform()
{
    FUNCTION_MARKER(0x8005C580, "SLUS_003.73");
    Player &player = *static_cast<Player *>(visual_lists.players->owner);
    player.saved_pos = player.model.pos;
    player.saved_angles = player.model.angles;
}

Player::Player() : Character(), facing_target(0), facing_step(0), facing_ticks(0), pad(0), death_camera_enabled(0), disable_timer(0), weapon_stop_ticks(0), movement_state(0), update_disabled(0), hit_reaction_ticks(0), damage_glow_ticks(0), analog_aim(0), aim_press_ticks(0), move_heading(0), aim_heading(0), fall_start_y(0), fall_damage_threshold(0), fatal_fall_distance(0), rail_voice(0), on_rail(0), smartbombs(0), fire_ticks(0), jump_velocity(0), jump_hold_ticks(0), jump_latch(0), airborne_ticks(0), weapons_enabled(0), slowdown_ticks(0), slowdown_duration(0), ground_grace_ticks(0), idle_ticks(0), ground_offset(0), col_probe_size(0), action_heading(0), move_ramp(0), sel_weapon(0), prev_weapon(0), death_ticks(0)
{
    FUNCTION_MARKER(0x8005C5C0, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        muzzle_pos.comp[axis] = 0;
        saved_surface_pos.comp[axis] = 0;
        rail_pos.comp[axis] = 0;
        saved_pos.comp[axis] = 0;
        saved_angles.comp[axis] = 0;
        ledge_pos[axis] = 0;
        ledge_normal[axis] = 0;
    }
    move_input[0] = move_input[1] = 0;
    aim_input[0] = aim_input[1] = 0;
    WeaponNode *new_weapon = 0;
    for (uint32 slot = 0; slot < 8; ++slot)
        weapons[slot] = 0;
    GameHeapBlock *weapon;
    uint32 weapon_damage;
    bool difficulty;
    runtime.kills_since_quote = 0;
    runtime.rail_entered = 0;
    runtime.jump_released = 0;
    AddToList(visual_lists.players);
    col_radius = 100;
    ++runtime.count;
    lifecycle_flags &= 0xFFFD;
    pad = &game_pads[0];
    model.flags |= 0x80;
    health = GetGameTuning().player.maximum_health;
    SetModelBank("bruce");
    kind = 50;
    motion.drag[0] = 1;
    motion.drag[2] = 1;
    motion.drag[1] = GetGameTuning().player.vertical_drag;
    motion.angular_drag[0] = 5;
    motion.angular_drag[1] = 1;
    motion.angular_drag[2] = 5;
    ground_offset = GetGameTuning().player.ground_offset;
    col_radius = 100;
    death_camera_enabled = 1;
    projection.enabled = true;
    col_probe_size = GetGameTuning().player.col_probe_size;
    projection.dir.x = 0;
    projection.dir.y = -ONE;
    projection.dir.z = 0;
    projection.color = 0x808080;
    InitializeAnimation();
    if (visual_lists.cameras)
        static_cast<Camera *>(visual_lists.cameras->owner)->mode = 3;
    weapon = AllocateZeroedHeapBlock(88);
    if (weapon)
    {
        new_weapon = new (weapon) MachineGun(&visual_lists.actors);
    }
    weapon_damage = 5;
    difficulty = game_settings.difficulty != 0;
    weapons[0] = new_weapon;
    if (!difficulty)
        weapon_damage = 7;
    new_weapon->damage = weapon_damage;
    sel_weapon = 0;
    prev_weapon = 1;
    smartbombs = 3;
    model.SetAttachmentTable(PlayerAttachments);
    InitializeModelAnimation(PlayerHierarchy);
}

uint32 Player::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8005C7C0, "SLUS_003.73");
    projection.enabled = false;
    RemoveFromList(visual_lists.players);
    --runtime.count;
    Player::ReleaseResources();
    Character::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : 0;
}

void Player::StopWeapon()
{
    FUNCTION_MARKER(0x8005C868, "SLUS_003.73");
    StopSelWeapon(sel_weapon);
}

uint32 Player::TryPlayKillQuote()
{
    FUNCTION_MARKER(0x8005C8C4, "SLUS_003.73");
    uint32 count, total, random, now, prev_time, level, variant_count;
    uint8 kind, variant, sel;
    count = runtime.kills_since_quote;
    total = game_progress.level.enemy_kills;
    runtime.kills_since_quote = count + 1;
    game_progress.level.enemy_kills = total + 1;
    random = RandomRange(2);
    count = runtime.kills_since_quote;
    sint32 threshold = random + 3;
    sint32 kills = count;
    if (threshold >= kills)
        return 0;
    now = game_clock.vblank_frame;
    prev_time = cd_voice.last_frame;
    if (now - prev_time < 900)
        return 1;
    runtime.kills_since_quote = 0;
    for (;;)
    {
        level = level_environment.area_kind;
        if (level == 1)
        {
            random = RandomRange(4);
            kind = random ? 3 : 2;
            variant_count = random ? 6 : 8;
        }
        else if (level == 2)
        {
            random = RandomRange(4);
            if (random)
            {
                kind = 4;
                variant_count = 4;
            }
            else
            {
                random = RandomRange(2);
                kind = random ? 3 : 2;
                variant_count = random ? 6 : 8;
            }
        }
        else if (level == 0)
        {
            kind = 2;
            variant_count = 8;
        }
        else
        {
            random = RandomRange(4);
            if (random)
            {
                kind = 5;
                variant_count = 7;
            }
            else
            {
                random = RandomRange(3);
                if (!random)
                {
                    kind = 4;
                    variant_count = 4;
                }
                else
                {
                    random = RandomRange(3);
                    kind = random ? 2 : 3;
                    variant_count = random ? 8 : 6;
                }
            }
        }
        runtime.quote_kind = kind;
        variant = RandomRange(variant_count);
        runtime.quote_variant = variant;
        kind = runtime.quote_kind;
        variant = runtime.quote_variant;
        sel = (kind << 3) + variant;
        bool repeated = false;
        for (uint32 idx = 0; idx < 5; ++idx)
        {
            if (runtime.quote_history[idx] == sel)
            {
                repeated = true;
                break;
            }
        }
        if (repeated)
            continue;
        for (uint32 idx = 0; idx < 4; ++idx)
            runtime.quote_history[idx] = runtime.quote_history[idx + 1];
        runtime.quote_history[4] = sel;
        kind = runtime.quote_kind;
        variant = runtime.quote_variant;
        return PlayCdVoiceEvent(kind, variant, &model.pos);
    }
}

uint32 Player::ReadInput()
{
    FUNCTION_MARKER(0x8005CB1C, "SLUS_003.73");
    uint32 disabled = disable_timer;
    move_input[0] = 0;
    move_input[1] = 0;
    aim_input[0] = 0;
    aim_input[1] = 0;
    analog_aim = 1;
    if (disabled)
    {
        aim_press_ticks = 0;
        return 1;
    }

    GamePadInput *input = pad;
    uint8 pad_type = input->type;
    if (pad_type == 115 || pad_type == 83)
    {
        move_input[0] = input->axis[0];
        input = pad;
        move_input[1] = input->axis[1];
        aim_input[0] = input->axis[2];
        aim_input[1] = input->axis[3];
    }
    if (!(move_input[0] | move_input[1]))
    {
        input = pad;
        if (input->buttons[10].down)
        {
            move_input[0] = 128;
            input = pad;
        }
        else if (input->buttons[11].down)
        {
            move_input[0] = 127;
            input = pad;
        }
        if (input->buttons[9].down)
            move_input[1] = 127;
        else if (input->buttons[8].down)
            move_input[1] = 128;
    }

    uint16 factor = slowdown_ticks;
    if (factor)
    {
        sint32 axis = (sint8)move_input[0];
        uint16 divisor = slowdown_duration;
        sint32 quotient = DivideSignedWord(factor * axis, divisor);
        move_input[0] = axis - quotient;
        axis = (sint8)move_input[1];
        factor = slowdown_ticks;
        divisor = slowdown_duration;
        quotient = DivideSignedWord(factor * axis, divisor);
        move_input[1] = axis - quotient;
    }

    if (aim_input[0] | aim_input[1])
        aim_press_ticks = 0;
    else
    {
        input = pad;
        analog_aim = 0;
        if (input->buttons[0].down)
            aim_input[0] = 128;
        else if (input->buttons[3].down)
            aim_input[0] = 127;
        input = pad;
        if (input->buttons[2].down)
        {
            aim_input[1] = 127;
            input = pad;
        }
        else if (input->buttons[1].down)
        {
            aim_input[1] = 128;
            input = pad;
        }
        if (input->buttons[0].down || input->buttons[3].down || input->buttons[2].down || input->buttons[1].down)
        {
            ++aim_press_ticks;
            if (aim_press_ticks >= 2)
                aim_press_ticks = 2;
        }
        else
            aim_press_ticks = 0;
    }

    sint32 x = (sint8)move_input[0];
    sint32 y = (sint8)move_input[1];
    if (x | y)
        move_heading = (1024 - ratan2(-x, y)) & 4095;
    x = (sint8)aim_input[0];
    y = (sint8)aim_input[1];
    uint32 result = aim_input[0] | aim_input[1];
    if (result)
    {
        result = ratan2(-x, y);
        aim_heading = (1024 - result) & 4095;
    }
    return result;
}

uint32 Player::RestoreHealth(sint16 amount)
{
    FUNCTION_MARKER(0x8005CE70, "SLUS_003.73");
    health += amount;
    sint16 maximum = GetGameTuning().player.maximum_health;
    if (maximum < health)
        health = maximum;
    uint32 sound = RandomRange(2);
    PlayCdVoiceEvent(1, sound, &model.pos);
    return 1;
}

uint32 Player::UpdateLighting()
{
    FUNCTION_MARKER(0x8005CF0C, "SLUS_003.73");
    ShortVector3 dir = {{{0, -ONE, 0}}};
    LightColor light_color = {ONE, ONE, ONE};
    LightColor surface_color;
    uint32 color = model.color;
    sint32 r = color & 255;
    sint32 g = (color >> 8) & 255;
    sint32 b = (color >> 16) & 255;
    uint8 intensity = color >> 24;
    if (intensity >= 1 && intensity <= 127)
    {
        sint32 scale = 0x80000 / intensity;
        r = (r * scale) >> 12;
        g = (g * scale) >> 12;
        b = (b * scale) >> 12;
    }
    surface_color.r = r * 16;
    surface_color.g = g * 16;
    surface_color.b = b * 16;
    WeaponNode *weapon = weapons[sel_weapon];
    uint8 light_r, light_g, light_b;
    if (weapon && weapon->ReadLight(&light_r, &light_g, &light_b))
    {
        Vector3 difference, shifted, normal;
        difference.Subtract(muzzle_pos, model.pos);
        shifted.ShiftRight(difference, 12);
        normal.Normalize(shifted);
        dir.x = normal.x;
        dir.y = 0;
        dir.z = normal.z;
        light_color.r = light_r * 16;
        light_color.g = light_g * 16;
        light_color.b = light_b * 16;
    }
    else
    {
        uint8 fallback_intensity = model.color >> 24;
        sint16 fallback_color = fallback_intensity < 64 ? 1024 : fallback_intensity * 16;
        light_color.r = fallback_color;
        light_color.g = fallback_color;
        light_color.b = fallback_color;
    }
    Camera *camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    uint16 heading = camera ? (uint16)camera->angles.y & 4095 : 0;
    sint16 s = rsin(heading);
    sint16 c = rcos(heading);
    ShortVector3 up = {{{0, ONE, 0}}};
    ShortVector3 camera_dir = {{{s, 0, c}}};
    SetLightingDirs(dir, up, camera_dir);
    SetLightingColors(light_color, surface_color, surface_color);
    game_lighting.background_color.r = 64;
    game_lighting.background_color.g = 64;
    game_lighting.background_color.b = 64;
    return 64;
}

uint32 Player::SetContactColor(const ModelPolygon &polygon, const ModelInstance &surface)
{
    FUNCTION_MARKER(0x8005D20C, "SLUS_003.73");
    uint16 flags = polygon.flags;
    uint8 maximum = 0;
    uint32 color;
    if (flags & 2048)
    {
        const ModelPalette *pal = &ModelBank::Get(surface.bank).colors;
        uint32 count = (flags & 16) ? 3 : 4;
        // Human-approved repair selects the first vertex when all colors are black
        uint8 sel = polygon.color;
        for (uint32 idx = 0; idx < count; ++idx)
        {
            uint8 vertex = polygon.color >> (idx * 8);
            color = ReadModelPalColor(pal, vertex) & 0xFFFFFF;
            uint8 component = color;
            if (maximum < component)
            {
                maximum = component;
                sel = vertex;
            }
            component = color >> 8;
            if (maximum < component)
            {
                maximum = component;
                sel = vertex;
            }
            component = color >> 16;
            if (maximum < component)
            {
                maximum = component;
                sel = vertex;
            }
        }
        color = ReadModelPalColor(pal, sel);
    }
    else
    {
        color = polygon.color & 0xFFFFFF;
        maximum = color;
        uint8 component = color >> 8;
        if (maximum < component)
            maximum = component;
        component = color >> 16;
        if (maximum < component)
            maximum = component;
    }
    uint32 result = color + ((uint32)maximum << 24);
    model.color = result;
    return result;
}

uint32 Player::UpdateWeapons(const ShortVector3 &rotation, bool enabled)
{
    FUNCTION_MARKER(0x8005D360, "SLUS_003.73");
    // TODO Original cheat allocation failure writes through a null weapon
    WeaponNode *new_weapon = 0;
    sint32 result;
    bool weapon_activity;
    GameHeapBlock *ring_storage;
    GameHeapBlock *weapon_storage;
    uint32 weapon_damage;
    GamePadInput *input;
    bool not_easy;
    uint32 prev_slot;
    uint32 start_slot;
    uint32 next_slot;
    uint32 wrapped_slot;
    uint32 cone;
    uint32 weapon_type;
    bool firing_blocked;
    uint32 fire_result;
    uint32 fire_slot;
    WeaponNode *active_weapon;
    uint16 actuator_ticks;
    uint8 actuator;
    uint8 actuator_val;
    WeaponNode *fired_weapon;
    uint32 slot;
    uint32 depleted_slot;
    WeaponNode *idle_weapon;
    WeaponNode *other_weapon;
    Vector3 weapon_pos;
    ShortVector3 aim_angles;
    if (!HasModelJointAngles())
        return 0;
    weapon_activity = false;
    if (pad->buttons[18].down && movement_state != 0x40000 && !active_ring_attack && !disable_timer && smartbombs)
    {
        ring_storage = EffectObject::AllocateStorage(172);
        if (ring_storage)
        {
            new (ring_storage) RingAttack(100, 10, 8192);
        }
        if (game_settings.vibration == VibrationFull)
            GamePadInput::SetActuator(0, 12, 1, 120);
        if (!GetCheatOptions().weapon_cheat && smartbombs)
            --smartbombs;
        PlayCdVoiceEvent(0, 2, &model.pos);
    }
    input = pad;
    if (input->buttons[19].pressed)
    {
        input->buttons[19].pressed = false;
        if (GetCheatOptions().weapon_cheat)
        {
            PlaySoundEffect(26, 8192, 0);
            switch (weapons[sel_weapon]->type)
            {
                case WPN_FLAMETHROWER:
                    if (!weapons[2])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(68);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) PulseLaser(&visual_lists.actors);
                        }
                        weapon_damage = 10;
                        not_easy = game_settings.difficulty != 0;
                        weapons[2] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 15;
                        new_weapon->damage = weapon_damage;
                    }
                    prev_weapon = 1;
                    sel_weapon = 2;
                    break;

                case WPN_PULSE_LASER:
                    if (!weapons[3])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(104);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) ParticleBeam(&visual_lists.actors, 3000, 0, 256, 9);
                        }
                        weapon_damage = 10;
                        not_easy = game_settings.difficulty != 0;
                        weapons[3] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 15;
                        new_weapon->damage = weapon_damage;
                    }
                    prev_weapon = 2;
                    sel_weapon = 3;
                    break;

                case WPN_PARTICLE_BEAM:
                    if (!weapons[4])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(100);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) RipLaser(&visual_lists.actors);
                        }
                        weapon_damage = 2;
                        not_easy = game_settings.difficulty != 0;
                        weapons[4] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 3;
                        new_weapon->damage = weapon_damage;
                    }
                    prev_weapon = 3;
                    sel_weapon = 4;
                    break;

                case WPN_RIP_LASER:
                    if (!weapons[5])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(68);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) Grenades(&visual_lists.actors);
                        }
                        weapon_damage = 120;
                        not_easy = game_settings.difficulty != 0;
                        weapons[5] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 180;
                        new_weapon->damage = weapon_damage;
                    }
                    prev_weapon = 4;
                    sel_weapon = 5;
                    break;

                case WPN_GRENADES:
                    if (!weapons[6])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(68);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) HomingMissiles(&visual_lists.actors);
                        }
                        weapon_damage = 15;
                        not_easy = game_settings.difficulty != 0;
                        weapons[6] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 22;
                        new_weapon->damage = weapon_damage;
                    }
                    prev_weapon = 5;
                    sel_weapon = 6;
                    break;

                case WPN_HOMING_MISSILES:
                    if (!weapons[7])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(68);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) Rockets(&visual_lists.actors);
                        }
                        weapon_damage = 40;
                        not_easy = game_settings.difficulty != 0;
                        weapons[7] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 60;
                        new_weapon->damage = weapon_damage;
                    }
                    prev_weapon = 6;
                    sel_weapon = 7;
                    break;

                case WPN_ROCKETS:
                    prev_weapon = 7;
                    sel_weapon = 0;
                    break;

                case WPN_MACHINE_GUN:
                    if (!weapons[1])
                    {
                        weapon_storage = AllocateZeroedHeapBlock(72);
                        if (weapon_storage)
                        {
                            new_weapon = new (weapon_storage) Flamethrower(&visual_lists.actors, 0);
                        }
                        weapon_damage = 10;
                        not_easy = game_settings.difficulty != 0;
                        weapons[1] = new_weapon;
                        if (!not_easy)
                            weapon_damage = 15;
                        new_weapon->damage = weapon_damage;
                    }
                    prev_weapon = 0;
                    sel_weapon = 1;
                    break;

                default:
                    break;
            }
        }
        else
        {
            start_slot = sel_weapon;
            next_slot = start_slot + 1;
            if ((sint32)(start_slot + 1) < 8)
            {
                while (!weapons[next_slot])
                {
                    ++next_slot;
                    if ((sint32)next_slot >= 8)
                        goto WrapWeaponSelection;
                }

                PlaySoundEffect(26, 8192, 0);
                prev_slot = sel_weapon;
                sel_weapon = next_slot;
                prev_weapon = prev_slot;
            }
        WrapWeaponSelection:
            if (start_slot == sel_weapon && start_slot)
            {
                PlaySoundEffect(26, 8192, 0);
                wrapped_slot = sel_weapon;
                sel_weapon = 0;
                prev_weapon = wrapped_slot;
            }
        }
    }
    GetModelAttachment(weapon_pos, *this, 0);
    firing_blocked = (movement_state & 0xF3800) || !(aim_input[0] | aim_input[1]) || !enabled;
    if (firing_blocked)
    {
        idle_weapon = weapons[sel_weapon];
        if (idle_weapon)
            idle_weapon->Update(&weapon_pos);
        fire_ticks = 0;
        weapon_pos.Add(motion.velocity);
        goto BeginOtherWeapons;
    }
    idle_ticks = 0;
    ++fire_ticks;
    aim_angles = rotation;
    if (!weapons[sel_weapon])
        goto MarkWeaponActivity;
    muzzle_pos = weapon_pos;
    weapon_type = weapons[sel_weapon]->type;
    if (weapon_type == WPN_GRENADES || weapon_type == WPN_PULSE_LASER || weapon_type == WPN_ROCKETS || weapon_type == WPN_MACHINE_GUN)
    {
        cone = 768;
        if ((game_pads[0].buttons[0].down || game_pads[0].buttons[3].down) && (game_pads[0].buttons[1].down || game_pads[0].buttons[2].down))
            cone = 1536;
        WeaponNode::SelectAimTarget(aim_angles, weapon_pos, rotation, 4096, cone, 3072, 0, 0, visual_lists.actors);
    }
    fire_result = 0;

    if (fire_ticks < 2)
    {
        active_weapon = weapons[sel_weapon];
        active_weapon->Update(&weapon_pos);
    }
    else
    {
        fire_slot = sel_weapon;
        fire_ticks = 1;
        active_weapon = weapons[fire_slot];
        fire_result = active_weapon->Fire(weapon_pos, aim_angles);
    }
    if (game_settings.vibration)
    {
        switch (weapon_type)
        {
            case WPN_FLAMETHROWER:
                if (game_settings.vibration == VibrationFull && !hit_reaction_ticks)
                {
                    actuator_ticks = 4;
                    actuator = 1;
                    actuator_val = 60;
                    goto ApplyWeaponVibration;
                }
                break;

            case WPN_PULSE_LASER:
                if (fire_result && game_settings.vibration == VibrationFull && !hit_reaction_ticks)
                {
                    actuator_ticks = 2;
                    goto SetImpulseActuator;
                }
                break;

            case WPN_PARTICLE_BEAM:

            case WPN_RIP_LASER:
                if (game_settings.vibration == VibrationFull && !hit_reaction_ticks)
                {
                    actuator_ticks = 2;
                    goto SetImpulseActuator;
                }
                break;

            case WPN_GRENADES:

            case WPN_HOMING_MISSILES:

            case WPN_ROCKETS:
                if (fire_result && game_settings.vibration == VibrationFull && !hit_reaction_ticks)
                {
                    actuator_ticks = 4;
                SetImpulseActuator:
                    actuator = 0;

                    actuator_val = 1;
                    goto ApplyWeaponVibration;
                }
                break;

            case WPN_MACHINE_GUN:
                if (game_settings.vibration == VibrationFull && !hit_reaction_ticks)
                {
                    actuator_ticks = 4;
                    actuator = 1;
                    actuator_val = 120;
                ApplyWeaponVibration:
                    GamePadInput::SetActuator(0, actuator_ticks, actuator, actuator_val);
                }
                break;

            default:
                break;
        }
    }
    fired_weapon = weapons[sel_weapon];
    weapon_activity = true;
    if (fired_weapon && fired_weapon->type == WPN_MACHINE_GUN)
        goto BeginOtherWeapons;
    slot = 0;
    if (fired_weapon && (sint32)fired_weapon->ammo > 0)
        goto UpdateOtherWeapons;
    if (fired_weapon)
    {
        fired_weapon->Destroy(3);
    }
    weapons[sel_weapon] = 0;
    depleted_slot = sel_weapon;
    sel_weapon = 0;
    prev_weapon = depleted_slot;
MarkWeaponActivity:
    weapon_activity = true;

BeginOtherWeapons:
    slot = 0;

UpdateOtherWeapons:

    do
    {
        if (slot != sel_weapon)
        {
            other_weapon = weapons[slot];
            if (other_weapon)
                other_weapon->Update(&weapon_pos);
        }
        ++slot;
    } while (slot < 8);
    // Historical unconsumed V0 carrier from the original data-page load
    result = -2146500608;
    if (weapon_activity)
    {
        result = GetGameTuning().weapon_activity;
        game_session.weapon_activity = result;
    }
    return result;
}

uint32 Player::Finish()
{
    FUNCTION_MARKER(0x8005DD40, "SLUS_003.73");
    uint32 result = IsFinished();
    if (!result)
    {
        result = lifecycle_flags;
        uint16 flags = model.flags;
        result |= 0x40;
        flags |= 1;
        lifecycle_flags = result;
        model.flags = flags;
    }
    return result;
}

uint32 Player::ReleaseResources()
{
    FUNCTION_MARKER(0x8005DD84, "SLUS_003.73");
    for (uint32 idx = 0; idx < 8; ++idx)
        if (weapons[idx])
        {
            weapons[idx]->Destroy(3);
            weapons[idx] = 0;
        }
    return 0;
}

uint32 Player::Damage(uint32 damage, const Vector3 *vector, uint32 reason)
{
    FUNCTION_MARKER(0x8005DDF0, "SLUS_003.73");
    if (GetCheatOptions().invulnerable)
        return 0;
    if (health <= 0)
        return 0;
    if (VisualObject::IsFinished())
        return 0;
    if (disable_timer)
        return 0;
    if (weapon_stop_ticks)
        return 0;
    Vector3 impulse;
    impulse.ShiftRight(vector ? *vector : zero_vector, 1);
    motion.velocity.Add(impulse);
    if ((sint32)damage >= 2)
    {
        sint32 maximum = (sint16)GetGameTuning().player.maximum_health;
        sint32 threshold = maximum / 5;
        if (health < threshold)
            damage >>= 1;
        if (!game_settings.difficulty)
            damage >>= 1;
    }
    health -= damage;
    if (health <= 0)
    {
        health = 0;
        EnterDeathState();
        GamePadInput::ClearActuator(0, 0);
        GamePadInput::ClearActuator(0, 1);
        return 1;
    }
    hit_reaction_ticks = 6;
    if (reason == 5 || reason == 31)
    {
        damage_glow_ticks = 90;
        SetColor(90, 255, 0, 0);
        model_effect.scale = 10240;
        uint32 intensity = damage_glow_ticks;
        model_effect.depth_bias = 10;
        model_effect.color_mode = (intensity << 1) | (intensity << 9) | 0x2000000 | (intensity << 17);
    }
    if (game_settings.vibration)
    {
        GamePadInput::SetActuator(0, 4, 0, 1);
        GamePadInput::SetActuator(1, 4, 1, 255);
    }
    SetScrFlash(128, 0, 0, 8, 0, 0);
    uint32 sound = RandomRange(4);
    PlaySoundEffect(sound + 4, 8192, 0);
    if (!model.animation)
    {
        uint32 range = RandomRange(4);
        uint8 first_frame = hurt_frames[range][0];
        uint8 last_frame = hurt_frames[range][1];
        SetAnimationRange(6, first_frame, last_frame);
    }
    return 1;
}

Vector3 *Player::ApplyContactImpulse(const VisualObject &object)
{
    Vector3 difference, shifted, scaled, divided, impulse;
    FUNCTION_MARKER(0x8005E07C, "SLUS_003.73");
    difference.Subtract(model.pos, object.model.pos);
    difference.y = 0;
    uint32 distance = difference.Len();
    if (!distance)
        distance = 1;
    shifted.ShiftRight(difference, 12);
    scaled.Scale(object.contact_strength, shifted);
    divided.Divide(scaled, distance);
    impulse.ShiftLeft(divided, 12);
    motion.velocity.Add(impulse);
    return &motion.velocity;
}

uint32 Player::ApplyBeamContactImpulse(const BeamBarrier &barrier)
{
    FUNCTION_MARKER(0x8005E154, "SLUS_003.73");
    sint32 dz = (uint32)barrier.target.z - barrier.model.pos.z;
    sint32 dx = (uint32)barrier.target.x - barrier.model.pos.x;
    uint16 beam_angle = (1024 - ratan2(dz, dx)) & 4095;
    dz = (uint32)pos_history.prev.z - barrier.model.pos.z;
    dx = (uint32)pos_history.prev.x - barrier.model.pos.x;
    uint16 player_angle = (1024 - ratan2(dz, dx)) & 4095;
    bool subtract = beam_angle < player_angle ? player_angle - beam_angle < 2049 : beam_angle - player_angle >= 2049;
    ShortVector3 angles;
    angles.x = 0;
    angles.y = (subtract ? beam_angle - 1024 : beam_angle + 1024) & 4095;
    angles.z = 0;
    Vector3 impulse;
    impulse.SetDir(512, angles);
    motion.velocity.Add(impulse);
    slowdown_ticks = 60;
    slowdown_duration = 60;
    return 60;
}

uint32 Player::ReplaceWeapon(uint16 kind)
{
    FUNCTION_MARKER(0x8005E260, "SLUS_003.73");
    GameHeapBlock *object;
    uint32 weapon_damage;
    uint32 prev;
    WeaponNode *new_weapon = 0;
    WeaponNode **slot;
    if (kind < 1 || kind > 7)
        return 0;
    if (kind == 1)
        PlayCdVoiceEvent(0, 3, &model.pos);
    else
        PlayCdVoiceEvent(0, RandomRange(2), &model.pos);
    slot = &weapons[kind];
    if (*slot)
        (*slot)->Destroy(3);
    switch (kind)
    {
        case WPN_FLAMETHROWER:
            object = AllocateZeroedHeapBlock(0x48);
            if (object)
            {
                new_weapon = new (object) Flamethrower(&visual_lists.actors, 0);
            }
            weapon_damage = game_settings.difficulty ? 10 : 15;
            break;
        case WPN_PULSE_LASER:
            object = AllocateZeroedHeapBlock(0x44);
            if (object)
            {
                new_weapon = new (object) PulseLaser(&visual_lists.actors);
            }
            weapon_damage = game_settings.difficulty ? 10 : 15;
            break;
        case WPN_PARTICLE_BEAM:
            object = AllocateZeroedHeapBlock(0x68);
            if (object)
            {
                new_weapon = new (object) ParticleBeam(&visual_lists.actors, 3000, 0, 256, 9);
            }
            weapon_damage = game_settings.difficulty ? 10 : 15;
            break;
        case WPN_RIP_LASER:
            object = AllocateZeroedHeapBlock(0x64);
            if (object)
            {
                new_weapon = new (object) RipLaser(&visual_lists.actors);
            }
            weapon_damage = game_settings.difficulty ? 2 : 3;
            break;
        case WPN_GRENADES:
            object = AllocateZeroedHeapBlock(0x44);
            if (object)
            {
                new_weapon = new (object) Grenades(&visual_lists.actors);
            }
            weapon_damage = game_settings.difficulty ? 120 : 180;
            break;
        case WPN_HOMING_MISSILES:
            object = AllocateZeroedHeapBlock(0x44);
            if (object)
            {
                new_weapon = new (object) HomingMissiles(&visual_lists.actors);
            }
            weapon_damage = game_settings.difficulty ? 15 : 22;
            break;
        default:
            object = AllocateZeroedHeapBlock(0x44);
            if (object)
            {
                new_weapon = new (object) Rockets(&visual_lists.actors);
            }
            weapon_damage = game_settings.difficulty ? 40 : 60;
            break;
    }
    *slot = new_weapon;
    // TODO Original allocation failure leaves no weapon for the damage store
    new_weapon->damage = weapon_damage;
    prev = sel_weapon;
    sel_weapon = kind;
    prev_weapon = prev;
    return 1;
}

uint32 Player::EnterDeathState()
{
    FUNCTION_MARKER(0x8005E708, "SLUS_003.73");
    PlaySoundEffect(8, 0x2000, 0);
    PrepareDeathCamera();
    uint32 state = movement_state;
    uint8 animation;
    health = 0;
    switch (state)
    {
        case 1:
        case 16:
        case 32:
        case 256:
        case 512:
        case 1024:
        case 2048:
            animation = 15;
            break;
        case 2:
        case 4:
        case 8:
        case 128:
        case 4096:
        case 8192:
        case 16384:
        case 32768:
        case 65536:
        case 131072:
        case 524288:
            animation = 16;
            break;
        case 262144:
            return 0x80000;
        default:
            return Finish();
    }
    motion.velocity = zero_vector;
    movement_state = 0x40000;
    return SetAnimationRange(animation, 0, -1);
}

void Player::PrepareDeathCamera()
{
    uint32 mode;
    uint16 angle;
    Camera *camera;
    sint16 height;
    FUNCTION_MARKER(0x8005E90C, "SLUS_003.73");
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    if (!camera)
        return;
    if (!death_camera_enabled)
        return;
    death_ticks = 0;
    camera->SetOrbitDistanceTransition(64, 32);
    mode = level_environment.area_kind;
    height = mode == 10 ? -1400 : -700;
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    camera->SetOrbitHeightTransition(height, 32);
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    camera->SetTargetLateralOffsetTransition(0, 32);
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    camera->SetTargetHeightOffsetTransition(0, 32);
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    camera->SetTargetForwardOffsetTransition(0, 32);
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    angle = camera->angles.y;
    camera->SetYawTransition((angle + 1024) & 0xFFF, 34);
}

uint32 Player::InitializeAnimation()
{
    FUNCTION_MARKER(0x8005E9E4, "SLUS_003.73");
    SetAnimationRange(0, 0, -1);
    movement_state = 1;
    return 1;
}

uint32 Player::TryEnterMovementState16()
{
    FUNCTION_MARKER(0x8005EA1C, "SLUS_003.73");
    if ((sint8)move_input[0] >= 0)
        return 0;
    uint16 angle = move_heading;
    if (angle >= 768 && angle <= 3328)
        return 0;
    uint8 animation = model.animation;
    movement_state = 16;
    if (animation != 1)
        StartAnimation(1, 1);
    return 1;
}

uint32 Player::TryEnterMovementState32()
{
    FUNCTION_MARKER(0x8005EA80, "SLUS_003.73");
    if ((sint8)move_input[0] <= 0)
        return 0;
    uint16 angle = move_heading;
    if (angle < 1281 || angle >= 2816)
        return 0;
    uint8 animation = model.animation;
    movement_state = 32;
    if (animation != 1)
        StartAnimation(1, 1);
    return 1;
}

uint32 Player::TryEnterMovementState512()
{
    FUNCTION_MARKER(0x8005EAF0, "SLUS_003.73");
    if ((sint8)move_input[1] >= 0)
        return 0;
    uint16 angle = move_heading;
    if (angle < 2816 || angle >= 3329)
        return 0;
    uint8 animation = model.animation;
    movement_state = 512;
    if (animation != 1)
        StartAnimation(1, 1);
    return 1;
}

uint32 Player::TryEnterMovementState1024()
{
    FUNCTION_MARKER(0x8005EB60, "SLUS_003.73");
    if ((sint8)move_input[1] <= 0)
        return 0;
    uint16 angle = move_heading;
    if (angle < 768 || angle >= 1281)
        return 0;
    uint8 animation = model.animation;
    movement_state = 1024;
    if (animation != 1)
        StartAnimation(1, 1);
    return 1;
}

uint32 Player::TryActivateOneShotActivator()
{
    ObjectBorrow borrow;
    FUNCTION_MARKER(0x8005EBD0, "SLUS_003.73");
    if (!one_shot_activator)
        return 0;
    if (!(aim_input[0] | aim_input[1]))
        return 0;
    motion.velocity = zero_vector;
    motion.acceleration = zero_vector;
    model.angles = one_shot_activator->model.angles;
    SetAnimationRange(7, 0, -1);
    movement_state = 65536;
    if (!one_shot_activator)
    {
        GameDiagnostic(("TODO Interaction expired during player activation\n"));
        abort();
    }
    one_shot_activator->Activate();
    return 1;
}

uint32 Player::TryActivateResettingActivator()
{
    ObjectBorrow borrow;
    FUNCTION_MARKER(0x8005EC7C, "SLUS_003.73");
    if (!resetting_activator)
        return 0;
    if (!(aim_input[0] | aim_input[1]))
        return 0;
    motion.velocity = zero_vector;
    motion.acceleration = zero_vector;
    model.angles = resetting_activator->model.angles;
    SetAnimationRange(18, 0, -1);
    movement_state = 131072;
    if (!resetting_activator)
    {
        GameDiagnostic(("TODO Interaction expired during player activation\n"));
        abort();
    }
    resetting_activator->BeginActivation();
    return 1;
}

uint32 Player::TryEnterActionState()
{
    GamePadInput *input;
    uint32 state, heading;
    uint16 angle, flags;
    sint32 sound;
    Camera *camera;
    FUNCTION_MARKER(0x8005ED30, "SLUS_003.73");
    input = pad;
    if (!input->buttons[16].down)
        return 0;
    state = movement_state;
    if (state == 256)
    {
        if (!(move_input[0] | move_input[1]))
            return 0;
        angle = move_heading;
        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
        action_heading = angle;
        heading = camera ? (uint16)camera->angles.y + angle : angle;
        model.angles.y = heading & 4095;
        flags = model.flags;
        movement_state = 2048;
        sound = (flags & 8) ? 55 : 0;
        PlaySoundEffect(sound, 8192, 0);
        SetAnimationRange(12, 6, -1);
        return 1;
    }
    if (!(state & 1584))
    {
        flags = model.flags & 8;
        movement_state = 256;
        if (flags)
            PlaySoundEffect(72, 8192, 0);
        SetAnimationRange(5, 0, -1);
        return 1;
    }
    if (!(move_input[0] | move_input[1]))
        return 0;
    angle = move_heading;
    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
    action_heading = angle;
    heading = camera ? (uint16)camera->angles.y + angle : angle;
    model.angles.y = heading & 4095;
    movement_state = 2048;
    SetAnimationRange(12, 0, -1);
    sound = (model.flags & 8) ? 55 : 0;
    PlaySoundEffect(sound, 8192, 0);
    return 1;
}

uint32 Player::TryEnterRecMovementState()
{
    FUNCTION_MARKER(0x8005EEDC, "SLUS_003.73");
    if (!rail || runtime.rail_entered)
        return 0;
    if (!rail->Project(model.pos, rail_pos))
        return 0;
    model.pos = rail_pos;
    model.pos.y = (uint32)model.pos.y + (524288);
    uint16 kind = rail->kind;
    if (kind != 400 && kind != 401)
        return 0;
    movement_state = kind == 400 ? 32768 : 16384;
    motion.velocity.y = 0;
    model.angles.y = (rail->heading + 2048) & 4095;
    pad->buttons[17].pressed = false;
    on_rail = 1;
    runtime.rail_entered = 1;
    if (kind == 400)
        rail_voice = PlaySoundEffect(100, 8192, 0);
    return 1;
}

uint32 Player::TryEnterJumpState()
{
    FUNCTION_MARKER(0x8005F00C, "SLUS_003.73");
    if (!pad->buttons[17].pressed)
        return 0;
    if (jump_latch)
        return 0;
    PlaySoundEffect(5, 8192, 0);
    if (model.flags & 8)
        PlaySoundEffect(63, 8192, 0);
    ++jump_latch;
    sint32 impulse = GetGameTuning().player.jump_impulse;
    ModelInstance *linked = ground.GetObject();
    impulse *= -ONE;
    jump_velocity = impulse;
    if (linked)
    {
        uint32 velocity = (uint32) static_cast<VisualObject *>(linked->owner)->motion.velocity.y;
        if ((sint32)velocity < -262144)
            velocity <<= 1;
        jump_velocity = impulse + velocity;
    }
    movement_state = 2;
    sint16 first = GetGameTuning().player.jump_first_frame;
    sint16 last = GetGameTuning().player.jump_last_frame;
    SetAnimationRange(2, first + 1, last);
    uint16 flags = contact_flags & 1;
    jump_hold_ticks = GetGameTuning().player.jump_hold_ticks;
    runtime.jump_released = 0;
    if (flags)
        move_ramp = 0;
    return 1;
}

uint32 Player::TryEnterAirborneState()
{
    FUNCTION_MARKER(0x8005F124, "SLUS_003.73");
    if (!(contact_flags & 2) && ground_grace_ticks)
        --ground_grace_ticks;
    if (ground_grace_ticks)
        return 0;
    fall_start_y = (uint32)model.pos.y;
    sint16 first = GetGameTuning().player.jump_last_frame;
    sint16 last = GetGameTuning().player.airborne_last_frame;
    SetAnimationRange(2, first + 1, last);
    movement_state = 4;
    return 1;
}

uint32 Player::TryEnterLandingState()
{
    FUNCTION_MARKER(0x8005F1B0, "SLUS_003.73");
    if (!(contact_flags & 2))
        return 0;
    uint32 threshold = fall_damage_threshold;
    ground_grace_ticks = 4;
    sint32 distance = 0;
    if (threshold)
        distance = (sint32)((uint32)model.pos.y - fall_start_y) >> 12;
    if (threshold && (sint32)threshold < distance)
    {
        sint32 numerator = (distance - threshold) * (sint16)GetGameTuning().player.maximum_health;
        sint32 denominator = fatal_fall_distance - threshold;
        uint32 damage = DivideSignedWord(numerator, denominator);
        Damage(damage, &zero_vector, 0);
        if (health <= 0)
            return 1;
    }
    else if (game_settings.vibration)
        GamePadInput::SetActuator(0, 4, 0, 1);
    SetAnimationRange(2, GetGameTuning().player.airborne_last_frame + 1, -1);
    sint32 sound = (model.flags & 8) ? 56 : 19;
    PlaySoundEffect(sound, 8192, 0);
    GamePadInput *input = pad;
    movement_state = 8;
    input->buttons[17].pressed = false;
    runtime.rail_entered = 0;
    return 1;
}

uint32 Player::TryEnterAnimation10State()
{
    FUNCTION_MARKER(0x8005F300, "SLUS_003.73");
    if (!(contact_flags & 64))
        return 0;
    SetAnimationRange(10, 1, -1);
    uint16 flags = contact_flags & 128;
    movement_state = 4096;
    if (!flags)
    {
        model.pos.y = (uint32)ledge_pos.y + 524288;
        return 1;
    }
    Vector3 scaled, pos, offset;
    scaled.Multiply(ledge_normal, 64);
    pos.Add(ledge_pos, scaled);
    model.pos = pos;
    sint32 angle_z = ledge_normal.z;
    sint32 angle_x = ledge_normal.x;
    model.pos.y = (uint32)model.pos.y + (524288);
    model.angles.y = (1024 - ratan2(angle_z, angle_x)) & 4095;
    offset.Scale(128, ledge_normal);
    pos.Subtract(model.pos, offset);
    pos.y = (uint32)pos.y - (884736);
    if (!ground.GetObject())
    {
        Camera *camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
        camera->SetTargetTransition(pos, 36);
    }
    return 1;
}

uint32 Player::UpdateGroundContact()
{
    FUNCTION_MARKER(0x8005F470, "SLUS_003.73");
    uint32 disabled = movement_state & 0x3000;
    if (disabled)
    {
        projection.enabled = false;
        return disabled;
    }
    if (model.flags & 8)
    {
        Vector3 water;
        water.x = model.pos.x;
        projection.enabled = true;
        water.y = level_environment.water_y;
        water.z = model.pos.z;
        surface.n.vx = 0;
        surface.n.vy = -ONE;
        surface.n.vz = 0;
        surface.pos = water;
        return -ONE;
    }
    if (contact_flags & 2)
    {
        projection.enabled = true;
        uint32 offset = ground_offset << 12;
        surface.pos.y = (uint32)model.pos.y + offset;
        return offset;
    }

    CollisionRay ray;
    ray.endpoints[0] = model.pos;
    ray.endpoints[0].y = (uint32)ray.endpoints[0].y - (0x40000);
    ray.endpoints[1] = ray.endpoints[0];
    ray.endpoints[1].y = (uint32)ray.endpoints[1].y + (0x800000);
    ray.Initialize();
    ray.Query(1);
    if (!ray.hit.GetObject())
    {
        projection.enabled = false;
        return 1;
    }
    projection.enabled = true;
    surface.pos = ray.hit_pos;
    surface.n.vx = ray.n.vx;
    surface.n.vy = ray.n.vy;
    surface.n.vz = ray.n.vz;
    return (uint16)ray.n.vz;
}

void Player::SetWeaponStop(uint32 duration)
{
    uint32 sel;
    FUNCTION_MARKER(0x8005F62C, "SLUS_003.73");
    sel = sel_weapon;
    weapon_stop_ticks = duration;
    StopSelWeapon(sel);
}

uint32 Player::Update()
{
    GamePadInput *cheat_pad;
    GamePadInput *rail_pad;
    GamePadInput *disabled_pad;
    GamePadInput *climb_pad;
    ObjectBorrow borrow;
    Camera *transition_camera;
    uint32 trig_angle;
    sint8 movement_x, movement_z;
    Camera *movement_camera;
    ShortVector3 camera_angles, weapon_angles;
    uint8 aim_axis0, aim_axis1;
    uint16 body_heading, model_heading;
    FUNCTION_MARKER(0x8005F688, "SLUS_003.73");

    sint32 rail_step;
    uint32 result;
    uint32 transition_result;
    ModelInstance *prev_ground;
    uint8 initial_animation;
    sint32 prev_vertical_velocity;
    uint32 script_motion_mask;
    uint32 area_kind;
    uint16 dry_model_flags;
    uint32 water_damage;
    sint32 ripple_radius;
    GameHeapBlock *ripple_storage;
    uint32 ripple_steps;
    uint32 ripple_size;
    uint32 free_move_y;
    uint32 free_move_z;
    uint16 motion_contacts;
    sint8 animation_start;
    uint32 activation_target_z;
    sint32 ramp_up;
    uint32 movement_state;
    sint32 move_octant;
    uint16 idle_ticks;
    uint32 idle_quote0;
    uint32 idle_quote1;
    bool jump_ready;
    ModelInstance *jump_platform;
    sint32 jump_impulse;
    uint32 next_state;
    uint32 activator_pos_x;
    uint32 activator_target_x;
    uint32 activator_pos_z;
    uint32 reset_pos_x;
    uint32 reset_target_x;
    uint32 reset_pos_z;
    uint16 death_tick_after;
    uint8 rail_animation;
    uint32 rail_frame3;
    uint32 rail_frame33;
    uint32 rail_frame33_event;
    uint8 air_animation;
    bool restart_air_animation;
    bool climb_complete;
    sint32 ledge_ground_y;
    uint16 ledge_contact_flags;
    sint32 move_axis;
    sint8 crouch_frame;
    sint32 aim_octant;
    uint16 move_angle;
    sint16 camera_yaw;
    uint16 aim_angle;
    sint32 move_idx;
    sint32 aim_idx;
    uint32 facing_state;
    uint16 facing_angle;
    bool immediate_facing;
    uint16 desired_heading;
    uint8 dir_animation;
    sint8 animation_dir;
    uint8 idle_animation;
    uint32 acceleration_state;
    bool velocity_changed;
    uint32 velocity_state;
    sint16 rail_forward_speed;
    sint16 rail_heading;
    sint16 rail_backward_speed;
    sint32 move_strength;
    uint32 rail_state;
    uint32 rail_sound;
    bool platform_moved;
    ModelInstance *air_platform;
    uint32 air_platform_velocity_x;
    uint32 platform_carried_x;
    ModelInstance *platform_z_owner;
    ModelInstance *ground_platform;
    uint32 ground_velocity_x;
    ModelInstance *platform_y_owner;
    uint32 platform_carried_y;
    sint32 footstep_sound;
    uint8 footstep_animation;
    uint32 footstep_crossed;
    uint32 footstep_event;
    sint32 played_footstep_sound;
    sint16 footstep_frame;
    uint8 footstep_animation_id;
    uint32 slowdown_intensity;
    uint32 slowdown_g;
    uint16 slowdown_remaining;
    ModelInstance *contact;
    ModelInstance *pickup;
    uint32 cleanup_state;
    Vector3 move_work;
    MATRIX inverse_camera_matrix;
    MATRIX camera_matrix;
    Vector3 camera_velocity, world_velocity, rail_point;
    Vector3 platform_pos, frame_delta, trace_dir;
    CollisionRay platform_ray, motion_ray;
    Vector3 trace_offset, trace_start;
    const uint32 ledge_offset_scale = 128;
    const uint32 velocity_shift = 6;
    const uint32 trace_offset_scale = 16;
    if (weapon_stop_ticks)
        return --weapon_stop_ticks;
    prev_ground = ground.GetObject();
    if (prev_ground && ((static_cast<VisualObject *>(prev_ground->owner)->lifecycle_flags & 0x40) != 0))
        ground.Reset();
    col_query.allow_surface20 = false;
    if (hit_reaction_ticks)
        --hit_reaction_ticks;
    if (damage_glow_ticks)
    {
        --damage_glow_ticks;
        if (damage_glow_ticks)
        {
            uint32 intensity = damage_glow_ticks;
            model_effect.color_mode = intensity | (intensity << 8) | 0x2000000 | (intensity << 16);
        }
        else
            model_effect.color_mode = 0;
    }
    if (disable_timer)
    {
        disabled_pad = pad;
        --disable_timer;
        disabled_pad->buttons[8].down = false;
        pad->buttons[9].down = false;
        pad->buttons[10].down = false;
        pad->buttons[11].down = false;
        pad->buttons[0].down = false;
        pad->buttons[2].down = false;
        pad->buttons[3].down = false;
        pad->buttons[1].down = false;
        pad->buttons[15].down = false;
        pad->buttons[4].down = false;
        pad->buttons[5].down = false;
        pad->buttons[6].down = false;
        pad->buttons[7].down = false;
        pad->buttons[17].pressed = false;
        pad->buttons[5].pressed = false;
    }
    initial_animation = model.animation;
    playback.speed = 0x10000;
    if ((initial_animation == 5) && (playback.dir == 1))
        playback.speed = 0x20000;
    if (GetCheatOptions().enemy_finish_cheat)
    {
        cheat_pad = pad;
        if (cheat_pad->buttons[5].pressed)
        {
            cheat_pad->buttons[5].pressed = false;
            VisualObject::FinishSelEnemies();
        }
    }
    result = update_disabled;
    if (!result)
    {
        model.flags = model.flags | 4;
        pos_history.prev = model.pos;
        script_motion_mask = this->movement_state & 0xF000;
        prev_vertical_velocity = motion.velocity.y;
        model.color = 8421504;
        if (!script_motion_mask)
            ResolveColMotion();
        dry_model_flags = model.flags & 0xFFF7;
        area_kind = level_environment.area_kind;
        model.flags = dry_model_flags;
        bool water_contact = false;
        if (area_kind == 1 || area_kind == 5)
        {
            uint32 offset = ground_offset;
            uint32 height = model.pos.y;
            sint32 surface = level_environment.water_y;
            water_contact = surface < (sint32)(height + (offset << 12));
        }
        if (water_contact)
        {
            water_damage = level_environment.water_damage;
            model.flags = dry_model_flags | 8;
            if (!water_damage)
            {
                if ((game_clock.tick & 3) == 0)
                {
                    sint32 surface = level_environment.water_y;
                    move_work = model.pos;
                    move_work.y = surface;
                    ripple_radius = RandomRange(32) + 32;
                    trig_angle = RandomRange(4096) & 0xFFF;
                    move_work.x = (uint32)move_work.x + (ripple_radius * rsin(trig_angle));
                    move_work.z = (uint32)move_work.z + (ripple_radius * rcos(trig_angle));
                    ripple_storage = EffectObject::AllocateStorage(116);
                    if (ripple_storage)
                    {
                        ripple_size = RandomRange(32) + 32;
                        ripple_steps = RandomRange(4);
                        new (ripple_storage) ExpandingRing(&move_work, ripple_size, 16, 64, 16, 12, ripple_steps + 4);
                    }
                }
            }
            else
            {
                Damage(water_damage, &zero_vector, 0);
            }
        }
        if ((GetCheatOptions().free_movement && game_pads[0].buttons[4].down) && game_pads[0].buttons[6].down)
        {
            free_move_y = pos_history.prev.y;
            model.pos.y = free_move_y;
            if (game_pads[0].buttons[0].down)
                model.pos.y = free_move_y - 393216;
            if (game_pads[0].buttons[3].down)
                model.pos.y = (uint32)model.pos.y + 393216;
            free_move_z = (uint32)model.pos.z + (5 * ((uint32)model.pos.z - pos_history.prev.z));
            model.pos.x = (uint32)model.pos.x + (5 * ((uint32)model.pos.x - pos_history.prev.x));
            model.pos.z = free_move_z;
        }
        motion_contacts = contact_flags;
        col_radius = 100;
        model.angles.z = 0;
        model.angles.x = 0;
        if (!(motion_contacts & 2))
        {
            ++airborne_ticks;
        }
        else
        {
            airborne_ticks = 0;
            jump_latch = 0;
        }
        if (((!GetCheatOptions().free_movement && (model.animation == 17)) && playback.complete) && !fall_damage_threshold)
            EnterDeathState();
        ReadInput();
        if (move_input[0] | move_input[1])
        {
            ramp_up = move_ramp;
            if (ramp_up < 8)
                move_ramp = ramp_up + 1;
        }
        else
        {
            if (move_ramp)
                --move_ramp;
        }
        movement_state = this->movement_state;
        weapons_enabled = 1;
        if (movement_state == 512)
        {
            this->idle_ticks = 0;
            transition_result = TryEnterMovementState16();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterMovementState32();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterMovementState1024();
            goto CheckActionTransitions;
        }
        if (movement_state < 0x201)
        {
            if (movement_state == 8)
            {
                this->idle_ticks = 0;
                transition_result = TryEnterMovementState16();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterMovementState32();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterMovementState512();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterMovementState1024();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterJumpState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterAirborneState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterActionState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                move_octant = 0;
                if (!playback.complete)
                    goto SelectDirectionalAnimation;
            ReturnToIdleAnimation:
                InitializeAnimation();

                move_octant = 0;
                goto SelectDirectionalAnimation;
            }
            if (movement_state < 9)
            {
                if (movement_state != 2)
                {
                    if (movement_state >= 3)
                    {
                        if (movement_state == 4)
                        {
                            this->idle_ticks = 0;
                            if (airborne_ticks < 49 || model.animation != 2)
                            {
                                air_animation = model.animation;
                                if (air_animation != 17)
                                {
                                    restart_air_animation = false;
                                    if (((motion.velocity.y ^ prev_vertical_velocity) < 0) || (air_animation != 2))
                                        restart_air_animation = true;
                                    if (restart_air_animation)
                                    {
                                        fall_start_y = (uint32)model.pos.y;
                                        SetAnimationRange(2, GetGameTuning().player.jump_last_frame + 1, GetGameTuning().player.airborne_last_frame);
                                        motion.drag[1] = GetGameTuning().player.airborne_drag;
                                    }
                                }
                            }
                            else
                            {
                                SetAnimationRange(17, 0, -1);
                            }
                            transition_result = TryEnterLandingState();
                            move_octant = 0;
                            if (!transition_result)
                            {
                                transition_result = TryEnterRecMovementState();
                                move_octant = 0;
                                if (!transition_result)
                                {
                                    TryEnterAnimation10State();
                                    move_octant = 0;
                                }
                            }
                        }
                        else
                        {
                            move_octant = 0;
                        }
                    }
                    else
                    {
                        move_octant = 0;
                        if (movement_state == 1)
                        {
                            transition_result = TryEnterMovementState16();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryEnterMovementState32();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryEnterMovementState512();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryEnterMovementState1024();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryEnterJumpState();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryEnterAirborneState();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryEnterActionState();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryActivateOneShotActivator();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            transition_result = TryActivateResettingActivator();
                            move_octant = 0;
                            if (transition_result)
                                goto SelectDirectionalAnimation;
                            if (playback.complete)
                                SetAnimationRange(0, 0, -1);
                            idle_ticks = this->idle_ticks + 1;
                            this->idle_ticks = idle_ticks;
                            if (idle_ticks == 1350)
                            {
                                idle_quote0 = RandomRange(8);
                                ::SelectCdVoice(8, idle_quote0);
                                move_octant = 0;
                                goto SelectDirectionalAnimation;
                            }
                            move_octant = 0;
                            if (idle_ticks == 4050)
                            {
                                idle_quote1 = RandomRange(8);
                                PlayCdVoiceEvent(8, idle_quote1, &model.pos);
                                this->idle_ticks = 1350;
                            ClearMovementOctant:
                                move_octant = 0;
                                goto SelectDirectionalAnimation;
                            }
                        }
                    }
                    goto SelectDirectionalAnimation;
                }
                this->idle_ticks = 0;
                transition_result = TryEnterRecMovementState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                if (TryEnterAnimation10State())
                    goto ClearMovementOctant;
                if (!pad->buttons[17].down)
                    runtime.jump_released = 1;
                next_state = 4;
                if (jump_hold_ticks)
                {
                    move_octant = 0;
                    if (pad->buttons[17].down)
                        goto SelectDirectionalAnimation;
                    next_state = 4;
                }
            ApplyMovementState:
                this->movement_state = next_state;

                goto ClearMovementOctant;
            }
            if (movement_state == 32)
            {
                this->idle_ticks = 0;
                transition_result = TryEnterMovementState512();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterMovementState1024();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterJumpState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterAirborneState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterActionState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryActivateOneShotActivator();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryActivateResettingActivator();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                move_axis = (sint8)move_input[0];
                goto CheckMovementInput;
            }
            if (movement_state < 0x21)
            {
                move_octant = 0;
                if (movement_state != 16)
                    goto SelectDirectionalAnimation;
                this->idle_ticks = 0;
                transition_result = TryEnterMovementState512();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterMovementState1024();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterJumpState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterAirborneState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryEnterActionState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryActivateOneShotActivator();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                transition_result = TryActivateResettingActivator();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                move_axis = (sint8)move_input[0];
            CheckMovementInput:
                move_octant = 0;

                if (move_axis)
                    goto SelectDirectionalAnimation;
                goto ReturnToIdleAnimation;
            }
            if (movement_state == 128)
            {
                jump_ready = playback.complete;
                this->idle_ticks = 0;
                if (jump_ready)
                {
                    jump_platform = ground.GetObject();
                    jump_impulse = -GetGameTuning().player.jump_impulse * ONE;
                    jump_velocity = jump_impulse;
                    if (jump_platform)
                        jump_velocity = jump_impulse + (uint32) static_cast<VisualObject *>(jump_platform->owner)->motion.velocity.y;
                    this->movement_state = 2;
                    jump_hold_ticks = GetGameTuning().player.jump_hold_ticks;
                }
                goto ClearMovementOctant;
            }
            move_octant = 0;
            if (movement_state != 256)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterJumpState();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            this->idle_ticks = 0;
            col_radius = 50;
            transition_result = TryEnterActionState();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            animation_start = 21;
            if (pad->buttons[16].down)
                goto SelectDirectionalAnimation;
            if (((sint8)model.frame) < 21)
                animation_start = model.frame;
        ReverseCrouchAnimation:
            SetAnimationRange(5, animation_start, 0);

            next_state = 1;
            goto ApplyMovementState;
        }
        if (movement_state == 0x4000)
        {
            rail_animation = model.animation;
            this->idle_ticks = 0;
            if (rail_animation != 13)
                goto ClearMovementOctant;
            rail_frame3 = DidAnimationCrossFrame(13, 3);
            if (UpdateEventLatch(rail_frame3, 1) || ((rail_frame33 = DidAnimationCrossFrame(13, 33)), (rail_frame33_event = UpdateEventLatch(rail_frame33, 1)), (move_octant = 0), rail_frame33_event))
            {
                static_cast<FlexibleTraversalRail *>(rail.Get())->ApplyRandomOffset(64);
                move_octant = 0;
            }
            goto SelectDirectionalAnimation;
        }
        if (movement_state < 0x4001)
        {
            if (movement_state == 2048)
            {
                transition_result = TryEnterJumpState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                this->idle_ticks = 0;
                col_radius = 50;
                transition_result = TryEnterAirborneState();
                move_octant = 0;
                if (transition_result)
                    goto SelectDirectionalAnimation;
                crouch_frame = model.frame;
                if ((crouch_frame < 21) && (pad->buttons[16].down || (crouch_frame < 11)))
                    goto SelectDirectionalAnimation;
                next_state = 256;
                if (pad->buttons[16].down)
                    goto ApplyMovementState;
                animation_start = 21;
                goto ReverseCrouchAnimation;
            }
            if (movement_state >= 0x801)
            {
                if (movement_state == 4096)
                {
                    this->idle_ticks = 0;
                    this->movement_state = 0x2000;
                    playback.speed = 111408;
                    SetAnimationRange(11, 0, -1);
                    move_octant = 0;
                    goto SelectDirectionalAnimation;
                }
                move_octant = 0;
                if (movement_state == 0x2000)
                {
                    climb_complete = playback.complete;
                    this->idle_ticks = 0;
                    playback.speed = 111408;
                    if (!climb_complete)
                        goto ClearMovementOctant;
                    SetAnimationRange(0, 0, -1);

                    move_work.Scale(ledge_offset_scale, ledge_normal);
                    model.pos.Subtract(move_work);
                    climb_pad = pad;
                    model.pos.y = (uint32)model.pos.y - 884736;
                    climb_pad->buttons[17].pressed = false;
                    ledge_ground_y = CollisionRay::FindGroundHeight(model.pos, 0, 2048);
                    if (ledge_ground_y != -1)
                    {
                        ledge_contact_flags = contact_flags | 2;
                        model.pos.y = (uint32)ledge_ground_y - (ground_offset << 12);
                        contact_flags = ledge_contact_flags;
                    }
                    ground_grace_ticks = 4;
                    transition_camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    this->movement_state = 1;
                    motion.velocity.z = 0;
                    motion.velocity.x = 0;
                    transition_camera->ClearTargetTransition();
                    move_octant = 0;
                }
                goto SelectDirectionalAnimation;
            }
            move_octant = 0;
            if (movement_state != 1024)
                goto SelectDirectionalAnimation;
            this->idle_ticks = 0;
            transition_result = TryEnterMovementState16();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterMovementState32();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterMovementState512();
        CheckActionTransitions:
            move_octant = 0;

            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterJumpState();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterAirborneState();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryEnterActionState();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryActivateOneShotActivator();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            transition_result = TryActivateResettingActivator();
            move_octant = 0;
            if (transition_result)
                goto SelectDirectionalAnimation;
            move_axis = (sint8)move_input[1];
            goto CheckMovementInput;
        }
        if (movement_state == 0x10000)
        {
            OneShotActivator *one_shot = one_shot_activator.Get();
            if (!one_shot)
            {
                GameDiagnostic(("TODO Missing active one-shot activator\n"));
                abort();
            }
            activator_pos_x = model.pos.x;
            this->idle_ticks = 0;
            activator_target_x = one_shot->activation_pos.x;
            if (abs((sint32)(activator_target_x - activator_pos_x) >> 3) <= 0x7FFF)
                model.pos.x = activator_target_x;
            else
                model.pos.x = activator_pos_x + ((sint32)(activator_target_x - activator_pos_x) >> 3);
            activator_pos_z = model.pos.z;
            activation_target_z = one_shot_activator->activation_pos.z;
            if (abs((sint32)(activation_target_z - activator_pos_z) >> 3) <= 0x7FFF)
                model.pos.z = activation_target_z;
            else
                model.pos.z = activator_pos_z + ((sint32)(activation_target_z - activator_pos_z) >> 3);
            if (model.frame >= 25 && model.frame < 50)
            {
                if (one_shot_activator)
                {
                    one_shot_activator->model.angles.z = ((uint16)one_shot_activator->model.angles.z - 28) & 4095;
                    if (model.frame == 32)
                        one_shot_activator->SpawnObjects();
                }
            }
            if (playback.complete)
                this->movement_state = 1;
            goto ClearMovementOctant;
        }
        if (movement_state <= 0x10000)
        {
            move_octant = 0;
            if (movement_state == 0x8000)
            {
                this->idle_ticks = 0;
                goto ClearMovementOctant;
            }
        SelectDirectionalAnimation:
            aim_octant = 0;

            if (move_input[0] | move_input[1])
            {
                move_angle = move_heading;
                if (move_angle >= 257 && move_angle <= 3839)
                {
                    move_octant = ((move_angle - 256) >> 9) + 2;
                }
                else
                {
                    move_octant = 1;
                }
            }
            camera_yaw = 0;
            if (aim_input[0] | aim_input[1])
            {
                aim_angle = aim_heading;
                if (aim_angle >= 257 && aim_angle <= 3839)
                {
                    aim_octant = ((aim_angle - 256) >> 9) + 2;
                    camera_yaw = 0;
                }
                else
                {
                    aim_octant = 1;
                }
            }
            move_idx = move_octant - 1;
            aim_idx = aim_octant - 1;
            movement_camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
            if (movement_camera)
                camera_yaw = movement_camera->angles.y;
            facing_state = this->movement_state;
            if ((facing_state & 0x630) != 0)
            {
                if (aim_octant)
                {
                    const DirectionAnimation &entry = dir_animations[move_idx];
                    dir_animation = entry.animation[aim_idx];
                    animation_dir = entry.dir[aim_idx];
                    if ((dir_animation != model.animation) || (animation_dir != playback.dir))
                    {
                        StartAnimation(dir_animation, animation_dir);
                    }
                    immediate_facing = false;
                    facing_angle = entry.facing[aim_idx];
                    desired_heading = camera_yaw + entry.body_heading[aim_idx] + move_heading - move_idx * 512;
                }
                else
                {
                    if ((model.animation != 1) || ((facing_angle = 0), (playback.dir != 1)))
                    {
                        StartAnimation(1, 1);
                        facing_angle = 0;
                    }
                    immediate_facing = false;
                    desired_heading = camera_yaw + move_heading;
                }
                goto ApplyFacingTransition;
            }
            if (facing_state != 1)
            {
                if (facing_state == 256)
                {
                    if (aim_octant)
                    {
                        immediate_facing = false;
                        facing_angle = (dir_animations[9].facing[aim_idx] + aim_heading - aim_idx * 512) & 0xFFF;
                        desired_heading = camera_yaw + dir_animations[9].body_heading[aim_idx];
                    }
                    else
                    {
                        facing_angle = 0;
                        desired_heading = model.angles.y;
                        immediate_facing = false;
                    }
                    goto ApplyFacingTransition;
                }
                if ((facing_state & 0x8E) != 0)
                {
                    if (aim_octant)
                    {
                        immediate_facing = false;
                        facing_angle = (dir_animations[9].facing[aim_idx] + aim_heading - aim_idx * 512) & 0xFFF;
                        desired_heading = camera_yaw + dir_animations[9].body_heading[aim_idx];
                    }
                    else
                    {
                        facing_angle = 0;
                        immediate_facing = false;
                        desired_heading = camera_yaw + move_heading;
                    }
                    goto ApplyFacingTransition;
                }
                if (facing_state == 0x4000)
                {
                    if (aim_octant || !(move_input[0] | move_input[1]))
                    {
                        if (model.animation != 14)
                            SetAnimationRange(14, 0, -1);
                        if (!aim_octant)
                        {
                        AdvanceFacingTransition:
                            if (facing_ticks)
                            {
                                ModelJointAngles &facing = GetModelJointAngles(7);
                                facing.rotation.y = ((uint16)facing.rotation.y + facing_step) & 4095;
                                uint32 remaining = facing_ticks - 1;
                                facing_ticks = remaining;
                                if (!remaining)
                                {
                                    facing.rotation.y = facing_target;
                                }
                            }

                            acceleration_state = this->movement_state;
                            motion.acceleration.z = 0;
                            motion.acceleration.x = 0;
                            if (acceleration_state != 0x4000)
                            {
                                if (acceleration_state >= 0x4001)
                                {
                                    if (acceleration_state != 0x8000)
                                    {
                                    ApplyGravity:
                                        motion.acceleration.y = GetGameTuning().player.gravity * ONE;

                                    TransformMovement:
                                        if ((this->movement_state & 0x80000) == 0)
                                        {
                                            motion.angular_velocity = zero_angles;
                                            motion.angular_acceleration = zero_angles;
                                        }

                                        velocity_changed = false;
                                        movement_camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                                        if (movement_camera)
                                        {
                                            camera_angles = movement_camera->angles;
                                            camera_angles.BuildRotationMatrix(camera_matrix);
                                        }
                                        else
                                        {
                                            // Use the approved identity transform when the camera is absent
                                            memset(&camera_matrix, 0, sizeof(camera_matrix));
                                            camera_matrix.m[0][0] = ONE;
                                            camera_matrix.m[1][1] = ONE;
                                            camera_matrix.m[2][2] = ONE;
                                        }
                                        TransposeRotationMatrix(camera_matrix, inverse_camera_matrix);
                                        SetRotMatrix(&inverse_camera_matrix);

                                        camera_velocity.ShiftRight(motion.velocity, velocity_shift);

                                        move_work = camera_velocity;
                                        xport_gte_write_data(9, move_work.comp[0]);
                                        xport_gte_write_data(10, move_work.comp[1]);
                                        xport_gte_write_data(11, move_work.comp[2]);
                                        xport_gte_execute(0x49E012);
                                        move_work.comp[0] = xport_gte_read_data(25);
                                        move_work.comp[1] = xport_gte_read_data(26);
                                        move_work.comp[2] = xport_gte_read_data(27);

                                        move_work.ShiftLeft(velocity_shift);
                                        velocity_state = this->movement_state;
                                        if ((velocity_state & 0x71100) == 0)
                                        {
                                            switch (velocity_state)
                                            {
                                                case 0x800:
                                                    if (((sint8)model.frame) < 20)
                                                    {
                                                        move_work.x = -(sint16)GetGameTuning().player.move_speed * rsin(action_heading & 0xFFF);
                                                        velocity_changed = true;
                                                        move_work.z = -(sint16)GetGameTuning().player.move_speed * rcos(action_heading & 0xFFF);
                                                    }
                                                    break;

                                                case 0x4000:
                                                    if (!(aim_input[0] | aim_input[1]) && (move_input[0] | move_input[1]))
                                                    {
                                                        uint16 relative_heading = (rail->heading - (camera_yaw + move_heading)) & 4095;
                                                        if (relative_heading >= 1025 && relative_heading <= 3071)
                                                        {
                                                            rail_backward_speed = GetGameTuning().player.move_speed;
                                                            playback.speed = 0x20000;
                                                            rail_step = rail_backward_speed / 4;
                                                            rail_heading = (rail->heading + 2048) & 0xFFF;
                                                        }
                                                        else
                                                        {
                                                            rail_forward_speed = GetGameTuning().player.move_speed;
                                                            playback.speed = 0x20000;
                                                            rail_heading = rail->heading;
                                                            rail_step = rail_forward_speed / -4;
                                                        }
                                                        model.angles.y = rail_heading;
                                                        velocity_changed = true;
                                                    }
                                                    break;

                                                case 0x8000:
                                                    rail_forward_speed = GetGameTuning().player.move_speed;
                                                    move_work.x = (rail_forward_speed / 4) * 8 * rsin((rail->heading - camera_yaw) & 0xFFF);
                                                    velocity_changed = true;
                                                    move_work.z = (rail_forward_speed / 4) * 8 * rcos((rail->heading - camera_yaw) & 0xFFF);
                                                    break;

                                                default:
                                                    movement_x = move_input[0];
                                                    movement_z = move_input[1];

                                                    if (movement_x | movement_z)
                                                    {
                                                        move_strength = abs((movement_x * ONE) / 96) + abs((movement_z * ONE) / 96);
                                                        if (move_strength >= 4097)
                                                            move_strength = 4096;
                                                        if (movement_x)
                                                        {
                                                            move_work.z = -((rcos(move_heading & 0xFFF) * move_strength) >> 12) * (sint16)GetGameTuning().player.move_speed;
                                                            move_work.z = (sint32)((uint32)move_work.z * move_ramp) / 8;
                                                            velocity_changed = true;
                                                        }
                                                        if (move_input[1])
                                                        {
                                                            move_work.x = -((rsin(move_heading & 0xFFF) * move_strength) >> 12) * (sint16)GetGameTuning().player.move_speed;
                                                            move_work.x = (sint32)((uint32)move_work.x * move_ramp) / 8;
                                                            velocity_changed = true;
                                                        }
                                                    }
                                                    else if (!active_force)
                                                    {
                                                        velocity_changed = true;
                                                        move_work.comp[0] = 0;
                                                        move_work.comp[2] = 0;
                                                    }
                                                    break;
                                            }
                                        }
                                        SetRotMatrix(&camera_matrix);
                                        if (velocity_changed)
                                        {
                                            world_velocity.ShiftRight(move_work, velocity_shift);

                                            motion.velocity = world_velocity;
                                            xport_gte_write_data(9, (uint32)motion.velocity.x);
                                            xport_gte_write_data(10, (uint32)motion.velocity.y);
                                            xport_gte_write_data(11, (uint32)motion.velocity.z);
                                            xport_gte_execute(0x49E012);
                                            motion.velocity.x = xport_gte_read_data(25);
                                            motion.velocity.y = xport_gte_read_data(26);
                                            motion.velocity.z = xport_gte_read_data(27);

                                            motion.velocity.ShiftLeft(velocity_shift);
                                            rail_state = this->movement_state;
                                            if ((rail_state & 0x8000) != 0)
                                            {
                                                model.pos.Add(motion.velocity);
                                                rail_pos.Add(motion.velocity);
                                                if (!rail->Project(rail_pos, rail_point) || pad->buttons[17].pressed)
                                                {
                                                    pad->buttons[17].pressed = false;
                                                    rail_sound = rail_voice;
                                                    this->movement_state = 4;
                                                    on_rail = 0;
                                                    ::StopSoundVoices(rail_sound);
                                                    goto ApplyPlatformMotion;
                                                }
                                            ApplyRailPoint:
                                                model.pos = rail_point;
                                                rail_pos = model.pos;
                                                model.pos.y = (uint32)model.pos.y + 0x80000;
                                                goto ApplyPlatformMotion;
                                            }
                                            if ((rail_state & 0x4000) == 0)
                                                goto ApplyPlatformMotion;
                                            if (rail->Advance(rail_pos, rail_step, rail_point) && !pad->buttons[17].pressed)
                                            {
                                                goto ApplyRailPoint;
                                            }
                                            pad->buttons[17].pressed = false;
                                        }
                                        else
                                        {
                                            if (((this->movement_state & 0xC000) == 0) || ((rail_pad = pad), !rail_pad->buttons[17].pressed))
                                            {
                                            ApplyPlatformMotion:
                                                if (!(ground.GetObject()))
                                                    goto UpdateFootsteps;

                                                platform_moved = false;
                                                platform_pos = model.pos;
                                                if ((this->movement_state & 0x3000) != 0)
                                                {
                                                    model.pos.Add(static_cast<VisualObject *>(ground.GetObject()->owner)->motion.velocity);
                                                    platform_moved = true;
                                                    goto CheckPlatformMotion;
                                                }
                                                if (airborne_ticks)
                                                {
                                                    if ((move_input[0] | move_input[1]) || ((air_platform = ground.GetObject()), (air_platform_velocity_x = (uint32) static_cast<VisualObject *>(air_platform->owner)->motion.velocity.x), !(air_platform_velocity_x | (uint32) static_cast<VisualObject *>(air_platform->owner)->motion.velocity.z)))
                                                    {
                                                    CheckPlatformMotion:
                                                        if (platform_moved)
                                                        {
                                                            platform_ray.endpoints[0] = platform_pos;
                                                            platform_ray.endpoints[1] = model.pos;
                                                            platform_ray.Initialize();
                                                            platform_ray.Query(1);
                                                            if (platform_ray.hit.GetObject())
                                                            {
                                                                model.pos = platform_pos;
                                                            }
                                                        }

                                                    UpdateFootsteps:
                                                        if ((contact_flags & 0x100) != 0)
                                                            runtime.jump_released = 1;

                                                        if ((jump_hold_ticks && pad->buttons[17].down) && !runtime.jump_released)
                                                            motion.velocity.y = jump_velocity;
                                                        footstep_sound = 74;
                                                        if ((model.flags & 8) != 0)
                                                        {
                                                            if (runtime.last_footstep_sound == 74)
                                                                footstep_sound = 75;
                                                        }
                                                        else
                                                        {
                                                            do
                                                                footstep_sound = RandomRange(4) + 31;
                                                            while (footstep_sound == runtime.last_footstep_sound);
                                                        }
                                                        footstep_animation = model.animation;
                                                        switch (footstep_animation)
                                                        {
                                                            case 1:
                                                                footstep_crossed = DidAnimationCrossFrame(1, GetGameTuning().player.footstep_frames[0]);
                                                                footstep_event = UpdateEventLatch(footstep_crossed, 1);
                                                                played_footstep_sound = footstep_sound;
                                                                if (!footstep_event)
                                                                {
                                                                    footstep_frame = GetGameTuning().player.footstep_frames[1];
                                                                    footstep_animation_id = 1;
                                                                    break;
                                                                }
                                                            PlayFootstep:
                                                                PlaySoundEffect(played_footstep_sound, 0x3FFF, 0);

                                                                runtime.last_footstep_sound = footstep_sound;
                                                            AdvanceMovementTimers:
                                                                if (jump_hold_ticks)
                                                                    --jump_hold_ticks;
                                                                slowdown_intensity = slowdown_ticks;
                                                                if (slowdown_intensity)
                                                                {
                                                                    slowdown_g = slowdown_intensity << 8;
                                                                    if (slowdown_intensity >= 129)
                                                                    {
                                                                        slowdown_intensity = 128;
                                                                        slowdown_g = 0x8000;
                                                                    }
                                                                    model_effect.color_mode = ((slowdown_intensity | slowdown_g) | 0x6000000) | (slowdown_intensity << 17);
                                                                    model_effect.scale = 0x2000;
                                                                    slowdown_remaining = slowdown_ticks;
                                                                    model_effect.depth_bias = 10;
                                                                    slowdown_ticks = --slowdown_remaining;
                                                                    if (slowdown_remaining)
                                                                    {
                                                                    CheckActorContact:
                                                                        if (!pos_history.prev.Differs(model.pos))
                                                                        {
                                                                        UpdateMovementTrace:
                                                                            if (!runtime.motion_update_seen)
                                                                                runtime.motion_update_seen = 1;

                                                                            frame_delta.Subtract(model.pos, pos_history.prev);
                                                                            if (this->movement_state != 0x40000)
                                                                            {
                                                                                if (((uint32)frame_delta.x | frame_delta.y) | (uint32)frame_delta.z)
                                                                                {
                                                                                    runtime.trace_direction = frame_delta;
                                                                                }
                                                                                trace_dir.Normalize(runtime.trace_direction);

                                                                                trace_offset.Multiply(trace_dir, trace_offset_scale);
                                                                                trace_start.Subtract(pos_history.prev, trace_offset);
                                                                                motion_ray.endpoints[0] = trace_start;
                                                                                motion_ray.endpoints[1] = model.pos;
                                                                                motion_ray.Initialize();
                                                                                col_query.require_surface02 = false;
                                                                                motion_ray.triggers = true;
                                                                                motion_ray.Query(1);
                                                                                col_query.require_surface02 = false;
                                                                            }
                                                                            UpdateAnimationModel(PlayerHierarchy);
                                                                            weapon_angles.comp[0] = 0;
                                                                            weapon_angles.y = camera_yaw;
                                                                            weapon_angles.comp[2] = 0;
                                                                            aim_axis0 = 0;
                                                                            aim_axis1 = 0;
                                                                            if (analog_aim)
                                                                            {
                                                                                aim_axis0 = aim_input[0];
                                                                                aim_axis1 = aim_input[1];
                                                                            }
                                                                            if (aim_axis0 | aim_axis1)
                                                                            {
                                                                                weapon_angles.y = (camera_yaw + aim_heading) & 4095;
                                                                            }
                                                                            else
                                                                            {
                                                                                body_heading = model.angles.y;
                                                                                model_heading = (uint16)GetModelJointAngles(7).rotation.y;
                                                                                weapon_angles.y = body_heading + model_heading;
                                                                            }
                                                                            UpdateWeapons(weapon_angles, weapons_enabled);
                                                                            UpdateGroundContact();
                                                                            if (health < 0)
                                                                                health = 0;
                                                                            if (!on_rail)
                                                                                rail = 0;
                                                                            if ((this->movement_state & 0x180000) == 0)
                                                                                traversal_region = 0;
                                                                            cleanup_state = this->movement_state;
                                                                            result = 0x20000;
                                                                            if (cleanup_state != 0x10000)
                                                                            {
                                                                                one_shot_activator = 0;
                                                                                cleanup_state = this->movement_state;
                                                                            }
                                                                            if (cleanup_state != 0x20000)
                                                                                resetting_activator = 0;
                                                                            active_force = 0;
                                                                            return result;
                                                                        }

                                                                        contact = CollisionRay::TraceSeg(pos_history.prev, model.pos, visual_lists.actors);
                                                                        if (contact)
                                                                        {
                                                                            if (static_cast<VisualObject *>(contact->owner)->kind == 201)
                                                                                goto CheckPickupContact;
                                                                            ApplyContactImpulse(*static_cast<VisualObject *>(contact->owner));
                                                                        }
                                                                    CheckPickupContact:
                                                                        pickup = CollisionRay::TraceSeg(pos_history.prev, model.pos, visual_lists.pickups);

                                                                        if (pickup)
                                                                            static_cast<Pickup *>(pickup->owner)->Collect(*this);
                                                                        goto UpdateMovementTrace;
                                                                    }
                                                                    model_effect.color_mode = 0;
                                                                }
                                                                goto CheckActorContact;

                                                            case 3:
                                                                footstep_crossed = DidAnimationCrossFrame(3, GetGameTuning().player.footstep_frames[0]);
                                                                footstep_event = UpdateEventLatch(footstep_crossed, 1);
                                                                played_footstep_sound = footstep_sound;
                                                                if (footstep_event)
                                                                    goto PlayFootstep;
                                                                footstep_frame = GetGameTuning().player.footstep_frames[1];
                                                                footstep_animation_id = 3;
                                                                break;

                                                            case 4:
                                                                footstep_crossed = DidAnimationCrossFrame(4, GetGameTuning().player.footstep_frames[0]);
                                                                footstep_event = UpdateEventLatch(footstep_crossed, 1);
                                                                played_footstep_sound = footstep_sound;
                                                                if (footstep_event)
                                                                    goto PlayFootstep;
                                                                footstep_frame = GetGameTuning().player.footstep_frames[1];
                                                                footstep_animation_id = 4;
                                                                break;

                                                            case 8:
                                                                footstep_crossed = DidAnimationCrossFrame(8, 7);
                                                                footstep_event = UpdateEventLatch(footstep_crossed, 1);
                                                                played_footstep_sound = footstep_sound;
                                                                if (footstep_event)
                                                                    goto PlayFootstep;
                                                                footstep_animation_id = 8;
                                                                footstep_frame = 18;
                                                                break;

                                                            default:
                                                                goto AdvanceMovementTimers;
                                                        }

                                                        footstep_crossed = DidAnimationCrossFrame(footstep_animation_id, footstep_frame);
                                                        footstep_event = UpdateEventLatch(footstep_crossed, 2);
                                                        played_footstep_sound = footstep_sound;
                                                        if (!footstep_event)
                                                            goto AdvanceMovementTimers;
                                                        goto PlayFootstep;
                                                    }
                                                    platform_carried_x = (uint32)model.pos.x + air_platform_velocity_x;
                                                    platform_z_owner = ground.GetObject();
                                                    model.pos.x = platform_carried_x;
                                                }
                                                else
                                                {
                                                    ground_platform = ground.GetObject();
                                                    ground_velocity_x = (uint32) static_cast<VisualObject *>(ground_platform->owner)->motion.velocity.x;
                                                    uint32 platform_y = (uint32) static_cast<VisualObject *>(ground_platform->owner)->motion.velocity.y;
                                                    uint32 platform_z = (uint32) static_cast<VisualObject *>(ground_platform->owner)->motion.velocity.z;
                                                    if (!(ground_velocity_x | platform_y | platform_z))
                                                        goto CheckPlatformMotion;
                                                    platform_y_owner = ground.GetObject();
                                                    model.pos.x = (uint32)model.pos.x + ground_velocity_x;
                                                    platform_carried_y = (uint32)model.pos.y + (uint32) static_cast<VisualObject *>(platform_y_owner->owner)->motion.velocity.y;
                                                    platform_z_owner = ground.GetObject();
                                                    model.pos.y = platform_carried_y;
                                                }
                                                platform_moved = true;
                                                model.pos.z = (uint32)model.pos.z + ((uint32) static_cast<VisualObject *>(platform_z_owner->owner)->motion.velocity.z);
                                                goto CheckPlatformMotion;
                                            }
                                            rail_pad->buttons[17].pressed = false;
                                        }
                                        this->movement_state = 4;
                                        on_rail = 0;
                                        goto ApplyPlatformMotion;
                                    }
                                }
                                else if (acceleration_state != 4096)
                                {
                                    goto ApplyGravity;
                                }
                            }
                            motion.velocity = zero_vector;
                            motion.acceleration = zero_vector;
                            goto TransformMovement;
                        }

                        immediate_facing = true;
                        facing_angle = (dir_animations[9].facing[aim_idx] + aim_heading - aim_idx * 512) & 0xFFF;
                        desired_heading = camera_yaw + dir_animations[9].body_heading[aim_idx];
                    ApplyFacingTransition:
                        SetFacingTransition(facing_angle, desired_heading, immediate_facing);

                        goto AdvanceFacingTransition;
                    }
                    if (model.animation != 13)
                        StartAnimation(13, 1);
                    desired_heading = model.angles.y;
                    facing_angle = 0;
                }
                else if (facing_state == 0x8000)
                {
                    if (model.animation != 14)
                        SetAnimationRange(14, 0, -1);
                    if (aim_octant)
                    {
                        immediate_facing = true;
                        facing_angle = dir_animations[9].facing[aim_idx] + aim_heading - aim_idx * 512;
                        desired_heading = camera_yaw + dir_animations[9].body_heading[aim_idx];
                        goto ApplyFacingTransition;
                    }
                    desired_heading = model.angles.y;
                    facing_angle = 0;
                }
                else
                {
                    if ((facing_state & 0x1F3800) != 0)
                    {
                        desired_heading = model.angles.y;
                        facing_angle = 0;
                    }
                    else
                    {
                        facing_angle = 0;
                        desired_heading = camera_yaw;
                    }
                }
                immediate_facing = true;
                goto ApplyFacingTransition;
            }
            if (aim_octant)
            {
                if (model.animation != 9)
                    StartAnimation(9, 1);

                immediate_facing = false;
                facing_angle = (dir_animations[8].facing[aim_idx] + aim_heading - aim_idx * 512) & 0xFFF;
                desired_heading = camera_yaw + dir_animations[8].body_heading[aim_idx];
                goto ApplyFacingTransition;
            }
            idle_animation = model.animation;
            if (idle_animation == 6)
            {
                if (!playback.complete)
                {
                KeepCurrentFacing:

                    facing_angle = 0;
                    desired_heading = model.angles.y;
                    immediate_facing = false;
                    goto ApplyFacingTransition;
                }
            }
            else
            {
                if (!model.animation)
                    goto KeepCurrentFacing;
                if (idle_animation == 5)
                    goto KeepCurrentFacing;
            }
            SetAnimationRange(0, 0, -1);
            goto KeepCurrentFacing;
        }
        if (movement_state == 0x20000)
        {
            ResettingActivator *resetting = resetting_activator.Get();
            if (!resetting)
            {
                GameDiagnostic(("TODO Missing active resetting activator\n"));
                abort();
            }
            reset_pos_x = model.pos.x;
            this->idle_ticks = 0;
            reset_target_x = resetting->activation_pos.x;
            if (abs((sint32)(reset_target_x - reset_pos_x) >> 3) <= 0x7FFF)
                model.pos.x = reset_target_x;
            else
                model.pos.x = reset_pos_x + ((sint32)(reset_target_x - reset_pos_x) >> 3);
            reset_pos_z = model.pos.z;
            activation_target_z = resetting_activator->activation_pos.z;
            if (abs((sint32)(activation_target_z - reset_pos_z) >> 3) <= 0x7FFF)
                model.pos.z = activation_target_z;
            else
                model.pos.z = reset_pos_z + ((sint32)(activation_target_z - reset_pos_z) >> 3);
            if ((model.frame == 14) && (playback.dir == 1))
                resetting_activator->Activate();
            if (playback.complete)
            {
                if (playback.dir == 1)
                {
                    SetAnimationRange(18, (sint8)model.frame, 0);
                    move_octant = 0;
                    goto SelectDirectionalAnimation;
                }
                this->movement_state = 1;
            }
            goto ClearMovementOctant;
        }
        move_octant = 0;
        if (movement_state != 0x40000)
            goto SelectDirectionalAnimation;
        this->idle_ticks = 0;
        death_tick_after = ++death_ticks;
        if (death_tick_after == 32 || death_tick_after == 64 || death_tick_after == 96)
        {
            if (visual_lists.cameras)
            {
                if (death_camera_enabled)
                    static_cast<Camera *>(visual_lists.cameras->owner)->SetYawTransition(((uint16) static_cast<Camera *>(visual_lists.cameras->owner)->angles.y + 1024) & 0xFFF, 34);
            }
            if (death_ticks == 64)
                ::FadeScrToWhite(128, 1);
        }
        if (!playback.complete)
        {
            runtime.death_wait = 0;
            move_octant = 0;
            goto SelectDirectionalAnimation;
        }
        if (game_progress.lives == 1)
        {
            if ((sint32)runtime.death_wait < 128)
                goto AdvanceDeathWait;
        }
        else if ((sint32)runtime.death_wait < 64)
        {
        AdvanceDeathWait:
            ++runtime.death_wait;

            move_octant = 0;
            goto SelectDirectionalAnimation;
        }
        Finish();
        ::ResetScrFlash();
        goto AdvanceDeathWait;
    }
    return result;
}

uint32 Player::SqVelocityComponent(sint32 velocity)
{
    uint32 component = velocity >> 9;
    return component * component;
}

void Player::SetFacingTransition(uint16 target_angle, uint16 heading, bool immediate)
{
    FUNCTION_MARKER(0x80061A78, "SLUS_003.73");
    sint32 target = target_angle & 4095;
    sint32 desired = heading & 4095;
    ModelJointAngles &facing = GetModelJointAngles(7);
    if (analog_aim || aim_press_ticks == 1)
        immediate = true;
    if (immediate)
    {
        model.angles.y = desired;
        facing.rotation.y = target;
        facing_step = 0;
        facing_ticks = 0;
        return;
    }
    sint32 old_heading = model.angles.y;
    sint32 delta = desired - old_heading;
    sint32 yaw_distance = delta < 0 ? -delta : delta;
    if (yaw_distance >= 1025 && yaw_distance <= 3071)
    {
        model.angles.y = desired;
        facing.rotation.y = target;
        facing_step = 0;
        facing_ticks = 0;
        return;
    }
    sint32 old_angle = facing.rotation.y;
    delta = target - old_angle;
    sint32 angle_distance = delta < 0 ? -delta : delta;
    uint16 angle_word = facing.rotation.y;
    if (angle_distance >= 1025 && angle_distance <= 3071)
    {
        model.angles.y = desired;
        facing.rotation.y = target;
        facing_step = 0;
        facing_ticks = 0;
        return;
    }
    if (yaw_distance && angle_distance)
    {
        sint32 angle = old_heading < desired ? angle_word - yaw_distance : angle_word + yaw_distance;
        facing.rotation.y = angle & 4095;
    }
    facing_ticks = 8;
    model.angles.y = desired;
    facing_target = target;
    old_angle = facing.rotation.y;
    angle_word = facing.rotation.y;
    if (old_angle < target)
        delta = target - old_angle < 1024 ? target - old_angle : target - 4096 - old_angle;
    else
    {
        delta = target - old_angle;
        if (old_angle - target >= 1024)
        {
            sint16 wrapped_angle = angle_word - 4096;
            delta = target - wrapped_angle;
        }
    }
    facing_step = delta / 8;
}

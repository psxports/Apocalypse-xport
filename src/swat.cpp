#include <new>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "game_heap.h"
#include "level_session.h"
#include "explosion_effects.h"
#include "game_tuning.h"
#include "game_tick.h"

#include "record_scripts.h"

#include "world_gravity.h"

#include "object_initializers.h"

#include "effect_object.h"

#include "model_resources.h"
#include "visual_object.h"
#include "object_damage.h"
#include "collision.h"
#include "model_animation.h"
#include "game_diagnostics.h"
#include "vector_arithmetic.h"
#include "projectile_launch.h"
#include "projectile_guided.h"
#include "flamethrower.h"
#include "effect_lifecycle.h"
#include "model_attachment.h"
#include "audio_session.h"
#include "audio_voice.h"
#include "projectile_oscillating.h"
#include "weapon_nodes.h"
#include "machine_gun.h"
#include "spiral_ray_weapon.h"
#include "vector_distance.h"
#include "angle_math.h"
#include "player.h"
#include "game_random.h"
#include "game_object_allocation.h"
#include "particle_debris.h"
#include "particle_settings.h"
#include "fading_particle.h"

#include "swat.h"
#include "ring_burst_effect.h"
#include "record_data.h"
#include "turret.h"
#include "fireball_effects.h"
#include "panel_particle.h"
#include "oscillating_ring.h"

SwatPopulation swat_population = {0, 0};

void ResetSwatPopulation()
{
    swat_population.count = 0;
    swat_population.zombies = 0;
}

struct BurnAttachments
{
    uint32 count;
    uint8 bones[4];
};

static const BurnAttachments BurnAttachmentSets[6] = {{4, {7, 12, 10, 8}}, {4, {0, 2, 4, 6}}, {4, {7, 15, 9, 12}}, {4, {7, 16, 15, 12}}, {4, {7, 13, 10, 8}}, {3, {0, 5, 10, 0}}};

Swat::Swat(const RecordCursor &record_data, uint32 index) : Enemy(), jump_ticks(0), attack_frame(0), fragment_first(0), fragment_count(0)
{
    aim_pos.x = aim_pos.y = aim_pos.z = 0;
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x8004E35C, "SLUS_003.73");
    RecordCursor decoded;

    decoded = ReadPosition(record_data);
    script_cursor = ReadAngles(decoded);
    this->control.script_active = 1;
    this->record_idx = index;
    Init();
    ExecuteScript(script_cursor);
}

Swat::Swat(const uint32 *position_vector, const RecordCursor &script) : Enemy(), jump_ticks(0), attack_frame(0), fragment_first(0), fragment_count(0)
{
    aim_pos.x = aim_pos.y = aim_pos.z = 0;
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x8004E3E4, "SLUS_003.73");
    uint32 x, y, z;
    RecordCursor cursor;
    x = position_vector[0];
    y = position_vector[1];
    z = position_vector[2];
    pos_history.saved.words[0] = x;
    pos_history.saved.words[1] = y;
    pos_history.saved.words[2] = z;
    x = pos_history.saved.words[0];
    y = pos_history.saved.words[1];
    z = pos_history.saved.words[2];
    this->model.pos.words[0] = x;
    this->model.pos.words[1] = y;
    this->model.pos.words[2] = z;
    script_cursor = script;
    Init();
    cursor = script_cursor;
    if (cursor)
    {
        this->control.script_active = 1;
        ExecuteScript(script_cursor);
    }
}

uint32 Swat::Init()
{
    FUNCTION_MARKER(0x8004E48C, "SLUS_003.73");
    sint32 population_count;
    sint32 result;
    game_progress.level.enemy_total = game_progress.level.enemy_total + 1;
    motion.drag[0] = 31;
    motion.drag[2] = 31;
    motion.drag[1] = 3;
    AddToList(visual_lists.actors);
    population_count = swat_population.count;
    result = 200;
    this->kind = 200;
    ai.state = 0;
    ai.phase = 0;
    swat_population.count = (uint32)population_count + 1;
    this->control.hierarchy_ready = 0;
    return result;
}

uint32 Swat::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8004E508, "SLUS_003.73");
    uint32 kind, count;
    RemoveFromList(visual_lists.actors);
    if (burn_ring)
    {
        ObjectBorrow borrow;
        burn_ring->Destroy(3);
    }
    if (weapon)
    {
        ObjectBorrow borrow;
        weapon->Destroy(3);
    }
    Enemy::FreeBehaviorTable();
    count = swat_population.count;
    kind = (uint32)this->control.variant;
    swat_population.count = count - 1;
    if (kind == 128)
        swat_population.zombies = swat_population.zombies - 1;
    if ((uint32)this->control.variant == 256)
    {
        uint32 sound = ai.script_data;
        if (sound)
        {
            StopSoundVoices(sound);
            ai.script_data = 0;
        }
    }
    Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(this);
    return reason & 1;
}

uint32 Swat::Update()
{
    ModelBorrow model_borrow;
    RecordDataBorrow record_borrow;
    ShortVector3 motion_target;
    FUNCTION_MARKER(0x8004E71C, "SLUS_003.73");
    sint32 ambient_roll0;
    sint32 ambient_sound;
    sint32 ambient_roll1;
    uint16 activation;
    uint8 activation_count;
    uint16 script_delay;
    bool jump_done;
    bool melee_hit;
    uint16 next_script_delay;
    const ModelAttachmentTable *attachments;
    uint32 ground_period;
    uint8 ground_tick;
    sint32 ground_probe_offset;
    sint32 ground_y;
    uint16 ground_contact_flags;
    uint32 surface_z;
    uint32 surface_flags;
    uint32 ground_height;
    uint32 surface_y;
    sint16 subject_heading;
    sint16 dst_heading;
    uint32 movement_flags;
    sint32 jump_velocity_x;
    sint32 jump_velocity_z;
    sint32 jump_duration;
    sint32 jump_height;
    sint32 jump_remaining;
    sint8 next_jump_tick;
    uint32 dst_y;
    uint32 dst_z;
    uint32 next_movement_flags;
    sint32 landing_y;
    sint32 landing_frame;
    sint32 turn_speed;
    uint32 move_speed;
    uint32 move_velocity_y;
    uint32 move_velocity_z;
    bool reached_dst;
    sint32 attack_animation;
    sint32 melee_event;
    uint16 base_damage;
    sint32 melee_damage;
    uint16 attack_variant;
    sint32 close_fire_event;
    sint32 fire_event;
    sint16 player_heading;
    sint32 fire_frame;
    ModelInstance *contact_player;
    sint32 contact_damage;
    sint32 particle_roll;
    uint32 particles_remaining;
    GameHeapBlock *particle_storage;
    GameHeapBlock *ring_storage;
    uint16 final_variant;
    sint32 visibility_animation;
    sint16 visibility_flags;
    sint32 result;
    sint32 footstep_event0;
    sint32 footstep_event1;
    Vector3 attack_vector, attachment_pos;
    ModelAttachmentPoint attachment_desc;
    uint32 flags;
    uint32 surface_x;
    uint32 aim_delta_x, aim_delta_z;
    uint32 dst_x;
    uint32 melee_ground_height, melee_y, player_ground_height, player_y;
    ModelInstance *player;
    Vector3 move_delta, move_dir;
    motion.acceleration = zero_vector;
    if (weapon)
        weapon->Update(0, 0);
    if (control.variant == 128)
    {
        ambient_roll0 = RandomRange(240);
        ambient_sound = 98;
        if (!ambient_roll0 || (ambient_roll1 = RandomRange(240), ambient_sound = 99, !ambient_roll1))
            PlayPositionedSound(ambient_sound, model.pos.words, 0);
    }
    activation = activation_flags;
    control.prev_activation_flags = activation;
    if (activation & 1)
    {
        activation_count = control.activation_count;
        activation_flags = (activation & 0xFFFE);
        control.activation_count = activation_count + 1;
    }
    if (((activation_flags & 4) != 0))
        ApplyExplosionDamage();
    if (control.script_active)
    {
        script_delay = control.script_delay;
        next_script_delay = script_delay - 1;
        if (!script_delay)
            ExecuteScript(script_cursor);
        else
            control.script_delay = next_script_delay;
    }
    else
    {
        UpdateState();
    }
    flags = control.flags;
    control.flags = flags & ~0x400;
    if (!control.hierarchy_ready)
    {
        attachments = 0;
        switch (control.variant)
        {
            case 1:
                attachments = &SwatMode1Attachments;
                break;
            case 2:
            case 128:
                attachments = &SwatMode2Attachments;
                break;
            case 4:
                attachments = &SwatMode4Attachments;
                break;
            case 8:
                attachments = &SwatMode8Attachments;
                break;
            case 16:
                attachments = &SwatMode16Attachments;
                break;
            case 64:
                attachments = &SwatMode64Attachments;
                break;
            case 256:
                attachments = &SwatMode256Attachments;
                break;
        }
        if (attachments)
            SetModelBoneTable(model.bank, *attachments);
        control.hierarchy_ready = 1;
    }
    if (control.gravity_enabled)
        AddVectors(motion.acceleration.words, world_gravity.vector.words);

    IntegrateMotion();
    if (control.flags & 2)
    {
        ground_tick = control.ground_tick + 1;
        ground_period = control.ground_period;
        control.ground_tick = ground_tick;
        if (ground_tick >= ground_period)
        {
            ground_probe_offset = (sint16)ground_offset;
            control.ground_tick = 0;
            ground_y = FindGroundHeight(model.pos.words, ground_probe_offset, 4 * (uint32)ground_probe_offset);
            if (ground_y != -1)
            {
                ground_height = (uint32)(sint32)(sint16)ground_offset;
                ground_contact_flags = contact_flags | 2;
                model.pos.words[1] = (uint32)ground_y - (ground_height << 12);
                contact_flags = ground_contact_flags;
            }
        }
    }
    UpdateWorldTransform();
    surface_x = model.pos.words[0];
    ground_height = (uint32)(sint32)(sint16)ground_offset << 12;
    surface_z = model.pos.words[2];
    surface_flags = control.flags;
    surface.pos.words[0] = surface_x;
    surface_y = model.pos.words[1];
    surface.pos.words[2] = surface_z;
    surface.pos.words[1] = (uint32)surface_y + (uint32)ground_height;
    if (surface_flags & 0x80)
    {
        ModelInstance *subject = aim_subject.GetObject();
        if (subject)
        {
            const uint32 *position = static_cast<VisualObject *>(subject->owner)->kind == 50 ? static_cast<VisualObject *>(subject->owner)->surface.pos.words : subject->pos.words;
            aim_pos.x = position[0];
            aim_pos.y = position[1];
            aim_pos.z = position[2];
            aim_delta_z = (uint32)aim_pos.z - model.pos.words[2];
            aim_delta_x = (uint32)aim_pos.x - model.pos.words[0];
            subject_heading = ratan2(aim_delta_z, aim_delta_x);
            motion_target.x = 0;
            motion_target.y = (3072 - (uint32)subject_heading) & 0xFFF;
            motion_target.z = 0;
            UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, 32);
        }
    }
    if (control.flags & 1)
    {
        aim_delta_z = destination.words[2] - model.pos.words[2];
        aim_delta_x = destination.words[0] - model.pos.words[0];
        dst_heading = ratan2(aim_delta_z, aim_delta_x);
        motion_target.x = 0;
        motion_target.y = (3072 - (uint32)dst_heading) & 0xFFF;
        motion_target.z = 0;
        UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, 32);
        movement_flags = control.flags;
        if (movement_flags & 8)
        {
            if ((sint8)model.frame >= 3)
            {
                if (jump_ticks)
                    control.gravity_enabled = 1;
            }
            else
            {
                playback.speed = 0x10000;
                control.gravity_enabled = 0;
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                motion.velocity.words[0] = 0;
                motion.acceleration.words[2] = 0;
                motion.acceleration.words[1] = 0;
                motion.acceleration.words[0] = 0;
            }
            if ((sint8)model.frame < 3 || (control.flags & 0x20))
            {
                if ((sint8)model.frame >= 22)
                {
                    if (!jump_ticks)
                    {
                    FinishJumpAnimation:
                        if (!playback.complete)
                            goto UpdateAttack;

                        next_movement_flags = (control.flags & 0xFFFFFFD6);
                        goto StoreMovementFlags;
                    }
                    playback.speed = 0;
                }
            }
            else
            {
                jump_velocity_x = Divide((sint32)(destination.words[0] - model.pos.words[0]), jump_ticks);
                jump_velocity_z = Divide((sint32)(destination.words[2] - model.pos.words[2]), jump_ticks);
                jump_height = destination.words[1] - model.pos.words[1];
                jump_duration = jump_ticks;
                motion.drag[0] = 31;
                control.flags |= 0x20;
                motion.velocity.words[0] = jump_velocity_x;
                motion.velocity.words[2] = jump_velocity_z;
                motion.velocity.words[1] = (uint32)Divide(jump_height, (uint32)jump_duration) - ((uint32)jump_duration << 14);
                motion.drag[1] = 31;
                motion.drag[2] = 31;
            }
            jump_remaining = jump_ticks;
            jump_done = (jump_remaining == 0);
            next_jump_tick = jump_remaining - 1;
            if (!jump_done)
            {
                jump_ticks = next_jump_tick;
                jump_done = (next_jump_tick == 0);
                if (!jump_done && (sint32)motion.velocity.words[1] > 0)
                {
                    jump_done = (sint32)destination.words[1] < (sint32)model.pos.words[1];
                }
                if (jump_done)
                {
                    jump_ticks = 0;
                    dst_x = destination.words[0];
                    dst_y = destination.words[1];
                    dst_z = destination.words[2];
                    model.pos.words[0] = dst_x;
                    model.pos.words[1] = dst_y;
                    model.pos.words[2] = dst_z;
                    motion.velocity.words[2] = 0;
                    motion.velocity.words[1] = 0;
                    motion.velocity.words[0] = 0;
                    motion.acceleration.words[2] = 0;
                    motion.acceleration.words[1] = 0;
                    playback.speed = 0x10000;
                    motion.acceleration.words[0] = 0;
                    control.gravity_enabled = 0;
                    motion.drag[0] = 31;
                    motion.drag[1] = 3;
                    motion.drag[2] = 31;
                }
            }
            goto FinishJumpAnimation;
        }
        if (movement_flags & 0x10)
        {
            landing_y = destination.words[1];
            if (landing_y < (sint32)model.pos.words[1])
            {
                model.pos.words[1] = landing_y;
                control.gravity_enabled = 0;
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                landing_frame = (sint8)model.frame;
                motion.velocity.words[0] = 0;
                SetAnimationRange(4, landing_frame, -1);
            }
            if (playback.complete)
            {
                next_movement_flags = (control.flags & 0xFFFFFFEE);
            StoreMovementFlags:
                control.flags = next_movement_flags;
            }
        }
        else
        {
            move_delta.words[0] = (sint32)(destination.words[0] - model.pos.words[0]) >> 12;
            move_delta.words[1] = (sint32)(destination.words[1] - model.pos.words[1]) >> 12;
            move_delta.words[2] = (sint32)(destination.words[2] - model.pos.words[2]) >> 12;
            NormalizeShortVector(move_delta.words, move_dir.words);
            turn_speed = (sint16)motion.angular_velocity.words[1];
            if (turn_speed < 0)
                turn_speed = -turn_speed;
            if (turn_speed >= 65)
                turn_speed = 64;
            move_speed = (uint32)ai.parameters[0] * (64 - (uint32)turn_speed) / 64;
            move_velocity_y = move_dir.words[1] * move_speed;
            move_velocity_z = move_dir.words[2] * move_speed;
            motion.velocity.words[0] = move_dir.words[0] * move_speed;
            motion.velocity.words[1] = move_velocity_y;
            motion.velocity.words[2] = move_velocity_z;
            if (control.flags & 2)
                reached_dst = (MeasureHorizontalDistance(model.pos.words, destination.words) < 0x40);
            else
                reached_dst = (MeasureEuclideanDistance(model.pos.words, destination.words) < 0x40);
            if (reached_dst)
            {
                next_movement_flags = (control.flags & 0xFFFFFFFE);
                goto StoreMovementFlags;
            }
        }
    }
UpdateAttack:
    attack_animation = model.animation;

    if (attack_animation == 3)
    {
        melee_event = DidAnimationCrossFrame(3, attack_frame);
        melee_hit = 0;
        if (UpdateEventLatch(melee_event, 1))
        {
            player = visual_lists.players;
            if (player && MeasureHorizontalDistance(model.pos.words, player->pos.words) < 256)
            {
                melee_ground_height = (uint32)(sint32)(sint16)ground_offset << 12;
                melee_y = model.pos.words[1];
                player = visual_lists.players;
                player_ground_height = (uint32) static_cast<Player *>(visual_lists.players->owner)->ground_offset << 12;
                player_y = player->pos.words[1];
                melee_hit = Abs32((sint32)(player_y + player_ground_height - melee_y - melee_ground_height)) <= 0x7FFFF;
            }
        }
        if (melee_hit)
        {
            base_damage = GetBehaviorValue(0, 1);
            melee_damage = ScaleDamageForDifficulty(base_damage);
            SubtractVectors(attack_vector.words, visual_lists.players->pos.words, model.pos.words);
            VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
            damage_target->Damage((uint32)melee_damage, attack_vector.components, 0);
        }
        attack_animation = model.animation;
    }
    if (attack_animation == 1)
    {
        attack_variant = (uint32)control.variant;
        if (attack_variant & 0x10)
        {
            close_fire_event = DidAnimationCrossFrame(1, 10);
            if ((UpdateEventLatch(close_fire_event, 2) && ((sint32)MeasureHorizontalDistance(model.pos.words, visual_lists.players->pos.words) < 4500)))
                goto FireAttack;
        }
        else if (attack_variant & 8)
        {
            fire_event = DidAnimationCrossFrame(1, 12);
            if (UpdateEventLatch(fire_event, 2))
            FireAttack:
                Fire();
        }
        else if (attack_variant & 0x40)
        {
            player = visual_lists.players;
            aim_delta_z = player->pos.words[2] - model.pos.words[2];
            aim_delta_x = player->pos.words[0] - model.pos.words[0];
            player_heading = ratan2(aim_delta_z, aim_delta_x);
            motion_target.x = 0;
            motion_target.y = (3072 - (uint32)player_heading) & 0xFFF;
            motion_target.z = 0;
            UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, 32);
            fire_frame = model.frame;
            if (((((uint32)(((uint32)(fire_frame) - (uint32)(21)))) < 0x13) && ((fire_frame & 3) == 0)))
                goto FireAttack;
        }
    }
    if (control.variant == 128)
    {
        contact_player = visual_lists.players;
        model_effect.color_mode = 100687952;
        model_effect.scale = 10240;
        model_effect.depth_bias = 10;
        if ((((((uint32)(MeasureHorizontalDistance(model.pos.words, contact_player->pos.words))) < 0xA0) && (ai.state != 16)) && visual_lists.players))
        {
            contact_damage = ScaleDamageForDifficulty(1);
            VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
            damage_target->Damage((uint32)contact_damage, zero_vector.components, 0);
        }
    }
    if ((((control.flags & 0x100) != 0) && !RandomRange(4)))
    {
        use_small_effect_pool = false;
        particle_roll = RandomRange(3);
        particles_remaining = (uint32)particle_roll + 2;
        if ((particle_roll != -2))
        {
            do
            {
                particle_storage = AllocateEffectStorage(116);
                if (particle_storage)
                {
                    new (particle_storage) PanelParticle(model.pos.words);
                }
                --particles_remaining;
            } while (particles_remaining);
        }
        use_small_effect_pool = true;
    }
    if (control.flags & 4)
    {
        if (!burn_ring)
        {
            ring_storage = AllocateEffectStorage(176);
            if (ring_storage)
            {
                burn_ring = new (ring_storage) OscillatingRing(model.pos.words, 64, 32, 64, 112, 96, 0, 48, 0x25, 0);
            }
            if (!burn_ring)
            {
                GameDiagnostic("Swat burn ring allocation failed\n");
                abort();
            }
            burn_ring->persistent = 1;
            burn_ring->SetPosition(model.pos.words);
            control.flags |= 0x800;
        }
        if (game_clock.tick & 1)
        {
            const BurnAttachments *attachments = 0;
            switch (control.variant)
            {
                case 1:
                    attachments = BurnAttachmentSets;
                    break;
                case 4:
                    attachments = BurnAttachmentSets + 1;
                    break;
                case 2:
                    attachments = BurnAttachmentSets + 2;
                    break;
                case 8:
                    attachments = BurnAttachmentSets + 3;
                    break;
                case 16:
                    attachments = BurnAttachmentSets + 4;
                    break;
                case 256:
                    attachments = BurnAttachmentSets + 5;
                    break;
            }
            uint32 ring_bone = 0;
            if (attachments)
            {
                ring_bone = attachments->bones[0];
                uint32 bone_idx = RandomRange(attachments->count);
                use_small_effect_pool = false;
                GameHeapBlock *allocation = AllocateEffectStorage(132);
                if (allocation)
                {
                    new (allocation) Fireball(*this, attachments->bones[bone_idx]);
                }
                use_small_effect_pool = true;
            }
            if (burn_ring)
            {
                attachment_desc.x = attachment_desc.y = attachment_desc.z = 0;
                attachment_desc.bone = ring_bone;
                GetModelBonePosition(attachment_pos.words, *this, attachment_desc);
                burn_ring->SetPosition(attachment_pos.words);
            }
        }
    }
    final_variant = (uint32)control.variant;
    if (final_variant == 8)
    {
        visibility_animation = model.animation;
        if (visibility_animation == 8)
        {
            visibility_flags = (model.flags & 0xFFFE);
        }
        else
        {
            if ((!(playback.complete) || (visibility_animation != 7)))
                goto UpdateFootsteps;
            visibility_flags = (model.flags | 1);
        }
        model.flags = visibility_flags;
    UpdateFootsteps:
        final_variant = (uint32)control.variant;
    }
    result = 2;
    if (((final_variant == 256) && (model.animation == 2)))
    {
        footstep_event0 = DidAnimationCrossFrame(2, 3);
        if (UpdateEventLatch(footstep_event0, 1))
            return PlayPositionedSound(121, model.pos.words, 3);
        footstep_event1 = DidAnimationCrossFrame(2, 17);
        result = UpdateEventLatch(footstep_event1, 2);
        if (result)
            return PlayPositionedSound(121, model.pos.words, 3);
    }
    return result;
}

// Native helper preserves original signed DIV corner results
static uint32 DivideExplosionDamage(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

uint32 Swat::ApplyExplosionDamage()
{
    uint32 index = 0, count, result, radius, distance, damage;
    FUNCTION_MARKER(0x8004E624, "SLUS_003.73");
    while (1)
    {
        count = explosion_queue.count;
        result = (uint32)((sint32)index < (sint32)count);
        if (!result)
            return result;
        const ExplosionEvent *event = &explosion_queue.entries[index];
        distance = MeasurePositionDistance(model.pos.words, event->pos.words);
        radius = event->radius;
        if ((sint32)distance < (sint32)radius)
        {
            damage = event->strength;
            damage = DivideExplosionDamage(damage * (radius - distance), radius);
            Damage(damage, zero_vector.components, 0);
            result = (uint32)(sint32)this->health;
            if ((sint32)result <= 0)
                return result;
        }
        ++index;
    }
}

uint32 Swat::Damage(uint32 damage, const sint32 *input_vector, uint32 reason)
{
    ModelBorrow model_borrow;
    FUNCTION_MARKER(0x8004F5C8, "SLUS_003.73");
    uint32 count, flags, remaining;
    ModelInstance *hero;
    sint32 health;
    if ((uint32)this->control.variant == 64 && reason == 5)
        return 0;
    count = (uint32)this->control.damage_phase;
    flags = this->control.flags;
    this->control.damage_phase = count + 1;
    health = this->health;
    this->control.flags = flags | 0x400;
    remaining = (uint16)this->health - damage;
    if (health <= 0)
        return 1;
    this->health = (sint16)(uint16)(remaining);
    if ((sint32)(remaining << 16) > 0)
    {
        SetColor(3, 255, 255, 255);
        SpawnZombieFragment();
    }
    else
    {
        hero = visual_lists.players;
        if (hero)
            TryPlayPlayerKillQuote(hero->pos.words);
        flags = this->lifecycle_flags;
        motion.angular_acceleration.words[1] = 0;
        motion.angular_velocity.words[1] = 0;
        this->lifecycle_flags = flags & 0xFFEF;
        DetachTurret();
        DeathReaction(input_vector, reason);
    }
    return 1;
}

uint32 Swat::SpawnZombieFragment()
{
    uint32 trig_angle;
    uint32 angle;
    uint32 random;
    Vector3 velocity;
    GameHeapBlock *child;
    uint32 kind;
    uint32 height;
    uint32 flags;
    FUNCTION_MARKER(0x8004F6C4, "SLUS_003.73");
    if ((uint32)this->control.variant != 128)
        return 128;
    angle = RandomRange(4096);
    random = RandomRange(16);
    trig_angle = angle & 0xFFF;
    velocity.words[0] = (uint32)rsin(trig_angle) * (random + 24);
    random = RandomRange(16);
    velocity.words[1] = (0 - 32 - random) << 12;
    random = RandomRange(16);
    velocity.words[2] = (uint32)rcos(trig_angle) * (random + 24);
    child = AllocateZeroedObject(320);
    if (child)
    {
        random = RandomRange(fragment_count);
        kind = fragment_first;
        height = (uint32)(sint32)(sint16)(uint32)this->ground_offset << 12;
        height += this->model.pos.words[1];
        new (child) Debris(this->model.pos.words, velocity.words, (kind + random) & 0xFFFF, height, 9, 24, GetRecordModelBankName());
    }
    if (!RandomRange(2))
        return 0x10000;
    flags = hidden_models;
    if (!(flags & 0x10000))
        flags |= 0x100C0;
    else if (!(flags & 0x1000))
        flags |= 0x1300;
    else
    {
        uint32 result = flags | 7;
        if (!(flags & 1))
            hidden_models = result;
        return result;
    }
    hidden_models = flags;
    return flags;
}

void Swat::DeathReaction(const sint32 *vector, uint32 reason)
{
    FUNCTION_MARKER(0x8004F840, "SLUS_003.73");
    uint32 x, y, z, nonzero, flags, value, divisor, factor;
    Vector3 direction;
    ShortVector3 angles;
    x = vector[0];
    y = vector[1];
    z = vector[2];
    nonzero = x | y | z;
    if ((uint32)this->control.variant == 128)
    {
        flags = hidden_models;
        SetDeathState(16);
        this->control.script_active = 0;
        hidden_models = flags | 0x113FF;
        return;
    }
    if ((this->control.flags & 0x200) || this->model.animation == 4)
    {
        FragmentAndFinish(0);
        return;
    }
    motion.angular_velocity.words[2] = 0;
    motion.angular_velocity.words[1] = 0;
    motion.angular_velocity.words[0] = 0;
    motion.angular_acceleration.words[2] = 0;
    motion.angular_acceleration.words[1] = 0;
    motion.angular_acceleration.words[0] = 0;
    if (reason == 5)
    {
        if (!RandomRange(5))
            FragmentAndFinish(5);
        else
        {
            flags = this->control.flags;
            this->control.script_active = 0;
            SetDeathState(11);
            this->control.flags = flags | 4;
        }
        return;
    }
    if ((sint32)reason < 6 && reason == 2)
    {
        FragmentAndFinish(0);
        return;
    }
    if (reason == 9 || reason == 10)
    {
        if (((uint32)this->control.variant & 31) && !RandomRange(reason == 9 ? 3 : 4))
        {
            EnterDeathState14();
            return;
        }
        value = RandomRange(reason == 9 ? 3 : 2);
        if (reason == 9 && !value)
        {
            this->control.script_active = 0;
            SetDeathState(8);
        }
        else if (reason == 10 && value)
            EnterDeathState13();
        else
            FragmentAndFinish(0);
        return;
    }
    if (RandomRange(2))
    {
        FragmentAndFinish(0);
        return;
    }
    if (((uint32)this->control.variant & 31) && !RandomRange(5))
    {
        EnterDeathState14();
        return;
    }
    value = (uint32)this->control.variant;
    this->control.script_active = 0;
    if (value != 4)
    {
        SetDeathState(8);
        return;
    }
    if (nonzero)
    {
        x = vector[0];
        z = vector[2];
        direction.components[0] = (sint32)x;
        direction.components[1] = 0;
        direction.components[2] = (sint32)z;
        divisor = MeasureVectorLength(direction.words) + 1;
        if ((sint32)divisor < 128)
        {
            factor = (uint32)Divide(128, divisor);
            direction.components[0] = (sint32)((uint32)direction.components[0] * factor);
            direction.components[2] = (sint32)((uint32)direction.components[2] * factor);
        }
        CalculateDirectionAngles(angles.words, direction.words, zero_vector.words);
        this->model.angles.words[1] = (uint16)angles.components[1];
    }
    SetDeathState(10);
}

void Swat::FragmentAndFinish(uint32 mode)
{
    uint32 trig_angle;
    FUNCTION_MARKER(0x8004FB00, "SLUS_003.73");
    uint32 angle = RandomRange(1024);
    uint32 index;
    for (index = 0; index < 6; ++index)
    {
        Vector3 velocity;
        GameHeapBlock *child;
        if (player_distance >= 2048 || !visual_lists.cameras || RandomRange(4))
        {
            uint32 random = RandomRange(16);
            uint32 trig_angle = angle & 4095;
            velocity.words[0] = (uint32)rsin(trig_angle) * (random + 24);
            velocity.words[1] = (0 - 32 - RandomRange(16)) << 12;
            random = RandomRange(16);
            velocity.words[2] = (uint32)rcos(trig_angle) * (random + 24);
        }
        else
        {
            ModelInstance *target = visual_lists.cameras;
            Vector3 position;
            uint32 divisor;
            sint32 numerator;
            position = target->pos;
            position.words[1] += 0x80000;
            divisor = MeasurePositionDistance(this->model.pos.words, position.words) >> 7;
            // MIPS DIV defines LO even for zero
            numerator = (sint32)(position.words[0] - this->model.pos.words[0]);
            velocity.words[0] = (uint32)Divide(numerator, divisor);
            numerator = (sint32)(position.words[2] - this->model.pos.words[2]);
            velocity.words[2] = (uint32)Divide(numerator, divisor);
            numerator = (sint32)(position.words[1] - this->model.pos.words[1]);
            velocity.words[1] = ((uint32)Divide(numerator, divisor)) - (divisor << 13);
        }
        uint32 fragment_type = mode == 5 ? 5 : ((uint32)this->control.variant & 0x180) ? 9 : 3;
        child = AllocateZeroedObject(320);
        if (child)
        {
            uint32 variation = RandomRange(fragment_count);
            uint32 kind = (fragment_first + variation) & 65535;
            uint32 height = (uint32)(sint32)(sint16)(uint32)this->ground_offset << 12;
            height += this->model.pos.words[1];
            new (child) Debris(this->model.pos.words, velocity.words, kind, height, fragment_type, 24, GetRecordModelBankName());
            if (mode == 5)
                SetColor(0, 0, 0, 0);
        }
        angle += 682;
    }
    Finish();
}

uint32 Swat::EnterDeathState13()
{
    FUNCTION_MARKER(0x8004FE20, "SLUS_003.73");
    uint32 kind = (uint32)this->control.variant;
    uint32 mask, result;
    ai.state = 13;
    this->control.script_active = 0;
    ai.phase = 0;
    switch (kind)
    {
        case 1:
            mask = 0x1F80;
            break;
        case 2:
            mask = 0xFF80;
            break;
        case 4:
            mask = 0x7D;
            break;
        case 8:
            mask = 0x1FF80;
            break;
        default:
            return kind < 3 ? 1 : 8;
    }
    result = hidden_models | mask;
    hidden_models = result;
    return result;
}

void Swat::EnterDeathState14()
{
    FUNCTION_MARKER(0x8004FEC4, "SLUS_003.73");
    uint32 kind = (uint32)this->control.variant;
    uint32 index = kind - 1;
    uint32 mask, result;
    this->control.script_active = 0;
    ai.state = 14;
    ai.phase = 0;
    if (index >= 16)
        return;
    switch (kind)
    {
        case 1:
            mask = 0x1000;
            break;
        case 2:
            mask = 0x8000;
            break;
        case 4:
            mask = 4;
            break;
        case 8:
            mask = 0x10000;
            break;
        case 16:
            mask = 0x2000;
            break;
        default:
            return;
    }
    result = hidden_models | mask;
    hidden_models = result;
    return;
}

void Swat::UpdateState()
{
    ModelBorrow model_borrow;
    FUNCTION_MARKER(0x8004FF6C, "SLUS_003.73");
    CollisionRay raycast;

    Vector3 pos_output, effect_pos0, effect_pos1, sight_pos;
    ShortVector3 desired_angles, effect_angles;
    sint32 result;
    uint32 fall_phase;
    ModelInstance *facing_player;
    bool player_missing;
    bool sight_blocked;
    bool attack_wait;
    bool fade_elapsed;
    bool repeat_move;
    bool airborne_elapsed;
    bool below_floor;
    sint32 fall_ground_y;
    sint32 fall_ground_offset;
    sint32 fall_variant;
    sint16 fall_lifecycle_flags;
    sint32 landing_frame;
    sint32 fall_target_y;
    sint32 fall_height;
    sint32 landed_lifecycle_flags;
    sint32 idle_flags;
    sint32 idle_animation_complete;
    sint16 behavior;
    sint32 idle_variant;
    sint32 idle_sound_roll;
    sint32 idle_sound;
    sint32 fallback_state;
    sint32 approach_phase;
    sint32 minimum_radius;
    sint32 search_distance;
    sint32 approach_flags;
    sint32 attack_phase;
    sint32 sight_x;
    ModelInstance *sight_player;
    sint32 phase_before_advance;
    sint32 attack_ticks;
    sint32 move_phase;
    sint32 move_flags;
    sint16 player_heading;
    sint32 death_flags;
    sint32 death_variant;
    sint32 fade_ticks;
    sint16 next_fade_tick;
    sint16 fade_model_flags;
    uint32 retreat_phase;
    sint32 retreat_flags;
    sint32 retreat_y;
    sint32 retreat_z;
    sint32 retreat_distance;
    sint32 retreat_duration_numerator;
    sint32 clipped_retreat_y;
    sint32 clipped_retreat_z;
    sint32 retreat_velocity_y;
    sint32 retreat_velocity_z;
    sint32 free_retreat_velocity_y;
    sint32 free_retreat_velocity_z;
    sint32 retreat_ground_y;
    sint32 burst_idx;
    GameHeapBlock *burst_storage;
    sint32 retreat_ticks;
    sint32 death_ground_y;
    uint32 burn_phase;
    sint16 burn_phase_before_advance;
    sint32 burn_variant;
    sint32 burn_repeat_range;
    sint32 burn_repeat_roll;
    uint32 moving_death_phase;
    sint32 death_move_chosen;
    sint32 moving_burst_idx;
    sint32 moving_burst_speed;
    GameHeapBlock *moving_burst_storage;
    sint32 moving_death_flags;
    uint32 attached_death_phase;
    sint16 attached_lifecycle_flags;
    sint16 attached_phase_before_advance;
    sint32 attached_burst_idx;
    sint32 attached_burst_speed;
    GameHeapBlock *attached_burst_storage;
    sint32 attached_repeat_roll;
    sint32 final_burst_idx;
    sint32 final_burst_speed;
    GameHeapBlock *final_burst_storage;
    sint32 airborne_speed_roll;
    sint16 airborne_yaw_roll;
    sint16 airborne_pitch_speed;
    sint16 airborne_lifetime_roll;
    sint32 airborne_y;
    sint16 airborne_phase_before_advance;
    sint32 airborne_ticks;
    sint16 next_airborne_tick;
    sint32 airborne_floor;
    sint32 airborne_floor_y;
    sint32 airborne_velocity_y;
    uint32 emergence_phase;
    sint16 emergence_lifecycle_flags;
    sint32 rise_phase;
    sint32 rise_y;
    sint32 rise_ticks;
    sint16 rise_model_flags;
    sint32 scripted_rise_phase;
    sint32 rise_phase_before_advance;
    sint16 hidden_model_flags;
    sint32 maximum_radius;
    uint32 position_x;
    sint32 position_z;
    Vector3 retreat_vector;
    uint32 clipped_retreat_divisor;
    uint32 free_retreat_divisor;
    result = -2146828288;
    switch (ai.state)
    {
        case 0:
            fall_phase = ai.phase;
            result = (fall_phase < 2);
            if ((fall_phase == 1))
            {
                if (((control.flags & 2) != 0))
                {
                    result = (contact_flags & 2);
                    if (!result)
                        return;
                    landing_frame = ((sint8)(model.frame));
                    control.gravity_enabled = 0;
                }
                else
                {
                    fall_target_y = (sint32)((uint32)(sint16)ai.work[0] << 14);
                    fall_height = (sint32)((uint32)(sint16)(uint32)ground_offset << 12);
                    result = (fall_target_y < (sint32)(model.pos.words[1] + (uint32)fall_height));
                    if ((fall_target_y >= (sint32)(model.pos.words[1] + (uint32)fall_height)))
                        return;
                    model.pos.words[1] = ((uint32)fall_target_y - (uint32)fall_height);
                    control.gravity_enabled = 0;
                    motion.velocity.words[2] = 0;
                    motion.velocity.words[1] = 0;
                    landing_frame = ((sint8)(model.frame));
                    motion.velocity.words[0] = 0;
                }
                SetAnimationRange(4, landing_frame, -1);
                fall_variant = (uint32)control.variant;
                ai.phase = 2;
                result = 8;
                goto EnableLandingShadow;
            }
            if ((ai.phase < 2))
            {
                if (ai.phase)
                    return;
                facing_player = visual_lists.players;
                player_missing = (visual_lists.players == 0);
                control.flags = (control.flags & (~1));
                if (!player_missing)
                    model.angles.words[1] = (3072 - ratan2((sint32)(facing_player->pos.words[2] - model.pos.words[2]), (sint32)(facing_player->pos.words[0] - model.pos.words[0])));
                fall_ground_y = FindGroundHeight(model.pos.words, 0, 4096);
                if ((fall_ground_y == -1))
                {
                    (void)Finish();
                    return;
                }
                fall_ground_offset = ((sint16)((uint32)ground_offset));
                if (((sint32)((uint32)fall_ground_y - model.pos.words[1]) >> 12) >= 2 * fall_ground_offset)
                {
                    ai.work[0] = (fall_ground_y >> 14);
                    ai.phase = 1;
                    SetAnimationRange(4, 21, 22);
                    fall_lifecycle_flags = lifecycle_flags;
                    control.gravity_enabled = 1;
                    lifecycle_flags = fall_lifecycle_flags & 0xFFEF;
                    {
                        (void)RemoveShadow();
                        return;
                    }
                }
                model.pos.words[1] = ((uint32)fall_ground_y - ((uint32)fall_ground_offset << 12));
                ai.state = 1;
                control.gravity_enabled = 0;
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                fall_variant = (uint32)control.variant;
                result = 8;
                motion.velocity.words[0] = 0;
            EnableLandingShadow:
                if ((fall_variant != 8))
                {
                    (void)EnableShadow();
                    return;
                }

                return;
            }
            result = 2;
            if ((fall_phase == 2))
            {
                result = playback.complete;
                if (playback.complete)
                {
                    landed_lifecycle_flags = lifecycle_flags;
                    ai.SetStatePhase(1);
                    result = (landed_lifecycle_flags | 0x10);
                    lifecycle_flags = result;
                }
            }
            return;

        case 1:
            result = ai.phase;
            if (!(ai.phase))
            {
                idle_flags = control.flags;
                control.gravity_enabled = 0;
                control.flags = (idle_flags & 0xFFFFFFFE);
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                motion.velocity.words[0] = 0;
                motion.angular_velocity.words[2] = 0;
                motion.angular_velocity.words[1] = 0;
                motion.angular_velocity.words[0] = 0;
                motion.angular_acceleration.words[2] = 0;
                motion.angular_acceleration.words[1] = 0;
                idle_animation_complete = playback.complete;
                motion.angular_acceleration.words[0] = 0;
                if (idle_animation_complete)
                    SetAnimationRange(0, 0, -1);
                behavior = ChooseBehavior();
                idle_variant = (uint32)control.variant;
                ai.state = (behavior + 2);
                result = 4;
                ai.phase = 0;
                if ((idle_variant == 4))
                {
                    idle_sound_roll = RandomRange(16);
                    idle_sound = 43;
                    if (!idle_sound_roll)
                    {
                        (void)PlaySoundEffect(idle_sound, 0x2000, 0);
                        return;
                    }
                    result = RandomRange(16);
                    idle_sound = 44;
                    if (!result)
                    {
                        (void)PlaySoundEffect(idle_sound, 0x2000, 0);
                        return;
                    }
                }
            }
            return;

        case 2:
            fallback_state = ai.phase;
            result = 1;
            if (!(ai.phase))
            {
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                motion.velocity.words[0] = 0;
                SetAnimationRange(3, 0, -1);
                result = (ai.phase + 1);
                goto StorePhase;
            }
            if ((fallback_state == 1))
            {
                if (visual_lists.players)
                {
                    CalculateDirectionAngles(desired_angles.words, model.pos.words, visual_lists.players->pos.words);
                    desired_angles.components[0] = 0;
                    UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired_angles, 32);
                }
                result = playback.complete;
                if (playback.complete)
                {
                    motion.angular_acceleration.words[1] = 0;
                    motion.angular_velocity.words[1] = 0;
                    goto ResetFallbackState;
                }
            }
            return;

        case 3:
            approach_phase = ai.phase;
            result = 1;
            if (!(ai.phase))
            {
                minimum_radius = 128;
                maximum_radius = 192;
                search_distance = 1024;
                goto ChooseApproachPosition;
            }
            if ((approach_phase != 1))
                return;
            approach_flags = control.flags;
            if (((approach_flags & 0x400) != 0))
            {
                result = (approach_flags & 0xFFFFFFFE);
                control.flags = (approach_flags & 0xFFFFFFFE);
            FinishApproach:
                ai.SetStatePhase(((uint16)(approach_phase)));

                return;
            }
            result = (approach_flags & 1);
            if (((approach_flags & 1) == 0))
                goto FinishApproach;
            result = visual_lists.players != 0;
            if (visual_lists.players)
            {
                result = -2;
                if ((player_distance < behavior_ranges[0].minimum_distance))
                {
                    control.flags = (approach_flags & 0xFFFFFFFE);
                    result = 2;
                    goto StoreState;
                }
            }
            return;

        case 4:
            attack_phase = ai.phase;
            result = 1;
            if (!(ai.phase))
            {
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                sight_x = model.pos.words[0];
                sight_player = visual_lists.players;
                motion.velocity.words[0] = 0;
                sight_pos.words[0] = sight_x;
                sight_pos.words[1] = model.pos.words[1] - ((uint32)(sint16)(uint32)sight_offset << 12);
                sight_pos.words[2] = model.pos.words[2];
                sight_blocked = IsSegmentVisible(sight_pos.words, sight_player->pos.words) == 0;
                result = 1;
                if (sight_blocked)
                    goto StoreState;
                BeginAttack();
                phase_before_advance = ai.phase;
                ai.work[0] = 0;
                goto AdvancePhase;
            }
            if ((attack_phase == 1))
            {
                attack_ticks = ((sint16)(ai.work[0]));
                attack_wait = (attack_ticks < ((uint16)(GetBehaviorValue(1, 3))));
                result = (attack_ticks + 1);
                if (attack_wait)
                    ai.work[0] = result;
                else
                    ai.SetStatePhase(((uint16)(attack_phase)));
                if (visual_lists.players)
                {
                    CalculateDirectionAngles(desired_angles.words, model.pos.words, visual_lists.players->pos.words);
                    desired_angles.components[0] = 0;

                    {
                        (void)UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired_angles, 32);
                        return;
                    }
                }
            }
            return;

        case 5:
            move_phase = ai.phase;
            if (!(ai.phase))
            {
                minimum_radius = 1024;
                maximum_radius = 2048;
                search_distance = 512;
            ChooseApproachPosition:
                fallback_state = 1;

                result = ChoosePosition(destination.words, visual_lists.players->pos.words, minimum_radius, maximum_radius, (uint32)control.height_limit, search_distance, 1, 1);
                goto ApplyChosenPosition;
            }
            result = 1;
            if ((move_phase == 1))
            {
                move_flags = control.flags;
                if (((move_flags & 0x400) == 0))
                    goto WaitForMovement;
                result = (move_flags & 0xFFFFFFFE);
                control.flags = (move_flags & 0xFFFFFFFE);
                goto FinishMovement;
            }
            return;

        case 6:
            if (ai.phase)
            {
                result = 128;
                if ((ai.phase == 1))
                {
                    if ((((uint32)control.variant == 128) && visual_lists.players))
                    {
                        player_heading = ratan2((sint32)(visual_lists.players->pos.words[2] - model.pos.words[2]), (sint32)(visual_lists.players->pos.words[0] - model.pos.words[0]));
                        desired_angles.components[0] = 0;
                        desired_angles.components[1] = (3072 - player_heading) & 0xFFF;
                        desired_angles.components[2] = 0;
                        UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired_angles, 32);
                    }
                    result = 1;
                    if (((control.flags & 0x400) != 0))
                        goto StoreState;
                    result = 1;
                    if (playback.complete)
                        goto StoreState;
                    result = behavior_ranges[2].start_weight;
                    if (result)
                    {
                        result = 1;
                        if ((player_distance < behavior_ranges[2].maximum_distance))
                        {
                            ai.state = 1;
                            goto ResetPhase;
                        }
                    }
                }
            }
            else
            {
                if (model.animation)
                    SetAnimationRange(0, 0, -1);
                result = 1;
                ai.phase = 1;
            }
            return;

        case 7:
            move_phase = ai.phase;
            result = 1;
            if (ai.phase)
            {
                if ((move_phase != 1))
                    return;
                move_flags = control.flags;
                if (((move_flags & 0x400) != 0))
                {
                    result = (move_flags & 0xFFFFFFFE);
                    control.flags = (move_flags & 0xFFFFFFFE);
                }
                else
                {
                WaitForMovement:
                    result = (move_flags & 1);

                    if (((move_flags & 1) != 0))
                        return;
                }
            FinishMovement:
                ai.SetStatePhase(((uint16)(move_phase)));
            }
            else
            {
                fallback_state = 1;
                result = ChoosePosition(destination.words, model.pos.words, 1024, 2048, (uint32)control.height_limit, 4096, 1, 1);
            ApplyChosenPosition:

                if (result)
                    goto StartMovement;
            ResetFallbackState:
                ai.state = fallback_state;

                ai.phase = 0;
            }
            return;

        case 8:
            result = 1;
            if (ai.phase)
            {
                if ((ai.phase == 1))
                {
                    result = playback.complete;
                    if (playback.complete)
                    {
                        fade_ticks = ((sint16)(ai.work[0]));
                        fade_elapsed = (fade_ticks == 0);
                        next_fade_tick = (fade_ticks - 1);
                        if (fade_elapsed)
                            Finish();
                        else
                            ai.work[0] = next_fade_tick;
                        result = ((sint16)(ai.work[1]));
                        if (!(ai.work[1]))
                        {
                            SetColor(-32, 0, 0, 0);
                            fade_model_flags = model.flags;
                            model.render_mode = 128;
                            model.flags = (fade_model_flags | 0x800);
                            result = 1;
                            ai.work[1] = 1;
                        }
                    }
                }
            }
            else
            {
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                motion.velocity.words[0] = 0;
                motion.angular_velocity.words[2] = 0;
                motion.angular_velocity.words[1] = 0;
                motion.angular_velocity.words[0] = 0;
                motion.angular_acceleration.words[2] = 0;
                motion.angular_acceleration.words[1] = 0;
                death_flags = control.flags;
                motion.angular_acceleration.words[0] = 0;
                collision_radius = 0;
                control.flags = (death_flags & 0xFFFFFFFE);
                ActivateLinkedRecords();
                SetAnimationRange(5, 0, -1);
                RemoveShadow();
                death_variant = (uint32)control.variant;
                ai.phase = 1;
                ai.work[0] = 32;
                result = 256;
                ai.work[1] = 0;
                if ((death_variant == 256))
                {
                    (void)PlayPositionedSound(120, model.pos.words, 3);
                    return;
                }
            }
            return;

        case 0xA:
            retreat_phase = ai.phase;
            result = (retreat_phase < 2);
            if ((retreat_phase == 1))
            {
                if (ai.work[0])
                {
                    SetParticleAngles(model.angles.components);
                    ::SetParticleSize(2);
                    burst_idx = 0;
                    SetParticleColor((uint8)GetGameTuning().death_particle_color[0], (uint8)GetGameTuning().death_particle_color[1], (uint8)GetGameTuning().death_particle_color[2]);
                    SetParticleColorStep(((((uint16)(GetGameTuning().death_particle_color[0])) >> 5) & 255), ((((uint16)(GetGameTuning().death_particle_color[1])) >> 5) & 255), ((((uint16)(GetGameTuning().death_particle_color[2])) >> 5) & 255));
                    use_small_effect_pool = false;
                    do
                    {
                        burst_storage = AllocateEffectStorage(88);
                        if (burst_storage)
                        {
                            new (burst_storage) FadingParticle(model.pos.words, 16, 0x2000, 16);
                        }
                        ++burst_idx;
                    } while ((burst_idx < 16));
                    retreat_ticks = ((sint16)(ai.work[0]));
                    use_small_effect_pool = true;
                    result = (retreat_ticks - 1);
                    ai.work[0] = result;
                    return;
                }
            StopAndAdvancePhase:
                motion.velocity.words[2] = 0;

                motion.velocity.words[1] = 0;
                phase_before_advance = ai.phase;
                motion.velocity.words[0] = 0;
            AdvancePhase:
                result = (phase_before_advance + 1);

                goto StorePhase;
            }
            if ((ai.phase < 2))
            {
                if (ai.phase)
                    return;
                model.angles.words[0] = 0;
                motion.angular_velocity.words[2] = 0;
                motion.angular_velocity.words[1] = 0;
                motion.angular_velocity.words[0] = 0;
                motion.angular_acceleration.words[2] = 0;
                motion.angular_acceleration.words[1] = 0;
                retreat_flags = control.flags;
                motion.angular_acceleration.words[0] = 0;
                collision_radius = 0;
                control.flags = (retreat_flags & 0xFFFFFFFE);
                RemoveShadow();
                BuildDirectionVector(destination.words, 1024, model.angles.words);
                SubtractVectors(pos_output.words, model.pos.words, destination.words);
                position_x = pos_output.words[0];
                position_z = pos_output.words[2];
                retreat_y = (sint32)pos_output.words[1];
                retreat_z = position_z;
                destination.words[0] = position_x;
                destination.words[1] = retreat_y;
                destination.words[2] = retreat_z;
                raycast.endpoints[0].words[0] = model.pos.words[0];
                raycast.endpoints[0].words[1] = model.pos.words[1];
                raycast.endpoints[0].words[2] = model.pos.words[2];
                raycast.endpoints[1].words[0] = destination.words[0];
                raycast.endpoints[1].words[1] = destination.words[1];
                raycast.endpoints[1].words[2] = destination.words[2];
                InitializeCollisionRay(raycast);
                raycast.triggers = 0;
                QueryCollisionRay(raycast, 1);
                if (raycast.hit.GetObject())
                {
                    retreat_distance = ((sint32)((uint32)raycast.distance - 256));
                    if ((((sint32)((uint32)raycast.distance - 256)) >= 64))
                    {
                        retreat_duration_numerator = ((sint32)((uint32)raycast.distance - 256));
                        if ((retreat_distance < 0))
                            retreat_duration_numerator = ((sint32)((uint32)raycast.distance - 193));
                        ai.work[0] = (retreat_duration_numerator >> 6);
                        BuildDirectionVector(destination.words, retreat_distance, model.angles.words);
                        SubtractVectors(pos_output.words, model.pos.words, destination.words);
                        position_x = pos_output.words[0];
                        position_z = pos_output.words[2];
                        clipped_retreat_y = (sint32)pos_output.words[1];
                        clipped_retreat_z = position_z;
                        destination.words[0] = position_x;
                        destination.words[1] = clipped_retreat_y;
                        destination.words[2] = clipped_retreat_z;
                        SubtractVectors(retreat_vector.words, destination.words, model.pos.words);
                        clipped_retreat_divisor = ((sint16)(ai.work[0]));
                        DivideVector(pos_output.words, retreat_vector.words, clipped_retreat_divisor);
                        position_x = pos_output.words[0];
                        position_z = pos_output.words[2];
                        retreat_velocity_y = (sint32)pos_output.words[1];
                        retreat_velocity_z = position_z;
                        motion.velocity.words[0] = position_x;
                        motion.velocity.words[1] = retreat_velocity_y;
                        motion.velocity.words[2] = retreat_velocity_z;
                        goto StartRetreatDeath;
                    }
                    motion.velocity.words[2] = 0;
                    motion.velocity.words[1] = 0;
                    motion.velocity.words[0] = 0;
                    ai.work[0] = 1;
                }
                else
                {
                    ai.work[0] = 16;
                    SubtractVectors(pos_output.words, destination.words, model.pos.words);
                    position_x = pos_output.words[0];
                    position_z = pos_output.words[2];
                    free_retreat_divisor = 16;
                    DivideVector(retreat_vector.words, pos_output.words, free_retreat_divisor);
                    free_retreat_velocity_y = retreat_vector.words[1];
                    free_retreat_velocity_z = retreat_vector.words[2];
                    motion.velocity.words[0] = retreat_vector.words[0];
                    motion.velocity.words[1] = free_retreat_velocity_y;
                    motion.velocity.words[2] = free_retreat_velocity_z;
                    retreat_ground_y = FindGroundHeight(destination.words, ((sint16)((uint32)ground_offset)), (4 * ((sint16)((uint32)ground_offset))));
                    if ((retreat_ground_y != -1))
                    {
                    StartRetreatDeath:
                        SpawnExplosionRing(model.pos.words, 128, 128, 64, 0, 5, 0, 100);

                        SetAnimationRange(5, 0, -1);
                        PlaySoundEffect(45, 0x2000, 0);
                        collision_radius = 0;
                        ActivateLinkedRecords();
                        result = (ai.phase + 1);
                        goto StorePhase;
                    }
                    control.gravity_enabled = 1;
                }
                goto StartRetreatDeath;
            }
            result = 2;
            if ((retreat_phase == 2))
            {
                if ((uint32)control.gravity_enabled)
                {
                FinishAfterAnimation:
                    result = playback.complete;

                    if (playback.complete)
                    {
                        (void)Finish();
                        return;
                    }
                    return;
                }
                result = playback.complete;
                if (!(playback.complete))
                    return;
                death_ground_y = FindGroundHeight(model.pos.words, 0, 256);
                if ((death_ground_y == -1))
                {
                    (void)FragmentAndFinish(0);
                    return;
                }
                result = (ai.phase + 1);
            StorePhase:
                ai.phase = result;
            }
            return;

        case 0xB:
            burn_phase = ai.phase;
            result = (burn_phase < 2);
            if ((burn_phase == 1))
                goto UpdateBurnAnimation;
            if ((ai.phase >= 2))
            {
                result = 256;
                if ((burn_phase != 2))
                    return;
                if (((uint32)control.variant == 256))
                {
                    ::StopSoundVoices(ai.script_data);
                    ai.script_data = 0;
                }
                goto FinishAfterAnimation;
            }
            if (ai.phase)
                return;
            SetColor(0, 255, 0, 0);
            RemoveShadow();
            ActivateLinkedRecords();
            model_effect.color_mode = 37765184;
            model_effect.scale = 10240;
            collision_radius = 0;
            model_effect.depth_bias = 10;
            SetAnimationRange(6, 0, -1);
            if (((uint32)control.variant == 256))
                ai.script_data = PlayPositionedSound(18, model.pos.words, 3);
            else
                PlayPositionedSound(25, model.pos.words, 3);
            control.flags = (control.flags & (~1));
            motion.velocity.words[2] = 0;
            motion.velocity.words[1] = 0;
            burn_phase_before_advance = ai.phase;
            motion.velocity.words[0] = 0;
            ai.phase = (burn_phase_before_advance + 1);
        UpdateBurnAnimation:
            result = 256;

            if (!(playback.complete))
                return;
            burn_variant = (uint32)control.variant;
            if ((burn_variant != 256))
            {
                if (!RandomRange(4))
                    PlayPositionedSound(25, model.pos.words, 3);
                burn_variant = (uint32)control.variant;
            }
            burn_repeat_range = 4;
            if ((burn_variant == 256))
                burn_repeat_range = 2;
            burn_repeat_roll = RandomRange(burn_repeat_range);
            if ((!burn_repeat_roll || IsGameHeapUnderPressure()))
            {
                SetAnimationRange(9, 0, -1);
                result = (ai.phase + 1);
                goto StorePhase;
            }
            {
                (void)SetAnimationRange(6, 0, -1);
                return;
            }

        case 0xD:
            moving_death_phase = ai.phase;
            result = (moving_death_phase < 2);
            if ((moving_death_phase == 1))
            {
                effect_angles.components[0] = -1024;
                effect_angles.components[1] = 0;
                effect_angles.components[2] = 0;
                SetParticleAngles(effect_angles.components);
                effect_angles.components[0] = 512;
                effect_angles.components[1] = 4096;
                effect_angles.components[2] = 0;
                SetParticleSpread(effect_angles.components);
                ::SetParticleSize(2);
                moving_burst_idx = 0;
                SetParticleColor((uint8)GetGameTuning().death_particle_color[0], (uint8)GetGameTuning().death_particle_color[1], (uint8)GetGameTuning().death_particle_color[2]);
                moving_burst_speed = 24;
                SetParticleColorStep(((((uint16)(GetGameTuning().death_particle_color[0])) >> 5) & 255), ((((uint16)(GetGameTuning().death_particle_color[1])) >> 5) & 255), ((((uint16)(GetGameTuning().death_particle_color[2])) >> 5) & 255));
                use_small_effect_pool = false;
                do
                {
                    moving_burst_storage = AllocateEffectStorage(88);
                    if (moving_burst_storage)
                    {
                        new (moving_burst_storage) FadingParticle(model.pos.words, moving_burst_speed, 0x2000, 16);
                    }
                    ++moving_burst_idx;
                    moving_burst_speed += 4;
                } while ((moving_burst_idx < 4));
                moving_death_flags = control.flags;
                use_small_effect_pool = true;
                result = (moving_death_flags & 1);
                if (result)
                    return;
                repeat_move = (RandomRange(2) == 0);
                result = 8;
                if (!repeat_move)
                {
                    result = (ai.phase - 1);
                    ai.phase = result;
                    return;
                }
            }
            else
            {
                if ((ai.phase >= 2))
                {
                    result = 2;
                    if ((moving_death_phase == 2))
                        goto FinishAfterAnimation;
                    return;
                }
                if (ai.phase)
                    return;
                death_move_chosen = ChoosePosition(destination.words, model.pos.words, 256, 512, (uint32)control.height_limit, 4096, 1, 1);
                if (death_move_chosen)
                {
                StartMovement:
                    result = SetMoveTarget(destination.words);

                    ai.phase = 1;
                    return;
                }
                result = 8;
            }
        StoreState:
            ai.state = result;

        ResetPhase:
            ai.phase = 0;

            return;

        case 0xE:
            attached_death_phase = ai.phase;
            result = (attached_death_phase < 2);
            if ((attached_death_phase == 1))
                goto EmitAttachedDeathBurst;
            if ((ai.phase >= 2))
            {
                result = 2;
                if ((attached_death_phase != 2))
                    return;
                effect_angles.components[0] = 512;
                effect_angles.components[1] = 4096;
                effect_angles.components[2] = 0;
                SetParticleSpread(effect_angles.components);
                ::SetParticleSize(2);
                final_burst_idx = 0;
                SetParticleColor((uint8)GetGameTuning().death_particle_color[0], (uint8)GetGameTuning().death_particle_color[1], (uint8)GetGameTuning().death_particle_color[2]);
                final_burst_speed = 32;
                SetParticleColorStep(((((uint16)(GetGameTuning().death_particle_color[0])) >> 5) & 255), ((((uint16)(GetGameTuning().death_particle_color[1])) >> 5) & 255), ((((uint16)(GetGameTuning().death_particle_color[2])) >> 5) & 255));
                GetModelAttachment(effect_pos0.words, *this, 1);
                effect_angles.components[0] = -1024;
                effect_angles.components[1] = 0;
                effect_angles.components[2] = 0;
                SetParticleAngles(effect_angles.components);
                use_small_effect_pool = false;
                effect_pos0.words[1] += 0x20000;
                do
                {
                    final_burst_storage = AllocateEffectStorage(88);
                    if (final_burst_storage)
                    {
                        new (final_burst_storage) FadingParticle(effect_pos0.words, final_burst_speed, 0x2000, 16);
                    }
                    ++final_burst_idx;
                    final_burst_speed += 4;
                } while ((final_burst_idx < 4));
                use_small_effect_pool = true;
                goto FinishAfterAnimation;
            }
            if (ai.phase)
                return;
            RemoveShadow();
            attached_lifecycle_flags = lifecycle_flags;
            collision_radius = 0;
            lifecycle_flags = attached_lifecycle_flags & 0xFFEF;
            ActivateLinkedRecords();
            SetAnimationRange(6, 0, -1);
            control.flags = ((control.flags & 0xFFFFEFFE) | 0x1000);
            motion.velocity.words[2] = 0;
            motion.velocity.words[1] = 0;
            attached_phase_before_advance = ai.phase;
            motion.velocity.words[0] = 0;
            ai.phase = (attached_phase_before_advance + 1);
        EmitAttachedDeathBurst:
            effect_angles.components[0] = -1024;
            effect_angles.components[1] = 0;
            effect_angles.components[2] = 0;
            SetParticleAngles(effect_angles.components);
            effect_angles.components[0] = 512;
            effect_angles.components[1] = 4096;
            effect_angles.components[2] = 0;
            SetParticleSpread(effect_angles.components);
            ::SetParticleSize(2);
            attached_burst_idx = 0;
            SetParticleColor((uint8)GetGameTuning().death_particle_color[0], (uint8)GetGameTuning().death_particle_color[1], (uint8)GetGameTuning().death_particle_color[2]);
            attached_burst_speed = 32;
            SetParticleColorStep(((((uint16)(GetGameTuning().death_particle_color[0])) >> 5) & 255), ((((uint16)(GetGameTuning().death_particle_color[1])) >> 5) & 255), ((((uint16)(GetGameTuning().death_particle_color[2])) >> 5) & 255));
            GetModelAttachment(effect_pos1.words, *this, 1);
            use_small_effect_pool = false;
            effect_pos1.words[1] += 0x20000;
            do
            {
                attached_burst_storage = AllocateEffectStorage(88);
                if (attached_burst_storage)
                {
                    new (attached_burst_storage) FadingParticle(effect_pos1.words, attached_burst_speed, 0x2000, 16);
                }
                ++attached_burst_idx;
                attached_burst_speed += 4;
            } while ((attached_burst_idx < 4));
            use_small_effect_pool = true;
            result = playback.complete;
            if (!(playback.complete))
                return;
            attached_repeat_roll = RandomRange(2);
            if (attached_repeat_roll)
            {
                (void)SetAnimationRange(6, 0, -1);
                return;
            }
            SetAnimationRange(9, 0, -1);
            result = (ai.phase + 1);
            goto StorePhase;

        case 0x10:
            result = 1;
            if (!(ai.phase))
            {
                airborne_speed_roll = RandomRange(64);
                motion.velocity.words[0] = 0;
                motion.velocity.words[1] = (0 - ((uint32)airborne_speed_roll + 96)) << 12;
                motion.velocity.words[2] = 0;
                airborne_pitch_speed = (RandomRange(256) - 128);
                airborne_yaw_roll = RandomRange(256);
                motion.angular_velocity.words[0] = airborne_pitch_speed;
                motion.angular_velocity.words[1] = (airborne_yaw_roll - 128);
                motion.angular_velocity.words[2] = 0;
                motion.angular_drag[0] = 16;
                motion.angular_drag[1] = 16;
                motion.angular_drag[2] = 16;
                control.gravity_enabled = 1;
                control.flags = (control.flags & (~1));
                airborne_lifetime_roll = RandomRange(32);
                airborne_y = model.pos.words[1];
                ai.work[0] = (airborne_lifetime_roll + 16);
                result = ((uint32)ground_offset + ((sint32)airborne_y >> 12));
                airborne_phase_before_advance = (ai.phase + 1);
                ai.work[1] = result;
                ai.phase = airborne_phase_before_advance;
                return;
            }
            if ((ai.phase != 1))
                return;
            airborne_ticks = ((sint16)(ai.work[0]));
            airborne_elapsed = (airborne_ticks == 0);
            next_airborne_tick = (airborne_ticks - 1);
            if (airborne_elapsed)
            {
                (void)FragmentAndFinish(0);
                return;
            }
            ai.work[0] = next_airborne_tick;
            airborne_floor = ((sint16)(ai.work[1]));
            result = (airborne_floor < (((sint32)(model.pos.words[1])) >> 12));
            below_floor = (airborne_floor >= (((sint32)(model.pos.words[1])) >> 12));
            airborne_floor_y = (sint32)((uint32)airborne_floor << 12);
            if (!below_floor)
            {
                airborne_velocity_y = motion.velocity.words[1];
                model.pos.words[1] = airborne_floor_y;
                result = (sint32)(0 - (uint32)airborne_velocity_y);
                motion.velocity.words[1] = result;
            }
            return;

        case 0x20:
            emergence_phase = ai.phase;
            result = (emergence_phase < 2);
            if ((emergence_phase == 1))
            {
                if (((control.flags & 1) == 0))
                    goto StopAndAdvancePhase;
                result = visual_lists.players != 0;
                if (visual_lists.players)
                {
                    result = 2;
                    if ((player_distance < behavior_ranges[0].minimum_distance))
                        ai.phase = 2;
                }
            }
            else if ((ai.phase >= 2))
            {
                result = 2;
                if ((emergence_phase == 2))
                {
                    control.flags = (control.flags & (~1));
                    SetAnimationRange(8, 0, -1);
                    emergence_lifecycle_flags = lifecycle_flags;
                    collision_radius = 128;
                    result = 8;
                    control.script_delay = 8;
                    ai.SetStatePhase(1);
                    lifecycle_flags = emergence_lifecycle_flags | 0x10;
                }
            }
            else if (!(ai.phase))
            {
                if (ChoosePosition(destination.words, visual_lists.players->pos.words, 128, 192, (uint32)control.height_limit, 1024, 1, 1))
                {
                    result = SetMoveTarget(destination.words);
                    ai.phase = 1;
                }
                else
                {
                    result = 2;
                    ai.phase = 2;
                }
            }
            return;

        case 0x30:
            rise_phase = ai.phase;
            result = 1;
            if (!(ai.phase))
            {
                lifecycle_flags = (lifecycle_flags & (~0x10));
                rise_y = model.pos.words[1];
                goto StartRise;
            }
            if ((rise_phase != 1))
                return;
            rise_ticks = ((sint16)(ai.work[1]));
            if ((rise_ticks != 60))
                goto StepRise;
            lifecycle_flags |= 0x10;
            SetAnimationRange(0, 0, -1);
            rise_model_flags = model.flags;
            ai.SetStatePhase(((uint16)(rise_phase)));
            result = (rise_model_flags & 0xFFF7);
            model.flags = result;
            return;

        case 0x31:
            scripted_rise_phase = ai.phase;
            result = 1;
            if (ai.phase)
            {
                if ((scripted_rise_phase == 1))
                {
                    rise_ticks = ((sint16)(ai.work[1]));
                    if ((rise_ticks == 60))
                    {
                        model.flags = (model.flags & (~8));
                        SetAnimationRange(0, 0, -1);
                        result = 1;
                        ai.SetStatePhase(((uint16)(scripted_rise_phase)));
                        control.script_active = 1;
                    }
                    else
                    {
                    StepRise:
                        ai.work[1] = rise_ticks + 1;

                        result = (model.pos.words[1] - 34952);
                        model.pos.words[1] = result;
                    }
                }
            }
            else
            {
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                rise_y = model.pos.words[1];
                motion.velocity.words[0] = 0;
            StartRise:
                model.pos.words[1] = ((uint32)rise_y + 0x200000);

                StartAnimation(0, 1);
                rise_phase_before_advance = ai.phase;
                hidden_model_flags = model.flags;
                ai.work[1] = 0;
                result = (rise_phase_before_advance + 1);
                ai.phase = result;
                model.flags = (hidden_model_flags | 8);
            }
            return;

        default:
            return;
    }
}

void Swat::SetBehaviorRange(uint32 index, uint32 minimum_distance, uint32 maximum_distance, uint32 start_weight, uint32 end_weight)
{
    FUNCTION_MARKER(0x8005171C, "SLUS_003.73");
    BehaviorRange *range = behavior_ranges + index;
    range->minimum_distance = minimum_distance;
    range->maximum_distance = maximum_distance;
    range->start_weight = start_weight;
    range->end_weight = end_weight;
}

uint32 Swat::SetMode(uint32 mode)
{
    FUNCTION_MARKER(0x80051740, "SLUS_003.73");
    sint16 scaled_model_flags;
    GameHeapBlock *weapon_storage;
    uint16 weapon_damage;
    sint16 prisoner_model_flags;
    sint8 fragment_total;
    sint32 zombie_count;
    sint32 ground_y;
    sint32 ground_height;
    sint32 result;
    uint32 own_y;
    uint8 bank_slot;
    sint16 shadow_height;
    mode = (uint16)mode;
    switch (mode)
    {
        case 0:
            SetModelBank("swat");
            control.variant = 1;
            health = 20;
            ground_offset = 130;
            sight_offset = 64;
            attack_frame = 10;
            AllocateBehaviorTable(2, 5);
            ai.parameters[0] = 32;
            ai.parameters[1] = 32;
            SetBehaviorValue(0, 1, 20);
            SetBehaviorValue(1, 1, 10);
            SetBehaviorValue(1, 2, 64);
            SetBehaviorValue(1, 3, 16);
            SetBehaviorValue(1, 4, 45);
            goto SetSwatBehaviorRanges;

        case 1:
            SetModelBank("swat");
            control.variant = 1;
            scaled_model_flags = model.flags;
            model.scale.words[2] = 6144;
            model.scale.words[1] = 6144;
            model.scale.words[0] = 6144;
            model.flags = (scaled_model_flags | 0x200);
            SetColor(0, 208, 96, 96);
            health = 60;
            ground_offset = 200;
            sight_offset = 96;
            attack_frame = 10;
            AllocateBehaviorTable(2, 5);
            ai.parameters[0] = 32;
            ai.parameters[1] = 32;
            SetBehaviorValue(0, 1, 20);
            SetBehaviorValue(1, 1, 10);
            SetBehaviorValue(1, 2, 64);
            SetBehaviorValue(1, 3, 32);
            SetBehaviorValue(1, 4, 90);
            SetBehaviorRange(2, 0, 4096, 256, 128);
            SetBehaviorRange(4, 4064, 0x4000, 128, 128);
            EnableShadow();
            bank_slot = (uint8)GetRecordModelBankSlot();
            shadow_height = (uint32)ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 96;
            goto SetSwatFragments;

        case 2:
            SetModelBank("swat");
            control.variant = 1;
            health = 20;
            ground_offset = 130;
            sight_offset = 64;
            attack_frame = 10;
            AllocateBehaviorTable(2, 5);
            ai.parameters[0] = 32;
            ai.parameters[1] = 32;
            SetBehaviorValue(0, 1, 20);
            SetBehaviorValue(1, 1, 10);
            SetBehaviorValue(1, 2, 64);
            SetBehaviorValue(1, 3, 16);
            SetBehaviorValue(1, 4, 45);
            weapon_storage = AllocateZeroedHeapBlock(124);
            if (weapon_storage)
            {
                weapon = new (weapon_storage) SpiralRayWeapon(&visual_lists.players);
            }
            else
                weapon = 0;
            if (!weapon)
            {
                GameDiagnostic("Swat weapon allocation failed\n");
                abort();
            }
            weapon_damage = GetBehaviorValue(1, 1);
            weapon->damage = weapon_damage;
        SetSwatBehaviorRanges:
            SetBehaviorRange(0, 128, 192, 256, 0);

            SetBehaviorRange(1, 0, 512, 128, 16);
            SetBehaviorRange(2, 512, 4096, 256, 0);
            SetBehaviorRange(3, 1024, 12288, 128, 96);
            SetBehaviorRange(4, 2048, 12288, 128, 256);
            SetBehaviorRange(5, 1024, 12288, 64, 256);
            EnableShadow();
            bank_slot = (uint8)GetRecordModelBankSlot();
            shadow_height = (uint32)ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 64;
        SetSwatFragments:
            shadow.height = shadow_height;

            fragment_first = ::FindModelResourceIndex(-617804721, bank_slot);
            fragment_count = 10;
            goto PlaceOnGround;

        case 16:
            SetModelBank("prisoner");
            control.variant = 2;
            health = 1;
            ground_offset = 112;
            model.scale.words[0] = 7000;
            model.scale.words[1] = 5000;
            model.scale.words[2] = 0x2000;
            prisoner_model_flags = model.flags;
            sight_offset = 32;
            attack_frame = 15;
            model.flags = (prisoner_model_flags | 0x200);
            AllocateBehaviorTable(2, 5);
            ai.parameters[0] = 16;
            ai.parameters[1] = 32;
            SetBehaviorValue(0, 1, 5);
            SetBehaviorRange(0, 128, 192, 256, 0);
            SetBehaviorRange(1, 0, 1024, 128, 64);
            goto SetHumanoidRangesAndFragments;

        case 32:

        case 33:

        case 34:

        case 35:
            SetModelBank("guard");
            control.variant = 4;
            ground_offset = 128;
            sight_offset = 128;
            attack_frame = 15;
            AllocateBehaviorTable(2, 5);
            EnableShadow();
            bank_slot = (uint8)GetRecordModelBankSlot();
            shadow_height = (uint32)ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 96;
            shadow.height = shadow_height;
            fragment_first = ::FindModelResourceIndex(-317030919, bank_slot);
            fragment_count = 10;
            switch (mode)
            {
                case 32:
                    health = 30;
                    ai.parameters[0] = 32;
                    ai.parameters[1] = 32;
                    SetBehaviorValue(0, 1, 10);
                    SetBehaviorValue(1, 1, 5);
                    SetBehaviorValue(1, 2, 48);
                    SetBehaviorValue(1, 3, 16);
                    SetBehaviorValue(1, 4, 45);
                    SetBehaviorRange(0, 128, 192, 256, 0);
                    SetBehaviorRange(1, 0, 1024, 128, 64);
                    SetBehaviorRange(2, 512, 4096, 256, 64);
                    SetBehaviorRange(3, 1024, 0x2000, 128, 96);
                    SetBehaviorRange(4, 2048, 0x2000, 64, 128);
                    SetBehaviorRange(5, 1024, 0x2000, 64, 256);
                    goto PlaceOnGround;

                case 33:
                    health = 30;
                    ai.parameters[0] = 32;
                    ai.parameters[1] = 32;
                    SetBehaviorValue(0, 1, 10);
                    SetBehaviorValue(1, 1, 5);
                    SetBehaviorValue(1, 2, 48);
                    SetBehaviorValue(1, 3, 16);
                    SetBehaviorValue(1, 4, 45);
                    SetBehaviorRange(0, 128, 256, 256, 0);
                    SetBehaviorRange(2, 232, 4096, 256, 0);
                    SetBehaviorRange(4, 2048, 12288, 128, 256);
                    break;

                case 34:
                    health = 30;
                    ai.parameters[0] = 32;
                    ai.parameters[1] = 32;
                    SetBehaviorValue(0, 1, 10);
                    SetBehaviorValue(1, 1, 5);
                    SetBehaviorValue(1, 2, 48);
                    SetBehaviorValue(1, 3, 16);
                    SetBehaviorValue(1, 4, 45);
                    SetBehaviorRange(0, 128, 192, 256, 0);
                    SetBehaviorRange(1, 0, 4096, 0, 128);
                    SetBehaviorRange(4, 3072, 12288, 32, 256);
                    break;

                default:
                    if ((mode != 35))
                        goto PlaceOnGround;
                    health = 30;
                    SetBehaviorValue(1, 1, 5);
                    SetBehaviorValue(1, 2, 48);
                    SetBehaviorValue(1, 3, 32);
                    SetBehaviorValue(1, 4, 45);
                    SetBehaviorRange(2, 0, 4096, 256, 0);
                    SetBehaviorRange(4, 4096, 0x4000, 256, 256);
                    break;
            }

        PlaceOnGround:
            ground_y = FindGroundHeight(model.pos.words, 64, 512);

            if ((ground_y == -1))
                return RemoveShadow();
            own_y = model.pos.words[1];
            ground_height = (sint16)(uint32)ground_offset;
            result = (sint32)((uint32)ground_height << 12);
            if (((sint32)((uint32)ground_y - own_y) >> 12) < (sint32)((uint32)ground_height << 1))
            {
                result = (sint32)((uint32)ground_y - (uint32)result);
                model.pos.words[1] = result;
            }
            return result;

        case 48:

        case 49:

        case 50:
            SetModelBank("scum");
            control.variant = 8;
            health = 10;
            ground_offset = 140;
            sight_offset = 32;
            attack_frame = 15;
            AllocateBehaviorTable(2, 5);
            ai.parameters[0] = 16;
            ai.parameters[1] = 32;
            SetBehaviorValue(0, 1, 5);
            SetBehaviorValue(1, 1, 10);
            SetBehaviorValue(1, 3, 32);
            switch (mode)
            {
                case 48:
                    SetBehaviorRange(0, 128, 192, 256, 0);
                    SetBehaviorRange(1, 0, 1024, 128, 64);
                    SetBehaviorRange(2, 512, 4096, 256, 0);
                    SetBehaviorRange(3, 1024, 12288, 128, 96);
                    SetBehaviorRange(4, 2048, 12288, 128, 256);
                    SetBehaviorRange(5, 1024, 12288, 64, 256);
                    break;

                case 49:
                    SetBehaviorRange(0, 128, 192, 256, 0);
                    SetBehaviorRange(2, 512, 4096, 256, 0);
                    SetBehaviorRange(4, 2048, 12288, 128, 256);
                    break;

                case 50:
                    SetBehaviorRange(0, 128, 192, 256, 0);
                    SetBehaviorRange(1, 0, 0x2000, 128, 64);
                    SetBehaviorRange(4, 4096, 12288, 128, 256);
                    SetBehaviorRange(5, 2048, 12288, 64, 256);
                    break;
            }

            fragment_first = ::FindModelResourceIndex(808338210, (uint8)GetRecordModelBankSlot());
            fragment_total = 15;
            goto SetFragmentCount;

        case 64:
            SetModelBank("punk");
            health = 30;
            ground_offset = 120;
            control.variant = 16;
            sight_offset = 32;
            attack_frame = 13;
            AllocateBehaviorTable(2, 5);
            ai.parameters[0] = 29;
            ai.parameters[1] = 32;
            SetBehaviorValue(0, 1, 5);
            SetBehaviorValue(1, 1, 17);
            SetBehaviorValue(1, 3, 32);
            SetBehaviorRange(0, 128, 192, 256, 0);
            SetBehaviorRange(1, 0, 1024, 128, 64);
            SetBehaviorRange(2, 512, 4096, 256, 0);
            SetBehaviorRange(3, 1024, 12288, 128, 96);
        SetHumanoidRangesAndFragments:
            SetBehaviorRange(4, 2048, 12288, 128, 256);

            SetBehaviorRange(5, 1024, 12288, 64, 256);
            EnableShadow();
            bank_slot = (uint8)GetRecordModelBankSlot();
            shadow_height = (uint32)ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 64;
            shadow.height = shadow_height;
            fragment_first = ::FindModelResourceIndex(1020334405, bank_slot);
            fragment_count = 16;
            goto PlaceOnGround;

        case 96:
            SetModelBank("demon");
            health = 100;
            ground_offset = 116;
            control.variant = 64;
            sight_offset = 64;
            attack_frame = 10;
            AllocateBehaviorTable(2, 5);
            ai.parameters[0] = 48;
            ai.parameters[1] = 32;
            SetBehaviorValue(0, 1, 20);
            SetBehaviorValue(1, 1, 10);
            SetBehaviorValue(1, 2, 64);
            SetBehaviorValue(1, 3, 64);
            SetBehaviorValue(1, 4, 45);
            SetBehaviorRange(0, 128, 192, 256, 0);
            SetBehaviorRange(1, 0, 512, 128, 16);
            SetBehaviorRange(2, 512, 4096, 256, 0);
            SetBehaviorRange(3, 1024, 12288, 128, 96);
            SetBehaviorRange(4, 3072, 12288, 128, 256);
            SetBehaviorRange(5, 2048, 12288, 64, 256);
            EnableShadow();
            bank_slot = (uint8)GetRecordModelBankSlot();
            shadow_height = (uint32)ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 64;
            shadow.height = shadow_height;
            fragment_first = ::FindModelResourceIndex(-87017481, bank_slot);
            fragment_total = 14;
            goto SetFragmentCount;

        case 112:
            SetModelBank("zombie");
            zombie_count = swat_population.zombies;
            control.variant = 128;
            health = 40;
            ground_offset = 116;
            sight_offset = 40;
            swat_population.zombies = (uint32)zombie_count + 1;
            AllocateBehaviorTable(2, 5);
            ai.parameters[0] = 24;
            ai.parameters[1] = 32;
            SetBehaviorValue(0, 1, 5);
            SetBehaviorRange(1, 64, 4096, 128, 64);
            SetBehaviorRange(4, 0, 64, 256, 8);
            SetBehaviorRange(5, 256, 12288, 64, 256);
            EnableShadow();
            bank_slot = (uint8)GetRecordModelBankSlot();
            shadow_height = (uint32)ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 64;
            shadow.height = shadow_height;
            fragment_first = ::FindModelResourceIndex(1020334405, bank_slot);
            fragment_total = 16;
            goto SetFragmentCount;

        case 128:

        case 129:
            SetModelBank("droid");
            control.variant = 256;
            ground_offset = 180;
            sight_offset = 64;
            attack_frame = 19;
            AllocateBehaviorTable(2, 5);
            health = 30;
            ai.parameters[0] = 32;
            ai.parameters[1] = 32;
            SetBehaviorValue(0, 1, 10);
            SetBehaviorValue(1, 1, 10);
            SetBehaviorValue(1, 2, 96);
            SetBehaviorValue(1, 3, 16);
            SetBehaviorValue(1, 4, 45);
            SetBehaviorRange(0, 128, 192, 256, 0);
            if ((mode == 128))
            {
                SetBehaviorRange(1, 0, 1024, 128, 64);
                SetBehaviorRange(2, 512, 4096, 256, 64);
                SetBehaviorRange(3, 1024, 0x2000, 128, 96);
                SetBehaviorRange(4, 2048, 0x2000, 64, 128);
                SetBehaviorRange(5, 1024, 0x2000, 64, 256);
            }
            else
            {
                SetBehaviorRange(2, 512, 4096, 256, 64);
                SetBehaviorRange(4, 2048, 0x2000, 64, 128);
            }
            EnableShadow();
            bank_slot = (uint8)GetRecordModelBankSlot();
            shadow_height = (uint32)ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 64;
            shadow.height = shadow_height;
            fragment_first = ::FindModelResourceIndex(873792324, bank_slot);
            fragment_total = 12;
        SetFragmentCount:
            fragment_count = fragment_total;

            goto PlaceOnGround;

        default:

            goto PlaceOnGround;
    }
}

uint32 Swat::ExecuteCommand(uint32 command)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x800529AC, "SLUS_003.73");
    uint32 shift = 12;
    Vector3 position;
    command = (uint16)command;
    switch (command)
    {
        case 16902:
            SetMode(ReadCommandWord());
            return 1;
        case 16928:
        case 16931:
        case 16930:
        case 16933:
        {
            RecordCursor cursor(AlignScriptCursor(script_cursor));
            for (uint32 i = 0; i < 3; ++i)
                position.words[i] = (cursor + i * 4).Read32();
            ShiftVectorLeft(position.words, shift);
            if (command == 16930 || command == 16933)
                AddVectors(position.words, this->model.pos.words);
            script_cursor = cursor + 12;
            if (command == 16928 || command == 16930)
                SetMoveTarget(position.words);
            else
                SetJumpTarget(position.words);
            return 1;
        }
        case 16929:
        case 16932:
        case 16936:
        {
            uint32 value = ResolveCommandValue(ReadCommandWord());
            ReadRecordPosition(position.words, value);
            sint32 height = QueryGround(position.words, 0, 2048);
            if (height != -1)
                position.words[1] = (uint32)height - ((uint32)(sint16)(uint32)this->ground_offset << 12);
            if (command == 16929)
                SetMoveTarget(position.words);
            else if (command == 16932)
                SetJumpTarget(position.words);
            else
                SetSpecialTarget(position.words);
            return 1;
        }
        case 16938:
        case 16939:
            SetAnimationRange(command == 16938 ? 7 : 8, 0, 0xFFFFFFFF);
            this->collision_radius = command == 16938 ? 32 : 128;
            {
                uint32 flags = this->control.flags;
                uint32 state = this->lifecycle_flags;
                this->control.flags = flags & ~1;
                this->lifecycle_flags = command == 16938 ? state & ~0x10 : state | 0x10;
            }
            motion.velocity.words[2] = 0;
            motion.velocity.words[1] = 0;
            motion.velocity.words[0] = 0;
            this->control.script_delay = command == 16938 ? 12 : 14;
            return 0;
        case 16937:
        {
            uint32 value = ResolveCommandValue(ReadCommandWord());
            for (ModelInstance *node = visual_lists.solids; node; node = node->next)
                if (static_cast<VisualObject *>(node->owner)->record_idx == value)
                {
                    AttachTurret(node);
                    break;
                }
            return 1;
        }
        case 16994:
            if (ReadCommandWord() == 1)
                BeginAttack();
            return 1;
        case 17415:
        {
            uint32 value = ReadCommandWord();
            if (value)
                SetTargetRecord(ResolveCommandValue(value));
            else
                ClearTarget();
            return 1;
        }
        case 18176:
            if (ReadCommandWord())
                this->control.flags = this->control.flags | 0x100;
            else
                this->control.flags = this->control.flags & ~0x100;
            return 1;
        case 18177:
            this->control.flags = this->control.flags | 0x200;
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 Swat::DetachTurret()
{
    FUNCTION_MARKER(0x80052EA8, "SLUS_003.73");
    ModelInstance *linked = mounted_object.GetObject();
    if (linked && static_cast<VisualObject *>(linked->owner)->kind == 206)
        return static_cast<Turret *>(linked->owner)->DetachAndDestroy();
    return 206;
}

uint32 Swat::AttachTurret(ModelInstance *linked)
{
    FUNCTION_MARKER(0x80052EE8, "SLUS_003.73");
    mounted_object.SetObject(linked);
    if (static_cast<VisualObject *>(linked->owner)->kind == 206)
    {
        static_cast<Turret *>(linked->owner)->AttachOperator(this);
        ai.state = 12;
        ai.phase = 0;
        return 12;
    }
    return 206;
}

uint32 Swat::BeginAttack()
{
    FUNCTION_MARKER(0x80052F34, "SLUS_003.73");
    uint32 kind = (uint32)this->control.variant;
    switch (kind)
    {
        case 1:
        case 4:
        case 256:
            return FireProjectile();
        case 8:
        case 16:
        case 64:
            return SetAnimationRange(1, 0, 0xFFFFFFFF);
        default:
            return kind < 9 ? 4 : kind < 65 ? 16 : 256;
    }
}

uint32 Swat::Fire()
{
    ObjectBorrow object_borrow;
    uint32 mode;
    Vector3 muzzle, velocity, predicted, aim, tmp;
    GameHeapBlock *child;
    uint32 random;
    uint32 speed;
    uint32 damage;
    uint32 index;
    uint32 scale;
    uint32 distance;
    ModelInstance *player;
    LaunchedProjectile *launched = 0;
    GuidedProjectile *guided = 0;
    FUNCTION_MARKER(0x80052FCC, "SLUS_003.73");
    mode = (uint32)this->control.variant;
    if (mode == 16)
    {
        GetModelAttachment(muzzle.words, *this, 0);
        child = AllocateZeroedObject(400);
        if (child)
        {
            random = RandomRange(8);
            speed = player_distance;
            speed = DivideUnsigned(speed, random + 24);
            index = GetBehaviorValue(1, 1) & 65535;
            damage = ScaleDamageForDifficulty(index);
            player = visual_lists.players;
            launched = new (child) LaunchedProjectile(muzzle.words, player->pos.words, speed, damage, 512, &visual_lists.players, 0, 1);
        }
        if (!launched)
        {
            GameDiagnostic("Swat launched projectile allocation failed\n");
            abort();
        }
        return launched->SetGlowColor(128, 128, 0);
    }
    if (mode == 64)
    {
        index = (this->model.frame & 4) ? 1 : 0;
        GetModelAttachment(muzzle.words, *this, index);
        child = AllocateZeroedObject(428);
        if (child)
        {
            index = GetBehaviorValue(1, 1) & 65535;
            damage = ScaleDamageForDifficulty(index);
            guided = new (child) GuidedProjectile(muzzle.words, damage, &visual_lists.players, 0);
        }
        if (!guided)
        {
            GameDiagnostic("Swat guided projectile allocation failed\n");
            abort();
        }
        guided->SetAngles(model.angles.words);
        player = visual_lists.players;
        guided->Launch(0, static_cast<VisualObject *>(player->owner)->surface.pos.words);
        return guided->SetSize(2048);
    }
    if (mode != 8)
        return 8;
    random = RandomRange(16);
    player = visual_lists.players;
    scale = random + 24;
    if (!player)
        return scale;
    GetModelAttachment(muzzle.words, *this, 0);
    random = RandomRange(3);
    if (random)
    {
        player = visual_lists.players;
        const VisualObject &hero = *static_cast<VisualObject *>(player->owner);
        velocity.words[0] = hero.motion.velocity.words[0];
        velocity.words[1] = hero.motion.velocity.words[1];
        velocity.words[2] = hero.motion.velocity.words[2];
        DampVector(velocity.words, hero.motion.drag);
        MultiplyVector(predicted.words, velocity.words, scale);
        player = visual_lists.players;
        AddVectors(tmp.words, player->pos.words, predicted.words);
        player = visual_lists.players;
        aim = tmp;
        aim.words[1] = player->pos.words[1];
        distance = MeasureEuclideanDistance(muzzle.words, aim.words);
        random = RandomRange(12);
        speed = DivideUnsigned(distance, random + 16);
    }
    else
    {
        player = visual_lists.players;
        aim = player->pos;
        random = RandomRange(8);
        speed = player_distance;
        speed = DivideUnsigned(speed, random + 24);
    }
    child = AllocateZeroedObject(400);
    if (child)
    {
        index = GetBehaviorValue(1, 1) & 65535;
        damage = ScaleDamageForDifficulty(index);
        launched = new (child) LaunchedProjectile(muzzle.words, aim.words, speed, damage, 512, &visual_lists.players, 0, 1);
    }
    if (!launched)
    {
        GameDiagnostic("Swat launched projectile allocation failed\n");
        abort();
    }
    launched->SetGlowColor(32, 128, 32);
    return PlayPositionedSound(68, this->model.pos.words, 0);
}

uint32 Swat::FireProjectile()
{
    FUNCTION_MARKER(0x80053358, "SLUS_003.73");
    ModelInstance *player;
    uint32 target_x;
    uint32 own_x;
    uint32 target_z;
    uint32 own_z;
    uint32 mode;
    GameHeapBlock *projectile;
    uint32 damage, parameter, speed;
    Vector3 position, aim;
    ShortVector3 angles;
    if (this->control.flags & 64)
    {
        target_z = aim_pos.z;
        own_z = this->model.pos.words[2];
        target_x = aim_pos.x;
        own_x = this->model.pos.words[0];
        this->model.angles.words[1] = 3072 - ratan2(target_z - own_z, target_x - own_x);
        SetAnimationRange(1, 0, 0xFFFFFFFF);
        GetModelAttachment(position.words, *this, 0);
        CalculateDirectionAngles(angles.words, position.words, aim_pos.words);
    }
    else
    {
        player = visual_lists.players;
        if (!player)
            return 0;
        target_z = player->pos.words[2];
        own_z = this->model.pos.words[2];
        target_x = player->pos.words[0];
        own_x = this->model.pos.words[0];
        this->model.angles.words[1] = 3072 - ratan2(target_z - own_z, target_x - own_x);
        SetAnimationRange(1, 0, 0xFFFFFFFF);
        GetModelAttachment(position.words, *this, 0);
        player = visual_lists.players;
        static_cast<Character *>(player->owner)->GetAttachment(aim.words, 9);
        aim.components[1] = (sint32)((uint32)aim.components[1] + 0x10000);
        CalculateDirectionAngles(angles.words, position.words, aim.words);
    }
    mode = (uint32)this->control.variant;
    if (mode == 4 || mode == 256)
    {
        projectile = AllocateEffectStorage(108);
        if (projectile)
        {
            parameter = GetBehaviorValue(1, 1) & 65535;
            damage = ScaleDamageForDifficulty(parameter);
            parameter = GetBehaviorValue(1, 4) & 65535;
            speed = GetBehaviorValue(1, 2) & 65535;
            if (mode == 4)
            {
                new (projectile) OscillatingProjectile(position.words, angles.components, damage, parameter, speed, 16, 240, 240, 240, 32, 128, 64);
            }
            else
            {
                new (projectile) OscillatingProjectile(position.words, angles.components, damage, parameter, speed, 16, 240, 240, 240, 128, 32, 32);
            }
        }
        if (mode == 256)
        {
            GetModelAttachment(position.words, *this, 1);
            projectile = AllocateEffectStorage(108);
            if (projectile)
            {
                parameter = GetBehaviorValue(1, 1) & 65535;
                damage = ScaleDamageForDifficulty(parameter);
                parameter = GetBehaviorValue(1, 4) & 65535;
                speed = GetBehaviorValue(1, 2) & 65535;
                {
                    new (projectile) OscillatingProjectile(position.words, angles.components, damage, parameter, speed, 16, 240, 240, 240, 128, 32, 32);
                }
            }
        }
    }
    else
    {
        WeaponNode *fire_weapon = weapon.Get();
        if (fire_weapon)
            fire_weapon->Fire(position.words, angles.components);
        else
        {
            projectile = AllocateEffectStorage(108);
            if (projectile)
            {
                parameter = GetBehaviorValue(1, 1) & 65535;
                damage = ScaleDamageForDifficulty(parameter);
                parameter = GetBehaviorValue(1, 4) & 65535;
                speed = GetBehaviorValue(1, 2) & 65535;
                {
                    new (projectile) OscillatingProjectile(position.words, angles.components, damage, parameter, speed, 16, 240, 240, 240, 64, 32, 128);
                }
            }
        }
    }
    return PlayPositionedSound(20, this->model.pos.words, 2);
}

// Native input bridge for the original vector loads
uint32 Swat::SetMoveTarget(const uint32 *position)
{
    FUNCTION_MARKER(0x80053784, "SLUS_003.73");
    uint32 result;
    Vector3 values;
    if (this->model.animation != 2)
        StartAnimation(2, 1);
    if ((uint32)this->control.variant == 128 && (hidden_models & 9) && this->model.animation != 1)
        StartAnimation(1, 1);
    memcpy(values.words, position, sizeof(values.words));
    this->destination = values;
    result = (this->control.flags | 1) & 0xFFFFFFC7;
    this->control.flags = result;
    return result;
}

uint32 Swat::SetJumpTarget(const uint32 *position)
{
    FUNCTION_MARKER(0x80053834, "SLUS_003.73");
    uint32 result, state;
    Vector3 values;
    SetAnimationRange(4, 0, 0xFFFFFFFF);
    memcpy(values.words, position, sizeof(values.words));
    this->destination = values;
    result = this->control.flags | 9;
    state = (uint8)ai.parameters[1];
    jump_ticks = state;
    this->control.flags = result;
    return result;
}

uint32 Swat::SetSpecialTarget(const uint32 *position)
{
    FUNCTION_MARKER(0x8005389C, "SLUS_003.73");
    uint32 result;
    Vector3 values;
    SetAnimationRange(4, 0, 0xFFFFFFFF);
    memcpy(values.words, position, sizeof(values.words));
    this->destination = values;
    result = this->control.flags;
    this->control.gravity_enabled = 1;
    result |= 0x11;
    this->control.flags = result;
    return result;
}

uint32 Swat::SetTargetRecord(uint32 index)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x80053950, "SLUS_003.73");
    uint32 result;
    ReadRecordPosition(aim_pos.words, (uint16)index);
    result = (this->control.flags | 0x40);
    this->control.flags = result;
    return result;
}

uint32 Swat::ClearTarget()
{
    FUNCTION_MARKER(0x8005398C, "SLUS_003.73");
    uint32 result;
    result = (this->control.flags & 0xFFFFFF3F);
    this->control.flags = result;
    return result;
}

uint32 Swat::ChooseBehavior()
{
    uint32 cumulative[6];
    uint32 distance, total = 0, index, minimum, maximum;
    uint32 start_weight, end_weight, weight, product, denominator, random;
    FUNCTION_MARKER(0x800539A8, "SLUS_003.73");
    distance = MeasureEuclideanDistance(model.pos.words, visual_lists.players->pos.words);
    for (index = 0; index < 6; ++index)
    {
        const BehaviorRange *range = behavior_ranges + index;
        minimum = range->minimum_distance;
        weight = 0;
        if (minimum < distance)
        {
            maximum = range->maximum_distance;
            if (distance >= maximum)
                weight = range->end_weight;
            else
            {
                end_weight = range->end_weight;
                start_weight = range->start_weight;
                product = (end_weight - start_weight) * (distance - minimum);
                denominator = maximum - minimum;
                weight = (uint32)Divide((sint32)product, denominator) + start_weight;
            }
        }
        total += weight;
        cumulative[index] = total;
    }
    random = RandomRange(total);
    for (index = 0; index < 6; ++index)
        if ((sint32)random < (sint32)cumulative[index])
            return index;
    return 4;
}

void Swat::SetDeathState(uint32 state)
{
    ai.state = state;
    ai.phase = 0;
}

sint32 Swat::Divide(sint32 numerator, uint32 divisor)
{
    if (!divisor)
        return numerator < 0 ? 1 : -1;
    if ((uint32)numerator == 0x80000000 && divisor == 0xFFFFFFFF)
        return numerator;
    return numerator / (sint32)divisor;
}

uint32 Swat::DivideUnsigned(uint32 numerator, uint32 denominator)
{
    return denominator ? numerator / denominator : 0xFFFFFFFF;
}

sint32 Swat::Abs32(sint32 value)
{
    return value < 0 ? (sint32)(0 - (uint32)value) : value;
}

uint32 Swat::ResolveCommandValue(uint32 value)
{
    return (value & 0x2000) ? ResolveParameter(value) : value;
}

sint32 Swat::QueryGround(const uint32 position[3], uint32 below, uint32 above)
{
    return (sint32)FindGroundHeight((const uint32 *)position, below, above);
}

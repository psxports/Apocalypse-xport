#include <new>
#include <string.h>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "level_session.h"
#include "explosion_effects.h"
#include "game_tuning.h"
#include "game_tick.h"
#include "resource_script.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "effect_object.h"
#include "primitive_particle.h"
#include "model_bank.h"
#include "visual_object.h"
#include "object_damage.h"
#include "collision.h"
#include "model_animation.h"
#include "model_attachment_data.h"
#include "game_diagnostics.h"
#include "global.h"
#include "launched_projectile.h"
#include "guided_projectile.h"
#include "flamethrower.h"
#include "effect_lifecycle.h"
#include "model_attachment.h"
#include "audio_session.h"
#include "audio_voice.h"
#include "oscillating_projectile.h"
#include "weapon_type.h"
#include "weapon_node.h"
#include "machine_gun.h"
#include "spiral_ray_weapon.h"
#include "angle_math.h"
#include "player.h"
#include "game_object_allocation.h"
#include "debris.h"
#include "fading_particle.h"
#include "swat.h"
#include "ring_burst_effect.h"
#include "record_data.h"
#include "turret.h"
#include "fireball.h"
#include "panel_particle.h"
#include "oscillating_ring.h"
#include "enemy.h"

SwatPopulation swat_population = {0, 0};

struct BurnAttachments
{
    uint8 count;
    uint8 bones[4];
};

static const BurnAttachments burn_attachment_sets[6] = {{4, {7, 12, 10, 8}}, {4, {0, 2, 4, 6}}, {4, {7, 15, 9, 12}}, {4, {7, 16, 15, 12}}, {4, {7, 13, 10, 8}}, {3, {0, 5, 10, 0}}};

void Swat::ResetPopulation()
{
    swat_population.count = 0;
    swat_population.zombies = 0;
}

Swat::Swat(const RecordCursor &rec_data, uint16 idx) : Enemy(), jump_ticks(0), attack_frame(0), fragment_first(0), fragment_count(0)
{
    aim_pos.x = aim_pos.y = aim_pos.z = 0;
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x8004E35C, "SLUS_003.73");
    RecordCursor decoded;

    decoded = ReadPos(rec_data);
    script_cursor = ReadAngles(decoded);
    control.script_active = 1;
    rec_idx = idx;
    Init();
    ExecuteScript(script_cursor);
}

Swat::Swat(const Vector3 *pos_vector, const RecordCursor &script) : Enemy(), jump_ticks(0), attack_frame(0), fragment_first(0), fragment_count(0)
{
    aim_pos.x = aim_pos.y = aim_pos.z = 0;
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x8004E3E4, "SLUS_003.73");
    pos_history.saved = *pos_vector;
    model.pos = pos_history.saved;
    script_cursor = script;
    Init();
    if (script_cursor)
    {
        control.script_active = 1;
        ExecuteScript(script_cursor);
    }
}

void Swat::Init()
{
    FUNCTION_MARKER(0x8004E48C, "SLUS_003.73");
    ++game_progress.level.enemy_total;
    motion.drag[0] = 31;
    motion.drag[2] = 31;
    motion.drag[1] = 3;
    AddToList(visual_lists.actors);
    kind = 200;
    ai.state = 0;
    ai.phase = 0;
    ++swat_population.count;
    control.hierarchy_ready = 0;
}

uint32 Swat::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8004E508, "SLUS_003.73");
    uint16 variant;
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
    variant = control.variant;
    --swat_population.count;
    if (variant == 128)
        --swat_population.zombies;
    if (control.variant == 256)
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
        return ReleaseStorage();
    return reason & 1;
}

void Swat::ApplyExplosionDamage()
{
    sint32 idx = 0, distance;
    uint32 radius, damage;
    FUNCTION_MARKER(0x8004E624, "SLUS_003.73");
    while (idx < (sint32)explosion_queue.count)
    {
        const ExplosionEvent *event = &explosion_queue.entries[idx];
        distance = model.pos.Distance(event->pos);
        radius = event->radius;
        if (distance < (sint32)radius)
        {
            damage = event->strength;
            damage = DivideSignedWord(damage * (radius - distance), radius);
            Damage(damage, &zero_vector, 0);
            if (health <= 0)
                return;
        }
        ++idx;
    }
}

uint32 Swat::Update()
{
    ModelBorrow model_borrow;
    RecordDataBorrow rec_borrow;
    ShortVector3 motion_target;
    FUNCTION_MARKER(0x8004E71C, "SLUS_003.73");
    uint16 activation;
    uint8 activation_count;
    uint16 script_delay;
    bool jump_done;
    bool melee_hit;
    const ModelAttachmentTable *attachments;
    uint8 ground_period;
    uint8 ground_tick;
    sint16 ground_probe_offset;
    sint32 ground_y;
    uint16 ground_contact_flags;
    uint32 surface_flags;
    sint32 ground_height;
    sint32 subject_heading;
    sint32 dst_heading;
    uint32 movement_flags;
    sint32 jump_velocity_x;
    sint32 jump_velocity_z;
    uint32 jump_duration;
    sint32 jump_height;
    uint8 jump_remaining;
    uint8 next_jump_tick;
    sint8 jump_frame;
    uint32 next_movement_flags;
    sint32 landing_y;
    sint8 landing_frame;
    sint32 turn_speed;
    uint16 move_speed;
    bool reached_dst;
    uint8 attack_animation;
    uint32 melee_event;
    uint16 base_damage;
    uint32 melee_damage;
    uint16 attack_variant;
    uint32 close_fire_event;
    uint32 fire_event;
    sint32 player_heading;
    uint8 fire_frame;
    ModelInstance *contact_player;
    uint32 contact_damage;
    uint8 particles_remaining;
    GameHeapBlock *particle_storage;
    GameHeapBlock *ring_storage;
    uint16 final_variant;
    uint8 visibility_animation;
    uint16 visibility_flags;
    uint32 result;
    uint32 footstep_event0;
    uint32 footstep_event1;
    Vector3 attack_vector, attachment_pos;
    ModelAttachmentPoint attachment_desc;
    uint32 aim_delta_x, aim_delta_z;
    uint32 melee_ground_height, melee_y, player_ground_height, player_y, melee_delta;
    ModelInstance *player;
    Vector3 move_delta, move_dir;
    motion.acceleration = zero_vector;
    if (weapon)
        weapon->Update(&model.pos);
    if (control.variant == 128)
    {
        if (!RandomRange(240))
            PlayPositionedSound(98, &model.pos, 0);
        else if (!RandomRange(240))
            PlayPositionedSound(99, &model.pos, 0);
    }
    activation = activation_flags;
    control.prev_activation_flags = activation;
    if (activation & 1)
    {
        activation_count = control.activation_count;
        activation_flags = activation & 0xFFFE;
        control.activation_count = activation_count + 1;
    }
    if ((activation_flags & 4) != 0)
        ApplyExplosionDamage();
    if (control.script_active)
    {
        script_delay = control.script_delay;
        if (!script_delay)
            ExecuteScript(script_cursor);
        else
            control.script_delay = script_delay - 1;
    }
    else
    {
        UpdateState();
    }
    control.flags &= ~0x400;
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
            model.SetAttachmentTable(*attachments);
        control.hierarchy_ready = 1;
    }
    if (control.gravity_enabled)
        motion.acceleration.Add(world_gravity.vector);

    IntegrateMotion();
    if (control.flags & 2)
    {
        ground_tick = control.ground_tick + 1;
        ground_period = control.ground_period;
        control.ground_tick = ground_tick;
        if (ground_tick >= ground_period)
        {
            ground_probe_offset = ground_offset;
            control.ground_tick = 0;
            ground_y = CollisionRay::FindGroundHeight(model.pos, ground_probe_offset, 4 * ground_probe_offset);
            if (ground_y != -1)
            {
                ground_height = ground_offset * ONE;
                ground_contact_flags = contact_flags | 2;
                model.pos.y = (uint32)ground_y - ground_height;
                contact_flags = ground_contact_flags;
            }
        }
    }
    UpdateWorldTransform();
    ground_height = ground_offset * ONE;
    surface_flags = control.flags;
    surface.pos = model.pos;
    surface.pos.y = (uint32)surface.pos.y + ground_height;
    if (surface_flags & 0x80)
    {
        ModelInstance *subject = aim_subject.GetObject();
        if (subject)
        {
            const VisualObject *object = static_cast<VisualObject *>(subject->owner);
            aim_pos = object->kind == 50 ? object->surface.pos : subject->pos;
            aim_delta_z = (uint32)aim_pos.z - model.pos.z;
            aim_delta_x = (uint32)aim_pos.x - model.pos.x;
            subject_heading = ratan2(aim_delta_z, aim_delta_x);
            motion_target.x = 0;
            motion_target.y = (3072 - subject_heading) & 0xFFF;
            motion_target.z = 0;
            UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, 32);
        }
    }
    if (control.flags & 1)
    {
        aim_delta_z = (uint32)dst.z - model.pos.z;
        aim_delta_x = (uint32)dst.x - model.pos.x;
        dst_heading = ratan2(aim_delta_z, aim_delta_x);
        motion_target.x = 0;
        motion_target.y = (3072 - dst_heading) & 0xFFF;
        motion_target.z = 0;
        UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, 32);
        movement_flags = control.flags;
        if (movement_flags & 8)
        {
            jump_frame = model.frame;
            if (jump_frame >= 3)
            {
                if (jump_ticks)
                    control.gravity_enabled = 1;
            }
            else
            {
                playback.speed = 0x10000;
                control.gravity_enabled = 0;
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                motion.velocity.x = 0;
                motion.acceleration.z = 0;
                motion.acceleration.y = 0;
                motion.acceleration.x = 0;
            }
            if (jump_frame < 3 || (control.flags & 0x20))
            {
                if (jump_frame >= 22)
                {
                    if (!jump_ticks)
                    {
                    FinishJumpAnimation:
                        if (!playback.complete)
                            goto UpdateAttack;

                        next_movement_flags = control.flags & 0xFFFFFFD6;
                        goto StoreMovementFlags;
                    }
                    playback.speed = 0;
                }
            }
            else
            {
                jump_velocity_x = DivideSignedWord((uint32)dst.x - model.pos.x, jump_ticks);
                jump_velocity_z = DivideSignedWord((uint32)dst.z - model.pos.z, jump_ticks);
                jump_height = (uint32)dst.y - model.pos.y;
                jump_duration = jump_ticks;
                motion.drag[0] = 31;
                control.flags |= 0x20;
                motion.velocity.x = jump_velocity_x;
                motion.velocity.z = jump_velocity_z;
                motion.velocity.y = DivideSignedWord(jump_height, jump_duration) - (jump_duration << 14);
                motion.drag[1] = 31;
                motion.drag[2] = 31;
            }
            jump_remaining = jump_ticks;
            jump_done = jump_remaining == 0;
            next_jump_tick = jump_remaining - 1;
            if (!jump_done)
            {
                jump_ticks = next_jump_tick;
                jump_done = next_jump_tick == 0;
                if (!jump_done && motion.velocity.y > 0)
                {
                    jump_done = dst.y < model.pos.y;
                }
                if (jump_done)
                {
                    jump_ticks = 0;
                    model.pos = dst;
                    motion.velocity.z = 0;
                    motion.velocity.y = 0;
                    motion.velocity.x = 0;
                    motion.acceleration.z = 0;
                    motion.acceleration.y = 0;
                    playback.speed = 0x10000;
                    motion.acceleration.x = 0;
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
            landing_y = dst.y;
            if (landing_y < model.pos.y)
            {
                model.pos.y = landing_y;
                control.gravity_enabled = 0;
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                landing_frame = (sint8)model.frame;
                motion.velocity.x = 0;
                SetAnimationRange(4, landing_frame, -1);
            }
            if (playback.complete)
            {
                next_movement_flags = control.flags & 0xFFFFFFEE;
            StoreMovementFlags:
                control.flags = next_movement_flags;
            }
        }
        else
        {
            move_delta.x = (sint32)((uint32)dst.x - model.pos.x) >> 12;
            move_delta.y = (sint32)((uint32)dst.y - model.pos.y) >> 12;
            move_delta.z = (sint32)((uint32)dst.z - model.pos.z) >> 12;
            NormalizeShortVector(move_delta, move_dir);
            turn_speed = motion.angular_velocity.y;
            if (turn_speed < 0)
                turn_speed = -turn_speed;
            if (turn_speed >= 65)
                turn_speed = 64;
            move_speed = ai.parameters[0] * (64 - turn_speed) / 64;
            motion.velocity.x = move_dir.x * move_speed;
            motion.velocity.y = move_dir.y * move_speed;
            motion.velocity.z = move_dir.z * move_speed;
            if (control.flags & 2)
                reached_dst = model.pos.HorizontalDistance(dst) < 0x40;
            else
                reached_dst = model.pos.EuclideanDistance(dst) < 0x40;
            if (reached_dst)
            {
                next_movement_flags = control.flags & 0xFFFFFFFE;
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
            if (player && model.pos.HorizontalDistance(player->pos) < 256)
            {
                melee_ground_height = ground_offset * ONE;
                melee_y = (uint32)model.pos.y;
                player = visual_lists.players;
                player_ground_height = static_cast<Player *>(visual_lists.players->owner)->ground_offset * ONE;
                player_y = (uint32)player->pos.y;
                melee_delta = player_y + player_ground_height - melee_y - melee_ground_height;
                if ((sint32)melee_delta < 0)
                    melee_delta = -melee_delta;
                melee_hit = (sint32)melee_delta <= 0x7FFFF;
            }
        }
        if (melee_hit)
        {
            base_damage = GetBehaviorVal(0, 1);
            melee_damage = ScaleDamageForDifficulty(base_damage);
            attack_vector.Subtract(visual_lists.players->pos, model.pos);
            VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
            damage_target->Damage(melee_damage, &attack_vector, 0);
        }
        attack_animation = model.animation;
    }
    if (attack_animation == 1)
    {
        attack_variant = control.variant;
        if (attack_variant & 0x10)
        {
            close_fire_event = DidAnimationCrossFrame(1, 10);
            if (UpdateEventLatch(close_fire_event, 2) && ((sint32)model.pos.HorizontalDistance(visual_lists.players->pos) < 4500))
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
            aim_delta_z = (uint32)player->pos.z - model.pos.z;
            aim_delta_x = (uint32)player->pos.x - model.pos.x;
            player_heading = ratan2(aim_delta_z, aim_delta_x);
            motion_target.x = 0;
            motion_target.y = (3072 - player_heading) & 0xFFF;
            motion_target.z = 0;
            UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, 32);
            fire_frame = model.frame;
            if (fire_frame >= 21 && fire_frame < 40 && !(fire_frame & 3))
                goto FireAttack;
        }
    }
    if (control.variant == 128)
    {
        contact_player = visual_lists.players;
        model_effect.color_mode = 100687952;
        model_effect.scale = 10240;
        model_effect.depth_bias = 10;
        if (model.pos.HorizontalDistance(contact_player->pos) < 0xA0 && ai.state != 16 && visual_lists.players)
        {
            contact_damage = ScaleDamageForDifficulty(1);
            VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
            damage_target->Damage(contact_damage, &zero_vector, 0);
        }
    }
    if (((control.flags & 0x100) != 0) && !RandomRange(4))
    {
        use_small_effect_pool = false;
        particles_remaining = RandomRange(3) + 2;
        do
        {
            particle_storage = EffectObject::AllocateStorage(116);
            if (particle_storage)
                new (particle_storage) PanelParticle(&model.pos);
            --particles_remaining;
        } while (particles_remaining);
        use_small_effect_pool = true;
    }
    if (control.flags & 4)
    {
        if (!burn_ring)
        {
            ring_storage = EffectObject::AllocateStorage(176);
            if (ring_storage)
            {
                burn_ring = new (ring_storage) OscillatingRing(&model.pos, 64, 32, 64, 112, 96, 0, 48, 0x25, 0);
            }
            if (!burn_ring)
            {
                GameDiagnostic(("Swat burn ring allocation failed\n"));
                abort();
            }
            burn_ring->persistent = true;
            burn_ring->SetPos(&model.pos);
            control.flags |= 0x800;
        }
        if (game_clock.tick & 1)
        {
            const BurnAttachments *burn_attachments = 0;
            switch (control.variant)
            {
                case 1:
                    burn_attachments = burn_attachment_sets;
                    break;
                case 4:
                    burn_attachments = burn_attachment_sets + 1;
                    break;
                case 2:
                    burn_attachments = burn_attachment_sets + 2;
                    break;
                case 8:
                    burn_attachments = burn_attachment_sets + 3;
                    break;
                case 16:
                    burn_attachments = burn_attachment_sets + 4;
                    break;
                case 256:
                    burn_attachments = burn_attachment_sets + 5;
                    break;
            }
            uint8 ring_bone = 0;
            if (burn_attachments)
            {
                ring_bone = burn_attachments->bones[0];
                uint8 bone_idx = RandomRange(burn_attachments->count);
                use_small_effect_pool = false;
                GameHeapBlock *allocation = EffectObject::AllocateStorage(132);
                if (allocation)
                {
                    new (allocation) Fireball(*this, burn_attachments->bones[bone_idx]);
                }
                use_small_effect_pool = true;
            }
            if (burn_ring)
            {
                attachment_desc.x = attachment_desc.y = attachment_desc.z = 0;
                attachment_desc.bone = ring_bone;
                GetModelBonePos(attachment_pos, *this, attachment_desc);
                burn_ring->SetPos(&attachment_pos);
            }
        }
    }
    final_variant = control.variant;
    if (final_variant == 8)
    {
        visibility_animation = model.animation;
        if (visibility_animation == 8)
        {
            visibility_flags = model.flags & 0xFFFE;
        }
        else
        {
            if (!playback.complete || (visibility_animation != 7))
                goto UpdateFootsteps;
            visibility_flags = model.flags | 1;
        }
        model.flags = visibility_flags;
    UpdateFootsteps:
        final_variant = control.variant;
    }
    result = 2;
    if ((final_variant == 256) && (model.animation == 2))
    {
        footstep_event0 = DidAnimationCrossFrame(2, 3);
        if (UpdateEventLatch(footstep_event0, 1))
            return PlayPositionedSound(121, &model.pos, 3);
        footstep_event1 = DidAnimationCrossFrame(2, 17);
        result = UpdateEventLatch(footstep_event1, 2);
        if (result)
            return PlayPositionedSound(121, &model.pos, 3);
    }
    return result;
}

uint32 Swat::Damage(uint32 damage, const Vector3 *input_vector, uint32 reason)
{
    ModelBorrow model_borrow;
    FUNCTION_MARKER(0x8004F5C8, "SLUS_003.73");
    uint8 count;
    uint16 lifecycle;
    uint32 flags;
    ModelInstance *hero;
    sint16 health;
    if (control.variant == 64 && reason == 5)
        return 0;
    count = control.damage_phase;
    flags = control.flags;
    control.damage_phase = count + 1;
    health = this->health;
    control.flags = flags | 0x400;
    if (health <= 0)
        return 1;
    this->health = health - damage;
    if (this->health > 0)
    {
        SetColor(3, 255, 255, 255);
        SpawnZombieFragment();
    }
    else
    {
        hero = visual_lists.players;
        if (hero)
            static_cast<Player *>(hero->owner)->TryPlayKillQuote();
        lifecycle = lifecycle_flags;
        motion.angular_acceleration.y = 0;
        motion.angular_velocity.y = 0;
        lifecycle_flags = lifecycle & 0xFFEF;
        DetachTurret();
        DeathReaction(input_vector, reason);
    }
    return 1;
}

void Swat::SpawnZombieFragment()
{
    uint16 trig_angle, angle;
    uint8 random;
    Vector3 velocity;
    GameHeapBlock *child;
    uint16 kind;
    uint32 height;
    uint32 flags;
    FUNCTION_MARKER(0x8004F6C4, "SLUS_003.73");
    if (control.variant != 128)
        return;
    angle = RandomRange(4096);
    random = RandomRange(16);
    trig_angle = angle & 0xFFF;
    velocity.x = rsin(trig_angle) * (random + 24);
    random = RandomRange(16);
    velocity.y = (-32 - random) * ONE;
    random = RandomRange(16);
    velocity.z = rcos(trig_angle) * (random + 24);
    child = AllocateZeroedObject(320);
    if (child)
    {
        random = RandomRange(fragment_count);
        kind = fragment_first;
        height = ground_offset * ONE;
        height += (uint32)model.pos.y;
        new (child) Debris(&model.pos, &velocity, kind + random, height, 9, 24, GetRecModelBankName());
    }
    if (!RandomRange(2))
        return;
    flags = hidden_models;
    if (!(flags & 0x10000))
        flags |= 0x100C0;
    else if (!(flags & 0x1000))
        flags |= 0x1300;
    else
    {
        if (!(flags & 1))
            hidden_models = flags | 7;
        return;
    }
    hidden_models = flags;
}

void Swat::DeathReaction(const Vector3 *vector, uint32 reason)
{
    FUNCTION_MARKER(0x8004F840, "SLUS_003.73");
    uint32 flags, divisor;
    uint16 val, factor;
    bool nonzero;
    Vector3 dir;
    ShortVector3 angles;
    nonzero = ((uint32)vector->x | vector->y | vector->z) != 0;
    if (control.variant == 128)
    {
        flags = hidden_models;
        SetDeathState(16);
        control.script_active = 0;
        hidden_models = flags | 0x113FF;
        return;
    }
    if ((control.flags & 0x200) || model.animation == 4)
    {
        FragmentAndFinish(0);
        return;
    }
    motion.angular_velocity.z = 0;
    motion.angular_velocity.y = 0;
    motion.angular_velocity.x = 0;
    motion.angular_acceleration.z = 0;
    motion.angular_acceleration.y = 0;
    motion.angular_acceleration.x = 0;
    if (reason == 5)
    {
        if (!RandomRange(5))
            FragmentAndFinish(5);
        else
        {
            flags = control.flags;
            control.script_active = 0;
            SetDeathState(11);
            control.flags = flags | 4;
        }
        return;
    }
    if (reason == 2)
    {
        FragmentAndFinish(0);
        return;
    }
    if (reason == 9 || reason == 10)
    {
        if ((control.variant & 31) && !RandomRange(reason == 9 ? 3 : 4))
        {
            EnterAttachmentParticleDeath();
            return;
        }
        val = RandomRange(reason == 9 ? 3 : 2);
        if (reason == 9 && !val)
        {
            control.script_active = 0;
            SetDeathState(8);
        }
        else if (reason == 10 && val)
            EnterMovingParticleDeath();
        else
            FragmentAndFinish(0);
        return;
    }
    if (RandomRange(2))
    {
        FragmentAndFinish(0);
        return;
    }
    if ((control.variant & 31) && !RandomRange(5))
    {
        EnterAttachmentParticleDeath();
        return;
    }
    val = control.variant;
    control.script_active = 0;
    if (val != 4)
    {
        SetDeathState(8);
        return;
    }
    if (nonzero)
    {
        dir.x = vector->x;
        dir.y = 0;
        dir.z = vector->z;
        divisor = dir.Len() + 1;
        if (divisor < 128)
        {
            factor = DivideSignedWord(128, divisor);
            dir.x = (uint32)dir.x * factor;
            dir.z = (uint32)dir.z * factor;
        }
        angles.SetDir(dir, zero_vector);
        model.angles.y = angles.y;
    }
    SetDeathState(10);
}

void Swat::FragmentAndFinish(uint32 mode)
{
    FUNCTION_MARKER(0x8004FB00, "SLUS_003.73");
    uint16 angle = RandomRange(1024);
    uint32 idx;
    for (idx = 0; idx < 6; ++idx)
    {
        Vector3 velocity;
        GameHeapBlock *child;
        if (player_distance >= 2048 || !visual_lists.cameras || RandomRange(4))
        {
            uint8 random = RandomRange(16);
            uint16 trig_angle = angle & 4095;
            velocity.x = rsin(trig_angle) * (random + 24);
            random = RandomRange(16);
            velocity.y = (-32 - random) * ONE;
            random = RandomRange(16);
            velocity.z = rcos(trig_angle) * (random + 24);
        }
        else
        {
            ModelInstance *target = visual_lists.cameras;
            Vector3 pos;
            uint32 divisor;
            sint32 numerator;
            pos = target->pos;
            pos.y = (uint32)pos.y + (0x80000);
            divisor = model.pos.Distance(pos) >> 7;
            // MIPS DIV defines LO even for zero
            numerator = (sint32)((uint32)pos.x - model.pos.x);
            velocity.x = DivideSignedWord(numerator, divisor);
            numerator = (sint32)((uint32)pos.z - model.pos.z);
            velocity.z = DivideSignedWord(numerator, divisor);
            numerator = (sint32)((uint32)pos.y - model.pos.y);
            velocity.y = DivideSignedWord(numerator, divisor) - (divisor << 13);
        }
        uint16 fragment_type = mode == 5 ? 5 : (control.variant & 0x180) ? 9 : 3;
        child = AllocateZeroedObject(320);
        if (child)
        {
            uint8 variation = RandomRange(fragment_count);
            uint16 kind = fragment_first + variation;
            uint32 height = ground_offset * ONE;
            height += (uint32)model.pos.y;
            new (child) Debris(&model.pos, &velocity, kind, height, fragment_type, 24, GetRecModelBankName());
            if (mode == 5)
                SetColor(0, 0, 0, 0);
        }
        angle += 682;
    }
    Finish();
}

void Swat::EnterMovingParticleDeath()
{
    FUNCTION_MARKER(0x8004FE20, "SLUS_003.73");
    uint16 kind = control.variant;
    uint32 mask;
    ai.state = 13;
    control.script_active = 0;
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
            return;
    }
    hidden_models |= mask;
}

void Swat::EnterAttachmentParticleDeath()
{
    FUNCTION_MARKER(0x8004FEC4, "SLUS_003.73");
    uint16 kind = control.variant;
    uint32 mask;
    control.script_active = 0;
    ai.state = 14;
    ai.phase = 0;
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
    hidden_models |= mask;
}

void Swat::UpdateState()
{
    ModelBorrow model_borrow;
    FUNCTION_MARKER(0x8004FF6C, "SLUS_003.73");
    CollisionRay raycast;

    Vector3 pos_output, effect_pos0, effect_pos1, sight_pos;
    ShortVector3 desired_angles, effect_angles;
    uint16 next_state, next_phase;
    bool pos_chosen;
    uint16 fall_phase;
    ModelInstance *facing_player;
    bool player_missing;
    bool sight_blocked;
    bool attack_wait;
    bool fade_elapsed;
    bool repeat_move;
    bool airborne_elapsed;
    bool below_floor;
    sint32 fall_ground_y;
    sint16 fall_ground_offset;
    uint16 fall_variant;
    uint16 fall_lifecycle_flags;
    sint8 landing_frame;
    sint32 fall_target_y;
    sint32 fall_height;
    uint16 landed_lifecycle_flags;
    uint32 idle_flags;
    bool idle_animation_complete;
    uint8 behavior;
    uint16 idle_variant;
    uint8 idle_sound_roll;
    uint8 idle_sound;
    uint16 fallback_state;
    uint16 approach_phase;
    uint32 minimum_radius;
    sint16 search_distance;
    uint32 approach_flags;
    uint16 attack_phase;
    uint32 sight_x;
    ModelInstance *sight_player;
    uint16 phase_before_advance;
    sint16 attack_ticks;
    uint16 move_phase;
    uint32 move_flags;
    sint32 player_heading;
    uint32 death_flags;
    uint16 death_variant;
    sint16 fade_ticks;
    sint16 next_fade_tick;
    uint16 fade_model_flags;
    uint16 retreat_phase;
    uint32 retreat_flags;
    sint32 retreat_distance;
    sint32 retreat_ground_y;
    uint8 burst_idx;
    GameHeapBlock *burst_storage;
    sint16 retreat_ticks;
    sint32 death_ground_y;
    uint16 burn_phase;
    uint16 burn_phase_before_advance;
    uint16 burn_variant;
    uint8 burn_repeat_range;
    uint8 burn_repeat_roll;
    uint16 moving_death_phase;
    bool death_move_chosen;
    uint8 moving_burst_idx;
    uint8 moving_burst_speed;
    GameHeapBlock *moving_burst_storage;
    uint32 moving_death_flags;
    uint16 attached_death_phase;
    uint16 attached_lifecycle_flags;
    uint16 attached_phase_before_advance;
    uint8 attached_burst_idx;
    uint8 attached_burst_speed;
    GameHeapBlock *attached_burst_storage;
    uint8 attached_repeat_roll;
    uint8 final_burst_idx;
    uint8 final_burst_speed;
    GameHeapBlock *final_burst_storage;
    uint8 airborne_speed_roll;
    uint8 airborne_yaw_roll;
    sint16 airborne_pitch_speed;
    uint8 airborne_lifetime_roll;
    sint32 airborne_y;
    uint16 airborne_phase_before_advance;
    sint16 airborne_ticks;
    sint16 next_airborne_tick;
    sint16 airborne_floor;
    sint32 airborne_floor_y;
    uint32 airborne_velocity_y;
    uint16 emergence_phase;
    uint16 emergence_lifecycle_flags;
    uint16 rise_phase;
    uint32 rise_y;
    sint16 rise_ticks;
    uint16 rise_model_flags;
    uint16 scripted_rise_phase;
    uint16 rise_phase_before_advance;
    uint16 hidden_model_flags;
    uint32 maximum_radius;
    Vector3 retreat_vector;
    uint32 clipped_retreat_divisor;
    uint32 free_retreat_divisor;
    switch (ai.state)
    {
        case 0:
            fall_phase = ai.phase;
            if (fall_phase == 1)
            {
                if ((control.flags & 2) != 0)
                {
                    if (!(contact_flags & 2))
                        return;
                    landing_frame = (sint8)model.frame;
                    control.gravity_enabled = 0;
                }
                else
                {
                    fall_target_y = (sint16)ai.work[0] * 16384;
                    fall_height = ground_offset * ONE;
                    if (fall_target_y >= (sint32)((uint32)model.pos.y + fall_height))
                        return;
                    model.pos.y = (uint32)fall_target_y - fall_height;
                    control.gravity_enabled = 0;
                    motion.velocity.z = 0;
                    motion.velocity.y = 0;
                    landing_frame = (sint8)model.frame;
                    motion.velocity.x = 0;
                }
                SetAnimationRange(4, landing_frame, -1);
                fall_variant = control.variant;
                ai.phase = 2;
                goto EnableLandingShadow;
            }
            if (ai.phase < 2)
            {
                if (ai.phase)
                    return;
                facing_player = visual_lists.players;
                player_missing = visual_lists.players == 0;
                control.flags &= ~1;
                if (!player_missing)
                    model.angles.y = (3072 - ratan2((sint32)((uint32)facing_player->pos.z - model.pos.z), (sint32)((uint32)facing_player->pos.x - model.pos.x)));
                fall_ground_y = CollisionRay::FindGroundHeight(model.pos, 0, 4096);
                if (fall_ground_y == -1)
                {
                    Finish();
                    return;
                }
                fall_ground_offset = ground_offset;
                if (((sint32)((uint32)fall_ground_y - (uint32)model.pos.y) >> 12) >= 2 * fall_ground_offset)
                {
                    ai.work[0] = fall_ground_y >> 14;
                    ai.phase = 1;
                    SetAnimationRange(4, 21, 22);
                    fall_lifecycle_flags = lifecycle_flags;
                    control.gravity_enabled = 1;
                    lifecycle_flags = fall_lifecycle_flags & 0xFFEF;
                    RemoveShadow();
                    return;
                }
                model.pos.y = (uint32)fall_ground_y - fall_ground_offset * ONE;
                ai.state = 1;
                control.gravity_enabled = 0;
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                fall_variant = control.variant;
                motion.velocity.x = 0;
            EnableLandingShadow:
                if (fall_variant != 8)
                {
                    EnableShadow();
                    return;
                }

                return;
            }
            if (fall_phase == 2)
            {
                if (playback.complete)
                {
                    landed_lifecycle_flags = lifecycle_flags;
                    ai.SetStatePhase(1);
                    lifecycle_flags = landed_lifecycle_flags | 0x10;
                }
            }
            return;

        case 1:
            if (!ai.phase)
            {
                idle_flags = control.flags;
                control.gravity_enabled = 0;
                control.flags = idle_flags & 0xFFFFFFFE;
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                motion.velocity.x = 0;
                motion.angular_velocity.z = 0;
                motion.angular_velocity.y = 0;
                motion.angular_velocity.x = 0;
                motion.angular_acceleration.z = 0;
                motion.angular_acceleration.y = 0;
                idle_animation_complete = playback.complete;
                motion.angular_acceleration.x = 0;
                if (idle_animation_complete)
                    SetAnimationRange(0, 0, -1);
                behavior = ChooseBehavior();
                idle_variant = control.variant;
                ai.state = behavior + 2;
                ai.phase = 0;
                if (idle_variant == 4)
                {
                    idle_sound_roll = RandomRange(16);
                    idle_sound = 43;
                    if (!idle_sound_roll)
                    {
                        PlaySoundEffect(idle_sound, 0x2000, 0);
                        return;
                    }
                    idle_sound_roll = RandomRange(16);
                    idle_sound = 44;
                    if (!idle_sound_roll)
                    {
                        PlaySoundEffect(idle_sound, 0x2000, 0);
                        return;
                    }
                }
            }
            return;

        case 2:
            fallback_state = ai.phase;
            if (!ai.phase)
            {
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                motion.velocity.x = 0;
                SetAnimationRange(3, 0, -1);
                next_phase = ai.phase + 1;
                goto StorePhase;
            }
            if (fallback_state == 1)
            {
                if (visual_lists.players)
                {
                    desired_angles.SetDir(model.pos, visual_lists.players->pos);
                    desired_angles.comp[0] = 0;
                    UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired_angles, 32);
                }
                if (playback.complete)
                {
                    motion.angular_acceleration.y = 0;
                    motion.angular_velocity.y = 0;
                    goto ResetFallbackState;
                }
            }
            return;

        case 3:
            approach_phase = ai.phase;
            if (!ai.phase)
            {
                minimum_radius = 128;
                maximum_radius = 192;
                search_distance = 1024;
                goto ChooseApproachPosition;
            }
            if (approach_phase != 1)
                return;
            approach_flags = control.flags;
            if ((approach_flags & 0x400) != 0)
            {
                control.flags = approach_flags & 0xFFFFFFFE;
            FinishApproach:
                ai.SetStatePhase(approach_phase);

                return;
            }
            if ((approach_flags & 1) == 0)
                goto FinishApproach;
            if (visual_lists.players)
            {
                if (player_distance < behavior_ranges[0].minimum_distance)
                {
                    control.flags = approach_flags & 0xFFFFFFFE;
                    next_state = 2;
                    goto StoreState;
                }
            }
            return;

        case 4:
            attack_phase = ai.phase;
            if (!ai.phase)
            {
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                sight_x = model.pos.x;
                sight_player = visual_lists.players;
                motion.velocity.x = 0;
                sight_pos.x = sight_x;
                sight_pos.y = (uint32)model.pos.y - sight_offset * ONE;
                sight_pos.z = model.pos.z;
                sight_blocked = CollisionRay::IsSegVisible(sight_pos, sight_player->pos) == 0;
                next_state = 1;
                if (sight_blocked)
                    goto StoreState;
                BeginAttack();
                phase_before_advance = ai.phase;
                ai.work[0] = 0;
                goto AdvancePhase;
            }
            if (attack_phase == 1)
            {
                attack_ticks = (sint16)ai.work[0];
                attack_wait = attack_ticks < GetBehaviorVal(1, 3);
                if (attack_wait)
                    ai.work[0] = attack_ticks + 1;
                else
                    ai.SetStatePhase(attack_phase);
                if (visual_lists.players)
                {
                    desired_angles.SetDir(model.pos, visual_lists.players->pos);
                    desired_angles.comp[0] = 0;

                    UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired_angles, 32);
                    return;
                }
            }
            return;

        case 5:
            move_phase = ai.phase;
            if (!ai.phase)
            {
                minimum_radius = 1024;
                maximum_radius = 2048;
                search_distance = 512;
            ChooseApproachPosition:
                fallback_state = 1;

                pos_chosen = ChoosePos(&dst, &visual_lists.players->pos, minimum_radius, maximum_radius, control.height_limit, search_distance, 1, 1);
                goto ApplyChosenPosition;
            }
            if (move_phase == 1)
            {
                move_flags = control.flags;
                if ((move_flags & 0x400) == 0)
                    goto WaitForMovement;
                control.flags = move_flags & 0xFFFFFFFE;
                goto FinishMovement;
            }
            return;

        case 6:
            if (ai.phase)
            {
                if (ai.phase == 1)
                {
                    if ((control.variant == 128) && visual_lists.players)
                    {
                        player_heading = ratan2((sint32)((uint32)visual_lists.players->pos.z - model.pos.z), (sint32)((uint32)visual_lists.players->pos.x - model.pos.x));
                        desired_angles.comp[0] = 0;
                        desired_angles.comp[1] = (3072 - player_heading) & 0xFFF;
                        desired_angles.comp[2] = 0;
                        UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired_angles, 32);
                    }
                    next_state = 1;
                    if ((control.flags & 0x400) != 0)
                        goto StoreState;
                    next_state = 1;
                    if (playback.complete)
                        goto StoreState;
                    if (behavior_ranges[2].start_weight)
                    {
                        if (player_distance < behavior_ranges[2].maximum_distance)
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
                ai.phase = 1;
            }
            return;

        case 7:
            move_phase = ai.phase;
            if (ai.phase)
            {
                if (move_phase != 1)
                    return;
                move_flags = control.flags;
                if ((move_flags & 0x400) != 0)
                {
                    control.flags = move_flags & 0xFFFFFFFE;
                }
                else
                {
                WaitForMovement:

                    if ((move_flags & 1) != 0)
                        return;
                }
            FinishMovement:
                ai.SetStatePhase(move_phase);
            }
            else
            {
                fallback_state = 1;
                pos_chosen = ChoosePos(&dst, &model.pos, 1024, 2048, control.height_limit, 4096, 1, 1);
            ApplyChosenPosition:

                if (pos_chosen)
                    goto StartMovement;
            ResetFallbackState:
                ai.state = fallback_state;

                ai.phase = 0;
            }
            return;

        case 8:
            if (ai.phase)
            {
                if (ai.phase == 1)
                {
                    if (playback.complete)
                    {
                        fade_ticks = (sint16)ai.work[0];
                        fade_elapsed = fade_ticks == 0;
                        next_fade_tick = fade_ticks - 1;
                        if (fade_elapsed)
                            Finish();
                        else
                            ai.work[0] = next_fade_tick;
                        if (!ai.work[1])
                        {
                            SetColor(-32, 0, 0, 0);
                            fade_model_flags = model.flags;
                            model.render_mode = 128;
                            model.flags = fade_model_flags | 0x800;
                            ai.work[1] = 1;
                        }
                    }
                }
            }
            else
            {
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                motion.velocity.x = 0;
                motion.angular_velocity.z = 0;
                motion.angular_velocity.y = 0;
                motion.angular_velocity.x = 0;
                motion.angular_acceleration.z = 0;
                motion.angular_acceleration.y = 0;
                death_flags = control.flags;
                motion.angular_acceleration.x = 0;
                col_radius = 0;
                control.flags = death_flags & 0xFFFFFFFE;
                ActivateLinkedRecs();
                SetAnimationRange(5, 0, -1);
                RemoveShadow();
                death_variant = control.variant;
                ai.phase = 1;
                ai.work[0] = 32;
                ai.work[1] = 0;
                if (death_variant == 256)
                {
                    PlayPositionedSound(120, &model.pos, 3);
                    return;
                }
            }
            return;

        case 0xA:
            retreat_phase = ai.phase;
            if (retreat_phase == 1)
            {
                if (ai.work[0])
                {
                    PrimitiveParticle::SetAngles(&model.angles);
                    PrimitiveParticle::SetSize(2);
                    burst_idx = 0;
                    PrimitiveParticle::SetColor(GetGameTuning().death_particle_color[0], GetGameTuning().death_particle_color[1], GetGameTuning().death_particle_color[2]);
                    PrimitiveParticle::SetColorStep(GetGameTuning().death_particle_color[0] >> 5, GetGameTuning().death_particle_color[1] >> 5, GetGameTuning().death_particle_color[2] >> 5);
                    use_small_effect_pool = false;
                    do
                    {
                        burst_storage = EffectObject::AllocateStorage(88);
                        if (burst_storage)
                        {
                            new (burst_storage) FadingParticle(&model.pos, 16, 0x2000, 16);
                        }
                        ++burst_idx;
                    } while (burst_idx < 16);
                    retreat_ticks = (sint16)ai.work[0];
                    use_small_effect_pool = true;
                    ai.work[0] = retreat_ticks - 1;
                    return;
                }
            StopAndAdvancePhase:
                motion.velocity.z = 0;

                motion.velocity.y = 0;
                phase_before_advance = ai.phase;
                motion.velocity.x = 0;
            AdvancePhase:
                next_phase = phase_before_advance + 1;

                goto StorePhase;
            }
            if (ai.phase < 2)
            {
                if (ai.phase)
                    return;
                model.angles.x = 0;
                motion.angular_velocity.z = 0;
                motion.angular_velocity.y = 0;
                motion.angular_velocity.x = 0;
                motion.angular_acceleration.z = 0;
                motion.angular_acceleration.y = 0;
                retreat_flags = control.flags;
                motion.angular_acceleration.x = 0;
                col_radius = 0;
                control.flags = retreat_flags & 0xFFFFFFFE;
                RemoveShadow();
                dst.SetDir(1024, model.angles);
                pos_output.Subtract(model.pos, dst);
                dst = pos_output;
                raycast.endpoints[0] = model.pos;
                raycast.endpoints[1] = dst;
                raycast.Initialize();
                raycast.triggers = false;
                raycast.Query(1);
                if (raycast.hit.GetObject())
                {
                    retreat_distance = raycast.distance - 256;
                    if (retreat_distance >= 64)
                    {
                        ai.work[0] = retreat_distance >> 6;
                        dst.SetDir(retreat_distance, model.angles);
                        pos_output.Subtract(model.pos, dst);
                        dst = pos_output;
                        retreat_vector.Subtract(dst, model.pos);
                        clipped_retreat_divisor = (sint16)ai.work[0];
                        pos_output.Divide(retreat_vector, clipped_retreat_divisor);
                        motion.velocity = pos_output;
                        goto StartRetreatDeath;
                    }
                    motion.velocity.z = 0;
                    motion.velocity.y = 0;
                    motion.velocity.x = 0;
                    ai.work[0] = 1;
                }
                else
                {
                    ai.work[0] = 16;
                    pos_output.Subtract(dst, model.pos);
                    free_retreat_divisor = 16;
                    retreat_vector.Divide(pos_output, free_retreat_divisor);
                    motion.velocity = retreat_vector;
                    retreat_ground_y = CollisionRay::FindGroundHeight(dst, ground_offset, 4 * ground_offset);
                    if (retreat_ground_y != -1)
                    {
                    StartRetreatDeath:
                        RingBurstEffect::SpawnExplosion(&model.pos, 128, 128, 64, 0, 5, 0, 100);

                        SetAnimationRange(5, 0, -1);
                        PlaySoundEffect(45, 0x2000, 0);
                        col_radius = 0;
                        ActivateLinkedRecs();
                        next_phase = ai.phase + 1;
                        goto StorePhase;
                    }
                    control.gravity_enabled = 1;
                }
                goto StartRetreatDeath;
            }
            if (retreat_phase == 2)
            {
                if (control.gravity_enabled)
                {
                FinishAfterAnimation:

                    if (playback.complete)
                    {
                        Finish();
                        return;
                    }
                    return;
                }
                if (!playback.complete)
                    return;
                death_ground_y = CollisionRay::FindGroundHeight(model.pos, 0, 256);
                if (death_ground_y == -1)
                {
                    FragmentAndFinish(0);
                    return;
                }
                next_phase = ai.phase + 1;
            StorePhase:
                ai.phase = next_phase;
            }
            return;

        case 0xB:
            burn_phase = ai.phase;
            if (burn_phase == 1)
                goto UpdateBurnAnimation;
            if (ai.phase >= 2)
            {
                if (burn_phase != 2)
                    return;
                if (control.variant == 256)
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
            ActivateLinkedRecs();
            model_effect.color_mode = 37765184;
            model_effect.scale = 10240;
            col_radius = 0;
            model_effect.depth_bias = 10;
            SetAnimationRange(6, 0, -1);
            if (control.variant == 256)
                ai.script_data = PlayPositionedSound(18, &model.pos, 3);
            else
                PlayPositionedSound(25, &model.pos, 3);
            control.flags &= ~1;
            motion.velocity.z = 0;
            motion.velocity.y = 0;
            burn_phase_before_advance = ai.phase;
            motion.velocity.x = 0;
            ai.phase = burn_phase_before_advance + 1;
        UpdateBurnAnimation:

            if (!playback.complete)
                return;
            burn_variant = control.variant;
            if (burn_variant != 256)
            {
                if (!RandomRange(4))
                    PlayPositionedSound(25, &model.pos, 3);
                burn_variant = control.variant;
            }
            burn_repeat_range = 4;
            if (burn_variant == 256)
                burn_repeat_range = 2;
            burn_repeat_roll = RandomRange(burn_repeat_range);
            if (!burn_repeat_roll || IsGameHeapUnderPressure())
            {
                SetAnimationRange(9, 0, -1);
                next_phase = ai.phase + 1;
                goto StorePhase;
            }
            SetAnimationRange(6, 0, -1);
            return;

        case 0xD:
            moving_death_phase = ai.phase;
            if (moving_death_phase == 1)
            {
                effect_angles.comp[0] = -1024;
                effect_angles.comp[1] = 0;
                effect_angles.comp[2] = 0;
                PrimitiveParticle::SetAngles(&effect_angles);
                effect_angles.comp[0] = 512;
                effect_angles.comp[1] = 4096;
                effect_angles.comp[2] = 0;
                PrimitiveParticle::SetSpread(&effect_angles);
                PrimitiveParticle::SetSize(2);
                moving_burst_idx = 0;
                PrimitiveParticle::SetColor(GetGameTuning().death_particle_color[0], GetGameTuning().death_particle_color[1], GetGameTuning().death_particle_color[2]);
                moving_burst_speed = 24;
                PrimitiveParticle::SetColorStep(GetGameTuning().death_particle_color[0] >> 5, GetGameTuning().death_particle_color[1] >> 5, GetGameTuning().death_particle_color[2] >> 5);
                use_small_effect_pool = false;
                do
                {
                    moving_burst_storage = EffectObject::AllocateStorage(88);
                    if (moving_burst_storage)
                    {
                        new (moving_burst_storage) FadingParticle(&model.pos, moving_burst_speed, 0x2000, 16);
                    }
                    ++moving_burst_idx;
                    moving_burst_speed += 4;
                } while (moving_burst_idx < 4);
                moving_death_flags = control.flags;
                use_small_effect_pool = true;
                if (moving_death_flags & 1)
                    return;
                repeat_move = RandomRange(2) == 0;
                next_state = 8;
                if (!repeat_move)
                {
                    ai.phase = ai.phase - 1;
                    return;
                }
            }
            else
            {
                if (ai.phase >= 2)
                {
                    if (moving_death_phase == 2)
                        goto FinishAfterAnimation;
                    return;
                }
                if (ai.phase)
                    return;
                death_move_chosen = ChoosePos(&dst, &model.pos, 256, 512, control.height_limit, 4096, 1, 1);
                if (death_move_chosen)
                {
                StartMovement:
                    SetMoveTarget(&dst);

                    ai.phase = 1;
                    return;
                }
                next_state = 8;
            }
        StoreState:
            ai.state = next_state;

        ResetPhase:
            ai.phase = 0;

            return;

        case 0xE:
            attached_death_phase = ai.phase;
            if (attached_death_phase == 1)
                goto EmitAttachedDeathBurst;
            if (ai.phase >= 2)
            {
                if (attached_death_phase != 2)
                    return;
                effect_angles.comp[0] = 512;
                effect_angles.comp[1] = 4096;
                effect_angles.comp[2] = 0;
                PrimitiveParticle::SetSpread(&effect_angles);
                PrimitiveParticle::SetSize(2);
                final_burst_idx = 0;
                PrimitiveParticle::SetColor(GetGameTuning().death_particle_color[0], GetGameTuning().death_particle_color[1], GetGameTuning().death_particle_color[2]);
                final_burst_speed = 32;
                PrimitiveParticle::SetColorStep(GetGameTuning().death_particle_color[0] >> 5, GetGameTuning().death_particle_color[1] >> 5, GetGameTuning().death_particle_color[2] >> 5);
                GetModelAttachment(effect_pos0, *this, 1);
                effect_angles.comp[0] = -1024;
                effect_angles.comp[1] = 0;
                effect_angles.comp[2] = 0;
                PrimitiveParticle::SetAngles(&effect_angles);
                use_small_effect_pool = false;
                effect_pos0.y = (uint32)effect_pos0.y + (0x20000);
                do
                {
                    final_burst_storage = EffectObject::AllocateStorage(88);
                    if (final_burst_storage)
                    {
                        new (final_burst_storage) FadingParticle(&effect_pos0, final_burst_speed, 0x2000, 16);
                    }
                    ++final_burst_idx;
                    final_burst_speed += 4;
                } while (final_burst_idx < 4);
                use_small_effect_pool = true;
                goto FinishAfterAnimation;
            }
            if (ai.phase)
                return;
            RemoveShadow();
            attached_lifecycle_flags = lifecycle_flags;
            col_radius = 0;
            lifecycle_flags = attached_lifecycle_flags & 0xFFEF;
            ActivateLinkedRecs();
            SetAnimationRange(6, 0, -1);
            control.flags = (control.flags & 0xFFFFEFFE) | 0x1000;
            motion.velocity.z = 0;
            motion.velocity.y = 0;
            attached_phase_before_advance = ai.phase;
            motion.velocity.x = 0;
            ai.phase = attached_phase_before_advance + 1;
        EmitAttachedDeathBurst:
            effect_angles.comp[0] = -1024;
            effect_angles.comp[1] = 0;
            effect_angles.comp[2] = 0;
            PrimitiveParticle::SetAngles(&effect_angles);
            effect_angles.comp[0] = 512;
            effect_angles.comp[1] = 4096;
            effect_angles.comp[2] = 0;
            PrimitiveParticle::SetSpread(&effect_angles);
            PrimitiveParticle::SetSize(2);
            attached_burst_idx = 0;
            PrimitiveParticle::SetColor(GetGameTuning().death_particle_color[0], GetGameTuning().death_particle_color[1], GetGameTuning().death_particle_color[2]);
            attached_burst_speed = 32;
            PrimitiveParticle::SetColorStep(GetGameTuning().death_particle_color[0] >> 5, GetGameTuning().death_particle_color[1] >> 5, GetGameTuning().death_particle_color[2] >> 5);
            GetModelAttachment(effect_pos1, *this, 1);
            use_small_effect_pool = false;
            effect_pos1.y = (uint32)effect_pos1.y + (0x20000);
            do
            {
                attached_burst_storage = EffectObject::AllocateStorage(88);
                if (attached_burst_storage)
                {
                    new (attached_burst_storage) FadingParticle(&effect_pos1, attached_burst_speed, 0x2000, 16);
                }
                ++attached_burst_idx;
                attached_burst_speed += 4;
            } while (attached_burst_idx < 4);
            use_small_effect_pool = true;
            if (!playback.complete)
                return;
            attached_repeat_roll = RandomRange(2);
            if (attached_repeat_roll)
            {
                SetAnimationRange(6, 0, -1);
                return;
            }
            SetAnimationRange(9, 0, -1);
            next_phase = ai.phase + 1;
            goto StorePhase;

        case 0x10:
            if (!ai.phase)
            {
                airborne_speed_roll = RandomRange(64);
                motion.velocity.x = 0;
                motion.velocity.y = -(airborne_speed_roll + 96) * ONE;
                motion.velocity.z = 0;
                airborne_pitch_speed = RandomRange(256) - 128;
                airborne_yaw_roll = RandomRange(256);
                motion.angular_velocity.x = airborne_pitch_speed;
                motion.angular_velocity.y = airborne_yaw_roll - 128;
                motion.angular_velocity.z = 0;
                motion.angular_drag[0] = 16;
                motion.angular_drag[1] = 16;
                motion.angular_drag[2] = 16;
                control.gravity_enabled = 1;
                control.flags &= ~1;
                airborne_lifetime_roll = RandomRange(32);
                airborne_y = model.pos.y;
                ai.work[0] = airborne_lifetime_roll + 16;
                ai.work[1] = (uint16)ground_offset + (airborne_y >> 12);
                airborne_phase_before_advance = ai.phase + 1;
                ai.phase = airborne_phase_before_advance;
                return;
            }
            if (ai.phase != 1)
                return;
            airborne_ticks = (sint16)ai.work[0];
            airborne_elapsed = airborne_ticks == 0;
            next_airborne_tick = airborne_ticks - 1;
            if (airborne_elapsed)
            {
                FragmentAndFinish(0);
                return;
            }
            ai.work[0] = next_airborne_tick;
            airborne_floor = (sint16)ai.work[1];
            below_floor = airborne_floor >= (model.pos.y >> 12);
            airborne_floor_y = airborne_floor * ONE;
            if (!below_floor)
            {
                airborne_velocity_y = motion.velocity.y;
                model.pos.y = airborne_floor_y;
                motion.velocity.y = -airborne_velocity_y;
            }
            return;

        case 0x20:
            emergence_phase = ai.phase;
            if (emergence_phase == 1)
            {
                if ((control.flags & 1) == 0)
                    goto StopAndAdvancePhase;
                if (visual_lists.players)
                {
                    if (player_distance < behavior_ranges[0].minimum_distance)
                        ai.phase = 2;
                }
            }
            else if (ai.phase >= 2)
            {
                if (emergence_phase == 2)
                {
                    control.flags &= ~1;
                    SetAnimationRange(8, 0, -1);
                    emergence_lifecycle_flags = lifecycle_flags;
                    col_radius = 128;
                    control.script_delay = 8;
                    ai.SetStatePhase(1);
                    lifecycle_flags = emergence_lifecycle_flags | 0x10;
                }
            }
            else if (!ai.phase)
            {
                if (ChoosePos(&dst, &visual_lists.players->pos, 128, 192, control.height_limit, 1024, 1, 1))
                {
                    SetMoveTarget(&dst);
                    ai.phase = 1;
                }
                else
                {
                    ai.phase = 2;
                }
            }
            return;

        case 0x30:
            rise_phase = ai.phase;
            if (!ai.phase)
            {
                lifecycle_flags &= ~0x10;
                rise_y = model.pos.y;
                goto StartRise;
            }
            if (rise_phase != 1)
                return;
            rise_ticks = (sint16)ai.work[1];
            if (rise_ticks != 60)
                goto StepRise;
            lifecycle_flags |= 0x10;
            SetAnimationRange(0, 0, -1);
            rise_model_flags = model.flags;
            ai.SetStatePhase(rise_phase);
            model.flags = rise_model_flags & 0xFFF7;
            return;

        case 0x31:
            scripted_rise_phase = ai.phase;
            if (ai.phase)
            {
                if (scripted_rise_phase == 1)
                {
                    rise_ticks = (sint16)ai.work[1];
                    if (rise_ticks == 60)
                    {
                        model.flags &= ~8;
                        SetAnimationRange(0, 0, -1);
                        ai.SetStatePhase(scripted_rise_phase);
                        control.script_active = 1;
                    }
                    else
                    {
                    StepRise:
                        ai.work[1] = rise_ticks + 1;

                        model.pos.y = (uint32)model.pos.y - (34952);
                    }
                }
            }
            else
            {
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                rise_y = model.pos.y;
                motion.velocity.x = 0;
            StartRise:
                model.pos.y = rise_y + 0x200000;

                StartAnimation(0, 1);
                rise_phase_before_advance = ai.phase;
                hidden_model_flags = model.flags;
                ai.work[1] = 0;
                next_phase = rise_phase_before_advance + 1;
                ai.phase = next_phase;
                model.flags = hidden_model_flags | 8;
            }
            return;

        default:
            return;
    }
}

void Swat::SetBehaviorRange(uint32 idx, uint32 minimum_distance, uint32 maximum_distance, sint32 start_weight, sint32 end_weight)
{
    FUNCTION_MARKER(0x8005171C, "SLUS_003.73");
    BehaviorRange *range = behavior_ranges + idx;
    range->minimum_distance = minimum_distance;
    range->maximum_distance = maximum_distance;
    range->start_weight = start_weight;
    range->end_weight = end_weight;
}

void Swat::SetMode(uint16 mode)
{
    FUNCTION_MARKER(0x80051740, "SLUS_003.73");
    uint16 scaled_model_flags;
    GameHeapBlock *weapon_storage;
    uint16 weapon_damage;
    uint16 prisoner_model_flags;
    uint8 fragment_total;
    sint32 ground_y;
    sint16 ground_height;
    uint32 own_y;
    uint8 bank_slot;
    sint16 shadow_height;
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
            SetBehaviorVal(0, 1, 20);
            SetBehaviorVal(1, 1, 10);
            SetBehaviorVal(1, 2, 64);
            SetBehaviorVal(1, 3, 16);
            SetBehaviorVal(1, 4, 45);
            goto SetSwatBehaviorRanges;

        case 1:
            SetModelBank("swat");
            control.variant = 1;
            scaled_model_flags = model.flags;
            model.scale.z = 6144;
            model.scale.y = 6144;
            model.scale.x = 6144;
            model.flags = scaled_model_flags | 0x200;
            SetColor(0, 208, 96, 96);
            health = 60;
            ground_offset = 200;
            sight_offset = 96;
            attack_frame = 10;
            AllocateBehaviorTable(2, 5);
            ai.parameters[0] = 32;
            ai.parameters[1] = 32;
            SetBehaviorVal(0, 1, 20);
            SetBehaviorVal(1, 1, 10);
            SetBehaviorVal(1, 2, 64);
            SetBehaviorVal(1, 3, 32);
            SetBehaviorVal(1, 4, 90);
            SetBehaviorRange(2, 0, 4096, 256, 128);
            SetBehaviorRange(4, 4064, 0x4000, 128, 128);
            EnableShadow();
            bank_slot = GetRecModelBankSlot();
            shadow_height = ground_offset;
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
            SetBehaviorVal(0, 1, 20);
            SetBehaviorVal(1, 1, 10);
            SetBehaviorVal(1, 2, 64);
            SetBehaviorVal(1, 3, 16);
            SetBehaviorVal(1, 4, 45);
            weapon_storage = AllocateZeroedHeapBlock(124);
            if (weapon_storage)
            {
                weapon = new (weapon_storage) SpiralRayWeapon(&visual_lists.players);
            }
            else
                weapon = 0;
            if (!weapon)
            {
                GameDiagnostic(("Swat weapon allocation failed\n"));
                abort();
            }
            weapon_damage = GetBehaviorVal(1, 1);
            weapon->damage = weapon_damage;
        SetSwatBehaviorRanges:
            SetBehaviorRange(0, 128, 192, 256, 0);

            SetBehaviorRange(1, 0, 512, 128, 16);
            SetBehaviorRange(2, 512, 4096, 256, 0);
            SetBehaviorRange(3, 1024, 12288, 128, 96);
            SetBehaviorRange(4, 2048, 12288, 128, 256);
            SetBehaviorRange(5, 1024, 12288, 64, 256);
            EnableShadow();
            bank_slot = GetRecModelBankSlot();
            shadow_height = ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 64;
        SetSwatFragments:
            shadow.height = shadow_height;

            fragment_first = ModelBank::FindResourceIdx(-617804721, bank_slot);
            fragment_count = 10;
            goto PlaceOnGround;

        case 16:
            SetModelBank("prisoner");
            control.variant = 2;
            health = 1;
            ground_offset = 112;
            model.scale.x = 7000;
            model.scale.y = 5000;
            model.scale.z = 0x2000;
            prisoner_model_flags = model.flags;
            sight_offset = 32;
            attack_frame = 15;
            model.flags = prisoner_model_flags | 0x200;
            AllocateBehaviorTable(2, 5);
            ai.parameters[0] = 16;
            ai.parameters[1] = 32;
            SetBehaviorVal(0, 1, 5);
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
            bank_slot = GetRecModelBankSlot();
            shadow_height = ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 96;
            shadow.height = shadow_height;
            fragment_first = ModelBank::FindResourceIdx(-317030919, bank_slot);
            fragment_count = 10;
            switch (mode)
            {
                case 32:
                    health = 30;
                    ai.parameters[0] = 32;
                    ai.parameters[1] = 32;
                    SetBehaviorVal(0, 1, 10);
                    SetBehaviorVal(1, 1, 5);
                    SetBehaviorVal(1, 2, 48);
                    SetBehaviorVal(1, 3, 16);
                    SetBehaviorVal(1, 4, 45);
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
                    SetBehaviorVal(0, 1, 10);
                    SetBehaviorVal(1, 1, 5);
                    SetBehaviorVal(1, 2, 48);
                    SetBehaviorVal(1, 3, 16);
                    SetBehaviorVal(1, 4, 45);
                    SetBehaviorRange(0, 128, 256, 256, 0);
                    SetBehaviorRange(2, 232, 4096, 256, 0);
                    SetBehaviorRange(4, 2048, 12288, 128, 256);
                    break;

                case 34:
                    health = 30;
                    ai.parameters[0] = 32;
                    ai.parameters[1] = 32;
                    SetBehaviorVal(0, 1, 10);
                    SetBehaviorVal(1, 1, 5);
                    SetBehaviorVal(1, 2, 48);
                    SetBehaviorVal(1, 3, 16);
                    SetBehaviorVal(1, 4, 45);
                    SetBehaviorRange(0, 128, 192, 256, 0);
                    SetBehaviorRange(1, 0, 4096, 0, 128);
                    SetBehaviorRange(4, 3072, 12288, 32, 256);
                    break;

                default:
                    if (mode != 35)
                        goto PlaceOnGround;
                    health = 30;
                    SetBehaviorVal(1, 1, 5);
                    SetBehaviorVal(1, 2, 48);
                    SetBehaviorVal(1, 3, 32);
                    SetBehaviorVal(1, 4, 45);
                    SetBehaviorRange(2, 0, 4096, 256, 0);
                    SetBehaviorRange(4, 4096, 0x4000, 256, 256);
                    break;
            }

        PlaceOnGround:
            ground_y = CollisionRay::FindGroundHeight(model.pos, 64, 512);

            if (ground_y == -1)
            {
                RemoveShadow();
                return;
            }
            own_y = model.pos.y;
            ground_height = ground_offset;
            if (((sint32)((uint32)ground_y - own_y) >> 12) < ground_height * 2)
                model.pos.y = (uint32)ground_y - ground_height * ONE;
            return;

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
            SetBehaviorVal(0, 1, 5);
            SetBehaviorVal(1, 1, 10);
            SetBehaviorVal(1, 3, 32);
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

            fragment_first = ModelBank::FindResourceIdx(808338210, GetRecModelBankSlot());
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
            SetBehaviorVal(0, 1, 5);
            SetBehaviorVal(1, 1, 17);
            SetBehaviorVal(1, 3, 32);
            SetBehaviorRange(0, 128, 192, 256, 0);
            SetBehaviorRange(1, 0, 1024, 128, 64);
            SetBehaviorRange(2, 512, 4096, 256, 0);
            SetBehaviorRange(3, 1024, 12288, 128, 96);
        SetHumanoidRangesAndFragments:
            SetBehaviorRange(4, 2048, 12288, 128, 256);

            SetBehaviorRange(5, 1024, 12288, 64, 256);
            EnableShadow();
            bank_slot = GetRecModelBankSlot();
            shadow_height = ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 64;
            shadow.height = shadow_height;
            fragment_first = ModelBank::FindResourceIdx(1020334405, bank_slot);
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
            SetBehaviorVal(0, 1, 20);
            SetBehaviorVal(1, 1, 10);
            SetBehaviorVal(1, 2, 64);
            SetBehaviorVal(1, 3, 64);
            SetBehaviorVal(1, 4, 45);
            SetBehaviorRange(0, 128, 192, 256, 0);
            SetBehaviorRange(1, 0, 512, 128, 16);
            SetBehaviorRange(2, 512, 4096, 256, 0);
            SetBehaviorRange(3, 1024, 12288, 128, 96);
            SetBehaviorRange(4, 3072, 12288, 128, 256);
            SetBehaviorRange(5, 2048, 12288, 64, 256);
            EnableShadow();
            bank_slot = GetRecModelBankSlot();
            shadow_height = ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 64;
            shadow.height = shadow_height;
            fragment_first = ModelBank::FindResourceIdx(-87017481, bank_slot);
            fragment_total = 14;
            goto SetFragmentCount;

        case 112:
            SetModelBank("zombie");
            control.variant = 128;
            health = 40;
            ground_offset = 116;
            sight_offset = 40;
            ++swat_population.zombies;
            AllocateBehaviorTable(2, 5);
            ai.parameters[0] = 24;
            ai.parameters[1] = 32;
            SetBehaviorVal(0, 1, 5);
            SetBehaviorRange(1, 64, 4096, 128, 64);
            SetBehaviorRange(4, 0, 64, 256, 8);
            SetBehaviorRange(5, 256, 12288, 64, 256);
            EnableShadow();
            bank_slot = GetRecModelBankSlot();
            shadow_height = ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 64;
            shadow.height = shadow_height;
            fragment_first = ModelBank::FindResourceIdx(1020334405, bank_slot);
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
            SetBehaviorVal(0, 1, 10);
            SetBehaviorVal(1, 1, 10);
            SetBehaviorVal(1, 2, 96);
            SetBehaviorVal(1, 3, 16);
            SetBehaviorVal(1, 4, 45);
            SetBehaviorRange(0, 128, 192, 256, 0);
            if (mode == 128)
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
            bank_slot = GetRecModelBankSlot();
            shadow_height = ground_offset;
            shadow.max_height = 1024;
            shadow.radius = 64;
            shadow.height = shadow_height;
            fragment_first = ModelBank::FindResourceIdx(873792324, bank_slot);
            fragment_total = 12;
        SetFragmentCount:
            fragment_count = fragment_total;

            goto PlaceOnGround;

        default:

            goto PlaceOnGround;
    }
}

uint32 Swat::ExecuteCommand(uint16 command)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x800529AC, "SLUS_003.73");
    Vector3 pos;
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
            RecordCursor cursor(script_cursor.Aligned(4));
            for (uint32 i = 0; i < 3; ++i)
                pos.comp[i] = (cursor + i * 4).Read32();
            pos.ShiftLeft(12);
            if (command == 16930 || command == 16933)
                pos.Add(model.pos);
            script_cursor = cursor + 12;
            if (command == 16928 || command == 16930)
                SetMoveTarget(&pos);
            else
                SetJumpTarget(&pos);
            return 1;
        }
        case 16929:
        case 16932:
        case 16936:
        {
            uint16 val = ResolveCommandVal(ReadCommandWord());
            ReadRecPos(&pos, val);
            sint32 height = CollisionRay::FindGroundHeight(pos, 0, 2048);
            if (height != -1)
                pos.y = (uint32)height - ground_offset * ONE;
            if (command == 16929)
                SetMoveTarget(&pos);
            else if (command == 16932)
                SetJumpTarget(&pos);
            else
                SetSpecialTarget(&pos);
            return 1;
        }
        case 16938:
        case 16939:
        {
            SetAnimationRange(command == 16938 ? 7 : 8, 0, 0xFFFFFFFF);
            col_radius = command == 16938 ? 32 : 128;
            uint32 flags = control.flags;
            uint16 lifecycle = lifecycle_flags;
            control.flags = flags & ~1;
            lifecycle_flags = command == 16938 ? lifecycle & ~0x10 : lifecycle | 0x10;
            motion.velocity = zero_vector;
            control.script_delay = command == 16938 ? 12 : 14;
            return 0;
        }
        case 16937:
        {
            uint16 val = ResolveCommandVal(ReadCommandWord());
            for (ModelInstance *node = visual_lists.solids; node; node = node->next)
                if (static_cast<VisualObject *>(node->owner)->rec_idx == val)
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
            uint16 val = ReadCommandWord();
            if (val)
                SetTargetRec(ResolveCommandVal(val));
            else
                ClearTarget();
            return 1;
        }
        case 18176:
            if (ReadCommandWord())
                control.flags |= 0x100;
            else
                control.flags &= ~0x100;
            return 1;
        case 18177:
            control.flags |= 0x200;
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

void Swat::DetachTurret()
{
    FUNCTION_MARKER(0x80052EA8, "SLUS_003.73");
    ModelInstance *linked = mounted_object.GetObject();
    if (linked && static_cast<VisualObject *>(linked->owner)->kind == 206)
        static_cast<Turret *>(linked->owner)->DetachAndDestroy();
}

void Swat::AttachTurret(ModelInstance *linked)
{
    FUNCTION_MARKER(0x80052EE8, "SLUS_003.73");
    mounted_object.SetObject(linked);
    if (static_cast<VisualObject *>(linked->owner)->kind == 206)
    {
        static_cast<Turret *>(linked->owner)->AttachOperator(this);
        ai.state = 12;
        ai.phase = 0;
    }
}

void Swat::BeginAttack()
{
    FUNCTION_MARKER(0x80052F34, "SLUS_003.73");
    uint16 variant = control.variant;
    switch (variant)
    {
        case 1:
        case 4:
        case 256:
            FireProjectile();
            break;
        case 8:
        case 16:
        case 64:
            SetAnimationRange(1, 0, 0xFFFFFFFF);
            break;
    }
}

void Swat::Fire()
{
    ObjectBorrow object_borrow;
    uint16 variant;
    Vector3 muzzle, velocity, predicted, aim;
    GameHeapBlock *child;
    uint8 random;
    uint32 speed;
    uint32 damage;
    uint16 base_damage;
    uint8 joint_idx;
    uint32 lead_ticks;
    uint32 distance;
    ModelInstance *player;
    LaunchedProjectile *launched = 0;
    GuidedProjectile *guided = 0;
    FUNCTION_MARKER(0x80052FCC, "SLUS_003.73");
    variant = control.variant;
    if (variant == 16)
    {
        GetModelAttachment(muzzle, *this, 0);
        child = AllocateZeroedObject(400);
        if (child)
        {
            random = RandomRange(8);
            speed = player_distance / (random + 24);
            base_damage = GetBehaviorVal(1, 1);
            damage = ScaleDamageForDifficulty(base_damage);
            player = visual_lists.players;
            launched = new (child) LaunchedProjectile(muzzle, player->pos, speed, damage, 512, &visual_lists.players, 0, 1);
        }
        if (!launched)
        {
            GameDiagnostic(("Swat launched projectile allocation failed\n"));
            abort();
        }
        launched->SetGlowColor(128, 128, 0);
        return;
    }
    if (variant == 64)
    {
        joint_idx = (model.frame & 4) ? 1 : 0;
        GetModelAttachment(muzzle, *this, joint_idx);
        child = AllocateZeroedObject(428);
        if (child)
        {
            base_damage = GetBehaviorVal(1, 1);
            damage = ScaleDamageForDifficulty(base_damage);
            guided = new (child) GuidedProjectile(muzzle, damage, &visual_lists.players, 0);
        }
        if (!guided)
        {
            GameDiagnostic(("Swat guided projectile allocation failed\n"));
            abort();
        }
        guided->SetAngles(model.angles);
        player = visual_lists.players;
        guided->Launch(0, &static_cast<VisualObject *>(player->owner)->surface.pos);
        guided->SetSize(2048);
        return;
    }
    if (variant != 8)
        return;
    random = RandomRange(16);
    player = visual_lists.players;
    lead_ticks = random + 24;
    if (!player)
        return;
    GetModelAttachment(muzzle, *this, 0);
    random = RandomRange(3);
    if (random)
    {
        player = visual_lists.players;
        const VisualObject &hero = *static_cast<VisualObject *>(player->owner);
        velocity = hero.motion.velocity;
        velocity.Damp(hero.motion.drag);
        predicted.Multiply(velocity, lead_ticks);
        player = visual_lists.players;
        aim.Add(player->pos, predicted);
        player = visual_lists.players;
        aim.y = player->pos.y;
        distance = muzzle.EuclideanDistance(aim);
        random = RandomRange(12);
        speed = distance / (random + 16);
    }
    else
    {
        player = visual_lists.players;
        aim = player->pos;
        random = RandomRange(8);
        speed = player_distance / (random + 24);
    }
    child = AllocateZeroedObject(400);
    if (child)
    {
        base_damage = GetBehaviorVal(1, 1);
        damage = ScaleDamageForDifficulty(base_damage);
        launched = new (child) LaunchedProjectile(muzzle, aim, speed, damage, 512, &visual_lists.players, 0, 1);
    }
    if (!launched)
    {
        GameDiagnostic(("Swat launched projectile allocation failed\n"));
        abort();
    }
    launched->SetGlowColor(32, 128, 32);
    PlayPositionedSound(68, &model.pos, 0);
}

void Swat::FireProjectile()
{
    FUNCTION_MARKER(0x80053358, "SLUS_003.73");
    ModelInstance *player;
    sint32 dx, dz;
    uint16 variant;
    GameHeapBlock *projectile;
    uint32 damage;
    uint16 parameter, speed;
    Vector3 pos, aim;
    ShortVector3 angles;
    if (control.flags & 64)
    {
        dz = (uint32)aim_pos.z - model.pos.z;
        dx = (uint32)aim_pos.x - model.pos.x;
        model.angles.y = 3072 - ratan2(dz, dx);
        SetAnimationRange(1, 0, 0xFFFFFFFF);
        GetModelAttachment(pos, *this, 0);
        angles.SetDir(pos, aim_pos);
    }
    else
    {
        player = visual_lists.players;
        if (!player)
            return;
        dz = (uint32)player->pos.z - model.pos.z;
        dx = (uint32)player->pos.x - model.pos.x;
        model.angles.y = 3072 - ratan2(dz, dx);
        SetAnimationRange(1, 0, 0xFFFFFFFF);
        GetModelAttachment(pos, *this, 0);
        player = visual_lists.players;
        static_cast<Character *>(player->owner)->GetAttachment(aim, 9);
        aim.y = (uint32)aim.y + (0x10000);
        angles.SetDir(pos, aim);
    }
    variant = control.variant;
    if (variant == 4 || variant == 256)
    {
        projectile = EffectObject::AllocateStorage(108);
        if (projectile)
        {
            parameter = GetBehaviorVal(1, 1);
            damage = ScaleDamageForDifficulty(parameter);
            parameter = GetBehaviorVal(1, 4);
            speed = GetBehaviorVal(1, 2);
            if (variant == 4)
            {
                new (projectile) OscillatingProjectile(&pos, &angles, damage, parameter, speed, 16, 240, 240, 240, 32, 128, 64);
            }
            else
            {
                new (projectile) OscillatingProjectile(&pos, &angles, damage, parameter, speed, 16, 240, 240, 240, 128, 32, 32);
            }
        }
        if (variant == 256)
        {
            GetModelAttachment(pos, *this, 1);
            projectile = EffectObject::AllocateStorage(108);
            if (projectile)
            {
                parameter = GetBehaviorVal(1, 1);
                damage = ScaleDamageForDifficulty(parameter);
                parameter = GetBehaviorVal(1, 4);
                speed = GetBehaviorVal(1, 2);
                new (projectile) OscillatingProjectile(&pos, &angles, damage, parameter, speed, 16, 240, 240, 240, 128, 32, 32);
            }
        }
    }
    else
    {
        WeaponNode *fire_weapon = weapon.Get();
        if (fire_weapon)
            fire_weapon->Fire(pos, angles);
        else
        {
            projectile = EffectObject::AllocateStorage(108);
            if (projectile)
            {
                parameter = GetBehaviorVal(1, 1);
                damage = ScaleDamageForDifficulty(parameter);
                parameter = GetBehaviorVal(1, 4);
                speed = GetBehaviorVal(1, 2);
                new (projectile) OscillatingProjectile(&pos, &angles, damage, parameter, speed, 16, 240, 240, 240, 64, 32, 128);
            }
        }
    }
    PlayPositionedSound(20, &model.pos, 2);
}

void Swat::SetMoveTarget(const Vector3 *pos)
{
    FUNCTION_MARKER(0x80053784, "SLUS_003.73");
    if (model.animation != 2)
        StartAnimation(2, 1);
    if (control.variant == 128 && (hidden_models & 9) && model.animation != 1)
        StartAnimation(1, 1);
    dst = *pos;
    control.flags = (control.flags | 1) & 0xFFFFFFC7;
}

void Swat::SetJumpTarget(const Vector3 *pos)
{
    FUNCTION_MARKER(0x80053834, "SLUS_003.73");
    uint32 flags;
    SetAnimationRange(4, 0, 0xFFFFFFFF);
    dst = *pos;
    flags = control.flags | 9;
    jump_ticks = ai.parameters[1];
    control.flags = flags;
}

void Swat::SetSpecialTarget(const Vector3 *pos)
{
    FUNCTION_MARKER(0x8005389C, "SLUS_003.73");
    uint32 flags;
    SetAnimationRange(4, 0, 0xFFFFFFFF);
    dst = *pos;
    flags = control.flags;
    control.gravity_enabled = 1;
    control.flags = flags | 0x11;
}

void Swat::SetTargetRec(uint16 idx)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x80053950, "SLUS_003.73");
    ReadRecPos(&aim_pos, idx);
    control.flags |= 0x40;
}

void Swat::ClearTarget()
{
    FUNCTION_MARKER(0x8005398C, "SLUS_003.73");
    control.flags &= 0xFFFFFF3F;
}

uint8 Swat::ChooseBehavior()
{
    sint32 cumulative[6];
    uint32 distance, idx, minimum, maximum;
    sint32 total = 0, start_weight, end_weight, weight, offset, span, random;
    FUNCTION_MARKER(0x800539A8, "SLUS_003.73");
    distance = model.pos.EuclideanDistance(visual_lists.players->pos);
    for (idx = 0; idx < 6; ++idx)
    {
        const BehaviorRange *range = behavior_ranges + idx;
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
                offset = distance - minimum;
                span = maximum - minimum;
                weight = (end_weight - start_weight) * offset / span + start_weight;
            }
        }
        total += weight;
        cumulative[idx] = total;
    }
    random = RandomRange(total);
    for (idx = 0; idx < 6; ++idx)
        if (random < cumulative[idx])
            return idx;
    return 4;
}

void Swat::SetDeathState(uint16 state)
{
    ai.state = state;
    ai.phase = 0;
}

uint16 Swat::ResolveCommandVal(uint16 val)
{
    return (val & 0x2000) ? ResolveParameter(val) : val;
}

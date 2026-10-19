#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio_session.h"
#include "game_tuning.h"
#include "game_tick.h"

#include "pickup_factory.h"

#include "world_gravity.h"

#include "object_initializers.h"

#include "effect_object.h"
#include "camera.h"
#include "player.h"

#include "weapon_nodes.h"
#include "projectile_line.h"
#include "enemy_list.h"
#include "visual_object.h"
#include "angle_math.h"
#include "object_damage.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "collision.h"
#include "model_animation.h"
#include "model_attachment.h"
#include "game_random.h"
#include "game_diagnostics.h"

#include "plague.h"
#include "ring_burst_effect.h"
#include "game_panel.h"
#include "glow_effect.h"
#include "ring_effect.h"
#include "game_heap.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include "effect_lifecycle.h"
#include "record_data.h"

#include "object_force.h"

// Native extractions of original packed-angle loads and repeated motion blocks

#include "level_session.h"
#include "projectile_trail.h"
#include "plague_ground_effects.h"
#include "particle_settings.h"

#include "textured_quad.h"
#include "plague_area.h"

// Native extractions of repeated waypoint, velocity and recovery blocks

// Native extractions of original weapon construction and sine-table blocks

// Native callback ABI discards the original parameter-reader result

// Native pointer adapter for original coordinate loads

static uint32 DividePlagueJump(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

void Plague::StopMotion()
{
    motion.velocity.words[2] = 0;
    motion.velocity.words[1] = 0;
    motion.velocity.words[0] = 0;
    motion.acceleration.words[2] = 0;
    motion.acceleration.words[1] = 0;
    motion.acceleration.words[0] = 0;
}

void Plague::SelectFarTarget(Vector3 &position)
{
    RecordDataBorrow record_borrow;
    uint32 random;
    do
    {
        random = RandomRange(8);
        ReadRecordPosition(position.words, spawn_points.far_points[random]);
    } while (MeasurePositionDistance(position.words, model.pos.words) < 257);
}

void Plague::ClearVelocity()
{
    motion.velocity.words[2] = 0;
    motion.velocity.words[1] = 0;
    motion.velocity.words[0] = 0;
}

void Plague::SaveRecovery(uint32 state)
{
    uint32 previous = ai.state;
    uint32 substate = ai.phase;
    ai.state = state;
    ai.phase = 0;
    ai.parameters[2] = previous;
    ai.parameters[3] = substate;
}

void Plague::CreateOrbitWeapon(uint32 index)
{
    GameHeapBlock *allocation = AllocateZeroedHeapBlock(104);
    ParticleBeam *created = 0;
    if (allocation)
    {
        created = new (allocation) ParticleBeam(&visual_lists.players, 64, 1, 0, 118);
    }
    orbit_weapons[index] = created;
    if (!created)
    {
        GameDiagnostic("Plague orbit weapon allocation failed\n");
        abort();
    }
    created->damage = 4;
}

uint32 Plague::ReadOrbitDelta() const
{
    uint32 sine = rsin(orbit_phase & 4095);
    return (uint32)((sint32)(19 * sine) >> 11);
}

void Plague::SetParameters(uint32 command)
{
    ReadParameters(command);
}

static const uint8 spread_radius[16] = {32, 60, 64, 60, 58, 58, 64, 76, 96, 76, 64, 58, 58, 60, 64, 60};

Plague::Plague(const RecordCursor &record_data, uint32 index) : Enemy(), last_pickup_tick(0), jump_index(0), attack_count(0), jump_timer(0), pickup_stage(0), orbit_sound(0), orbit_phase(0), orbit_timer(0)
{
    RecordDataBorrow record_borrow;

    uint32 value;
    RecordCursor rotation;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8004509C, "SLUS_003.73");
    facing_pos.words[0] = facing_pos.words[1] = facing_pos.words[2] = 0;
    orbit_angles.words[0] = orbit_angles.words[1] = orbit_angles.words[2] = 0;
    rotation = ReadPosition(record_data);
    value = rotation.Read16();
    cursor = rotation + 6;
    this->model.angles.words[0] = value;
    value = (rotation + 2).Read16();
    this->model.angles.words[1] = value;
    value = (rotation + 4).Read16();
    script_cursor = cursor;
    this->control.script_active = 1;
    this->record_idx = index;
    this->model.angles.words[2] = value;
    Configure();
    ExecuteScript(script_cursor);
}

void Plague::Configure()
{
    uint32 value;
    FUNCTION_MARKER(0x80045134, "SLUS_003.73");
    SetModelBank("plague");
    motion.drag[0] = 31;
    motion.drag[1] = 31;
    motion.drag[2] = 31;
    AddToList(visual_lists.actors);
    StartAnimation(0, 1);
    AllocateBehaviorTable(1, 5);
    value = ScaleDamageForDifficulty(10);
    SetBehaviorValue(0, 1, value & 0xFFFF);
    SetBehaviorValue(0, 2, 160);
    ai.parameters[0] = 16;
    ai.parameters[1] = 32;
    this->kind = 223;
    contact_strength = 128;
    ai.parameters[9] = (ai.parameters[9] & 0xFF00) | (uint8)(0);
    ai.state = 0;
    ai.phase = 0;
    this->model.scale.words[0] = 6000;
    this->model.scale.words[1] = 6000;
    this->model.scale.words[2] = 6000;
    this->ground_offset = 292;
    value = this->model.flags;
    this->control.hierarchy_ready = 0;
    projection.enabled = true;
    this->model.flags = value | 0x200;
    memset(&projection.dir, 0, sizeof(projection.dir));
    projection.dir.m[0][1] = -4096;
    projection.color = 0x808080;
    camera_subject.SetObject(&model);
}

Plague::~Plague()
{
    if (attack_ring)
        attack_ring->Destroy(3);
    if (orbit_sound)
        StopSoundVoices(orbit_sound);
    for (uint32 index = 0; index < 3; ++index)
        if (orbit_weapons[index])
            orbit_weapons[index]->Destroy(3);
}

uint32 Plague::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800452B4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    FreeBehaviorTable();
    ClearTrackedObjectHealth();
    projection.enabled = false;
    if (attack_ring)
        attack_ring->Destroy(3);
    attack_ring = 0;
    if (orbit_sound)
        StopSoundVoices(orbit_sound);
    orbit_sound = 0;
    for (uint32 index = 0; index < 3; ++index)
    {
        if (orbit_weapons[index])
            orbit_weapons[index]->Destroy(3);
        orbit_weapons[index] = 0;
    }
    camera_subject.Reset();
    Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(this);
    return 0;
}

uint32 Plague::Update()
{
    ModelBorrow model_borrow;
    RecordDataBorrow record_borrow;
    ModelInstance *hero;
    uint32 x, y, z, flags, value, timer;
    uint32 scale, vx, vy, vz, distance, damage;
    CollisionRay record;
    Vector3 delta, normal;
    ShortVector3 desired;
    sint32 frame, magnitude;
    FUNCTION_MARKER(0x800453F4, "SLUS_003.73");
    hero = visual_lists.players;
    if (!hero)
        return 0;
    x = (uint32)zero_vector[0];
    y = (uint32)zero_vector[1];
    z = (uint32)zero_vector[2];
    motion.acceleration.words[0] = x;
    motion.acceleration.words[1] = y;
    motion.acceleration.words[2] = z;
    if (!(uint32)this->control.hierarchy_ready)
    {
        SetModelBoneTable(this->model.bank, PlagueAttachments);
        this->control.hierarchy_ready = 1;
    }
    flags = this->activation_flags;
    this->control.prev_activation_flags = flags;
    if (flags & 1)
    {
        value = (uint32)this->control.activation_count;
        this->control.activation_count = value + 1;
        value = this->control.flags;
        this->activation_flags = flags & 0xFFFE;
        this->control.flags = value | 0x100;
    }
    if (player_distance < 256)
    {
        hero = visual_lists.players;
        if (hero)
            static_cast<VisualObject *>(visual_lists.players->owner)->ApplyContactImpulse(*this);
    }
    if ((uint32)this->control.script_active)
    {
        timer = (uint32)this->control.script_delay;
        if (timer)
            this->control.script_delay = timer - 1;
        else
            ExecuteScript(script_cursor);
    }
    else
        UpdateState();
    UpdateOrbitingWeapons();
    SpawnPickup();
    if ((uint32)this->control.gravity_enabled)
        AddVectors(motion.acceleration.words, world_gravity.vector.words);
    IntegrateMotion();
    record.endpoints[0].words[0] = this->model.pos.words[0];
    record.endpoints[0].words[1] = this->model.pos.words[1] - 0x40000;
    z = this->model.pos.words[2];
    record.endpoints[1].words[0] = record.endpoints[0].words[0];
    record.endpoints[1].words[1] = record.endpoints[0].words[1] + 0x1000000;
    record.endpoints[0].words[2] = z;
    record.endpoints[1].words[2] = z;
    InitializeCollisionRay(record);
    QueryCollisionRay(record, 1);
    if (record.hit.GetObject())
    {
        projection.enabled = true;
        surface.pos.words[0] = record.hit_pos.words[0];
        surface.pos.words[1] = record.hit_pos.words[1];
        surface.pos.words[2] = record.hit_pos.words[2];
        surface.n.vx = (uint16)record.n.vx;
        surface.n.vy = (uint16)record.n.vy;
        surface.n.vz = (uint16)record.n.vz;
    }
    else
        projection.enabled = false;
    flags = this->control.flags;
    if (flags & 0x31)
    {
        if (flags & 16)
        {
            ModelInstance *subject = tracked_target.GetObject();
            if (subject)
            {
                facing_pos.words[0] = subject->pos.words[0];
                facing_pos.words[1] = subject->pos.words[1];
                facing_pos.words[2] = subject->pos.words[2];
            }
            else
                control.flags &= ~16;
        }
        const uint32 *aim_pos = flags & 0x30 ? facing_pos.words : destination.words;
        CalculateDirectionAngles(desired.words, model.pos.words, aim_pos);
        desired.components[0] = 0;
        scale = flags & 0x30 ? 32 : ai.parameters[1];
        UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, scale);
        if (flags & 0x30)
            goto attack;
        if (this->control.flags & 4)
        {
            frame = (sint8)this->model.frame;
            if (frame < 13)
            {
                this->playback.speed = 0x10000;
                this->control.gravity_enabled = 0;
                StopMotion();
            }
            else if (jump_timer)
                this->control.gravity_enabled = 1;
            frame = (sint8)this->model.frame;
            if (frame >= 13 && !(this->control.flags & 8))
            {
                x = this->destination.words[0];
                y = this->model.pos.words[0];
                timer = jump_timer;
                vx = DividePlagueJump(x - y, timer);
                x = this->destination.words[2];
                y = this->model.pos.words[2];
                timer = jump_timer;
                vz = DividePlagueJump(x - y, timer);
                x = this->destination.words[1];
                y = this->model.pos.words[1];
                timer = jump_timer;
                vy = DividePlagueJump(x - y, timer);
                value = this->control.flags;
                this->control.flags = value | 8;
                motion.velocity.words[0] = vx;
                motion.velocity.words[2] = vz;
                motion.velocity.words[1] = vy - (timer << 14);
                PlayPositionedSound(114, model.pos.words, 0);
            }
            else
            {
                frame = (sint8)this->model.frame;
                if (frame >= 23)
                {
                    if (!jump_timer)
                        goto jump_finished;
                    vy = motion.velocity.words[1];
                    this->playback.speed = (sint32)vy >= 0 && frame < 34 ? 0x10000 : 0;
                }
            }
            timer = jump_timer;
            if (timer)
            {
                value = (timer - 1) & 0xFFFF;
                jump_timer = value;
                if (!value)
                {
                    PlayPositionedSound(119, model.pos.words, 0);
                    x = this->destination.words[0];
                    y = this->destination.words[1];
                    z = this->destination.words[2];
                    this->model.pos.words[0] = x;
                    this->model.pos.words[1] = y;
                    this->model.pos.words[2] = z;
                    StopMotion();
                    this->playback.speed = 0x10000;
                    this->control.gravity_enabled = 0;
                }
            }
        jump_finished:
            if (this->playback.complete)
                this->control.flags = this->control.flags & 0xFFFFFFF2;
        }
        else
        {
            value = this->model.animation;
            if (value != 6 && value != 1)
                StartAnimation(1, 1);
            for (value = 0; value < 3; ++value)
            {
                x = this->destination.words[value];
                y = this->model.pos.words[value];
                delta.words[value] = (uint32)((sint32)(x - y) >> 12);
            }
            NormalizeShortVector(delta.words, normal.words);
            magnitude = (sint16)motion.angular_velocity.words[1];
            if (magnitude < 0)
                magnitude = -magnitude;
            if (magnitude > 128)
                magnitude = 128;
            scale = ai.parameters[0] * (128 - (uint32)magnitude) / 128;
            vx = normal.words[0] * scale;
            vy = normal.words[1] * scale;
            vz = normal.words[2] * scale;
            motion.velocity.words[0] = vx;
            motion.velocity.words[1] = vy;
            motion.velocity.words[2] = vz;
            distance = MeasureHorizontalDistance(this->model.pos.words, this->destination.words);
            scale = ai.parameters[0];
            if (distance < 2 * scale)
                this->control.flags = this->control.flags & ~1;
        }
    }
attack:
    if (this->model.animation == 7)
    {
        hero = visual_lists.players;
        if (hero && (uint32)(this->model.frame - 15) < 5 && MeasureHorizontalDistance(model.pos.words, hero->pos.words) < 280)
        {
            damage = ScaleDamageForDifficulty(2);
            VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
            damage_target->Damage(damage, zero_vector.components, 0);
        }
    }
    return UpdateWorldTransform();
}

uint32 Plague::Damage(uint32 damage, const sint32 *, uint32 reason)
{
    uint32 value, health;
    FUNCTION_MARKER(0x80045B14, "SLUS_003.73");
    value = (uint32)this->control.damage_phase;
    this->control.damage_phase = value + 1;
    value = (uint32)((sint32)damage >> 1);
    if (reason == 30)
        value = (uint32)((sint32)value >> 1);
    else if (reason == 11)
        value -= (uint32)((sint32)value >> 1);
    health = (uint16)this->health - value;
    this->health = (sint16)(uint16)(health);
    if ((sint32)(health << 16) <= 0)
        ClearTrackedObjectHealth();
    SetColor(3, 0, 255, 0);
    return 1;
}

void Plague::UpdateState()
{
    ModelBorrow model_borrow;
    RecordDataBorrow record_borrow;
    ModelInstance *hero;
    uint32 state = ai.state, substate, value, flags;
    GameHeapBlock *child;
    uint32 second;
    uint32 random;
    uint32 timer;
    uint32 phase;
    uint32 speed;
    uint32 damage;
    Vector3 position, velocity, scaled, sum1, sum2, sum3, sum4;
    uint32 scalar, x, y, angle;
    ShortVector3 direction, aim;
    sint32 health, radius, ground, increment;
    FUNCTION_MARKER(0x80045BA8, "SLUS_003.73");
    if (state >= 33)
        return;
    switch (state)
    {
        case 0:
            substate = ai.phase;
            if (!substate)
            {
                ground = (sint32)FindGroundHeight(model.pos.words, 0, 4096);
                if (ground == -1)
                    return;
                value = this->model.pos.words[1];
                radius = (sint16)(uint32)this->ground_offset;
                if (((sint32)((uint32)ground - value) >> 12) >= 2 * radius)
                    return;
                value = (uint32)ground - ((uint32)radius << 12);
                this->model.pos.words[1] = value;
                shadow.height = (uint32)((sint32)((uint32)ground - value) >> 12);
                surface.pos.words[1] = (uint32)ground;
                this->control.gravity_enabled = 0;
                ClearVelocity();
                ai.work[1] = 0;
                this->health = (sint16)(uint16)(800);
                ::TrackObjectHealth(*this, 24, 42, 4096);
                StartAnimation(0, 1);
                goto advance_substate;
            }
            if (substate != 1)
                return;
            value = ai.work[1];
            increment = (sint16)ai.work[1];
            if (value < 180)
                ai.work[1] = (uint32)increment + 1;
            else
            {
                ai.work[1] = 0;
                ai.state = substate;
                ai.phase = 0;
            }
            return;

        case 1:
            pickup_stage = 1;
            if (!RandomRange(64))
            {
                random = RandomRange(2);
                ReadRecordPosition(position.words, spawn_points.ground_effect[random]);
                child = AllocateEffectStorage(112);
                if (child)
                {
                    damage = ScaleDamageForDifficulty(10);
                    new (child) PlagueGroundEffect(position.words, damage, 64, 128, 16, 20, 48);
                }
                PlayPositionedSound(116, position.words, 0);
            }
            substate = ai.phase;
            if (!substate)
            {
                if (this->control.flags & 1)
                    return;
                if (this->health < 560)
                {
                    second = spawn_points.ground_effect[0];
                    ::ActivateObjectsByIndex(visual_lists.solids, second);
                    second = spawn_points.ground_effect[1];
                    ::ActivateObjectsByIndex(visual_lists.solids, second);
                    goto advance_state;
                }
                value = jump_index + 1;
                jump_index = value;
                if ((value & 255) >= 2)
                    jump_index = 0;
                value = jump_index;
                ReadRecordPosition(position.words, spawn_points.near_points[value]);
                ground = (sint32)FindGroundHeight(position.words, 512, 2048);
                radius = (sint16)(uint32)this->ground_offset;
                position.words[1] = (uint32)ground - ((uint32)radius << 12);
                SetJumpTarget(position.words);
                value = ai.phase;
                attack_count = 0;
                goto store_substate;
            }
            if (substate != 1 || (this->control.flags & 1))
                return;
            phase = attack_count;
            if (phase >= 2 || (phase == substate && RandomRange(2)))
                goto far_jump_and_repeat;
            if (this->health < 560)
                goto far_jump_and_repeat;
            phase = attack_count;
            state = ai.state;
            substate = ai.phase;
            ai.state = 16;
            ai.phase = 0;
            attack_count = phase + 1;
            ai.parameters[2] = state;
            ai.parameters[3] = substate;
            return;

        case 2:
            flags = this->control.flags;
            pickup_stage = 2;
            if (!(flags & 128))
            {
                increment = (sint16)ai.work[0];
                if (increment >= 101)
                {
                    value = orbit_sound;
                    this->control.flags = flags | 128;
                    ::StopSoundVoices(value);
                }
                else
                    ai.work[0] = (uint32)increment + 1;
            }
            substate = ai.phase;
            if (!substate)
            {
                second = spawn_points.orbit_record;
                ::ActivateObjectsByIndex(visual_lists.solids, second);
                value = PlaySoundEffect(115, 8192, 0);
                orbit_sound = value;
            }
            if (substate <= 1)
            {
                if (this->control.flags & 1)
                    return;
                random = RandomRange(2);
                ReadRecordPosition(position.words, spawn_points.near_points[random]);
                ground = (sint32)FindGroundHeight(position.words, 512, 2048);
                radius = (sint16)(uint32)this->ground_offset;
                position.words[1] = (uint32)ground - ((uint32)radius << 12);
                SetJumpTarget(position.words);
                goto advance_substate;
            }
            if (substate != 2)
                return;
            flags = this->control.flags;
            if (flags & 1)
                return;
            if (this->health < 320 && (flags & 128))
            {
                second = spawn_points.orbit_record;
                ::ActivateObjectsByIndex(visual_lists.solids, second);
                this->control.flags = this->control.flags & 0xFFFFFF7F;
                SelectFarTarget(position);
                ground = (sint32)FindGroundHeight(position.words, 512, 2048);
                radius = (sint16)(uint32)this->ground_offset;
                position.words[1] = (uint32)ground - ((uint32)radius << 12);
                SetJumpTarget(position.words);
                value = ai.state;
                ai.phase = 0;
                ai.work[0] = 0;
                goto store_state;
            }
            goto far_jump_and_repeat;

        case 3:
            substate = ai.phase;
            pickup_stage = 3;
            if (!substate)
            {
                if (this->control.flags & 1)
                    return;
                StartAnimation(0, 1);
                TrackTarget(visual_lists.players);
                goto advance_substate;
            }
            if (substate != 1)
                return;
            increment = (sint16)ai.work[0];
            if (increment < 161)
                ai.work[0] = (uint32)increment + 1;
            else
            {
                ClearFacing();
                goto advance_state;
            }
            return;

        case 4:
            substate = ai.phase;
            pickup_stage = 4;
            if (!substate)
            {
                value = ai.phase;
                ai.parameters[0] = 42;
                goto store_substate;
            }
            if (substate == 1)
            {
                if (this->control.flags & 1)
                    return;
                if (this->model.animation != 9)
                    SetAnimationRange(9, 0, 0xFFFFFFFF);
                else if (this->playback.complete)
                    ai.phase = 2;
                return;
            }
            if (substate != 2)
                return;
            if (this->health <= 0)
            {
                this->control.flags = this->control.flags & 0xFFFFFFFE;
                ClearVelocity();
                ai.state = 5;
                ai.phase = 0;
                return;
            }
            if (this->model.animation != 6)
                StartAnimation(6, 1);
            flags = this->control.flags;
            if (!(flags & 1) || !RandomRange(12))
            {
                if (RandomRange(3))
                {
                    hero = visual_lists.players;
                    position = hero->pos;
                }
                else
                {
                    hero = visual_lists.players;
                    scalar = 8;
                    position = hero->pos;
                    MultiplyScalarVector(velocity.words, scalar, static_cast<VisualObject *>(hero->owner)->motion.velocity.words);
                    velocity.words[1] = 0;
                    scalar = 8;
                    MultiplyScalarVector(scaled.words, scalar, velocity.words);
                    AddVectors(position.words, scaled.words);
                }
                ground = (sint32)FindGroundHeight(position.words, 512, 2048);
                if (ground == -1)
                    position.words[1] = this->model.pos.words[1];
                else
                {
                    radius = (sint16)(uint32)this->ground_offset;
                    position.words[1] = (uint32)ground - ((uint32)radius << 12);
                }
                SetMoveTarget(position.words);
            }
            increment = (sint16)ai.work[1];
            if (increment)
                value = (uint32)increment - 1;
            else
            {
                position = this->model.pos;
                y = position.words[1];
                radius = (sint16)(uint32)this->ground_offset;
                use_small_effect_pool = false;
                position.words[1] = y + 0xFFFF0000 + ((uint32)radius << 12);
                child = AllocateEffectStorage(172);
                if (child)
                {
                    health = this->health;
                    damage = (uint32)((sint32)(150 * (240 - (uint32)health)) / 240) + 150;
                    PlagueArea *area = new (child) PlagueArea();
                    area->EnableSemiTransparency();
                    area->SelectResource(0x019D23F9);
                    area->depth_bias = 128;
                    direction.components[2] = 0;
                    direction.components[0] = 0;
                    direction.components[1] = -4096;
                    random = RandomRange(48);
                    area->SetColors((random + 64) & 255, 128, 0);
                    area->radius = 0;
                    value = area->radius;
                    area->SetPosition(position.words, direction.components, value, 0);
                    area->lifetime = damage;
                    AddVectors(sum1.words, area->pos.words, area->corners[0].words);
                    AddVectors(sum2.words, sum1.words, area->corners[1].words);
                    AddVectors(sum3.words, sum2.words, area->corners[2].words);
                    scalar = 2;
                    ShiftVectorRight(sum4.words, sum3.words, scalar);
                    area->center = sum4;
                }
                use_small_effect_pool = true;
                value = 5;
            }
            ai.work[1] = value;
            hero = visual_lists.players;
            if (MeasureHorizontalDistance(model.pos.words, hero->pos.words) < 256)
                SaveRecovery(32);
            return;

        case 5:
            substate = ai.phase;
            if (!substate)
            {
                hero = visual_lists.players;
                Camera *camera;
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                static_cast<Player *>(visual_lists.players->owner)->disable_timer = 900;
                if (camera)
                {
                    camera->follow.flags = 1;
                    camera->follow.target.SetObject(&model);
                    camera->aim.flags = 1;
                    camera->aim.target.SetObject(&model);
                    SetCameraOrbitDistanceTransition(1584, 16);
                    SetCameraOrbitHeightTransition((uint32)-1208, 16);
                    SetCameraTargetLateralOffsetTransition(0, 16);
                    SetCameraTargetHeightOffsetTransition((uint32)-76, 16);
                    SetCameraTargetForwardOffsetTransition(240, 16);
                    camera->SetYawTransition(2496, 16);
                }
                SetAnimationRange(8, 0, 0xFFFFFFFF);
                goto advance_substate;
            }
            if (substate == 3)
            {
                CompleteLevel(0);
                return;
            }
            if (substate == 1)
            {
                if (this->playback.complete)
                {
                    SetAnimationRange(4, 0, 0xFFFFFFFF);
                    value = ai.phase;
                    ai.work[0] = 0;
                    ai.work[1] = 16;
                    ai.phase = value + 1;
                }
            }
            else if (substate != 2)
                return;
            increment = (sint8)this->model.frame;
            timer = ai.work[0];
            if (increment >= 45)
            {
                direction.components[0] = 512;
                use_small_effect_pool = false;
                direction.components[1] = 4096;
                direction.components[2] = 0;
                SetParticleSpread(direction.components);
                ::SetParticleSize(2);
                random = RandomRange(64);
                SetParticleColor((random + 48) & 255, 128, 0);
                SetParticleColorStep(1, 2, 0);
                GetModelAttachment(position.words, *this, 0);
                CalculateDirectionAngles(aim.words, model.pos.words, position.words);
                velocity = position;
                direction.components[0] = -1024;
                direction.components[1] = 0;
                direction.components[2] = 0;
                velocity.words[1] += 0xFF800000;
                child = AllocateEffectStorage(108);
                if (child)
                {
                    new (child) TrailProjectile(position.words, velocity.words, direction.words, 16, 64, 64, 200, 240, 200, 0, 128, 0);
                }
                GetModelAttachment(position.words, *this, 1);
                CalculateDirectionAngles(aim.words, model.pos.words, position.words);
                velocity = position;
                velocity.words[1] += 0xFF800000;
                child = AllocateEffectStorage(108);
                if (child)
                {
                    new (child) TrailProjectile(position.words, velocity.words, direction.words, 16, 64, 64, 200, 240, 200, 0, 128, 0);
                }
                use_small_effect_pool = true;
                if ((sint8)this->model.frame >= 68)
                    this->model.frame = 68;
            }
            if (!RandomRange(12))
            {
                random = RandomRange(2);
                PlayPositionedSound(random + 1, model.pos.words, 0);
                child = AllocateEffectStorage(124);
                if (child)
                {
                    new (child) ExpandingGlow(model.pos.words, 64, 128, 0, 512, 10);
                }
                child = AllocateEffectStorage(124);
                if (child)
                {
                    new (child) RingBurstEffect(model.pos.words, 5, 255, 255, 255, 1, 32, 128, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 4);
                }
            }
            if (timer > 180)
            {
                increment = (sint16)this->model.scale.words[0];
                value = this->model.scale.words[0] - 128;
                if (increment > 512)
                {
                    second = this->model.scale.words[2];
                    this->model.scale.words[0] = value;
                    this->model.scale.words[2] = second - 128;
                }
                else
                {
                    increment = (sint16)this->model.scale.words[1];
                    value = this->model.scale.words[1] - 512;
                    if (increment > 512)
                        this->model.scale.words[1] = value;
                    else
                        ai.phase = ai.phase + 1;
                }
            }
            ai.work[0] = timer + 1;
            increment = (sint16)ai.work[1];
            angle = this->model.angles.words[1];
            this->model.angles.words[1] = angle + (uint32)increment;
            ai.work[1] = (uint32)increment + 1;
            return;

        case 16:
            substate = ai.phase;
            if (!substate)
            {
                flags = this->control.flags;
                hero = visual_lists.players;
                this->control.flags = flags & 0xFFFFFFFE;
                ClearVelocity();
                TrackTarget(visual_lists.players);
                SetAnimationRange(2, 0, 0xFFFFFFFF);
                GetModelAttachment(position.words, *this, 0);
                GameHeapBlock *allocation = AllocateEffectStorage(104);
                RingEffect *created = 0;
                if (allocation)
                {
                    created = new (allocation) RingEffect(position.words, 32, 64, 128, 128, 128, 16, 64, 0);
                }
                attack_ring = created;
                if (!attack_ring)
                {
                    GameDiagnostic("Plague attack ring allocation failed\n");
                    abort();
                }
                attack_ring->SetRadii(16);
                goto advance_substate;
            }
            if (substate == 2)
                goto restore_recovery;
            if (substate != 1)
                return;
            GetModelAttachment(position.words, *this, 0);
            if ((sint8)this->model.frame < 37)
            {
                if (attack_ring)
                {
                    attack_ring->SetPosition(position.words);
                    increment = (sint8)model.frame;
                    attack_ring->SetRadii(((uint32)increment << 2) + 16);
                }
                return;
            }
            if (attack_ring)
            {
                attack_ring->Destroy(3);
                attack_ring = 0;
            }
            hero = visual_lists.players;
            CalculateDirectionAngles(aim.words, position.words, hero->pos.words);
            child = AllocateEffectStorage(268);
            if (child)
            {
                speed = GetBehaviorValue(0, 2) & 65535;
                damage = GetBehaviorValue(0, 1) & 65535;
                new (child) LineProjectile(position.words, aim.components, speed, damage, &visual_lists.players, 64, 96, 0, 0);
            }
            for (phase = 0; phase < 4096; phase += 256)
            {
                increment = rcos(phase & 4095);
                value = spread_radius[phase >> 8];
                x = (uint32)increment * value;
                increment = rsin(phase & 4095);
                y = (uint32)increment * value;
                direction.components[2] = 0;
                direction.components[0] = (sint16)(((uint32)(uint16)aim.components[0] - (uint32)((sint32)x >> 12)) & 4095);
                direction.components[1] = (sint16)(((uint32)(uint16)aim.components[1] + (uint32)((sint32)y >> 12)) & 4095);
                child = AllocateEffectStorage(268);
                if (child)
                {
                    speed = GetBehaviorValue(0, 2) & 65535;
                    damage = GetBehaviorValue(0, 1) & 65535;
                    new (child) LineProjectile(position.words, direction.components, speed, damage, &visual_lists.players, 64, 96, 0, 0);
                }
            }
            PlayPositionedSound(117, model.pos.words, 0);
            goto advance_substate;

        case 32:
            substate = ai.phase;
            if (!substate)
            {
                flags = this->control.flags;
                hero = visual_lists.players;
                this->control.flags = flags & 0xFFFFFFFE;
                ClearVelocity();
                TrackTarget(visual_lists.players);
                SetAnimationRange(7, 0, 0xFFFFFFFF);
                goto advance_substate;
            }
            if (substate == 1)
            {
                if (!this->playback.complete)
                    return;
                increment = (sint8)this->model.frame;
                SetAnimationRange(7, (uint32)increment, 0);
                goto advance_substate;
            }
            if (substate == 2)
                goto restore_recovery;
            return;

        default:
            return;
    }

far_jump_and_repeat:
    SelectFarTarget(position);
    ground = (sint32)FindGroundHeight(position.words, 512, 2048);
    radius = (sint16)(uint32)this->ground_offset;
    position.words[1] = (uint32)ground - ((uint32)radius << 12);
    SetJumpTarget(position.words);
    value = ai.phase;
    ai.phase = value - 1;
    return;

advance_state:
    value = ai.state;
    ai.phase = 0;
store_state:
    ai.state = value + 1;
    return;

advance_substate:
    value = ai.phase;
store_substate:
    ai.phase = value + 1;
    return;

restore_recovery:
    if (!this->playback.complete)
        return;
    ClearFacing();
    state = ai.parameters[2];
    substate = ai.parameters[3];
    ai.state = state;
    ai.phase = substate;
}

uint32 Plague::SpawnPickup()
{
    RecordDataBorrow record_borrow;
    uint32 state, now, elapsed, index, trigger, choice, resource;
    Vector3 position;
    sint32 health, maximum;
    FUNCTION_MARKER(0x80046EBC, "SLUS_003.73");
    state = pickup_stage;
    now = game_clock.tick;
    elapsed = now - last_pickup_tick;
    if ((sint32)elapsed < 481)
        return 0xFFFFFFFF;
    if (state - 1 < 2)
    {
        if (state == 2)
        {
            maximum = (sint32)(sint16)GetGameTuning().player.maximum_health;
            ModelInstance *player = visual_lists.players;
            health = static_cast<VisualObject *>(player->owner)->health;
            maximum = (maximum + (maximum < 0 ? 1 : 0)) >> 1;
            if (health < maximum)
            {
                index = RandomRange(2);
                trigger = spawn_points.early_support[index];
                ReadRecordPosition(position.words, trigger);
                resource = 14;
                goto create;
            }
        }
        choice = RandomRange(3);
        if (!choice)
        {
            index = RandomRange(2);
            trigger = spawn_points.early_support[index];
            ReadRecordPosition(position.words, trigger);
            resource = 5;
            goto create;
        }
        index = RandomRange(4);
        trigger = spawn_points.early_pickups[index];
    }
    else
    {
        choice = RandomRange(3);
        if (choice == 1)
        {
            index = RandomRange(2);
            trigger = spawn_points.late_health[index + 2];
            ReadRecordPosition(position.words, trigger);
            resource = 14;
            goto create;
        }
        if (choice)
        {
            index = RandomRange(2);
            trigger = spawn_points.late_health[index];
            ReadRecordPosition(position.words, trigger);
            resource = 15;
            goto create;
        }
        index = RandomRange(4);
        trigger = spawn_points.late_pickups[index];
    }
    ReadRecordPosition(position.words, trigger);
    choice = RandomRange(2);
    resource = choice ? 3 : 4;
create:
    ::SpawnPickup(resource, position.words, 4, 240, zero_vector.words);
    now = game_clock.tick;
    last_pickup_tick = now;
    return now;
}

uint32 Plague::UpdateOrbitingWeapons()
{
    RecordDataBorrow record_borrow;
    ModelInstance *node = visual_lists.solids;
    uint32 timer, angle, index, random;
    Vector3 position, direction;
    ShortVector3 angles;
    FUNCTION_MARKER(0x800470B0, "SLUS_003.73");
    orbit_object.Reset();
    if (node)
    {
        uint32 identity = spawn_points.orbit_record;
        while (static_cast<VisualObject *>(node->owner)->record_idx != identity)
        {
            node = node->next;
            if (!node)
                break;
        }
        if (node)
            orbit_object.SetObject(node);
    }
    if (control.flags & 128)
    {
        ReadRecordPosition(position.words, spawn_points.orbit_center);
        if (!orbit_weapons[0])
        {
            orbit_angles.words[1] = 1365;
            orbit_angles.words[0] = 0;
            orbit_angles.words[2] = 2730;
            CreateOrbitWeapon(0);
        }
        else if (!orbit_weapons[1])
        {
            if (orbit_timer >= 151)
                CreateOrbitWeapon(1);
            else
            {
                timer = orbit_timer;
                orbit_weapons[0]->segment_len = (uint32)((sint32)(timer << 12) / 150 / 9);
            }
        }
        else if (!orbit_weapons[2])
        {
            if (orbit_timer >= 301)
                CreateOrbitWeapon(2);
            else
            {
                timer = (uint32)orbit_timer - 150;
                orbit_weapons[1]->segment_len = (uint32)((sint32)(timer << 12) / 150 / 9);
            }
        }
        else if (orbit_timer < 450)
        {
            timer = (uint32)orbit_timer - 300;
            orbit_weapons[2]->segment_len = (uint32)((sint32)(timer << 12) / 150 / 9);
        }
        for (index = 0; index < 3; ++index)
        {
            uint32 delta = ReadOrbitDelta();
            angle = (orbit_angles.words[index] + delta) & 4095;
            orbit_angles.words[index] = angle;
            if (orbit_weapons[index])
            {
                angles.components[0] = 0;
                angles.components[1] = angle;
                angles.components[2] = 0;
                BuildDirectionVector(direction.words, 145, angles.words);
                AddVectors(direction.words, position.words);
                orbit_weapons[index]->Fire(direction.words, angles.components);
            }
        }
        node = orbit_object.GetObject();
        if (node)
            node->angles.words[1] = (node->angles.words[1] + ReadOrbitDelta()) & 4095;
        random = RandomRange(4);
        orbit_phase += 4 + random;
        ++orbit_timer;
        return random;
    }
    node = orbit_object.GetObject();
    if (node)
        node->angles.words[1] = (node->angles.words[1] + 32) & 4095;
    for (index = 0; index < 3; ++index)
    {
        if (orbit_weapons[index])
            orbit_weapons[index]->Destroy(3);
        orbit_weapons[index] = 0;
    }
    return 0;
}

uint32 Plague::ReadParameters(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint16 *fields[8] = {spawn_points.ground_effect, spawn_points.ground_effect + 1, &spawn_points.orbit_center, 0, 0, 0, spawn_points.early_support, spawn_points.early_support + 1};
    uint32 value, index;
    RecordCursor cursor;
    RecordCursor cached;
    FUNCTION_MARKER(0x800474F0, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x2130)
        return Enemy::ReadParameters(command);
    cursor = script_cursor + 2;
    script_cursor = cursor;
    value = cursor.Read16();
    cursor += 2;
    script_cursor = cursor;
    for (index = 0; index < 8; ++index)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        cursor += 2;
        spawn_points.far_points[index] = value;
        script_cursor = cursor;
    }
    cached = cursor;
    value = cached.Read16();
    script_cursor = cached + 2;
    spawn_points.orbit_record = value;
    value = (cached + 2).Read16();
    script_cursor = cached + 36;
    for (index = 0; index < 2; ++index)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        cursor += 2;
        spawn_points.near_points[index] = value;
        script_cursor = cursor;
    }
    cached = cursor;
    for (index = 0; index < 8; ++index)
    {
        value = (cached + 2 * index).Read16();
        script_cursor = cached + 2 * index + 2;
        // Consume unused encoded slots without retaining dead runtime fields
        if (fields[index])
            *fields[index] = value;
    }
    for (index = 0; index < 8; ++index)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        cursor += 2;
        if (index < 4)
            spawn_points.early_pickups[index] = value;
        else
            spawn_points.late_pickups[index - 4] = value;
        script_cursor = cursor;
    }
    cached = cursor;
    for (index = 0; index < 4; ++index)
    {
        value = (cached + 2 * index).Read16();
        script_cursor = cached + 2 * index + 2;
        spawn_points.late_health[index] = value;
    }
    return 0;
}

uint32 Plague::ExecuteCommand(uint32 command)
{
    FUNCTION_MARKER(0x800476D0, "SLUS_003.73");
    return Enemy::ExecuteCommand(command & 0xFFFF);
}

uint32 Plague::SetJumpTarget(const uint32 *position)
{
    FUNCTION_MARKER(0x800476F0, "SLUS_003.73");
    SetAnimationRange(5, 0, (uint32)-1);
    uint32 x = position[0];
    uint32 y = position[1];
    uint32 z = position[2];
    this->destination.words[0] = x;
    this->destination.words[1] = y;
    this->destination.words[2] = z;
    uint32 flags = this->control.flags;
    jump_timer = 40;
    flags |= 5;
    this->control.flags = flags;
    return flags;
}

uint32 Plague::SetMoveTarget(const uint32 *position)
{
    FUNCTION_MARKER(0x80047758, "SLUS_003.73");
    uint32 x = position[0];
    uint32 y = position[1];
    uint32 z = position[2];
    this->destination.words[0] = x;
    this->destination.words[1] = y;
    this->destination.words[2] = z;
    uint32 flags = (this->control.flags | 1) & 0xFFFFFFF3;
    this->control.flags = flags;
    return flags;
}

uint32 Plague::TrackTarget(ModelInstance *target)
{
    FUNCTION_MARKER(0x80047788, "SLUS_003.73");
    uint32 flags = static_cast<VisualObject *>(target->owner)->IsFinished();
    if (flags)
        return flags;
    tracked_target.SetObject(target);
    flags = (control.flags & 0xFFFFFFDF) | 16;
    control.flags = flags;
    return flags;
}

uint16 *Plague::ClearFacing()
{
    FUNCTION_MARKER(0x80047808, "SLUS_003.73");
    uint32 flags = this->control.flags & 0xFFFFFFCF;
    this->control.flags = flags;
    motion.angular_velocity.words[2] = 0;
    motion.angular_velocity.words[1] = 0;
    motion.angular_velocity.words[0] = 0;
    motion.angular_acceleration.words[2] = 0;
    motion.angular_acceleration.words[1] = 0;
    motion.angular_acceleration.words[0] = 0;
    return motion.angular_acceleration.words;
}

#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "effect_lifecycle.h"
#include "game_tuning.h"
#include "explosion_effects.h"
#include "game_settings.h"

#include "record_data.h"

#include "collision.h"

#include "world_gravity.h"

#include "object_initializers.h"

#include "camera.h"
#include "player.h"

#include "weapon_nodes.h"
#include "visual_object.h"
#include "enemy_list.h"
#include "record_factory.h"
#include "cheat_input.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "angle_math.h"
#include "object_damage.h"
#include "model_animation.h"
#include "model_attachment.h"
#include "game_random.h"
#include "game_diagnostics.h"
#include "object_force.h"
#include "level_session.h"
#include "fire_cycle.h"

#include "war.h"
#include "war_effects.h"
#include "ring_burst_effect.h"
#include "game_object_allocation.h"
#include "game_panel.h"
#include "projectile_tracking.h"
#include "game_heap.h"
#include "audio_session.h"
#include "cd_voice_event.h"

// Script tables retain their original shared module lifetime
struct WarSpawnPoints
{
    uint16 destinations[2][4];
    uint16 stage_records[6];
    uint16 path[8];
};

static WarSpawnPoints spawn_points = {0};
static const sint32 progression_thresholds[34] = {-6488, -10079, -16450, -17012, -20206, -26773, -30390, -38543, -39406, -62300, -64000, -66000, -68200, -70200, -72100, -74200, -76500, -78800, -80900, -82900, -85000, -87500, -89400, -91900, -94100, -96200, -97600, -99500, -101500, -103500, -105600, -107700, -110100, -99999999};
static const uint8 voice_choices[6] = {0, 6, 10, 11, 12, 13};

struct WarActivationWords
{
    uint16 initial_words[256];
    uint16 *words;
    uint32 capacity;

    WarActivationWords() : words(initial_words), capacity(256)
    {
        memset(initial_words, 0, sizeof(initial_words));
    }

    ~WarActivationWords()
    {
        if (words != initial_words)
            free(words);
    }

    void SetWord(uint32 index, uint16 value)
    {
        if (index >= capacity)
        {
            uint32 new_capacity = capacity;
            while (index >= new_capacity)
                new_capacity *= 2;
            uint16 *new_words = (uint16 *)calloc(new_capacity, sizeof(*new_words));
            if (!new_words)
            {
                GameDiagnostic("Cannot allocate War activation words\n");
                abort();
            }
            memcpy(new_words, words, capacity * sizeof(*words));
            if (words != initial_words)
                free(words);
            words = new_words;
            capacity = new_capacity;
        }
        words[index] = value;
    }
};

static WarActivationWords activation_words;

War::War(const RecordCursor &record_data, uint32 index) : Enemy(), jump_timer(0), last_voice(0), group_cursor(0), threshold_index(0)
{
    RecordDataBorrow record_borrow;
    uint32 roll;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8003D41C, "SLUS_003.73");
    aim_pos.words[0] = aim_pos.words[1] = aim_pos.words[2] = 0;

    cursor = ReadPosition(record_data);
    this->model.angles.words[0] = cursor.Read16();
    this->model.angles.words[1] = (cursor + 2).Read16();
    roll = (cursor + 4).Read16();
    script_cursor = cursor + 6;
    this->control.script_active = 1;
    this->record_idx = index;
    this->collision_radius = 0;
    this->model.angles.words[2] = roll;
    Configure();
    cursor = script_cursor;
    ExecuteScript(RecordCursor(cursor));
}

uint32 War::Configure()
{
    uint32 flags;
    FUNCTION_MARKER(0x8003D4B8, "SLUS_003.73");
    SetModelBank("war");
    motion.drag[0] = 31;
    motion.drag[1] = 3;
    motion.drag[2] = 31;
    AddToList(visual_lists.actors);
    StartAnimation(0, 1);
    AllocateBehaviorTable(3, 8);
    SetBehaviorValue(0, 0, 0);
    SetBehaviorValue(0, 1, 10);
    SetBehaviorValue(1, 0, 0);
    SetBehaviorValue(2, 0, 0);
    SetBehaviorValue(2, 1, 20);
    SetBehaviorValue(2, 2, 30);
    SetBehaviorValue(2, 3, 512);
    SetBehaviorValue(2, 4, 8);
    SetBehaviorValue(2, 5, 600);
    SetBehaviorValue(2, 6, 256);
    SetBehaviorValue(2, 7, 128);
    flags = this->lifecycle_flags;
    ai.work[4] = 450;
    ai.parameters[0] = 12;
    ai.parameters[1] = 32;
    contact_strength = 128;
    this->kind = 221;
    this->ground_offset = 216;
    this->health = (sint16)(uint16)(800);
    beam = 0;
    ai.state = 0;
    ai.phase = 0;
    this->control.hierarchy_ready = 0;
    camera_subject.SetObject(&model);
    this->lifecycle_flags = flags & 65533;
    return 1;
}

War::~War()
{
    if (beam)
        beam->Destroy(3);
    if (attack_beam)
        attack_beam->Destroy(3);
    for (uint32 index = 0; index < 5; ++index)
        if (death_beams[index])
            death_beams[index]->Destroy(3);
}

uint32 War::Destroy(uint32 reason)
{
    uint32 index;
    FUNCTION_MARKER(0x8003D66C, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    if (beam)
        beam->Destroy(3);
    beam = 0;
    if (attack_beam)
        attack_beam->Destroy(3);
    attack_beam = 0;
    Enemy::FreeBehaviorTable();
    ClearTrackedObjectHealth();
    camera_subject.Reset();
    for (index = 0; index < 5; ++index)
    {
        if (death_beams[index])
            death_beams[index]->Destroy(3);
        death_beams[index] = 0;
    }
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : 0;
}

static uint32 DivideWarMovement(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

uint32 War::Update()
{
    ModelBorrow model_borrow;
    RecordDataBorrow record_borrow;
    uint32 x, y, z, flags, value, second, third, scale, speed, distance, timer, result;
    Vector3 position, muzzle;
    ShortVector3 desired, bone_angles, weapon_angles;
    sint32 signed_value, absolute, high;
    FUNCTION_MARKER(0x8003D790, "SLUS_003.73");
    x = (uint32)zero_vector[0];
    y = (uint32)zero_vector[1];
    z = (uint32)zero_vector[2];
    motion.acceleration.words[0] = x;
    motion.acceleration.words[1] = y;
    motion.acceleration.words[2] = z;
    if (!(uint32)this->control.hierarchy_ready)
    {
        SetModelBoneTable(this->model.bank, WarAttachments);
        InitializeModelAnimation(WarHierarchy);
        value = this->model.flags;
        this->control.hierarchy_ready = 1;
        this->model.flags = value | 4;
    }
    flags = this->activation_flags;
    this->control.prev_activation_flags = flags;
    if (flags & 1)
    {
        value = (uint32)this->control.activation_count;
        this->activation_flags = flags & 0xFFFE;
        this->control.activation_count = value + 1;
    }
    flags = this->activation_flags;
    if (flags & 4)
    {
        this->activation_flags = flags & 0xFFFB;
        distance = MeasureHorizontalDistance(model.pos.words, explosion_queue.entries[0].pos.words);
        if (ai.state == 5 && (sint32)distance < 4096)
        {
            value = (4096 - distance) * 320;
            if ((sint32)value < 0)
                value += 4095;
            second = game_settings.difficulty;
            third = (uint32)((sint32)value >> 12);
            if (!second)
                third += (uint32)((sint32)value >> 13);
            Damage(third, zero_vector.components, 0);
            if (this->health > 0)
            {
                ai.state = 64;
                ai.phase = 0;
            }
        }
    }
    if (player_distance < 256 && !(this->model.flags & 1))
    {
        ModelInstance *contact_player = visual_lists.players;
        if (contact_player)
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
    {
        /* TODO Resolve the War AI undefined destination branch */
        UpdateState();
    }
    if ((uint32)this->control.gravity_enabled)
        AddVectors(motion.acceleration.words, world_gravity.vector.words);
    IntegrateMotion();
    x = this->model.pos.words[0];
    z = this->model.pos.words[2];
    flags = this->control.flags;
    surface.pos.words[0] = x;
    surface.pos.words[2] = z;
    if (flags & 16)
    {
        ModelInstance *target_model = tracked_target.GetObject();
        if (target_model)
        {
            aim_pos.words[0] = target_model->pos.words[0];
            aim_pos.words[1] = target_model->pos.words[1];
            aim_pos.words[2] = target_model->pos.words[2];
        }
        else
            control.flags &= ~16;
        CalculateDirectionAngles(desired.words, model.pos.words, aim_pos.words);
        desired.words[0] = 0;
        UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, 32);
        goto MovementAnimation;
    }
    if (flags & 32)
    {
        CalculateDirectionAngles(desired.words, model.pos.words, aim_pos.words);
        desired.words[0] = 0;
        UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, 32);
        goto MovementAnimation;
    }
    if (!(flags & 1))
        goto Footsteps;
    if (flags & 4)
    {
        if ((sint8)this->model.frame < 3)
        {
            this->playback.speed = 65536;
            this->control.gravity_enabled = 0;
            motion.velocity.words[2] = 0;
            motion.velocity.words[1] = 0;
            motion.velocity.words[0] = 0;
            motion.acceleration.words[2] = 0;
            motion.acceleration.words[1] = 0;
            motion.acceleration.words[0] = 0;
        }
        else if ((uint32)jump_timer)
            this->control.gravity_enabled = 1;
        if ((sint8)this->model.frame >= 3 && !(this->control.flags & 8))
        {
            x = this->destination.words[0];
            value = this->model.pos.words[0];
            x -= value;
            timer = (uint32)jump_timer;
            x = DivideWarMovement(x, timer);
            z = this->destination.words[2];
            value = this->model.pos.words[2];
            z -= value;
            timer = (uint32)jump_timer;
            z = DivideWarMovement(z, timer);
            y = this->destination.words[1];
            value = this->model.pos.words[1];
            y -= value;
            timer = (uint32)jump_timer;
            y = DivideWarMovement(y, timer);
            flags = this->control.flags;
            this->control.flags = flags | 8;
            motion.velocity.words[0] = x;
            motion.velocity.words[2] = z;
            motion.velocity.words[1] = y - (timer << 14);
        }
        else if ((sint8)this->model.frame >= 22 && (uint32)jump_timer)
            this->playback.speed = 0;
        timer = (uint32)jump_timer;
        if (timer)
        {
            jump_timer = timer - 1;
            if (!((timer - 1) & 0xFFFF))
            {
                x = this->destination.words[0];
                y = this->destination.words[1];
                z = this->destination.words[2];
                this->model.pos.words[0] = x;
                this->model.pos.words[1] = y;
                this->model.pos.words[2] = z;
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                motion.velocity.words[0] = 0;
                motion.acceleration.words[2] = 0;
                motion.acceleration.words[1] = 0;
                motion.acceleration.words[0] = 0;
                this->playback.speed = 65536;
                this->control.gravity_enabled = 0;
            }
        }
        if (this->playback.complete)
            this->control.flags = this->control.flags & 0xFFFFFFF2;
        goto MovementAnimation;
    }
    CalculateDirectionAngles(desired.words, this->model.pos.words, this->destination.words);
    value = desired.words[0];
    desired.words[0] = 0;
    scale = ai.parameters[1];
    UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, scale);
    desired.words[0] = (uint16)value;
    signed_value = (sint16)motion.angular_velocity.words[1];
    absolute = signed_value < 0 ? -signed_value : signed_value;
    if (absolute >= 65)
        absolute = 64;
    scale = ai.parameters[0];
    value = scale * (64 - (uint32)absolute);
    if ((sint32)value < 0)
        value += 63;
    speed = (uint32)((sint32)value >> 6);
    value = this->model.animation;
    if (value < 2)
    {
        second = (uint32)(sint32)(sint8)this->model.frame << 12;
        if (!value)
        {
            high = (sint32)(((sint64)(sint32)second * (sint32)0x82082083) >> 32);
            third = (uint32)((sint32)((uint32)high + second) >> 5);
        }
        else
        {
            high = (sint32)(((sint64)(sint32)second * 0x38E38E39) >> 32);
            third = (uint32)(high >> 3);
        }
        third -= (uint32)((sint32)second >> 31);
        signed_value = rsin(third & 4095);
        absolute = signed_value < 0 ? -signed_value : signed_value;
        speed = (scale >> 2) + (uint32)((sint32)(speed * (uint32)absolute) >> 12);
    }
    BuildDirectionVector(motion.velocity.words, speed, desired.words);
    position.words[0] = this->model.pos.words[0];
    position.words[1] = this->destination.words[1];
    position.words[2] = this->model.pos.words[2];
    distance = MeasureEuclideanDistance(position.words, this->destination.words);
    scale = ai.parameters[0];
    if (distance < scale * 2)
        this->control.flags = this->control.flags & ~1;
MovementAnimation:
    if ((this->control.flags & 1) && this->model.animation >= 2)
        StartAnimation(0, 1);
Footsteps:
    if (this->model.animation == 1)
    {
        value = DidAnimationCrossFrame(1, 7);
        if (UpdateEventLatch(value, 1))
        {
            PlaySoundEffect(128, 8192, 0);
            static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(2);
        }
        value = DidAnimationCrossFrame(1, 25);
        if (UpdateEventLatch(value, 2))
        {
            PlaySoundEffect(129, 8192, 0);
            static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(2);
        }
    }
    bone_angles.words[0] = 0;
    bone_angles.words[1] = 0;
    bone_angles.words[2] = 0;
    flags = this->control.flags;
    if (flags & 16)
    {
        if (visual_lists.players)
        {
            GetModelAttachment(position.words, *this, 2);
            CalculateDirectionAngles(bone_angles.words, position.words, static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos.words);
            bone_angles.words[1] = (uint16)((bone_angles.words[1] - this->model.angles.words[1]) & 4095);
        }
    }
    else if (flags & 32)
    {
        GetModelAttachment(position.words, *this, 2);
        CalculateDirectionAngles(bone_angles.words, position.words, aim_pos.words);
        bone_angles.words[1] = (uint16)((bone_angles.words[1] - this->model.angles.words[1]) & 4095);
    }
    UpdateAngleAcceleration(aim.angles, aim.velocity, aim.acceleration, bone_angles, 8);
    AddAngleVector(aim.angles.words, aim.velocity.words);
    NormalizeAngles(aim.angles.words);
    AddAngleVector(aim.velocity.words, aim.acceleration.words);
    DampAngles(aim.velocity.words, motion.angular_drag);
    SnapSmallAngles(aim.velocity.words);
    GetModelJointAngles(3).rotation.words[0] = aim.angles.words[0];
    GetModelJointAngles(3).rotation.words[1] = aim.angles.words[1];
    UpdateModelAnimationData();
    UpdateWorldTransform();
    BuildModelPose(*this);
    if (this->model.animation == 4 && ai.state == 16)
    {
        GetModelAttachment(position.words, *this, 0);
        GetModelAttachment(muzzle.words, *this, 1);
        CalculateDirectionAngles(weapon_angles.words, position.words, muzzle.words);
        if (!attack_beam)
        {
            GameHeapBlock *allocation = AllocateZeroedHeapBlock(104);
            ParticleBeam *created = 0;
            if (allocation)
            {
                value = (uint32)(sint32)(sint16)model.scale.words[0] * 75;
                created = new (allocation) ParticleBeam(&visual_lists.players, (uint32)((sint32)value >> 9), 1, 128, 127);
            }
            attack_beam = created;
            if (attack_beam)
            {
                attack_beam->SetColors(0, 64, 128, 185, 185, 185, 0, 64, 128, 255, 255, 255);
                signed_value = (sint16)GetGameTuning().player.maximum_health;
                value = ScaleDamageForDifficulty((uint32)(sint32)(sint16)(signed_value / 5));
                attack_beam->damage = value;
                attack_beam->Fire(position.words, weapon_angles.components);
            }
        }
        else
            attack_beam->Fire(position.words, weapon_angles.components);
    }
    else if (attack_beam)
    {
        attack_beam->Destroy(3);
        attack_beam = 0;
    }
    result = GetBehaviorValue(1, 0) & 0xFFFF;
    if (result)
    {
        if (!beam)
        {
            GameHeapBlock *allocation = AllocateZeroedHeapBlock(104);
            ParticleBeam *created = 0;
            if (allocation)
            {
                created = new (allocation) ParticleBeam(&visual_lists.players, 3000, 0, 256, 9);
            }
            beam = created;
            if (!beam)
                return 0;
            beam->damage = 1;
            beam->SetColors(0, 0, 64, 185, 185, 185, 0, 0, 64, 255, 255, 255);
        }
        GetModelAttachment(position.words, *this, 3);
        bone_angles.words[1] = (model.angles.words[1] + aim.angles.words[1]) & 4095;
        return beam->Fire(position.words, bone_angles.components);
    }
    if (beam)
    {
        result = beam->Destroy(3);
        beam = 0;
    }
    return result;
}

uint32 War::Damage(uint32 damage, const sint32 *, uint32 kind)
{
    uint32 count, stored_health;
    sint32 health, reduced;
    FUNCTION_MARKER(0x8003E354, "SLUS_003.73");
    if (ai.state < 2)
        return 0;
    health = this->health;
    if (health <= 0)
        return 1;
    count = (uint32)this->control.damage_phase;
    this->control.damage_phase = count + 1;
    reduced = (sint32)damage >> 1;
    if (kind == 30)
        reduced >>= 1;
    else if (kind == 5 || kind == 10)
        reduced >>= 2;
    health = this->health;
    stored_health = (uint16)this->health;
    if (reduced < health)
    {
        this->health = (sint16)(uint16)(stored_health - (uint32)reduced);
        SetColor(3, 255, 255, 255);
    }
    else
    {
        this->health = (sint16)(uint16)(0);
        ai.state = 9;
        ai.phase = 0;
    }
    return 1;
}

uint32 War::Resize(uint32 increment)
{
    uint32 flags, scale, height, displacement, y;
    FUNCTION_MARKER(0x8003E418, "SLUS_003.73");
    flags = this->model.flags;
    scale = (uint32)(sint32)(sint16)this->model.scale.words[0] + increment;
    this->model.flags = flags | 512;
    height = (uint32)((sint32)(scale * 216) >> 12);
    this->model.scale.words[2] = scale;
    this->model.scale.words[1] = scale;
    this->model.scale.words[0] = scale;
    displacement = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
    this->ground_offset = height;
    displacement = ((uint32)(sint32)(sint16)height - displacement) << 12;
    y = this->model.pos.words[1];
    this->model.pos.words[1] = y - displacement;
    return displacement;
}

// State-machine helpers are native extractions of original inline blocks
void War::AdvancePhase()
{
    uint32 phase = ai.phase;
    ai.phase = phase + 1;
}

void War::AdvanceState()
{
    uint32 state = ai.state;
    ai.phase = 0;
    ai.state = state + 1;
}

void War::RestoreState()
{
    uint32 state = ai.parameters[2];
    uint32 phase = ai.parameters[3];
    ai.state = state;
    ai.phase = phase;
}

void War::ClearVelocity()
{
    motion.velocity.words[2] = 0;
    motion.velocity.words[1] = 0;
    motion.velocity.words[0] = 0;
}

void War::SelectPoint(const uint16 *table, uint32 minimum, uint32 attempts, Vector3 &point)
{
    RecordDataBorrow record_borrow;
    uint32 index, attempt = 0, distance;
    do
    {
        ++attempt;
        index = RandomRange(4);
        index = table[index];
        ReadRecordPosition(point.words, index);
        distance = MeasurePositionDistance(model.pos.words, point.words);
        if (distance >= minimum)
            break;
        if (attempts)
        {
            uint32 retry = attempt < attempts;
            // Original retry delay slot increments the counter again
            ++attempt;
            if (!retry)
                break;
        }
    } while (1);
    SetDestination(point.words);
    AdvancePhase();
}

void War::ActivateGroup(uint32 maximum)
{
    ModelBorrow model_borrow;
    uint32 cursor, count, index, entry;
    ModelInstance *list;
    cursor = group_cursor;
    group_cursor = cursor + 1;
    count = activation_words.words[cursor];
    if (count >= maximum)
        return;
    for (index = 0; index < count; ++index)
    {
        cursor = group_cursor;
        list = visual_lists.solids;
        group_cursor = cursor + 1;
        entry = activation_words.words[cursor];
        ::ActivateObjectsByIndex(list, entry);
    }
}

static void SetWarCamera(Camera &camera, uint32 distance, uint32 pitch, uint32 yaw, uint32 x, uint32 y, uint32 time, uint32 field, uint32 set_field)
{
    SetCameraOrbitDistanceTransition(distance, time);
    SetCameraOrbitHeightTransition(pitch, time);
    SetCameraTargetLateralOffsetTransition(yaw, time);
    SetCameraTargetHeightOffsetTransition(x, time);
    SetCameraTargetForwardOffsetTransition(y, time);
    if (set_field)
        camera.SetYawTransition(field, time);
}

void War::FireDeathWeapons()
{
    Vector3 origin, endpoint;
    ShortVector3 angles;
    for (uint32 index = 0; index < 5; ++index)
    {
        if (!death_beams[index])
            continue;
        GetModelAttachment(origin.words, *this, 5 + index * 2);
        GetModelAttachment(endpoint.words, *this, 6 + index * 2);
        CalculateDirectionAngles(angles.words, origin.words, endpoint.words);
        death_beams[index]->Fire(origin.words, angles.components);
    }
}

void War::CreateDeathWeapon(uint32 index)
{
    GameHeapBlock *allocation = AllocateZeroedHeapBlock(104);
    ParticleBeam *created = 0;
    if (allocation)
    {
        created = new (allocation) ParticleBeam(&visual_lists.actors, 4096, 1, 0, 9);
    }
    death_beams[index] = created;
    if (!death_beams[index])
    {
        GameDiagnostic("War death beam allocation failed\n");
        abort();
    }
    death_beams[index]->SetColors(64, 64, 64, 185, 185, 185, 64, 64, 64, 255, 255, 255);
}

void War::UpdateState()
{
    ModelBorrow model_borrow;
    RecordDataBorrow record_borrow;
    uint32 state;
    uint32 phase;
    uint32 flags;
    uint32 value;
    uint32 second;
    uint32 index;
    uint32 count;
    uint32 cursor;
    GameHeapBlock *target;
    Camera *camera;
    ModelInstance *player;
    Vector3 point;
    sint32 height, timer, distance;
    FUNCTION_MARKER(0x8003E478, "SLUS_003.73");
    state = ai.state;
    if (state > 64)
        return;
    switch (state)
    {
        case 0:
            phase = ai.phase;
            if (phase)
                return;
            flags = this->model.flags;
            this->model.flags = flags | 1;
            value = FindGroundHeight(model.pos.words, 0, 4096);
            if (value == (uint32)-1)
                return;
            second = this->model.pos.words[1];
            height = (sint16)(uint32)this->ground_offset;
            if (((sint32)(value - second) >> 12) < height * 2)
            {
                second = value - ((uint32)height << 12);
                surface.pos.words[1] = second;
                this->model.pos.words[1] = second;
                shadow.height = (uint32)((sint32)(value - second) >> 12);
                ai.state = 1;
                this->control.gravity_enabled = 0;
                ClearVelocity();
                ai.work[1] = 0;
            }
            return;
        case 1:
            phase = ai.phase;
            if (!phase)
                SelectPoint(spawn_points.destinations[0], 257, 0, point);
            else if (phase == 1)
            {
                if ((uint32)this->control.activation_count)
                {
                    this->control.activation_count = 0;
                    ai.state = 2;
                    ai.phase = 0;
                }
                else if (!(this->control.flags & 1))
                    ai.phase = 0;
            }
            return;
        case 2:
            phase = ai.phase;
            if (!phase)
            {
                this->collision_radius = 128;
                flags = this->model.flags;
                ai.parameters[0] = 20;
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                this->model.flags = flags & 0xFFFE;
                camera->mode = 18;
                this->health = (sint16)(uint16)(1800);
                ::TrackObjectHealth(*this, 64, 32, 2048);
                AdvancePhase();
            }
            else if (phase == 1)
                SelectPoint(spawn_points.destinations[1], 201, 17, point);
            else if (phase == 2 && !(this->control.flags & 1))
            {
                player = visual_lists.actors;
                value = spawn_points.stage_records[5];
                ::ActivateObjectsByIndex(player, value);
                state = ai.state;
                ai.parameters[0] = 14;
                ai.phase = 0;
                ai.state = state + 1;
            }
            return;
        case 3:
            phase = ai.phase;
            if (!phase)
            {
                ai.work[1] = 0;
                ai.phase = phase + 1;
                phase = 1;
            }
            if (phase == 1)
                SelectPoint(spawn_points.destinations[1], 201, 17, point);
            else if (phase == 2)
            {
                timer = (sint16)ai.work[1];
                if (!timer)
                {
                    ai.work[1] = 90;
                    height = this->health;
                    SpawnProjectileWave(height < 1656 ? 2 : 1);
                }
                else
                    ai.work[1] = (uint32)timer - 1;
                if (this->health < 1512)
                    AdvanceState();
                else if (!(this->control.flags & 1))
                {
                    phase = ai.phase;
                    ai.phase = phase - 1;
                }
                else if (player_distance < 512 && !RandomRange(2))
                {
                    state = ai.state;
                    ai.parameters[3] = 1;
                    ai.parameters[4] = 1512;
                    ai.state = 16;
                    ai.phase = 0;
                    ai.parameters[2] = state;
                }
            }
            return;
        case 4:
            phase = ai.phase;
            switch (phase)
            {
                case 0:
                    value = spawn_points.stage_records[0];
                    ReadRecordPosition(point.words, value);
                    SetDestination(point.words);
                    AdvancePhase();
                    return;
                case 1:
                    if (!(this->control.flags & 1))
                    {
                        state = ai.state;
                        ai.parameters[3] = 2;
                        ai.state = 32;
                        ai.phase = 0;
                        ai.parameters[2] = state;
                    }
                    return;
                case 2:
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    player = visual_lists.players;
                    camera->mode = 3;
                    camera->follow.flags = 1;
                    camera->follow.target.SetObject(player);
                    camera->aim.flags = 1;
                    camera->aim.target.SetObject(player);
                    SetWarCamera(*camera, 2276, (uint32)-520, 0, (uint32)-1590, (uint32)-3280, 30, 2048, 1);
                    SetAnimationRange(2, 0, (uint32)-1);
                    AdvancePhase();
                    return;
                case 3:
                    if (this->playback.complete)
                    {
                        SetAnimationRange(3, 0, (uint32)-1);
                        AdvancePhase();
                    }
                    return;
                case 4:
                    if (this->playback.complete)
                        AdvancePhase();
                    else
                        Resize(178);
                    return;
                case 5:
                    SetAnimationRange(7, 0, (uint32)-1);
                    AdvancePhase();
                    return;
                case 6:
                    if ((sint8)this->model.frame >= 25)
                    {
                        PlayPositionedSound(131, model.pos.words, 0);
                        for (index = 0; index < 2; ++index)
                        {
                            value = spawn_points.stage_records[1 + index];
                            player = visual_lists.solids;
                            ::ActivateObjectsByIndex(player, value);
                        }
                        AdvancePhase();
                    }
                    return;
                case 7:
                    if (this->playback.complete)
                    {
                        value = (uint32)(sint32)(sint8)this->model.frame;
                        SetAnimationRange(7, value, 0);
                        AdvancePhase();
                    }
                    return;
                case 8:
                    if (this->playback.complete)
                    {
                        if (GetCheatOptions().war_record_spawn)
                        {
                            value = spawn_points.stage_records[4];
                            ::SpawnRecord(value);
                            model_effect.color_mode = 0x06403030;
                            model_effect.scale = 8192;
                            model_effect.depth_bias = 10;
                        }
                        SetAnimationRange(4, 0, (uint32)-1);
                        AdvancePhase();
                    }
                    return;
                case 9:
                    if (this->playback.complete)
                    {
                        phase = ai.phase;
                        ai.work[1] = 0;
                        ai.phase = phase + 1;
                    }
                    return;
                case 10:
                    timer = (sint16)ai.work[1];
                    if (timer < 60)
                        ai.work[1] = (uint32)timer + 1;
                    else
                    {
                        ai.work[1] = 0;
                        AdvanceState();
                    }
                    return;
                default:
                    return;
            }
        case 5:
            player = visual_lists.players;
            value = this->model.pos.words[2];
            second = player->pos.words[2];
            if ((sint32)value < (sint32)second)
            {
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                ai.state = 48;
                flags = this->control.flags;
                motion.velocity.words[0] = 0;
                ai.phase = 0;
                this->control.flags = flags & ~1;
                return;
            }
            if ((sint32)second < (sint32)0xF13AC000)
            {
                flags = this->control.flags;
                if (!(flags & 64))
                {
                    this->control.flags = flags | 64;
                    motion.velocity.words[2] = 0;
                    motion.velocity.words[1] = 0;
                    flags = this->control.flags;
                    state = ai.state;
                    motion.velocity.words[0] = 0;
                    ai.phase = 0;
                    this->control.flags = flags & ~1;
                    ai.state = state + 1;
                    return;
                }
            }
            index = threshold_index;
            value = progression_thresholds[index];
            second = this->model.pos.words[2];
            if ((sint32)second < (sint32)(value << 12))
            {
                cursor = group_cursor;
                threshold_index = index + 1;
                group_cursor = cursor + 1;
                count = activation_words.words[cursor];
                for (index = 0; index < count; ++index)
                {
                    cursor = group_cursor;
                    player = visual_lists.solids;
                    group_cursor = cursor + 1;
                    value = activation_words.words[cursor];
                    ::ActivateObjectsByIndex(player, value);
                }
            }
            if (this->model.animation != 1)
                StartAnimation(1, 1);
            phase = ai.phase;
            if (phase == 1)
            {
                SetDestination(this->destination.words);
                phase = ai.phase;
                ai.phase = phase - 1;
            }
            else if (!phase)
            {
                player = visual_lists.players;
                value = this->model.pos.words[2];
                second = player->pos.words[2];
                distance = (sint32)(value - second);
                if (distance <= 10649599)
                {
                    value = this->control.flags & 64;
                    second = ai.parameters[0];
                    if (second > (value ? 40 : 60))
                        ai.parameters[0] = second - 2;
                }
                else if (distance > 11468800)
                {
                    value = this->control.flags & 64;
                    second = ai.parameters[0];
                    if (second < (value ? 80 : 128))
                        ai.parameters[0] = second + 1;
                }
                if (!(this->control.flags & 1))
                {
                    index = ai.work[1];
                    timer = (sint16)ai.work[1];
                    if (index >= 8)
                    {
                        /* TODO Original local XYZ is uninitialized; native recovery awaits human contract */
                        GameDiagnostic("TODO War AI state5 exhausted path vector\n");
                        abort();
                    }
                    value = spawn_points.path[index];
                    ReadRecordPosition(point.words, value);
                    SetDestination(point.words);
                    ai.work[1] = (uint32)timer + 1;
                }
            }
            return;
        case 6:
            phase = ai.phase;
            if (!phase)
            {
                player = visual_lists.players;
                static_cast<Player *>(player->owner)->disable_timer = 240;
                SetAnimationRange(7, 0, (uint32)-1);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                if ((sint8)this->model.frame >= 24)
                {
                    player = visual_lists.actors;
                    value = spawn_points.stage_records[3];
                    ::ActivateObjectsByIndex(player, value);
                    phase = ai.phase;
                    ai.work[0] = 0;
                    ai.phase = phase + 1;
                }
            }
            else if (phase == 2 && this->playback.complete)
            {
                value = ai.work[0];
                timer = (sint16)ai.work[0];
                if (value < 150)
                    ai.work[0] = (uint32)timer + 1;
                else
                {
                    value = (uint32)(sint32)(sint8)this->model.frame;
                    SetAnimationRange(7, value, 0);
                    AdvancePhase();
                }
            }
            else if (phase == 3 && this->playback.complete)
            {
                flags = this->model.flags;
                this->model.flags = flags | 8;
                SetDestination(this->destination.words);
                state = ai.state;
                ai.phase = 0;
                ai.state = state - 1;
            }
            return;
        case 9:
            phase = ai.phase;
            if (!phase)
            {
                if (this->control.flags & 64)
                    ActivateGroup(4);
                static_cast<Player *>(visual_lists.players->owner)->SetWeaponStop(900);
                PlayCdVoiceEvent(18, 4, model.pos.words, 0);
                this->playback.speed = 32768;
                ::ClearTrackedObjectHealth();
                flags = this->control.flags;
                this->control.flags = flags & ~1;
                ClearVelocity();
                ClearFacing();
                SetAnimationRange(8, 0, (uint32)-1);
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                camera->aim.flags = 1;
                camera->aim.target.SetObject(&model);
                camera->follow.flags = 1;
                camera->follow.target.SetObject(&model);
                SetWarCamera(*camera, 2276, (uint32)-1872, 64, (uint32)-30, (uint32)-714, 30, 2048, 1);
                phase = ai.phase;
                ai.work[0] = 0;
                ai.phase = phase + 1;
            }
            else if (phase == 1)
            {
                if (!RandomRange(3))
                {
                    point = this->model.pos;
                    point.words[1] += 1638400;
                    point.words[2] -= 1048576;
                    value = RandomRange(512);
                    point.words[0] += (value - 256) << 12;
                    value = RandomRange(512);
                    point.words[1] += (value - 256) << 12;
                    value = RandomRange(512);
                    point.words[2] += (value - 256) << 12;
                    SpawnFireCycle(point.words, 60, 1, 2);
                    value = RandomRange(2);
                    PlayPositionedSound(value + 1, model.pos.words, 2);
                }
                if ((sint8)this->model.frame >= 12)
                    this->model.frame = 13;
                timer = (sint16)ai.work[0];
                ai.work[0] = (uint32)timer + 1;
                if (timer == 42)
                    CreateDeathWeapon(0);
                else if (timer == 58)
                    CreateDeathWeapon(1);
                else if (timer == 68)
                    CreateDeathWeapon(2);
                else if (timer == 73)
                    CreateDeathWeapon(3);
                else if (timer == 89)
                    CreateDeathWeapon(4);
                else if (timer == 90)
                {
                    camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                    SetWarCamera(*camera, 2252, (uint32)-552, 60, 522, 268, 40, 0, 0);
                }
                else if (timer >= 141)
                    AdvancePhase();
                FireDeathWeapons();
            }
            else if (phase == 2)
            {
                FireDeathWeapons();
                if ((sint8)this->model.frame >= 28)
                {
                    for (index = 0; index < 5; ++index)
                    {
                        if (death_beams[index])
                        {
                            death_beams[index]->Destroy(3);
                            death_beams[index] = 0;
                        }
                    }
                }
                if ((sint8)this->model.frame >= 33)
                {
                    static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(0);
                    PlayPositionedSound(130, model.pos.words, 0);
                    phase = ai.phase;
                    ai.work[0] = 0;
                    ai.phase = phase + 1;
                }
            }
            else if (phase == 3 && this->playback.complete)
            {
                value = ai.work[0];
                timer = (sint16)ai.work[0];
                if (value < 17)
                    ai.work[0] = (uint32)timer + 1;
                else
                    CompleteLevel(0);
            }
            return;
        case 16:
            phase = ai.phase;
            if (!phase)
            {
                flags = this->control.flags;
                ModelInstance *target_model = visual_lists.players;
                this->control.flags = flags & ~1;
                ClearVelocity();
                TrackTarget(target_model);
                SetAnimationRange(4, 0, (uint32)-1);
                AdvancePhase();
            }
            else if (phase == 1 && this->playback.complete)
            {
                if (player_distance < 512)
                {
                    height = this->health;
                    value = ai.parameters[4];
                    if ((sint32)value < height)
                    {
                        SetAnimationRange(4, 0, (uint32)-1);
                        return;
                    }
                }
                SetAnimationRange(6, 0, (uint32)-1);
                ClearFacing();
                AdvancePhase();
            }
            else if (phase == 2 && this->playback.complete)
                RestoreState();
            return;
        case 32:
            phase = ai.phase;
            if (!phase)
            {
                flags = this->control.flags;
                ModelInstance *target_model = visual_lists.players;
                this->control.flags = flags & ~1;
                ClearVelocity();
                TrackTarget(target_model);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                StartAnimation(0, 1);
                SetBehaviorValue(1, 0, 1);
                phase = ai.phase;
                ai.work[0] = 180;
                ai.phase = phase + 1;
            }
            else if (phase == 2)
            {
                if (visual_lists.players)
                {
                    timer = (sint16)ai.work[0];
                    if (timer)
                    {
                        ai.work[0] = (uint32)timer - 1;
                        return;
                    }
                }
                ai.phase = 3;
            }
            else if (phase == 3)
            {
                SetBehaviorValue(1, 0, 0);
                ClearFacing();
                RestoreState();
            }
            return;
        case 48:
            phase = ai.phase;
            if (!phase)
            {
                player = visual_lists.players;
                static_cast<Player *>(player->owner)->disable_timer = 900;
                SetAnimationRange(7, 0, (uint32)-1);
                if (this->control.flags & 64)
                    ActivateGroup(4);
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                camera->aim.flags = 1;
                camera->aim.target.SetObject(&model);
                camera->follow.flags = 1;
                camera->follow.target.SetObject(&model);
                SetWarCamera(*camera, 832, (uint32)-568, 208, (uint32)-624, (uint32)-1104, 16, 1736, 1);
                AdvancePhase();
            }
            else if (phase == 1 && (sint8)this->model.frame >= 25)
            {
                PlayPositionedSound(131, model.pos.words, 0);
                GetModelAttachment(point.words, *this, 5);
                target = AllocateEffectStorage(84);
                if (target)
                {
                    new (target) WarImpact(point.words);
                }
                player = visual_lists.players;
                height = static_cast<Player *>(player->owner)->health;
                static_cast<Player *>(player->owner)->disable_timer = 0;
                static_cast<Player *>(player->owner)->death_camera_enabled = 0;
                static_cast<Player *>(player->owner)->Damage((uint32)height << 2, zero_vector.components, 0);
                player = visual_lists.players;
                static_cast<Player *>(player->owner)->disable_timer = 900;
                AdvancePhase();
            }
            return;
        case 64:
            phase = ai.phase;
            if (!phase)
            {
                flags = this->control.flags;
                motion.velocity.words[2] = 0;
                this->control.flags = flags & ~1;
                SetAnimationRange(9, 0, (uint32)-1);
                ai.phase = 3;
            }
            else if (phase == 2 && this->playback.complete)
            {
                value = this->lifecycle_flags;
                this->lifecycle_flags = value | 16;
                StartAnimation(1, 1);
                ai.state = 5;
                ai.phase = 1;
            }
            else if (phase == 3 && (sint8)this->model.frame >= 20)
            {
                value = this->lifecycle_flags;
                this->lifecycle_flags = value & 0xFFEF;
                ClearVelocity();
                PlaySoundEffect(130, 8192, 0);
                phase = ai.phase;
                ai.work[4] = 0;
                ai.phase = phase + 1;
            }
            else if (phase == 4 && this->playback.complete)
            {
                timer = (sint16)ai.work[4];
                value = (uint32)timer + 1;
                if ((value & 0xFFFF) < 9)
                    ai.work[4] = value;
                else
                {
                    do
                    {
                        index = RandomRange(6);
                        value = last_voice;
                    } while ((index & 255) == value);
                    value = voice_choices[index & 255];
                    PlayCdVoiceEvent(18, value, model.pos.words, 0);
                    last_voice = index;
                    SetAnimationRange(10, 0, (uint32)-1);
                    ai.phase = 2;
                }
            }
            return;
        default:
            return;
    }
}

uint32 War::ReadParameters(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value, index, group, count, word_index;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8003FCEC, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x2130)
        return Enemy::ReadParameters(command);
    cursor = script_cursor;
    script_cursor = cursor + 2;
    for (index = 0; index < 4; ++index)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        spawn_points.destinations[0][index] = value;
        script_cursor = cursor + 2;
    }
    for (index = 0; index < 4; ++index)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        cursor += 2;
        spawn_points.destinations[1][index] = value;
        script_cursor = cursor;
    }
    value = cursor.Read16();
    script_cursor = cursor + 10;
    spawn_points.stage_records[0] = value;
    for (index = 0; index < 2; ++index)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        spawn_points.stage_records[1 + index] = value;
        script_cursor = cursor + 2;
    }
    for (index = 0; index < 8; ++index)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        spawn_points.path[index] = value;
        script_cursor = cursor + 2;
    }
    word_index = 0;
    for (group = 0; group < 9; ++group)
    {
        cursor = script_cursor;
        script_cursor = cursor + 2;
        count = (cursor + 2).Read16();
        script_cursor = cursor + 4;
        activation_words.SetWord(word_index++, count);
        if (count)
        {
            for (index = 0; index < count; ++index)
            {
                cursor = script_cursor;
                value = cursor.Read16();
                activation_words.SetWord(word_index, value);
                ++word_index;
                script_cursor = cursor + 2;
            }
        }
    }
    cursor = script_cursor;
    value = cursor.Read16();
    script_cursor = cursor + 2;
    spawn_points.stage_records[3] = value;
    for (group = 0; group < 24; ++group)
    {
        activation_words.SetWord(word_index++, 1);
        cursor = script_cursor;
        value = cursor.Read16();
        activation_words.SetWord(word_index, value);
        ++word_index;
        script_cursor = cursor + 2;
    }
    cursor = script_cursor;
    value = cursor.Read16();
    cursor += 2;
    script_cursor = cursor;
    spawn_points.stage_records[4] = value;
    value = cursor.Read16();
    cursor += 2;
    script_cursor = cursor;
    spawn_points.stage_records[5] = value;
    return 0;
}

uint32 War::ExecuteCommand(uint32 command)
{
    FUNCTION_MARKER(0x8003FF2C, "SLUS_003.73");
    return Enemy::ExecuteCommand(command & 0xFFFF);
}

uint32 War::SetDestination(const uint32 *position)
{
    uint32 x, y, z, flags;
    FUNCTION_MARKER(0x8003FF4C, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    this->destination.words[0] = x;
    this->destination.words[1] = y;
    this->destination.words[2] = z;
    uint32 floor;
    floor = FindGroundHeight(position, 1024, 4096);
    if (floor != (uint32)-1)
        y = floor - ((uint32)(sint32)(sint16)(uint32)this->ground_offset << 12);
    else
        y = this->model.pos.words[1];
    this->destination.words[1] = y;
    flags = (this->control.flags | 1) & 0xFFFFFFF3;
    this->control.flags = flags;
    return flags;
}

uint32 War::TrackTarget(ModelInstance *target)
{
    uint32 result, flags;
    FUNCTION_MARKER(0x8003FFD8, "SLUS_003.73");
    result = static_cast<VisualObject *>(target->owner)->IsFinished();
    if (!result)
    {
        flags = this->control.flags;
        tracked_target.SetObject(target);
        result = (flags & 0xFFFFFFDF) | 16;
        this->control.flags = result;
    }
    return result;
}

uint16 *War::ClearFacing()
{
    uint32 flags;
    FUNCTION_MARKER(0x80040058, "SLUS_003.73");
    flags = this->control.flags;
    this->control.flags = flags & 0xFFFFFFCF;
    motion.angular_velocity.words[2] = 0;
    motion.angular_velocity.words[1] = 0;
    motion.angular_velocity.words[0] = 0;
    motion.angular_acceleration.words[2] = 0;
    motion.angular_acceleration.words[1] = 0;
    motion.angular_acceleration.words[0] = 0;
    return motion.angular_acceleration.words;
}

uint32 War::SpawnProjectileWave(uint32 count)
{
    ModelInstance *player;
    GameHeapBlock *allocation;
    uint32 damage;
    uint32 index;
    uint32 turn = 1024;
    Vector3 origin, target, offset;
    ShortVector3 angles, direction;
    FUNCTION_MARKER(0x80040094, "SLUS_003.73");
    player = visual_lists.players;
    if (!player)
        return 0;
    GetModelAttachment(origin.words, *this, 4);
    SpawnExplosionRing(origin.words, 64, 64, 64, 160, 5, 0, 100);
    for (index = 0; (sint32)index < (sint32)count; ++index)
    {
        player = visual_lists.players;
        angles.components[0] = -1024;
        angles.components[1] = 0;
        angles.components[2] = 0;
        target.words[0] = static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos.words[0];
        target.words[1] = static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos.words[1];
        target.words[2] = static_cast<VisualObject *>(visual_lists.players->owner)->surface.pos.words[2];
        target.words[1] += 262144;
        if (count == 2)
        {
            player = visual_lists.players;
            CalculateDirectionAngles(direction.words, model.pos.words, player->pos.words);
            direction.components[0] = 0;
            direction.components[1] = (sint16)(((uint32)(uint16)direction.components[1] + turn) & 4095);
            BuildDirectionVector(offset.words, 180, direction.words);
            target.words[0] += offset.words[0];
            target.words[2] += offset.words[2];
            angles.components[1] = index ? 512 : -512;
        }
        /* Original computed target is not passed to the projectile constructor */
        (void)target;
        allocation = AllocateZeroedObject(420);
        if (allocation)
        {
            damage = ScaleDamageForDifficulty(10);
            TrackingProjectile *projectile = new (allocation) TrackingProjectile(origin.words, angles.components, 1024, damage, &visual_lists.players);
            projectile->SetTrailColor(8, 0, 64, 128);
            projectile->SetSpeed(80);
        }
        turn += 2048;
    }
    return (uint32)-1024;
}

// Native callback adapter discards the original parameter reader result
void War::SetParameters(uint32 command)
{
    ReadParameters(command);
}

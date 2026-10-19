#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_heap.h"
#include "level_session.h"
#include "game_tick.h"

#include "record_data.h"

#include "collision.h"

#include "world_gravity.h"

#include "object_initializers.h"

#include "visual_object.h"
#include "object_damage.h"
#include "model_animation.h"
#include "game_diagnostics.h"
#include "player.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"

#include "mini_tank.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include "model_attachment.h"
#include "angle_math.h"
#include "projectile_oscillating.h"
#include "game_random.h"
#include "vector_distance.h"
#include "sprite_trail.h"
#include "audio_session.h"

MiniTank::MiniTank(const RecordCursor &record_data, uint32 index) : Enemy(), trail(0), state_timer(0), engine_sound(0), sound_period(0), bounce_speed(0), age(0)
{
    RecordDataBorrow record_borrow;
    uint32 angle;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8007488C, "SLUS_003.73");
    target_angles.words[0] = target_angles.words[1] = target_angles.words[2] = 0;

    cursor = ReadPosition(record_data);
    angle = cursor.Read16();
    this->model.angles.words[0] = angle;
    angle = (cursor + 2).Read16();
    this->model.angles.words[1] = angle;
    angle = (cursor + 4).Read16();
    script_cursor = cursor + 6;
    this->record_idx = index;
    this->model.angles.words[2] = angle;
    Init();
    cursor = script_cursor;
    if (cursor)
    {
        this->control.script_active = 1;
        ExecuteScript(RecordCursor(cursor));
    }
}

MiniTank::~MiniTank()
{
    if (engine_sound)
        StopSoundVoices(engine_sound);
}

uint32 MiniTank::Destroy(uint32 reason)
{
    uint32 sound;
    FUNCTION_MARKER(0x800749C4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    sound = engine_sound;
    if (sound)
        StopSoundVoices(sound);
    engine_sound = 0;
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : 0;
}

uint32 MiniTank::Init()
{
    uint32 total, floor, sound, height;
    FUNCTION_MARKER(0x80074A58, "SLUS_003.73");
    total = game_progress.level.enemy_total;
    target_angles.words[0] = model.angles.words[0];
    target_angles.words[1] = model.angles.words[1];
    target_angles.words[2] = model.angles.words[2];
    game_progress.level.enemy_total = total + 1u;
    SetModelBank("MiniTank");
    AddToList(visual_lists.actors);
    this->kind = 207;
    this->ground_offset = 64u;
    AllocateBehaviorTable(1, 4);
    ai.parameters[0] = 50;
    ai.parameters[1] = 128;
    this->health = (sint16)(uint16)(20u);
    SetBehaviorValue(0, 1, 10);
    SetBehaviorValue(0, 2, 140);
    SetBehaviorValue(0, 3, 50);
    floor = FindGroundHeight(model.pos.words, 512, 512);
    if (floor != 0xFFFFFFFF)
    {
        height = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
        this->model.pos.words[1] = floor - (height << 12);
    }
    sound_period = 0xFFFFFFFF;
    sound = PlayPositionedSound(89, model.pos.words, 0);
    engine_sound = sound;
    if (sound)
        sound_period = 16u;
    this->control.script_active = 1;
    return 1;
}

uint32 MiniTank::Divide(uint32 numerator, uint32 denominator)
{
    sint32 left = (sint32)numerator, right = (sint32)denominator;
    if (!right)
        return left < 0 ? 1u : 0xFFFFFFFF;
    if (left == (-2147483647 - 1) && right == -1)
        return numerator;
    return (uint32)(left / right);
}

uint32 MiniTank::Remainder(uint32 numerator, uint32 denominator)
{
    sint32 left = (sint32)numerator, right = (sint32)denominator;
    if (!right)
        return numerator;
    if (left == (-2147483647 - 1) && right == -1)
        return 0;
    return (uint32)(left % right);
}

uint32 MiniTank::Update()
{
    RecordDataBorrow record_borrow;
    uint32 value, flags, count, radius, factor, damage;
    uint32 index, tick, scale, period, frame, sound;
    Vector3 constants;
    sint32 distance;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80074BA8, "SLUS_003.73");
    value = age;
    age = value + 1u;
    constants = zero_vector;
    motion.acceleration = constants;
    if ((uint32)this->control.script_active)
    {
        value = (uint32)this->control.script_delay;
        if (value)
            this->control.script_delay = value - 1u;
        else
            ExecuteScript(script_cursor);
    }
    else
        UpdateState();
    if (!(uint32)this->control.hierarchy_ready)
    {
        SetModelBoneTable(this->model.bank, MiniTankAttachments);
        InitializeModelAnimation(MiniTankHierarchy);
        flags = this->model.flags;
        this->control.hierarchy_ready = 1;
        this->model.flags = flags | 4u;
    }
    flags = this->activation_flags;
    if (flags & 1u)
    {
        tick = (uint32)this->control.activation_count;
        flags = this->activation_flags;
        this->control.activation_count = tick + 1u;
        this->activation_flags = flags & 0xFFFEu;
    }
    flags = this->activation_flags;
    if (flags & 4u)
    {
        index = 0;
        for (;;)
        {
            count = explosion_queue.count;
            if ((sint32)index >= (sint32)count)
                break;
            const ExplosionEvent *event = &explosion_queue.entries[index];
            distance = (sint32)MeasurePositionDistance(model.pos.words, event->pos.words);
            radius = event->radius;
            if (distance < (sint32)radius)
            {
                value = radius - (uint32)distance;
                factor = event->strength;
                damage = Divide(factor * value, radius);
                Damage(damage, zero_vector.components, 0);
                if (this->health <= 0)
                    break;
            }
            ++index;
        }
    }
    else if (this->control.flags & 1u)
    {
        value = MeasurePositionDistance(this->model.pos.words, this->destination.words);
        if (value < 129u)
        {
            flags = this->control.flags;
            this->control.flags = flags & 0xFFFFFFFE;
        }
        else
        {
            CalculateDirectionAngles(angles.words, this->model.pos.words, this->destination.words);
            value = this->model.angles.words[1];
            value = ((value - (uint16)angles.components[1]) & 4095u) - 64u;
            if (value < 0xF81u)
            {
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                motion.velocity.words[0] = 0;
                UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, angles, 32u);
            }
            else
            {
                model.angles.words[0] = target_angles.words[0];
                model.angles.words[1] = target_angles.words[1];
                model.angles.words[2] = target_angles.words[2];
                motion.angular_velocity.words[2] = 0;
                motion.angular_velocity.words[1] = 0;
                motion.angular_velocity.words[0] = 0;
                motion.angular_acceleration.words[2] = 0;
                motion.angular_acceleration.words[1] = 0;
                scale = ai.parameters[0];
                motion.angular_acceleration.words[0] = 0;
                BuildDirectionVector(motion.velocity.words, scale, angles.words);
            }
        }
    }
    Aim();
    UpdateModelAnimationData();
    UpdateWorldTransform();
    BuildModelPose(*this);
    if ((uint32)this->control.gravity_enabled)
        AddVectors(motion.acceleration.words, world_gravity.vector.words);
    IntegrateMotion();
    period = sound_period;
    if (period != 0xFFFFFFFF)
    {
        frame = game_clock.tick;
        if (!Remainder(frame, period))
        {
            sound = engine_sound;
            UpdatePositionedSoundVoices(sound, model.pos.words, 0);
        }
    }
    // The legacy behavior caller discards the original return carrier
    return 0;
}

uint32 MiniTank::Aim()
{
    uint32 angle, yaw;
    Vector3 position;
    ShortVector3 rotation;
    FUNCTION_MARKER(0x80074F3C, "SLUS_003.73");
    GetModelAttachment(position.words, *this, 1);
    CalculateDirectionAngles(rotation.words, position.words, visual_lists.players->pos.words);
    angle = rotation.words[1];
    yaw = this->model.angles.words[1];
    angle = (angle - yaw) & 4095;
    GetModelJointAngles(2).rotation.words[1] = (uint16)angle;
    return angle;
}

MiniTankRing::MiniTankRing(const uint32 acceleration[3]) : MeshRingEffect(12, 0xCCE0187C), age(0), lifetime(0), radius_step(0), width_step(0)
{
    uint32 x = acceleration[0], y = acceleration[1], z = acceleration[2];
    // Original constructor is inlined in MiniTank state and damage handlers
    this->velocity.words[0] = x;
    this->velocity.words[1] = y;
    this->velocity.words[2] = z;
}

uint32 MiniTankRing::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80075F38, "SLUS_003.73");
    result = MeshRingEffect::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(this);
    return result;
}

void MiniTankRing::Update()
{
    FUNCTION_MARKER(0x80075E7C, "SLUS_003.73");
    ++age;
    if (lifetime < (sint16)age)
    {
        Finish();
        return;
    }
    AddVectors(pos.words, velocity.words);
    SetGeometry(pos.words, radius + (uint32)(sint32)radius_step, expansion + (uint32)(sint32)width_step);
    uint32 value = lifetime - (uint32)(sint32)(sint16)age;
    value = (value << 8) - value;
    uint32 color = lifetime ? (uint32)((sint32)value / (sint32)lifetime) : ((sint32)value < 0 ? 1 : (uint32)-1);
    SetColors(color, color, (sint32)color >> 1);
}

void MiniTank::Burst(uint32 green)
{
    GameHeapBlock *child;
    Vector3 acceleration;
    HandleExplosionProximity(model.pos.words, 0);
    SpawnExplosionRing(model.pos.words, 200, 240, green, 0, 5, 0, 100);
    child = AllocateEffectStorage(120);
    if (child)
    {
        acceleration = zero_vector;
        MiniTankRing *ring = new (child) MiniTankRing(acceleration.words);
        ring->SetGeometry(model.pos.words, 0, 0);
        ring->lifetime = 32;
        ring->radius_step = 32;
        ring->width_step = 16;
    }
    acceleration.words[0] = 0;
    acceleration.words[1] = 0xFFFEC000;
    acceleration.words[2] = 0;
    child = AllocateEffectStorage(120);
    if (child)
    {
        MiniTankRing *ring = new (child) MiniTankRing(acceleration.words);
        ring->SetGeometry(model.pos.words, 0, 0);
        ring->lifetime = 32;
        ring->radius_step = 32;
        ring->width_step = 16;
    }
}

void MiniTank::UpdateState()
{
    ModelInstance *player;
    uint32 state;
    uint32 phase;
    uint32 flags;
    uint32 count;
    uint32 parameter;
    uint32 random;
    uint32 value;
    GameHeapBlock *child;
    uint32 damage, x, y, z, dx, dy, dz, height;
    Vector3 candidate, position;
    sint32 distance, floor;
    FUNCTION_MARKER(0x80074FA8, "SLUS_003.73");
    state = ai.state;
    if (!state)
    {
        phase = ai.phase;
        switch (phase)
        {
            case 0:
                flags = this->control.flags;
                if (!(flags & 1u))
                {
                    player = visual_lists.players;
                    value = ChoosePosition(candidate.words, visual_lists.players->pos.words, 128u, 512u, 64u, 1024u, 1u, 1u);
                    if (value)
                        SetTarget(candidate.words);
                }
                player = visual_lists.players;
                distance = (sint32)MeasurePositionDistance(this->model.pos.words, player->pos.words);
                if (distance < 204)
                {
                    phase = ai.phase;
                    ai.phase = phase + 1u;
                    return;
                }
                count = state_timer;
                state_timer = count + 1u;
                parameter = GetBehaviorValue(0, 3);
                random = RandomRange(5);
                parameter = (parameter & 0xFFFFu) + random;
                count = state_timer;
                if (count < parameter)
                    return;
                position.words[0] = this->model.pos.words[0];
                player = visual_lists.players;
                position.words[1] = this->model.pos.words[1];
                position.words[2] = this->model.pos.words[2];
                if (IsSegmentVisible(position.words, player->pos.words))
                {
                    ai.phase = 3;
                    flags = this->control.flags;
                    state_timer = 0u;
                    this->control.flags = flags & 0xFFFFFFFE;
                }
                return;
            case 1:
                phase = ai.phase;
                ai.phase = phase + 1u;
                return;
            case 2:
                value = this->model.angles.words[1];
                count = age & 3u;
                this->model.angles.words[1] = value + 128u;
                if (!count)
                    SetColor(3, 255, 255, 255);
                else if (count == 2u)
                    SetColor(3, 255, 0, 0);
                if (RandomRange(16))
                    return;
                player = visual_lists.players;
                distance = (sint32)MeasurePositionDistance(this->model.pos.words, player->pos.words);
                if (distance < 1024)
                {
                    damage = (uint32)((sint32)(1024u - (uint32)distance) >> 6);
                    VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
                    damage_target->Damage(damage, zero_vector.components, 0);
                }
                flags = this->lifecycle_flags;
                this->lifecycle_flags = flags & 0xFFEFu;
                Burst(200u);
                Finish();
                return;
            case 3:
                value = state_timer + 1u;
                state_timer = value;
                if (value >= 5u)
                {
                    ai.phase = 4;
                    state_timer = 0u;
                }
                return;
            case 4:
                if (!RandomRange(8))
                {
                    Fire();
                    ai.phase = 0;
                }
                return;
            default:
                return;
        }
    }
    if (state != 2u)
        return;
    phase = ai.phase;
    if (!phase)
    {
        value = this->model.pos.words[1];
        this->control.flags = 0;
        this->control.gravity_enabled = 1;
        ai.work[0] = (uint32)((sint32)value >> 12);
        random = RandomRange(256);
        ai.work[1] = 128u - random;
        random = RandomRange(256);
        ai.work[2] = 128u - random;
        random = RandomRange(256);
        ai.work[3] = 128u - random;
        random = RandomRange(120);
        bounce_speed = (random + 40u) << 12;
        motion.drag[2] = 31;
        motion.drag[1] = 31;
        motion.drag[0] = 31;
        random = RandomRange(20);
        value = bounce_speed;
        motion.velocity.words[0] = (10u - random) << 12;
        motion.velocity.words[1] = 0u - value;
        random = RandomRange(20);
        phase = ai.phase;
        motion.velocity.words[2] = (10u - random) << 12;
        ai.phase = phase + 1u;
        SpawnExplosionRing(model.pos.words, 50, 240, 220, 0, 5, 0, 100);
        use_small_effect_pool = false;
        child = AllocateEffectStorage(100);
        if (child)
        {
            trail = new (child) RibbonTrail(this->model.pos.words, 4, 128, 128, 128);
        }
        else
            trail = 0;
        if (!trail)
        {
            GameDiagnostic("MiniTank trail allocation failed\n");
            abort();
        }
        use_small_effect_pool = true;
        return;
    }
    if (phase != 1u)
        return;
    if (trail)
        trail->SetPosition(model.pos.words);
    dx = (uint32)(sint32)(sint16)ai.work[1];
    x = this->model.angles.words[0];
    x += dx;
    dz = (uint32)(sint32)(sint16)ai.work[3];
    this->model.angles.words[0] = x;
    z = this->model.angles.words[2];
    dy = (uint32)(sint32)(sint16)ai.work[2];
    z += dz;
    this->model.angles.words[2] = z;
    y = this->model.angles.words[1];
    x = this->model.angles.words[0];
    y += dy;
    this->model.angles.words[1] = y;
    z = this->model.angles.words[2];
    this->model.angles.words[0] = x & 4095u;
    y = this->model.angles.words[1];
    this->model.angles.words[2] = z & 4095u;
    height = this->model.pos.words[1];
    this->model.angles.words[1] = y & 4095u;
    floor = (sint16)ai.work[0];
    if (floor >= ((sint32)height >> 12))
        return;
    value = bounce_speed;
    if ((sint32)value > 163840)
    {
        value -= 163840u;
        bounce_speed = value;
        motion.velocity.words[1] = 0u - value;
        return;
    }
    Burst(220u);
    if (trail)
        trail->fading = 1;
    Finish();
}

uint32 MiniTank::Damage(uint32 damage, const sint32 *input_vector, uint32)
{
    FUNCTION_MARKER(0x80075930, "SLUS_003.73");
    ModelInstance *player;
    uint32 tick, health, signed_health, flags;
    uint32 shift = 6;
    Vector3 impulse;
    if (age < 60)
    {
        SetColor(3, 255, 255, 255);
        return 1;
    }
    tick = (uint32)this->control.damage_phase;
    this->control.damage_phase = tick + 1;
    signed_health = (uint32)(sint32)this->health;
    health = (uint16)this->health;
    if ((sint32)signed_health <= 0)
        return 1;
    this->health = (sint16)(uint16)(health - damage);
    ShiftVectorLeft(impulse.words, (const uint32 *)input_vector, shift);
    AddVectors(motion.velocity.words, impulse.words);
    signed_health = (uint32)(sint32)this->health;
    motion.velocity.words[1] = 0;
    if ((sint32)signed_health > 0)
    {
        SetColor(3, 255, 255, 255);
        return 1;
    }
    player = visual_lists.players;
    if (player)
        TryPlayPlayerKillQuote(player->pos.words);
    flags = this->lifecycle_flags;
    this->lifecycle_flags = flags & 0xFFEF;
    if (!RandomRange(2))
    {
        ai.state = 2;
        ai.phase = 0;
        return 1;
    }
    Burst(200);
    Finish();
    return 1;
}

uint32 MiniTank::SetTarget(const uint32 *target)
{
    FUNCTION_MARKER(0x80075BE8, "SLUS_003.73");
    uint32 x, y, z, left, right, scale, velocity_x, velocity_z, angle, flags;
    Vector3 difference, normal;
    if (this->model.animation)
        StartAnimation(0, 1);
    x = target[0];
    y = target[1];
    z = target[2];
    this->destination.words[0] = x;
    this->destination.words[1] = y;
    this->destination.words[2] = z;
    left = this->destination.words[0];
    right = this->model.pos.words[0];
    difference.words[0] = (uint32)((sint32)(left - right) >> 12);
    left = this->destination.words[1];
    right = this->model.pos.words[1];
    difference.words[1] = (uint32)((sint32)(left - right) >> 12);
    left = this->destination.words[2];
    right = this->model.pos.words[2];
    difference.words[2] = (uint32)((sint32)(left - right) >> 12);
    NormalizeShortVector(difference.words, normal.words);
    scale = ai.parameters[0];
    velocity_x = normal.words[0] * scale;
    velocity_z = normal.words[2] * scale;
    motion.velocity.words[0] = velocity_x;
    motion.velocity.words[1] = 0;
    motion.velocity.words[2] = velocity_z;
    left = this->destination.words[2];
    right = this->model.pos.words[2];
    z = left - right;
    left = this->destination.words[0];
    right = this->model.pos.words[0];
    angle = 3072 - (uint32)ratan2((sint32)z, (sint32)(left - right));
    flags = this->control.flags;
    target_angles.words[1] = angle & 4095;
    this->control.flags = flags | 1;
    return flags | 1;
}

uint32 MiniTank::Fire()
{
    ModelInstance *player;
    Vector3 position, target;
    GameHeapBlock *projectile;
    uint32 damage;
    uint32 lifetime;
    uint32 speed;
    ShortVector3 rotation;
    FUNCTION_MARKER(0x80075CFC, "SLUS_003.73");
    player = visual_lists.players;
    if (!player)
        return 0;
    GetModelAttachment(position.words, *this, 1);
    player = visual_lists.players;
    static_cast<Character *>(player->owner)->GetAttachment(target.words, 9);
    target.words[1] += 65536;
    CalculateDirectionAngles(rotation.words, position.words, target.words);
    GetModelAttachment(position.words, *this, 0);
    projectile = AllocateEffectStorage(108);
    if (projectile)
    {
        damage = GetBehaviorValue(0, 1) & 65535;
        lifetime = GetBehaviorValue(0, 2) & 65535;
        speed = GetBehaviorValue(0, 2) & 65535;
        new (projectile) OscillatingProjectile(position.words, rotation.components, damage, lifetime, speed, 16, 240, 240, 240, 96, 128, 0);
    }
    return PlayPositionedSound(20, this->model.pos.words, 0);
}

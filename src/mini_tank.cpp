#include <new>
#include <stdlib.h>
#include <string.h>

#include "collision_ray.h"
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
#include "model_attachment_data.h"
#include "game_diagnostics.h"
#include "player.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"
#include "mini_tank.h"
#include "mini_tank_ring.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "model_attachment.h"
#include "angle_math.h"
#include "oscillating_projectile.h"
#include "sprite_trail.h"
#include "ribbon_trail.h"
#include "audio_session.h"
#include "enemy.h"
#include "ring_effect.h"
#include "mesh_ring_effect.h"
#include "effect_object.h"

MiniTank::MiniTank(const RecordCursor &rec_data, uint16 idx) : Enemy(), trail(0), state_timer(0), engine_sound(0), sound_period(0), bounce_speed(0), age(0)
{
    RecordDataBorrow rec_borrow;
    uint16 angle;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8007488C, "SLUS_003.73");
    target_angles.x = target_angles.y = target_angles.z = 0;

    cursor = ReadPos(rec_data);
    angle = cursor.Read16();
    model.angles.x = angle;
    angle = (cursor + 2).Read16();
    model.angles.y = angle;
    angle = (cursor + 4).Read16();
    script_cursor = cursor + 6;
    rec_idx = idx;
    model.angles.z = angle;
    Init();
    cursor = script_cursor;
    if (cursor)
    {
        control.script_active = 1;
        ExecuteScript(cursor);
    }
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
    return (reason & 1) ? ReleaseStorage() : 0;
}

uint32 MiniTank::Init()
{
    uint32 floor, sound;
    FUNCTION_MARKER(0x80074A58, "SLUS_003.73");
    target_angles = model.angles;
    ++game_progress.level.enemy_total;
    SetModelBank("MiniTank");
    AddToList(visual_lists.actors);
    kind = 207;
    ground_offset = 64;
    AllocateBehaviorTable(1, 4);
    ai.parameters[0] = 50;
    ai.parameters[1] = 128;
    health = 20;
    SetBehaviorVal(0, 1, 10);
    SetBehaviorVal(0, 2, 140);
    SetBehaviorVal(0, 3, 50);
    floor = CollisionRay::FindGroundHeight(model.pos, 512, 512);
    if (floor != 0xFFFFFFFF)
    {
        model.pos.y = floor - ground_offset * ONE;
    }
    sound_period = 0xFFFFFFFF;
    sound = PlayPositionedSound(89, &model.pos, 0);
    engine_sound = sound;
    if (sound)
        sound_period = 16;
    control.script_active = 1;
    return 1;
}

uint32 MiniTank::Update()
{
    RecordDataBorrow rec_borrow;
    uint32 val, count, radius, factor, damage;
    uint32 idx, period, frame, sound;
    uint16 delay, flags, scale, angle;
    sint32 distance;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80074BA8, "SLUS_003.73");
    ++age;
    motion.acceleration = zero_vector;
    if (control.script_active)
    {
        delay = control.script_delay;
        if (delay)
            control.script_delay = delay - 1;
        else
            ExecuteScript(script_cursor);
    }
    else
        UpdateState();
    if (!control.hierarchy_ready)
    {
        model.SetAttachmentTable(MiniTankAttachments);
        InitializeModelAnimation(MiniTankHierarchy);
        control.hierarchy_ready = 1;
        model.flags |= 4;
    }
    flags = activation_flags;
    if (flags & 1)
    {
        ++control.activation_count;
        activation_flags &= 0xFFFE;
    }
    flags = activation_flags;
    if (flags & 4)
    {
        idx = 0;
        for (;;)
        {
            count = explosion_queue.count;
            if ((sint32)idx >= (sint32)count)
                break;
            const ExplosionEvent *event = &explosion_queue.entries[idx];
            distance = (sint32)model.pos.Distance(event->pos);
            radius = event->radius;
            if (distance < (sint32)radius)
            {
                val = radius - (uint32)distance;
                factor = event->strength;
                damage = DivideSignedWord(factor * val, radius);
                Damage(damage, &zero_vector, 0);
                if (health <= 0)
                    break;
            }
            ++idx;
        }
    }
    else if (control.flags & 1)
    {
        val = model.pos.Distance(dst);
        if (val < 129)
        {
            control.flags &= ~1;
        }
        else
        {
            angles.SetDir(model.pos, dst);
            angle = ((uint16)model.angles.y - (uint16)angles.y) & 4095;
            if (angle >= 64 && angle < 4033)
            {
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                motion.velocity.x = 0;
                UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, angles, 32);
            }
            else
            {
                model.angles = target_angles;
                motion.angular_velocity.z = 0;
                motion.angular_velocity.y = 0;
                motion.angular_velocity.x = 0;
                motion.angular_acceleration.z = 0;
                motion.angular_acceleration.y = 0;
                scale = ai.parameters[0];
                motion.angular_acceleration.x = 0;
                motion.velocity.SetDir(scale, angles);
            }
        }
    }
    Aim();
    UpdateModelAnimationData();
    UpdateWorldTransform();
    BuildModelPose();
    if (control.gravity_enabled)
        motion.acceleration.Add(world_gravity.vector);
    IntegrateMotion();
    period = sound_period;
    if (period != 0xFFFFFFFF)
    {
        frame = game_clock.tick;
        if (!RemainderSignedWord(frame, period))
        {
            sound = engine_sound;
            UpdatePositionedSoundVoices(sound, &model.pos, 0);
        }
    }
    // The legacy behavior caller discards the original return carrier
    return 0;
}

uint16 MiniTank::Aim()
{
    uint16 angle, yaw;
    Vector3 pos;
    ShortVector3 rotation;
    FUNCTION_MARKER(0x80074F3C, "SLUS_003.73");
    GetModelAttachment(pos, *this, 1);
    rotation.SetDir(pos, visual_lists.players->pos);
    angle = rotation.y;
    yaw = (uint16)model.angles.y;
    angle = (angle - yaw) & 4095;
    GetModelJointAngles(2).rotation.y = angle;
    return angle;
}

void MiniTank::UpdateState()
{
    ModelInstance *player;
    uint16 state;
    uint16 phase;
    uint32 flags;
    uint32 count;
    uint32 parameter;
    sint32 random;
    uint32 val;
    GameHeapBlock *child;
    uint32 damage;
    Vector3 candidate, pos;
    sint32 distance, floor;
    FUNCTION_MARKER(0x80074FA8, "SLUS_003.73");
    state = ai.state;
    if (!state)
    {
        phase = ai.phase;
        switch (phase)
        {
            case 0:
                flags = control.flags;
                if (!(flags & 1))
                {
                    if (ChoosePos(&candidate, &visual_lists.players->pos, 128, 512, 64, 1024, 1, 1))
                        SetTarget(&candidate);
                }
                player = visual_lists.players;
                distance = (sint32)model.pos.Distance(player->pos);
                if (distance < 204)
                {
                    ++ai.phase;
                    return;
                }
                ++state_timer;
                parameter = GetBehaviorVal(0, 3);
                random = RandomRange(5);
                parameter += random;
                count = state_timer;
                if (count < parameter)
                    return;
                pos = model.pos;
                player = visual_lists.players;
                if (CollisionRay::IsSegVisible(pos, player->pos))
                {
                    ai.phase = 3;
                    flags = control.flags;
                    state_timer = 0;
                    control.flags = flags & 0xFFFFFFFE;
                }
                return;
            case 1:
                ++ai.phase;
                return;
            case 2:
                count = age & 3;
                model.angles.y = (uint16)model.angles.y + (128);
                if (!count)
                    SetColor(3, 255, 255, 255);
                else if (count == 2)
                    SetColor(3, 255, 0, 0);
                if (RandomRange(16))
                    return;
                player = visual_lists.players;
                distance = (sint32)model.pos.Distance(player->pos);
                if (distance < 1024)
                {
                    damage = (sint32)(1024 - (uint32)distance) >> 6;
                    VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
                    damage_target->Damage(damage, &zero_vector, 0);
                }
                lifecycle_flags &= 0xFFEF;
                Burst(200);
                Finish();
                return;
            case 3:
                val = state_timer + 1;
                state_timer = val;
                if (val >= 5)
                {
                    ai.phase = 4;
                    state_timer = 0;
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
    if (state != 2)
        return;
    phase = ai.phase;
    if (!phase)
    {
        sint32 height = model.pos.y;
        control.flags = 0;
        control.gravity_enabled = 1;
        ai.work[0] = height >> 12;
        random = RandomRange(256);
        ai.work[1] = 128 - random;
        random = RandomRange(256);
        ai.work[2] = 128 - random;
        random = RandomRange(256);
        ai.work[3] = 128 - random;
        random = RandomRange(120);
        bounce_speed = (random + 40) * ONE;
        motion.drag[2] = 31;
        motion.drag[1] = 31;
        motion.drag[0] = 31;
        random = RandomRange(20);
        motion.velocity.x = (10 - random) * ONE;
        motion.velocity.y = -bounce_speed;
        random = RandomRange(20);
        motion.velocity.z = (10 - random) * ONE;
        ++ai.phase;
        RingBurstEffect::SpawnExplosion(&model.pos, 50, 240, 220, 0, 5, 0, 100);
        use_small_effect_pool = false;
        child = EffectObject::AllocateStorage(100);
        if (child)
        {
            trail = new (child) RibbonTrail(&model.pos, 4, 128, 128, 128);
        }
        else
            trail = 0;
        if (!trail)
        {
            GameDiagnostic(("MiniTank trail allocation failed\n"));
            abort();
        }
        use_small_effect_pool = true;
        return;
    }
    if (phase != 1)
        return;
    if (trail)
        trail->SetPos(&model.pos);
    model.angles.x = (uint16)model.angles.x + ai.work[1];
    model.angles.z = (uint16)model.angles.z + ai.work[3];
    model.angles.y = (uint16)model.angles.y + ai.work[2];
    model.angles.x = (uint16)model.angles.x & (4095);
    model.angles.z = (uint16)model.angles.z & (4095);
    model.angles.y = (uint16)model.angles.y & (4095);
    floor = (sint16)ai.work[0];
    if (floor >= (model.pos.y >> 12))
        return;
    if (bounce_speed > 163840)
    {
        bounce_speed -= 163840;
        motion.velocity.y = -bounce_speed;
        return;
    }
    Burst(220);
    if (trail)
        trail->fading = 1;
    Finish();
}

uint32 MiniTank::Damage(uint32 damage, const Vector3 *input_vector, uint32)
{
    FUNCTION_MARKER(0x80075930, "SLUS_003.73");
    ModelInstance *player;
    Vector3 impulse;
    if (age < 60)
    {
        SetColor(3, 255, 255, 255);
        return 1;
    }
    ++control.damage_phase;
    if (health <= 0)
        return 1;
    health -= damage;
    impulse.ShiftLeft(*input_vector, 6);
    motion.velocity.Add(impulse);
    motion.velocity.y = 0;
    if (health > 0)
    {
        SetColor(3, 255, 255, 255);
        return 1;
    }
    player = visual_lists.players;
    if (player)
        static_cast<Player *>(player->owner)->TryPlayKillQuote();
    lifecycle_flags &= 0xFFEF;
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

uint32 MiniTank::SetTarget(const Vector3 *target)
{
    FUNCTION_MARKER(0x80075BE8, "SLUS_003.73");
    uint16 scale, angle;
    uint32 flags;
    Vector3 difference, normal;
    if (model.animation)
        StartAnimation(0, 1);
    dst = *target;
    difference.x = (sint32)((uint32)dst.x - model.pos.x) >> 12;
    difference.y = (sint32)((uint32)dst.y - model.pos.y) >> 12;
    difference.z = (sint32)((uint32)dst.z - model.pos.z) >> 12;
    NormalizeShortVector(difference, normal);
    scale = ai.parameters[0];
    motion.velocity.x = normal.x * scale;
    motion.velocity.y = 0;
    motion.velocity.z = normal.z * scale;
    angle = 3072 - ratan2((sint32)((uint32)dst.z - model.pos.z), (sint32)((uint32)dst.x - model.pos.x));
    flags = control.flags;
    target_angles.y = angle & 4095;
    control.flags = flags | 1;
    return flags | 1;
}

uint32 MiniTank::Fire()
{
    ModelInstance *player;
    Vector3 pos, target;
    GameHeapBlock *projectile;
    uint16 damage;
    uint16 lifetime;
    uint16 speed;
    ShortVector3 rotation;
    FUNCTION_MARKER(0x80075CFC, "SLUS_003.73");
    player = visual_lists.players;
    if (!player)
        return 0;
    GetModelAttachment(pos, *this, 1);
    player = visual_lists.players;
    static_cast<Character *>(player->owner)->GetAttachment(target, 9);
    target.y = (uint32)target.y + (65536);
    rotation.SetDir(pos, target);
    GetModelAttachment(pos, *this, 0);
    projectile = EffectObject::AllocateStorage(108);
    if (projectile)
    {
        damage = GetBehaviorVal(0, 1);
        lifetime = GetBehaviorVal(0, 2);
        speed = GetBehaviorVal(0, 2);
        new (projectile) OscillatingProjectile(&pos, &rotation, damage, lifetime, speed, 16, 240, 240, 240, 96, 128, 0);
    }
    return PlayPositionedSound(20, &model.pos, 0);
}

MiniTank::~MiniTank()
{
    if (engine_sound)
        StopSoundVoices(engine_sound);
}

void MiniTank::Burst(uint8 g)
{
    GameHeapBlock *child;
    Vector3 velocity;
    HandleExplosionProximity(&model.pos, 0);
    RingBurstEffect::SpawnExplosion(&model.pos, 200, 240, g, 0, 5, 0, 100);
    child = EffectObject::AllocateStorage(120);
    if (child)
    {
        velocity = zero_vector;
        MiniTankRing *ring = new (child) MiniTankRing(velocity);
        ring->SetGeometry(&model.pos, 0, 0);
        ring->lifetime = 32;
        ring->radius_step = 32;
        ring->width_step = 16;
    }
    velocity.x = 0;
    velocity.y = -81920;
    velocity.z = 0;
    child = EffectObject::AllocateStorage(120);
    if (child)
    {
        MiniTankRing *ring = new (child) MiniTankRing(velocity);
        ring->SetGeometry(&model.pos, 0, 0);
        ring->lifetime = 32;
        ring->radius_step = 32;
        ring->width_step = 16;
    }
}

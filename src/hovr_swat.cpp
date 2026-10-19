#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_heap.h"
#include "level_session.h"
#include "game_tick.h"

#include "record_scripts.h"

#include "world_gravity.h"

#include "object_initializers.h"

#include "effect_object.h"
#include "effect_sprite.h"
#include "effect_lifecycle.h"
#include "audio_session.h"

#include "vector_distance.h"
#include "visual_object.h"
#include "model_resources.h"
#include "angle_math.h"
#include "game_diagnostics.h"
#include "player.h"
#include "vector_arithmetic.h"
#include "game_random.h"
#include "game_object_allocation.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"
#include "particle_debris.h"
#include "projectile_oscillating.h"

#include "hovr_swat.h"
#include "record_data.h"
#include "audio_voice.h"

HovrSwat::HovrSwat(const RecordCursor &record_data, uint32 index) : Enemy(), sound_timer(0), rotor_sound(0), sound_distance(0), debris_first(0), debris_count(0)
{
    RecordDataBorrow record_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x800590DC, "SLUS_003.73");
    aim_pos.words[0] = aim_pos.words[1] = aim_pos.words[2] = 0;
    aim_offset.words[0] = aim_offset.words[1] = aim_offset.words[2] = 0;

    cursor = ReadPosition(record_data);
    script_cursor = ReadAngles(cursor);
    this->record_idx = index;
    Init();
}

HovrSwat::~HovrSwat()
{
    if (rotor_sound)
        StopSoundVoices(rotor_sound);
    DestroyEffects();
}

uint32 HovrSwat::Init()
{
    uint32 random, bank, count;
    FUNCTION_MARKER(0x800591D4, "SLUS_003.73");
    count = game_progress.level.enemy_total;
    game_progress.level.enemy_total = count + 1u;
    SetModelBank("HovrSwat");
    AddToList(visual_lists.actors);
    this->kind = 201;
    motion.drag[0] = 3;
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    sound_distance = MeasurePositionDistance(model.pos.words, visual_lists.cameras->pos.words);
    AllocateBehaviorTable(1, 4);
    ai.parameters[0] = 12;
    ai.parameters[1] = 128;
    SetBehaviorValue(0, 1, 10);
    SetBehaviorValue(0, 2, 80);
    SetBehaviorValue(0, 3, 90);
    SetAnimationRange(0u, 0u, 0xFFFFFFFF);
    InitEffects();
    this->health = (sint16)(uint16)(20u);
    rotor_sound = PlayPositionedSound(101, model.pos.words, 0);
    random = RandomRange(8);
    bank = GetRecordModelBankSlot();
    sound_timer = random + 1;
    debris_first = ::FindModelResourceIndex(0xDB2D0C4F, bank);
    debris_count = 10;
    this->control.script_active = 1;
    return 1;
}

uint32 HovrSwat::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8005933C, "SLUS_003.73");
    StopSoundVoices(rotor_sound);
    rotor_sound = 0;
    DestroyEffects();
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    result = Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(this);
    return result;
}

uint32 HovrSwat::Damage(uint32 damage, const sint32 *input_vector, uint32)
{
    FUNCTION_MARKER(0x800593CC, "SLUS_003.73");
    uint32 hits = (uint32)this->control.damage_phase;
    sint32 health = this->health;
    Vector3 impulse;
    uint32 shift = 3, remaining, flags;
    ModelInstance *player;
    this->control.damage_phase = hits + 1;
    if (health > 0)
    {
        ShiftVectorRight(impulse.words, (const uint32 *)input_vector, shift);
        AddVectors(motion.velocity.words, impulse.words);
        remaining = (uint16)this->health - damage;
        this->health = (sint16)(uint16)(remaining);
        if ((sint16)remaining > 0)
            SetColor(3, 255, 255, 255);
        else
        {
            player = visual_lists.players;
            if (player)
                TryPlayPlayerKillQuote(player->pos.words);
            flags = this->lifecycle_flags;
            this->lifecycle_flags = flags & 0xFFEF;
            Fragment();
            Finish();
        }
    }
    return 1;
}

uint32 HovrSwat::Fragment()
{
    uint32 trig_angle;
    Vector3 velocity;
    uint32 index;
    uint32 angle;
    uint32 factor;
    GameHeapBlock *child;
    uint32 kind;
    FUNCTION_MARKER(0x800594BC, "SLUS_003.73");
    HandleExplosionProximity(this->model.pos.words, 0);
    SpawnExplosionRing(this->model.pos.words, 200, 240, 200, 0, 5, 0, 100);
    for (index = 0; index < 6; ++index)
    {
        velocity = motion.velocity;
        angle = RandomRange(4096);
        factor = RandomRange(8);
        trig_angle = angle & 0xFFF;
        velocity.words[0] += (uint32)rsin(trig_angle) * (factor + 24);
        factor = RandomRange(16);
        velocity.words[1] -= (factor + 32) << 12;
        factor = RandomRange(8);
        velocity.words[2] += (uint32)rcos(trig_angle) * (factor + 24);
        child = AllocateZeroedObject(320);
        if (child)
        {
            factor = RandomRange(debris_count);
            kind = (debris_first + factor) & 0xFFFF;
            new (child) Debris(this->model.pos.words, velocity.words, kind, 0, 2, 32, GetRecordModelBankName());
        }
    }
    return 0;
}

uint32 HovrSwat::InitEffects()
{
    uint32 index, size = 1024, offset_y = 0;
    Vector3 position;
    uint32 red = 64, green = 64, blue = 128;
    FUNCTION_MARKER(0x80059644, "SLUS_003.73");
    position = model.pos;
    use_small_effect_pool = false;
    for (index = 0; index < 8; ++index)
    {
        GameHeapBlock *allocation = AllocateEffectStorage(112);
        if (allocation)
        {
            Sprite *sprite = new (allocation) Sprite();
            sprite->persistent = 1;
            position.words[1] = model.pos.words[1] + offset_y;
            sprite->SetPosition(position.words);
            sprite->SelectResource("Smoke");
            sprite->SetSize(size);
            sprite->EnableSemiTransparency();
            sprite->SetColors(red, green, blue);
            red -= 8;
            green -= 8;
            blue -= 12;
            smoke[index] = sprite;
        }
        size += 256;
        offset_y += 0x18000;
    }
    use_small_effect_pool = true;
    return 1;
}

uint32 HovrSwat::DestroyEffects()
{
    FUNCTION_MARKER(0x8005979C, "SLUS_003.73");
    for (uint32 index = 0; index < 8; ++index)
    {
        Sprite *sprite = smoke[index];
        if (sprite)
        {
            sprite->Destroy(3);
            smoke[index] = 0;
        }
    }
    return 0;
}

uint32 HovrSwat::UpdateEffects()
{
    Vector3 position;
    FUNCTION_MARKER(0x80059808, "SLUS_003.73");
    position = model.pos;
    for (uint32 index = 0; index < 8; ++index)
    {
        Sprite *sprite = smoke[index];
        if (sprite)
        {
            uint32 random = RandomRange(32);
            uint32 offset = ((index << 5) + random - 16) << 12;
            position.words[1] = model.pos.words[1] + offset;
            sprite->SetPosition(position.words);
        }
    }
    return 0;
}

uint32 HovrSwat::Update()
{
    RecordDataBorrow record_borrow;
    uint32 flags, hits, timer, distance, scale;
    Vector3 position;
    ShortVector3 angles;
    FUNCTION_MARKER(0x800598B8, "SLUS_003.73");
    flags = this->activation_flags;
    this->control.prev_activation_flags = flags;
    if (flags & 1u)
    {
        hits = (uint32)this->control.activation_count;
        this->activation_flags = flags & 0xFFFEu;
        this->control.activation_count = hits + 1u;
    }
    position = zero_vector;
    motion.acceleration = position;
    if ((uint32)this->control.script_active)
    {
        timer = (uint32)this->control.script_delay;
        if (timer)
            this->control.script_delay = timer - 1u;
        else
            ExecuteScript(script_cursor);
    }
    else
        UpdateState();
    if (this->control.flags & 1u)
    {
        distance = MeasurePositionDistance(this->model.pos.words, this->destination.words);
        if (distance < 129u)
            this->control.flags = this->control.flags & ~1u;
        else
        {
            CalculateDirectionAngles(angles.words, this->model.pos.words, this->destination.words);
            scale = ai.parameters[0];
            BuildDirectionVector(motion.acceleration.words, scale, angles.words);
        }
    }
    flags = this->control.flags;
    if (flags & 0x1Du)
    {
        if (flags & 0x10u)
        {
            ModelInstance *subject = tracked_target.GetObject();
            if (subject)
            {
                aim_pos.words[0] = subject->pos.words[0];
                aim_pos.words[1] = subject->pos.words[1];
                aim_pos.words[2] = subject->pos.words[2];
            }
            else
                control.flags &= ~16;
        }
        else if (flags & 8u)
        {
            AddVectors(position.words, model.pos.words, this->aim_offset.words);
            this->aim_pos = position;
        }
        if (flags & 0x1Cu)
        {
            position = this->model.pos;
            CalculateDirectionAngles(angles.words, position.words, this->aim_pos.words);
            if (flags & 0x10u)
                angles.words[0] = 0;
        }
        else
            CalculateDirectionAngles(angles.words, this->model.pos.words, this->destination.words);
        scale = ai.parameters[1];
        UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, angles, scale);
    }
    else
    {
        motion.angular_velocity.words[2] = 0;
        motion.angular_velocity.words[1] = 0;
        motion.angular_velocity.words[0] = 0;
        motion.angular_acceleration.words[2] = 0;
        motion.angular_acceleration.words[1] = 0;
        motion.angular_acceleration.words[0] = 0;
    }
    this->model.angles.words[0] = 0;
    UpdateWorldTransform();
    if ((uint32)this->control.gravity_enabled)
        AddVectors(motion.acceleration.words, world_gravity.vector.words);
    IntegrateMotion();
    if (rotor_sound)
    {
        timer = sound_timer;
        if (timer)
            sound_timer = timer - 1;
        else
        {
            distance = MeasurePositionDistance(model.pos.words, visual_lists.cameras->pos.words);
            sound_timer = 8;
            UpdatePositionedSoundVoices(rotor_sound, model.pos.words, (sint32)(distance - sound_distance));
            sound_distance = distance;
        }
    }
    else if (!(game_clock.tick & 7u))
        rotor_sound = PlayPositionedSound(101, model.pos.words, 0);
    return UpdateEffects();
}

uint32 HovrSwat::ExecuteCommand(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value, shift = 12;
    Vector3 position;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80059D1C, "SLUS_003.73");
    command &= 0xFFFFu;
    switch (command)
    {
        case 0x4220:
        case 0x4222:
            cursor = AlignScriptCursor(script_cursor);
            position.words[0] = cursor.Read32();
            position.words[1] = (cursor + 4u).Read32();
            position.words[2] = (cursor + 8u).Read32();
            ShiftVectorLeft(position.words, shift);
            if (command == 0x4222u)
                AddVectors(position.words, this->model.pos.words);
            script_cursor = cursor + 12u;
            SetDestination(position.words);
            return 1;
        case 0x4221:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2u;
            if (value & 0x2000u)
            {
                value = ResolveParameter(value);
            }
            ReadRecordPosition(position.words, value & 0xFFFF);
            SetDestination(position.words);
            return 1;
        case 0x4262:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2u;
            if (!value)
                Fire();
            return 1;
        case 0x4406:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2u;
            if (value)
                SetTargetObject(visual_lists.players);
            else
                ClearTarget();
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 HovrSwat::SetDestination(const uint32 *position)
{
    FUNCTION_MARKER(0x80059F28, "SLUS_003.73");
    const uint32 x = position[0];
    const uint32 y = position[1];
    const uint32 z = position[2];
    destination.words[0] = x;
    destination.words[1] = y;
    destination.words[2] = z;
    control.flags |= 1;
    return control.flags;
}

uint32 HovrSwat::SetTargetObject(ModelInstance *target)
{
    FUNCTION_MARKER(0x8005A004, "SLUS_003.73");
    uint32 flags = control.flags;
    tracked_target.SetObject(target);
    flags = (flags | 16) & 0xFFFFFFF3;
    control.flags = flags;
    return flags;
}

uint32 HovrSwat::ClearTarget()
{
    uint32 flags = this->control.flags & 0xFFFFFFE3;
    FUNCTION_MARKER(0x8005A020, "SLUS_003.73");
    this->control.flags = flags;
    return flags;
}

uint32 HovrSwat::Fire()
{
    Vector3 position;
    GameHeapBlock *projectile;
    uint32 damage;
    uint32 speed;
    ShortVector3 rotation;
    FUNCTION_MARKER(0x8005A034, "SLUS_003.73");
    if (!visual_lists.players)
        return 0;
    SetAnimationRange(2, 0, 0xFFFFFFFF);
    position = model.pos;
    CalculateDirectionAngles(rotation.words, position.words, visual_lists.players->pos.words);
    projectile = AllocateEffectStorage(108);
    if (projectile)
    {
        damage = GetBehaviorValue(0, 1) & 0xFFFF;
        damage = ScaleDamageForDifficulty(damage);
        speed = GetBehaviorValue(0, 2) & 0xFFFF;
        new (projectile) OscillatingProjectile(position.words, rotation.components, damage, 45, speed, 16, 240, 240, 240, 64, 32, 128);
    }
    return PlayPositionedSound(20, model.pos.words, 0);
}

uint32 HovrSwat::UpdateState()
{
    uint32 trig_angle;
    uint32 state, value, radius, orbit_size, yaw;
    Vector3 position, vector, normal;
    ModelInstance *player;
    sint32 saved_yaw;
    ShortVector3 rotation;
    FUNCTION_MARKER(0x8005A170, "SLUS_003.73");
    if (ai.state)
        return 1;
    state = ai.phase;
    if (state == 0u || state == 1u)
    {
        value = (uint32)this->control.prev_activation_flags & 1u;
        if (value)
        {
            position = this->model.pos;
            position.words[1] -= 0x800000u;
            SetDestination(position.words);
            ai.phase = 2;
            return 2;
        }
        if (!state)
        {
            player = visual_lists.players;
            if (!player)
                return 0;
            value = this->model.pos.words[0];
            vector.words[0] = (uint32)((sint32)(value - player->pos.words[0]) >> 12);
            vector.words[1] = 0;
            value = this->model.pos.words[2];
            vector.words[2] = (uint32)((sint32)(value - player->pos.words[2]) >> 12);
            NormalizeShortVector(vector.words, normal.words);
            ai.work[0] = normal.words[0];
            ai.work[2] = 1024;
            ai.work[3] = 512;
            ai.work[4] = 0;
            ai.work[1] = normal.words[2];
            if (RandomRange(64) & 1u)
                this->control.flags = this->control.flags | 0x20u;
            value = ai.phase + 1u;
            ai.phase = value;
            return value;
        }
        if (!(this->control.flags & 1u))
        {
            value = RandomRange(32);
            if (!value)
            {
                trig_angle = RandomRange(4096) & 0xFFFu;
                ai.work[0] = (uint16)rsin(trig_angle);
                value = (uint16)rcos(trig_angle);
                ai.work[1] = value;
                return value;
            }
            player = visual_lists.players;
            if (!player)
                return value;
            SetTargetObject(visual_lists.players);
            player = visual_lists.players;
            radius = ai.work[2];
            orbit_size = ai.work[3];
            yaw = ai.work[4];
            saved_yaw = (sint16)ai.work[4];
            position = player->pos;
            position.words[0] += (uint32)(sint32)(sint16)ai.work[0] * radius;
            value = (uint32)(sint32)(sint16)ai.work[1] * radius;
            position.words[1] -= 0x100000u;
            position.words[2] += value;
            trig_angle = RandomRange(4096) & 0xFFFu;
            rotation.words[0] = (uint16)((sint32)rsin(trig_angle) >> 6);
            rotation.words[1] = (uint16)yaw;
            rotation.words[2] = 0;
            BuildDirectionVector(vector.words, orbit_size, rotation.words);
            AddVectors(position.words, vector.words);
            SetDestination(position.words);
            value = this->control.flags & 0x20u;
            yaw = RandomRange(64);
            yaw = value ? (uint32)saved_yaw + 256u + yaw : (uint32)saved_yaw - 256u - yaw;
            ai.work[4] = yaw & 0xFFFu;
            if (!RandomRange(4))
            {
                if (RandomRange(2))
                {
                    if (radius < 2048u)
                        ai.work[2] = radius + 32u;
                }
                else if (radius >= 513u)
                    ai.work[2] = radius - 32u;
            }
            value = RandomRange(4);
            if (value)
                return value;
            if (RandomRange(2))
            {
                value = orbit_size + 16u;
                if (orbit_size < 1024u)
                    ai.work[3] = value;
            }
            else
            {
                value = orbit_size - 16u;
                if (orbit_size >= 257u)
                    ai.work[3] = value;
            }
            return value;
        }
        value = this->model.animation;
        if (value == 1u)
        {
            value = this->playback.complete;
            if (!value)
                return value;
            value = (uint32)(sint32)(sint8)this->model.frame;
            if (!value)
            {
                value = GetBehaviorValue(0, 3) & 0xFFFFu;
                value = RandomRange(value);
                if (value)
                    return value;
                return SetAnimationRange(1u, 0u, 0xFFFFFFFF);
            }
            ai.work[5] = value;
        }
        else if (value == 2u)
        {
            value = this->playback.complete;
            if (!value)
                return value;
            if (RandomRange(3))
                return SetAnimationRange(1u, (uint32)(sint32)(sint16)ai.work[5], 0u);
        }
        else
            return SetAnimationRange(1u, 0u, 0xFFFFFFFF);
        return Fire();
    }
    if (state == 2u)
    {
        value = this->control.flags & 1u;
        if (value)
            return value;
        return Finish();
    }
    return 2;
}

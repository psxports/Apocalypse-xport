#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_heap.h"
#include "effect_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"

#include "collision.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "angle_math.h"
#include "vector_distance.h"
#include "object_damage.h"
#include "game_diagnostics.h"
#include "player.h"
#include "audio_session.h"
#include "vector_arithmetic.h"

#include "bat.h"
#include "record_data.h"
#include "game_random.h"
#include "waving_beam.h"
#include "game_object_allocation.h"
#include "audio_voice.h"

static ObjectReference<Bat> swarm_parents[8];

Bat::Bat(const RecordCursor &record_data, uint32 index) : Enemy(), swarm_slot(255), swarm_size(0), swarm_sound(0)
{
    RecordDataBorrow record_borrow;
    uint32 slot, value, flags;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80049F24, "SLUS_003.73");

    for (slot = 0; slot < 8; ++slot)
    {
        if (!swarm_parents[slot].Get())
        {
            swarm_parents[slot] = this;
            swarm_slot = slot;
            break;
        }
    }
    value = this->control.flags;
    flags = this->model.flags;
    this->collision_radius = 0;
    this->control.flags = value | 0x40;
    value = this->lifecycle_flags;
    this->model.flags = flags | 1;
    this->lifecycle_flags = value & 0xFFEF;
    cursor = ReadPosition(record_data);
    cursor = ReadAngles(cursor);
    script_cursor = cursor;
    this->record_idx = index;
    Initialize();
}

Bat::Bat(const uint32 *position, uint32 slot) : Enemy(), swarm_slot(slot), swarm_size(0), swarm_sound(0)
{
    RecordDataBorrow record_borrow;
    uint32 x, y, z;
    FUNCTION_MARKER(0x8004A000, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    pos_history.saved.words[0] = x;
    pos_history.saved.words[1] = y;
    pos_history.saved.words[2] = z;
    this->model.pos.words[0] = x;
    this->model.pos.words[1] = y;
    this->model.pos.words[2] = z;
    script_cursor = 0;
    Initialize();
    this->control.script_active = 0;
}

Bat::~Bat()
{
    if (swarm_slot < 8 && swarm_parents[swarm_slot].Get() == this)
        swarm_parents[swarm_slot] = 0;
    if (swarm_sound)
        StopSoundVoices(swarm_sound);
}

uint32 Bat::Initialize()
{
    uint32 total, x, y, z, value;
    FUNCTION_MARKER(0x8004A090, "SLUS_003.73");
    total = game_progress.level.enemy_total;
    x = this->model.pos.words[0];
    y = this->model.pos.words[1];
    z = this->model.pos.words[2];
    this->destination.words[0] = x;
    this->destination.words[1] = y;
    this->destination.words[2] = z;
    game_progress.level.enemy_total = total + 1;
    SetModelBank("bat");
    AddToList(visual_lists.actors);
    this->kind = 209;
    motion.drag[0] = 3;
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    AllocateBehaviorTable(1, 4);
    ai.parameters[0] = 12;
    ai.parameters[1] = 128;
    ai.parameters[2] = 12;
    SetBehaviorValue(0, 1, 10);
    SetBehaviorValue(0, 2, 80);
    SetBehaviorValue(0, 3, 90);
    StartAnimation(0, 1);
    value = RandomRange(16);
    this->model.frame = value;
    this->health = (sint16)(uint16)(5);
    this->control.script_active = 1;
    return 1;
}

uint32 Bat::Destroy(uint32 reason)
{
    uint32 flags;
    FUNCTION_MARKER(0x8004A1AC, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    flags = this->control.flags;
    if (flags & 0x40)
    {
        if (swarm_slot >= 8)
        {
            GameDiagnostic("Bat swarm parent has no valid slot\n");
            abort();
        }
        swarm_parents[swarm_slot] = 0;
    }
    StopSoundVoices(swarm_sound);
    swarm_sound = 0;
    Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(this);
    return 0;
}

uint32 Bat::Damage(uint32 damage, const sint32 *input_vector, uint32 kind)
{
    uint32 tick, health, value, axis, flags;
    Vector3 shifted;
    ModelInstance *player;
    FUNCTION_MARKER(0x8004A25C, "SLUS_003.73");
    tick = (uint32)this->control.damage_phase + 1;
    health = (uint32)(sint32)this->health;
    this->control.damage_phase = tick;
    if ((sint32)health <= 0)
        return tick;
    for (axis = 0; axis < 3; ++axis)
    {
        value = (uint32)input_vector[axis];
        shifted.words[axis] = (uint32)((sint32)value >> 3);
    }
    AddVectors(motion.velocity.words, shifted.words);
    value = (uint16)this->health - damage;
    this->health = (sint16)(uint16)(value);
    if ((sint32)(value << 16) > 0)
        return SetColor(3, 255, 255, 255);
    player = visual_lists.players;
    if (player)
        TryPlayPlayerKillQuote(player->pos.words);
    PlayPositionedSound(77, this->model.pos.words, 0);
    flags = this->lifecycle_flags;
    this->lifecycle_flags = flags & 0xFFEF;
    if (kind == 5)
    {
        ai.state = 2;
        ai.phase = 0;
        this->control.script_active = 0;
        return 2;
    }
    return Finish();
}

uint32 Bat::Update()
{
    RecordDataBorrow record_borrow;
    uint32 value, flags, found, sound;
    ModelInstance *node, *player;
    uint32 scale;
    ShortVector3 angles;
    FUNCTION_MARKER(0x8004A37C, "SLUS_003.73");
    flags = this->activation_flags;
    this->control.prev_activation_flags = flags;
    if (flags & 1u)
    {
        value = (uint32)this->control.activation_count;
        this->activation_flags = flags & 0xFFFEu;
        this->control.activation_count = value + 1u;
    }
    flags = this->control.flags;
    if (flags & 64u)
    {
        if (!(game_clock.tick & 63u))
        {
            found = 0;
            node = visual_lists.actors;
            while (node)
            {
                if (node->owner != this && static_cast<VisualObject *>(node->owner)->kind == 209)
                {
                    value = static_cast<Bat *>(node->owner)->swarm_slot;
                    if (value == swarm_slot)
                    {
                        found = 1;
                        break;
                    }
                }
                node = node->next;
            }
            if (!found)
            {
                Finish();
                return 0;
            }
        }
        value = this->player_distance;
        if (value < 2049u)
        {
            sound = swarm_sound;
            if (!sound)
            {
                value = PlayPositionedSound(76, model.pos.words, 0);
                swarm_sound = value;
            }
            else if (!RandomRange(12))
            {
                sound = swarm_sound;
                UpdatePositionedSoundVoices(sound, model.pos.words, 0);
            }
        }
        else
        {
            sound = swarm_sound;
            if (sound)
            {
                ::StopSoundVoices(sound);
                swarm_sound = 0;
            }
        }
    }
    else if (this->player_distance < 96u)
    {
        player = visual_lists.players;
        static_cast<VisualObject *>(player->owner)->Damage(1, zero_vector.components, 0);
    }
    motion.acceleration = zero_vector;
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
    if (this->control.flags & 1u)
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
            scale = ai.parameters[0];
            BuildDirectionVector(motion.acceleration.words, scale, angles.words);
            scale = ai.parameters[1];
            UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, angles, scale);
        }
    }
    if ((uint32)this->control.gravity_enabled)
        AddVectors(motion.acceleration.words, world_gravity.vector.words);
    IntegrateMotion();
    UpdateWorldTransform();
    // The legacy behavior caller discards the original return carrier
    return 0;
}

uint32 Bat::ExecuteCommand(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value;
    uint32 count;
    GameHeapBlock *child;
    uint32 slot;
    uint32 speed;
    RecordCursor cursor;
    Vector3 point;
    FUNCTION_MARKER(0x8004A690, "SLUS_003.73");
    command &= 0xFFFFu;
    switch (command)
    {
        case 0x4220:
            cursor = AlignScriptCursor(script_cursor);
            point.words[0] = cursor.Read32();
            point.words[1] = (cursor + 4u).Read32();
            point.words[2] = (cursor + 8u).Read32();
            point.words[0] <<= 12;
            point.words[1] <<= 12;
            point.words[2] <<= 12;
            script_cursor = cursor + 12u;
            SetTarget(point.words);
            return 1;
        case 0x4221:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2u;
            if (value & 0x2000u)
            {
                value = ResolveParameter(value);
            }
            ReadRecordPosition(point.words, value & 0xFFFF);
            SetTarget(point.words);
            return 1;
        case 0x4600:
            cursor = script_cursor;
            count = cursor.Read16();
            script_cursor = cursor + 2u;
            swarm_size = count;
            while (count)
            {
                point.words[0] = RandomRange(1024);
                point.words[1] = 0 - RandomRange(512);
                point.words[2] = RandomRange(1024) - 512;
                point.words[0] -= 512u;
                value = this->model.pos.words[0];
                point.words[0] += value;
                value = this->model.pos.words[1];
                point.words[1] += value;
                value = this->model.pos.words[2];
                point.words[2] += value;
                child = AllocateZeroedObject(504);
                if (child)
                {
                    slot = swarm_slot;
                    Bat *spawned = new (child) Bat(point.words, slot);
                    spawned->swarm_size = swarm_size;
                    speed = ai.parameters[2];
                    spawned->ai.parameters[0] = speed;
                }
                --count;
            }
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 Bat::SetTarget(const uint32 *target)
{
    FUNCTION_MARKER(0x8004A874, "SLUS_003.73");
    uint32 x, y, z, flags;
    x = target[0];
    y = target[1];
    z = target[2];
    this->destination.words[0] = x;
    this->destination.words[1] = y;
    this->destination.words[2] = z;
    flags = this->control.flags | 1;
    this->control.flags = flags;
    return flags;
}

void Bat::UpdateState()
{
    GameHeapBlock *reservation;
    uint32 trig_angle;
    uint32 state, phase, flags, value, left, right, radius, amplitude;
    ModelInstance *player;
    Bat *parent;
    uint32 yaw, signed_yaw, axis, ground, height;
    Vector3 point, normal, offset;
    ShortVector3 angles;
    FUNCTION_MARKER(0x8004A8E4, "SLUS_003.73");
    state = ai.state;
    if (state == 0u)
    {
        phase = ai.phase;
        if (phase == 0u)
        {
            player = visual_lists.players;
            if (!player)
                return;
            if (this->control.flags & 64u)
            {
                left = this->model.pos.words[0];
                right = player->pos.words[0];
                point.words[0] = (uint32)((sint32)(left - right) >> 12);
                point.words[1] = 0;
                left = this->model.pos.words[2];
                right = player->pos.words[2];
                point.words[2] = (uint32)((sint32)(left - right) >> 12);
                NormalizeShortVector(point.words, normal.words);
                ai.work[0] = normal.words[0];
                value = normal.words[2] & 0xFFFFu;
                ai.work[2] = 1024;
                ai.work[3] = 1024;
                ai.work[4] = 0;
                ai.work[1] = value;
                value = RandomRange(64);
                flags = this->control.flags;
                this->control.flags = (value & 1u) ? flags | 32u : flags & 0xFFFFFFDF;
            }
            else
            {
                value = RandomRange(256);
                amplitude = swarm_size >> 3;
                ai.work[3] = (value + 128u) * amplitude * amplitude;
                value = RandomRange(4096);
                ai.work[4] = value;
            }
            phase = ai.phase;
            ai.phase = phase + 1u;
            return;
        }
        if (phase == 2u)
        {
            if (!(this->control.flags & 1u))
                Finish();
            return;
        }
        if (phase != 1u)
            return;
        if ((uint32)this->control.prev_activation_flags & 1u)
        {
            point = this->model.pos;
            point.words[1] -= 0x800000u;
            SetTarget(point.words);
            ai.phase = 2;
            return;
        }
        if (this->control.flags & 1u)
            return;
        if (!RandomRange(32))
        {
            value = RandomRange(4096);
            trig_angle = value & 4095u;
            left = (uint16)rsin(trig_angle);
            ai.work[0] = left;
            right = (uint16)rcos(trig_angle);
            ai.work[1] = right;
            return;
        }
        flags = this->control.flags;
        if (!(flags & 64u))
        {
            if (!RandomRange(16) && this->player_distance < 1024u)
            {
                ai.state = 1;
                ai.phase = 0;
                return;
            }
            flags = this->control.flags;
        }
        if (flags & 64u)
        {
            player = visual_lists.players;
            if (!player)
                return;
            radius = ai.work[2];
            amplitude = ai.work[3];
            yaw = ai.work[4];
            signed_yaw = (uint32)(sint32)(sint16)ai.work[4];
            point = player->pos;
            value = (uint32)(sint32)(sint16)ai.work[0];
            point.words[0] += value * radius;
            value = (uint32)(sint32)(sint16)ai.work[1];
            point.words[2] += value * radius;
            value = RandomRange(128);
            point.words[1] -= (value + 256u) << 12;
            value = RandomRange(4096);
            value = (uint16)rsin(value & 4095u);
            angles.words[0] = (uint16)((sint32)(sint16)value >> 6);
            angles.words[1] = (uint16)yaw;
            angles.words[2] = 0;
            BuildDirectionVector(offset.words, amplitude, angles.words);
            for (axis = 0; axis < 3u; ++axis)
                point.words[axis] += offset.words[axis];
            SetTarget(point.words);
            flags = this->control.flags;
            value = RandomRange(64);
            value = (flags & 32u) ? signed_yaw + 256u + value : signed_yaw - 256u - value;
            ai.work[4] = value & 4095u;
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
            if (!RandomRange(4))
            {
                if (RandomRange(2))
                {
                    if (amplitude < 1600u)
                        ai.work[3] = amplitude + 16u;
                }
                else if (amplitude >= 513u)
                    ai.work[3] = amplitude - 16u;
            }
            return;
        }
        if (swarm_slot >= 8)
        {
            GameDiagnostic("Bat child has no valid swarm slot\n");
            abort();
        }
        parent = swarm_parents[swarm_slot].Get();
        if (!parent)
        {
            Finish();
            return;
        }
        amplitude = ai.work[3];
        yaw = ai.work[4];
        point.words[0] = parent->destination.words[0];
        point.words[1] = parent->destination.words[1];
        point.words[2] = parent->destination.words[2];
        value = RandomRange(4096);
        value = (uint16)rsin(value & 4095u);
        angles.words[0] = (uint16)((sint32)(sint16)value >> 6);
        angles.words[1] = (uint16)yaw;
        angles.words[2] = 0;
        BuildDirectionVector(offset.words, amplitude, angles.words);
        for (axis = 0; axis < 3u; ++axis)
            point.words[axis] += offset.words[axis];
        value = parent->destination.words[1];
        if ((sint32)value < (sint32)point.words[1])
            point.words[1] = value;
        SetTarget(point.words);
        return;
    }
    if (state == 1u)
    {
        phase = ai.phase;
        if (phase == 2u)
        {
            if (!(this->control.flags & 1u))
            {
                ai.state = 0;
                ai.phase = 0;
            }
            return;
        }
        if (phase > 1u || (phase == 1u && (this->control.flags & 1u)))
            return;
        player = visual_lists.players;
        if (!player)
            return;
        point = player->pos;
        if (phase == 0u)
            point.words[1] = this->model.pos.words[1] - 0x100000u;
        else
            point.words[1] -= 0x80000u;
        SetTarget(point.words);
        phase = ai.phase;
        ai.phase = phase + 1u;
        return;
    }
    if (state != 2u)
        return;
    phase = ai.phase;
    if (phase > 1u)
        return;
    if (!phase)
    {
        flags = this->control.flags;
        motion.velocity.words[2] = 0;
        motion.velocity.words[0] = 0;
        this->control.gravity_enabled = 1;
        this->control.flags = flags & 0xFFFFFFFE;
        ground = FindGroundHeight(model.pos.words, 256, 4096);
        if (ground == 0xFFFFFFFF)
            Finish();
        else
        {
            value = (uint32)((sint32)ground >> 16);
            phase = ai.phase;
            ai.work[0] = value;
            ai.phase = phase + 1u;
        }
        /* Failed ground lookup still falls through to the height check */
    }
    height = this->model.pos.words[1];
    value = (uint32)level_environment.water_y;
    if ((sint32)value < (sint32)height)
    {
        this->model.pos.words[1] = value;
        angles.words[0] = (uint16)-1024;
        angles.words[1] = 0;
        angles.words[2] = 0;
        reservation = AllocateEffectStorage(364);
        if (reservation)
        {
            new (reservation) WavingBeam(model.pos.words, angles.words, 512, 32, 0, 128, 128, 128, 5, 0xCCE0187C);
        }
        Finish();
    }
    else
    {
        value = (uint32)(sint32)(sint16)ai.work[0];
        if ((sint32)value < ((sint32)height >> 16))
            Finish();
    }
}

#include <new>
#include <stdlib.h>
#include <string.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "effect_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"
#include "collision.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "angle_math.h"
#include "global.h"
#include "object_damage.h"
#include "game_diagnostics.h"
#include "player.h"
#include "audio_session.h"
#include "bat.h"
#include "record_data.h"
#include "waving_beam.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include "enemy.h"
#include "effect_object.h"
#include "object_reference.h"

static ObjectReference<Bat> swarm_parents[8];

Bat::Bat(const RecordCursor &rec_data, uint16 idx) : Enemy(), swarm_slot(255), swarm_size(0), swarm_sound(0)
{
    RecordDataBorrow rec_borrow;
    uint32 slot;
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
    col_radius = 0;
    control.flags |= 0x40;
    model.flags |= 1;
    lifecycle_flags &= 0xFFEF;
    cursor = ReadPos(rec_data);
    cursor = ReadAngles(cursor);
    script_cursor = cursor;
    rec_idx = idx;
    Initialize();
}

Bat::Bat(const Vector3 *pos, uint8 slot) : Enemy(), swarm_slot(slot), swarm_size(0), swarm_sound(0)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x8004A000, "SLUS_003.73");
    pos_history.saved = *pos;
    model.pos = pos_history.saved;
    script_cursor = 0;
    Initialize();
    control.script_active = 0;
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
    FUNCTION_MARKER(0x8004A090, "SLUS_003.73");
    dst = model.pos;
    ++game_progress.level.enemy_total;
    SetModelBank("bat");
    AddToList(visual_lists.actors);
    kind = 209;
    motion.drag[0] = 3;
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    AllocateBehaviorTable(1, 4);
    ai.parameters[0] = 12;
    ai.parameters[1] = 128;
    ai.parameters[2] = 12;
    SetBehaviorVal(0, 1, 10);
    SetBehaviorVal(0, 2, 80);
    SetBehaviorVal(0, 3, 90);
    StartAnimation(0, 1);
    model.frame = RandomRange(16);
    health = 5;
    control.script_active = 1;
    return 1;
}

uint32 Bat::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8004A1AC, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    if (control.flags & 0x40)
    {
        if (swarm_slot >= 8)
        {
            GameDiagnostic(("Bat swarm parent has no valid slot\n"));
            abort();
        }
        swarm_parents[swarm_slot] = 0;
    }
    StopSoundVoices(swarm_sound);
    swarm_sound = 0;
    Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseStorage();
    return 0;
}

uint32 Bat::Damage(uint32 damage, const Vector3 *input_vector, uint32 kind)
{
    uint32 tick, axis;
    sint16 health;
    Vector3 shifted;
    ModelInstance *player;
    FUNCTION_MARKER(0x8004A25C, "SLUS_003.73");
    tick = control.damage_phase + 1;
    health = this->health;
    control.damage_phase = tick;
    if (health <= 0)
        return tick;
    for (axis = 0; axis < 3; ++axis)
    {
        shifted.comp[axis] = input_vector->comp[axis] >> 3;
    }
    motion.velocity.Add(shifted);
    this->health -= damage;
    if (this->health > 0)
        return SetColor(3, 255, 255, 255);
    player = visual_lists.players;
    if (player)
        static_cast<Player *>(player->owner)->TryPlayKillQuote();
    PlayPositionedSound(77, &model.pos, 0);
    lifecycle_flags &= 0xFFEF;
    if (kind == 5)
    {
        ai.state = 2;
        ai.phase = 0;
        control.script_active = 0;
        return 2;
    }
    return Finish();
}

uint32 Bat::Update()
{
    RecordDataBorrow rec_borrow;
    uint16 flags, delay, scale;
    uint32 distance, sound;
    bool found;
    ModelInstance *node, *player;
    VisualObject *actor;
    ShortVector3 angles;
    FUNCTION_MARKER(0x8004A37C, "SLUS_003.73");
    flags = activation_flags;
    control.prev_activation_flags = flags;
    if (flags & 1)
    {
        activation_flags = flags & 0xFFFE;
        ++control.activation_count;
    }
    if (control.flags & 64)
    {
        if (!(game_clock.tick & 63))
        {
            found = false;
            node = visual_lists.actors;
            while (node)
            {
                actor = static_cast<VisualObject *>(node->owner);
                if (actor != this && actor->kind == 209 && static_cast<Bat *>(actor)->swarm_slot == swarm_slot)
                {
                    found = true;
                    break;
                }
                node = node->next;
            }
            if (!found)
            {
                Finish();
                return 0;
            }
        }
        distance = player_distance;
        if (distance < 2049)
        {
            sound = swarm_sound;
            if (!sound)
            {
                swarm_sound = PlayPositionedSound(76, &model.pos, 0);
            }
            else if (!RandomRange(12))
            {
                sound = swarm_sound;
                UpdatePositionedSoundVoices(sound, &model.pos, 0);
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
    else if (player_distance < 96)
    {
        player = visual_lists.players;
        static_cast<VisualObject *>(player->owner)->Damage(1, &zero_vector, 0);
    }
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
    if (control.flags & 1)
    {
        distance = model.pos.Distance(dst);
        if (distance < 129)
        {
            control.flags &= ~1;
        }
        else
        {
            angles.SetDir(model.pos, dst);
            scale = ai.parameters[0];
            motion.acceleration.SetDir(scale, angles);
            scale = ai.parameters[1];
            UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, angles, scale);
        }
    }
    if (control.gravity_enabled)
        motion.acceleration.Add(world_gravity.vector);
    IntegrateMotion();
    UpdateWorldTransform();
    // The legacy behavior caller discards the original return carrier
    return 0;
}

uint32 Bat::ExecuteCommand(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 val, count;
    GameHeapBlock *child;
    RecordCursor cursor;
    Vector3 point;
    FUNCTION_MARKER(0x8004A690, "SLUS_003.73");
    switch (command)
    {
        case 0x4220:
            cursor = script_cursor.Aligned(4);
            point.x = cursor.Read32();
            point.y = (cursor + 4).Read32();
            point.z = (cursor + 8).Read32();
            point.x = (uint32)point.x << (12);
            point.y = (uint32)point.y << (12);
            point.z = (uint32)point.z << (12);
            script_cursor = cursor + 12;
            SetTarget(&point);
            return 1;
        case 0x4221:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            if (val & 0x2000)
            {
                val = ResolveParameter(val);
            }
            ReadRecPos(&point, val);
            SetTarget(&point);
            return 1;
        case 0x4600:
            cursor = script_cursor;
            count = cursor.Read16();
            script_cursor = cursor + 2;
            swarm_size = count;
            while (count)
            {
                point.x = RandomRange(1024);
                point.y = -(sint32)RandomRange(512);
                point.z = RandomRange(1024) - 512;
                point.x = (uint32)point.x - (512);
                point.Add(model.pos);
                child = AllocateZeroedObject(504);
                if (child)
                {
                    Bat *spawned = new (child) Bat(&point, swarm_slot);
                    spawned->swarm_size = swarm_size;
                    spawned->ai.parameters[0] = ai.parameters[2];
                }
                --count;
            }
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 Bat::SetTarget(const Vector3 *target)
{
    FUNCTION_MARKER(0x8004A874, "SLUS_003.73");
    uint32 flags;
    dst = *target;
    flags = control.flags | 1;
    control.flags = flags;
    return flags;
}

void Bat::UpdateState()
{
    GameHeapBlock *reservation;
    uint16 trig_angle, state, phase, radius, amplitude, yaw;
    uint32 flags, val;
    ModelInstance *player;
    Bat *parent;
    sint32 ground, height, water_y;
    Vector3 point, normal, offset;
    ShortVector3 angles;
    FUNCTION_MARKER(0x8004A8E4, "SLUS_003.73");
    state = ai.state;
    if (state == 0)
    {
        phase = ai.phase;
        if (phase == 0)
        {
            player = visual_lists.players;
            if (!player)
                return;
            if (control.flags & 64)
            {
                point.x = (sint32)((uint32)model.pos.x - player->pos.x) >> 12;
                point.y = 0;
                point.z = (sint32)((uint32)model.pos.z - player->pos.z) >> 12;
                NormalizeShortVector(point, normal);
                ai.work[0] = (uint32)normal.x;
                ai.work[2] = 1024;
                ai.work[3] = 1024;
                ai.work[4] = 0;
                ai.work[1] = (uint32)normal.z;
                val = RandomRange(64);
                flags = control.flags;
                control.flags = (val & 1) ? flags | 32 : flags & 0xFFFFFFDF;
            }
            else
            {
                val = RandomRange(256);
                amplitude = swarm_size >> 3;
                ai.work[3] = (val + 128) * amplitude * amplitude;
                val = RandomRange(4096);
                ai.work[4] = val;
            }
            ++ai.phase;
            return;
        }
        if (phase == 2)
        {
            if (!(control.flags & 1))
                Finish();
            return;
        }
        if (phase != 1)
            return;
        if (control.prev_activation_flags & 1)
        {
            point = model.pos;
            point.y = (uint32)point.y - (0x800000);
            SetTarget(&point);
            ai.phase = 2;
            return;
        }
        if (control.flags & 1)
            return;
        if (!RandomRange(32))
        {
            val = RandomRange(4096);
            trig_angle = val & 4095;
            ai.work[0] = rsin(trig_angle);
            ai.work[1] = rcos(trig_angle);
            return;
        }
        flags = control.flags;
        if (!(flags & 64))
        {
            if (!RandomRange(16) && player_distance < 1024)
            {
                ai.state = 1;
                ai.phase = 0;
                return;
            }
            flags = control.flags;
        }
        if (flags & 64)
        {
            player = visual_lists.players;
            if (!player)
                return;
            radius = ai.work[2];
            amplitude = ai.work[3];
            yaw = ai.work[4];
            point = player->pos;
            point.x = (uint32)point.x + ((sint16)ai.work[0] * radius);
            point.z = (uint32)point.z + ((sint16)ai.work[1] * radius);
            val = RandomRange(128);
            point.y = (uint32)point.y - ((val + 256) << 12);
            val = RandomRange(4096);
            angles.x = rsin(val & 4095) >> 6;
            angles.y = yaw;
            angles.z = 0;
            offset.SetDir(amplitude, angles);
            point.Add(offset);
            SetTarget(&point);
            flags = control.flags;
            val = RandomRange(64);
            val = (flags & 32) ? yaw + 256 + val : yaw - 256 - val;
            ai.work[4] = val & 4095;
            if (!RandomRange(4))
            {
                if (RandomRange(2))
                {
                    if (radius < 2048)
                        ai.work[2] = radius + 32;
                }
                else if (radius >= 513)
                    ai.work[2] = radius - 32;
            }
            if (!RandomRange(4))
            {
                if (RandomRange(2))
                {
                    if (amplitude < 1600)
                        ai.work[3] = amplitude + 16;
                }
                else if (amplitude >= 513)
                    ai.work[3] = amplitude - 16;
            }
            return;
        }
        if (swarm_slot >= 8)
        {
            GameDiagnostic(("Bat child has no valid swarm slot\n"));
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
        point = parent->dst;
        val = RandomRange(4096);
        angles.x = rsin(val & 4095) >> 6;
        angles.y = yaw;
        angles.z = 0;
        offset.SetDir(amplitude, angles);
        point.Add(offset);
        height = parent->dst.y;
        if (height < point.y)
            point.y = height;
        SetTarget(&point);
        return;
    }
    if (state == 1)
    {
        phase = ai.phase;
        if (phase == 2)
        {
            if (!(control.flags & 1))
            {
                ai.state = 0;
                ai.phase = 0;
            }
            return;
        }
        if (phase > 1 || (phase == 1 && (control.flags & 1)))
            return;
        player = visual_lists.players;
        if (!player)
            return;
        point = player->pos;
        if (phase == 0)
            point.y = (uint32)model.pos.y - 0x100000;
        else
            point.y = (uint32)point.y - (0x80000);
        SetTarget(&point);
        ++ai.phase;
        return;
    }
    if (state != 2)
        return;
    phase = ai.phase;
    if (phase > 1)
        return;
    if (!phase)
    {
        flags = control.flags;
        motion.velocity.z = 0;
        motion.velocity.x = 0;
        control.gravity_enabled = 1;
        control.flags = flags & 0xFFFFFFFE;
        ground = CollisionRay::FindGroundHeight(model.pos, 256, 4096);
        if (ground == -1)
            Finish();
        else
        {
            phase = ai.phase;
            ai.work[0] = ground >> 16;
            ai.phase = phase + 1;
        }
        /* Failed ground lookup still falls through to the height check */
    }
    height = model.pos.y;
    water_y = level_environment.water_y;
    if (water_y < height)
    {
        model.pos.y = water_y;
        angles.x = -1024;
        angles.y = 0;
        angles.z = 0;
        reservation = EffectObject::AllocateStorage(364);
        if (reservation)
        {
            new (reservation) WavingBeam(&model.pos, &angles, 512, 32, 0, 128, 128, 128, 5, 0xCCE0187C);
        }
        Finish();
    }
    else if ((sint16)ai.work[0] < (height >> 16))
        Finish();
}

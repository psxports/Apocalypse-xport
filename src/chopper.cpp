#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "geometry_commands.h"
#include "level_session.h"
#include "game_tick.h"

#include "collision.h"

#include "world_gravity.h"

#include "object_initializers.h"

#include "vector_distance.h"

#include "machine_gun.h"
#include "projectile_weaving.h"
#include "visual_object.h"
#include "record_factory.h"
#include "angle_math.h"
#include "model_animation.h"
#include "game_random.h"
#include "game_diagnostics.h"
#include "player.h"
#include "game_panel.h"

#include "chopper.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"
#include "projectile_base.h"
#include "tank.h"
#include "swat.h"
#include "record_data.h"
#include "vector_arithmetic.h"
#include "game_object_allocation.h"
#include "game_heap.h"
#include "audio_voice.h"
#include "audio_session.h"
#include "items.h"

Chopper::Chopper(const RecordCursor &record_data, uint32 index) : Enemy(), fire_timer(0), hover_phase(0), sound_timer(0), rotor_sound(0), sound_distance(0)
{
    RecordDataBorrow record_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80053AB4, "SLUS_003.73");
    aim_pos.words[0] = aim_pos.words[1] = aim_pos.words[2] = 0;
    aim_offset.words[0] = aim_offset.words[1] = aim_offset.words[2] = 0;

    cursor = ReadPosition(record_data);
    cursor = ReadAngles(cursor);
    script_cursor = cursor;
    this->control.script_active = 1;
    this->record_idx = index;
    Init();
}

uint32 Chopper::Init()
{
    GameHeapBlock *child;
    uint32 value;
    FUNCTION_MARKER(0x80053BA8, "SLUS_003.73");
    game_progress.level.enemy_total = game_progress.level.enemy_total + 1u;
    SetModelBank("Chopper");
    AddToList(visual_lists.actors);
    this->kind = 202;
    motion.drag[0] = 3;
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    AllocateBehaviorTable(4, 8);
    value = MeasurePositionDistance(model.pos.words, visual_lists.cameras->pos.words);
    this->sound_distance = value;
    ai.state = 2;
    this->control.gravity_enabled = 0;
    ai.phase = 0;
    this->control.hierarchy_ready = 0;
    this->health = (sint16)(uint16)(100u);
    child = AllocateZeroedHeapBlock(88);
    MachineGun *created = 0;
    if (child)
    {
        created = new (child) MachineGun(&visual_lists.players);
    }
    weapon = created;
    if (child)
    {
        weapon->damage = 5;
        weapon->Configure(2, 2, 10, 8096, 0, 11, 128);
    }
    ai.parameters[0] = 16;
    ai.parameters[1] = 48;
    SetBehaviorValue(0, 0, 0);
    SetBehaviorValue(0, 1, 0);
    SetBehaviorValue(0, 2, 0);
    SetBehaviorValue(1, 0, 0);
    SetBehaviorValue(1, 1, 20);
    SetBehaviorValue(1, 2, 30);
    SetBehaviorValue(1, 3, 30);
    SetBehaviorValue(2, 0, 0);
    SetBehaviorValue(2, 1, 20);
    SetBehaviorValue(2, 2, 30);
    SetBehaviorValue(2, 3, 512);
    SetBehaviorValue(2, 4, 8);
    SetBehaviorValue(2, 5, 300);
    SetBehaviorValue(2, 6, 256);
    SetBehaviorValue(2, 7, 128);
    SetBehaviorValue(3, 0, 0);
    SetBehaviorValue(3, 1, 90);
    SetBehaviorValue(3, 2, 60);
    value = PlayPositionedSound(84, model.pos.words, 0);
    this->rotor_sound = value;
    value = RandomRange(8) + 1u;
    this->sound_timer = value;
    return value;
}

Chopper::~Chopper()
{
    if (weapon)
        weapon->Destroy(3);
    if (rotor_sound)
        StopSoundVoices(rotor_sound);
}

uint32 Chopper::Destroy(uint32 reason)
{
    uint32 value;
    FUNCTION_MARKER(0x80053E6C, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    if (weapon)
        weapon->Destroy(3);
    weapon = 0;
    Enemy::FreeBehaviorTable();
    if (this->control.flags & 128)
    {
        ClearTrackedObjectHealth();
        value = this->control.flags;
        this->control.flags = value & 0xFFFFFF7F;
    }
    value = this->rotor_sound;
    if (value)
        StopSoundVoices(value);
    rotor_sound = 0;
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : 0;
}

void Chopper::SetParameters(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80053F48, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x2130)
    {
        Enemy::ReadParameters(command);
        return;
    }
    cursor = script_cursor;
    script_cursor = cursor + 2;
    value = (cursor + 2).Read16();
    cursor += 4;
    script_cursor = cursor;
    ai.work[4] = value;
    value = cursor.Read16();
    cursor += 2;
    script_cursor = cursor;
    ai.work[5] = value;
}

void Chopper::UpdateRoll(uint32 difference)
{
    if ((sint16)difference < -2048)
        difference += 4096u;
    if ((sint16)difference >= 2049)
        difference -= 4096u;
    if ((sint16)difference)
        motion.angular_acceleration.words[2] = (uint32)((sint32)(sint16)difference >> 3);
    else
        motion.angular_velocity.words[2] = 0;
}

uint32 Chopper::SquaredDistance(const Vector3 &vector)
{
    uint32 axis, sum;
    for (axis = 0; axis < 3u; ++axis)
        xport_gte_write_data(9u + axis, (uint32)((sint32)vector[axis] >> 12));
    ExecuteGeometryCommand(0xA00428u);
    sum = xport_gte_read_data(25u);
    sum += xport_gte_read_data(26u);
    sum += xport_gte_read_data(27u);
    return sum;
}

uint32 Chopper::Update()
{
    RecordDataBorrow record_borrow;
    uint32 x, y, z, value, flags, target, tail, scale, pointer, table, adjustment, slot, distance;
    Vector3 vector;
    ShortVector3 angles;
    sint32 difference;
    FUNCTION_MARKER(0x80053FA8, "SLUS_003.73");
    x = (uint32)zero_vector[0];
    y = (uint32)zero_vector[1];
    z = (uint32)zero_vector[2];
    motion.acceleration.words[0] = x;
    motion.acceleration.words[1] = y;
    motion.acceleration.words[2] = z;
    flags = this->activation_flags;
    this->control.prev_activation_flags = flags;
    if (flags & 1u)
    {
        value = (uint32)this->control.activation_count;
        this->activation_flags = flags & 0xFFFEu;
        this->control.activation_count = value + 1u;
    }
    if (!(uint32)this->control.hierarchy_ready)
    {
        InitializeModelAnimation(ChopperHierarchy);
        value = this->model.flags;
        this->control.hierarchy_ready = 1;
        this->model.flags = value | 4u;
        SetAnimationRange(0u, 0u, 0xFFFFFFFF);
    }
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
    flags = this->control.flags;
    if (flags & 16u)
    {
        ModelInstance *subject = tracked_target.GetObject();
        if (subject)
        {
            VisualObject *visual = static_cast<VisualObject *>(subject->owner);
            const uint32 *pos = visual->kind == 50 ? visual->surface.pos.words : subject->pos.words;
            aim_pos.words[0] = pos[0];
            aim_pos.words[1] = pos[1];
            aim_pos.words[2] = pos[2];
        }
        else
            control.flags &= ~16;
        goto TrackTarget;
    }
    if (flags & 8u)
    {
        AddVectors(vector.words, model.pos.words, this->aim_offset.words);
        this->aim_pos = vector;
        goto TrackTarget;
    }
    if (flags & 4u)
    {
    TrackTarget:
        Aim();
        if (!(this->control.flags & 1u))
            goto LevelRoll;
        CalculateDirectionAngles(angles.words, this->model.pos.words, this->destination.words);
        goto MoveTarget;
    }
    if (!(flags & 1u))
        goto LevelRoll;
    CalculateDirectionAngles(angles.words, this->model.pos.words, this->destination.words);
    SubtractVectors(vector.words, this->destination.words, this->model.pos.words);
    distance = MeasureVectorLength(vector.words);
    value = ai.parameters[1];
    scale = TraceDivide(value * 170u, distance);
    if ((sint32)scale < -256)
        scale = (uint32)-256;
    else if ((sint32)scale >= 257)
        scale = 256;
    else if (scale - 1u < 3u)
        scale = 4;
    else if ((sint32)scale < 0 && (sint32)scale >= -3)
        scale = (uint32)-4;
    angles.components[0] = 0;
    UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, angles, scale);
    this->model.angles.words[0] = 0;
MoveTarget:
    if (!(this->control.flags & 1u))
        goto LevelRoll;
    SubtractVectors(vector.words, this->model.pos.words, this->destination.words);
    if ((sint32)SquaredDistance(vector) <= 65536)
    {
        flags = this->control.flags;
        this->control.flags = flags & 0xFFFFFFFE;
        goto UpdateModel;
    }
    CalculateDirectionAngles(angles.words, this->model.pos.words, this->destination.words);
    value = ai.parameters[0];
    BuildDirectionVector(motion.acceleration.words, value, angles.words);
    flags = this->control.flags;
    if (flags & 32u)
        value = ((uint16)angles.components[1] - this->model.angles.words[1]) & 4095u;
    else
        value = 0u;
    if ((sint16)value >= 2049)
        value -= 4096u;
    difference = (sint16)value;
    if (difference > 1024)
        difference = 1024;
    else if (difference < -1024)
        difference = -1024;
    value = (uint32)((sint32)(0u - (uint32)difference) >> 1);
    tail = this->model.angles.words[2];
    value = (value & 4095u) - tail;
    UpdateRoll(value);
    goto UpdateModel;
LevelRoll:
    value = 0u - this->model.angles.words[2];
    UpdateRoll(value);
UpdateModel:
    if ((uint32)this->control.gravity_enabled)
        AddVectors(motion.acceleration.words, world_gravity.vector.words);
    IntegrateMotion();
    value = GetModelJointAngles(1).rotation.words[1];
    GetModelJointAngles(1).rotation.words[1] = (uint16)((value + 512) & 4095);
    value = GetModelJointAngles(2).rotation.words[1];
    GetModelJointAngles(2).rotation.words[1] = (uint16)((value + 512) & 4095);
    value = GetModelJointAngles(3).rotation.words[1];
    GetModelJointAngles(3).rotation.words[1] = (uint16)((value + 512) & 4095);
    UpdateWorldTransform();
    BuildModelPose(*this);
    if (GetBehaviorValue(0, 0) & 65535u)
        SetFireDuration(1u);
    if (GetBehaviorValue(1, 0) & 65535u)
    {
        value = GetBehaviorValue(1, 3) & 65535u;
        if (!RandomRange(value))
            FireProjectile();
    }
    if (GetBehaviorValue(2, 0) & 65535u)
    {
        value = GetBehaviorValue(2, 2) & 65535u;
        if (!RandomRange(value))
            SpawnDebris();
    }
    if (GetBehaviorValue(3, 0) & 65535u)
    {
        value = GetBehaviorValue(3, 2) & 65535u;
        if (!RandomRange(value))
            SpawnEnemy(0xFFFFFFFF);
    }
    value = this->fire_timer;
    if (!weapon)
    {
        GameDiagnostic("Chopper machine gun is missing\n");
        abort();
    }
    if (value)
        weapon->Fire(model.pos.words, model.angles.components);
    else
        weapon->Update(model.pos.words, 0);
    if (value)
    {
        value = this->fire_timer;
        this->fire_timer = value - 1u;
    }
    if (this->control.flags & 64u)
    {
        value = (uint32)this->hover_phase;
        tail = (uint32)this->hover_phase;
        x = (uint32)rsin(value & 4095u);
        y = this->model.pos.words[1];
        this->hover_phase = (tail + 64u) & 4095u;
        this->model.pos.words[1] = y + (x << 4);
    }
    if (this->rotor_sound)
    {
        value = (uint32)this->sound_timer;
        if (value)
            this->sound_timer = value - 1u;
        else
        {
            distance = MeasurePositionDistance(model.pos.words, visual_lists.cameras->pos.words);
            pointer = this->rotor_sound;
            value = this->sound_distance;
            this->sound_timer = 8u;
            UpdatePositionedSoundVoices(pointer, model.pos.words, (sint32)(distance - value));
            this->sound_distance = distance;
        }
    }
    else if (!(game_clock.tick & 7u))
    {
        value = PlayPositionedSound(84, model.pos.words, 0);
        this->rotor_sound = value;
    }
    // The legacy behavior caller discards the original return carrier
    return 0;
}

uint32 Chopper::Damage(uint32 damage, const sint32 *, uint32)
{
    uint32 count, remaining, flags, state;
    ModelInstance *hero;
    sint32 health;
    FUNCTION_MARKER(0x80054714, "SLUS_003.73");
    count = (uint32)this->control.damage_phase;
    this->control.damage_phase = count + 1;
    health = this->health;
    remaining = (uint16)this->health - damage;
    if (health <= 0)
        return 1;
    this->health = (sint16)(uint16)(remaining);
    if ((sint32)(remaining << 16) > 0)
        SetColor(3, 255, 255, 255);
    else
    {
        hero = visual_lists.players;
        if (hero)
            TryPlayPlayerKillQuote(hero->pos.words);
        flags = this->lifecycle_flags;
        state = ai.state;
        this->lifecycle_flags = flags & 0xFFEF;
        if (state)
        {
            ai.state = 2;
            ai.phase = 0;
        }
        else
            ai.phase = 5;
        this->control.script_active = 0;
    }
    return 1;
}

void Chopper::SpawnRandomEffect(uint32 kind, uint32 debris)
{
    uint32 x, y, z, shift = 12;
    Vector3 position;
    x = RandomRange(256);
    y = RandomRange(256);
    z = RandomRange(256);
    position.words[0] = x - 128;
    position.words[1] = y - 128;
    position.words[2] = z - 128;
    ShiftVectorLeft(position.words, shift);
    AddVectors(position.words, model.pos.words);
    HandleExplosionProximity(position.words, kind);
    if (debris)
        SpawnExplosionRing(position.words, 400, 240, 160, 40, 5, 0, 100);
}

void Chopper::SetDestinationRelative(uint32 x)
{
    uint32 offset;
    Vector3 position;
    position.words[0] = (uint32)(sint32)(sint16)x << 16;
    ModelInstance *hero = visual_lists.players;
    if (hero)
    {
        offset = (uint32)(sint32)(sint16)ai.work[4] << 12;
        position.words[1] = hero->pos.words[1] - offset;
        offset = (uint32)(sint32)(sint16)ai.work[3] << 12;
        position.words[2] = hero->pos.words[2] - offset;
    }
    else
    {
        position.words[1] = model.pos.words[1];
        position.words[2] = model.pos.words[2];
    }
    SetDestination(position.words);
    ++ai.phase;
}

void Chopper::TrackHero()
{
    ModelInstance *hero = visual_lists.players;
    if (!hero)
        return;
    uint32 y = hero->pos.words[1] - model.pos.words[1];
    uint32 z = hero->pos.words[2] - model.pos.words[2];
    SetRelativeTarget(0, (uint32)((sint32)y >> 12), (uint32)((sint32)z >> 12));
}

void Chopper::UpdateState()
{
    RecordDataBorrow record_borrow;
    uint32 state, step, flags, value, index, best, delta, minimum;
    ModelInstance *hero;
    RecordCursor record;
    uint32 distances[3], shift, ground, x, y, z;
    Vector3 pos[3];
    sint32 timer;
    FUNCTION_MARKER(0x800547D0, "SLUS_003.73");
    state = ai.state;
    if (state > 4u)
        return;
    if (state == 1u && ((uint32)this->control.prev_activation_flags & 1u))
    {
        pos[0] = this->model.pos;
        pos[0].words[1] -= 0x400000u;
        SetDestination(pos[0].words);
        ai.phase = 6;
    }
    step = ai.phase;
    if (state == 0u || state == 3u)
    {
        if (step == 0u)
        {
            TrackObjectHealth(*this, 64, 32, 4096);
            if (state == 0u)
            {
                flags = this->control.flags;
                hero = visual_lists.players;
            }
            else
            {
                hero = visual_lists.players;
                flags = this->control.flags;
                ai.work[1] = 0;
            }
            this->control.flags = flags | 128u;
            SetTargetObject(visual_lists.players);
            record = GetRecordPayload(this->record_idx);
            value = (record + 2u).Read16();
            ai.work[0] = value;
            SetDestinationRecord(value);
            step = ai.phase;
            ai.work[3] = 4096;
            ai.phase = step + 1u;
            return;
        }
        if (step == 1u)
        {
            if (this->control.flags & 1u)
                return;
            if (state == 3u)
            {
                value = (uint32)(sint32)(sint16)ai.work[1];
                ai.work[1] = value + 1u;
            }
            record = GetRecordPayload((uint32)(sint32)(sint16)ai.work[0]);
            value = (record + 2u).Read16();
            ai.work[0] = value;
            if (state == 0u)
                SetDestinationRecord(value);
            else if ((sint16)ai.work[1] < 3)
            {
                SetDestinationRecord(value);
                return;
            }
            else
            {
                ReadRecordPosition(pos[0].words, (uint32)(sint32)(sint16)value);
                ai.work[2] = pos[0].words[0] >> 16;
                ClearTarget();
                SetTargetRecord(ai.work[0]);
            }
            value = ai.phase;
            ai.phase = value + 1u;
            return;
        }
        if (state == 3u)
        {
            if (step == 2u)
            {
                if (!(this->control.flags & 1u))
                {
                    pos[0] = this->model.pos;
                    value = ai.parameters[3];
                    x = (uint32)(sint32)(sint16)ai.work[2] << 16;
                    y = RandomRange(value);
                    value = ai.parameters[3];
                    pos[0].words[0] = x - (((value >> 1) - y) << 12);
                    pos[0].words[2] -= 0x400000u;
                    SetDestination(pos[0].words);
                }
                ReadRecordPosition(pos[0].words, (uint32)(sint32)(sint16)ai.work[0]);
                value = MeasureEuclideanDistance(model.pos.words, pos[0].words);
                if (value >= ai.parameters[2])
                    return;
                FireProjectile();
                ::SpawnRecord((uint32)(sint32)(sint16)ai.work[0]);
                record = GetRecordPayload((uint32)(sint32)(sint16)ai.work[0]);
                if (!record.Read16())
                {
                    value = ai.phase;
                    ai.phase = value + 1u;
                }
                else
                {
                    value = (record + 2u).Read16();
                    ai.work[0] = value;
                    SetTargetRecord(value);
                }
            }
            else if (step == 3u)
            {
                ClearTarget();
                SetTargetObject(visual_lists.players);
                value = (uint16)(this->model.pos.words[0] >> 16);
                ai.work[0] = value;
                SetBehaviorValue(0, 0, 1);
                SetBehaviorValue(1, 0, 1);
                value = ai.phase;
                ai.parameters[0] = 4;
                ai.phase = value + 1u;
            }
            else if (step == 4u && !(this->control.flags & 1u))
            {
                pos[0].words[1] = this->model.pos.words[1];
                x = (uint32)(sint32)(sint16)ai.work[0] << 16;
                value = RandomRange(4096);
                pos[0].words[0] = x + ((value - 2048u) << 12);
                pos[0].words[2] = this->model.pos.words[2];
                SetDestination(pos[0].words);
            }
            return;
        }
        switch (step)
        {
            case 2:
                if (this->control.flags & 1u)
                {
                    if ((sint32)swat_population.count < 4)
                    {
                        value = GetBehaviorValue(3, 2) & 65535u;
                        if (!RandomRange(value))
                        {
                            Swat *spawned_swat = SpawnEnemy(0xFFFFFFFF);
                            if (spawned_swat)
                                spawned_swat->SetMode(0);
                        }
                    }
                    return;
                }
                SetBehaviorValue(1, 0, 1);
                SetBehaviorValue(2, 0, 1);
                ReadRecordPosition(pos[0].words, (uint32)(sint32)(sint16)ai.work[0]);
                record = GetRecordPayload((uint32)(sint32)(sint16)ai.work[0]);
                value = (record + 2u).Read16();
                ai.work[1] = value;
                ReadRecordPosition(pos[1].words, (uint32)(sint32)(sint16)value);
                value = (record + 4u).Read16();
                ai.work[2] = value;
                ReadRecordPosition(pos[2].words, (uint32)(sint32)(sint16)value);
                if (!RandomRange(3))
                {
                    value = RandomRange(4096);
                    ai.work[3] = value + 2048u;
                }
                hero = visual_lists.players;
                if (hero)
                {
                    distances[0] = MeasurePositionDistance(pos[0].words, hero->pos.words);
                    hero = visual_lists.players;
                    distances[1] = MeasurePositionDistance(pos[1].words, hero->pos.words);
                    hero = visual_lists.players;
                    distances[2] = MeasurePositionDistance(pos[2].words, hero->pos.words);
                    minimum = 0xFFFFFF;
                    best = 0xFFFFFFFF;
                    for (index = 0; index < 3u; ++index)
                    {
                        value = (uint32)(sint32)(sint16)ai.work[3];
                        delta = distances[index] - value;
                        value = (uint32)((sint32)delta >> 31);
                        delta = (delta ^ value) - value;
                        if ((sint32)delta < (sint32)minimum)
                        {
                            minimum = delta;
                            best = index;
                        }
                    }
                    if (best + 1u >= 2u)
                    {
                        value = this->control.flags;
                        this->control.flags = value | 32u;
                        SetDestinationRecord(ai.work[best & 255u]);
                        value = ai.work[best & 255u];
                        ai.work[0] = value;
                        return;
                    }
                }
                value = RandomRange(16);
                ai.work[1] = value + 16u;
                ai.phase = 3;
                return;
            case 3:
                Hover();
                value = ai.phase;
                ai.phase = value + 1u;
                return;
            case 4:
                timer = (sint16)ai.work[1];
                if (timer)
                {
                    SetBehaviorValue(2, 0, 0);
                    ai.work[1] = (uint32)timer - 1u;
                }
                else
                    ai.phase = 2;
                return;
            case 5:
                SetBehaviorValue(1, 0, 0);
                SetBehaviorValue(2, 0, 0);
                ::SpawnRecord((uint32)(sint32)(sint16)ai.work[5]);
                ClearTarget();
                value = this->control.flags;
                index = ai.work[4];
                this->control.flags = value | 32u;
                SetDestinationRecord(index);
                value = ai.phase;
                ai.phase = value + 1u;
                return;
            case 6:
                if (!(this->control.flags & 1u))
                    Finish();
                else
                    SpawnRandomEffect(0u, 0u);
                return;
            default:
                return;
        }
    }
    if (state == 1u)
    {
        if (step == 0u)
        {
            record = GetRecordPayload(this->record_idx);
            ReadRecordPosition(pos[0].words, (record + 2u).Read16());
            ReadRecordPosition(pos[1].words, (record + 4u).Read16());
            ai.work[4] = 640;
            x = pos[0].words[0];
            y = pos[1].words[0];
            if ((sint32)x < (sint32)y)
            {
                ai.work[0] = x >> 16;
                z = x + (uint32)((sint32)(y - x) >> 1);
                ai.work[1] = z >> 16;
                ai.work[2] = y >> 16;
            }
            else
            {
                ai.work[0] = y >> 16;
                z = y + (uint32)((sint32)(x - y) >> 1);
                ai.work[1] = z >> 16;
                ai.work[2] = x >> 16;
            }
            pos[0].words[0] = this->model.pos.words[0];
            pos[0].words[1] = this->model.pos.words[1] - 7372800u;
            pos[0].words[2] = this->model.pos.words[2];
            SetDestination(pos[0].words);
            value = ai.phase;
            ai.phase = value + 1u;
        }
        else if (step >= 1u && step <= 4u)
        {
            flags = this->control.flags;
            if (flags & 1u)
                TrackHero();
            else
            {
                if (step == 1u)
                {
                    this->control.flags = flags | 32u;
                    SetBehaviorValue(0, 0, 1);
                }
                index = step == 1u ? 2 : step == 3u ? 0 : 1;
                SetDestinationRelative(ai.work[index]);
            }
        }
        else if (step == 5u)
        {
            if (this->control.flags & 1u)
                TrackHero();
            else
                ai.phase = 1;
        }
        else if (step == 6u && !(this->control.flags & 1u))
            Finish();
        return;
    }
    if (state == 2u)
    {
        if (step == 0u)
        {
            motion.velocity.words[2] = 0;
            motion.velocity.words[1] = 0;
            motion.velocity.words[0] = 0;
            motion.acceleration.words[2] = 0;
            motion.acceleration.words[1] = 0;
            motion.angular_velocity.words[0] = 64;
            motion.acceleration.words[0] = 0;
            motion.angular_velocity.words[1] = 0;
            motion.angular_velocity.words[2] = 0;
            flags = this->control.flags;
            this->control.flags = flags & 0xFFFFFFE2;
            ground = FindGroundHeight(model.pos.words, 0, 4096);
            if (ground == 0xFFFFFFFF)
                ai.phase = 2;
            else
            {
                ai.work[0] = (uint32)((sint32)ground >> 13);
                this->control.gravity_enabled = 1;
                ai.phase = 1;
            }
        }
        else if (step == 1u)
        {
            ground = (uint32)(sint32)(sint16)ai.work[0] << 13;
            y = this->model.pos.words[1];
            if ((sint32)ground < (sint32)y)
            {
                this->model.pos.words[1] = ground;
                this->control.gravity_enabled = 0;
                ai.phase = 2;
            }
            else if (!IsGameHeapUnderPressure())
                SpawnRandomEffect(0u, 0u);
        }
        else if (step == 2u)
        {
            for (index = 0; index < 3u; ++index)
                if (!IsGameHeapUnderPressure())
                    SpawnRandomEffect(1u, 1u);
            Finish();
        }
        return;
    }
    if (step)
        return;
    motion.velocity.words[2] = 0;
    motion.velocity.words[1] = 0;
    motion.velocity.words[0] = 0;
    motion.acceleration.words[2] = 0;
    motion.acceleration.words[1] = 0;
    flags = this->control.flags;
    motion.acceleration.words[0] = 0;
    this->control.flags = flags & 0xFFFFFFE2;
    for (index = 0; index < 3u; ++index)
        SpawnRandomEffect(1u, 1u);
    Finish();
}

uint32 Chopper::ResolveCommandValue(uint32 value)
{
    return (value & 0x2000) ? ResolveParameter(value) : value;
}

uint32 Chopper::ExecuteCommand(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value, shift = 12;
    Vector3 position;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8005568C, "SLUS_003.73");
    command &= 0xFFFFu;
    switch (command)
    {
        case 0x4220:
        case 0x4222:
            cursor = AlignScriptCursor(script_cursor);
            position.words[0] = cursor.Read32();
            position.words[1] = (cursor + 4u).Read32();
            position.words[2] = (cursor + 8u).Read32();
            cursor += 12u;
            ShiftVectorLeft(position.words, shift);
            if (command == 0x4222u)
                AddVectors(position.words, this->model.pos.words);
            script_cursor = cursor;
            SetDestination(position.words);
            return 1;
        case 0x4221:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2u;
            value = ResolveCommandValue(value);
            ReadRecordPosition(position.words, value & 0xFFFF);
            SetDestination(position.words);
            return 1;
        case 0x4262:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2u;
            if (value == 0u)
                SetFireDuration(1u);
            else if (value == 1u)
                FireProjectile();
            else if (value == 2u)
                SpawnDebris();
            return 1;
        case 0x4402:
        case 0x4403:
            cursor = script_cursor;
            value = (uint32)(sint32)(sint16)cursor.Read16();
            if (command == 0x4402u)
                SpawnTank(value);
            else
                SpawnEnemy(value);
            cursor = script_cursor;
            script_cursor = cursor + 2u;
            return 1;
        case 0x4404:
            value = this->control.flags;
            this->control.flags = value | 32u;
            return 1;
        case 0x4405:
            value = this->control.flags;
            this->control.flags = value & 0xFFFFFFDF;
            return 1;
        case 0x4406:
        case 0x4407:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2u;
            if (!value)
                ClearTarget();
            else if (command == 0x4406u)
                SetTargetObject(visual_lists.players);
            else
            {
                value = ResolveCommandValue(value);
                SetTargetRecord(value & 0xFFFFu);
            }
            return 1;
        case 0x4408:
            Hover();
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 Chopper::SetDestination(const uint32 *position)
{
    FUNCTION_MARKER(0x80055A1C, "SLUS_003.73");
    const uint32 x = position[0];
    const uint32 y = position[1];
    const uint32 z = position[2];
    destination.words[0] = x;
    destination.words[1] = y;
    destination.words[2] = z;
    control.flags |= 1;
    return control.flags;
}

uint32 Chopper::SetDestinationRecord(uint32 index)
{
    RecordDataBorrow record_borrow;
    uint32 flags;
    FUNCTION_MARKER(0x80055A48, "SLUS_003.73");
    ReadRecordPosition(this->destination.words, index & 0xFFFFu);
    flags = (this->control.flags | 1u) & 0xFFFFFFBF;
    this->control.flags = flags;
    return flags;
}

uint32 Chopper::Hover()
{
    uint32 flags;
    FUNCTION_MARKER(0x80055A88, "SLUS_003.73");
    motion.velocity.words[2] = 0;
    motion.velocity.words[1] = 0;
    motion.velocity.words[0] = 0;
    motion.acceleration.words[2] = 0;
    motion.acceleration.words[1] = 0;
    flags = this->control.flags;
    motion.acceleration.words[0] = 0;
    flags = (flags & 0xFFFFFFDF) | 64u;
    this->control.flags = flags;
    return flags;
}

uint32 Chopper::Aim()
{
    FUNCTION_MARKER(0x80055AC0, "SLUS_003.73");
    ShortVector3 desired;
    CalculateDirectionAngles(desired.words, model.pos.words, aim_pos.words);
    return UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, ai.parameters[1]);
}

uint32 Chopper::SetTargetRecord(uint32 index)
{
    RecordDataBorrow record_borrow;
    uint32 flags;
    FUNCTION_MARKER(0x80055B48, "SLUS_003.73");
    ReadRecordPosition(this->aim_pos.words, index & 0xFFFFu);
    flags = this->control.flags | 4u;
    this->control.flags = flags;
    return flags;
}

uint32 Chopper::SetRelativeTarget(uint32 x, uint32 y, uint32 z)
{
    uint32 flags;
    FUNCTION_MARKER(0x80055B84, "SLUS_003.73");
    this->aim_offset.words[0] = x << 12;
    this->aim_offset.words[1] = y << 12;
    this->aim_offset.words[2] = z << 12;
    flags = this->control.flags | 8u;
    this->control.flags = flags;
    return flags;
}

uint32 Chopper::SetTargetObject(ModelInstance *target)
{
    FUNCTION_MARKER(0x80055BB4, "SLUS_003.73");
    uint32 result = static_cast<VisualObject *>(target->owner)->IsFinished();
    if (result)
        return result;
    tracked_target.SetObject(target);
    result = control.flags | 16;
    control.flags = result;
    return result;
}

uint32 Chopper::ClearTarget()
{
    uint32 result = this->control.flags & 0xFFFFFFE3;
    FUNCTION_MARKER(0x80055C00, "SLUS_003.73");
    this->control.flags = result;
    return result;
}

uint32 Chopper::FireProjectile()
{
    Vector3 position;
    GameHeapBlock *child;
    uint32 value;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80055C14, "SLUS_003.73");
    position = this->model.pos;
    angles.words[0] = (uint16)this->model.angles.words[0];
    angles.words[1] = (uint16)this->model.angles.words[1];
    angles.words[2] = 0;
    WeavingProjectile *projectile = 0;
    child = AllocateZeroedObject(440);
    if (child)
    {
        projectile = new (child) WeavingProjectile(position.words, angles.components, 0, 6, 128, &visual_lists.players, zero_vector.words);
    }
    value = GetBehaviorValue(1, 1);
    value = ScaleDamageForDifficulty(value & 65535);
    if (!projectile)
    {
        GameDiagnostic("Chopper projectile allocation failed\n");
        abort();
    }
    projectile->SetDamage(value);
    projectile->SetBlastRadius(256);
    return 256;
}

uint32 Chopper::SetFireDuration(uint32 seconds)
{
    uint32 result = seconds ? seconds * 30u : 0xFFFFFFFF;
    FUNCTION_MARKER(0x80055CEC, "SLUS_003.73");
    this->fire_timer = result;
    return result;
}

Items *Chopper::SpawnDebris()
{
    ObjectBorrow object_borrow;
    GameHeapBlock *child;
    uint32 damage, arm_radius, blast_radius, fuse_ticks, life_ticks, hover_height;
    FUNCTION_MARKER(0x80055D10, "SLUS_003.73");
    child = AllocateZeroedObject(328);
    if (!child)
        return 0;
    damage = GetBehaviorValue(2, 1) & 65535;
    arm_radius = GetBehaviorValue(2, 3) & 65535;
    blast_radius = GetBehaviorValue(2, 6) & 65535;
    fuse_ticks = GetBehaviorValue(2, 4) & 65535;
    life_ticks = GetBehaviorValue(2, 5) & 65535;
    hover_height = GetBehaviorValue(2, 7) & 65535;
    return new (child) Items(model.pos.words, damage, arm_radius, blast_radius, fuse_ticks, life_ticks, hover_height);
}

Swat *Chopper::SpawnEnemy(uint32 index)
{
    RecordDataBorrow record_borrow;
    GameHeapBlock *child;
    Swat *created = 0;
    FUNCTION_MARKER(0x80055E0C, "SLUS_003.73");
    child = AllocateZeroedObject(624);
    if (child)
    {
        RecordCursor script = (sint32)index < 0 ? RecordCursor() : GetScriptLabel(index);
        created = new (child) Swat(model.pos.words, script);
    }
    if (!created)
    {
        GameDiagnostic("Chopper child allocation failed before control setup\n");
        abort();
    }
    created->control.flags |= 512;
    return created;
}

Tank *Chopper::SpawnTank(uint32 index)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x80055E90, "SLUS_003.73");
    GameHeapBlock *child = AllocateZeroedObject(576);
    if (!child)
        return 0;
    RecordCursor script = (sint32)index < 0 ? RecordCursor() : GetScriptLabel(index);
    return new (child) Tank(model.pos.words, script);
}

uint32 Chopper::TraceDivide(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1u : 0xFFFFFFFF;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

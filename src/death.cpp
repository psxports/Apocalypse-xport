#include <new>
#include <stdio.h>
#include <stdlib.h>

#include "game_heap.h"
#include "record_scripts.h"

#include "collision.h"

#include "pickup_factory.h"

#include "world_gravity.h"

#include "object_initializers.h"

#include "camera.h"
#include "player.h"
#include "projectile_line.h"
#include "enemy_list.h"
#include "model_resources.h"
#include "screen_flash.h"
#include "visual_object.h"
#include "angle_math.h"
#include "object_damage.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "model_animation.h"
#include "model_attachment.h"
#include "game_random.h"
#include "game_diagnostics.h"
#include "object_force.h"
#include "level_session.h"
#include "projectile_trail.h"
#include "game_panel.h"

#include "death.h"
#include "swat.h"
#include "game_panel.h"
#include "death_effects.h"
#include "ring_burst_effect.h"
#include "spawn_points.h"
#include "record_data.h"
#include "particle_debris.h"
#include "beam_endpoint.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include "audio_session.h"
#include "effect_lifecycle.h"

// Original Death summon scripts at 0x800A6C18 and 0x800A6C28
static const uint16 spawn_swat_script[] = {0x4206, 0x70, 0x2100, 20, 0x2125, 0, 32, 0x4100};
static const uint16 summon_swat_script[] = {0x4206, 0x70, 0x2100, 20, 0x2125, 0, 32, 0x2114, 0x30, 0x4100};

Death::Death(const RecordCursor &record_data, uint32 index) : Enemy(), pickup_timer(0), spawn_sound(0), pulse_angle(0)
{
    RecordDataBorrow record_borrow;
    uint32 roll;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80047A9C, "SLUS_003.73");
    aim_pos.words[0] = aim_pos.words[1] = aim_pos.words[2] = 0;

    cursor = ReadPosition(record_data);
    this->model.angles.words[0] = cursor.Read16();
    this->model.angles.words[1] = (cursor + 2).Read16();
    roll = (cursor + 4).Read16();
    script_cursor = cursor + 6;
    this->control.script_active = 1;
    this->record_idx = index;
    this->model.angles.words[2] = roll;
    Configure();
    cursor = script_cursor;
    ExecuteScript(RecordCursor(cursor));
}

uint32 Death::Configure()
{
    uint32 flags, behavior, damage, height, product;
    FUNCTION_MARKER(0x80047B34, "SLUS_003.73");
    SetModelBank("death");
    motion.drag[0] = 31;
    motion.drag[1] = 3;
    motion.drag[2] = 31;
    AddToList(visual_lists.actors);
    flags = this->model.flags;
    behavior = this->control.flags;
    this->model.flags = flags | 2048;
    this->control.flags = behavior | 64;
    StartAnimation(0, 1);
    AllocateBehaviorTable(2, 8);
    SetBehaviorValue(0, 0, 0);
    SetBehaviorValue(0, 1, 10);
    SetBehaviorValue(1, 0, 0);
    damage = ScaleDamageForDifficulty(10);
    SetBehaviorValue(1, 1, damage & 65535);
    SetBehaviorValue(1, 2, 96);
    SetBehaviorValue(1, 3, 30);
    SetBehaviorValue(1, 4, 45);
    ai.parameters[0] = 16;
    ai.parameters[1] = 32;
    // The original stores type221 although the factory branch is222
    this->kind = 221;
    contact_strength = 128;
    this->ground_offset = 128;
    height = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
    this->health = (sint16)(uint16)(1200);
    ai.parameters[9] = (ai.parameters[9] & 0xFF00) | (uint8)(0);
    ai.state = 0;
    ai.phase = 0;
    this->control.hierarchy_ready = 0;
    product = height * 6500;
    shadow.max_height = 1024;
    camera_subject.SetObject(&model);
    flags = this->model.flags;
    model_effect.depth_bias = 10;
    this->model.scale.words[2] = 6500;
    this->model.scale.words[1] = 6500;
    this->model.scale.words[0] = 6500;
    this->model.flags = flags | 512;
    height = (uint32)((sint32)product >> 12);
    this->ground_offset = height;
    return height;
}

Death::~Death()
{
    if (spawn_sound)
        StopSoundVoices(spawn_sound);
    for (uint32 index = 0; index < 10; ++index)
        if (beams[index])
            beams[index]->Finish();
}

uint32 Death::Destroy(uint32 reason)
{
    uint32 index;
    FUNCTION_MARKER(0x80047CE0, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    ClearTrackedObjectHealth();
    if (spawn_sound)
        StopSoundVoices(spawn_sound);
    spawn_sound = 0;
    for (index = 0; index < 10; ++index)
    {
        if (beams[index])
            beams[index]->Finish();
        beams[index] = 0;
    }
    camera_subject.Reset();
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : 0;
}

uint32 Death::Update()
{
    ModelBorrow model_borrow;
    RecordDataBorrow record_borrow;
    ModelInstance *player;
    uint32 x, y, z, flags, value, phase, strength, index, kind, scale, speed, distance;
    Vector3 point, difference, normal;
    ShortVector3 desired;
    sint32 signed_value, absolute;
    FUNCTION_MARKER(0x80047DBC, "SLUS_003.73");
    x = (uint32)zero_vector[0];
    y = (uint32)zero_vector[1];
    z = (uint32)zero_vector[2];
    motion.acceleration.words[0] = x;
    motion.acceleration.words[1] = y;
    motion.acceleration.words[2] = z;
    if (!(uint32)this->control.hierarchy_ready)
    {
        SetModelBoneTable(this->model.bank, DeathAttachments);
        this->control.hierarchy_ready = 1;
    }
    if (this->control.flags & 64)
    {
        if (!model_effect.color_mode)
            model_effect.color_mode = 0x06402020;
        value = model_effect.scale;
        if (value < 9216)
            model_effect.scale = value + 128;
    }
    else if ((uint32)this->control.damage_phase)
    {
        value = RandomRange(64);
        model_effect.color_mode = ((value + 64) << 16) | 0x06004040;
        phase = (uint32)pulse_angle;
        strength = (uint32)this->control.damage_phase;
        signed_value = rsin(phase & 4095);
        value = strength * (uint32)signed_value;
        if ((sint32)value < 0)
            value += 15;
        signed_value = (sint32)value >> 4;
        absolute = signed_value < 0 ? -signed_value : signed_value;
        phase = (uint32)pulse_angle;
        model_effect.scale = (uint32)absolute + 5632;
        strength = (uint32)this->control.damage_phase;
        pulse_angle = (phase + 128) & 4095;
        this->control.damage_phase = strength - 1;
    }
    else
    {
        value = model_effect.scale;
        if (value < 129)
            model_effect.color_mode = 0;
        else
            model_effect.scale = value - 128;
    }
    value = (uint32)pickup_timer + 1;
    pickup_timer = value;
    if ((value & 0xFFFF) >= 451)
    {
        pickup_timer = 0;
        for (index = 0; index < 8; ++index)
        {
            value = RandomRange(512);
            player = visual_lists.players;
            x = player->pos.words[0];
            point.words[0] = x + ((value - 256) << 12);
            y = player->pos.words[1];
            point.words[1] = y - 4194304;
            value = RandomRange(1024);
            player = visual_lists.players;
            z = player->pos.words[2];
            point.words[2] = z + ((value + 1024) << 12);
            value = FindGroundHeight(point.words, 0, 4096);
            if (value == (uint32)-1)
                continue;
            value = RandomRange(6);
            switch (value)
            {
                case 0:
                    kind = 7;
                    break;
                case 1:
                    kind = 4;
                    break;
                case 2:
                    kind = 6;
                    break;
                case 3:
                    kind = 3;
                    break;
                case 4:
                    kind = 2;
                    break;
                default:
                    kind = 1;
                    break;
            }
            ::SpawnPickup(kind, point.words, 4, 240, zero_vector.words);
            break;
        }
    }
    flags = this->activation_flags;
    this->control.prev_activation_flags = flags;
    if (flags & 1)
    {
        value = (uint32)this->control.activation_count;
        this->activation_flags = flags & 0xFFFE;
        this->control.activation_count = value + 1;
    }
    if (player_distance < 256)
    {
        player = visual_lists.players;
        if (player)
            static_cast<VisualObject *>(visual_lists.players->owner)->ApplyContactImpulse(*this);
    }
    if ((uint32)this->control.script_active)
    {
        value = (uint32)this->control.script_delay;
        if (value)
            this->control.script_delay = value - 1;
        else
            ExecuteScript(script_cursor);
    }
    else
    {
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
        ModelInstance *subject = tracked_target.GetObject();
        if (subject)
        {
            aim_pos.words[0] = subject->pos.words[0];
            aim_pos.words[1] = subject->pos.words[1];
            aim_pos.words[2] = subject->pos.words[2];
        }
        else
            control.flags &= ~16;
        CalculateDirectionAngles(desired.words, model.pos.words, this->aim_pos.words);
        desired.words[0] = 0;
        UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, 32);
    }
    else if (flags & 32)
    {
        CalculateDirectionAngles(desired.words, model.pos.words, this->aim_pos.words);
        desired.words[0] = 0;
        UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, 32);
    }
    else if (flags & 1)
    {
        point.words[0] = this->model.pos.words[0];
        point.words[1] = this->destination.words[1];
        point.words[2] = this->model.pos.words[2];
        distance = MeasureEuclideanDistance(point.words, this->destination.words);
        scale = ai.parameters[0];
        if (distance < scale * 2)
            this->control.flags = this->control.flags & ~1;
        else
        {
            CalculateDirectionAngles(desired.words, this->model.pos.words, this->destination.words);
            desired.words[0] = 0;
            scale = ai.parameters[1];
            UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, desired, scale);
            for (index = 0; index < 3; ++index)
            {
                value = this->destination.words[index];
                x = this->model.pos.words[index];
                difference.words[index] = (uint32)((sint32)(value - x) >> 12);
            }
            NormalizeShortVector(difference.words, normal.words);
            signed_value = (sint16)motion.angular_velocity.words[1];
            absolute = signed_value < 0 ? -signed_value : signed_value;
            if (absolute >= 65)
                absolute = 64;
            scale = ai.parameters[0];
            value = scale * (64 - (uint32)absolute);
            if ((sint32)value < 0)
                value += 63;
            speed = (uint32)((sint32)value >> 6);
            x = normal.words[0] * speed;
            y = normal.words[1] * speed;
            z = normal.words[2] * speed;
            motion.velocity.words[0] = x;
            motion.velocity.words[1] = y;
            motion.velocity.words[2] = z;
        }
    }
    else
        return UpdateWorldTransform();
    if ((this->control.flags & 1) && this->model.animation != 2)
        StartAnimation(2, 1);
    return UpdateWorldTransform();
}

uint32 Death::Damage(uint32 damage, const sint32 *, uint32 kind)
{
    uint32 health, result;
    sint32 reduced;
    FUNCTION_MARKER(0x80048428, "SLUS_003.73");
    if (this->control.flags & 64)
        return 0;
    this->control.damage_phase = 8;
    reduced = (sint32)damage >> 1;
    if (kind == 30)
        reduced >>= 1;
    if (kind == 5 || kind == 10)
        reduced >>= 2;
    health = (uint16)this->health;
    result = health - (uint32)reduced;
    this->health = (sint16)(uint16)(result);
    if ((sint32)(result << 16) <= 0)
        ClearTrackedObjectHealth();
    SetColor(3, 255, 255, 255);
    return 1;
}

// Native helpers retain the previously saved decomposition of the original state machine
void Death::AdvancePhase()
{
    ai.phase = ai.phase + 1;
}

void Death::ClearVelocity()
{
    motion.velocity.words[2] = 0;
    motion.velocity.words[1] = 0;
    motion.velocity.words[0] = 0;
}

void Death::HoldTarget(uint32 mask)
{
    uint32 flags = control.flags;
    control.flags = flags & mask;
    ClearVelocity();
    TrackTarget(visual_lists.players);
}

void Death::RestoreState()
{
    uint32 state = ai.parameters[2];
    uint32 phase = ai.parameters[3];
    ai.state = state;
    ai.phase = phase;
}

static EndpointBeam *CreateBeam(uint32 red, uint32 green, uint32 blue)
{
    GameHeapBlock *allocation = AllocateEffectStorage(164);
    if (!allocation)
        return 0;
    return new (allocation) EndpointBeam(red, green, blue, 5, 700, 100, 100, 440, 110, 1100, 93);
}

void Death::ReadSpawnPoint(uint32 index, Vector3 &point)
{
    RecordDataBorrow record_borrow;
    uint32 state = ai.parameters[2];
    uint32 waypoint = state == 5 ? spawn_points.special_spawn[index] : spawn_points.spawn[index];
    ReadRecordPosition(point.words, waypoint);
}

void Death::SelectDestination()
{
    RecordDataBorrow record_borrow;
    uint32 index, floor, height;
    Vector3 point;
    do
    {
        index = RandomRange(4);
        index = spawn_points.destinations[index];
        ReadRecordPosition(point.words, index);
    } while (MeasurePositionDistance(point.words, model.pos.words) < 257);
    floor = FindGroundHeight(point.words, 512, 2048);
    height = (uint32)(sint32)(sint16)ground_offset;
    point.words[1] = floor - (height << 12);
    SetDestination(point.words);
    AdvancePhase();
}

void Death::UpdateAttack(uint32 phase)
{
    RecordDataBorrow record_borrow;
    Vector3 point, origin;
    GameHeapBlock *child;
    uint32 value;
    uint32 index;
    uint32 state;
    uint32 next_phase;
    uint32 speed;
    uint32 damage;
    EndpointBeam *created;
    ModelInstance *player, *activation_list;
    ShortVector3 angles;
    sint32 timer;
    switch (phase)
    {
        case 0:
            control.flags = control.flags | 64;
            StartAnimation(0, 1);
            ai.work[1] = 190;
            HoldTarget(0xFFFFFFFE);
            activation_list = visual_lists.solids;
            index = spawn_points.activation_record;
            ::ActivateObjectsByIndex(activation_list, index);
            AdvancePhase();
            return;
        case 1:
            value = ai.work[1];
            timer = (sint16)ai.work[1];
            if (value)
                ai.work[1] = (uint32)timer - 1;
            else
            {
                ClearFacing();
                AdvancePhase();
            }
            return;
        case 2:
            timer = (sint16)ai.work[1];
            value = spawn_points.attack[timer];
            ReadRecordPosition(point.words, value);
            value = (uint32)(sint32)(sint16)(uint32)ground_offset;
            point.words[1] -= value << 12;
            SetDestination(point.words);
            AdvancePhase();
            return;
        case 3:
            if (!(control.flags & 1))
            {
                timer = (sint16)ai.work[1];
                value = (uint32)timer + 1;
                if ((sint32)value < 3)
                {
                    ai.work[1] = value;
                    ai.phase = ai.phase - 1;
                }
                else
                    AdvancePhase();
            }
            return;
        case 4:
            HoldTarget(0xFFFFFFBE);
            SetAnimationRange(6, 0, (uint32)-1);
            created = CreateBeam(32, 0, 96);
            value = spawn_points.beam_target;
            beams[0] = created;
            ReadRecordPosition(point.words, value);
            GetModelAttachment(origin.words, *this, 0);
            if (!beams[0])
            {
                GameDiagnostic("Death attack beam allocation failed\n");
                abort();
            }
            beams[0]->SetEndpoints(origin.words, point.words);
            value = ai.phase;
            ai.work[0] = 0;
            ai.work[1] = 0;
            ai.phase = value + 1;
            return;
        case 5:
            value = ai.work[0];
            timer = (sint16)ai.work[0];
            if (value < 15)
            {
                ai.work[0] = (uint32)timer + 1;
                model.frame = 0;
            }
            else if (value == 15)
            {
                ai.work[0] = (uint32)timer + 1;
                PlayPositionedSound(90, model.pos.words, 0);
            }
            value = model.frame;
            if (value - 4 < 9)
            {
                player = visual_lists.players;
                CalculateDirectionAngles(angles.words, model.pos.words, player->pos.words);
                timer = (sint8)model.frame;
                angles.components[1] = (sint16)(((uint32)(uint16)angles.components[1] - (((uint32)timer << 6) - 512)) & 4095);
                child = AllocateEffectStorage(268);
                if (child)
                {
                    speed = GetBehaviorValue(1, 2) & 65535;
                    damage = GetBehaviorValue(1, 1) & 65535;
                    new (child) LineProjectile(model.pos.words, angles.components, speed, damage, &visual_lists.players, 64, 32, 128, 1);
                }
                return;
            }
            if (!playback.complete)
                return;
            value = ai.work[1];
            if (value < 4 || (sint32)swat_population.zombies >= 4)
            {
                ai.work[0] = 0;
                ai.work[1] = value + 1;
                SetAnimationRange(6, 0, (uint32)-1);
            }
            else
            {
                state = ai.state;
                next_phase = ai.phase;
                ai.state = 32;
                ai.phase = 0;
                ai.parameters[2] = state;
                ai.parameters[3] = next_phase + 1;
            }
            return;
        case 6:
            ai.work[0] = 0;
            ai.work[1] = 0;
            SetAnimationRange(6, 0, (uint32)-1);
            ai.phase = ai.phase - 1;
            return;
    }
}

void Death::UpdateSpawn(uint32 phase)
{
    Vector3 point, origin;
    uint32 index;
    GameHeapBlock *child;
    uint32 resource;
    uint32 kind;
    uint32 value;
    uint32 state;
    sint32 timer;
    switch (phase)
    {
        case 0:
            SetAnimationRange(3, 0, (uint32)-1);
            HoldTarget(0xFFFFFFFE);
            value = ai.phase;
            ai.work[1] = 0;
            ai.phase = value + 1;
            return;
        case 1:
            timer = (sint8)model.frame;
            value = model.frame;
            if (timer < 14)
                return;
            if (!(value & 1))
            {
                timer = (sint16)ai.work[1];
                if (!timer)
                {
                    value = PlayPositionedSound(95, model.pos.words, 0);
                    spawn_sound = value;
                }
                if (timer < 10)
                {
                    ReadSpawnPoint((uint32)timer, point);
                    beams[timer] = CreateBeam(8, 24, 8);
                    if (!beams[timer])
                    {
                        GameDiagnostic("Death spawn beam allocation failed\n");
                        abort();
                    }
                    ai.work[1] = (uint32)timer + 1;
                }
                else if ((sint8)model.frame >= 58)
                    AdvancePhase();
            }
            GetModelAttachment(origin.words, *this, 0);
            for (index = 0; (sint32)index < (sint16)ai.work[1]; ++index)
            {
                ReadSpawnPoint(index, point);
                if (beams[index])
                    beams[index]->SetEndpoints(origin.words, point.words);
            }
            return;
        case 2:
            ::StopSoundVoices(spawn_sound);
            spawn_sound = 0;
            for (index = 0; index < 10; ++index)
            {
                if (beams[index])
                    beams[index]->Finish();
                beams[index] = 0;
                ReadSpawnPoint(index, point);
                SpawnDeathFadingRing(point.words);
                value = point.words[1];
                kind = RandomRange(256) + 128;
                point.words[1] -= kind << 12;
                child = AllocateZeroedObject(320);
                Debris *created = 0;
                if (child)
                {
                    kind = GetRecordModelBankSlot();
                    resource = ::FindModelResourceIndex(0x3CD11145, kind);
                    kind = (resource + RandomRange(16)) & 65535;
                    created = new (child) Debris(point.words, zero_vector.words, kind, value, 9, 999, GetRecordModelBankName());
                }
                spawn_debris[index] = created;
                if (!created)
                {
                    GameDiagnostic("Death spawn debris allocation failed\n");
                    abort();
                }
                created->model.flags |= 512;
                created->model.scale.words[0] = 8192;
                created->model.scale.words[1] = 8192;
                created->model.scale.words[2] = 8192;
                created->bounce_count = 64;
            }
            AdvancePhase();
            return;
        case 3:
            if ((sint8)model.frame < 58)
                return;
            PlaySoundEffect(94, 8192, 0);
            ReadSpawnPoint(0, point);
            point.words[1] = model.pos.words[1];
            for (index = 0; index < 10; ++index)
            {
                Debris *debris = spawn_debris[index];
                if (debris)
                {
                    debris->target_pos.words[0] = point.words[0];
                    debris->target_pos.words[1] = point.words[1];
                    debris->target_pos.words[2] = point.words[2];
                    debris->flags &= 0xFFFE;
                    debris->flags |= 256;
                }
            }
            AdvancePhase();
            return;
        case 4:
            if ((sint8)model.frame < 72)
                return;
            ::SetScreenFlash(64, 255, 0, 16, 0, 0);
            for (index = 0; index < 10; ++index)
            {
                if (spawn_debris[index])
                    spawn_debris[index]->Finish();
                spawn_debris[index] = 0;
            }
            ReadSpawnPoint(0, point);
            point.words[1] = model.pos.words[1];
            child = AllocateZeroedObject(624);
            if (child)
            {
                new (child) Swat(point.words, RecordCursor((const uint8 *)spawn_swat_script, sizeof(spawn_swat_script)));
            }
            PlaySoundEffect(93, 8192, 0);
            state = ai.parameters[2];
            ShuffleSpawnPoints(state == 5 ? spawn_points.special_spawn : spawn_points.spawn, state == 5 ? 10 : 21);
            AdvancePhase();
            return;
        case 5:
            if (playback.complete)
            {
                ClearFacing();
                RestoreState();
            }
            return;
    }
}

void Death::UpdateSummon(uint32 phase)
{
    RecordDataBorrow record_borrow;
    Vector3 point, origin;
    uint32 value;
    uint32 index;
    GameHeapBlock *child;
    sint32 timer;
    switch (phase)
    {
        case 0:
            HoldTarget(0xFFFFFFFE);
            SetAnimationRange(3, 0, (uint32)-1);
            value = ai.phase;
            ai.work[1] = 0;
            ai.phase = value + 1;
            return;
        case 1:
            if ((sint8)model.frame < 14)
                return;
            value = PlayPositionedSound(95, model.pos.words, 0);
            spawn_sound = value;
            for (index = 0; index < 5; ++index)
            {
                value = spawn_points.spawn[index];
                ReadRecordPosition(point.words, value);
                beams[index] = CreateBeam(8, 8, 24);
                GetModelAttachment(origin.words, *this, 0);
                if (!beams[index])
                {
                    GameDiagnostic("Death summon beam allocation failed\n");
                    abort();
                }
                beams[index]->SetEndpoints(origin.words, point.words);
                SpawnDeathPersistentRing(point.words);
                child = AllocateZeroedObject(624);
                if (child)
                {
                    new (child) Swat(point.words, RecordCursor((const uint8 *)summon_swat_script, sizeof(summon_swat_script)));
                }
            }
            ShuffleSpawnPoints(spawn_points.spawn, 21);
            AdvancePhase();
            return;
        case 2:
            timer = (sint16)ai.work[1];
            if (timer < 30)
            {
                ai.work[1] = (uint32)timer + 1;
                return;
            }
            ::StopSoundVoices(spawn_sound);
            spawn_sound = 0;
            for (index = 0; index < 5; ++index)
            {
                if (beams[index])
                    beams[index]->Finish();
                beams[index] = 0;
            }
            value = RandomRange(2);
            PlaySoundEffect(value ? 91 : 92, 8192, 0);
            AdvancePhase();
            return;
        case 3:
            if (playback.complete)
            {
                ClearFacing();
                RestoreState();
            }
            return;
    }
}

void Death::UpdateEnding(uint32 phase)
{
    Vector3 point, destination;
    uint32 value;
    uint32 index;
    GameHeapBlock *child;
    Camera *camera;
    ModelInstance *node;
    ShortVector3 angles;
    sint32 timer, interval;
    if (phase)
    {
        value = ((uint32)(sint32)(sint16)ai.work[2] - 32) & 4095;
        camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
        ai.work[2] = value;
        camera->SetYawTransition(value, 4);
        if (!RandomRange(30))
        {
            value = RandomRange(2);
            PlayPositionedSound(value + 96, model.pos.words, 3);
        }
        timer = (sint16)ai.work[3];
        if (timer)
            ai.work[3] = (uint32)timer - 1;
        else
        {
            interval = (sint16)ai.work[4];
            ai.work[4] = interval >= 4 ? (uint32)interval - 3 : 1;
            ai.work[3] = (uint32)interval - 1;
            GetModelAttachment(point.words, *this, 3);
            value = RandomRange(16);
            point.words[1] += value << 12;
            destination.words[0] = point.words[0];
            destination.words[1] = point.words[1] - 0x800000;
            destination.words[2] = point.words[2];
            angles.components[0] = -1024;
            angles.components[1] = 0;
            angles.components[2] = 0;
            child = AllocateEffectStorage(108);
            if (child)
            {
                new (child) TrailProjectile(point.words, destination.words, angles.words, 16, 64, 64, 240, 200, 200, 0, 0, 128);
            }
        }
    }
    phase = ai.phase;
    switch (phase)
    {
        case 0:
            node = visual_lists.actors;
            static_cast<Player *>(visual_lists.players->owner)->disable_timer = 900;
            while (node)
            {
                if (static_cast<VisualObject *>(node->owner)->kind == 200 && (uint32) static_cast<Enemy *>(node->owner)->control.variant == 128)
                {
                    value = (uint32)(sint32) static_cast<VisualObject *>(node->owner)->health;
                    static_cast<VisualObject *>(node->owner)->Damage(value, zero_vector.components, 0);
                }
                node = node->next;
            }
            value = spawn_sound;
            if (value)
            {
                ::StopSoundVoices(value);
                spawn_sound = 0;
            }
            PlayPositionedSound(96, model.pos.words, 3);
            PlayPositionedSound(96, model.pos.words, 3);
            PlayPositionedSound(96, model.pos.words, 3);
            for (index = 0; index < 10; ++index)
            {
                if (beams[index])
                {
                    beams[index]->Finish();
                    beams[index] = 0;
                }
            }
            ClearFacing();
            control.flags = control.flags & 0xFFFFFFFE;
            ClearVelocity();
            motion.angular_velocity.words[2] = 0;
            motion.angular_velocity.words[1] = 0;
            motion.angular_velocity.words[0] = 0;
            motion.angular_acceleration.words[2] = 0;
            motion.angular_acceleration.words[1] = 0;
            motion.angular_acceleration.words[0] = 0;
            SetAnimationRange(8, 0, (uint32)-1);
            camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
            ai.work[2] = 2048;
            ai.work[3] = 16;
            ai.work[4] = 16;
            ai.work[1] = 0;
            if (camera)
            {
                camera->follow.flags = 1;
                camera->follow.target.SetObject(&model);
                camera->aim.flags = 1;
                camera->aim.target.SetObject(&model);
                camera->SetYawTransition(2048, 4);
                SetCameraOrbitDistanceTransition(1332, 16);
                SetCameraOrbitHeightTransition((uint32)-1608, 64);
                SetCameraTargetLateralOffsetTransition(0, 16);
                SetCameraTargetHeightOffsetTransition(460, 16);
                SetCameraTargetForwardOffsetTransition(564, 16);
                static_cast<Camera *>(visual_lists.cameras->owner)->SetColDistances(0);
            }
            AdvancePhase();
            return;
        case 1:
            if ((sint8)model.frame < 89)
                return;
            SpawnExplosionRing(model.pos.words, 256, 32, 32, 128, 4, phase, 100);
            value = ai.phase;
            ai.work[0] = 0;
            ai.phase = value + 1;
            return;
        case 2:
            timer = (sint16)ai.work[0];
            model.frame = 90;
            if (timer < 180)
                ai.work[0] = (uint32)timer + 1;
            else
            {
                value = ai.phase;
                ai.work[0] = 0;
                ai.phase = value + 1;
            }
            return;
        case 3:
            timer = (sint16)ai.work[0];
            model.frame = 90;
            if (timer < 128)
                ai.work[0] = (uint32)timer + 1;
            else
            {
                Finish();
                CompleteLevel(0);
            }
            return;
    }
}

void Death::UpdateState()
{
    ModelBorrow model_borrow;
    uint32 state, phase, value, flags, floor, height;
    ModelInstance *player;
    sint32 timer;
    FUNCTION_MARKER(0x800484D0, "SLUS_003.73");
    if (this->health <= 0 && ai.state != 96)
    {
        ai.state = 96;
        ai.phase = 0;
    }
    state = ai.state;
    phase = ai.phase;
    switch (state)
    {
        case 0:
            if (!phase)
            {
                floor = FindGroundHeight(model.pos.words, 0, 4096);
                if (floor == (uint32)-1)
                    return;
                value = this->model.pos.words[1];
                height = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
                if (((sint32)(floor - value) >> 12) >= (sint32)(height << 1))
                    return;
                value = floor - (height << 12);
                this->model.pos.words[1] = value;
                height = floor - value;
                phase = ai.phase;
                surface.pos.words[1] = floor;
                this->control.gravity_enabled = 0;
                shadow.height = (uint32)((sint32)height >> 12);
                ai.phase = phase + 1;
                ClearVelocity();
                ai.work[1] = 0;
                this->health = (sint16)(uint16)(1200);
                TrackObjectHealth(*this, 24, 42, 4096);
            }
            else if (phase == 1)
            {
                timer = (sint16)ai.work[1];
                ai.work[1] = (uint32)timer + 1;
                if (timer != 60 && timer >= 180)
                {
                    flags = this->control.flags;
                    value = ai.state;
                    ai.phase = 0;
                    this->control.flags = flags & 0xFFFFFFBF;
                    ai.state = value + 1;
                }
            }
            return;
        case 1:
            if (!phase)
            {
                value = ai.phase;
                ai.work[1] = 0;
                ai.phase = value + 1;
                SelectDestination();
            }
            else if (phase == 1)
                SelectDestination();
            else if (phase == 2)
            {
                if (this->health < 960)
                {
                    ai.state = phase;
                    ai.phase = 0;
                    return;
                }
                timer = (sint16)ai.work[1];
                ai.work[1] = (uint32)timer + 1;
                if (player_distance < 768)
                {
                    value = ai.state;
                    phase = ai.phase;
                    ai.state = 16;
                    ai.phase = 0;
                    ai.parameters[2] = value;
                    ai.parameters[3] = phase;
                }
                else if (timer == 30 && (sint32)swat_population.zombies < 6)
                {
                    value = ai.state;
                    ai.parameters[3] = 0;
                    ai.state = 32;
                    ai.phase = 0;
                    ai.parameters[2] = value;
                }
                else if (!(this->control.flags & 1))
                    ai.phase = 1;
            }
            return;
        case 2:
            if (!phase)
                SelectDestination();
            else if (phase == 1)
            {
                if (this->health < 720)
                {
                    ai.state = 5;
                    ai.phase = 0;
                }
                else if (player_distance < 768)
                {
                    ai.parameters[2] = state;
                    value = ai.phase;
                    ai.state = 16;
                    ai.phase = 0;
                    ai.parameters[3] = value;
                }
                else if ((sint32)swat_population.zombies < 4)
                {
                    ai.parameters[2] = state;
                    ai.parameters[3] = 0;
                    ai.state = 64;
                    ai.phase = 0;
                }
                else if (!(this->control.flags & 1))
                    ai.phase = 0;
            }
            return;
        case 5:
            UpdateAttack(phase);
            return;
        case 16:
            if (!phase)
            {
                this->control.flags = this->control.flags & 0xFFFFFFFE;
                ClearVelocity();
                ClearFacing();
                SetAnimationRange(5, 0, (uint32)-1);
                PlayPositionedSound(90, model.pos.words, 0);
                AdvancePhase();
            }
            else if (phase == 1)
            {
                value = this->model.angles.words[1];
                flags = this->model.frame;
                this->model.angles.words[1] = (value - 113) & 4095;
                if (flags - 8 < 13 && player_distance < 768)
                {
                    player = visual_lists.players;
                    static_cast<VisualObject *>(player->owner)->Damage(2, zero_vector.components, 0);
                }
                if (this->playback.complete)
                {
                    SetAnimationRange(7, 0, (uint32)-1);
                    AdvancePhase();
                }
            }
            else if (phase == 2 && this->playback.complete)
                RestoreState();
            return;
        case 32:
            UpdateSpawn(phase);
            return;
        case 64:
            UpdateSummon(phase);
            return;
        case 96:
            UpdateEnding(phase);
            return;
    }
}

uint32 Death::ReadParameters(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value, index;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80049B38, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x2130)
        return Enemy::ReadParameters(command);
    cursor = script_cursor;
    script_cursor = cursor + 2;
    for (index = 0; index < 4; ++index)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        spawn_points.destinations[index] = value;
        script_cursor = cursor + 2;
    }
    for (index = 0; index < 3; ++index)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        cursor += 2;
        spawn_points.attack[index] = value;
        script_cursor = cursor;
    }
    value = cursor.Read16();
    script_cursor = cursor + 2;
    spawn_points.beam_target = value;
    for (index = 0; index < 21; ++index)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        cursor += 2;
        spawn_points.spawn[index] = value;
        script_cursor = cursor;
    }
    value = cursor.Read16();
    script_cursor = cursor + 2;
    spawn_points.activation_record = value;
    for (index = 0; index < 10; ++index)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        spawn_points.special_spawn[index] = value;
        script_cursor = cursor + 2;
    }
    return 0;
}

uint32 Death::ExecuteCommand(uint32 command)
{
    FUNCTION_MARKER(0x80049C48, "SLUS_003.73");
    return Enemy::ExecuteCommand(command & 0xFFFF);
}

uint32 Death::SetDestination(const uint32 *position)
{
    uint32 x, y, z, flags;
    FUNCTION_MARKER(0x80049C68, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    this->destination.words[0] = x;
    this->destination.words[1] = y;
    this->destination.words[2] = z;
    flags = (this->control.flags | 1) & 0xFFFFFFF3;
    this->control.flags = flags;
    return flags;
}

uint32 Death::TrackTarget(ModelInstance *target)
{
    FUNCTION_MARKER(0x80049C98, "SLUS_003.73");
    uint32 result = static_cast<VisualObject *>(target->owner)->IsFinished();
    if (result)
        return result;
    tracked_target.SetObject(target);
    result = (control.flags & 0xFFFFFFDF) | 16;
    control.flags = result;
    return result;
}

uint16 *Death::ClearFacing()
{
    uint32 flags;
    FUNCTION_MARKER(0x80049D18, "SLUS_003.73");
    flags = this->control.flags;
    flags &= 0xFFFFFFEF;
    flags &= 0xFFFFFFDF;
    this->control.flags = flags;
    motion.angular_velocity.words[2] = 0;
    motion.angular_velocity.words[1] = 0;
    motion.angular_velocity.words[0] = 0;
    motion.angular_acceleration.words[2] = 0;
    motion.angular_acceleration.words[1] = 0;
    motion.angular_acceleration.words[0] = 0;
    return motion.angular_acceleration.words;
}

void Death::SetParameters(uint32 command)
{
    ReadParameters(command);
}

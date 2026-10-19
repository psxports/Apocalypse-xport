#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_heap.h"
#include "effect_lifecycle.h"
#include "level_session.h"
#include "game_tuning.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "particle_settings.h"
#include "fading_particle.h"
#include "angle_math.h"
#include "vector_distance.h"
#include "object_damage.h"
#include "vector_arithmetic.h"
#include "collision.h"
#include "game_diagnostics.h"
#include "player.h"
#include "audio_session.h"

#include "hound_rat.h"
#include "record_data.h"
#include "game_random.h"
#include "game_object_allocation.h"

HoundRat::HoundRat(const RecordCursor &record_data, uint32 index, uint32 mode) : Enemy()
{
    RecordDataBorrow record_borrow;
    uint32 flags;
    RecordCursor cursor;
    FUNCTION_MARKER(0x800737A0, "SLUS_003.73");

    if (mode)
    {
        this->kind = 208;
        SetModelBank("hound");
        this->model.scale.words[2] = 6144;
        this->model.scale.words[1] = 6144;
        this->model.scale.words[0] = 6144;
        this->ground_offset = 52;
        this->sight_offset = 32;
        flags = this->model.flags;
        this->health = (sint16)(uint16)(60);
        this->model.flags = flags | 0x200;
        AllocateBehaviorTable(1, 2);
        SetBehaviorValue(0, 1, 10);
    }
    else
    {
        this->kind = 210;
        SetModelBank("rat");
        this->ground_offset = 64;
        this->sight_offset = 48;
        this->health = (sint16)(uint16)(40);
        AllocateBehaviorTable(1, 2);
        SetBehaviorValue(0, 1, 5);
    }
    cursor = ReadPosition(record_data);
    cursor = ReadAngles(cursor);
    AddToList(visual_lists.actors);
    script_cursor = cursor;
    this->record_idx = index;
    ai.state = 1;
    Init();
    cursor = script_cursor;
    if (cursor)
    {
        this->control.script_active = 1;
        ExecuteScript(RecordCursor(cursor));
    }
}

uint32 HoundRat::Init()
{
    uint32 total, ground, height;
    FUNCTION_MARKER(0x80073920, "SLUS_003.73");
    this->playback.speed = 0x18000u;
    total = game_progress.level.enemy_total;
    game_progress.level.enemy_total = total + 1u;
    StartAnimation(0u, 1u);
    ai.parameters[0] = 108;
    ai.parameters[1] = 80;
    ground = FindGroundHeight(model.pos.words, 512, 512);
    if (ground != 0xFFFFFFFF)
    {
        height = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
        this->model.pos.words[1] = ground - (height << 12);
    }
    this->control.script_active = 1;
    return 1;
}

uint32 HoundRat::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x800739C4, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    Enemy::FreeBehaviorTable();
    result = Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(this);
    return result;
}

uint32 HoundRat::Update()
{
    RecordDataBorrow record_borrow;
    ShortVector3 motion_target;
    Vector3 constants, difference, normal;
    uint32 value, flags, left, right, x, y, z, scale, angle;
    FUNCTION_MARKER(0x80073A3C, "SLUS_003.73");
    if (this->kind == 208u && !RandomRange(240))
        PlayPositionedSound(112, model.pos.words, 0);
    constants = zero_vector;
    motion.acceleration = constants;
    UpdateWorldTransform();
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
    if ((uint32)this->control.gravity_enabled)
        AddVectors(motion.acceleration.words, world_gravity.vector.words);
    IntegrateMotion();
    if (!(this->control.flags & 1u))
        return 0;
    value = MeasurePositionDistance(this->model.pos.words, this->destination.words);
    if ((sint32)value < 129)
    {
        flags = this->control.flags;
        this->control.flags = flags & 0xFFFFFFFE;
        return 0;
    }
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
    x = normal.words[0] * scale;
    y = normal.words[1] * scale;
    z = normal.words[2] * scale;
    motion.velocity.words[0] = x;
    motion.velocity.words[1] = y;
    motion.velocity.words[2] = z;
    left = this->destination.words[2];
    right = this->model.pos.words[2];
    z = left - right;
    left = this->destination.words[0];
    right = this->model.pos.words[0];
    angle = (3072u - (uint32)ratan2((sint32)z, (sint32)(left - right))) & 4095u;
    scale = ai.parameters[1];
    motion_target.x = 0;
    motion_target.y = angle;
    motion_target.z = 0;
    UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, scale);
    // The legacy behavior caller discards the original return carrier
    return 0;
}

uint32 HoundRat::ExecuteCommand(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value, ground, height;
    RecordCursor cursor;
    Vector3 position;
    FUNCTION_MARKER(0x80073C78, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x4221)
        return Enemy::ExecuteCommand(command);
    cursor = script_cursor;
    value = cursor.Read16();
    script_cursor = cursor + 2;
    if (value & 0x2000)
    {
        value = ResolveParameter(value) & 0xFFFF;
    }
    ReadRecordPosition(position.words, value);
    ground = FindGroundHeight(position.words, 0, 2048);
    if (ground != (uint32)-1)
    {
        height = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
        position.words[1] = ground - (height << 12);
    }
    HoundRat::SetTarget(position.words);
    return 1;
}

void HoundRat::Orbit(Vector3 &point, uint32 angle, uint32 reload_angle)
{
    uint32 trig_angle;
    uint32 radius, sine, cosine;
    ModelInstance *player;
    if (reload_angle)
        angle = ai.work[2];
    radius = (uint32)(sint32)(sint16)ai.work[0];
    trig_angle = angle & 4095u;
    sine = (uint32)rsin(trig_angle);
    cosine = (uint32)rcos(trig_angle);
    player = visual_lists.players;
    point.words[0] = radius * sine;
    point.words[1] = 0;
    point.words[2] = radius * cosine;
    AddVectors(point.words, player->pos.words);
}

void HoundRat::UpdateState()
{
    ShortVector3 motion_target;
    ModelInstance *player;
    uint32 distance, damage, state, phase;
    uint32 flags, value, left, right, angle, speed, direction;
    uint32 divisor;
    Vector3 candidate, point, difference;
    CollisionRay collision;
    uint32 count;
    GameHeapBlock *child;
    uint32 red;
    uint32 green;
    uint32 blue;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80073D3C, "SLUS_003.73");
    player = visual_lists.players;
    if (!player)
        ai.state = 666;
    distance = player_distance;
    if (distance < 129u)
    {
        damage = GetBehaviorValue(0, 1) & 0xFFFFu;
        VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
        damage_target->Damage(damage, zero_vector.components, 0);
    }
    state = ai.state;
    switch (state)
    {
        case 0:
            value = this->model.animation;
            if (value)
            {
                if (value != 1u || this->playback.complete)
                {
                    StartAnimation(0u, 1u);
                    value = RandomRange(32);
                    this->model.frame = value;
                }
            }
            else if (!RandomRange(512))
                SetAnimationRange(1u, 0u, 0xFFFFFFFF);
            flags = this->control.flags;
            player = visual_lists.players;
            right = this->model.pos.words[2];
            this->control.flags = flags & 0xFFFFFFFE;
            left = player->pos.words[2];
            direction = left - right;
            left = player->pos.words[0];
            right = this->model.pos.words[0];
            angle = (3072u - (uint32)ratan2((sint32)direction, (sint32)(left - right))) & 4095u;
            speed = ai.parameters[1];
            motion_target.x = 0;
            motion_target.y = angle;
            motion_target.z = 0;
            UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, speed);
            if (!RandomRange(16))
                ai.state = 1;
            return;
        case 1:
            if (player_distance < 513u)
            {
                player = visual_lists.players;
                value = (uint16)player_distance;
                ai.work[0] = value;
                CalculateDirectionAngles(angles.words, player->pos.words, model.pos.words);
                ai.state = 3;
                flags = this->control.flags;
                ai.phase = 0;
                ai.work[1] = (uint16)angles.components[1];
                this->control.flags = flags & 0xFFFFFFFE;
                return;
            }
            if (this->control.flags & 1u)
                return;
            player = visual_lists.players;
            if (player)
            {
                point = player->pos;
                if (ChoosePosition(candidate.words, point.words, 128u, 192u, 64u, 1024u, 1u, 1u))
                {
                    SetTarget(candidate.words);
                    return;
                }
            }
            ai.state = 0;
            return;
        case 2:
            if (this->model.animation == 3u)
            {
                if (this->playback.complete)
                    Finish();
                return;
            }
            flags = this->control.flags;
            this->control.flags = flags & 0xFFFFFFFE;
            motion.velocity.words[2] = 0;
            motion.velocity.words[1] = 0;
            motion.velocity.words[0] = 0;
            this->playback.speed = 0x18000u;
            SetAnimationRange(3u, 0u, 0xFFFFFFFF);
            return;
        case 3:
            phase = ai.phase;
            if (!phase)
            {
                if (!RandomRange(32))
                    ai.work[3] = RandomRange(2) ? 0u : 1u;
                if (!RandomRange(16))
                {
                    value = (uint32)(sint32)(sint16)ai.work[0];
                    value = value - 128u + RandomRange(256);
                    if ((sint32)value > 1024)
                        value = 1024;
                    if ((sint32)value < 400)
                        value = 400;
                    ai.work[0] = value;
                }
                direction = (uint32)(sint32)(sint16)ai.work[3];
                value = (uint32)(sint32)(sint16)ai.work[2];
                angle = RandomRange(128) + 256u;
                value = (direction ? value - angle : value + angle) & 4095u;
                ai.work[2] = value;
                Orbit(point, value, 1);
                if (!ChoosePosition(candidate.words, point.words, 0u, 0u, 64u, 32u, 1u, 1u))
                {
                    ai.state = 0;
                    return;
                }
                SetTarget(candidate.words);
                value = ai.phase;
                ai.phase = value + 1u;
                return;
            }
            if (phase != 1u || (this->control.flags & 1u))
                return;
            if (!this->playback.speed)
                this->playback.speed = 0x18000u;
            value = (uint32)(sint32)(sint16)ai.work[1] + 1u;
            ai.work[1] = value;
            if ((sint16)value < 9 || RandomRange(8))
            {
                ai.phase = 0;
                return;
            }
            value = ((uint32)(sint32)(sint16)ai.work[2] + 2048u) & 4095u;
            ai.work[2] = value;
            Orbit(point, value, 0);
            if (!ChoosePosition(candidate.words, point.words, 0u, 0u, 64u, 32u, phase, phase))
            {
                ai.phase = 0;
                return;
            }
            value = this->kind;
            PlayPositionedSound(value == 208 ? 111 : 54, model.pos.words, 0);
            SetTarget(candidate.words);
            ai.work[1] = 0;
            this->playback.speed = 0u;
            this->model.frame = 7u;
            return;
        case 10:
            phase = ai.phase;
            if (phase == 2u)
            {
                if (this->playback.complete)
                    Finish();
                return;
            }
            if (phase > 2u)
                return;
            if (phase == 1u)
            {
                if ((sint16)ai.work[0])
                {
                    SetParticleAngles(model.angles.components);
                    ::SetParticleSize(2u);
                    red = (uint8)GetGameTuning().death_particle_color[0];
                    green = (uint8)GetGameTuning().death_particle_color[1];
                    blue = (uint8)GetGameTuning().death_particle_color[2];
                    SetParticleColor(red, green, blue);
                    red = GetGameTuning().death_particle_color[0];
                    green = GetGameTuning().death_particle_color[1];
                    blue = GetGameTuning().death_particle_color[2];
                    SetParticleColorStep((red >> 5) & 255u, (green >> 5) & 255u, (blue >> 5) & 255u);
                    use_small_effect_pool = false;
                    for (count = 0; count < 16u; ++count)
                    {
                        child = AllocateEffectStorage(88);
                        if (child)
                        {
                            new (child) FadingParticle(model.pos.words, 16, 0x2000, 16);
                        }
                    }
                    value = (uint32)(sint32)(sint16)ai.work[0];
                    use_small_effect_pool = true;
                    ai.work[0] = value - 1u;
                    return;
                }
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                value = ai.phase;
                motion.velocity.words[0] = 0;
                ai.phase = value + 1u;
                return;
            }
            this->model.angles.words[0] = 0;
            motion.angular_velocity.words[2] = 0;
            motion.angular_velocity.words[1] = 0;
            motion.angular_velocity.words[0] = 0;
            motion.angular_acceleration.words[2] = 0;
            motion.angular_acceleration.words[1] = 0;
            flags = this->control.flags;
            motion.angular_acceleration.words[0] = 0;
            this->control.flags = flags & 0xFFFFFFFE;
            RemoveShadow();
            BuildDirectionVector(this->destination.words, 1024u, this->model.angles.words);
            SubtractVectors(point.words, this->model.pos.words, this->destination.words);
            this->destination = point;
            collision.endpoints[0].words[0] = this->model.pos.words[0];
            collision.endpoints[0].words[1] = this->model.pos.words[1];
            collision.endpoints[0].words[2] = this->model.pos.words[2];
            collision.endpoints[1].words[0] = this->destination.words[0];
            collision.endpoints[1].words[1] = this->destination.words[1];
            collision.endpoints[1].words[2] = this->destination.words[2];
            InitializeCollisionRay(collision);
            QueryCollisionRay(collision, 1u);
            if (collision.hit.GetObject())
            {
                value = collision.distance - 256u;
                if ((sint32)value < 64)
                {
                    motion.velocity.words[2] = 0;
                    motion.velocity.words[1] = 0;
                    motion.velocity.words[0] = 0;
                    ai.work[0] = 1;
                    goto death_motion;
                }
                divisor = (uint32)((sint32)(value + ((sint32)value < 0 ? 63u : 0u)) >> 6);
                ai.work[0] = divisor;
                BuildDirectionVector(this->destination.words, value, this->model.angles.words);
                SubtractVectors(point.words, this->model.pos.words, this->destination.words);
                this->destination = point;
                SubtractVectors(difference.words, this->destination.words, this->model.pos.words);
                divisor = (uint32)(sint32)(sint16)ai.work[0];
            }
            else
            {
                ai.work[0] = 16;
                SubtractVectors(difference.words, this->destination.words, this->model.pos.words);
                divisor = 16;
            }
            DivideVector(point.words, difference.words, divisor);
            motion.velocity = point;
        death_motion:
            motion.velocity.words[1] = 0xFFFC0000;
            this->control.gravity_enabled = 1;
            motion.drag[0] = 31;
            motion.drag[1] = 3;
            motion.drag[2] = 31;
            SetAnimationRange(3u, 0u, 0xFFFFFFFF);
            this->collision_radius = 0u;
            ActivateLinkedRecords();
            value = ai.phase;
            ai.phase = value + 1u;
            return;
        case 666:
            if (this->model.animation != 1u)
            {
                StartAnimation(1u, 1u);
                value = RandomRange(30);
                this->model.frame = value;
            }
            flags = this->control.flags;
            ai.state = 0;
            this->control.flags = flags & 0xFFFFFFFE;
            return;
        default:
            return;
    }
}

uint32 HoundRat::SetTarget(const uint32 *target)
{
    FUNCTION_MARKER(0x80074730, "SLUS_003.73");
    uint32 x, y, z, value;
    if (this->model.animation != 2)
    {
        StartAnimation(2, 1);
        value = RandomRange(16);
        this->model.frame = value;
    }
    x = target[0];
    y = target[1];
    z = target[2];
    this->destination.words[0] = x;
    this->destination.words[1] = y;
    this->destination.words[2] = z;
    value = this->control.flags | 1;
    this->control.flags = value;
    return value;
}

uint32 HoundRat::Damage(uint32 damage, const sint32 *, uint32 kind)
{
    uint32 tick, flags, health, value, type;
    ModelInstance *player;
    FUNCTION_MARKER(0x800747A8, "SLUS_003.73");
    tick = (uint32)this->control.damage_phase;
    this->control.damage_phase = tick + 1;
    if (kind == 5)
    {
        flags = this->control.flags;
        this->control.flags = flags | 2;
    }
    health = (uint32)(sint32)this->health;
    value = (uint16)this->health;
    if ((sint32)health <= 0)
        return 1;
    value -= damage;
    this->health = (sint16)(uint16)(value);
    if ((sint32)(value << 16) > 0)
    {
        SetColor(3, 255, 255, 255);
        return 1;
    }
    player = visual_lists.players;
    if (player)
    {
        // The original reads the kind although the quote callee ignores it
        (void)this->kind;
        TryPlayPlayerKillQuote(player->pos.words);
    }
    flags = this->lifecycle_flags;
    type = this->kind;
    this->lifecycle_flags = flags & 0xFFEF;
    PlayPositionedSound(type == 208 ? 110 : 53, this->model.pos.words, 0);
    ai.state = 10;
    ai.phase = 0;
    return 1;
}

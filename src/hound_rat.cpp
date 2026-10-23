#include <new>
#include <stdlib.h>
#include <string.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "effect_lifecycle.h"
#include "level_session.h"
#include "game_tuning.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "effect_object.h"
#include "primitive_particle.h"
#include "fading_particle.h"
#include "angle_math.h"
#include "global.h"
#include "object_damage.h"
#include "collision.h"
#include "game_diagnostics.h"
#include "player.h"
#include "audio_session.h"
#include "hound_rat.h"
#include "record_data.h"
#include "game_object_allocation.h"
#include "enemy.h"

HoundRat::HoundRat(const RecordCursor &rec_data, uint16 idx, bool hound) : Enemy()
{
    RecordDataBorrow rec_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x800737A0, "SLUS_003.73");

    if (hound)
    {
        kind = 208;
        SetModelBank("hound");
        model.scale.z = 6144;
        model.scale.y = 6144;
        model.scale.x = 6144;
        ground_offset = 52;
        sight_offset = 32;
        health = 60;
        model.flags |= 0x200;
        AllocateBehaviorTable(1, 2);
        SetBehaviorVal(0, 1, 10);
    }
    else
    {
        kind = 210;
        SetModelBank("rat");
        ground_offset = 64;
        sight_offset = 48;
        health = 40;
        AllocateBehaviorTable(1, 2);
        SetBehaviorVal(0, 1, 5);
    }
    cursor = ReadPos(rec_data);
    cursor = ReadAngles(cursor);
    AddToList(visual_lists.actors);
    script_cursor = cursor;
    rec_idx = idx;
    ai.state = 1;
    Init();
    cursor = script_cursor;
    if (cursor)
    {
        control.script_active = 1;
        ExecuteScript(cursor);
    }
}

uint32 HoundRat::Init()
{
    uint32 ground;
    sint32 offset;
    FUNCTION_MARKER(0x80073920, "SLUS_003.73");
    playback.speed = 0x18000;
    ++game_progress.level.enemy_total;
    StartAnimation(0, 1);
    ai.parameters[0] = 108;
    ai.parameters[1] = 80;
    ground = CollisionRay::FindGroundHeight(model.pos, 512, 512);
    if (ground != 0xFFFFFFFF)
    {
        offset = ground_offset * ONE;
        model.pos.y = ground - offset;
    }
    control.script_active = 1;
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
        return ReleaseStorage();
    return result;
}

uint32 HoundRat::Update()
{
    RecordDataBorrow rec_borrow;
    ShortVector3 motion_target;
    Vector3 difference, normal;
    uint16 delay;
    uint32 scale, angle;
    sint32 distance;
    FUNCTION_MARKER(0x80073A3C, "SLUS_003.73");
    if (kind == 208 && !RandomRange(240))
        PlayPositionedSound(112, &model.pos, 0);
    motion.acceleration = zero_vector;
    UpdateWorldTransform();
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
    if (control.gravity_enabled)
        motion.acceleration.Add(world_gravity.vector);
    IntegrateMotion();
    if (!(control.flags & 1))
        return 0;
    distance = model.pos.Distance(dst);
    if (distance < 129)
    {
        control.flags &= ~1;
        return 0;
    }
    difference.x = (sint32)((uint32)dst.x - model.pos.x) >> 12;
    difference.y = (sint32)((uint32)dst.y - model.pos.y) >> 12;
    difference.z = (sint32)((uint32)dst.z - model.pos.z) >> 12;
    NormalizeShortVector(difference, normal);
    scale = ai.parameters[0];
    motion.velocity.x = (uint32)normal.x * scale;
    motion.velocity.y = (uint32)normal.y * scale;
    motion.velocity.z = (uint32)normal.z * scale;
    angle = (3072 - (uint32)ratan2((sint32)((uint32)dst.z - model.pos.z), (sint32)((uint32)dst.x - model.pos.x))) & 4095;
    scale = ai.parameters[1];
    motion_target.x = 0;
    motion_target.y = angle;
    motion_target.z = 0;
    UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, scale);
    // The legacy behavior caller discards the original return carrier
    return 0;
}

uint32 HoundRat::ExecuteCommand(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 val;
    uint32 ground;
    sint32 offset;
    RecordCursor cursor;
    Vector3 pos;
    FUNCTION_MARKER(0x80073C78, "SLUS_003.73");
    if (command != 0x4221)
        return Enemy::ExecuteCommand(command);
    cursor = script_cursor;
    val = cursor.Read16();
    script_cursor = cursor + 2;
    if (val & 0x2000)
    {
        val = ResolveParameter(val);
    }
    ReadRecPos(&pos, val);
    ground = CollisionRay::FindGroundHeight(pos, 0, 2048);
    if (ground != 0xFFFFFFFF)
    {
        offset = ground_offset * ONE;
        pos.y = ground - offset;
    }
    SetTarget(&pos);
    return 1;
}

void HoundRat::BuildOrbitPoint(Vector3 &point, uint16 angle) const
{
    sint16 radius = ai.work[0];
    sint32 sine, cosine;
    ModelInstance *player;
    angle &= 4095;
    sine = rsin(angle);
    cosine = rcos(angle);
    player = visual_lists.players;
    point.x = radius * sine;
    point.y = 0;
    point.z = radius * cosine;
    point.Add(player->pos);
}

void HoundRat::UpdateState()
{
    ShortVector3 motion_target;
    ModelInstance *player;
    uint32 distance;
    uint16 damage, state, phase;
    sint32 radius;
    uint16 jitter;
    uint32 flags, val, left, right, angle, speed, dir;
    uint32 divisor;
    Vector3 candidate, point, difference;
    CollisionRay col;
    uint32 count;
    GameHeapBlock *child;
    uint16 r, g, b;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80073D3C, "SLUS_003.73");
    player = visual_lists.players;
    if (!player)
        ai.state = 666;
    distance = player_distance;
    if (distance < 129)
    {
        damage = GetBehaviorVal(0, 1);
        VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
        damage_target->Damage(damage, &zero_vector, 0);
    }
    state = ai.state;
    switch (state)
    {
        case 0:
            val = model.animation;
            if (val)
            {
                if (val != 1 || playback.complete)
                {
                    StartAnimation(0, 1);
                    val = RandomRange(32);
                    model.frame = val;
                }
            }
            else if (!RandomRange(512))
                SetAnimationRange(1, 0, -1);
            flags = control.flags;
            player = visual_lists.players;
            right = (uint32)model.pos.z;
            control.flags = flags & 0xFFFFFFFE;
            left = (uint32)player->pos.z;
            dir = left - right;
            left = (uint32)player->pos.x;
            right = (uint32)model.pos.x;
            angle = (3072 - (uint32)ratan2((sint32)dir, (sint32)(left - right))) & 4095;
            speed = ai.parameters[1];
            motion_target.x = 0;
            motion_target.y = angle;
            motion_target.z = 0;
            UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, speed);
            if (!RandomRange(16))
                ai.state = 1;
            return;
        case 1:
            if (player_distance < 513)
            {
                player = visual_lists.players;
                ai.work[0] = player_distance;
                angles.SetDir(player->pos, model.pos);
                ai.state = 3;
                flags = control.flags;
                ai.phase = 0;
                ai.work[1] = (uint16)angles.y;
                control.flags = flags & 0xFFFFFFFE;
                return;
            }
            if (control.flags & 1)
                return;
            player = visual_lists.players;
            if (player)
            {
                point = player->pos;
                if (ChoosePos(&candidate, &point, 128, 192, 64, 1024, 1, 1))
                {
                    SetTarget(&candidate);
                    return;
                }
            }
            ai.state = 0;
            return;
        case 2:
            if (model.animation == 3)
            {
                if (playback.complete)
                    Finish();
                return;
            }
            flags = control.flags;
            control.flags = flags & 0xFFFFFFFE;
            motion.velocity.z = 0;
            motion.velocity.y = 0;
            motion.velocity.x = 0;
            playback.speed = 0x18000;
            SetAnimationRange(3, 0, -1);
            return;
        case 3:
            phase = ai.phase;
            if (!phase)
            {
                if (!RandomRange(32))
                    ai.work[3] = RandomRange(2) ? 0 : 1;
                if (!RandomRange(16))
                {
                    radius = (sint16)ai.work[0];
                    jitter = RandomRange(256);
                    radius = radius - 128 + jitter;
                    if (radius > 1024)
                        radius = 1024;
                    if (radius < 400)
                        radius = 400;
                    ai.work[0] = radius;
                }
                dir = ai.work[3];
                val = ai.work[2];
                angle = RandomRange(128) + 256;
                val = (dir ? val - angle : val + angle) & 4095;
                ai.work[2] = val;
                BuildOrbitPoint(point, ai.work[2]);
                if (!ChoosePos(&candidate, &point, 0, 0, 64, 32, 1, 1))
                {
                    ai.state = 0;
                    return;
                }
                SetTarget(&candidate);
                ++ai.phase;
                return;
            }
            if (phase != 1 || (control.flags & 1))
                return;
            if (!playback.speed)
                playback.speed = 0x18000;
            ++ai.work[1];
            if ((sint16)ai.work[1] < 9 || RandomRange(8))
            {
                ai.phase = 0;
                return;
            }
            val = (ai.work[2] + 2048) & 4095;
            ai.work[2] = val;
            BuildOrbitPoint(point, val);
            if (!ChoosePos(&candidate, &point, 0, 0, 64, 32, phase, phase))
            {
                ai.phase = 0;
                return;
            }
            val = kind;
            PlayPositionedSound(val == 208 ? 111 : 54, &model.pos, 0);
            SetTarget(&candidate);
            ai.work[1] = 0;
            playback.speed = 0;
            model.frame = 7;
            return;
        case 10:
            phase = ai.phase;
            if (phase == 2)
            {
                if (playback.complete)
                    Finish();
                return;
            }
            if (phase > 2)
                return;
            if (phase == 1)
            {
                if (ai.work[0])
                {
                    PrimitiveParticle::SetAngles(&model.angles);
                    PrimitiveParticle::SetSize(2);
                    r = GetGameTuning().death_particle_color[0];
                    g = GetGameTuning().death_particle_color[1];
                    b = GetGameTuning().death_particle_color[2];
                    PrimitiveParticle::SetColor(r, g, b);
                    r = GetGameTuning().death_particle_color[0];
                    g = GetGameTuning().death_particle_color[1];
                    b = GetGameTuning().death_particle_color[2];
                    PrimitiveParticle::SetColorStep(r >> 5, g >> 5, b >> 5);
                    use_small_effect_pool = false;
                    for (count = 0; count < 16; ++count)
                    {
                        child = EffectObject::AllocateStorage(88);
                        if (child)
                        {
                            new (child) FadingParticle(&model.pos, 16, 0x2000, 16);
                        }
                    }
                    use_small_effect_pool = true;
                    --ai.work[0];
                    return;
                }
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                val = ai.phase;
                motion.velocity.x = 0;
                ai.phase = val + 1;
                return;
            }
            model.angles.x = 0;
            motion.angular_velocity.z = 0;
            motion.angular_velocity.y = 0;
            motion.angular_velocity.x = 0;
            motion.angular_acceleration.z = 0;
            motion.angular_acceleration.y = 0;
            flags = control.flags;
            motion.angular_acceleration.x = 0;
            control.flags = flags & 0xFFFFFFFE;
            RemoveShadow();
            dst.SetDir(1024, model.angles);
            point.Subtract(model.pos, dst);
            dst = point;
            col.endpoints[0] = model.pos;
            col.endpoints[1] = dst;
            col.Initialize();
            col.Query(1);
            if (col.hit.GetObject())
            {
                val = col.distance - 256;
                if ((sint32)val < 64)
                {
                    motion.velocity.z = 0;
                    motion.velocity.y = 0;
                    motion.velocity.x = 0;
                    ai.work[0] = 1;
                    goto death_motion;
                }
                divisor = val / 64;
                ai.work[0] = divisor;
                dst.SetDir(val, model.angles);
                point.Subtract(model.pos, dst);
                dst = point;
                difference.Subtract(dst, model.pos);
                divisor = (sint16)ai.work[0];
            }
            else
            {
                ai.work[0] = 16;
                difference.Subtract(dst, model.pos);
                divisor = 16;
            }
            point.Divide(difference, divisor);
            motion.velocity = point;
        death_motion:
            motion.velocity.y = 0xFFFC0000;
            control.gravity_enabled = 1;
            motion.drag[0] = 31;
            motion.drag[1] = 3;
            motion.drag[2] = 31;
            SetAnimationRange(3, 0, -1);
            col_radius = 0;
            ActivateLinkedRecs();
            ++ai.phase;
            return;
        case 666:
            if (model.animation != 1)
            {
                StartAnimation(1, 1);
                val = RandomRange(30);
                model.frame = val;
            }
            flags = control.flags;
            ai.state = 0;
            control.flags = flags & 0xFFFFFFFE;
            return;
        default:
            return;
    }
}

uint32 HoundRat::SetTarget(const Vector3 *target)
{
    FUNCTION_MARKER(0x80074730, "SLUS_003.73");
    uint32 val;
    if (model.animation != 2)
    {
        StartAnimation(2, 1);
        val = RandomRange(16);
        model.frame = val;
    }
    dst = *target;
    val = control.flags | 1;
    control.flags = val;
    return val;
}

uint32 HoundRat::Damage(uint32 damage, const Vector3 *, uint32 kind)
{
    uint16 type;
    sint16 health;
    ModelInstance *player;
    FUNCTION_MARKER(0x800747A8, "SLUS_003.73");
    ++control.damage_phase;
    if (kind == 5)
    {
        control.flags |= 2;
    }
    health = this->health;
    if (health <= 0)
        return 1;
    this->health = health - damage;
    if (this->health > 0)
    {
        SetColor(3, 255, 255, 255);
        return 1;
    }
    player = visual_lists.players;
    if (player)
    {
        // The original reads the kind although the quote callee ignores it
        (void)this->kind;
        static_cast<Player *>(player->owner)->TryPlayKillQuote();
    }
    type = this->kind;
    lifecycle_flags &= 0xFFEF;
    PlayPositionedSound(type == 208 ? 110 : 53, &model.pos, 0);
    ai.state = 10;
    ai.phase = 0;
    return 1;
}

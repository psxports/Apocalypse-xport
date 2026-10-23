#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "effect_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "record_factory.h"
#include "global.h"
#include "angle_math.h"
#include "object_damage.h"
#include "collision.h"
#include "model_animation.h"
#include "model_attachment_data.h"
#include "model_attachment.h"
#include "game_diagnostics.h"
#include "player.h"
#include "audio_session.h"
#include "game_panel.h"
#include "croc.h"
#include "record_data.h"
#include "panel_particle.h"
#include "game_object_allocation.h"
#include "enemy.h"
#include "effect_object.h"

Croc::Croc(const RecordCursor &rec_data, uint16 idx) : Enemy()
{
    RecordDataBorrow rec_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80071CEC, "SLUS_003.73");

    kind = 212;
    SetModelBank("croc");
    cursor = ReadPos(rec_data);
    cursor = ReadAngles(cursor);
    AddToList(visual_lists.actors);
    script_cursor = cursor;
    rec_idx = idx;
    Initialize();
    cursor = script_cursor;
    if (cursor)
    {
        control.script_active = 1;
        ExecuteScript(cursor);
    }
    if (level_environment.area_kind == 1)
        TrackHealth(24, 42, 4096);
}

uint32 Croc::Initialize()
{
    uint32 height;
    FUNCTION_MARKER(0x80071DD4, "SLUS_003.73");
    ground_offset = 64;
    sight_offset = 48;
    ai.state = 0;
    ++game_progress.level.enemy_total;
    model.flags |= 8;
    StartAnimation(0, 1);
    health = 120;
    ai.parameters[0] = 60;
    ai.parameters[1] = 40;
    AllocateBehaviorTable(1, 2);
    SetBehaviorVal(0, 1, 10);
    height = level_environment.water_y;
    model.pos.y = height;
    return height;
}

uint32 Croc::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80071E80, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    VisualObject::ClearTrackedHealth();
    Enemy::FreeBehaviorTable();
    result = Enemy::Destroy(0);
    if (reason & 1)
        result = ReleaseStorage();
    return result;
}

uint32 Croc::Update()
{
    RecordDataBorrow rec_borrow;
    ShortVector3 motion_target;
    uint16 delay, speed, angle;
    sint32 distance;
    uint8 count, jitter;
    uint32 axis;
    GameHeapBlock *particle;
    Vector3 pos;
    ModelInstance *player;
    FUNCTION_MARKER(0x80071EF0, "SLUS_003.73");
    if (!control.hierarchy_ready)
    {
        model.SetAttachmentTable(CrocAttachments);
        control.hierarchy_ready = 1;
    }
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
    if (control.flags & 1)
    {
        distance = model.pos.Distance(dst);
        speed = ai.parameters[0];
        if (speed * 2 < distance)
        {
            angle = (3072 - ratan2((sint32)((uint32)dst.z - model.pos.z), (sint32)((uint32)dst.x - model.pos.x))) & 4095;
            speed = ai.parameters[1];
            motion_target.x = 0;
            motion_target.y = angle;
            motion_target.z = 0;
            UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, speed);
            motion.velocity.SetDir(ai.parameters[0], model.angles);
        }
        else
        {
            control.flags &= ~1;
        }
    }
    if (model.flags & 1)
    {
        use_small_effect_pool = false;
        count = RandomRange(3) + 3;
        while (count)
        {
            pos = model.pos;
            for (axis = 0; axis < 3; ++axis)
            {
                jitter = RandomRange(256);
                pos.comp[axis] = (uint32)pos.comp[axis] + ((jitter - 128) * ONE);
            }
            particle = EffectObject::AllocateStorage(116);
            if (particle)
            {
                PanelParticle *panel = new (particle) PanelParticle(&pos);
                panel->depth_bias = 64;
            }
            else
            {
                GameDiagnostic(("Croc panel allocation failed\n"));
                abort();
            }
            --count;
        }
        use_small_effect_pool = true;
    }
    if (model.animation != 1)
        return 1;
    GetAttachment(pos, 0);
    player = visual_lists.players;
    distance = player->pos.EuclideanDistance(pos);
    if (distance >= 256)
        return 0;
    player = visual_lists.players;
    return static_cast<VisualObject *>(player->owner)->Damage(4, &zero_vector, 0);
}

void Croc::SetParameters(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 val;
    uint32 i;
    RecordCursor base;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80072204, "SLUS_003.73");
    if (command != 0x2130)
    {
        Enemy::ReadParameters(command);
        return;
    }
    base = script_cursor + 2;
    script_cursor = base;
    for (i = 0; i < 7; ++i)
    {
        val = (base + i * 2).Read16();
        script_cursor = base + (i + 1) * 2;
        if (i < 4)
            phase_config.speed[i] = val;
        else
            phase_config.health_threshold[i - 4] = val;
    }
    for (i = 0; i < 4; ++i)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        cursor += 2;
        phase_config.opening_spawn[i] = val;
        script_cursor = cursor;
    }
    // Retain the last cursor rather than reloading it after the field store
    val = cursor.Read16();
    script_cursor = cursor + 2;
    phase_config.reinforcement_count = val;
    for (i = 0; i < 13; ++i)
    {
        cursor = script_cursor;
        val = cursor.Read16();
        phase_config.reinforcement_spawn[i] = val;
        script_cursor = cursor + 2;
    }
}

uint32 Croc::ExecuteCommand(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 val;
    uint32 ground;
    sint32 offset;
    RecordCursor cursor;
    Vector3 pos;
    FUNCTION_MARKER(0x80072320, "SLUS_003.73");
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
    SetDst(&pos);
    return 1;
}

void Croc::UpdateState()
{
    uint16 state, phase, flags, val, old_state, old_phase, steps;
    uint32 count;
    ModelInstance *player, *node;
    VisualObject *actor;
    uint32 damage, idx, maximum;
    uint32 floor, height;
    Vector3 dir, point, scaled;
    CollisionRay col;
    bool hit_ground;
    ShortVector3 angles;
    FUNCTION_MARKER(0x800723E4, "SLUS_003.73");
    if (!visual_lists.players)
        ai.state = 666;
    state = ai.state;
    if (state < 4)
    {
        if (state < 3)
        {
            if (health < phase_config.health_threshold[state])
            {
                if (!state)
                {
                    ai.state = 1;
                    ai.phase = 0;
                    for (idx = 0; idx < 4; ++idx)
                        ::SpawnRec(phase_config.opening_spawn[idx]);
                }
                else
                {
                    ai.state = state + 1;
                    ai.phase = 0;
                }
                return;
            }
        }
        else if (health <= 0)
        {
            node = visual_lists.actors;
            while (node)
            {
                actor = static_cast<VisualObject *>(node->owner);
                if (actor->kind == 200 && static_cast<Enemy *>(actor)->control.variant == 8)
                {
                    damage = actor->health;
                    actor->Damage(damage, &zero_vector, 0);
                }
                node = node->next;
            }
            ai.state = 8;
            ai.phase = 0;
            return;
        }
        if (state >= 2 && !(game_clock.tick & 7))
        {
            count = 0;
            node = visual_lists.actors;
            while (node)
            {
                actor = static_cast<VisualObject *>(node->owner);
                if (actor->kind == 200 && static_cast<Enemy *>(actor)->control.variant == 8)
                    ++count;
                node = node->next;
            }
            while ((sint32)count < phase_config.reinforcement_count)
            {
                idx = RandomRange(13);
                val = phase_config.reinforcement_spawn[idx];
                ++count;
                ::SpawnRec(val);
            }
        }
    }
    if (state <= 4)
    {
        phase = ai.phase;
        switch (phase)
        {
            case 0:
                if (state < 4)
                {
                    val = phase_config.speed[state];
                    flags = model.flags;
                    ai.work[0] = 0;
                    ai.parameters[0] = val;
                }
                else
                {
                    flags = model.flags;
                    ai.work[0] = 0;
                    flags |= 0x200;
                    model.flags = flags;
                    model.scale.z = 2048;
                    model.scale.y = 2048;
                    model.scale.x = 2048;
                }
                if (flags & 1)
                    break;
                old_state = ai.state;
                old_phase = ai.phase;
                ai.state = 16;
                ai.phase = 0;
                ai.parameters[2] = old_state;
                ai.parameters[3] = old_phase + 1;
                return;
            case 1:
                steps = 0;
                if (state == 4)
                    val = RandomRange(4);
                else
                {
                    steps = ai.work[0];
                    val = state < 2 ? steps >= 2 : steps != 0;
                }
                if (val && player_distance >= 129)
                {
                    player = visual_lists.players;
                    angles.SetDir(model.pos, player->pos);
                    model.angles.y = angles.y;
                    ClearAngularVelocity();
                    old_state = ai.state;
                    ai.parameters[3] = 3;
                    motion.angular_acceleration.x = 0;
                    ai.state = 17;
                    ai.phase = 0;
                    ai.parameters[2] = old_state;
                    return;
                }
                maximum = state == 2 || state == 3 ? 2048 : 1600;
                if (!ChoosePos(&dst, &model.pos, 1024, maximum, 128, 4096, 1, 1))
                {
                    motion.velocity.z = 0;
                    motion.velocity.y = 0;
                    motion.velocity.x = 0;
                    return;
                }
                if (!state)
                {
                    if (level_environment.area_kind == 1 && !RandomRange(3))
                        PlaySoundEffect(64, 0x3000, 0);
                }
                else if (!RandomRange(3) && level_environment.area_kind == 1)
                    PlaySoundEffect(64, 0x3000, 0);
                if (state < 4)
                    ai.work[0] = steps + 1;
                SetDst(&dst);
                break;
            case 2:
                if (!(control.flags & 1))
                    --ai.phase;
                return;
            case 3:
                angles.x = 0;
                angles.y = model.angles.y;
                angles.z = 0;
                dir.SetDir(1, angles);
                point.Multiply(dir, 1600);
                point.Add(model.pos);
                col.endpoints[0] = model.pos;
                col.endpoints[1] = point;
                col.Initialize();
                col_query.allow_surface20 = true;
                col.Query(1);
                hit_ground = col.hit.GetObject() != 0;
                if (state < 2)
                    col_query.allow_surface20 = false;
                if (hit_ground)
                {
                    if ((sint32)col.distance < 129)
                    {
                        ai.phase = 0;
                        return;
                    }
                    point = col.hit_pos;
                }
                scaled.Multiply(dir, 128);
                point.Subtract(scaled);
                ClearAngularVelocity();
                motion.angular_acceleration.x = 0;
                StartAnimation(1, 1);
                ai.parameters[0] *= 2;
                SetDst(&point);
                if (level_environment.area_kind == 1)
                    PlaySoundEffect(66, 0x3000, 0);
                break;
            case 4:
                if (!(control.flags & 1))
                {
                    StartAnimation(0, 1);
                    val = ai.parameters[0];
                    ai.phase = 0;
                    ai.parameters[0] = val >> 1;
                }
                return;
            default:
                return;
        }
        ++ai.phase;
        return;
    }
    switch (state)
    {
        case 8:
            if (!RandomRange(4) && level_environment.area_kind == 1)
                PlaySoundEffect(65, 0x3000, 0);
            floor = level_environment.water_y;
            height = (uint32)model.pos.y;
            if ((sint32)(floor + 0x8000) < (sint32)height)
                model.pos.y = height - 0x8000;
            else if ((sint32)height < (sint32)(floor - 0x8000))
                model.pos.y = height + 0x8000;
            phase = ai.phase;
            if (!phase)
            {
                flags = lifecycle_flags;
                val = model.flags;
                lifecycle_flags = flags & 0xFFEF;
                model.flags = val & 0xFFFE;
                SetAnimationRange(2, 0, -1);
                ++ai.phase;
            }
            else if (phase == 1 && playback.complete)
            {
                Finish();
            }
            return;
        case 16:
            floor = level_environment.water_y;
            height = (uint32)model.pos.y;
            if ((sint32)height < (sint32)(floor + 0x40000))
                model.pos.y = height + 0x8000;
            else
            {
                flags = lifecycle_flags;
                old_state = ai.parameters[2];
                old_phase = ai.parameters[3];
                lifecycle_flags = flags & 0xFFEF;
                val = model.flags;
                ai.state = old_state;
                ai.phase = old_phase;
                model.flags = val | 1;
            }
            return;
        case 17:
            height = (uint32)model.pos.y;
            floor = level_environment.water_y;
            if ((sint32)floor < (sint32)height)
            {
                flags = model.flags;
                val = lifecycle_flags;
                model.flags = flags & 0xFFFE;
                height = (uint32)model.pos.y;
                lifecycle_flags = val | 0x10;
                model.pos.y = height - 0x8000;
            }
            else
            {
                old_state = ai.parameters[2];
                old_phase = ai.parameters[3];
                model.pos.y = floor;
                ai.state = old_state;
                ai.phase = old_phase;
            }
            return;
        case 666:
            ai.state = 0;
            control.flags &= ~1;
            return;
        default:
            return;
    }
}

uint32 Croc::SetDst(const Vector3 *pos)
{
    uint32 flags;
    FUNCTION_MARKER(0x80073680, "SLUS_003.73");
    dst = *pos;
    flags = control.flags | 1;
    control.flags = flags;
    return flags;
}

uint32 Croc::Damage(uint32 damage, const Vector3 *, uint32)
{
    uint32 mode;
    ModelInstance *player;
    sint16 health;
    FUNCTION_MARKER(0x800736AC, "SLUS_003.73");
    ++control.damage_phase;
    health = this->health;
    if (health <= 0)
        return 1;
    this->health = health - damage;
    if (!RandomRange(16))
    {
        mode = level_environment.area_kind;
        if (mode == 1)
            PlaySoundEffect(65, 0x3000, 0);
    }
    health = this->health;
    if (health > 0)
        SetColor(3, 255, 255, 255);
    else
    {
        VisualObject::ClearTrackedHealth();
        player = visual_lists.players;
        if (player)
        {
            static_cast<Player *>(player->owner)->TryPlayKillQuote();
        }
        mode = level_environment.area_kind;
        lifecycle_flags &= 0xFFEF;
        if (mode == 5)
        {
            ai.state = 8;
            ai.phase = 0;
        }
    }
    return 1;
}

void Croc::ClearAngularVelocity()
{
    motion.angular_velocity.z = 0;
    motion.angular_velocity.y = 0;
    motion.angular_velocity.x = 0;
    motion.angular_acceleration.z = 0;
    motion.angular_acceleration.y = 0;
}

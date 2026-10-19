#include <new>
#include <stdio.h>
#include <stdlib.h>

#include "game_heap.h"
#include "effect_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"

#include "world_gravity.h"

#include "object_initializers.h"

#include "visual_object.h"
#include "record_factory.h"
#include "vector_arithmetic.h"
#include "angle_math.h"
#include "object_damage.h"
#include "collision.h"
#include "model_animation.h"
#include "model_attachment.h"
#include "game_diagnostics.h"
#include "player.h"
#include "audio_session.h"
#include "game_random.h"
#include "vector_distance.h"
#include "game_panel.h"

#include "croc.h"
#include "record_data.h"
#include "panel_particle.h"
#include "game_object_allocation.h"

Croc::Croc(const RecordCursor &record_data, uint32 index) : Enemy()
{
    RecordDataBorrow record_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80071CEC, "SLUS_003.73");

    this->kind = 212;
    SetModelBank("croc");
    cursor = ReadPosition(record_data);
    cursor = ReadAngles(cursor);
    AddToList(visual_lists.actors);
    script_cursor = cursor;
    this->record_idx = index;
    Initialize();
    cursor = script_cursor;
    if (cursor)
    {
        this->control.script_active = 1;
        ExecuteScript(RecordCursor(cursor));
    }
    if (level_environment.area_kind == 1)
        TrackObjectHealth(*this, 24, 42, 4096);
}

uint32 Croc::Initialize()
{
    uint32 count, flags, height;
    FUNCTION_MARKER(0x80071DD4, "SLUS_003.73");
    this->ground_offset = 64;
    this->sight_offset = 48;
    count = game_progress.level.enemy_total;
    flags = this->model.flags;
    ai.state = 0;
    game_progress.level.enemy_total = count + 1;
    this->model.flags = flags | 8;
    StartAnimation(0, 1);
    this->health = (sint16)(uint16)(120);
    ai.parameters[0] = 60;
    ai.parameters[1] = 40;
    AllocateBehaviorTable(1, 2);
    SetBehaviorValue(0, 1, 10);
    height = (uint32)level_environment.water_y;
    this->model.pos.words[1] = height;
    return height;
}

uint32 Croc::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80071E80, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    ClearTrackedObjectHealth();
    Enemy::FreeBehaviorTable();
    result = Enemy::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(this);
    return result;
}

uint32 Croc::Update()
{
    RecordDataBorrow record_borrow;
    ShortVector3 motion_target;
    uint32 x, y, z, delay, distance, speed, left, right, angle;
    uint32 flags;
    uint32 count;
    GameHeapBlock *particle;
    Vector3 position;
    ModelInstance *player;
    FUNCTION_MARKER(0x80071EF0, "SLUS_003.73");
    if (!(uint32)this->control.hierarchy_ready)
    {
        SetModelBoneTable(this->model.bank, CrocAttachments);
        this->control.hierarchy_ready = 1;
    }
    x = (uint32)zero_vector[0];
    y = (uint32)zero_vector[1];
    z = (uint32)zero_vector[2];
    motion.acceleration.words[0] = x;
    motion.acceleration.words[1] = y;
    motion.acceleration.words[2] = z;
    UpdateWorldTransform();
    if ((uint32)this->control.script_active)
    {
        delay = (uint32)this->control.script_delay;
        if (delay)
            this->control.script_delay = delay - 1u;
        else
            ExecuteScript(script_cursor);
    }
    else
        UpdateState();
    if ((uint32)this->control.gravity_enabled)
        AddVectors(motion.acceleration.words, world_gravity.vector.words);
    IntegrateMotion();
    if (this->control.flags & 1u)
    {
        distance = MeasurePositionDistance(this->model.pos.words, this->destination.words);
        speed = ai.parameters[0];
        if ((sint32)(speed * 2u) < (sint32)distance)
        {
            left = this->destination.words[2];
            right = this->model.pos.words[2];
            z = left - right;
            left = this->destination.words[0];
            right = this->model.pos.words[0];
            angle = (3072u - (uint32)ratan2((sint32)z, (sint32)(left - right))) & 4095u;
            speed = ai.parameters[1];
            motion_target.x = 0;
            motion_target.y = angle;
            motion_target.z = 0;
            UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, speed);
            BuildDirectionVector(motion.velocity.words, ai.parameters[0], this->model.angles.words);
        }
        else
        {
            flags = this->control.flags;
            this->control.flags = flags & 0xFFFFFFFE;
        }
    }
    if (this->model.flags & 1u)
    {
        use_small_effect_pool = false;
        count = RandomRange(3) + 3u;
        while (count)
        {
            position = this->model.pos;
            position.words[0] += (RandomRange(256) - 128u) << 12u;
            position.words[1] += (RandomRange(256) - 128u) << 12u;
            position.words[2] += (RandomRange(256) - 128u) << 12u;
            particle = AllocateEffectStorage(116);
            if (particle)
            {
                PanelParticle *panel = new (particle) PanelParticle(position.words);
                panel->depth_bias = 64;
            }
            else
            {
                GameDiagnostic("Croc panel allocation failed\n");
                abort();
            }
            --count;
        }
        use_small_effect_pool = true;
    }
    if (this->model.animation != 1u)
        return 1;
    GetAttachment(position.words, 0);
    player = visual_lists.players;
    distance = MeasureEuclideanDistance(player->pos.words, position.words);
    if ((sint32)distance >= 256)
        return 0;
    player = visual_lists.players;
    return static_cast<VisualObject *>(player->owner)->Damage(4, zero_vector.components, 0);
}

uint32 Croc::Damage(uint32 damage, const sint32 *, uint32)
{
    uint32 hits, health_word, flags, mode;
    ModelInstance *player;
    sint32 health;
    FUNCTION_MARKER(0x800736AC, "SLUS_003.73");
    hits = (uint32)this->control.damage_phase;
    this->control.damage_phase = hits + 1;
    health = this->health;
    health_word = (uint16)this->health;
    if (health <= 0)
        return 1;
    this->health = (sint16)(uint16)(health_word - damage);
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
        ClearTrackedObjectHealth();
        player = visual_lists.players;
        if (player)
        {
            TryPlayPlayerKillQuote(player->pos.words);
        }
        flags = this->lifecycle_flags;
        mode = level_environment.area_kind;
        this->lifecycle_flags = flags & 0xFFEF;
        if (mode == 5)
        {
            ai.state = 8;
            ai.phase = 0;
        }
    }
    return 1;
}

void Croc::SetParameters(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value, i;
    RecordCursor base;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80072204, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x2130)
    {
        Enemy::ReadParameters(command);
        return;
    }
    base = script_cursor + 2;
    script_cursor = base;
    for (i = 0; i < 7; ++i)
    {
        value = (base + i * 2).Read16();
        script_cursor = base + (i + 1) * 2;
        if (i < 4)
            phase_config.speed[i] = value;
        else
            phase_config.health_threshold[i - 4] = value;
    }
    for (i = 0; i < 4; ++i)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        cursor += 2;
        phase_config.opening_spawn[i] = value;
        script_cursor = cursor;
    }
    // Retain the last cursor rather than reloading it after the field store
    value = cursor.Read16();
    script_cursor = cursor + 2;
    phase_config.reinforcement_count = value;
    for (i = 0; i < 13; ++i)
    {
        cursor = script_cursor;
        value = cursor.Read16();
        phase_config.reinforcement_spawn[i] = value;
        script_cursor = cursor + 2;
    }
}

uint32 Croc::ExecuteCommand(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value, ground, height;
    RecordCursor cursor;
    Vector3 position;
    FUNCTION_MARKER(0x80072320, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x4221)
        return Enemy::ExecuteCommand(command);
    cursor = script_cursor;
    value = cursor.Read16();
    script_cursor = cursor + 2;
    if (value & 0x2000)
    {
        value = ResolveParameter(value);
    }
    ReadRecordPosition(position.words, value & 0xFFFF);
    ground = FindGroundHeight(position.words, 0, 2048);
    if (ground != (uint32)-1)
    {
        height = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
        position.words[1] = ground - (height << 12);
    }
    Croc::SetDestination(position.words);
    return 1;
}

void Croc::ClearAngularVelocity()
{
    motion.angular_velocity.words[2] = 0;
    motion.angular_velocity.words[1] = 0;
    motion.angular_velocity.words[0] = 0;
    motion.angular_acceleration.words[2] = 0;
    motion.angular_acceleration.words[1] = 0;
}

void Croc::UpdateState()
{
    uint32 state, phase, flags, value, old_state, old_phase, count;
    ModelInstance *player, *node;
    uint32 damage, index, steps, maximum;
    uint32 scale, floor, height;
    Vector3 direction, point, scaled;
    CollisionRay collision;
    bool hit_ground;
    ShortVector3 angles;
    FUNCTION_MARKER(0x800723E4, "SLUS_003.73");
    if (!visual_lists.players)
        ai.state = 666;
    state = ai.state;
    if (state < 4u)
    {
        if (state < 3u)
        {
            if ((sint32)this->health < (sint32)phase_config.health_threshold[state])
            {
                if (!state)
                {
                    ai.state = 1;
                    ai.phase = 0;
                    for (index = 0; index < 4u; ++index)
                        ::SpawnRecord(phase_config.opening_spawn[index]);
                }
                else
                {
                    ai.state = state + 1u;
                    ai.phase = 0;
                }
                return;
            }
        }
        else if (this->health <= 0)
        {
            node = visual_lists.actors;
            while (node)
            {
                if (static_cast<VisualObject *>(node->owner)->kind == 200u && (uint32) static_cast<Enemy *>(node->owner)->control.variant == 8u)
                {
                    damage = (uint32)(sint32) static_cast<VisualObject *>(node->owner)->health;
                    static_cast<VisualObject *>(node->owner)->Damage(damage, zero_vector.components, 0);
                }
                node = node->next;
            }
            ai.state = 8;
            ai.phase = 0;
            return;
        }
        if (state >= 2u && !(game_clock.tick & 7u))
        {
            count = 0;
            node = visual_lists.actors;
            while (node)
            {
                if (static_cast<VisualObject *>(node->owner)->kind == 200u && (uint32) static_cast<Enemy *>(node->owner)->control.variant == 8u)
                    ++count;
                node = node->next;
            }
            while ((sint32)count < (sint32)phase_config.reinforcement_count)
            {
                index = RandomRange(13);
                value = phase_config.reinforcement_spawn[index];
                ++count;
                ::SpawnRecord(value);
            }
        }
    }
    if (state <= 4u)
    {
        phase = ai.phase;
        switch (phase)
        {
            case 0:
                if (state < 4u)
                {
                    value = phase_config.speed[state];
                    flags = this->model.flags;
                    ai.work[0] = 0;
                    ai.parameters[0] = value;
                }
                else
                {
                    flags = this->model.flags;
                    ai.work[0] = 0;
                    flags |= 0x200u;
                    this->model.flags = flags;
                    this->model.scale.words[2] = 2048;
                    this->model.scale.words[1] = 2048;
                    this->model.scale.words[0] = 2048;
                }
                if (flags & 1u)
                    break;
                old_state = ai.state;
                old_phase = ai.phase;
                ai.state = 16;
                ai.phase = 0;
                ai.parameters[2] = old_state;
                ai.parameters[3] = old_phase + 1u;
                return;
            case 1:
                steps = 0;
                if (state == 4u)
                    value = RandomRange(4);
                else
                {
                    steps = ai.work[0];
                    value = state < 2u ? steps >= 2u : steps != 0u;
                }
                if (value && this->player_distance >= 129u)
                {
                    player = visual_lists.players;
                    CalculateDirectionAngles(angles.words, model.pos.words, player->pos.words);
                    this->model.angles.words[1] = (uint16)angles.components[1];
                    ClearAngularVelocity();
                    old_state = ai.state;
                    ai.parameters[3] = 3;
                    motion.angular_acceleration.words[0] = 0;
                    ai.state = 17;
                    ai.phase = 0;
                    ai.parameters[2] = old_state;
                    return;
                }
                maximum = state == 2u || state == 3u ? 2048u : 1600u;
                if (!ChoosePosition(this->destination.words, model.pos.words, 1024u, maximum, 128u, 4096u, 1u, 1u))
                {
                    motion.velocity.words[2] = 0;
                    motion.velocity.words[1] = 0;
                    motion.velocity.words[0] = 0;
                    return;
                }
                if (!state)
                {
                    if (level_environment.area_kind == 1u && !RandomRange(3))
                        PlaySoundEffect(64, 0x3000, 0);
                }
                else if (!RandomRange(3) && level_environment.area_kind == 1u)
                    PlaySoundEffect(64, 0x3000, 0);
                if (state < 4u)
                    ai.work[0] = steps + 1u;
                SetDestination(this->destination.words);
                break;
            case 2:
                if (!(this->control.flags & 1u))
                    ai.phase = ai.phase - 1u;
                return;
            case 3:
                value = this->model.angles.words[1];
                angles.components[0] = 0;
                angles.components[2] = 0;
                angles.components[1] = (sint16)value;
                BuildDirectionVector(direction.words, 1, angles.words);
                scale = 1600;
                MultiplyVector(scaled.words, direction.words, scale);
                point = scaled;
                AddVectors(point.words, model.pos.words);
                collision.endpoints[0].words[0] = this->model.pos.words[0];
                collision.endpoints[0].words[1] = this->model.pos.words[1];
                collision.endpoints[0].words[2] = this->model.pos.words[2];
                collision.endpoints[1].words[0] = point.words[0];
                collision.endpoints[1].words[1] = point.words[1];
                collision.endpoints[1].words[2] = point.words[2];
                InitializeCollisionRay(collision);
                collision_query.allow_surface20 = true;
                QueryCollisionRay(collision, 1u);
                hit_ground = collision.hit.GetObject() != 0;
                if (state < 2u)
                    collision_query.allow_surface20 = false;
                if (hit_ground)
                {
                    if ((sint32)collision.distance < 129)
                    {
                        ai.phase = 0;
                        return;
                    }
                    point = collision.hit_pos;
                }
                scale = 128;
                MultiplyVector(scaled.words, direction.words, scale);
                point.words[0] -= scaled.words[0];
                point.words[1] -= scaled.words[1];
                point.words[2] -= scaled.words[2];
                ClearAngularVelocity();
                motion.angular_acceleration.words[0] = 0;
                StartAnimation(1u, 1u);
                value = ai.parameters[0];
                ai.parameters[0] = value * 2u;
                SetDestination(point.words);
                if (level_environment.area_kind == 1u)
                    PlaySoundEffect(66, 0x3000, 0);
                break;
            case 4:
                if (!(this->control.flags & 1u))
                {
                    StartAnimation(0u, 1u);
                    value = ai.parameters[0];
                    ai.phase = 0;
                    ai.parameters[0] = value >> 1u;
                }
                return;
            default:
                return;
        }
        ai.phase = ai.phase + 1u;
        return;
    }
    switch (state)
    {
        case 8:
            if (!RandomRange(4) && level_environment.area_kind == 1u)
                PlaySoundEffect(65, 0x3000, 0);
            floor = (uint32)level_environment.water_y;
            height = this->model.pos.words[1];
            if ((sint32)(floor + 0x8000u) < (sint32)height)
                this->model.pos.words[1] = height - 0x8000u;
            else if ((sint32)height < (sint32)(floor - 0x8000u))
                this->model.pos.words[1] = height + 0x8000u;
            phase = ai.phase;
            if (!phase)
            {
                flags = this->lifecycle_flags;
                value = this->model.flags;
                this->lifecycle_flags = flags & 0xFFEFu;
                this->model.flags = value & 0xFFFEu;
                SetAnimationRange(2u, 0u, 0xFFFFFFFF);
                ai.phase = ai.phase + 1u;
            }
            else if (phase == 1u && this->playback.complete)
            {
                Finish();
            }
            return;
        case 16:
            floor = (uint32)level_environment.water_y;
            height = this->model.pos.words[1];
            if ((sint32)height < (sint32)(floor + 0x40000u))
                this->model.pos.words[1] = height + 0x8000u;
            else
            {
                flags = this->lifecycle_flags;
                old_state = ai.parameters[2];
                old_phase = ai.parameters[3];
                this->lifecycle_flags = flags & 0xFFEFu;
                value = this->model.flags;
                ai.state = old_state;
                ai.phase = old_phase;
                this->model.flags = value | 1u;
            }
            return;
        case 17:
            height = this->model.pos.words[1];
            floor = (uint32)level_environment.water_y;
            if ((sint32)floor < (sint32)height)
            {
                flags = this->model.flags;
                value = this->lifecycle_flags;
                this->model.flags = flags & 0xFFFEu;
                height = this->model.pos.words[1];
                this->lifecycle_flags = value | 0x10u;
                this->model.pos.words[1] = height - 0x8000u;
            }
            else
            {
                old_state = ai.parameters[2];
                old_phase = ai.parameters[3];
                this->model.pos.words[1] = floor;
                ai.state = old_state;
                ai.phase = old_phase;
            }
            return;
        case 666:
            flags = this->control.flags;
            ai.state = 0;
            this->control.flags = flags & 0xFFFFFFFE;
            return;
        default:
            return;
    }
}

uint32 Croc::SetDestination(const uint32 *position)
{
    uint32 x, y, z, flags;
    FUNCTION_MARKER(0x80073680, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    this->destination.words[0] = x;
    this->destination.words[1] = y;
    this->destination.words[2] = z;
    flags = this->control.flags | 1;
    this->control.flags = flags;
    return flags;
}

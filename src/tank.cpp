#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "effect_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"

#include "record_scripts.h"

#include "collision.h"

#include "world_gravity.h"

#include "object_initializers.h"

#include "vector_distance.h"
#include "camera.h"

#include "flamethrower.h"
#include "projectile_weaving.h"
#include "model_resources.h"
#include "visual_object.h"
#include "angle_math.h"
#include "model_animation.h"
#include "model_attachment.h"
#include "game_random.h"
#include "game_diagnostics.h"
#include "player.h"
#include "game_panel.h"
#include "fire_cycle.h"

#include "tank.h"
#include "projectile_base.h"
#include "record_data.h"
#include "vector_arithmetic.h"
#include "particle_debris.h"
#include "effect_splash.h"
#include "ring_burst_effect.h"
#include "game_object_allocation.h"
#include "game_heap.h"
#include "audio_voice.h"
#include "audio_session.h"
#include "items.h"

Tank::Tank(const RecordCursor &record_data, uint32 index) : Enemy(), fire_mask(0), debris_first(0), debris_count(0), move_sound(0)
{
    RecordDataBorrow record_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80055F14, "SLUS_003.73");
    aim_pos.words[0] = aim_pos.words[1] = aim_pos.words[2] = 0;
    aim_offset.words[0] = aim_offset.words[1] = aim_offset.words[2] = 0;

    cursor = ReadPosition(record_data);
    cursor = ReadAngles(cursor);
    script_cursor = cursor;
    this->control.script_active = 1;
    this->record_idx = index;
    Init();
    cursor = script_cursor;
    ExecuteScript(RecordCursor(cursor));
}

Tank::Tank(const uint32 *position_vector, const RecordCursor &script) : Enemy(), fire_mask(0), debris_first(0), debris_count(0), move_sound(0)
{
    RecordDataBorrow record_borrow;
    uint32 x, y, z;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80055F9C, "SLUS_003.73");
    aim_pos.words[0] = aim_pos.words[1] = aim_pos.words[2] = 0;
    aim_offset.words[0] = aim_offset.words[1] = aim_offset.words[2] = 0;
    x = position_vector[0];
    y = position_vector[1];
    z = position_vector[2];
    pos_history.saved.words[0] = x;
    pos_history.saved.words[1] = y;
    pos_history.saved.words[2] = z;
    this->model.pos.words[0] = x;
    this->model.pos.words[1] = y;
    this->model.pos.words[2] = z;
    script_cursor = script;
    this->control.script_active = 1u;
    Init();
    cursor = script_cursor;
    ExecuteScript(script_cursor);
}

uint32 Tank::Init()
{
    uint32 flags;
    GameHeapBlock *child;
    uint32 value;
    uint32 bank;
    FUNCTION_MARKER(0x80056038, "SLUS_003.73");
    game_progress.level.enemy_total = game_progress.level.enemy_total + 1u;
    AddToList(visual_lists.solids);
    this->kind = 205;
    flags = this->model.flags;
    motion.drag[0] = 3;
    this->model.flags = flags | 16u;
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    AllocateBehaviorTable(3, 9);
    this->control.gravity_enabled = 0;
    ai.state = 0;
    ai.phase = 0;
    this->control.hierarchy_ready = 0;
    SetBehaviorValue(0, 0, 0);
    SetBehaviorValue(0, 1, 30);
    SetBehaviorValue(0, 2, 64);
    SetBehaviorValue(0, 3, 64);
    SetBehaviorValue(0, 4, 64);
    SetBehaviorValue(0, 5, 0);
    SetBehaviorValue(1, 0, 0);
    SetBehaviorValue(1, 1, 1);
    SetBehaviorValue(1, 2, 128);
    SetBehaviorValue(1, 3, 12);
    SetBehaviorValue(1, 4, 64);
    SetBehaviorValue(1, 5, 0);
    SetBehaviorValue(2, 0, 0);
    SetBehaviorValue(2, 1, 20);
    SetBehaviorValue(2, 2, 30);
    SetBehaviorValue(2, 3, 512);
    SetBehaviorValue(2, 4, 8);
    SetBehaviorValue(2, 5, 300);
    SetBehaviorValue(2, 6, 256);
    SetBehaviorValue(2, 7, 128);
    SetBehaviorValue(2, 8, 0);
    child = AllocateZeroedHeapBlock(72);
    Flamethrower *created = 0;
    if (child)
    {
        created = new (child) Flamethrower(&visual_lists.players, 0);
    }
    weapon = created;
    value = GetBehaviorValue(1, 1);
    bank = GetRecordModelBankSlot();
    if (!weapon)
    {
        GameDiagnostic("Tank flamethrower allocation failed\n");
        abort();
    }
    weapon->damage = value & 65535;
    value = ::FindModelResourceIndex(0xA48DD5EC, bank);
    this->debris_first = value;
    this->debris_count = 10u;
    return 10;
}

Tank::~Tank()
{
    if (weapon)
        weapon->Destroy(3);
    if (move_sound)
        StopSoundVoices(move_sound);
}

uint32 Tank::Destroy(uint32 reason)
{
    uint32 flags, sound;
    FUNCTION_MARKER(0x800562C4, "SLUS_003.73");
    RemoveFromList(visual_lists.solids);
    Enemy::FreeBehaviorTable();
    if (this->control.flags & 32)
    {
        ClearTrackedObjectHealth();
        flags = this->control.flags;
        this->control.flags = flags & 0xFFFFFFDF;
    }
    if (weapon)
        weapon->Destroy(3);
    weapon = 0;
    sound = this->move_sound;
    if (sound)
        StopSoundVoices(sound);
    move_sound = 0;
    if (camera_subject.GetObject() == &model)
        camera_subject.Reset();
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : 0;
}

uint32 Tank::Update()
{
    RecordDataBorrow record_borrow;
    ShortVector3 motion_target;
    uint32 flags, value, count, mode, sound;
    Vector3 position;
    ShortVector3 desired;
    sint32 wanted_yaw, current_yaw;
    uint32 move;
    FUNCTION_MARKER(0x800563B4, "SLUS_003.73");
    position = zero_vector;
    motion.acceleration = position;
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
        flags = this->model.flags;
        this->control.hierarchy_ready = 1;
        this->model.flags = flags | 4u;
        SetAnimationRange(0u, 0u, 0xFFFFFFFF);
    }
    if ((uint32)this->control.script_active)
    {
        count = (uint32)this->control.script_delay;
        if (count)
            this->control.script_delay = count - 1u;
        else
            ExecuteScript(script_cursor);
    }
    else
        UpdateState();
    if (!(this->model.flags & 4u))
        return 0;
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
        mode = (uint32)this->fire_mask;
    }
    else if (flags & 8u)
    {
        AddVectors(position.words, model.pos.words, this->aim_offset.words);
        this->aim_pos = position;
        mode = (uint32)this->fire_mask;
    }
    else
        mode = (flags & 4u) ? (uint32)this->fire_mask : 0u;
    Aim(mode);
    if (this->control.flags & 1u)
    {
        if ((sint32)MeasureHorizontalDistance(this->model.pos.words, this->destination.words) < 128)
        {
            flags = this->control.flags;
            this->control.flags = flags & 0xFFFFFFFE;
        }
        else
        {
            sound = this->move_sound;
            if (sound)
            {
                if (!(game_clock.tick & 15u))
                    UpdatePositionedSoundVoices(sound, model.pos.words, 0);
            }
            else
                this->move_sound = PlayPositionedSound(49, model.pos.words, 0);
            CalculateDirectionAngles(desired.words, this->model.pos.words, this->destination.words);
            desired.words[0] = 0;
            motion_target.x = 0;
            motion_target.y = desired.words[1];
            motion_target.z = 0;
            UpdateAngleAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, 16u);
            this->model.angles.words[0] = 0;
            wanted_yaw = (sint16)desired.words[1];
            current_yaw = (sint16)this->model.angles.words[1];
            move = wanted_yaw == current_yaw;
            if (wanted_yaw < current_yaw)
                move = (uint32)(current_yaw - wanted_yaw) - 64u >= 0xF81u;
            else if (wanted_yaw > current_yaw)
                move = (uint32)(wanted_yaw - current_yaw) - 64u >= 0xF81u;
            if (move)
                BuildDirectionVector(motion.velocity.words, ai.parameters[0], desired.words);
            else
            {
                motion.velocity.words[2] = 0;
                motion.velocity.words[1] = 0;
                motion.velocity.words[0] = 0;
            }
        }
    }
    else
    {
        sound = this->move_sound;
        if (sound)
        {
            ::StopSoundVoices(sound);
            this->move_sound = 0u;
        }
    }
    if ((uint32)this->control.gravity_enabled)
        AddVectors(motion.acceleration.words, world_gravity.vector.words);
    IntegrateMotion();
    AddAngleVector(joints[0].angles.words, joints[0].velocity.words);
    NormalizeAngles(joints[0].angles.words);
    AddAngleVector(joints[0].velocity.words, joints[0].acceleration.words);
    DampAngles(joints[0].velocity.words, motion.angular_drag);
    SnapSmallAngles(joints[0].velocity.words);
    GetModelJointAngles(1).rotation.words[1] = joints[0].angles.words[1];
    GetModelJointAngles(2).rotation.words[0] = joints[0].angles.words[0];
    value = GetModelJointAngles(2).rotation.words[0];
    if (value - 129u < 0x77Fu)
        GetModelJointAngles(2).rotation.words[0] = (uint16)(128u);
    AddAngleVector(joints[1].angles.words, joints[1].velocity.words);
    NormalizeAngles(joints[1].angles.words);
    AddAngleVector(joints[1].velocity.words, joints[1].acceleration.words);
    DampAngles(joints[1].velocity.words, motion.angular_drag);
    SnapSmallAngles(joints[1].velocity.words);
    GetModelJointAngles(3).rotation.words[1] = joints[1].angles.words[1];
    UpdateWorldTransform();
    BuildModelPose(*this);
    if (GetBehaviorValue(0, 0) & 0xFFFFu)
    {
        this->fire_mask = (uint32)this->fire_mask | 1u;
        count = GetBehaviorValue(0, 5);
        if (count & 0xFFFFu)
            SetBehaviorValue(0, 5, (count - 1u) & 0xFFFFu);
        else
        {
            FireProjectile();
            value = GetBehaviorValue(0, 3);
            SetBehaviorValue(0, 5, value & 0xFFFFu);
        }
    }
    else
        this->fire_mask = (uint32)this->fire_mask & 0xFEu;
    mode = (GetBehaviorValue(1, 0) & 0xFFFFu) != 0u;
    flags = (uint32)this->fire_mask;
    this->fire_mask = mode ? flags | 2u : flags & 0xFDu;
    FireBeam(mode);
    value = GetBehaviorValue(2, 0) & 0xFFFFu;
    if (!value)
        return value;
    count = GetBehaviorValue(2, 8);
    if (count & 0xFFFFu)
        value = (count - 1u) & 0xFFFFu;
    else
    {
        FireDebris();
        value = GetBehaviorValue(2, 2) & 0xFFFFu;
    }
    SetBehaviorValue(2, 8, value);
    return 0;
}

uint32 Tank::FireProjectile()
{
    uint32 yaw;
    uint32 pitch;
    Vector3 position;
    GameHeapBlock *child;
    uint32 value;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80056980, "SLUS_003.73");
    yaw = this->model.angles.words[1];
    yaw = (yaw + GetModelJointAngles(1).rotation.words[1]) & 0xFFFu;
    pitch = joints[0].angles.words[0];
    angles.words[0] = (uint16)pitch;
    angles.words[1] = (uint16)yaw;
    angles.words[2] = 0;
    GetModelAttachment(position.words, *this, 2u);
    WeavingProjectile *projectile = 0;
    child = AllocateZeroedObject(440);
    if (child)
    {
        projectile = new (child) WeavingProjectile(position.words, angles.components, 0, 4, 128, &visual_lists.players, zero_vector.words);
    }
    value = GetBehaviorValue(0, 1);
    value = ScaleDamageForDifficulty(value & 65535);
    if (!projectile)
    {
        GameDiagnostic("Tank projectile allocation failed\n");
        abort();
    }
    projectile->SetDamage(value);
    projectile->SetBlastRadius(512);
    return 512;
}

void Tank::FireBeam(uint32 fire)
{
    uint32 yaw, joint_yaw, pitch;
    Vector3 position;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80056A6C, "SLUS_003.73");
    if (!weapon)
        return;
    yaw = this->model.angles.words[1];
    joint_yaw = GetModelJointAngles(3).rotation.words[1];
    pitch = joints[1].angles.words[0];
    angles.words[0] = (uint16)pitch;
    angles.words[2] = 0;
    angles.words[1] = (uint16)((yaw + joint_yaw) & 0xFFFu);
    GetModelAttachment(position.words, *this, 3u);
    if (fire)
        weapon->Fire(position.words, angles.components);
    else
        weapon->Update(0, 0);
}

uint32 Tank::FireDebris()
{
    ObjectBorrow object_borrow;
    GameHeapBlock *child;
    uint32 damage;
    uint32 arm_radius;
    uint32 blast_radius;
    uint32 fuse_ticks;
    uint32 life_ticks;
    uint32 hover_height;
    FUNCTION_MARKER(0x80056B48, "SLUS_003.73");
    Items *item = 0;
    child = AllocateZeroedObject(328);
    if (child)
    {
        damage = GetBehaviorValue(2, 1) & 65535;
        arm_radius = GetBehaviorValue(2, 3) & 65535;
        blast_radius = GetBehaviorValue(2, 6) & 65535;
        fuse_ticks = GetBehaviorValue(2, 4) & 65535;
        life_ticks = GetBehaviorValue(2, 5) & 65535;
        hover_height = GetBehaviorValue(2, 7) & 65535;
        item = new (child) Items(model.pos.words, damage, arm_radius, blast_radius, fuse_ticks, life_ticks, hover_height);
    }
    if (!item)
    {
        GameDiagnostic("Tank debris allocation failed\n");
        abort();
    }
    item->motion.velocity.words[1] = 0xFFF80000;
    return 0xFFF80000;
}

uint32 Tank::Damage(uint32 damage, const sint32 *, uint32)
{
    uint32 remaining, flags, state;
    ModelInstance *hero;
    sint32 health;
    FUNCTION_MARKER(0x80056C50, "SLUS_003.73");
    this->control.damage_phase = (uint32)this->control.damage_phase + 1;
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
        state = this->control.flags;
        this->lifecycle_flags = flags & 0xFFEF;
        if (state & 32)
        {
            ClearTrackedObjectHealth();
            state = this->control.flags;
            this->control.flags = state & 0xFFFFFFDF;
        }
        this->control.script_active = 0;
        ai.state = 10;
        ai.phase = 0;
    }
    return 1;
}

void Tank::ClearVelocity()
{
    motion.velocity.words[2] = 0;
    motion.velocity.words[1] = 0;
    motion.velocity.words[0] = 0;
}

void Tank::ClearAngularVelocity()
{
    motion.angular_velocity.words[2] = 0;
    motion.angular_velocity.words[1] = 0;
    motion.angular_velocity.words[0] = 0;
    motion.angular_acceleration.words[2] = 0;
    motion.angular_acceleration.words[1] = 0;
    motion.angular_acceleration.words[0] = 0;
}

void Tank::SetState(uint32 state)
{
    ai.state = state;
    ai.phase = 0;
}

void Tank::UpdateState()
{
    uint32 state, step, flags, value, ground, height, x, z, sound;
    ModelInstance *hero;
    GameHeapBlock *child;
    Vector3 position, acceleration;
    uint32 scale;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80056D1C, "SLUS_003.73");
    /* The selected parent 563B4 discards this register-carried return */
    state = ai.state;
    if (state > 10u || (state >= 7u && state <= 9u))
        return;
    step = ai.phase;
    switch (state)
    {
        case 0:
            if (step == 0u)
            {
                hero = visual_lists.players;
                if (hero)
                {
                    z = hero->pos.words[2];
                    value = this->model.pos.words[2];
                    x = hero->pos.words[0];
                    height = this->model.pos.words[0];
                    value = (uint32)ratan2((sint32)(z - value), (sint32)(x - height));
                    this->model.angles.words[1] = 3072u - value;
                }
                ground = FindGroundHeight(model.pos.words, 0, 4096);
                if (ground == 0xFFFFFFFF)
                {
                    Finish();
                    return;
                }
                value = this->model.pos.words[1];
                height = (uint32)(sint32)(sint16)(uint32)this->ground_offset;
                if ((sint32)(ground - value) >> 12 < (sint32)(height << 1))
                {
                    this->model.pos.words[1] = ground - (height << 12);
                    ai.state = 1;
                    this->control.gravity_enabled = 0;
                    ClearVelocity();
                }
                else
                {
                    ai.work[0] = (uint32)((sint32)ground >> 13);
                    flags = this->lifecycle_flags;
                    ai.phase = 1;
                    this->control.gravity_enabled = 1;
                    this->lifecycle_flags = flags & 0xFFEFu;
                }
            }
            else if (step == 1u)
            {
                flags = this->control.flags;
                if (flags & 2u)
                {
                    if (!(this->contact_flags & 2u))
                        return;
                    static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(2);
                    PlayPositionedSound(1, model.pos.words, 0);
                    this->control.gravity_enabled = 0;
                    ai.phase = 2;
                }
                else
                {
                    ground = (uint32)(sint32)(sint16)ai.work[0] << 13;
                    height = (uint32)(sint32)(sint16)(uint32)this->ground_offset << 12;
                    value = this->model.pos.words[1];
                    if ((sint32)ground >= (sint32)(value + height))
                        return;
                    static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(2);
                    PlayPositionedSound(1, model.pos.words, 0);
                    height = (uint32)(sint32)(sint16)(uint32)this->ground_offset << 12;
                    this->control.gravity_enabled = 0;
                    this->model.pos.words[1] = ground - height;
                    ClearVelocity();
                    ai.phase = 2;
                }
            }
            else if (step == 2u && this->playback.complete)
            {
                flags = this->lifecycle_flags;
                SetState(1u);
                this->lifecycle_flags = flags | 16u;
            }
            return;
        case 1:
            if (!step)
            {
                this->control.gravity_enabled = 0;
                ClearVelocity();
                ClearAngularVelocity();
                value = RandomRange(4);
                SetState(value + 2u);
            }
            return;
        case 2:
            if (step)
                return;
            ClearTarget();
            value = player_distance;
            if (value - 1025u < 2047u)
            {
                if (visual_lists.players)
                {
                    SetBehaviorValue(0, 0, 1);
                    hero = visual_lists.players;
                    SetTargetObject(visual_lists.players);
                }
            }
            else
                SetBehaviorValue(0, 0, 0);
            if (player_distance < 2048u)
            {
                if (visual_lists.players)
                {
                    SetBehaviorValue(1, 0, 1);
                    hero = visual_lists.players;
                    SetTargetObject(visual_lists.players);
                }
            }
            else
                SetBehaviorValue(1, 0, 0);
            SetState(1u);
            return;
        case 3:
        case 5:
            if (!step)
            {
                if (state == 3u)
                {
                    height = (uint32)this->control.height_limit;
                    hero = visual_lists.players;
                    value = ChoosePosition(this->destination.words, visual_lists.players->pos.words, 128u, 192u, height, 1024u, 1u, 1u);
                }
                else
                {
                    height = (uint32)this->control.height_limit;
                    value = ChoosePosition(this->destination.words, model.pos.words, 1024u, 2048u, height, 4096u, 1u, 1u);
                }
                if (value)
                {
                    SetDestination(this->destination.words);
                    ai.phase = 1;
                }
                else
                    SetState(1u);
            }
            else if (step == 1u)
            {
                if (!(this->control.flags & 1u))
                    SetState(step);
                else if (state == 3u && visual_lists.players && player_distance < 1024u)
                    SetState(2u);
            }
            return;
        case 4:
            if (!step)
            {
                this->control.script_delay = 32;
                SetState(1u);
            }
            return;
        case 6:
            if (step)
                return;
            flags = this->control.flags;
            hero = visual_lists.players;
            this->control.flags = flags & 0xFFFFFFFE;
            ClearAngularVelocity();
            SetTargetObject(visual_lists.players);
            motion.velocity.words[2] = 0;
            motion.velocity.words[1] = 0;
            value = player_distance;
            motion.velocity.words[0] = 0;
            if (value < 2049u)
            {
                SetBehaviorValue(1, 0, 1);
                SetBehaviorValue(0, 0, 0);
            }
            else
            {
                SetBehaviorValue(0, 0, 1);
                SetBehaviorValue(1, 0, 0);
            }
            return;
        case 10:
            if (!step)
            {
                ai.work[0] = 96;
                ClearVelocity();
                motion.angular_velocity.words[2] = 0;
                motion.angular_velocity.words[1] = 0;
                motion.angular_velocity.words[0] = 0;
                motion.angular_acceleration.words[2] = 0;
                motion.angular_acceleration.words[1] = 0;
                flags = this->control.flags;
                motion.angular_acceleration.words[0] = 0;
                this->control.flags = flags & 0xFFFFFFFE;
                SetBehaviorValue(0, 0, 0);
                SetBehaviorValue(1, 0, 0);
                SetBehaviorValue(2, 0, 0);
                step = ai.phase;
                ai.phase = step + 1u;
            }
            else if (step == 1u)
            {
                if (!RandomRange(4) && !IsGameHeapUnderPressure())
                {
                    scale = (sint16)this->model.scale.words[0] == 2048 ? 370u : 768u;
                    value = RandomRange(1024);
                    angles.words[0] = (uint16)(0u - value);
                    angles.words[1] = (uint16)RandomRange(4096);
                    angles.words[2] = 0;
                    BuildDirectionVector(position.words, scale, angles.words);
                    AddVectors(position.words, model.pos.words);
                    value = RandomRange(60);
                    SpawnFireCycle(position.words, value + 60u, 0u, 2u);
                    sound = RandomRange(2);
                    PlayPositionedSound(sound + 1, model.pos.words, 0);
                }
                value = (uint32)(sint32)(sint16)ai.work[0] - 1u;
                ai.work[0] = value;
                if (!(value << 16))
                {
                    step = ai.phase;
                    ai.phase = step + 1u;
                }
            }
            else if (step == 2u)
            {
                SpawnExplosionRing(model.pos.words, 800, 240, 160, 40, 5, 0, 100);
                child = AllocateEffectStorage(120);
                if (child)
                {
                    acceleration = zero_vector;
                    ImpactRing *ring = new (child) ImpactRing(acceleration.words);
                    ring->SetGeometry(model.pos.words, 0, 0);
                    ring->lifetime = 32;
                    ring->radius_step = 32;
                    ring->width_step = 16;
                }
                position.words[0] = 0;
                position.words[1] = 0xFFFEC000;
                position.words[2] = 0;
                child = AllocateEffectStorage(120);
                if (child)
                {
                    ImpactRing *ring = new (child) ImpactRing(position.words);
                    ring->SetGeometry(model.pos.words, 0, 0);
                    ring->lifetime = 32;
                    ring->radius_step = 32;
                    ring->width_step = 16;
                }
                if (ai.script_data)
                {
                    flags = this->lifecycle_flags;
                    this->collision_radius = 0u;
                    this->lifecycle_flags = flags & 0xFFEFu;
                    ReleaseResources();
                    ActivateLinkedRecords();
                    step = ai.phase;
                    ai.phase = step + 1u;
                    SetModelBank(GetRecordModelBankName());
                    value = ai.script_data;
                    height = GetRecordModelBankSlot();
                    value = ::FindModelResourceIndex(value, height);
                    flags = this->model.flags;
                    this->model.model_idx = value;
                    this->model.animation = 0u;
                    this->model.frame = 0u;
                    this->model.flags = flags & 0xFFF9u;
                }
                else
                    FragmentAndFinish();
            }
            return;
    }
}

uint32 Tank::SetMode(uint32 mode)
{
    uint32 flags, result;
    FUNCTION_MARKER(0x800576B0, "SLUS_003.73");
    mode &= 0xFFFFu;
    this->control.variant = mode;
    if (mode != 0u && mode != 1u && mode != 16u)
        return 16;
    SetModelBank("tank");
    this->ground_offset = 0u;
    this->health = (sint16)(uint16)(mode == 0u ? 320u : (mode == 1u ? 400u : 1000u));
    SetModelBoneTable(this->model.bank, TankAttachments);
    InitializeModelAnimation(TankHierarchy);
    if (mode == 0u)
    {
        flags = this->model.flags;
        this->model.scale.words[0] = 2048;
        this->model.flags = flags | 0x200u;
        this->model.scale.words[1] = 2048;
        this->model.scale.words[2] = 2048;
        ai.parameters[0] = 32;
        result = 64u;
    }
    else
    {
        ai.parameters[0] = 16;
        result = 128u;
    }
    ai.parameters[1] = result;
    ai.parameters[2] = result;
    if (mode == 16u)
    {
        ::TrackObjectHealth(*this, 64u, 32u, 2048u);
        flags = this->control.flags;
        camera_subject.SetObject(&model);
        result = flags | 32u;
        this->control.flags = result;
    }
    return result;
}

uint32 Tank::ExecuteCommand(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value, shift;
    Vector3 position;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80057840, "SLUS_003.73");
    command &= 0xFFFFu;
    switch (command)
    {
        case 0x4206:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2u;
            SetMode(value);
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
        case 0x4407:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2u;
            if (!value)
            {
                ClearTarget();
                return 1;
            }
            if (value & 0x2000u)
            {
                value = ResolveParameter(value);
            }
            SetTargetRecord(value & 0xFFFFu);
            return 1;
        case 0x4220:
        case 0x4222:
            cursor = AlignScriptCursor(script_cursor);
            position.words[0] = cursor.Read32();
            position.words[1] = (cursor + 4u).Read32();
            position.words[2] = (cursor + 8u).Read32();
            shift = 12;
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
            script_cursor = cursor + 2u;
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 Tank::SetDestination(const uint32 *position)
{
    FUNCTION_MARKER(0x80057AE0, "SLUS_003.73");
    const uint32 x = position[0];
    const uint32 y = position[1];
    const uint32 z = position[2];
    destination.words[0] = x;
    destination.words[1] = y;
    destination.words[2] = z;
    control.flags |= 1;
    return control.flags;
}

uint32 Tank::Aim(uint32 enabled)
{
    FUNCTION_MARKER(0x80057B48, "SLUS_003.73");
    uint32 result = 0;
    for (uint32 axis = 0; axis < 2; ++axis)
    {
        Vector3 origin;
        ShortVector3 desired = {0};
        GetModelAttachment(origin.words, *this, axis);
        if (enabled & (1 << axis))
        {
            CalculateDirectionAngles(desired.words, origin.words, aim_pos.words);
            desired.words[1] = (desired.words[1] - model.angles.words[1]) & 4095;
        }
        AngleMotion *joint = joints + axis;
        uint32 scale = ai.parameters[1 + axis];
        result = UpdateAngleAcceleration(joint->angles, joint->velocity, joint->acceleration, desired, scale);
    }
    return result;
}

uint32 Tank::SetTargetRecord(uint32 index)
{
    RecordDataBorrow record_borrow;
    uint32 flags;
    FUNCTION_MARKER(0x80057CCC, "SLUS_003.73");
    ReadRecordPosition(this->aim_pos.words, index & 0xFFFFu);
    flags = this->control.flags | 4u;
    this->control.flags = flags;
    return flags;
}

uint32 Tank::SetTargetObject(ModelInstance *target)
{
    FUNCTION_MARKER(0x80057D38, "SLUS_003.73");
    uint32 result = static_cast<VisualObject *>(target->owner)->IsFinished();
    if (result)
        return result;
    tracked_target.SetObject(target);
    result = control.flags | 16;
    control.flags = result;
    return result;
}

uint32 Tank::ClearTarget()
{
    uint32 flags;
    FUNCTION_MARKER(0x80057D84, "SLUS_003.73");
    flags = this->control.flags & 0xFFFFFFE3;
    this->control.flags = flags;
    return flags;
}

uint32 Tank::FragmentAndFinish()
{
    uint32 trig_angle;
    ObjectBorrow object_borrow;
    ModelInstance *camera;
    uint32 angle;
    uint32 index;
    uint32 random;
    uint32 divisor;
    GameHeapBlock *child;
    uint32 kind;
    uint32 height;
    Vector3 position, velocity;
    FUNCTION_MARKER(0x80057D98, "SLUS_003.73");
    angle = RandomRange(1024);
    for (index = 0; index < 12u; ++index)
    {
        camera = visual_lists.cameras;
        if (camera && !RandomRange(8))
        {
            camera = visual_lists.cameras;
            position.words[0] = camera->pos.words[0];
            position.words[1] = camera->pos.words[1];
            position.words[2] = camera->pos.words[2];
            position.words[1] += 0x80000u;
            divisor = MeasurePositionDistance(model.pos.words, position.words) >> 7;
            velocity.words[0] = TraceDivide(position.words[0] - this->model.pos.words[0], divisor);
            velocity.words[2] = TraceDivide(position.words[2] - this->model.pos.words[2], divisor);
            velocity.words[1] = TraceDivide(position.words[1] - this->model.pos.words[1], divisor) - (divisor << 13);
        }
        else
        {
            random = RandomRange(16);
            trig_angle = angle & 0xFFFu;
            velocity.words[0] = (uint32)rsin(trig_angle) * (random + 24u);
            random = RandomRange(32);
            velocity.words[1] = (0u - 48u - random) << 12;
            random = RandomRange(16);
            velocity.words[2] = (uint32)rcos(trig_angle) * (random + 24u);
        }
        child = AllocateZeroedObject(320);
        if (child)
        {
            random = RandomRange((uint32)this->debris_count);
            kind = (uint32)this->debris_first;
            height = (uint32)(sint32)(sint16)(uint32)this->ground_offset << 12;
            height += this->model.pos.words[1];
            new (child) Debris(model.pos.words, velocity.words, (kind + random) & 65535, height, 5, 36, GetRecordModelBankName());
        }
        angle += 682u;
    }
    return Finish();
}

uint32 Tank::TraceDivide(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1u : 0xFFFFFFFF;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

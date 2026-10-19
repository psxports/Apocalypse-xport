#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "effect_lifecycle.h"
#include "level_session.h"
#include "record_scripts.h"

#include "object_initializers.h"

#include "flamethrower.h"
#include "projectile_line.h"
#include "visual_object.h"
#include "angle_math.h"
#include "object_damage.h"
#include "model_animation.h"
#include "model_attachment.h"
#include "game_diagnostics.h"
#include "player.h"
#include "game_random.h"
#include "audio_session.h"
#include "ring_burst_effect.h"
#include "model_resources.h"
#include "fire_cycle.h"

#include "turret.h"
#include "swat.h"
#include "vector_arithmetic.h"
#include "record_data.h"
#include "game_object_allocation.h"
#include "game_heap.h"

Turret::Turret(const RecordCursor &record_data, uint32 index) : Enemy()
{
    RecordDataBorrow record_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80057FAC, "SLUS_003.73");
    aim_bones[0] = aim_bones[1] = 0;
    aim_pos.words[0] = aim_pos.words[1] = aim_pos.words[2] = 0;
    aim_offset.words[0] = aim_offset.words[1] = aim_offset.words[2] = 0;

    cursor = ReadPosition(record_data);
    cursor = ReadAngles(cursor);
    script_cursor = cursor;
    this->control.script_active = 1;
    this->record_idx = index;
    Init();
    ExecuteScript(script_cursor);
}

uint32 Turret::SetMode(uint32 mode)
{
    uint32 count, level, flags, subtype, result;
    const char *model_name;
    const ModelHierarchyTable *hierarchy;
    const ModelAttachmentTable *attachments;
    FUNCTION_MARKER(0x800580AC, "SLUS_003.73");
    this->control.variant = mode;
    if (mode == 0u)
    {
        flags = this->model.flags;
        this->model.flags = flags | 16u;
        AddToList(visual_lists.solids);
        count = game_progress.level.enemy_total;
        level = level_environment.area_kind;
        game_progress.level.enemy_total = count - 1u;
        if (level == 4u)
        {
            model_name = "turret01";
            attachments = &TurretLevel4Attachments;
            hierarchy = &TurretLevel4Hierarchy;
            subtype = 1;
            result = 2u;
        }
        else
        {
            model_name = "turret";
            attachments = &TurretDefaultAttachments;
            hierarchy = &TurretDefaultHierarchy;
            subtype = 0;
            result = 3u;
        }
    }
    else if (mode == 16u || mode == 32u)
    {
        this->kind = 211;
        AddToList(visual_lists.actors);
        if (mode == 16u)
        {
            level = level_environment.area_kind;
            this->health = (sint16)(uint16)(64u);
        }
        else
        {
            flags = this->control.flags;
            this->health = (sint16)(uint16)(64u);
            level = level_environment.area_kind;
            this->control.flags = flags | 64u;
        }
        if (level == 10u)
        {
            model_name = "turret02";
            attachments = &TurretLevel10Attachments;
            hierarchy = &TurretLevel10Hierarchy;
            subtype = 1;
            result = 2u;
        }
        else
        {
            model_name = "unturret";
            attachments = &TurretRaisedAttachments;
            hierarchy = &TurretRaisedHierarchy;
            subtype = 0;
            result = 3u;
        }
    }
    else
        return (sint32)mode < 17 ? 1u : 32u;
    SetModelBank(model_name);
    SetModelBoneTable(this->model.bank, *attachments);
    InitializeModelAnimation(*hierarchy);
    aim_bones[0] = subtype;
    aim_bones[1] = result;
    return result;
}

uint32 Turret::Init()
{
    uint32 count = game_progress.level.enemy_total;
    FUNCTION_MARKER(0x800582D4, "SLUS_003.73");
    this->kind = 206;
    game_progress.level.enemy_total = count + 1u;
    AllocateBehaviorTable(1, 5);
    this->health = (sint16)(uint16)(100u);
    ai.parameters[0] = 16;
    this->control.gravity_enabled = 0;
    ai.state = 0;
    ai.phase = 0;
    this->control.hierarchy_ready = 0;
    ai.parameters[1] = 32;
    SetBehaviorValue(0, 0, 0);
    SetBehaviorValue(0, 1, 30);
    SetBehaviorValue(0, 2, 160);
    SetBehaviorValue(0, 3, 32);
    SetBehaviorValue(0, 4, 64);
    return 0;
}

uint32 Turret::Destroy(uint32 reason)
{
    uint32 mode;
    FUNCTION_MARKER(0x800583A8, "SLUS_003.73");
    mode = (uint32)this->control.variant;
    RemoveFromList(mode ? visual_lists.actors : visual_lists.solids);
    Enemy::FreeBehaviorTable();
    Turret::ReleaseResources();
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : 0;
}

Turret::~Turret()
{
    if (weapon)
        weapon->Destroy(3);
}

uint32 Turret::ReleaseResources()
{
    FUNCTION_MARKER(0x8005843C, "SLUS_003.73");
    if (weapon)
        weapon->Destroy(3);
    weapon = 0;
    return 0;
}

uint32 Turret::Update()
{
    RecordDataBorrow record_borrow;
    uint32 flags, hits, timer, height, yaw, base_yaw, damage;
    Vector3 position;
    FUNCTION_MARKER(0x8005848C, "SLUS_003.73");
    position = zero_vector;
    motion.acceleration = position;
    flags = this->activation_flags;
    this->control.prev_activation_flags = flags;
    if (flags & 1u)
    {
        hits = (uint32)this->control.activation_count;
        this->activation_flags = flags & 0xFFFEu;
        this->control.activation_count = hits + 1u;
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
        timer = (uint32)this->control.script_delay;
        if (timer)
            this->control.script_delay = timer - 1u;
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
            aim_pos.words[0] = subject->pos.words[0];
            aim_pos.words[1] = subject->pos.words[1];
            aim_pos.words[2] = subject->pos.words[2];
        }
        else
            control.flags &= ~16;
        Aim();
    }
    else if (flags & 8u)
    {
        AddVectors(position.words, model.pos.words, this->aim_offset.words);
        this->aim_pos = position;
        Aim();
    }
    else if (flags & 4u)
        Aim();
    IntegrateMotion();
    AddAngleVector(joint.angles.words, joint.velocity.words);
    NormalizeAngles(joint.angles.words);
    AddAngleVector(joint.velocity.words, joint.acceleration.words);
    DampAngles(joint.velocity.words, motion.angular_drag);
    SnapSmallAngles(joint.velocity.words);
    UpdateWorldTransform();
    BuildModelPose(*this);
    Swat *operator_actor = operator_object.Get();
    if (operator_actor)
    {
        if (operator_actor->lifecycle_flags & 64)
        {
            operator_object = 0;
            control.flags &= 0xFFFFFFDF;
        }
        else
        {
            GetModelAttachment(position.words, *this, 1);
            operator_actor->model.pos.words[0] = position.words[0];
            operator_actor->model.pos.words[1] = position.words[1];
            operator_actor->model.pos.words[2] = position.words[2];
            height = (uint32)(sint32)(sint16)operator_actor->ground_offset;
            operator_actor->model.pos.words[1] -= height << 12;
            yaw = joint.angles.words[1];
            base_yaw = model.angles.words[1];
            operator_actor->model.angles.words[1] = yaw + base_yaw;
        }
    }
    if (!(GetBehaviorValue(0, 0) & 0xFFFFu))
    {
        if (weapon)
            Fire(0u);
        return 0;
    }
    if (!(this->control.flags & 64u))
    {
        timer = GetBehaviorValue(0, 3) & 0xFFFFu;
        if (!RandomRange(timer))
            Fire(1u);
        return 0;
    }
    if (!weapon)
    {
        GameHeapBlock *allocation = AllocateZeroedHeapBlock(72);
        if (allocation)
        {
            weapon = new (allocation) Flamethrower(&visual_lists.players, &visual_lists.actors);
        }
        damage = GetBehaviorValue(0, 1) & 0xFFFF;
        if (!weapon)
        {
            GameDiagnostic("Turret flamethrower allocation failed\n");
            abort();
        }
        weapon->damage = damage;
    }
    Fire(1u);
    // The legacy behavior caller discards the original return carrier
    return 0;
}

uint32 Turret::DetachAndDestroy()
{
    FUNCTION_MARKER(0x80058814, "SLUS_003.73");
    operator_object = 0;
    control.flags &= 0xFFFFFFDF;
    return Damage(10000, zero_vector.components, 0);
}

void Turret::Fire(uint32 fire)
{
    uint32 pitch, yaw, base_yaw;
    Vector3 position;
    GameHeapBlock *child;
    uint32 speed;
    uint32 damage;
    uint32 random;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80058868, "SLUS_003.73");
    yaw = joint.angles.words[1];
    base_yaw = this->model.angles.words[1];
    pitch = joint.angles.words[0];
    angles.words[0] = (uint16)pitch;
    angles.words[1] = (uint16)(yaw + base_yaw);
    angles.words[2] = 0;
    GetModelAttachment(position.words, *this, 2u);
    if (weapon)
    {
        if (fire)
            weapon->Fire(position.words, angles.components);
        else
            weapon->Update(0, 0);
        return;
    }
    if (!fire)
        return;
    child = AllocateEffectStorage(268);
    if (child)
    {
        speed = GetBehaviorValue(0, 2) & 0xFFFFu;
        damage = GetBehaviorValue(0, 1) & 0xFFFFu;
        new (child) LineProjectile(position.words, angles.components, speed, damage, &visual_lists.players, 255, 64, 0, 1);
    }
    random = RandomRange(3);
    SpawnExplosionRing(position.words, 32, 240, 64, 64, random + 5, 1, 32);
    PlayPositionedSound(20, model.pos.words, 0);
}

uint32 Turret::Damage(uint32 damage, const sint32 *, uint32)
{
    uint32 tick, health, remaining, flags;
    ModelInstance *player, *node;
    uint32 random, key, bank, yaw, chained_damage;
    Vector3 position;
    sint32 signed_health;
    FUNCTION_MARKER(0x80058A14, "SLUS_003.73");
    tick = (uint32)this->control.damage_phase;
    this->control.damage_phase = tick + 1;
    signed_health = this->health;
    health = (uint16)this->health;
    remaining = health - damage;
    if (signed_health <= 0)
        return 1;
    this->health = (sint16)(uint16)(remaining);
    if ((sint16)(uint16)remaining > 0)
    {
        SetColor(3, 255, 255, 255);
        return 1;
    }
    if ((uint32)this->control.variant)
    {
        player = visual_lists.players;
        if (player)
            TryPlayPlayerKillQuote(player->pos.words);
    }
    position = this->model.pos;
    position.words[1] += 0x80000;
    random = RandomRange(2);
    PlayPositionedSound(random ? 1 : 2, this->model.pos.words, 0);
    SpawnExplosionRing(this->model.pos.words, 300, 240, 160, 40, 5, 0, 100);
    SpawnFireCycle(position.words, 120, 0, 2);
    flags = this->control.flags;
    node = visual_lists.actors;
    this->control.flags = flags | 32;
    while (node)
    {
        if (static_cast<VisualObject *>(node->owner)->kind == 200 && static_cast<Swat *>(node->owner)->mounted_object.GetObject() == &model)
        {
            if (!static_cast<VisualObject *>(node->owner)->IsFinished())
            {
                chained_damage = (uint32)(sint32) static_cast<VisualObject *>(node->owner)->health << 1;
                static_cast<VisualObject *>(node->owner)->Damage(chained_damage, zero_vector.components, 0);
            }
            break;
        }
        node = node->next;
    }
    if (ai.script_data)
    {
        flags = this->lifecycle_flags;
        this->collision_radius = 0;
        this->control.script_active = 0;
        this->lifecycle_flags = flags & 0xFFEF;
        ReleaseResources();
        ai.state = 2;
        ai.phase = 0;
        SetModelBank(GetRecordModelBankName());
        key = ai.script_data;
        bank = GetRecordModelBankSlot();
        this->model.model_idx = FindModelResourceIndex(key, bank);
        flags = this->model.flags;
        yaw = joint.angles.words[1];
        this->model.flags = flags & 0xFFF9;
        this->model.angles.words[1] = yaw;
    }
    else
    {
        Finish();
    }
    return 1;
}

uint32 Turret::UpdateState()
{
    uint32 state, phase, flags, result;
    ModelInstance *player;
    FUNCTION_MARKER(0x80058C80, "SLUS_003.73");
    state = ai.state;
    if (state == 1u)
    {
        phase = ai.phase;
        if (phase)
            return phase;
        player = visual_lists.players;
        if (player)
        {
            SetTargetObject(visual_lists.players);
            SetBehaviorValue(0, 0, 1);
            return 0;
        }
        ClearTarget();
        SetBehaviorValue(0, 0, 0);
        return 0;
    }
    if (state >= 2u)
        return 0;
    phase = ai.phase;
    if (!phase)
    {
        flags = this->control.flags & 32u;
        if (flags)
            ai.phase = 1;
        return flags;
    }
    if (phase != 1u)
        return phase;
    flags = this->control.flags & 32u;
    if (flags)
    {
        player = visual_lists.players;
        SetTargetObject(visual_lists.players);
        SetBehaviorValue(0, 0, 1);
        return 0;
    }
    ClearTarget();
    SetBehaviorValue(0, 0, 0);
    result = 0;
    ai.phase = 0;
    return result;
}

uint32 Turret::ExecuteCommand(uint32 command)
{
    RecordDataBorrow record_borrow;
    uint32 value;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80058DB8, "SLUS_003.73");
    command &= 0xFFFFu;
    switch (command)
    {
        case 0x4206:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2u;
            SetMode(value);
            return 1;
        case 0x4262:
            cursor = script_cursor;
            value = cursor.Read16();
            script_cursor = cursor + 2u;
            if (!value)
                Fire(1u);
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
        default:
            return Enemy::ExecuteCommand(command);
    }
}

void Turret::Aim()
{
    FUNCTION_MARKER(0x80058F24, "SLUS_003.73");
    Vector3 pos;
    ShortVector3 desired;
    GetModelAttachment(pos.words, *this, 0);
    CalculateDirectionAngles(desired.words, pos.words, aim_pos.words);
    desired.words[1] = (desired.words[1] - model.angles.words[1]) & 4095;
    UpdateAngleAcceleration(joint.angles, joint.velocity, joint.acceleration, desired, ai.parameters[1]);
    GetModelJointAngles(aim_bones[0]).rotation.words[1] = joint.angles.words[1];
    GetModelJointAngles(aim_bones[1]).rotation.words[0] = joint.angles.words[0];
}

uint32 Turret::SetTargetRecord(uint32 index)
{
    RecordDataBorrow record_borrow;
    uint32 flags;
    FUNCTION_MARKER(0x80059010, "SLUS_003.73");
    ReadRecordPosition(this->aim_pos.words, index & 0xFFFFu);
    flags = this->control.flags | 4u;
    this->control.flags = flags;
    return flags;
}

uint32 Turret::SetTargetObject(ModelInstance *target)
{
    FUNCTION_MARKER(0x8005907C, "SLUS_003.73");
    uint32 result = static_cast<VisualObject *>(target->owner)->IsFinished();
    if (result)
        return result;
    tracked_target.SetObject(target);
    result = control.flags | 16;
    control.flags = result;
    return result;
}

uint32 Turret::ClearTarget()
{
    uint32 flags = this->control.flags & 0xFFFFFFE3;
    FUNCTION_MARKER(0x800590C8, "SLUS_003.73");
    this->control.flags = flags;
    return flags;
}

uint32 Turret::AttachOperator(Swat *operator_actor)
{
    FUNCTION_MARKER(0x800587E8, "SLUS_003.73");
    if (operator_object)
        return 0;
    operator_object = operator_actor;
    control.flags |= 32;
    return 1;
}

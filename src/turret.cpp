#include <new>
#include <stdlib.h>

#include "effect_lifecycle.h"
#include "level_session.h"
#include "resource_script.h"
#include "object_initializers.h"
#include "flamethrower.h"
#include "projectile_line.h"
#include "line_projectile.h"
#include "visual_object.h"
#include "angle_math.h"
#include "object_damage.h"
#include "model_animation.h"
#include "model_attachment_data.h"
#include "model_attachment.h"
#include "game_diagnostics.h"
#include "player.h"
#include "global.h"
#include "audio_session.h"
#include "ring_burst_effect.h"
#include "model_bank.h"
#include "fire_cycle.h"
#include "turret.h"
#include "swat.h"
#include "record_data.h"
#include "game_object_allocation.h"
#include "game_heap.h"
#include "enemy.h"
#include "effect_object.h"

Turret::Turret(const RecordCursor &rec_data, uint16 idx) : Enemy()
{
    RecordDataBorrow rec_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80057FAC, "SLUS_003.73");
    aim_bones[0] = aim_bones[1] = 0;
    aim_pos.x = aim_pos.y = aim_pos.z = 0;
    aim_offset.x = aim_offset.y = aim_offset.z = 0;

    cursor = ReadPos(rec_data);
    cursor = ReadAngles(cursor);
    script_cursor = cursor;
    control.script_active = 1;
    rec_idx = idx;
    Init();
    ExecuteScript(script_cursor);
}

uint16 Turret::SetMode(uint16 mode)
{
    uint32 level;
    uint16 subtype, result;
    const char *model_name;
    const ModelHierarchyTable *hierarchy;
    const ModelAttachmentTable *attachments;
    FUNCTION_MARKER(0x800580AC, "SLUS_003.73");
    control.variant = mode;
    if (mode == 0)
    {
        model.flags |= 16;
        AddToList(visual_lists.solids);
        level = level_environment.area_kind;
        --game_progress.level.enemy_total;
        if (level == 4)
        {
            model_name = "turret01";
            attachments = &TurretLevel4Attachments;
            hierarchy = &TurretLevel4Hierarchy;
            subtype = 1;
            result = 2;
        }
        else
        {
            model_name = "turret";
            attachments = &TurretDefaultAttachments;
            hierarchy = &TurretDefaultHierarchy;
            subtype = 0;
            result = 3;
        }
    }
    else if (mode == 16 || mode == 32)
    {
        kind = 211;
        AddToList(visual_lists.actors);
        if (mode == 16)
        {
            level = level_environment.area_kind;
            health = 64;
        }
        else
        {
            health = 64;
            level = level_environment.area_kind;
            control.flags |= 64;
        }
        if (level == 10)
        {
            model_name = "turret02";
            attachments = &TurretLevel10Attachments;
            hierarchy = &TurretLevel10Hierarchy;
            subtype = 1;
            result = 2;
        }
        else
        {
            model_name = "unturret";
            attachments = &TurretRaisedAttachments;
            hierarchy = &TurretRaisedHierarchy;
            subtype = 0;
            result = 3;
        }
    }
    else
        return mode < 17 ? 1 : 32;
    SetModelBank(model_name);
    model.SetAttachmentTable(*attachments);
    InitializeModelAnimation(*hierarchy);
    aim_bones[0] = subtype;
    aim_bones[1] = result;
    return result;
}

uint32 Turret::Init()
{
    FUNCTION_MARKER(0x800582D4, "SLUS_003.73");
    kind = 206;
    ++game_progress.level.enemy_total;
    AllocateBehaviorTable(1, 5);
    health = 100;
    ai.parameters[0] = 16;
    control.gravity_enabled = 0;
    ai.state = 0;
    ai.phase = 0;
    control.hierarchy_ready = 0;
    ai.parameters[1] = 32;
    SetBehaviorVal(0, 0, 0);
    SetBehaviorVal(0, 1, 30);
    SetBehaviorVal(0, 2, 160);
    SetBehaviorVal(0, 3, 32);
    SetBehaviorVal(0, 4, 64);
    return 0;
}

uint32 Turret::Destroy(uint32 reason)
{
    uint16 mode;
    FUNCTION_MARKER(0x800583A8, "SLUS_003.73");
    mode = control.variant;
    RemoveFromList(mode ? visual_lists.actors : visual_lists.solids);
    Enemy::FreeBehaviorTable();
    Turret::ReleaseResources();
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : 0;
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
    RecordDataBorrow rec_borrow;
    uint32 flags;
    uint16 activation, timer, yaw, base_yaw, damage;
    Vector3 pos;
    FUNCTION_MARKER(0x8005848C, "SLUS_003.73");
    motion.acceleration = zero_vector;
    activation = activation_flags;
    control.prev_activation_flags = activation;
    if (activation & 1)
    {
        activation_flags = activation & 0xFFFE;
        ++control.activation_count;
    }
    if (!control.hierarchy_ready)
    {
        control.hierarchy_ready = 1;
        model.flags |= 4;
        SetAnimationRange(0, 0, 0xFFFFFFFF);
    }
    if (control.script_active)
    {
        timer = control.script_delay;
        if (timer)
            control.script_delay = timer - 1;
        else
            ExecuteScript(script_cursor);
    }
    else
        UpdateState();
    if (!(model.flags & 4))
        return 0;
    flags = control.flags;
    if (flags & 16)
    {
        ModelInstance *subject = tracked_target.GetObject();
        if (subject)
        {
            aim_pos = subject->pos;
        }
        else
            control.flags &= ~16;
        Aim();
    }
    else if (flags & 8)
    {
        pos.Add(model.pos, aim_offset);
        aim_pos = pos;
        Aim();
    }
    else if (flags & 4)
        Aim();
    IntegrateMotion();
    joint.angles.AddAngles(joint.velocity);
    joint.angles.NormalizeAngles();
    joint.velocity.AddAngles(joint.acceleration);
    joint.velocity.DampAngles(motion.angular_drag);
    joint.velocity.SnapAngles();
    UpdateWorldTransform();
    BuildModelPose();
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
            GetModelAttachment(pos, *this, 1);
            operator_actor->model.pos = pos;
            operator_actor->model.pos.y = (uint32)operator_actor->model.pos.y - (operator_actor->ground_offset * ONE);
            yaw = (uint16)joint.angles.y;
            base_yaw = (uint16)model.angles.y;
            operator_actor->model.angles.y = yaw + base_yaw;
        }
    }
    if (!GetBehaviorVal(0, 0))
    {
        if (weapon)
            Fire(false);
        return 0;
    }
    if (!(control.flags & 64))
    {
        timer = GetBehaviorVal(0, 3);
        if (!RandomRange(timer))
            Fire(true);
        return 0;
    }
    if (!weapon)
    {
        GameHeapBlock *allocation = AllocateZeroedHeapBlock(72);
        if (allocation)
        {
            weapon = new (allocation) Flamethrower(&visual_lists.players, &visual_lists.actors);
        }
        damage = GetBehaviorVal(0, 1);
        if (!weapon)
        {
            GameDiagnostic(("Turret flamethrower allocation failed\n"));
            abort();
        }
        weapon->damage = damage;
    }
    Fire(true);
    // The legacy behavior caller discards the original return carrier
    return 0;
}

bool Turret::AttachOperator(Swat *operator_actor)
{
    FUNCTION_MARKER(0x800587E8, "SLUS_003.73");
    if (operator_object)
        return false;
    operator_object = operator_actor;
    control.flags |= 32;
    return true;
}

uint32 Turret::DetachAndDestroy()
{
    FUNCTION_MARKER(0x80058814, "SLUS_003.73");
    operator_object = 0;
    control.flags &= 0xFFFFFFDF;
    return Damage(10000, &zero_vector, 0);
}

void Turret::Fire(bool fire)
{
    Vector3 pos;
    GameHeapBlock *child;
    uint16 speed;
    uint16 damage;
    uint8 random;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80058868, "SLUS_003.73");
    angles.x = joint.angles.x;
    angles.y = (uint16)joint.angles.y + (uint16)model.angles.y;
    angles.z = 0;
    GetModelAttachment(pos, *this, 2);
    if (weapon)
    {
        if (fire)
            weapon->Fire(pos, angles);
        else
            weapon->Update(0);
        return;
    }
    if (!fire)
        return;
    child = EffectObject::AllocateStorage(268);
    if (child)
    {
        speed = GetBehaviorVal(0, 2);
        damage = GetBehaviorVal(0, 1);
        new (child) LineProjectile(&pos, &angles, speed, damage, &visual_lists.players, 255, 64, 0, 1);
    }
    random = RandomRange(3);
    RingBurstEffect::SpawnExplosion(&pos, 32, 240, 64, 64, random + 5, 1, 32);
    PlayPositionedSound(20, &model.pos, 0);
}

uint32 Turret::Damage(uint32 damage, const Vector3 *, uint32)
{
    sint16 health;
    ModelInstance *player, *node;
    uint32 key, chained_damage;
    uint8 random, bank;
    Vector3 pos;
    FUNCTION_MARKER(0x80058A14, "SLUS_003.73");
    ++control.damage_phase;
    health = this->health;
    if (health <= 0)
        return 1;
    this->health = health - damage;
    if (this->health > 0)
    {
        SetColor(3, 255, 255, 255);
        return 1;
    }
    if (control.variant)
    {
        player = visual_lists.players;
        if (player)
            static_cast<Player *>(player->owner)->TryPlayKillQuote();
    }
    pos = model.pos;
    pos.y = (uint32)pos.y + (0x80000);
    random = RandomRange(2);
    PlayPositionedSound(random ? 1 : 2, &model.pos, 0);
    RingBurstEffect::SpawnExplosion(&model.pos, 300, 240, 160, 40, 5, 0, 100);
    FireCycle::Spawn(&pos, 120, 0, 2);
    node = visual_lists.actors;
    control.flags |= 32;
    while (node)
    {
        VisualObject *actor = static_cast<VisualObject *>(node->owner);
        if (actor->kind == 200 && static_cast<Swat *>(actor)->mounted_object.GetObject() == &model)
        {
            if (!actor->IsFinished())
            {
                chained_damage = actor->health * 2;
                actor->Damage(chained_damage, &zero_vector, 0);
            }
            break;
        }
        node = node->next;
    }
    if (ai.script_data)
    {
        col_radius = 0;
        control.script_active = 0;
        lifecycle_flags &= 0xFFEF;
        ReleaseResources();
        ai.state = 2;
        ai.phase = 0;
        SetModelBank(GetRecModelBankName());
        key = ai.script_data;
        bank = GetRecModelBankSlot();
        model.model_idx = ModelBank::FindResourceIdx(key, bank);
        model.flags &= 0xFFF9;
        model.angles.y = joint.angles.y;
    }
    else
    {
        Finish();
    }
    return 1;
}

void Turret::UpdateState()
{
    uint16 state, phase;
    uint32 flags;
    ModelInstance *player;
    FUNCTION_MARKER(0x80058C80, "SLUS_003.73");
    state = ai.state;
    if (state == 1)
    {
        phase = ai.phase;
        if (phase)
            return;
        player = visual_lists.players;
        if (player)
        {
            SetTargetObject(visual_lists.players);
            SetBehaviorVal(0, 0, 1);
            return;
        }
        ClearTarget();
        SetBehaviorVal(0, 0, 0);
        return;
    }
    if (state >= 2)
        return;
    phase = ai.phase;
    if (!phase)
    {
        flags = control.flags & 32;
        if (flags)
            ai.phase = 1;
        return;
    }
    if (phase != 1)
        return;
    flags = control.flags & 32;
    if (flags)
    {
        SetTargetObject(visual_lists.players);
        SetBehaviorVal(0, 0, 1);
        return;
    }
    ClearTarget();
    SetBehaviorVal(0, 0, 0);
    ai.phase = 0;
}

uint32 Turret::ExecuteCommand(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 val;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80058DB8, "SLUS_003.73");
    switch (command)
    {
        case 0x4206:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            SetMode(val);
            return 1;
        case 0x4262:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            if (!val)
                Fire(true);
            return 1;
        case 0x4406:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            if (val)
                SetTargetObject(visual_lists.players);
            else
                ClearTarget();
            return 1;
        case 0x4407:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            if (!val)
            {
                ClearTarget();
                return 1;
            }
            if (val & 0x2000)
            {
                val = ResolveParameter(val);
            }
            SetTargetRec(val);
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
    GetModelAttachment(pos, *this, 0);
    desired.SetDir(pos, aim_pos);
    desired.y = ((uint16)desired.y - (uint16)model.angles.y) & 4095;
    UpdateAngularAcceleration(joint.angles, joint.velocity, joint.acceleration, desired, ai.parameters[1]);
    GetModelJointAngles(aim_bones[0]).rotation.y = joint.angles.y;
    GetModelJointAngles(aim_bones[1]).rotation.x = joint.angles.x;
}

uint32 Turret::SetTargetRec(uint16 idx)
{
    RecordDataBorrow rec_borrow;
    uint32 flags;
    FUNCTION_MARKER(0x80059010, "SLUS_003.73");
    ReadRecPos(&aim_pos, idx);
    flags = control.flags | 4;
    control.flags = flags;
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
    uint32 flags = control.flags & 0xFFFFFFE3;
    FUNCTION_MARKER(0x800590C8, "SLUS_003.73");
    control.flags = flags;
    return flags;
}

Turret::~Turret()
{
    if (weapon)
        weapon->Destroy(3);
}

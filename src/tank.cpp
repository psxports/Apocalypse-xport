#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "effect_lifecycle.h"
#include "level_session.h"
#include "game_tick.h"
#include "resource_script.h"
#include "collision.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "global.h"
#include "camera.h"
#include "flamethrower.h"
#include "weaving_projectile.h"
#include "model_bank.h"
#include "visual_object.h"
#include "angle_math.h"
#include "model_animation.h"
#include "model_attachment_data.h"
#include "model_attachment.h"
#include "game_diagnostics.h"
#include "player.h"
#include "game_panel.h"
#include "fire_cycle.h"
#include "tank.h"
#include "projectile_base.h"
#include "record_data.h"
#include "debris.h"
#include "effect_splash.h"
#include "impact_ring.h"
#include "ring_burst_effect.h"
#include "game_object_allocation.h"
#include "game_heap.h"
#include "audio_voice.h"
#include "audio_session.h"
#include "items.h"
#include "enemy.h"
#include "effect_object.h"

Tank::Tank(const RecordCursor &rec_data, uint16 idx) : Enemy(), fire_mask(0), debris_first(0), debris_count(0), move_sound(0)
{
    RecordDataBorrow rec_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80055F14, "SLUS_003.73");
    aim_pos.x = aim_pos.y = aim_pos.z = 0;
    aim_offset.x = aim_offset.y = aim_offset.z = 0;

    cursor = ReadPos(rec_data);
    cursor = ReadAngles(cursor);
    script_cursor = cursor;
    control.script_active = 1;
    rec_idx = idx;
    Init();
    cursor = script_cursor;
    ExecuteScript(cursor);
}

Tank::Tank(const Vector3 *pos_vector, const RecordCursor &script) : Enemy(), fire_mask(0), debris_first(0), debris_count(0), move_sound(0)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x80055F9C, "SLUS_003.73");
    aim_pos.x = aim_pos.y = aim_pos.z = 0;
    aim_offset.x = aim_offset.y = aim_offset.z = 0;
    pos_history.saved = *pos_vector;
    model.pos = pos_history.saved;
    script_cursor = script;
    control.script_active = 1;
    Init();
    ExecuteScript(script_cursor);
}

uint32 Tank::Init()
{
    GameHeapBlock *child;
    uint16 damage;
    uint8 bank;
    FUNCTION_MARKER(0x80056038, "SLUS_003.73");
    ++game_progress.level.enemy_total;
    AddToList(visual_lists.solids);
    kind = 205;
    motion.drag[0] = 3;
    model.flags |= 16;
    motion.drag[1] = 3;
    motion.drag[2] = 3;
    AllocateBehaviorTable(3, 9);
    control.gravity_enabled = 0;
    ai.state = 0;
    ai.phase = 0;
    control.hierarchy_ready = 0;
    SetBehaviorVal(0, 0, 0);
    SetBehaviorVal(0, 1, 30);
    SetBehaviorVal(0, 2, 64);
    SetBehaviorVal(0, 3, 64);
    SetBehaviorVal(0, 4, 64);
    SetBehaviorVal(0, 5, 0);
    SetBehaviorVal(1, 0, 0);
    SetBehaviorVal(1, 1, 1);
    SetBehaviorVal(1, 2, 128);
    SetBehaviorVal(1, 3, 12);
    SetBehaviorVal(1, 4, 64);
    SetBehaviorVal(1, 5, 0);
    SetBehaviorVal(2, 0, 0);
    SetBehaviorVal(2, 1, 20);
    SetBehaviorVal(2, 2, 30);
    SetBehaviorVal(2, 3, 512);
    SetBehaviorVal(2, 4, 8);
    SetBehaviorVal(2, 5, 300);
    SetBehaviorVal(2, 6, 256);
    SetBehaviorVal(2, 7, 128);
    SetBehaviorVal(2, 8, 0);
    child = AllocateZeroedHeapBlock(72);
    Flamethrower *created = 0;
    if (child)
    {
        created = new (child) Flamethrower(&visual_lists.players, 0);
    }
    weapon = created;
    damage = GetBehaviorVal(1, 1);
    bank = GetRecModelBankSlot();
    if (!weapon)
    {
        GameDiagnostic(("Tank flamethrower allocation failed\n"));
        abort();
    }
    weapon->damage = damage;
    debris_first = ModelBank::FindResourceIdx(0xA48DD5EC, bank);
    debris_count = 10;
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
    uint32 sound;
    FUNCTION_MARKER(0x800562C4, "SLUS_003.73");
    RemoveFromList(visual_lists.solids);
    Enemy::FreeBehaviorTable();
    if (control.flags & 32)
    {
        VisualObject::ClearTrackedHealth();
        control.flags &= 0xFFFFFFDF;
    }
    if (weapon)
        weapon->Destroy(3);
    weapon = 0;
    sound = move_sound;
    if (sound)
        StopSoundVoices(sound);
    move_sound = 0;
    if (camera_subject.GetObject() == &model)
        camera_subject.Reset();
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : 0;
}

uint32 Tank::Update()
{
    RecordDataBorrow rec_borrow;
    ShortVector3 motion_target;
    uint32 flags, sound;
    uint16 val, count;
    uint16 activation;
    uint8 mode;
    bool fire;
    Vector3 tmp_pos;
    ShortVector3 desired;
    sint32 wanted_yaw, cur_yaw;
    bool move;
    sint32 yaw_delta;
    FUNCTION_MARKER(0x800563B4, "SLUS_003.73");
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
        count = control.script_delay;
        if (count)
            control.script_delay = count - 1;
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
            VisualObject *visual = static_cast<VisualObject *>(subject->owner);
            aim_pos = visual->kind == 50 ? visual->surface.pos : subject->pos;
        }
        else
            control.flags &= ~16;
        mode = fire_mask;
    }
    else if (flags & 8)
    {
        tmp_pos.Add(model.pos, aim_offset);
        aim_pos = tmp_pos;
        mode = fire_mask;
    }
    else
        mode = (flags & 4) ? fire_mask : 0;
    Aim(mode);
    if (control.flags & 1)
    {
        if ((sint32)model.pos.HorizontalDistance(dst) < 128)
        {
            control.flags &= 0xFFFFFFFE;
        }
        else
        {
            sound = move_sound;
            if (sound)
            {
                if (!(game_clock.tick & 15))
                    UpdatePositionedSoundVoices(sound, &model.pos, 0);
            }
            else
                move_sound = PlayPositionedSound(49, &model.pos, 0);
            desired.SetDir(model.pos, dst);
            desired.x = 0;
            motion_target.x = 0;
            motion_target.y = desired.y;
            motion_target.z = 0;
            UpdateAngularAcceleration(model.angles, motion.angular_velocity, motion.angular_acceleration, motion_target, 16);
            model.angles.x = 0;
            wanted_yaw = desired.y;
            cur_yaw = model.angles.y;
            yaw_delta = abs(wanted_yaw - cur_yaw);
            move = yaw_delta < 64 || yaw_delta >= 4033;
            if (move)
                motion.velocity.SetDir(ai.parameters[0], desired);
            else
            {
                motion.velocity.z = 0;
                motion.velocity.y = 0;
                motion.velocity.x = 0;
            }
        }
    }
    else
    {
        sound = move_sound;
        if (sound)
        {
            StopSoundVoices(sound);
            move_sound = 0;
        }
    }
    if (control.gravity_enabled)
        motion.acceleration.Add(world_gravity.vector);
    IntegrateMotion();
    joints[0].angles.AddAngles(joints[0].velocity);
    joints[0].angles.NormalizeAngles();
    joints[0].velocity.AddAngles(joints[0].acceleration);
    joints[0].velocity.DampAngles(motion.angular_drag);
    joints[0].velocity.SnapAngles();
    GetModelJointAngles(1).rotation.y = joints[0].angles.y;
    GetModelJointAngles(2).rotation.x = joints[0].angles.x;
    val = (uint16)GetModelJointAngles(2).rotation.x;
    if (val >= 129 && val < 2048)
        GetModelJointAngles(2).rotation.x = 128;
    joints[1].angles.AddAngles(joints[1].velocity);
    joints[1].angles.NormalizeAngles();
    joints[1].velocity.AddAngles(joints[1].acceleration);
    joints[1].velocity.DampAngles(motion.angular_drag);
    joints[1].velocity.SnapAngles();
    GetModelJointAngles(3).rotation.y = joints[1].angles.y;
    UpdateWorldTransform();
    BuildModelPose();
    if (GetBehaviorVal(0, 0))
    {
        fire_mask |= 1;
        count = GetBehaviorVal(0, 5);
        if (count)
            SetBehaviorVal(0, 5, count - 1);
        else
        {
            FireProjectile();
            val = GetBehaviorVal(0, 3);
            SetBehaviorVal(0, 5, val);
        }
    }
    else
        fire_mask &= 0xFE;
    fire = GetBehaviorVal(1, 0) != 0;
    if (fire)
        fire_mask |= 2;
    else
        fire_mask &= 0xFD;
    FireBeam(fire);
    val = GetBehaviorVal(2, 0);
    if (!val)
        return val;
    count = GetBehaviorVal(2, 8);
    if (count)
        val = count - 1;
    else
    {
        SpawnItem();
        val = GetBehaviorVal(2, 2);
    }
    SetBehaviorVal(2, 8, val);
    return 0;
}

void Tank::FireProjectile()
{
    Vector3 pos;
    GameHeapBlock *child;
    uint32 val;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80056980, "SLUS_003.73");
    angles.x = joints[0].angles.x;
    angles.y = ((uint16)model.angles.y + (uint16)GetModelJointAngles(1).rotation.y) & 0xFFF;
    angles.z = 0;
    GetModelAttachment(pos, *this, 2);
    WeavingProjectile *projectile = 0;
    child = AllocateZeroedObject(440);
    if (child)
    {
        projectile = new (child) WeavingProjectile(pos, angles, 0, 4, 128, &visual_lists.players, zero_vector);
    }
    val = GetBehaviorVal(0, 1);
    val = ScaleDamageForDifficulty(val);
    if (!projectile)
    {
        GameDiagnostic(("Tank projectile allocation failed\n"));
        abort();
    }
    projectile->SetDamage(val);
    projectile->SetBlastRadius(512);
}

void Tank::FireBeam(bool fire)
{
    Vector3 pos;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80056A6C, "SLUS_003.73");
    if (!weapon)
        return;
    angles.x = joints[1].angles.x;
    angles.z = 0;
    angles.y = ((uint16)model.angles.y + (uint16)GetModelJointAngles(3).rotation.y) & 0xFFF;
    GetModelAttachment(pos, *this, 3);
    if (fire)
        weapon->Fire(pos, angles);
    else
        weapon->Update(0);
}

void Tank::SpawnItem()
{
    ObjectBorrow object_borrow;
    GameHeapBlock *child;
    uint16 damage;
    uint16 arm_radius;
    uint16 blast_radius;
    uint16 fuse_ticks;
    uint16 life_ticks;
    uint16 hover_height;
    FUNCTION_MARKER(0x80056B48, "SLUS_003.73");
    Items *item = 0;
    child = AllocateZeroedObject(328);
    if (child)
    {
        damage = GetBehaviorVal(2, 1);
        arm_radius = GetBehaviorVal(2, 3);
        blast_radius = GetBehaviorVal(2, 6);
        fuse_ticks = GetBehaviorVal(2, 4);
        life_ticks = GetBehaviorVal(2, 5);
        hover_height = GetBehaviorVal(2, 7);
        item = new (child) Items(&model.pos, damage, arm_radius, blast_radius, fuse_ticks, life_ticks, hover_height);
    }
    if (!item)
    {
        GameDiagnostic(("Tank debris allocation failed\n"));
        abort();
    }
    item->motion.velocity.y = -524288;
}

uint32 Tank::Damage(uint32 damage, const Vector3 *, uint32)
{
    uint32 state;
    ModelInstance *hero;
    sint16 health;
    FUNCTION_MARKER(0x80056C50, "SLUS_003.73");
    ++control.damage_phase;
    health = this->health;
    if (health <= 0)
        return 1;
    this->health = health - damage;
    if (this->health > 0)
        SetColor(3, 255, 255, 255);
    else
    {
        hero = visual_lists.players;
        if (hero)
            static_cast<Player *>(hero->owner)->TryPlayKillQuote();
        state = control.flags;
        lifecycle_flags &= 0xFFEF;
        if (state & 32)
        {
            VisualObject::ClearTrackedHealth();
            control.flags &= 0xFFFFFFDF;
        }
        control.script_active = 0;
        ai.state = 10;
        ai.phase = 0;
    }
    return 1;
}

void Tank::ClearVelocity()
{
    motion.velocity.z = 0;
    motion.velocity.y = 0;
    motion.velocity.x = 0;
}

void Tank::ClearAngularVelocity()
{
    motion.angular_velocity.z = 0;
    motion.angular_velocity.y = 0;
    motion.angular_velocity.x = 0;
    motion.angular_acceleration.z = 0;
    motion.angular_acceleration.y = 0;
    motion.angular_acceleration.x = 0;
}

void Tank::SetState(uint16 state)
{
    ai.state = state;
    ai.phase = 0;
}

void Tank::UpdateState()
{
    uint16 state, step, random;
    sint16 offset;
    uint32 flags, val, ground, height, sound;
    ModelInstance *hero;
    GameHeapBlock *child;
    Vector3 pos, velocity;
    uint16 scale;
    ShortVector3 angles;
    FUNCTION_MARKER(0x80056D1C, "SLUS_003.73");
    // Update discards the original register-carried return
    state = ai.state;
    if (state > 10 || (state >= 7 && state <= 9))
        return;
    step = ai.phase;
    switch (state)
    {
        case 0:
            if (step == 0)
            {
                hero = visual_lists.players;
                if (hero)
                {
                    model.angles.y = 3072 - ratan2((sint32)((uint32)hero->pos.z - model.pos.z), (sint32)((uint32)hero->pos.x - model.pos.x));
                }
                ground = CollisionRay::FindGroundHeight(model.pos, 0, 4096);
                if (ground == 0xFFFFFFFF)
                {
                    Finish();
                    return;
                }
                val = (uint32)model.pos.y;
                offset = ground_offset;
                if (((sint32)(ground - val) >> 12) < offset * 2)
                {
                    model.pos.y = ground - offset * ONE;
                    ai.state = 1;
                    control.gravity_enabled = 0;
                    ClearVelocity();
                }
                else
                {
                    ai.work[0] = (sint32)ground >> 13;
                    flags = lifecycle_flags;
                    ai.phase = 1;
                    control.gravity_enabled = 1;
                    lifecycle_flags = flags & 0xFFEF;
                }
            }
            else if (step == 1)
            {
                flags = control.flags;
                if (flags & 2)
                {
                    if (!(contact_flags & 2))
                        return;
                    static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(2);
                    PlayPositionedSound(1, &model.pos, 0);
                    control.gravity_enabled = 0;
                    ai.phase = 2;
                }
                else
                {
                    ground = (sint16)ai.work[0] * (1 << 13);
                    height = ground_offset * ONE;
                    val = (uint32)model.pos.y;
                    if ((sint32)ground >= (sint32)(val + height))
                        return;
                    static_cast<Camera *>(visual_lists.cameras->owner)->ApplyExplosionShake(2);
                    PlayPositionedSound(1, &model.pos, 0);
                    height = ground_offset * ONE;
                    control.gravity_enabled = 0;
                    model.pos.y = ground - height;
                    ClearVelocity();
                    ai.phase = 2;
                }
            }
            else if (step == 2 && playback.complete)
            {
                flags = lifecycle_flags;
                SetState(1);
                lifecycle_flags = flags | 16;
            }
            return;
        case 1:
            if (!step)
            {
                control.gravity_enabled = 0;
                ClearVelocity();
                ClearAngularVelocity();
                random = RandomRange(4);
                SetState(random + 2);
            }
            return;
        case 2:
            if (step)
                return;
            ClearTarget();
            val = player_distance;
            if (val - 1025 < 2047)
            {
                if (visual_lists.players)
                {
                    SetBehaviorVal(0, 0, 1);
                    hero = visual_lists.players;
                    SetTargetObject(visual_lists.players);
                }
            }
            else
                SetBehaviorVal(0, 0, 0);
            if (player_distance < 2048)
            {
                if (visual_lists.players)
                {
                    SetBehaviorVal(1, 0, 1);
                    hero = visual_lists.players;
                    SetTargetObject(visual_lists.players);
                }
            }
            else
                SetBehaviorVal(1, 0, 0);
            SetState(1);
            return;
        case 3:
        case 5:
            if (!step)
            {
                if (state == 3)
                {
                    height = control.height_limit;
                    hero = visual_lists.players;
                    val = ChoosePos(&dst, &visual_lists.players->pos, 128, 192, height, 1024, 1, 1);
                }
                else
                {
                    height = control.height_limit;
                    val = ChoosePos(&dst, &model.pos, 1024, 2048, height, 4096, 1, 1);
                }
                if (val)
                {
                    SetDst(&dst);
                    ai.phase = 1;
                }
                else
                    SetState(1);
            }
            else if (step == 1)
            {
                if (!(control.flags & 1))
                    SetState(step);
                else if (state == 3 && visual_lists.players && player_distance < 1024)
                    SetState(2);
            }
            return;
        case 4:
            if (!step)
            {
                control.script_delay = 32;
                SetState(1);
            }
            return;
        case 6:
            if (step)
                return;
            flags = control.flags;
            hero = visual_lists.players;
            control.flags = flags & 0xFFFFFFFE;
            ClearAngularVelocity();
            SetTargetObject(visual_lists.players);
            motion.velocity.z = 0;
            motion.velocity.y = 0;
            val = player_distance;
            motion.velocity.x = 0;
            if (val < 2049)
            {
                SetBehaviorVal(1, 0, 1);
                SetBehaviorVal(0, 0, 0);
            }
            else
            {
                SetBehaviorVal(0, 0, 1);
                SetBehaviorVal(1, 0, 0);
            }
            return;
        case 10:
            if (!step)
            {
                ai.work[0] = 96;
                ClearVelocity();
                motion.angular_velocity.z = 0;
                motion.angular_velocity.y = 0;
                motion.angular_velocity.x = 0;
                motion.angular_acceleration.z = 0;
                motion.angular_acceleration.y = 0;
                flags = control.flags;
                motion.angular_acceleration.x = 0;
                control.flags = flags & 0xFFFFFFFE;
                SetBehaviorVal(0, 0, 0);
                SetBehaviorVal(1, 0, 0);
                SetBehaviorVal(2, 0, 0);
                ++ai.phase;
            }
            else if (step == 1)
            {
                if (!RandomRange(4) && !IsGameHeapUnderPressure())
                {
                    scale = model.scale.x == 2048 ? 370 : 768;
                    random = RandomRange(1024);
                    angles.x = -random;
                    angles.y = RandomRange(4096);
                    angles.z = 0;
                    pos.SetDir(scale, angles);
                    pos.Add(model.pos);
                    val = RandomRange(60);
                    FireCycle::Spawn(&pos, val + 60, 0, 2);
                    sound = RandomRange(2);
                    PlayPositionedSound(sound + 1, &model.pos, 0);
                }
                --ai.work[0];
                if (!ai.work[0])
                    ++ai.phase;
            }
            else if (step == 2)
            {
                RingBurstEffect::SpawnExplosion(&model.pos, 800, 240, 160, 40, 5, 0, 100);
                child = EffectObject::AllocateStorage(120);
                if (child)
                {
                    velocity = zero_vector;
                    ImpactRing *ring = new (child) ImpactRing(velocity);
                    ring->SetGeometry(&model.pos, 0, 0);
                    ring->lifetime = 32;
                    ring->radius_step = 32;
                    ring->width_step = 16;
                }
                pos.x = 0;
                pos.y = 0xFFFEC000;
                pos.z = 0;
                child = EffectObject::AllocateStorage(120);
                if (child)
                {
                    ImpactRing *ring = new (child) ImpactRing(pos);
                    ring->SetGeometry(&model.pos, 0, 0);
                    ring->lifetime = 32;
                    ring->radius_step = 32;
                    ring->width_step = 16;
                }
                if (ai.script_data)
                {
                    flags = lifecycle_flags;
                    col_radius = 0;
                    lifecycle_flags = flags & 0xFFEF;
                    ReleaseResources();
                    ActivateLinkedRecs();
                    ++ai.phase;
                    SetModelBank(GetRecModelBankName());
                    val = ai.script_data;
                    height = GetRecModelBankSlot();
                    val = ModelBank::FindResourceIdx(val, height);
                    flags = model.flags;
                    model.model_idx = val;
                    model.animation = 0;
                    model.frame = 0;
                    model.flags = flags & 0xFFF9;
                }
                else
                    FragmentAndFinish();
            }
            return;
    }
}

uint32 Tank::SetMode(uint16 mode)
{
    uint32 flags, result;
    FUNCTION_MARKER(0x800576B0, "SLUS_003.73");
    control.variant = mode;
    if (mode != 0 && mode != 1 && mode != 16)
        return 16;
    SetModelBank("tank");
    ground_offset = 0;
    health = mode == 0 ? 320 : (mode == 1 ? 400 : 1000);
    this->model.SetAttachmentTable(TankAttachments);
    InitializeModelAnimation(TankHierarchy);
    if (mode == 0)
    {
        flags = this->model.flags;
        this->model.scale.x = 2048;
        this->model.flags = flags | 0x200;
        this->model.scale.y = 2048;
        this->model.scale.z = 2048;
        ai.parameters[0] = 32;
        result = 64;
    }
    else
    {
        ai.parameters[0] = 16;
        result = 128;
    }
    ai.parameters[1] = result;
    ai.parameters[2] = result;
    if (mode == 16)
    {
        TrackHealth(64, 32, 2048);
        flags = control.flags;
        camera_subject.SetObject(&model);
        result = flags | 32;
        control.flags = result;
    }
    return result;
}

uint32 Tank::ExecuteCommand(uint16 command)
{
    RecordDataBorrow rec_borrow;
    uint16 val;
    Vector3 pos;
    RecordCursor cursor;
    FUNCTION_MARKER(0x80057840, "SLUS_003.73");
    switch (command)
    {
        case 0x4206:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            SetMode(val);
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
        case 0x4220:
        case 0x4222:
            cursor = script_cursor.Aligned(4);
            pos.x = cursor.Read32();
            pos.y = (cursor + 4).Read32();
            pos.z = (cursor + 8).Read32();
            pos.ShiftLeft(12);
            if (command == 0x4222)
                pos.Add(model.pos);
            script_cursor = cursor + 12;
            SetDst(&pos);
            return 1;
        case 0x4221:
            cursor = script_cursor;
            val = cursor.Read16();
            script_cursor = cursor + 2;
            if (val & 0x2000)
            {
                val = ResolveParameter(val);
            }
            ReadRecPos(&pos, val);
            SetDst(&pos);
            return 1;
        case 0x4262:
            cursor = script_cursor;
            script_cursor = cursor + 2;
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 Tank::SetDst(const Vector3 *pos)
{
    FUNCTION_MARKER(0x80057AE0, "SLUS_003.73");
    dst = *pos;
    control.flags |= 1;
    return control.flags;
}

void Tank::Aim(uint8 enabled)
{
    FUNCTION_MARKER(0x80057B48, "SLUS_003.73");
    for (uint32 axis = 0; axis < 2; ++axis)
    {
        Vector3 origin;
        ShortVector3 desired = {0};
        GetModelAttachment(origin, *this, axis);
        if (enabled & (1 << axis))
        {
            desired.SetDir(origin, aim_pos);
            desired.y = ((uint16)desired.y - (uint16)model.angles.y) & 4095;
        }
        AngleMotion *joint = joints + axis;
        uint16 scale = ai.parameters[1 + axis];
        UpdateAngularAcceleration(joint->angles, joint->velocity, joint->acceleration, desired, scale);
    }
}

uint32 Tank::SetTargetRec(uint16 idx)
{
    RecordDataBorrow rec_borrow;
    uint32 flags;
    FUNCTION_MARKER(0x80057CCC, "SLUS_003.73");
    ReadRecPos(&aim_pos, idx);
    flags = control.flags | 4;
    control.flags = flags;
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
    flags = control.flags & 0xFFFFFFE3;
    control.flags = flags;
    return flags;
}

uint32 Tank::FragmentAndFinish()
{
    uint16 trig_angle;
    ObjectBorrow object_borrow;
    ModelInstance *camera;
    uint16 angle;
    uint32 idx;
    uint16 random;
    uint32 divisor;
    GameHeapBlock *child;
    uint8 model_idx;
    uint32 height;
    Vector3 pos, velocity;
    FUNCTION_MARKER(0x80057D98, "SLUS_003.73");
    angle = RandomRange(1024);
    for (idx = 0; idx < 12; ++idx)
    {
        camera = visual_lists.cameras;
        if (camera && !RandomRange(8))
        {
            camera = visual_lists.cameras;
            pos = camera->pos;
            pos.y = (uint32)pos.y + (0x80000);
            divisor = model.pos.Distance(pos) >> 7;
            velocity.x = DivideSignedWord((uint32)pos.x - model.pos.x, divisor);
            velocity.z = DivideSignedWord((uint32)pos.z - model.pos.z, divisor);
            velocity.y = DivideSignedWord((uint32)pos.y - model.pos.y, divisor) - (divisor << 13);
        }
        else
        {
            random = RandomRange(16);
            trig_angle = angle & 0xFFF;
            velocity.x = rsin(trig_angle) * (random + 24);
            random = RandomRange(32);
            velocity.y = -(48 + random) * ONE;
            random = RandomRange(16);
            velocity.z = rcos(trig_angle) * (random + 24);
        }
        child = AllocateZeroedObject(320);
        if (child)
        {
            random = RandomRange(debris_count);
            model_idx = debris_first;
            height = ground_offset * ONE;
            height += (uint32)model.pos.y;
            new (child) Debris(&model.pos, &velocity, model_idx + random, height, 5, 36, GetRecModelBankName());
        }
        angle += 682;
    }
    return Finish();
}

#include "visual_object.h"
#include "angle_math.h"
#include "object_damage.h"
#include "model_animation.h"
#include "game_sdk_bindings.h"
#include "player.h"
#include "game_random.h"
#include "audio_session.h"
#include "ring_burst_effect.h"
#include "model_resources.h"
#include "fire_cycle.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "turret.h"
#include "object_parameters.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Turret::Turret(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor;
    FUNCTION_MARKER(0x80057FAC, "SLUS_003.73");
    if (!data)
    {
        // TODO Support native record data in the original parser adapter
        GameDiagnostic("Turret record data is outside guest memory\n");
        abort();
    }
    w_u32(object + 68, 0x800A2960);
    cursor = sub_80062B0C(object, data);
    cursor = sub_80062B50(object, cursor);
    w_u32(object + 400, cursor);
    w_u8(object + 380, 1);
    this->record_idx = index;
    Init();
    sub_8004BE90(object, r_u32(object + 400));
}

uint32 Turret::SetMode(uint32 mode)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, level, flags, model, subtype, result;
    const ModelHierarchyTable *hierarchy;
    const ModelAttachmentTable *attachments;
    FUNCTION_MARKER(0x800580ACu, "SLUS_003.73");
    w_u16(object + 390u, mode);
    if (mode == 0u)
    {
        flags = this->model.flags;
        this->model.flags = flags | 16u;
        AddToList(visual_lists.solids);
        count = r_u32(0x800FF308u);
        level = r_u32(0x800FF378u);
        w_u32(0x800FF308u, count - 1u);
        if (level == 4u)
        {
            model = 0x800A2760u;
            attachments = &TurretLevel4Attachments;
            hierarchy = &TurretLevel4Hierarchy;
            subtype = 1u;
            result = 2u;
        }
        else
        {
            model = 0x800FF56Cu;
            attachments = &TurretDefaultAttachments;
            hierarchy = &TurretDefaultHierarchy;
            subtype = 0u;
            result = 3u;
        }
    }
    else if (mode == 16u || mode == 32u)
    {
        this->kind = 211u;
        AddToList(visual_lists.actors);
        if (mode == 16u)
        {
            level = r_u32(0x800FF378u);
            w_u16(object + 218u, 64u);
        }
        else
        {
            flags = r_u32(object + 396u);
            w_u16(object + 218u, 64u);
            level = r_u32(0x800FF378u);
            w_u32(object + 396u, flags | 64u);
        }
        if (level == 10u)
        {
            model = 0x800A276Cu;
            attachments = &TurretLevel10Attachments;
            hierarchy = &TurretLevel10Hierarchy;
            subtype = 1u;
            result = 2u;
        }
        else
        {
            model = 0x800A2778u;
            attachments = &TurretRaisedAttachments;
            hierarchy = &TurretRaisedHierarchy;
            subtype = 0u;
            result = 3u;
        }
    }
    else
        return (sint32)mode < 17 ? 1u : 32u;
    sub_800626C8(object, model);
    SetModelBoneTable(this->model.bank, *attachments);
    InitializeModelAnimation(*hierarchy);
    w_u16(object + 496u, subtype);
    w_u16(object + 498u, result);
    return result;
}

uint32 Turret::Init()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count = r_u32(0x800FF308u);
    FUNCTION_MARKER(0x800582D4u, "SLUS_003.73");
    this->kind = 206u;
    w_u32(0x800FF308u, count + 1u);
    sub_8004BD04(object, 1u, 5u);
    w_u16(object + 218u, 100u);
    w_u16(object + 436u, 16u);
    w_u8(object + 382u, 0u);
    w_u16(object + 472u, 0u);
    w_u16(object + 474u, 0u);
    w_u8(object + 381u, 0u);
    w_u16(object + 438u, 32u);
    sub_8004BDCC(object, 0u, 0u, 0u);
    sub_8004BDCC(object, 0u, 1u, 30u);
    sub_8004BDCC(object, 0u, 2u, 160u);
    sub_8004BDCC(object, 0u, 3u, 32u);
    return sub_8004BDCC(object, 0u, 4u, 64u);
}

uint32 Turret::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 mode;
    FUNCTION_MARKER(0x800583A8, "SLUS_003.73");
    mode = r_u16(object + 390);
    w_u32(object + 68, 0x800A2960);
    RemoveFromList(mode ? visual_lists.actors : visual_lists.solids);
    Enemy::FreeBehaviorTable();
    Turret::ReleaseResources();
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 Turret::ReleaseResources()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, receiver, target;
    FUNCTION_MARKER(0x8005843C, "SLUS_003.73");
    child = r_u32(object + 552);
    if (!child)
        return 0;
    table = r_u32(child);
    receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
    target = r_u32(table + 12);
    (void)apocalypse_object_cleanup(target, receiver, 3);
    w_u32(object + 552, 0);
    return 0;
}

uint32 Turret::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 position[3], flags, hits, timer, linked, y, height, yaw, base_yaw, weapon, damage;
    FUNCTION_MARKER(0x8005848Cu, "SLUS_003.73");
    position[0] = r_u32(0x800A71CCu);
    position[1] = r_u32(0x800A71D0u);
    position[2] = r_u32(0x800A71D4u);
    motion.acceleration[0] = position[0];
    motion.acceleration[1] = position[1];
    motion.acceleration[2] = position[2];
    flags = this->activation_flags;
    w_u16(object + 388u, flags);
    if (flags & 1u)
    {
        hits = r_u8(object + 384u);
        this->activation_flags = flags & 0xFFFEu;
        w_u8(object + 384u, hits + 1u);
    }
    if (!r_u8(object + 381u))
    {
        flags = this->model.flags;
        w_u8(object + 381u, 1u);
        this->model.flags = flags | 4u;
        SetAnimationRange(0u, 0u, 0xFFFFFFFFu);
    }
    if (r_u8(object + 380u))
    {
        timer = r_u16(object + 476u);
        if (timer)
            w_u16(object + 476u, timer - 1u);
        else
            sub_8004BE90(object, r_u32(object + 400u));
    }
    else
        UpdateState();
    if (!(this->model.flags & 4u))
        return 0u;
    flags = r_u32(object + 396u);
    if (flags & 16u)
    {
        linked = r_u32(object + 548u);
        position[0] = r_u32(linked + 4u);
        position[1] = r_u32(linked + 8u);
        position[2] = r_u32(linked + 12u);
        w_u32(object + 500u, position[0]);
        w_u32(object + 504u, position[1]);
        w_u32(object + 508u, position[2]);
        Aim();
    }
    else if (flags & 8u)
    {
        xport_draft_host_sub_8006C34C_p1(position, object + 4u, object + 512u);
        w_u32(object + 500u, position[0]);
        w_u32(object + 504u, position[1]);
        w_u32(object + 508u, position[2]);
        Aim();
    }
    else if (flags & 4u)
        Aim();
    sub_8004B9A4(object);
    sub_8006C730(object + 524u, object + 530u);
    sub_8006C624(object + 524u);
    sub_8006C730(object + 530u, object + 536u);
    DampAngles((uint16 *)psx_addr(object + 530u, 6), motion.angular_drag);
    sub_8006C64C(object + 530u);
    sub_8007FDF0(object);
    BuildModelPose(*this);
    linked = r_u32(object + 544u);
    if (linked)
    {
        if (r_u16(linked + 78u) & 64u)
        {
            flags = r_u32(object + 396u);
            w_u32(object + 544u, 0u);
            w_u32(object + 396u, flags & 0xFFFFFFDFu);
        }
        else
        {
            xport_draft_host_sub_8007CC10_p1(position, object, 1u);
            linked = r_u32(object + 544u);
            w_u32(linked + 4u, position[0]);
            w_u32(linked + 8u, position[1]);
            w_u32(linked + 12u, position[2]);
            linked = r_u32(object + 544u);
            height = (uint32)(sint32)(sint16)r_u16(linked + 456u);
            y = r_u32(linked + 8u);
            w_u32(linked + 8u, y - (height << 12u));
            yaw = r_u16(object + 526u);
            base_yaw = this->model.angles[1];
            linked = r_u32(object + 544u);
            w_u16(linked + 18u, yaw + base_yaw);
        }
    }
    if (!(sub_8004BDFC(object, 0u, 0u) & 0xFFFFu))
    {
        if (r_u32(object + 552u))
            Fire(0u);
        return 0u;
    }
    if (!(r_u32(object + 396u) & 64u))
    {
        timer = sub_8004BDFC(object, 0u, 3u) & 0xFFFFu;
        if (!RandomRange(timer))
            Fire(1u);
        return 0u;
    }
    weapon = r_u32(object + 552u);
    if (!weapon)
    {
        weapon = sub_8002FED8(72u);
        if (weapon)
            weapon = sub_80024428(weapon, 0x800FF5A0u, 0x800FF4E8u);
        w_u32(object + 552u, weapon);
        damage = sub_8004BDFC(object, 0u, 1u) & 0xFFFFu;
        weapon = r_u32(object + 552u);
        /* The original writes through address 0x28 even when allocation returned0 */
        w_u32(weapon + 40u, damage);
        weapon = r_u32(object + 552u);
        if (!weapon)
            return 0u;
    }
    Fire(1u);
    // The legacy behavior caller discards the original return carrier
    return 0u;
}

uint32 Turret::DetachAndDestroy()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, receiver;
    FUNCTION_MARKER(0x80058814, "SLUS_003.73");
    flags = r_u32(object + 396);
    w_u32(object + 544, 0);
    w_u32(object + 396, flags & 0xFFFFFFDF);
    receiver = object;
    return ApplyObjectDamage(receiver, 10000, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
}

void Turret::Fire(uint32 fire)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 position[3], pitch, yaw, base_yaw, weapon, table, receiver, target;
    uint32 child, speed, damage, random;
    uint16 angles[3];
    FUNCTION_MARKER(0x80058868u, "SLUS_003.73");
    yaw = r_u16(object + 526u);
    base_yaw = this->model.angles[1];
    pitch = r_u16(object + 524u);
    angles[0] = (uint16)pitch;
    angles[1] = (uint16)(yaw + base_yaw);
    angles[2] = 0u;
    xport_draft_host_sub_8007CC10_p1(position, object, 2u);
    weapon = r_u32(object + 552u);
    if (weapon)
    {
        table = r_u32(weapon);
        receiver = weapon + (uint32)(sint32)(sint16)r_u16(table + (fire ? 16u : 24u));
        target = r_u32(table + (fire ? 20u : 28u));
        if (fire && target == 0x80024598u)
            (void)apocalypse_beam_24598_native(receiver, position, angles);
        else if (!fire && target == 0x80024FDCu)
            sub_80024FDC(receiver);
        else
        {
            /* TODO Audit additional class206 weapon callbacks before routing them */
            GameDiagnostic("Unsupported class206 weapon callback %08X\n", target);
            abort();
        }
        return;
    }
    if (!fire)
        return;
    child = sub_80032DC0(268u);
    if (child)
    {
        speed = sub_8004BDFC(object, 0u, 2u) & 0xFFFFu;
        damage = sub_8004BDFC(object, 0u, 1u) & 0xFFFFu;
        apocalypse_line_projectile_native(child, position, angles, speed, damage,
            0x800FF5A0u, 255u, 64u, 0u, 1u);
    }
    random = RandomRange(3u);
    xport_draft_host_sub_8001D320_p1(position, 32u, 240u, 64u, 64u, random + 5u, 1u, 32u);
    sub_80069EF4(20u, object + 4u, 0u);
}

uint32 Turret::Damage(uint32 damage, const sint32 *, uint32)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 tick, health, remaining, player, flags, node, table, receiver, target;
    uint32 position[3], random, name, key, bank, yaw, chained_damage;
    sint32 signed_health;
    FUNCTION_MARKER(0x80058A14, "SLUS_003.73");
    tick = r_u8(object + 383);
    w_u8(object + 383, tick + 1);
    signed_health = (sint16)r_u16(object + 218);
    health = r_u16(object + 218);
    remaining = health - damage;
    if (signed_health <= 0)
        return 1;
    w_u16(object + 218, remaining);
    if ((sint16)(uint16)remaining > 0)
    {
        SetColor(3, 255, 255, 255);
        return 1;
    }
    if (r_u16(object + 390))
    {
        player = visual_lists.players;
        if (player)
            TryPlayPlayerKillQuote((const uint32 *)psx_addr(player + 4, 12));
    }
    position[0] = this->model.pos[0];
    position[1] = this->model.pos[1];
    position[2] = this->model.pos[2];
    position[1] += 0x80000;
    random = RandomRange(2);
    PlayPositionedSound(random ? 1 : 2, (const uint32 *)this->model.pos, 0);
    SpawnExplosionRing((const uint32 *)this->model.pos, 300, 240, 160, 40, 5, 0, 100);
    SpawnFireCycle(position, 120, 0, 2);
    flags = r_u32(object + 396);
    node = visual_lists.actors;
    w_u32(object + 396, flags | 32);
    while (node)
    {
        if (r_u16(node + 58) == 200 && r_u32(node + 616) == object)
        {
            if (!sub_80062F48(node))
            {
                chained_damage = (uint32)(sint32)(sint16)r_u16(node + 218) << 1;
                receiver = node;
                ApplyObjectDamage(receiver, chained_damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
            }
            break;
        }
        node = r_u32(node + 28);
    }
    if (r_u32(object + 480))
    {
        flags = this->lifecycle_flags;
        w_u16(object + 212, 0);
        w_u8(object + 380, 0);
        this->lifecycle_flags = flags & 0xFFEF;
        ReleaseResources();
        name = r_u32(0x800FF62C);
        w_u16(object + 472, 2);
        w_u16(object + 474, 0);
        SetModelBank((const char *)psx_addr(name, 1));
        key = r_u32(object + 480);
        bank = r_u8(0x800FF644);
        this->model.model_idx = FindModelResourceIndex(key, bank);
        flags = this->model.flags;
        yaw = r_u16(object + 526);
        this->model.flags = flags & 0xFFF9;
        this->model.angles[1] = yaw;
    }
    else
    {
        table = r_u32(object + 68);
        receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
        target = r_u32(table + 20);
        apocalypse_object_virtual20(target, receiver);
    }
    return 1;
}

uint32 Turret::UpdateState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 state, phase, flags, player, result;
    FUNCTION_MARKER(0x80058C80u, "SLUS_003.73");
    state = r_u16(object + 472u);
    if (state == 1u)
    {
        phase = r_u16(object + 474u);
        if (phase)
            return phase;
        player = visual_lists.players;
        if (player)
        {
            SetTargetObject(player);
            return sub_8004BDCC(object, 0u, 0u, 1u);
        }
        ClearTarget();
        return sub_8004BDCC(object, 0u, 0u, 0u);
    }
    if (state >= 2u)
        return 0u;
    phase = r_u16(object + 474u);
    if (!phase)
    {
        flags = r_u32(object + 396u) & 32u;
        if (flags)
            w_u16(object + 474u, 1u);
        return flags;
    }
    if (phase != 1u)
        return phase;
    flags = r_u32(object + 396u) & 32u;
    if (flags)
    {
        player = visual_lists.players;
        SetTargetObject(player);
        return sub_8004BDCC(object, 0u, 0u, 1u);
    }
    ClearTarget();
    result = sub_8004BDCC(object, 0u, 0u, 0u);
    w_u16(object + 474u, 0u);
    return result;
}

uint32 Turret::ExecuteCommand(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value, table, receiver, target;
    FUNCTION_MARKER(0x80058DB8u, "SLUS_003.73");
    command &= 0xFFFFu;
    switch (command)
    {
        case 0x4206u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            SetMode(value);
            return 1u;
        case 0x4262u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            if (!value)
                Fire(1u);
            return 1u;
        case 0x4406u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            if (value)
                SetTargetObject(visual_lists.players);
            else
                ClearTarget();
            return 1u;
        case 0x4407u:
            cursor = r_u32(object + 400u);
            value = r_u16(cursor);
            w_u32(object + 400u, cursor + 2u);
            if (!value)
            {
                ClearTarget();
                return 1u;
            }
            if (value & 0x2000u)
            {
                table = r_u32(object + 68u);
                receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80u);
                target = r_u32(table + 84u);
                value = DispatchObjectParameterGetter(target, receiver, value);
            }
            SetTargetRecord(value & 0xFFFFu);
            return 1u;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

void Turret::Aim()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 position[3], current_xy, current_z, desired_xy, scale, joint, value;
    uint16 angles[3];
    FUNCTION_MARKER(0x80058F24u, "SLUS_003.73");
    xport_draft_host_sub_8007CC10_p1(position, object, 0u);
    xport_draft_host_sub_80066B8C_p12(angles, position, object + 500u);
    angles[1] = (uint16)((angles[1] - this->model.angles[1]) & 4095u);
    current_xy = r_u32(object + 524u);
    current_z = r_u16(object + 528u);
    desired_xy = (uint32)angles[0] | ((uint32)angles[1] << 16u);
    scale = r_u16(object + 438u);
    sub_80066CF0(current_xy, current_z, object + 530u, object + 536u,
        desired_xy, angles[2], scale);
    joint = r_u16(object + 496u);
    value = r_u16(object + 526u);
    GetModelJointAngles(joint).rotation[1] = (uint16)value;
    joint = r_u16(object + 498u);
    value = r_u16(object + 524u);
    GetModelJointAngles(joint).rotation[0] = (uint16)value;
}

uint32 Turret::SetTargetRecord(uint32 index)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags;
    FUNCTION_MARKER(0x80059010u, "SLUS_003.73");
    sub_8006613C(object + 500u, index & 0xFFFFu);
    flags = r_u32(object + 396u) | 4u;
    w_u32(object + 396u, flags);
    return flags;
}

uint32 Turret::SetTargetObject(uint32 target)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result = sub_80062F48(target);
    FUNCTION_MARKER(0x8005907Cu, "SLUS_003.73");
    if (!result)
    {
        result = r_u32(object + 396u);
        w_u32(object + 548u, target);
        result |= 16u;
        w_u32(object + 396u, result);
    }
    return result;
}

uint32 Turret::ClearTarget()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags = r_u32(object + 396u) & 0xFFFFFFE3u;
    FUNCTION_MARKER(0x800590C8u, "SLUS_003.73");
    w_u32(object + 396u, flags);
    return flags;
}

Turret &Turret::Attach(uint32 object)
{
    return *static_cast<Turret *>(FindNativeObjectStorage(object));
}

// Compatibility entry points retain the guest call contract
uint32 sub_80057FAC(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x80057FAC, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Turret));
    new (native_instance_storage) Turret((const uint8 *)psx_addr(data, 1), index);
    return object;
}

uint32 sub_800583A8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800583A8, "SLUS_003.73");
    return Turret::Attach(object).Turret::Destroy(reason);
}



void sub_8005848C(uint32 object)
{
    FUNCTION_MARKER(0x8005848C, "SLUS_003.73");
    Turret::Attach(object).Turret::Update();
}

uint32 sub_80058814(uint32 object)
{
    FUNCTION_MARKER(0x80058814, "SLUS_003.73");
    return Turret::Attach(object).DetachAndDestroy();
}

uint32 sub_80058A14(uint32 object, uint32 damage)
{
    FUNCTION_MARKER(0x80058A14, "SLUS_003.73");
    return Turret::Attach(object).Turret::Damage(damage, 0, 0);
}

uint32 sub_80058DB8(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x80058DB8, "SLUS_003.73");
    return Turret::Attach(object).Turret::ExecuteCommand(command);
}

uint32 Turret::AttachOperator(uint32 operator_object)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800587E8, "SLUS_003.73");
    if (r_u32(object + 544))
        return 0;
    uint32 flags = r_u32(object + 396);
    w_u32(object + 544, operator_object);
    w_u32(object + 396, flags | 32);
    return 1;
}



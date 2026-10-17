#include "visual_object.h"
#include "record_factory.h"
#include "vector_arithmetic.h"
#include "angle_math.h"
#include "object_damage.h"
#include "object_collision_events.h"
#include "model_animation.h"
#include "game_sdk_bindings.h"
#include "player.h"
#include "audio_session.h"
#include "game_random.h"
#include "vector_distance.h"
#include "game_panel.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "croc.h"
#include "object_parameters.h"
#include "record_data.h"
#include "panel_particle.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>


Croc::Croc(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor;
    FUNCTION_MARKER(0x80071CEC, "SLUS_003.73");
    if (!data)
    {
        // TODO Support native record data in the original parser adapter
        GameDiagnostic("Croc record data is outside guest memory\n");
        abort();
    }
    w_u32(object + 68, 0x800A3B64);
    this->kind = 212;
    sub_800626C8(object, 0x800FF87C);
    cursor = sub_80062B0C(object, data);
    cursor = sub_80062B50(object, cursor);
    AddToList(visual_lists.actors);
    w_u32(object + 400, cursor);
    this->record_idx = index;
    Initialize();
    cursor = r_u32(object + 400);
    if (cursor)
    {
        w_u8(object + 380, 1);
        sub_8004BE90(object, cursor);
    }
    if (r_u32(0x800FF378) == 1)
        TrackObjectHealth(object, 24, 42, 4096);
}

uint32 Croc::Initialize()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, flags, height;
    FUNCTION_MARKER(0x80071DD4, "SLUS_003.73");
    w_u16(object + 456, 64);
    w_u16(object + 458, 48);
    count = r_u32(0x800FF308);
    flags = this->model.flags;
    w_u16(object + 472, 0);
    w_u32(0x800FF308, count + 1);
    this->model.flags = flags | 8;
    StartAnimation(0, 1);
    w_u16(object + 218, 120);
    w_u16(object + 436, 60);
    w_u16(object + 438, 40);
    sub_8004BD04(object, 1, 2);
    sub_8004BDCC(object, 0, 1, 10);
    height = r_u32(0x800FF37C);
    this->model.pos[1] = height;
    return height;
}

uint32 Croc::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80071E80, "SLUS_003.73");
    w_u32(object + 68, 0x800A3B64);
    RemoveFromList(visual_lists.actors);
    ClearTrackedObjectHealth();
    Enemy::FreeBehaviorTable();
    result = Enemy::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(object);
    return result;
}

uint32 Croc::Update()
{
    SVECTOR motion_cur, motion_target;
    uint32 motion_cur_word, motion_cur_tail, motion_target_word, motion_target_tail;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, delay, distance, speed, left, right, angle, packed, tail;
    uint32 flags, count, particle, player, receiver, position[3];
    FUNCTION_MARKER(0x80071EF0u, "SLUS_003.73");
    if (!r_u8(object + 381u))
    {
        SetModelBoneTable(this->model.bank, CrocAttachments);
        w_u8(object + 381u, 1u);
    }
    x = r_u32(0x800A71CCu);
    y = r_u32(0x800A71D0u);
    z = r_u32(0x800A71D4u);
    motion.acceleration[0] = x;
    motion.acceleration[1] = y;
    motion.acceleration[2] = z;
    sub_8007FDF0(object);
    if (r_u8(object + 380u))
    {
        delay = r_u16(object + 476u);
        if (delay)
            w_u16(object + 476u, delay - 1u);
        else
            sub_8004BE90(object, r_u32(object + 400u));
    }
    else
        UpdateState();
    if (r_u8(object + 382u))
        AddVectors(motion.acceleration, (const uint32 *)psx_addr(0x800A6844u, 12));
    sub_8004B9A4(object);
    if (r_u32(object + 396u) & 1u)
    {
        distance = sub_8006696C(object + 4u, object + 484u);
        speed = r_u16(object + 436u);
        if ((sint32)(speed * 2u) < (sint32)distance)
        {
            left = r_u32(object + 492u);
            right = this->model.pos[2];
            z = left - right;
            left = r_u32(object + 484u);
            right = this->model.pos[0];
            angle = (3072u - (uint32)ratan2((sint32)z, (sint32)(left - right))) & 4095u;
            packed = apocalypse_angle_pair_word(object + 16u);
            tail = this->model.angles[2];
            speed = r_u16(object + 438u);
            motion_cur_word = packed;
            motion_cur_tail = tail;
            motion_target_word = angle << 16u;
            motion_target_tail = 0u;
            motion_cur.vx = (sint16)motion_cur_word;
            motion_cur.vy = (sint16)(motion_cur_word >> 16);
            motion_cur.vz = (sint16)motion_cur_tail;
            motion_cur.pad = (sint16)(motion_cur_tail >> 16);
            motion_target.vx = (sint16)motion_target_word;
            motion_target.vy = (sint16)(motion_target_word >> 16);
            motion_target.vz = (sint16)motion_target_tail;
            motion_target.pad = (sint16)(motion_target_tail >> 16);
            UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, speed);
            BuildDirectionVector(motion.velocity, r_u16(object + 436u), (const uint16 *)this->model.angles);
        }
        else
        {
            flags = r_u32(object + 396u);
            w_u32(object + 396u, flags & 0xFFFFFFFEu);
        }
    }
    if (this->model.flags & 1u)
    {
        w_u32(0x800FF3ACu, 0u);
        count = RandomRange(3u) + 3u;
        while (count)
        {
            position[0] = this->model.pos[0];
            position[1] = this->model.pos[1];
            position[2] = this->model.pos[2];
            position[0] += (RandomRange(256u) - 128u) << 12u;
            position[1] += (RandomRange(256u) - 128u) << 12u;
            position[2] += (RandomRange(256u) - 128u) << 12u;
            particle = sub_80032DC0(116u);
            if (particle)
            {
                void *dram_addr = AllocateNativeObjectStorage(particle, sizeof(PanelParticle));
                new (dram_addr) PanelParticle(position);
            }
            w_u16(particle + 64u, 64u);
            --count;
        }
        w_u32(0x800FF3ACu, 1u);
    }
    if (this->model.animation != 1u)
        return 1u;
    xport_draft_host_sub_8007CC10_p1(position, object, 0u);
    player = visual_lists.players;
    distance = MeasureEuclideanDistance((const uint32 *)psx_addr(player + 4, 12), position);
    if ((sint32)distance >= 256)
        return 0u;
    player = visual_lists.players;
    receiver = player;
    return ApplyObjectDamage(receiver, 4u, (const sint32 *)psx_addr(0x800A71CCu, 12u), 0u);
}

uint32 Croc::Damage(uint32 damage, const sint32 *, uint32)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 hits, health_word, player, flags, mode;
    sint32 health;
    FUNCTION_MARKER(0x800736AC, "SLUS_003.73");
    hits = r_u8(object + 383);
    w_u8(object + 383, hits + 1);
    health = (sint16)r_u16(object + 218);
    health_word = r_u16(object + 218);
    if (health <= 0)
        return 1;
    w_u16(object + 218, health_word - damage);
    if (!RandomRange(16))
    {
        mode = r_u32(0x800FF378);
        if (mode == 1)
            PlaySoundEffect(65, 0x3000, 0);
    }
    health = (sint16)r_u16(object + 218);
    if (health > 0)
        SetColor(3, 255, 255, 255);
    else
    {
        ClearTrackedObjectHealth();
        player = visual_lists.players;
        if (player)
        {
            // The complete kill-counter callee ignores its incoming A1
            (void)this->kind;
            TryPlayPlayerKillQuote((const uint32 *)psx_addr(player + 4, 12));
        }
        flags = this->lifecycle_flags;
        mode = r_u32(0x800FF378);
        this->lifecycle_flags = flags & 0xFFEF;
        if (mode == 5)
        {
            w_u16(object + 472, 8);
            w_u16(object + 474, 0);
        }
    }
    return 1;
}

void Croc::SetParameters(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 base, cursor, value, i;
    FUNCTION_MARKER(0x80072204, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x2130)
    {
        Enemy::ReadParameters(command);
        return;
    }
    base = r_u32(object + 400) + 2;
    w_u32(object + 400, base);
    for (i = 0; i < 7; ++i)
    {
        value = r_u16(base + i * 2);
        w_u32(object + 400, base + (i + 1) * 2);
        w_u16(object + 496 + i * 2, value);
    }
    for (i = 0; i < 4; ++i)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        cursor += 2;
        w_u16(object + 510 + i * 2, value);
        w_u32(object + 400, cursor);
    }
    // Retain the last cursor rather than reloading it after the field store
    value = r_u16(cursor);
    w_u32(object + 400, cursor + 2);
    w_u16(object + 518, value);
    for (i = 0; i < 13; ++i)
    {
        cursor = r_u32(object + 400);
        value = r_u16(cursor);
        w_u16(object + 520 + i * 2, value);
        w_u32(object + 400, cursor + 2);
    }
}

uint32 Croc::ExecuteCommand(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value, table, receiver, target, ground, height;
    uint32 position[4];
    FUNCTION_MARKER(0x80072320, "SLUS_003.73");
    command &= 0xFFFF;
    if (command != 0x4221)
        return Enemy::ExecuteCommand(command);
    cursor = r_u32(object + 400);
    value = r_u16(cursor);
    w_u32(object + 400, cursor + 2);
    if (value & 0x2000)
    {
        table = r_u32(object + 68);
        receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80);
        target = r_u32(table + 84);
        value = DispatchObjectParameterGetter(target, receiver, value);
    }
    ReadRecordPosition(position, value & 0xFFFF);
    ground = apocalypse_ground_native(position, 0, 2048);
    if (ground != (uint32)-1)
    {
        height = (uint32)(sint32)(sint16)r_u16(object + 456);
        position[1] = ground - (height << 12);
    }
    Croc::SetDestination(position);
    return 1;
}

void Croc::ClearAngularVelocity()
{
    const uint32 object = NativeObjectGuestAddress(this);
    motion.angular_velocity[2] = 0u;
    motion.angular_velocity[1] = 0u;
    motion.angular_velocity[0] = 0u;
    motion.angular_acceleration[2] = 0u;
    motion.angular_acceleration[1] = 0u;
}

void Croc::UpdateState()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 state, phase, flags, value, old_state, old_phase, player, count;
    uint32 node, table, receiver, target, damage, index, steps, maximum;
    uint32 direction[3], point[3], scaled[3], scale, floor, height;
    CollisionRay collision;
    sint16 angles[3];
    FUNCTION_MARKER(0x800723E4u, "SLUS_003.73");
    if (!visual_lists.players)
        w_u16(object + 472u, 666u);
    state = r_u16(object + 472u);
    if (state < 4u)
    {
        if (state < 3u)
        {
            if ((sint32)(sint16)r_u16(object + 218u) < (sint32)r_u16(object + 504u + state * 2u))
            {
                if (!state)
                {
                    w_u16(object + 472u, 1u);
                    w_u16(object + 474u, 0u);
                    for (index = 0u; index < 4u; ++index)
                        ::SpawnRecord(r_u16(object + 510u + index * 2u));
                }
                else
                {
                    w_u16(object + 472u, state + 1u);
                    w_u16(object + 474u, 0u);
                }
                return;
            }
        }
        else if ((sint16)r_u16(object + 218u) <= 0)
        {
            node = visual_lists.actors;
            while (node)
            {
                if (r_u16(node + 58u) == 200u && r_u16(node + 390u) == 8u)
                {
                    damage = (uint32)(sint32)(sint16)r_u16(node + 218u);
                    receiver = node;
                    ApplyObjectDamage(receiver, damage, (const sint32 *)psx_addr(0x800A71CCu, 12), 0u);
                }
                node = r_u32(node + 28u);
            }
            w_u16(object + 472u, 8u);
            w_u16(object + 474u, 0u);
            return;
        }
        if (state >= 2u && !(r_u32(0x800FF2F0u) & 7u))
        {
            count = 0u;
            node = visual_lists.actors;
            while (node)
            {
                if (r_u16(node + 58u) == 200u && r_u16(node + 390u) == 8u)
                    ++count;
                node = r_u32(node + 28u);
            }
            while ((sint32)count < (sint32)r_u16(object + 518u))
            {
                index = RandomRange(13u);
                value = r_u16(object + 520u + index * 2u);
                ++count;
                ::SpawnRecord(value);
            }
        }
    }
    if (state <= 4u)
    {
        phase = r_u16(object + 474u);
        switch (phase)
        {
            case 0u:
                if (state < 4u)
                {
                    value = r_u16(object + 496u + state * 2u);
                    flags = this->model.flags;
                    w_u16(object + 460u, 0u);
                    w_u16(object + 436u, value);
                }
                else
                {
                    flags = this->model.flags;
                    w_u16(object + 460u, 0u);
                    flags |= 0x200u;
                    this->model.flags = flags;
                    this->model.scale[2] = 2048u;
                    this->model.scale[1] = 2048u;
                    this->model.scale[0] = 2048u;
                }
                if (flags & 1u)
                    break;
                old_state = r_u16(object + 472u);
                old_phase = r_u16(object + 474u);
                w_u16(object + 472u, 16u);
                w_u16(object + 474u, 0u);
                w_u16(object + 440u, old_state);
                w_u16(object + 442u, old_phase + 1u);
                return;
            case 1u:
                steps = 0u;
                if (state == 4u)
                    value = RandomRange(4u);
                else
                {
                    steps = r_u16(object + 460u);
                    value = state < 2u ? steps >= 2u : steps != 0u;
                }
                if (value && r_u32(object + 220u) >= 129u)
                {
                    player = visual_lists.players;
                    xport_draft_host_sub_80066B8C_p1(angles, object + 4u, player + 4u);
                    this->model.angles[1] = (uint16)angles[1];
                    ClearAngularVelocity();
                    old_state = r_u16(object + 472u);
                    w_u16(object + 442u, 3u);
                    motion.angular_acceleration[0] = 0u;
                    w_u16(object + 472u, 17u);
                    w_u16(object + 474u, 0u);
                    w_u16(object + 440u, old_state);
                    return;
                }
                maximum = state == 2u || state == 3u ? 2048u : 1600u;
                if (!sub_8004BA64(object, object + 484u, object + 4u, 1024u, maximum, 128u, 4096u, 1u, 1u))
                {
                    motion.velocity[2] = 0u;
                    motion.velocity[1] = 0u;
                    motion.velocity[0] = 0u;
                    return;
                }
                if (!state)
                {
                    if (r_u32(0x800FF378u) == 1u && !RandomRange(3u))
                        sub_80069DF0(64u, 0x3000u, 0u);
                }
                else if (!RandomRange(3u) && r_u32(0x800FF378u) == 1u)
                    sub_80069DF0(64u, 0x3000u, 0u);
                if (state < 4u)
                    w_u16(object + 460u, steps + 1u);
                SetDestination((const uint32 *)psx_addr(object + 484, 12));
                break;
            case 2u:
                if (!(r_u32(object + 396u) & 1u))
                    w_u16(object + 474u, r_u16(object + 474u) - 1u);
                return;
            case 3u:
                value = this->model.angles[1];
                angles[0] = 0;
                angles[2] = 0;
                angles[1] = (sint16)value;
                xport_draft_host_sub_800667CC_p13(direction, 1u, angles);
                scale = 1600u;
                xport_draft_host_sub_8006C40C_p123(scaled, direction, &scale);
                point[0] = scaled[0];
                point[1] = scaled[1];
                point[2] = scaled[2];
                xport_draft_host_sub_8006C0B8_p1(point, object + 4u);
                collision.pos[0][0] = this->model.pos[0];
                collision.pos[0][1] = this->model.pos[1];
                collision.pos[0][2] = this->model.pos[2];
                collision.pos[1][0] = point[0];
                collision.pos[1][1] = point[1];
                collision.pos[1][2] = point[2];
                InitializeCollisionRay(collision);
                w_u32(0x800FF970u, 1u);
                QueryCollisionRay(collision, 1u);
                value = collision.hit.GetObject();
                if (state < 2u)
                    w_u32(0x800FF970u, 0u);
                if (value)
                {
                    if ((sint32)collision.distance < 129)
                    {
                        w_u16(object + 474u, 0u);
                        return;
                    }
                    point[0] = collision.hit_pos[0];
                    point[1] = collision.hit_pos[1];
                    point[2] = collision.hit_pos[2];
                }
                scale = 128u;
                xport_draft_host_sub_8006C40C_p123(scaled, direction, &scale);
                point[0] -= scaled[0];
                point[1] -= scaled[1];
                point[2] -= scaled[2];
                ClearAngularVelocity();
                motion.angular_acceleration[0] = 0u;
                StartAnimation(1u, 1u);
                value = r_u16(object + 436u);
                w_u16(object + 436u, value * 2u);
                SetDestination(point);
                if (r_u32(0x800FF378u) == 1u)
                    sub_80069DF0(66u, 0x3000u, 0u);
                break;
            case 4u:
                if (!(r_u32(object + 396u) & 1u))
                {
                    StartAnimation(0u, 1u);
                    value = r_u16(object + 436u);
                    w_u16(object + 474u, 0u);
                    w_u16(object + 436u, value >> 1u);
                }
                return;
            default:
                return;
        }
        w_u16(object + 474u, r_u16(object + 474u) + 1u);
        return;
    }
    switch (state)
    {
        case 8u:
            if (!RandomRange(4u) && r_u32(0x800FF378u) == 1u)
                sub_80069DF0(65u, 0x3000u, 0u);
            floor = r_u32(0x800FF37Cu);
            height = this->model.pos[1];
            if ((sint32)(floor + 0x8000u) < (sint32)height)
                this->model.pos[1] = height - 0x8000u;
            else if ((sint32)height < (sint32)(floor - 0x8000u))
                this->model.pos[1] = height + 0x8000u;
            phase = r_u16(object + 474u);
            if (!phase)
            {
                flags = this->lifecycle_flags;
                value = this->model.flags;
                this->lifecycle_flags = flags & 0xFFEFu;
                this->model.flags = value & 0xFFFEu;
                SetAnimationRange(2u, 0u, 0xFFFFFFFFu);
                w_u16(object + 474u, r_u16(object + 474u) + 1u);
            }
            else if (phase == 1u && r_u8(object + 303u))
            {
                table = r_u32(object + 68u);
                receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16u);
                target = r_u32(table + 20u);
                apocalypse_object_virtual20(target, receiver);
            }
            return;
        case 16u:
            floor = r_u32(0x800FF37Cu);
            height = this->model.pos[1];
            if ((sint32)height < (sint32)(floor + 0x40000u))
                this->model.pos[1] = height + 0x8000u;
            else
            {
                flags = this->lifecycle_flags;
                old_state = r_u16(object + 440u);
                old_phase = r_u16(object + 442u);
                this->lifecycle_flags = flags & 0xFFEFu;
                value = this->model.flags;
                w_u16(object + 472u, old_state);
                w_u16(object + 474u, old_phase);
                this->model.flags = value | 1u;
            }
            return;
        case 17u:
            height = this->model.pos[1];
            floor = r_u32(0x800FF37Cu);
            if ((sint32)floor < (sint32)height)
            {
                flags = this->model.flags;
                value = this->lifecycle_flags;
                this->model.flags = flags & 0xFFFEu;
                height = this->model.pos[1];
                this->lifecycle_flags = value | 0x10u;
                this->model.pos[1] = height - 0x8000u;
            }
            else
            {
                old_state = r_u16(object + 440u);
                old_phase = r_u16(object + 442u);
                this->model.pos[1] = floor;
                w_u16(object + 472u, old_state);
                w_u16(object + 474u, old_phase);
            }
            return;
        case 666u:
            flags = r_u32(object + 396u);
            w_u16(object + 472u, 0u);
            w_u32(object + 396u, flags & 0xFFFFFFFEu);
            return;
        default:
            return;
    }
}

uint32 Croc::SetDestination(const uint32 *position)
{
    const uint32 guest_position = GuestMemoryAddress(position);
    const uint32 *native_position = guest_position ? 0 : position;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, flags;
    FUNCTION_MARKER(0x80073680, "SLUS_003.73");
    if (native_position)
    {
        x = native_position[0];
        y = native_position[1];
        z = native_position[2];
    }
    else
    {
        x = r_u32(guest_position);
        y = r_u32(guest_position + 4);
        z = r_u32(guest_position + 8);
    }
    w_u32(object + 484, x);
    w_u32(object + 488, y);
    w_u32(object + 492, z);
    flags = r_u32(object + 396) | 1;
    w_u32(object + 396, flags);
    return flags;
}

Croc &Croc::Attach(uint32 object)
{
    return *static_cast<Croc *>(FindNativeObjectStorage(object));
}

// Compatibility entry points retain the guest call contract
uint32 sub_80071CEC(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x80071CEC, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Croc));
    new (native_instance_storage) Croc((const uint8 *)psx_addr(data, 1), index);
    return object;
}

uint32 sub_80071E80(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80071E80, "SLUS_003.73");
    return Croc::Attach(object).Croc::Destroy(reason);
}

uint32 sub_80071EF0(uint32 object)
{
    FUNCTION_MARKER(0x80071EF0, "SLUS_003.73");
    return Croc::Attach(object).Croc::Update();
}

uint32 sub_80072320(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x80072320, "SLUS_003.73");
    return Croc::Attach(object).Croc::ExecuteCommand(command);
}

uint32 sub_80073680(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80073680, "SLUS_003.73");
    return Croc::Attach(object).SetDestination((const uint32 *)psx_addr(position, 12));
}

void sub_80072204(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x80072204, "SLUS_003.73");
    Croc::Attach(object).Croc::SetParameters(command);
}

uint32 sub_800736AC(uint32 object, uint32 damage)
{
    FUNCTION_MARKER(0x800736AC, "SLUS_003.73");
    return Croc::Attach(object).Croc::Damage(damage, 0, 0);
}

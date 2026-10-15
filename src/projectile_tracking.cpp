#include "audio_voice.h"
#include "game_object_allocation.h"
#include "sprite_trail.h"
#include "vector_arithmetic.h"
#include "object_collision_events.h"
#include "model_resources.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "ring_effect.h"
#include "audio_session.h"
#include "fire_cycle.h"
#include "vector_distance.h"
#include "explosion_effects.h"
#include "game_random.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <string.h>
#include "projectile_tracking.h"
#include "game_heap.h"
#include "native_object_storage.h"
#include <new>




uint32 TrackingProjectile::Configure(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, uint32 list)
{
    uint32 guest_position = GuestMemoryAddress(position), guest_angles = GuestMemoryAddress(angles);
    const uint32 *native_position = guest_position ? 0 : position;
    const sint16 *native_angles = guest_angles ? 0 : angles;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 value, x, y, z, packed_angles, pitch, numerator, denominator, quotient, child;
    sint16 roll;
    FUNCTION_MARKER(0x8002CC78, "SLUS_003.73");
    value = r_u8(0x800FF645);
    value = FindModelResourceIndex(0xD40B155E, value);
    w_u16(object + 22, value);
    AddToList((uint32 *)psx_addr(0x800FF220, 4));
    w_u32(object + 312, 2);
    w_u16(object + 148, 1);
    w_u8(object + 393, 120);
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
    w_u32(object + 4, x);
    w_u32(object + 8, y);
    w_u32(object + 12, z);
    if (native_angles)
    {
        memcpy(&packed_angles, native_angles, 4);
        memcpy(&roll, native_angles + 2, 2);
    }
    else
    {
        packed_angles = ReadAnglesWord(guest_angles);
        roll = (sint16)r_u16(guest_angles + 4);
    }
    w_u32(object + 16, packed_angles);
    w_u16(object + 20, roll);
    pitch = r_u16(object + 16);
    w_u32(object + 416, range);
    w_u16(object + 308, damage);
    w_u32(object + 304, list);
    w_u16(object + 16, pitch - 10);
    w_u16(object + 58, list == 0x800FF5A0 ? 61 : 62);
    w_u32(object + 408, 60);
    BuildDirectionVector((uint32 *)psx_addr(object + 104, 12), 120, (const uint16 *)psx_addr(object + 16, 4));
    denominator = r_u8(object + 393);
    numerator = r_u32(object + 416);
    if (!denominator)
        quotient = (sint32)numerator < 0 ? 1 : (uint32)-1;
    else
        quotient = (uint32)((sint32)numerator / (sint32)denominator);
    w_u8(object + 129, 3);
    w_u32(object + 380, quotient);
    w_u8(object + 130, 3);
    w_u8(object + 131, 3);
    w_u8(object + 144, 1);
    w_u8(object + 145, 1);
    w_u8(object + 146, 1);
    w_u16(object + 136, 400);
    if (RandomRange(2))
    {
        value = r_u16(object + 136);
        w_u16(object + 136, 0 - value);
    }
    w_u32(0x800FF3AC, 0);
    child = AllocateEffectStorage(104);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingEffect));
        new (dram_addr) RingEffect((const uint32 *)psx_addr(object + 4, 12), 32, 32, 128, 128, 128, 64, 32, 0);
    }
    w_u32(object + 316, child);
    w_u32(0x800FF3AC, 1);
    w_u8(child + 66, 1);
    w_u32(object + 320, (uint32)-30);
    SyncChild();
    value = PlayPositionedSound(15, (const uint32 *)psx_addr(object + 4, 12), 0);
    w_u32(object + 412, value);
    return SetTrailColor(9, 80, 0, 0);
}

TrackingProjectile::TrackingProjectile(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, uint32 list) : ProjectileBase()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 reference[2], target;
    FUNCTION_MARKER(0x8002CE94, "SLUS_003.73");
    w_u32(object + 68, 0x800A1660);
    Configure(position, angles, range, damage, list);
    if (list == 0x800FF5A0)
    {
        target = r_u32(0x800FF5A0);
        MakeObjectReference(reference, target);
        w_u32(object + 384, reference[0]);
        w_u32(object + 388, reference[1]);
    }
    else
        SelectTarget();
}

TrackingProjectile::TrackingProjectile(const uint32 *position, const sint16 *angles, uint32 range, uint32 damage, uint32 list, const uint32 *target_position) : ProjectileBase()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z;
    FUNCTION_MARKER(0x8002CF60, "SLUS_003.73");
    w_u32(object + 68, 0x800A1660);
    Configure(position, angles, range, damage, list);
    w_u8(object + 392, 1);
    uint32 target_address = GuestMemoryAddress(target_position);
    x = target_address ? r_u32(target_address) : target_position[0];
    y = target_address ? r_u32(target_address + 4) : target_position[1];
    z = target_address ? r_u32(target_address + 8) : target_position[2];
    w_u32(object + 396, x);
    w_u32(object + 400, y);
    w_u32(object + 404, z);
}

uint32 TrackingProjectile::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 age, sound, target, result;
    FUNCTION_MARKER(0x8002D018, "SLUS_003.73");
    age = r_u32(object + 296);
    w_u32(object + 68, 0x800A1660);
    if ((sint32)age < 15)
    {
        sound = r_u32(object + 412);
        StopSoundVoices(sound);
    }
    RemoveFromList((uint32 *)psx_addr(0x800FF220, 4));
    target = ResolveHeapReference((uint32 *)psx_addr(object + 384, 8));
    if (target)
        w_u8(target + 128, 0);
    ProjectileBase::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseObjectStorage(object);
    return result;
}

uint32 TrackingProjectile::ConsiderTarget(uint32 target, sint32 *closest, uint32 pass)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 value, blocked;
    sint32 distance;
    FUNCTION_MARKER(0x8002D0A8, "SLUS_003.73");
    if (r_u8(target + 128) && !pass)
        return 0;
    if (!(r_u16(target + 78) & 0x10))
        return 0;
    if (r_u16(target + 58) == 204)
        return 0;
    distance = (sint32)MeasurePositionDistance((const uint32 *)psx_addr(target + 4, 12), (const uint32 *)psx_addr(object + 4, 12));
    if (distance >= *closest)
        return 0;
    value = r_u32(object + 4);
    w_u32(0x800ED638, value);
    value = r_u32(object + 8);
    w_u32(0x800ED63C, value);
    blocked = r_u32(object + 12);
    w_u32(0x800ED63C, value - 409600);
    w_u32(0x800ED640, blocked);
    value = r_u32(target + 4);
    w_u32(0x800ED644, value);
    value = r_u32(target + 8);
    w_u32(0x800ED648, value);
    value = r_u32(target + 12);
    w_u32(0x800ED64C, value);
    InitializeCollisionRay((uint8 *)psx_addr(0x800ED638, 140));
    w_u32(0x800FF974, 1);
    QueryCollisionRay((uint8 *)psx_addr(0x800ED638, 140), 0);
    blocked = r_u32(0x800ED6A0);
    w_u32(0x800FF974, 0);
    if (blocked)
        return 0;
    *closest = distance;
    return 1;
}

void TrackingProjectile::SelectTarget()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 selected = 0, pass = 0, list, candidate, reference[2];
    sint32 closest = 1000000;
    FUNCTION_MARKER(0x8002D1D4, "SLUS_003.73");
    while (pass < 2)
    {
        list = r_u32(object + 304);
        candidate = r_u32(list);
        while (candidate)
        {
            if (ConsiderTarget(candidate, &closest, pass))
                selected = candidate;
            candidate = r_u32(candidate + 28);
        }
        candidate = r_u32(0x800FF5DC);
        while (candidate)
        {
            if (r_u16(candidate + 58) == 205 && ConsiderTarget(candidate, &closest, pass))
                selected = candidate;
            candidate = r_u32(candidate + 28);
        }
        ++pass;
        if (selected)
            break;
    }
    MakeObjectReference(reference, selected);
    w_u32(object + 384, reference[0]);
    w_u32(object + 388, reference[1]);
}

uint32 TrackingProjectile::Explode()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 list, random, kind, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x8002D2F0, "SLUS_003.73");
    list = r_u32(object + 304);
    if (list == 0x800FF5A0)
    {
        SpawnFireCycle((const uint32 *)psx_addr(object + 4, 12), 180, 1, 2);
        random = RandomRange(2);
        PlayPositionedSound(random + 1, (const uint32 *)psx_addr(object + 4, 12), 0);
    }
    else
        HandleExplosionProximity((const uint32 *)psx_addr(object + 4, 12), 0);
    if (r_u32(0x800FF378) != 9)
    {
        kind = r_u32(object + 312);
        QueueExplosion((const uint32 *)psx_addr(object + 4, 12), kind, 512, 32, 256);
    }
    table = r_u32(object + 68);
    adjustment = (sint16)r_u16(table + 16);
    target = r_u32(table + 20);
    return xport_draft_guest_call1(target, object + (uint32)adjustment);
}

uint32 TrackingProjectile::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 trail, green, kind, red, blue, target, counter, x, y, z;
    uint32 position[3], vector[3];
    uint16 desired[3];
    uint32 scale, distance, packed, tail, desired_tail, age, list, head, hit;
    uint32 damage, table, callback, result, player;
    sint32 adjustment, limit;
    FUNCTION_MARKER(0x8002D3B4, "SLUS_003.73");
    AdvanceMotion();
    SyncChild();
    if (!r_u32(object + 296))
    {
        w_u32(0x800FF3AC, 0);
        trail = AllocateEffectStorage(100);
        if (trail)
        {
            green = r_u8(object + 377);
            kind = r_u8(object + 379);
            red = r_u8(object + 376);
            blue = r_u8(object + 378);
            void *dram_addr = AllocateNativeObjectStorage(trail, sizeof(RibbonTrail));
            new (dram_addr) RibbonTrail((const uint32 *)psx_addr(object + 324, 12), kind, red, green, blue);
        }
        w_u32(object + 336, trail);
        w_u8(trail + 66, 1);
        w_u32(0x800FF3AC, 1);
    }
    trail = r_u32(object + 336);
    SpriteTrail::Attach(trail).SetPosition((const uint32 *)psx_addr(object + 324, 12));
    target = ResolveHeapReference((uint32 *)psx_addr(object + 384, 8));
    if (target && (sub_80062F48(target) || !(r_u16(target + 78) & 0x10)))
        target = 0;
    counter = r_u32(object + 380);
    if (counter)
    {
        --counter;
        w_u32(object + 380, counter);
        if (!counter)
        {
            w_u16(object + 16, 0);
            w_u32(object + 108, 0);
        }
        goto CheckCollision;
    }
    if (!target && !r_u8(object + 392))
    {
        w_u16(object + 132, 0);
        w_u16(object + 134, 0);
        packed = ReadAnglesWord(0x800FF5E4);
        tail = r_u16(0x800FF5E8);
        w_u32(object + 138, packed);
        w_u16(object + 142, tail);
        goto CheckCollision;
    }
    if (!r_u8(object + 392) && r_u32(0x800FF378) == 9 && (sint32)r_u32(object + 296) >= 60 && r_u32(object + 304) == 0x800FF5A0)
    {
        x = r_u32(object + 4);
        w_u8(object + 392, 1);
        y = r_u32(object + 8);
        z = r_u32(object + 12);
        w_u32(object + 384, 0);
        w_u32(object + 388, 0);
        w_u32(object + 396, x);
        w_u32(object + 400, y + 0x1000000);
        w_u32(object + 404, z);
    }
    if (target)
    {
        position[0] = r_u32(target + 4);
        position[1] = r_u32(target + 8);
        position[2] = r_u32(target + 12);
        w_u8(target + 128, 1);
    }
    else
    {
        position[0] = r_u32(object + 396);
        position[1] = r_u32(object + 400);
        position[2] = r_u32(object + 404);
    }
    CalculateDirectionAngles(desired, (const uint32 *)psx_addr(object + 4, 12), position);
    scale = r_u32(object + 408);
    if ((sint32)scale >= 200 || MeasurePositionDistance((const uint32 *)psx_addr(object + 4, 12), position) < 101)
    {
        w_u32(object + 16, desired[0] | ((uint32)desired[1] << 16));
        w_u16(object + 20, desired[2]);
    }
    else
    {
        packed = ReadAnglesWord(object + 16);
        tail = r_u16(object + 20);
        desired_tail = desired[2];
        scale = r_u32(object + 408);
        SVECTOR current_angles, target_angles;
        current_angles.vx = (sint16)packed;
        current_angles.vy = (sint16)(packed >> 16);
        current_angles.vz = (sint16)tail;
        current_angles.pad = 0;
        target_angles.vx = (sint16)desired[0];
        target_angles.vy = (sint16)desired[1];
        target_angles.vz = (sint16)desired_tail;
        target_angles.pad = 0;
        UpdateAngleAcceleration(current_angles, (uint16 *)psx_addr(object + 132, 4), (uint16 *)psx_addr(object + 138, 4), target_angles, scale);
        scale = r_u32(object + 408);
        w_u32(object + 408, scale + 3);
    }
    scale = r_u8(object + 393);
    BuildDirectionVector((uint32 *)psx_addr(object + 104, 12), scale, (const uint16 *)psx_addr(object + 16, 4));
CheckCollision:
    list = r_u32(object + 304);
    head = r_u32(list);
    hit = TraceSegmentCollision((const uint32 *)psx_addr(object + 228, 12), (const uint32 *)psx_addr(object + 4, 12), head);
    if (hit)
    {
        NormalizeFixedVector(vector, (const uint32 *)psx_addr(object + 104, 12));
        damage = r_u16(object + 308);
        table = r_u32(hit + 68);
        kind = r_u32(object + 312);
        adjustment = (sint16)r_u16(table + 48);
        callback = r_u32(table + 52);
        apocalypse_object_virtual52_native(callback, hit + (uint32)adjustment, damage, (const sint32 *)vector, kind);
        x = r_u32(hit + 4);
        y = r_u32(hit + 8);
        z = r_u32(hit + 12);
        w_u32(object + 4, x);
        w_u32(object + 8, y);
        w_u32(object + 12, z);
        CallAction(64);
    }
    age = r_u32(object + 296) + 1;
    w_u32(object + 296, age);
    limit = (sint16)r_u16(0x800EC578);
    if (limit < (sint32)age)
        goto Expired;
    result = r_u8(object + 392);
    if (!result)
        return result;
    result = r_u32(object + 380);
    if (result)
        return result;
    distance = MeasurePositionDistance((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 396, 12));
    result = distance < 64;
    if (!result)
        return result;
Expired:
    player = r_u32(0x800FF5A0);
    distance = MeasurePositionDistance((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(player + 4, 12));
    if (distance < 8000)
        return CallAction(64);
    return CallAction(16);
}

uint32 TrackingProjectile::CallAction(uint32 slot)
{
    uint32 table, target, receiver;
    sint32 adjustment;
    table = r_u32(NativeObjectGuestAddress(this) + 68);
    adjustment = (sint16)r_u16(table + slot);
    target = r_u32(table + slot + 4);
    receiver = NativeObjectGuestAddress(this) + (uint32)adjustment;
    if (target == 0x8002D2F0)
        return TrackingProjectile::Attach(receiver).TrackingProjectile::Explode();
    return xport_draft_guest_call1(target, receiver);
}



extern "C" uint32 sub_8002CE94(uint32 object, uint32 position, uint32 angles, uint32 range, uint32 damage, uint32 list)
{
    FUNCTION_MARKER(0x8002CE94, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(TrackingProjectile));
    new (storage) TrackingProjectile((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6), range, damage, list);
    return object;
}

extern "C" uint32 apocalypse_projectile_2CE94_native(uint32 object, const uint32 *position, uint32 guest_angles, const sint16 *native_angles, uint32 range, uint32 damage, uint32 list)
{
    void *storage = AllocateNativeObjectStorage(object, sizeof(TrackingProjectile));
    new (storage) TrackingProjectile(position, native_angles ? native_angles : (const sint16 *)psx_addr(guest_angles, 6), range, damage, list);
    return object;
}

TrackingProjectile &TrackingProjectile::Attach(uint32 object)
{
    return *static_cast<TrackingProjectile *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_8002CF60(uint32 object, uint32 position, uint32 angles, uint32 range, uint32 damage, uint32 list, uint32 target_position)
{
    FUNCTION_MARKER(0x8002CF60, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(TrackingProjectile));
    new (storage) TrackingProjectile((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6), range, damage, list, (const uint32 *)psx_addr(target_position, 12));
    return object;
}

extern "C" void apocalypse_collision_metadata_native(uint32 output[2], uint32 node)
{
    MakeObjectReference(output, node);
}

extern "C" uint32 sub_8002D018(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002D018, "SLUS_003.73");
    return TrackingProjectile::Attach(object).TrackingProjectile::Destroy(reason);
}

extern "C" uint32 sub_8002D2F0(uint32 object)
{
    FUNCTION_MARKER(0x8002D2F0, "SLUS_003.73");
    return TrackingProjectile::Attach(object).TrackingProjectile::Explode();
}

extern "C" uint32 sub_8002D3B4(uint32 object)
{
    FUNCTION_MARKER(0x8002D3B4, "SLUS_003.73");
    return TrackingProjectile::Attach(object).TrackingProjectile::Update();
}

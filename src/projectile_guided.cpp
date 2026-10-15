#include "game_sdk_bindings.h"
#include "game_object_allocation.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "object_collision_events.h"
#include "sprite_trail.h"
#include "game_random.h"
#include "explosion_effects.h"
#include "angle_math.h"
#include "model_resources.h"
#include "effect_lifecycle.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}

#include "projectile_guided.h"
#include "effect_sprite.h"
#include "waving_beam.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Native input bridge for original vector loads
static uint32 ReadGuidedPositionWord(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

GuidedProjectile::GuidedProjectile(const uint32 *position, uint32 damage, const uint32 *owner_pointer, uint32 flags)
    : ProjectileBase()
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 owner = GuestMemoryAddress(owner_pointer);
    if (!owner)
    {
        // TODO Bind a native owner list without serializing its host pointer
        GameDiagnostic("TODO GuidedProjectile native owner list\n");
        abort();
    }
    uint32 tag, x, y, z, child;
    FUNCTION_MARKER(0x8002D7C4, "SLUS_003.73");
    tag = r_u8(0x800FF645);
    w_u32(object + 68, 0x800A1618);
    tag = FindModelResourceIndex(0x717FB6B2, tag);
    w_u16(object + 22, tag);
    AddToList((uint32 *)psx_addr(0x800FF220, 4));
    w_u16(object + 58, 61);
    w_u32(object + 312, 2);
    w_u16(object + 148, 1);
    x = ReadGuidedPositionWord(position, 0);
    y = ReadGuidedPositionWord(position, 1);
    z = ReadGuidedPositionWord(position, 2);
    w_u32(object + 4, x);
    w_u32(object + 8, y);
    w_u32(object + 12, z);
    w_u16(object + 308, damage);
    w_u32(object + 304, owner);
    w_u32(object + 412, 48);
    SetAngles((const uint16 *)psx_addr(object + 16, 6));
    w_u32(object + 380, 1);
    w_u8(object + 129, 3);
    w_u8(object + 130, 3);
    w_u8(object + 131, 3);
    w_u8(object + 144, 1);
    w_u8(object + 145, 1);
    w_u8(object + 146, 1);
    w_u32(0x800FF3AC, 0);
    child = AllocateEffectStorage(120);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(AnimatedSprite));
        new (dram_addr) AnimatedSprite((const uint32 *)psx_addr(object + 4, 12), (const char *)psx_addr(0x800A15E8, 1), 2048, 1, 0, (uint32)-1);
    }
    w_u32(object + 424, child);
    w_u8(child + 66, 1);
    w_u32(0x800FF3AC, 1);
    CaptureOwner();
    w_u32(object + 388, 1);
    w_u16(object + 420, flags);
}

uint32 GuidedProjectile::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x8002D930, "SLUS_003.73");
    w_u32(object + 68, 0x800A1618);
    RemoveFromList((uint32 *)psx_addr(0x800FF220, 4));
    child = r_u32(object + 424);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    ProjectileBase::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 GuidedProjectile::SetAngles(const uint16 *angles)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_angles = GuestMemoryAddress(angles);
    uint32 upper, lower, word, tail, address, shift;
    FUNCTION_MARKER(0x8002D9C0, "SLUS_003.73");
    if (!guest_angles)
    {
        uint16 last;
        memcpy(&word, angles, 4);
        memcpy(&last, (const uint8 *)angles + 4, 2);
        tail = last;
    }
    else
    {
        upper = r_u32((guest_angles + 3) & ~3);
        lower = r_u32(guest_angles & ~3);
        shift = (guest_angles & 3) * 8;
        word = shift ? (upper << (32 - shift)) | (lower >> shift) : lower;
        tail = r_u16(guest_angles + 4);
    }
    address = object + 19;
    shift = (3 - (address & 3)) * 8;
    w_masked_u32(address & ~3, word >> shift, (1 << ((address & 3) + 1)) - 1);
    address = object + 16;
    shift = (address & 3) * 8;
    w_masked_u32(address & ~3, word << shift, (15 << (address & 3)) & 15);
    w_u16(object + 20, tail);
    return BuildDirectionVector((uint32 *)psx_addr(object + 104, 12), 128, (const uint16 *)psx_addr(object + 16, 4));
}

uint32 GuidedProjectile::SetSize(uint32 size)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, child;
    FUNCTION_MARKER(0x8002DA04, "SLUS_003.73");
    flags = r_u16(object);
    child = r_u32(object + 424);
    w_u16(object + 40, size);
    w_u16(object + 38, size);
    w_u16(object + 36, size);
    flags |= 512;
    w_u16(object, flags);
    if (child)
        AnimatedSprite::Attach(child).SetSize((size >> 1) & 65535);
    return flags;
}

uint32 GuidedProjectile::CaptureOwner()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 address, owner;
    FUNCTION_MARKER(0x8002DA4C, "SLUS_003.73");
    address = r_u32(object + 304);
    owner = r_u32(address);
    w_u32(object + 384, owner);
    return owner;
}

void GuidedProjectile::Launch(uint32 mode, const uint32 *target)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z;
    FUNCTION_MARKER(0x8002DC0C, "SLUS_003.73");
    w_u32(object + 388, 0);
    w_u32(object + 392, mode);
    if (!mode)
    {
        x = ReadGuidedPositionWord(target, 0);
        y = ReadGuidedPositionWord(target, 1);
        z = ReadGuidedPositionWord(target, 2);
        w_u32(object + 396, x);
        w_u32(object + 400, y);
        w_u32(object + 404, z);
    }
}

uint32 GuidedProjectile::SpawnDebris()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index, angle, radius, position[3], child, entry;
    uint16 angles[3];
    FUNCTION_MARKER(0x8002DA60, "SLUS_003.73");
    for (index = 0; index < 6; ++index)
    {
        angle = RandomRange(4096);
        radius = RandomRange(256) + 128;
        entry = 0x800F863C + (angle & 4095) * 4;
        position[0] = r_u32(object + 4);
        position[1] = r_u32(object + 8);
        position[2] = r_u32(object + 12);
        position[0] += radius * (uint32)(sint32)(sint16)r_u16(entry);
        position[2] += radius * (uint32)(sint32)(sint16)r_u16(entry + 2);
        angles[1] = 0;
        angles[2] = 0;
        angles[0] = (uint16)-1024;
        child = AllocateEffectStorage(364);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(WavingBeam));
            new (dram_addr) WavingBeam(position, angles, 512, 60, 2, 160, 160, 160, 5, 0x68B7B136);
        }
    }
    return 0;
}

uint32 GuidedProjectile::Explode()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 table, target, kind;
    sint32 adjustment;
    FUNCTION_MARKER(0x8002DB9C, "SLUS_003.73");
    HandleExplosionProximity((const uint32 *)psx_addr(object + 4, 12), 0);
    kind = r_u32(object + 312);
    QueueExplosion((const uint32 *)psx_addr(object + 4, 12), kind, 512, 32, 256);
    table = r_u32(object + 68);
    adjustment = (sint16)r_u16(table + 16);
    target = r_u32(table + 20);
    return xport_draft_guest_call1(target, object + (uint32)adjustment);
}

// Native bridge retains adjusted guest callbacks for the original slot64 calls
uint32 GuidedProjectile::CallAction()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 table = r_u32(object + 68);
    sint32 adjustment = (sint16)r_u16(table + 64);
    uint32 target = r_u32(table + 68);
    if (target == 0x8002DB9C)
        return GuidedProjectile::Attach(object + (uint32)adjustment).GuidedProjectile::Explode();
    return xport_draft_guest_call1(target, object + (uint32)adjustment);
}

uint32 GuidedProjectile::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 owner, result, child, delay, mode, target, packed, tail, scale;
    uint32 vector[4], list, hit, damage, table, kind, callback, x, y, z, count, age;
    uint16 desired[4];
    sint32 adjustment;
    FUNCTION_MARKER(0x8002DC38, "SLUS_003.73");
    owner = r_u32(object + 384);
    if (owner)
    {
        result = sub_80062F48(owner);
        if (result)
            w_u32(object + 384, 0);
        else
        {
            owner = r_u32(object + 384);
            if (r_u8(owner + 224))
                w_u32(object + 384, 0);
        }
    }
    result = r_u32(object + 388);
    if (result)
    {
        child = r_u32(object + 424);
        return child ? static_cast<EffectObject *>(FindNativeObjectStorage(child))->SetPosition((const uint32 *)psx_addr(object + 4, 12)) : result;
    }
    AdvanceMotion();
    if (r_u16(object + 78) & 64)
    {
        result = r_u16(object + 420) & 1;
        return result ? SpawnDebris() : result;
    }
    child = r_u32(object + 424);
    if (child)
        static_cast<EffectObject *>(FindNativeObjectStorage(child))->SetPosition((const uint32 *)psx_addr(object + 4, 12));
    child = r_u32(object + 336);
    if (!child)
    {
        w_u32(0x800FF3AC, 0);
        child = AllocateEffectStorage(100);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RibbonTrail));
            new (dram_addr) RibbonTrail((const uint32 *)psx_addr(object + 4, 12), 9, 80, 0, 0);
        }
        w_u32(object + 336, child);
        w_u8(child + 66, 1);
        child = r_u32(object + 336);
        w_u32(0x800FF3AC, 1);
    }
    if (child)
        SpriteTrail::Attach(r_u32(object + 336)).SetPosition((const uint32 *)psx_addr(object + 4, 12));
    delay = r_u32(object + 380);
    if (delay)
    {
        --delay;
        w_u32(object + 380, delay);
        if (!delay)
        {
            w_u16(object + 16, 0);
            w_u32(object + 108, 0);
        }
    }
    else
    {
        target = r_u32(object + 384);
        if (target || !r_u32(object + 392))
        {
            mode = r_u32(object + 392);
            CalculateDirectionAngles(desired, (const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(mode ? target + 4 : object + 396, 12));
            scale = r_u32(object + 412);
            if ((sint32)scale < 100)
            {
                packed = ReadAnglesWord(object + 16);
                tail = r_u16(object + 20);
                scale = r_u32(object + 412);
                SVECTOR current_angles, target_angles;
                current_angles.vx = (sint16)packed;
                current_angles.vy = (sint16)(packed >> 16);
                current_angles.vz = (sint16)tail;
                current_angles.pad = 0;
                target_angles.vx = (sint16)desired[0];
                target_angles.vy = (sint16)desired[1];
                target_angles.vz = (sint16)desired[2];
                target_angles.pad = 0;
                UpdateAngleAcceleration(current_angles, (uint16 *)psx_addr(object + 132, 4), (uint16 *)psx_addr(object + 138, 4), target_angles, scale);
                scale = r_u32(object + 412);
                w_u32(object + 412, scale + 2);
            }
            else
            {
                packed = desired[0] | ((uint32)desired[1] << 16);
                w_u32(object + 16, packed);
                w_u32(object + 16, packed);
                w_u16(object + 20, desired[2]);
            }
            BuildDirectionVector((uint32 *)psx_addr(object + 104, 12), 128, (const uint16 *)psx_addr(object + 16, 4));
        }
        else
        {
            w_u16(object + 132, 0);
            w_u16(object + 134, 0);
            packed = ReadAnglesWord(0x800FF5E4);
            tail = r_u16(0x800FF5E8);
            w_masked_u32(object + 140, packed >> 16, 3);
            w_masked_u32(object + 136, packed << 16, 12);
            w_u16(object + 142, tail);
        }
    }
    list = r_u32(object + 304);
    owner = r_u32(list);
    hit = TraceSegmentCollision((const uint32 *)psx_addr(object + 228, 12), (const uint32 *)psx_addr(object + 4, 12), owner);
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
        CallAction();
    }
    if (!r_u32(object + 392) && (sint32)MeasurePositionDistance((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 396, 12)) < 128)
    {
        count = (r_u8(object + 408) + 1) & 255;
        w_u8(object + 408, count);
        if (count >= 3)
            CallAction();
    }
    age = r_u32(object + 296) + 1;
    w_u32(object + 296, age);
    result = (sint32)age < 121;
    return result ? result : CallAction();
}

GuidedProjectile &GuidedProjectile::Attach(uint32 object)
{
    return *static_cast<GuidedProjectile *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_8002D7C4(uint32 object, uint32 position, uint32, uint32 damage, uint32 owner, uint32 flags)
{
    FUNCTION_MARKER(0x8002D7C4, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(GuidedProjectile));
    new (dram_addr) GuidedProjectile((const uint32 *)psx_addr(position, 12), damage, (const uint32 *)psx_addr(owner, 4), flags);
    return object;
}

extern "C" uint32 xport_draft_host_sub_8002D7C4_p2(uint32 object, const uint32 *position, uint32, uint32 damage, uint32 owner, uint32 flags)
{
    FUNCTION_MARKER(0x8002D7C4, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(GuidedProjectile));
    new (dram_addr) GuidedProjectile(position, damage, (const uint32 *)psx_addr(owner, 4), flags);
    return object;
}

extern "C" uint32 sub_8002D9C0(uint32 object, uint32 angles)
{
    FUNCTION_MARKER(0x8002D9C0, "SLUS_003.73");
    return GuidedProjectile::Attach(object).GuidedProjectile::SetAngles((const uint16 *)psx_addr(angles, 6));
}

extern "C" uint32 apocalypse_guided_projectile_angles_native(uint32 object, const uint16 *angles)
{
    return GuidedProjectile::Attach(object).GuidedProjectile::SetAngles(angles);
}

extern "C" uint32 sub_8002DA4C(uint32 object)
{
    FUNCTION_MARKER(0x8002DA4C, "SLUS_003.73");
    return GuidedProjectile::Attach(object).GuidedProjectile::CaptureOwner();
}

extern "C" void sub_8002DC0C(uint32 object, uint32 mode, uint32 target)
{
    FUNCTION_MARKER(0x8002DC0C, "SLUS_003.73");
    GuidedProjectile::Attach(object).GuidedProjectile::Launch(mode, mode ? 0 : (const uint32 *)psx_addr(target, 12));
}

extern "C" uint32 sub_8002D930(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002D930, "SLUS_003.73");
    return GuidedProjectile::Attach(object).GuidedProjectile::Destroy(reason);
}

extern "C" uint32 sub_8002DA60(uint32 object)
{
    FUNCTION_MARKER(0x8002DA60, "SLUS_003.73");
    return GuidedProjectile::Attach(object).GuidedProjectile::SpawnDebris();
}

extern "C" uint32 sub_8002DB9C(uint32 object)
{
    FUNCTION_MARKER(0x8002DB9C, "SLUS_003.73");
    return GuidedProjectile::Attach(object).GuidedProjectile::Explode();
}

extern "C" uint32 sub_8002DC38(uint32 object)
{
    FUNCTION_MARKER(0x8002DC38, "SLUS_003.73");
    return GuidedProjectile::Attach(object).GuidedProjectile::Update();
}

extern "C" uint32 sub_8002DA04(uint32 object, uint32 size)
{
    FUNCTION_MARKER(0x8002DA04, "SLUS_003.73");
    // TODO Original allocation failure falls through with a null receiver; native binding fails fast
    return GuidedProjectile::Attach(object).SetSize(size);
}

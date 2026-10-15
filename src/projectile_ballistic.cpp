#include "model_resources.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "audio_session.h"
#include "angle_math.h"
#include "ring_burst_effect.h"
#include "sprite_trail.h"
#include "object_collision_events.h"
#include "game_object_allocation.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "projectile_tracking.h"
#include "projectile_ballistic.h"
#include "object_damage.h"
#include "vector_arithmetic.h"
#include "explosion_effects.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

BallisticProjectile::BallisticProjectile(const uint32 *position, const sint16 *angles, uint32 damage, uint32 speed, uint32 list) : ProjectileBase()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 guest_position = GuestMemoryAddress(position), guest_angles = GuestMemoryAddress(angles);
    const uint32 *native_position = guest_position ? 0 : position;
    const sint16 *native_angles = guest_angles ? 0 : angles;
    uint32 model, x, y, z, packed_angles, value, child;
    sint16 roll;
    FUNCTION_MARKER(0x8002B6A0, "SLUS_003.73");
    value = r_u8(0x800FF645);
    w_u32(object + 68, 0x800A12F0);
    model = FindModelResourceIndex(0x350E968C, value);
    w_u16(object + 22, model);
    AddToList((uint32 *)psx_addr(0x800FF220, 4));
    w_u32(object + 312, 32);
    w_u16(object + 308, damage);
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
    BuildDirectionVector((uint32 *)psx_addr(object + 104, 12), speed, (const uint16 *)psx_addr(object + 16, 4));
    w_u32(object + 304, list);
    w_u16(object + 58, list == 0x800FF5A0 ? 65 : 66);
    w_u32(object + 300, 60);
    PrepareTrajectory(speed);
    if (!r_u32(object + 300))
        ResolveImpact();
    w_u16(object + 136, 400);
    if (RandomRange(2))
    {
        value = r_u16(object + 136);
        w_u16(object + 136, 0 - value);
    }
    PlayPositionedSound(15, (const uint32 *)psx_addr(object + 4, 12), 0);
    child = AllocateEffectStorage(124);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingBurstEffect));
        new (dram_addr) RingBurstEffect((const uint32 *)psx_addr(object + 4, 12), 5, 255, 255, 255, 2, 0, 0, 200, 20, 40, 0, 0, 28, 0, 20, 40, 0, 2);
    }
    BuildDirectionVector((uint32 *)psx_addr(object + 324, 12), (uint32)-48, (const uint16 *)psx_addr(object + 16, 4));
    AddVectors((uint32 *)psx_addr(object + 324, 12), (const uint32 *)psx_addr(object + 4, 12));
    child = AllocateEffectStorage(104);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingEffect));
        new (dram_addr) RingEffect((const uint32 *)psx_addr(object + 324, 12), 32, 32, 128, 128, 128, 16, 16, 64);
    }
    w_u32(object + 316, child);
    w_u8(child + 66, 1);
    SetTrailColor(12, 64, 64, 128);
}

uint32 BallisticProjectile::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8002B918, "SLUS_003.73");
    w_u32(object + 68, 0x800A12F0);
    RemoveFromList((uint32 *)psx_addr(0x800FF220, 4));
    result = ProjectileBase::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(object);
    return result;
}

uint32 BallisticProjectile::ResolveImpact()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 list, head, target, polygon, damage, table, callback, position[3];
    sint32 adjustment;
    FUNCTION_MARKER(0x8002B97C, "SLUS_003.73");
    list = r_u32(object + 304);
    head = r_u32(list);
    ApplyBlast(head, 2000, 10, 32);
    head = r_u32(0x800FF5DC);
    ApplyBlast(head, 2000, 10, 32);
    target = r_u32(object + 344);
    if (target)
    {
        polygon = r_u32(object + 348);
        damage = r_u16(object + 308);
        DamageLevelObject(target, polygon, (sint32)damage);
    }
    DamageNearbyObjects(1000);
    SubtractVectors(position, (const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 104, 12));
    HandleExplosionProximity(position, 1);
    table = r_u32(object + 68);
    adjustment = (sint16)r_u16(table + 16);
    callback = r_u32(table + 20);
    return xport_draft_guest_call1(callback, object + (uint32)adjustment);
}

uint32 BallisticProjectile::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 trail, green, kind, red, blue, before[3], child, age, list, head, lifetime;
    FUNCTION_MARKER(0x8002BA40, "SLUS_003.73");
    age = r_u32(object + 296);
    if (!age)
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
        trail = r_u32(object + 336);
        SpriteTrail::Attach(trail).SetWidth(800);
        w_u32(0x800FF3AC, 1);
    }
    trail = r_u32(object + 336);
    SpriteTrail::Attach(trail).SetPosition((const uint32 *)psx_addr(object + 324, 12));
    before[0] = r_u32(object + 4);
    before[1] = r_u32(object + 8);
    before[2] = r_u32(object + 12);
    AddVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 104, 12));
    AddVectors((uint32 *)psx_addr(object + 324, 12), (const uint32 *)psx_addr(object + 104, 12));
    child = r_u32(object + 316);
    RingEffect::Attach(child).SetPosition((const uint32 *)psx_addr(object + 324, 12));
    AddAngleVector((uint16 *)psx_addr(object + 16, 6), (const uint16 *)psx_addr(object + 132, 6));
    age = r_u32(object + 296);
    list = r_u32(object + 304);
    w_u32(object + 296, age + 1);
    head = r_u32(list);
    if (TraceSegmentCollision(before, (const uint32 *)psx_addr(object + 4, 12), head))
        ResolveImpact();
    lifetime = r_u32(object + 300);
    if (!lifetime)
        return ResolveImpact();
    --lifetime;
    w_u32(object + 300, lifetime);
    if (!lifetime)
        return ResolveImpact();
    return lifetime;
}

BallisticProjectile &BallisticProjectile::Attach(uint32 object)
{
    return *static_cast<BallisticProjectile *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_8002B918(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002B918, "SLUS_003.73");
    return BallisticProjectile::Attach(object).BallisticProjectile::Destroy(reason);
}
extern "C" uint32 sub_8002B97C(uint32 object)
{
    FUNCTION_MARKER(0x8002B97C, "SLUS_003.73");
    return BallisticProjectile::Attach(object).ResolveImpact();
}
extern "C" uint32 sub_8002BA40(uint32 object)
{
    FUNCTION_MARKER(0x8002BA40, "SLUS_003.73");
    return BallisticProjectile::Attach(object).BallisticProjectile::Update();
}

extern "C" uint32 sub_8002B6A0(uint32 object, uint32 position, uint32 angles, uint32 damage, uint32 speed, uint32 list)
{
    FUNCTION_MARKER(0x8002B6A0, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(BallisticProjectile));
    new (storage) BallisticProjectile((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6), damage, speed, list);
    return object;
}

extern "C" uint32 apocalypse_projectile_2B6A0_native(uint32 object, uint32 guest_position, const uint32 *native_position, uint32 guest_angles, const sint16 *native_angles, uint32 damage, uint32 speed, uint32 list)
{
    void *storage = AllocateNativeObjectStorage(object, sizeof(BallisticProjectile));
    new (storage) BallisticProjectile(native_position ? native_position : (const uint32 *)psx_addr(guest_position, 12), native_angles ? native_angles : (const sint16 *)psx_addr(guest_angles, 6), damage, speed, list);
    return object;
}

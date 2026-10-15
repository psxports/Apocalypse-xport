#include "model_resources.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "audio_session.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "sprite_trail.h"
#include "object_collision_events.h"
#include "game_object_allocation.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "projectile_weaving.h"
#include "ring_effect.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

WeavingProjectile::WeavingProjectile(const uint32 *position, const sint16 *angles, uint32 reverse, uint32 trail_segments, uint32 speed, uint32 owner_pointer, const uint32 *additional_vector)
    : ProjectileBase()
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_angles = GuestMemoryAddress(angles);
    FUNCTION_MARKER(0x8002C66C, "SLUS_003.73");
    uint32 packed, tail, value, child, coordinates[3];
    uint16 rotation[3], tail_half;
    value = r_u8(0x800FF645);
    w_u32(object + 68, 0x800A16A8);
    value = FindModelResourceIndex(0x350E968C, value);
    w_u16(object + 22, value);
    AddToList((uint32 *)psx_addr(0x800FF220, 4));
    w_u16(object + 58, 60);
    w_u32(object + 312, 1);
    w_u16(object + 308, 300);
    w_u32(object + 436, trail_segments);
    if (guest_angles)
    {
        packed = ReadAnglesWord(guest_angles);
        tail = r_u16(guest_angles + 4);
    }
    else
    {
        memcpy(&packed, angles, 4);
        memcpy(&tail_half, (const uint8 *)angles + 4, 2);
        tail = tail_half;
    }
    // Allocated objects have word alignment for the original SWL/SWR pair
    w_u32(object + 16, packed);
    w_u32(object + 16, packed);
    w_u16(object + 20, tail);
    BuildDirectionVector((uint32 *)psx_addr(object + 104, 12), speed, (const uint16 *)psx_addr(object + 16, 4));
    AddVectors((uint32 *)psx_addr(object + 104, 12), additional_vector);
    SubtractVectors(coordinates, position, (const uint32 *)psx_addr(object + 104, 12));
    w_u32(object + 4, coordinates[0]);
    w_u32(object + 8, coordinates[1]);
    w_u32(object + 12, coordinates[2]);
    w_u32(object + 304, owner_pointer);
    w_u32(object + 300, 75);
    PrepareTrajectory(speed);
    w_u8(object + 131, 31);
    w_u8(object + 130, 31);
    w_u8(object + 129, 31);
    w_u8(object + 144, 1);
    w_u8(object + 145, 1);
    w_u8(object + 146, 1);
    w_u16(object + 136, reverse ? (uint32)-400 : 400);
    w_u32(0x800FF3AC, 0);
    child = AllocateEffectStorage(104);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingEffect));
        new (dram_addr) RingEffect((const uint32 *)psx_addr(object + 4, 12), 32, 32, 128, 128, 128, 16, 16, 64);
    }
    w_u32(object + 316, child);
    w_u8(child + 66, 1);
    w_u32(object + 320, (uint32)-70);
    SyncChild();
    packed = ReadAnglesWord(object + 16);
    tail = r_u16(object + 20);
    rotation[0] = (uint16)packed;
    rotation[1] = (uint16)(packed >> 16);
    rotation[2] = (uint16)tail;
    w_u32(0x800FF3AC, 1);
    rotation[1] = (rotation[1] + 1024) & 4095;
    BuildDirectionVector((uint32 *)psx_addr(object + 380, 12), 8, rotation);
    value = r_u16(object + 18);
    rotation[0] = (rotation[0] + 1024) & 4095;
    rotation[1] = (uint16)value;
    BuildDirectionVector((uint32 *)psx_addr(object + 392, 12), 8, rotation);
    value = RandomRange(4096);
    w_u32(object + 428, value);
    value = RandomRange(4096);
    w_u32(object + 432, value);
    PlayPositionedSound(15, position, 0);
}

uint32 WeavingProjectile::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8002C8F8, "SLUS_003.73");
    w_u32(object + 68, 0x800A16A8);
    RemoveFromList((uint32 *)psx_addr(0x800FF220, 4));
    uint32 result = ProjectileBase::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : result;
}

uint32 WeavingProjectile::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 before[3], scaled[3], offset[3], difference[3];
    uint32 age, phase, wave, divisor, effect, hit, list, table, receiver, target, damage, kind, limit, distance;
    FUNCTION_MARKER(0x8002C95C, "SLUS_003.73");
    before[0] = r_u32(object + 4);
    before[1] = r_u32(object + 8);
    before[2] = r_u32(object + 12);
    SubtractVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 404, 12));
    SubtractVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 416, 12));
    AddVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 104, 12));
    age = r_u32(object + 296);
    phase = r_u32(object + 428);
    wave = (uint32)(sint32)(sint16)r_u16(0x800F863C + ((500 * age + phase) & 4095) * 4);
    MultiplyVector(scaled, (const uint32 *)psx_addr(object + 380, 12), &wave);
    divisor = 4096;
    DivideVector(offset, scaled, &divisor);
    w_u32(object + 404, offset[0]);
    w_u32(object + 408, offset[1]);
    w_u32(object + 412, offset[2]);
    age = r_u32(object + 296);
    phase = r_u32(object + 432);
    wave = (uint32)(sint32)(sint16)r_u16(0x800F863C + ((300 * age + phase) & 4095) * 4);
    MultiplyVector(scaled, (const uint32 *)psx_addr(object + 392, 12), &wave);
    divisor = 4096;
    DivideVector(offset, scaled, &divisor);
    w_u32(object + 416, offset[0]);
    w_u32(object + 420, offset[1]);
    w_u32(object + 424, offset[2]);
    AddVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 404, 12));
    AddVectors((uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 416, 12));
    SyncChild();
    if (!r_u32(object + 296))
    {
        w_u32(0x800FF3AC, 0);
        effect = AllocateEffectStorage(100);
        if (effect)
        {
            uint32 segments = r_u32(object + 436);
            void *dram_addr = AllocateNativeObjectStorage(effect, sizeof(RibbonTrail));
            new (dram_addr) RibbonTrail((const uint32 *)psx_addr(object + 324, 12), segments, 128, 128, 128);
        }
        w_u32(object + 336, effect);
        w_u8(effect + 66, 1);
        w_u32(0x800FF3AC, 1);
    }
    effect = r_u32(object + 336);
    SpriteTrail::Attach(effect).SetPosition((const uint32 *)psx_addr(object + 324, 12));
    list = r_u32(object + 304);
    hit = TraceSegmentCollision(before, (const uint32 *)psx_addr(object + 4, 12), r_u32(list));
    if (hit)
    {
        damage = r_u16(object + 308);
        table = r_u32(hit + 68);
        kind = r_u32(object + 312);
        receiver = hit + (uint32)(sint32)(sint16)r_u16(table + 48);
        target = r_u32(table + 52);
        apocalypse_object_virtual52_native(target, receiver, damage, (const sint32 *)psx_addr(0x800A71CC, 12), kind);
        CallAction(64);
    }
    age = r_u32(object + 296);
    limit = r_u32(object + 300);
    ++age;
    w_u32(object + 296, age);
    if ((sint32)limit < (sint32)age)
        CallAction(64);
    SubtractVectors(difference, (const uint32 *)psx_addr(r_u32(0x800FF904) + 4, 12), (const uint32 *)psx_addr(object + 4, 12));
    distance = MeasureSquaredVectorLength(difference);
    limit = r_u32(0x800FFAC8);
    if ((sint32)(limit * limit) < (sint32)distance)
        return CallAction(16);
    return 0;
}

uint32 WeavingProjectile::CallAction(uint32 slot)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 table = r_u32(object + 68);
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + slot);
    uint32 target = r_u32(table + slot + 4);
    if (target == 0x8002BFFC)
        return ProjectileBase::Attach(receiver).ProjectileBase::Explode();
    return xport_draft_guest_call1(target, receiver);
}

WeavingProjectile &WeavingProjectile::Attach(uint32 object)
{
    return *static_cast<WeavingProjectile *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_8002C66C(uint32 object, uint32 position, uint32 angles, uint32 reverse, uint32 trail_segments, uint32 speed, uint32 owner_pointer, uint32 additional_vector)
{
    FUNCTION_MARKER(0x8002C66C, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(WeavingProjectile));
    new (dram_addr) WeavingProjectile((const uint32 *)psx_addr(position, 12), (const sint16 *)psx_addr(angles, 6), reverse, trail_segments, speed, owner_pointer, (const uint32 *)psx_addr(additional_vector, 12));
    return object;
}

extern "C" uint32 apocalypse_chopper_projectile_native(uint32 object, const uint32 position[3], const uint16 angles[3], uint32 reverse, uint32 trail_segments, uint32 speed, uint32 owner_pointer, uint32 additional_vector)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(WeavingProjectile));
    new (dram_addr) WeavingProjectile(position, (const sint16 *)angles, reverse, trail_segments, speed, owner_pointer, (const uint32 *)psx_addr(additional_vector, 12));
    return object;
}

extern "C" uint32 sub_8002C8F8(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002C8F8, "SLUS_003.73");
    return WeavingProjectile::Attach(object).WeavingProjectile::Destroy(reason);
}

extern "C" uint32 sub_8002C95C(uint32 object)
{
    FUNCTION_MARKER(0x8002C95C, "SLUS_003.73");
    return WeavingProjectile::Attach(object).WeavingProjectile::Update();
}

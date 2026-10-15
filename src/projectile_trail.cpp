extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "projectile_trail.h"
#include "ring_effect.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "effect_lifecycle.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

static uint32 ReadTrailPosition(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

TrailProjectile::TrailProjectile(const uint32 *position, const uint32 *target, const uint16 *angles, uint32 lifetime, uint32 speed, uint32 scale, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue) : EffectController()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, child;
    FUNCTION_MARKER(0x8004B08C, "SLUS_003.73");
    w_u32(object + 68, 0x800A2B70);
    x = ReadTrailPosition(position, 0);
    y = ReadTrailPosition(position, 1);
    z = ReadTrailPosition(position, 2);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    x = ReadTrailPosition(target, 0);
    y = ReadTrailPosition(target, 1);
    z = ReadTrailPosition(target, 2);
    w_u32(object + 72, x);
    w_u32(object + 76, y);
    w_u32(object + 80, z);
    CopyAngleVector((uint16 *)psx_addr(object + 84, 6), angles);
    w_u32(object + 48, speed);
    child = AllocateEffectStorage(104);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingEffect));
        new (dram_addr) RingEffect(4, 1);
    }
    w_u32(object + 104, child);
    // TODO Original allocation failure falls through with a null child; native binding fails fast
    RingEffect::Attach(child).SetCenterColor(red & 255, green & 255, blue & 255);
    child = r_u32(object + 104);
    RingEffect::Attach(child).SetRingColors(tail_red & 255, tail_green & 255, tail_blue & 255);
    child = r_u32(object + 104);
    RingEffect::Attach(child).SetRowColors(0, 0, 0, 0);
    child = r_u32(object + 104);
    RingEffect::Attach(child).SetOddRadii(scale);
    child = r_u32(object + 104);
    RingEffect::Attach(child).SetEvenRadii(scale);
    child = r_u32(object + 104);
    RingEffect::Attach(child).SetRowExtent(0, scale << 1);
    child = r_u32(object + 104);
    RingEffect::Attach(child).SetPosition((const uint32 *)psx_addr(object + 24, 12));
    child = r_u32(object + 104);
    w_u8(child + 66, 1);
    w_u16(object + 10, lifetime);
}

uint32 TrailProjectile::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x8004B23C, "SLUS_003.73");
    child = r_u32(object + 104);
    w_u32(object + 68, 0x800A2B70);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

void TrailProjectile::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint16 desired[3], current[3];
    uint32 child, value, acceleration, first, second, age, lifetime;
    SVECTOR current_value, desired_value;
    FUNCTION_MARKER(0x8004B2B8, "SLUS_003.73");
    CalculateDirectionAngles(desired, (const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 72, 12));
    CopyAngleVector(current, (const uint16 *)psx_addr(object + 84, 6));
    current_value.vx = (sint16)current[0];
    current_value.vy = (sint16)current[1];
    current_value.vz = (sint16)current[2];
    current_value.pad = 0;
    desired_value.vx = (sint16)desired[0];
    desired_value.vy = (sint16)desired[1];
    desired_value.vz = (sint16)desired[2];
    desired_value.pad = 0;
    UpdateAngleAcceleration(current_value, (uint16 *)psx_addr(object + 90, 4), (uint16 *)psx_addr(object + 96, 4), desired_value, 4);
    value = r_u32(object + 48);
    BuildDirectionVector((uint32 *)psx_addr(object + 36, 12), value, (const uint16 *)psx_addr(object + 84, 6));
    AddVectors((uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 36, 12));
    child = r_u32(object + 104);
    RingEffect::Attach(child).SetPosition((const uint32 *)psx_addr(object + 24, 12));
    AddAngleVector((uint16 *)psx_addr(object + 84, 6), (const uint16 *)psx_addr(object + 90, 6));
    NormalizeAngles((uint16 *)psx_addr(object + 84, 6));
    value = r_u16(object + 90);
    acceleration = r_u16(object + 96);
    w_u16(object + 90, value + acceleration);
    value = r_u16(object + 92);
    acceleration = r_u16(object + 98);
    first = r_u16(object + 90);
    w_u16(object + 92, value + acceleration);
    second = r_u16(object + 92);
    w_u16(object + 90, first - (uint32)((sint32)(sint16)first >> 2));
    w_u16(object + 92, second - (uint32)((sint32)(sint16)second >> 2));
    SnapSmallAngles((uint16 *)psx_addr(object + 90, 6));
    age = r_u16(object + 8);
    lifetime = r_u16(object + 10);
    age += 1;
    w_u16(object + 8, age);
    if ((sint32)lifetime < (sint16)age)
        Finish();
}

TrailProjectile &TrailProjectile::Attach(uint32 object)
{
    return *static_cast<TrailProjectile *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_8004B08C(uint32 object, uint32 position, uint32 target, uint32 angles, uint32 lifetime, uint32 speed, uint32 scale, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue)
{
    FUNCTION_MARKER(0x8004B08C, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(TrailProjectile));
    new (dram_addr) TrailProjectile((const uint32 *)psx_addr(position, 12), (const uint32 *)psx_addr(target, 12), (const uint16 *)psx_addr(angles, 6), lifetime, speed, scale, red, green, blue, tail_red, tail_green, tail_blue);
    return object;
}

extern "C" uint32 apocalypse_trail_projectile_native(uint32 object, const void *position, const void *target, const void *angles, uint32 lifetime, uint32 speed, uint32 scale, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(TrailProjectile));
    new (dram_addr) TrailProjectile((const uint32 *)position, (const uint32 *)target, (const uint16 *)angles, lifetime, speed, scale, red, green, blue, tail_red, tail_green, tail_blue);
    return object;
}

extern "C" uint32 sub_8004B23C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8004B23C, "SLUS_003.73");
    return TrailProjectile::Attach(object).TrailProjectile::Destroy(reason);
}

extern "C" void sub_8004B2B8(uint32 object)
{
    FUNCTION_MARKER(0x8004B2B8, "SLUS_003.73");
    TrailProjectile::Attach(object).TrailProjectile::Update();
}

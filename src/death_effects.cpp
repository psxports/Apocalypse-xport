#include "death_effects.h"
#include "native_object_storage.h"
#include "effect_lifecycle.h"
#include <new>

// Recovered inline construction at 80049574..800495B0
DeathPersistentRing::DeathPersistentRing(const uint32 *position) : MeshRingEffect(12, 0xCCE0187C)
{
    w_u32(NativeObjectGuestAddress(this) + 68, 0x800A2608);
    SetGeometry(position, 32, 32);
    SetColors(64, 64, 240);
}

// Recovered inline construction at 8004911C..80049158
DeathFadingRing::DeathFadingRing(const uint32 *position) : MeshRingEffect(12, 0xCCE0187C)
{
    w_u32(NativeObjectGuestAddress(this) + 68, 0x800A2628);
    SetGeometry(position, 32, 32);
    SetColors(240, 96, 96);
}

DeathPersistentRing &DeathPersistentRing::Attach(uint32 object)
{
    return *static_cast<DeathPersistentRing *>(FindNativeObjectStorage(object));
}

DeathFadingRing &DeathFadingRing::Attach(uint32 object)
{
    return *static_cast<DeathFadingRing *>(FindNativeObjectStorage(object));
}

// Native factories extract the original allocation and inline construction
void SpawnDeathPersistentRing(const uint32 *position)
{
    uint32 object = AllocateEffectStorage(116);
    if (object)
    {
        void *dram_addr = AllocateNativeObjectStorage(object, sizeof(DeathPersistentRing));
        new (dram_addr) DeathPersistentRing(position);
    }
}

void SpawnDeathFadingRing(const uint32 *position)
{
    uint32 object = AllocateEffectStorage(116);
    if (object)
    {
        void *dram_addr = AllocateNativeObjectStorage(object, sizeof(DeathFadingRing));
        new (dram_addr) DeathFadingRing(position);
    }
}

uint32 DeathPersistentRing::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 age, radius, expansion;
    FUNCTION_MARKER(0x80049D54, "SLUS_003.73");
    age = r_u16(object + 8) + 1;
    w_u16(object + 8, age);
    age = (uint32)(sint32)(sint16)age;
    if ((sint32)age < 30)
    {
        radius = r_u32(object + 92);
        expansion = r_u32(object + 96);
        radius += 3;
        w_u32(object + 92, radius);
        expansion += 7;
    }
    else
    {
        if ((sint32)age < 61)
            return 1;
        if ((sint32)age >= 90)
            return Finish();
        radius = r_u32(object + 92);
        expansion = r_u32(object + 96);
        radius -= 3;
        w_u32(object + 92, radius);
        expansion -= 7;
    }
    w_u32(object + 96, expansion);
    return SetGeometry((const uint32 *)psx_addr(object + 24, 12), radius, expansion);
}

uint32 DeathFadingRing::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 age, red, green, radius, expansion;
    FUNCTION_MARKER(0x80049E38, "SLUS_003.73");
    age = r_u16(object + 8) + 1;
    w_u16(object + 8, age);
    if ((sint16)age >= 32)
        return Finish();
    age = (uint32)(sint32)(sint16)r_u16(object + 8);
    red = 240 - age * 6;
    green = 96 - age * 3;
    radius = r_u32(object + 92);
    expansion = r_u32(object + 96);
    radius += 1;
    expansion += 7;
    w_u32(object + 92, radius);
    w_u32(object + 96, expansion);
    SetColors(red, green, green);
    radius = r_u32(object + 92);
    expansion = r_u32(object + 96);
    return SetGeometry((const uint32 *)psx_addr(object + 24, 12), radius, expansion);
}

uint32 DeathPersistentRing::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80049DF0, "SLUS_003.73");
    uint32 result = MeshRingEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 DeathFadingRing::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80049EDC, "SLUS_003.73");
    uint32 result = MeshRingEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

extern "C" uint32 sub_80049D54(uint32 object)
{
    FUNCTION_MARKER(0x80049D54, "SLUS_003.73");
    return DeathPersistentRing::Attach(object).DeathPersistentRing::Update();
}
extern "C" uint32 sub_80049E38(uint32 object)
{
    FUNCTION_MARKER(0x80049E38, "SLUS_003.73");
    return DeathFadingRing::Attach(object).DeathFadingRing::Update();
}
extern "C" uint32 sub_80049DF0(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80049DF0, "SLUS_003.73");
    return DeathPersistentRing::Attach(object).DeathPersistentRing::Destroy(reason);
}
extern "C" uint32 sub_80049EDC(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80049EDC, "SLUS_003.73");
    return DeathFadingRing::Attach(object).DeathFadingRing::Destroy(reason);
}

#include "game_random.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "plague_ground_effects.h"
#include "effect_lifecycle.h"
#include "vector_distance.h"
#include "ring_burst_effect.h"
#include "mesh_ring_effect.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

static uint32 ReadPlagueEffectPosition(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

PlagueImpactEffect::PlagueImpactEffect(const uint32 *position, uint32 red, uint32 green, uint32 blue) : EffectController()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, child, first, second;
    FUNCTION_MARKER(0x80044A58, "SLUS_003.73");
    w_u32(object + 68, 0x800A24FC);
    w_u8(object + 67, 3);
    x = ReadPlagueEffectPosition(position, 0);
    y = ReadPlagueEffectPosition(position, 1);
    z = ReadPlagueEffectPosition(position, 2);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    child = AllocateEffectStorage(116);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(MeshRingEffect));
        new (dram_addr) MeshRingEffect(16, 0xCCE0187C);
    }
    w_u32(object + 72, child);
    // TODO Original allocation failure still writes through a null child
    w_u8(child + 66, 1);
    child = r_u32(object + 72);
    MeshRingEffect::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 24, 12), 0, 0);
    first = RandomRange(64);
    second = RandomRange(64);
    child = r_u32(object + 72);
    MeshRingEffect::Attach(child).SetTextureOffsets(first, second, 2);
    child = r_u32(object + 72);
    w_u16(child + 114, 32);
    child = r_u32(object + 72);
    w_u32(object + 80, red);
    w_u32(object + 84, green);
    w_u32(object + 88, blue);
    MeshRingEffect::Attach(child).SetColors(red, green, blue);
    w_u32(object + 92, 64);
}

uint32 PlagueImpactEffect::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x80044B80, "SLUS_003.73");
    child = r_u32(object + 72);
    w_u32(object + 68, 0x800A24FC);
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

void PlagueImpactEffect::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 age, radius, child, width;
    FUNCTION_MARKER(0x80044BFC, "SLUS_003.73");
    age = r_u16(object + 8) + 1;
    w_u16(object + 8, age);
    if ((sint16)age >= 13)
    {
        Finish();
        return;
    }
    radius = r_u32(object + 76);
    child = r_u32(object + 72);
    width = r_u32(object + 92);
    radius += 48;
    w_u32(object + 76, radius);
    MeshRingEffect::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 24, 12), radius, width);
}

PlagueGroundEffect::PlagueGroundEffect(const uint32 *position, uint32 damage, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 radial_speed) : EffectController()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z;
    FUNCTION_MARKER(0x80044C64, "SLUS_003.73");
    w_u32(object + 68, 0x800A24E4);
    w_u8(object + 67, 3);
    x = ReadPlagueEffectPosition(position, 0);
    y = ReadPlagueEffectPosition(position, 1);
    z = ReadPlagueEffectPosition(position, 2);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    w_u32(object + 96, damage);
    w_u32(object + 80, red);
    w_u32(object + 100, 128);
    w_u32(object + 104, radial_speed);
    w_u32(object + 92, segments);
    w_u32(object + 84, green);
    w_u32(object + 88, blue);
}

uint32 PlagueGroundEffect::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target;
    sint32 adjustment;
    FUNCTION_MARKER(0x80044D2C, "SLUS_003.73");
    child = r_u32(object + 72);
    w_u32(object + 68, 0x800A24E4);
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

void PlagueGroundEffect::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 age, child, first, second, red, green, blue, old_radius, speed, player, distance, radius, width, y, height, player_y, table, damage, target;
    uint32 position[3];
    sint32 adjustment;
    FUNCTION_MARKER(0x80044DA8, "SLUS_003.73");
    age = r_u16(object + 8) + 1;
    w_u16(object + 8, age);
    if ((sint16)age >= 8 && !r_u32(object + 72))
    {
        child = AllocateEffectStorage(116);
        if (child)
        {
            uint32 segments = r_u32(object + 92);
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(MeshRingEffect));
            new (dram_addr) MeshRingEffect(segments, 0xCCE0187C);
        }
        w_u32(object + 72, child);
        // TODO Original allocation failure still writes through a null child
        w_u8(child + 66, 1);
        child = r_u32(object + 72);
        MeshRingEffect::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 24, 12), 0, 0);
        first = RandomRange(64);
        second = RandomRange(64);
        child = r_u32(object + 72);
        MeshRingEffect::Attach(child).SetTextureOffsets(first, second, 2);
        child = r_u32(object + 72);
        w_u16(child + 114, 32);
        child = r_u32(object + 72);
        red = r_u32(object + 80);
        green = r_u32(object + 84);
        blue = r_u32(object + 88);
        MeshRingEffect::Attach(child).SetColors(red, green, blue);
        return;
    }
    age = (uint32)(sint32)(sint16)r_u16(object + 8);
    if ((sint32)age >= 91)
    {
        Finish();
        return;
    }
    if (!r_u32(object + 72))
        return;
    old_radius = r_u32(object + 76);
    speed = r_u32(object + 104);
    player = r_u32(0x800FF5A0);
    w_u32(object + 76, old_radius + speed);
    if (player && !r_u32(object + 108))
    {
        distance = MeasureHorizontalDistance((const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(player + 4, 12));
        if ((sint32)distance >= (sint32)old_radius)
        {
            radius = r_u32(object + 76);
            width = r_u32(object + 100);
            if ((sint32)(radius + width) >= (sint32)distance)
            {
                player = r_u32(0x800FF5A0);
                y = r_u32(object + 28);
                height = r_u8(player + 582);
                player_y = r_u32(player + 8);
                if ((sint32)y < (sint32)(player_y + (height << 12)))
                {
                    table = r_u32(player + 68);
                    damage = r_u32(object + 96);
                    adjustment = (sint16)r_u16(table + 48);
                    target = r_u32(table + 52);
                    apocalypse_object_virtual52(target, player + (uint32)adjustment, damage, 0x800A71CC, 0);
                    player = r_u32(0x800FF5A0);
                    position[0] = r_u32(player + 4);
                    position[1] = r_u32(player + 8);
                    position[2] = r_u32(player + 12);
                    position[1] = r_u32(object + 28);
                    child = AllocateEffectStorage(96);
                    if (child)
                    {
                        red = r_u32(object + 80);
                        green = r_u32(object + 84);
                        blue = r_u32(object + 88);
                        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(PlagueImpactEffect));
                        new (dram_addr) PlagueImpactEffect(position, (uint32)((sint32)red >> 1), (uint32)((sint32)green >> 1), (uint32)((sint32)blue >> 1));
                    }
                    child = AllocateEffectStorage(124);
                    if (child)
                    {
                        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingBurstEffect));
                        new (dram_addr) RingBurstEffect(position, 5, 255, 255, 255, 1, 64, 160, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 8);
                    }
                    w_u32(object + 108, 1);
                }
            }
        }
    }
    child = r_u32(object + 72);
    radius = r_u32(object + 76);
    width = r_u32(object + 100);
    MeshRingEffect::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 24, 12), radius, width);
    child = r_u32(object + 72);
    second = r_u32(child + 104);
    first = r_u32(child + 100);
    MeshRingEffect::Attach(child).SetTextureOffsets(first, second - 1, 2);
}

PlagueImpactEffect &PlagueImpactEffect::Attach(uint32 object)
{
    return *static_cast<PlagueImpactEffect *>(FindNativeObjectStorage(object));
}

PlagueGroundEffect &PlagueGroundEffect::Attach(uint32 object)
{
    return *static_cast<PlagueGroundEffect *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_80044C64(uint32 object, uint32 position, uint32 damage, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 radial_speed)
{
    FUNCTION_MARKER(0x80044C64, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(PlagueGroundEffect));
    new (dram_addr) PlagueGroundEffect((const uint32 *)psx_addr(position, 12), damage, red, green, blue, segments, radial_speed);
    return object;
}

extern "C" uint32 apocalypse_enemy_effect_constructor_native(uint32 object, const void *position, uint32 damage, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 radial_speed)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(PlagueGroundEffect));
    new (dram_addr) PlagueGroundEffect((const uint32 *)position, damage, red, green, blue, segments, radial_speed);
    return object;
}

extern "C" uint32 sub_80044B80(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80044B80, "SLUS_003.73");
    return PlagueImpactEffect::Attach(object).PlagueImpactEffect::Destroy(reason);
}

extern "C" void sub_80044BFC(uint32 object)
{
    FUNCTION_MARKER(0x80044BFC, "SLUS_003.73");
    PlagueImpactEffect::Attach(object).PlagueImpactEffect::Update();
}

extern "C" uint32 sub_80044D2C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80044D2C, "SLUS_003.73");
    return PlagueGroundEffect::Attach(object).PlagueGroundEffect::Destroy(reason);
}

extern "C" void sub_80044DA8(uint32 object)
{
    FUNCTION_MARKER(0x80044DA8, "SLUS_003.73");
    PlagueGroundEffect::Attach(object).PlagueGroundEffect::Update();
}

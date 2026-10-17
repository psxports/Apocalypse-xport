extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "war_effects.h"
#include "fire_cycle.h"
#include "mesh_ring_effect.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "audio_session.h"
#include "vector_arithmetic.h"
#include "native_object_storage.h"
#include <new>

WarImpact::WarImpact(const uint32 *position) : EffectController()
{
    uint32 guest_position = GuestMemoryAddress(position);
    const uint32 *native_position = guest_position ? 0 : position;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, child, first, second;
    FUNCTION_MARKER(0x8003D134, "SLUS_003.73");
    w_u32(object + 68, 0x800A1E9C);
    w_u8(object + 67, 3);
    x = native_position ? native_position[0] : r_u32(guest_position);
    y = native_position ? native_position[1] : r_u32(guest_position + 4);
    z = native_position ? native_position[2] : r_u32(guest_position + 8);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    w_u32(object + 76, 128);
    child = AllocateEffectStorage(116);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(MeshRingEffect));
        new (dram_addr) MeshRingEffect(16, 0xCCE0187C);
    }
    w_u32(object + 72, child);
    w_u8(child + 66, 1);
    MeshRingEffect::Attach(r_u32(object + 72)).SetGeometry((const uint32 *)psx_addr(object + 24, 12), 0, 0);
    first = RandomRange(64);
    second = RandomRange(64);
    MeshRingEffect::Attach(r_u32(object + 72)).SetTextureOffsets(first, second, 2);
    child = r_u32(object + 72);
    w_u16(child + 114, 32);
    MeshRingEffect::Attach(r_u32(object + 72)).SetColors(128, 128, 64);
}

uint32 WarImpact::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target, receiver;
    FUNCTION_MARKER(0x8003D22C, "SLUS_003.73");
    child = r_u32(object + 72);
    w_u32(object + 68, 0x800A1E9C);
    if (child)
    {
        table = r_u32(child + 68);
        receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, receiver, 3);
    }
    EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : 0;
}

uint32 WarImpact::Update()
{
    uint32 trig_angle;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 radius, child, value, index, phase, position[3];
    FUNCTION_MARKER(0x8003D2A8, "SLUS_003.73");
    if ((sint16)r_u16(object + 8) >= 46)
        Finish();
    else
    {
        radius = r_u32(object + 76);
        child = r_u32(object + 72);
        radius += 128;
        w_u32(object + 76, radius);
        MeshRingEffect::Attach(child).SetGeometry((const uint32 *)psx_addr(object + 24, 12), radius, 512);
        if (!(r_u16(object + 8) & 3))
        {
            value = RandomRange(2);
            PlaySoundEffect(value ? 1 : 2, 8192, 0);
            for (index = 0, phase = 0; index < 6; ++index, phase += 682)
            {
                value = r_u32(object + 80);
                radius = r_u32(object + 76);
                trig_angle = (value + phase) & 4095;
                position[0] = radius * (uint32)rsin(trig_angle);
                value = (uint32)rcos(trig_angle);
                position[1] = 0;
                position[2] = radius * value;
                AddVectors(position, (const uint32 *)psx_addr(object + 24, 12));
                child = AllocateEffectStorage(156);
                if (child)
                {
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(FireCycle));
                    new (dram_addr) FireCycle(position, 45, 0, 2);
                }
            }
            value = r_u32(object + 80);
            w_u32(object + 80, (value + 423) & 4095);
        }
    }
    value = r_u16(object + 8) + 1;
    w_u16(object + 8, value);
    return value;
}



extern "C" uint32 apocalypse_war_impact_native(uint32 object, const uint32 position[3])
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(WarImpact));
    new (dram_addr) WarImpact(position);
    return object;
}

extern "C" uint32 sub_8003D22C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8003D22C, "SLUS_003.73");
    return WarImpact::Attach(object).WarImpact::Destroy(reason);
}

extern "C" uint32 sub_8003D2A8(uint32 object)
{
    FUNCTION_MARKER(0x8003D2A8, "SLUS_003.73");
    return WarImpact::Attach(object).WarImpact::Update();
}

WarImpact &WarImpact::Attach(uint32 object)
{
    return *static_cast<WarImpact *>(FindNativeObjectStorage(object));
}

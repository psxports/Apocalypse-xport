extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "proximity_ring_emitter.h"
#include "oscillating_ring.h"
#include "effect_lifecycle.h"
#include "native_object_storage.h"
#include <new>

ProximityRingEmitter::ProximityRingEmitter(const uint32 *position, uint32 index, uint32 range, uint32 field72, uint32 field76, uint32 field88, uint32 red0, uint32 green0, uint32 blue0, uint32 red1, uint32 green1, uint32 blue1) : EffectController()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8001C8E8, "SLUS_003.73");
    w_u32(object + 68, 0x800A10B4);
    SetPosition(position);
    w_u32(object + 92, range);
    w_u8(object + 67, 2);
    w_u16(object + 10, index);
    w_u32(object + 72, field72);
    w_u32(object + 76, field76);
    w_u8(object + 80, red0);
    w_u8(object + 81, green0);
    w_u8(object + 82, blue0);
    w_u8(object + 83, red1);
    w_u8(object + 84, green1);
    w_u8(object + 85, blue1);
    w_u32(object + 88, field88);
}

ProximityRingEmitter &ProximityRingEmitter::Attach(uint32 object)
{
    return *static_cast<ProximityRingEmitter *>(FindNativeObjectStorage(object));
}

uint32 ProximityRingEmitter::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, receiver, target;
    FUNCTION_MARKER(0x8001C9EC, "SLUS_003.73");
    child = r_u32(object + 96);
    w_u32(object + 68, 0x800A10B4);
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

uint32 ProximityRingEmitter::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 player, x, z, distance, result, child, table, receiver, target, parameters[9];
    FUNCTION_MARKER(0x8001CA68, "SLUS_003.73");
    player = r_u32(0x800FF5A0);
    x = (uint32)((sint32)(r_u32(object + 24) - r_u32(player + 4)) >> 12);
    if ((sint32)x < 0)
        x = (uint32)0 - x;
    z = (uint32)((sint32)(r_u32(object + 32) - r_u32(player + 12)) >> 12);
    if ((sint32)z < 0)
        z = (uint32)0 - z;
    distance = (sint32)x < (sint32)z ? z + (uint32)((sint32)x / 2) : x + (uint32)((sint32)z / 2);
    result = (sint32)distance < (sint32)r_u32(object + 92);
    if (result)
    {
        child = r_u32(object + 96);
        if (!child)
        {
            w_u32(0x800FF3AC, 0);
            child = AllocateEffectStorage(176);
            if (child)
            {
                parameters[0] = r_u32(object + 88);
                parameters[1] = r_u8(object + 80);
                parameters[2] = r_u8(object + 81);
                parameters[3] = r_u8(object + 82);
                parameters[4] = r_u8(object + 83);
                parameters[5] = r_u8(object + 84);
                parameters[6] = r_u8(object + 85);
                parameters[7] = r_u32(object + 72);
                parameters[8] = r_u32(object + 76);
                void *dram_addr = AllocateNativeObjectStorage(child, sizeof(OscillatingRing));
                new (dram_addr) OscillatingRing((const uint32 *)psx_addr(object + 24, 12), parameters[7], parameters[8], parameters[0], parameters[1], parameters[2], parameters[3], parameters[4], parameters[5], parameters[6]);
            }
            w_u32(object + 96, child);
            w_u32(0x800FF3AC, 1);
            // TODO The original stores through zero after child allocation failure
            w_u8(child + 66, 1);
        }
        result = (distance * 4 + distance) * 2;
        child = r_u32(object + 96);
        result = (uint32)((sint32)result / 16);
        w_u16(child + 96, result);
    }
    else
    {
        child = r_u32(object + 96);
        if (child)
        {
            table = r_u32(child + 68);
            receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
            target = r_u32(table + 12);
            result = apocalypse_object_cleanup(target, receiver, 3);
            w_u32(object + 96, 0);
        }
    }
    return result;
}

extern "C" uint32 apocalypse_trigger_object_create(uint32 object, const uint32 position[3], uint32 index, uint32 range, uint32 field72, uint32 field76, uint32 field88, uint32 red0, uint32 green0, uint32 blue0, uint32 red1, uint32 green1, uint32 blue1)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ProximityRingEmitter));
    new (dram_addr) ProximityRingEmitter(position, index, range, field72, field76, field88, red0, green0, blue0, red1, green1, blue1);
    return object;
}

extern "C" uint32 sub_8001C8E8(uint32 object, uint32 position, uint32 index, uint32 range, uint32 field72, uint32 field76, uint32 field88, uint32 red0, uint32 green0, uint32 blue0, uint32 red1, uint32 green1, uint32 blue1)
{
    FUNCTION_MARKER(0x8001C8E8, "SLUS_003.73");
    return apocalypse_trigger_object_create(object, (const uint32 *)psx_addr(position, 12), index, range, field72, field76, field88, red0, green0, blue0, red1, green1, blue1);
}

extern "C" uint32 sub_8001C9EC(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001C9EC, "SLUS_003.73");
    return ProximityRingEmitter::Attach(object).ProximityRingEmitter::Destroy(reason);
}
extern "C" uint32 sub_8001CA68(uint32 object)
{
    FUNCTION_MARKER(0x8001CA68, "SLUS_003.73");
    return ProximityRingEmitter::Attach(object).ProximityRingEmitter::Update();
}

extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "beam_barrier.h"
#include "game_object_allocation.h"
#include "object_collision_events.h"
#include "particle_settings.h"
#include "effect_lifecycle.h"
#include "fading_particle.h"
#include "player.h"
#include "beam_endpoint.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

// Native input bridge preserves the original sequential record-word loads
static uint32 ReadBeamBarrierWord(const uint32 *data, uint32 axis)
{
    uint32 address = GuestMemoryAddress(data), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)data + axis * 4, 4);
    return value;
}

void BeamBarrier::SetMode(uint32 mode)
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8001A144, "SLUS_003.73");
    w_u32(object + 324, mode);
}

uint32 BeamBarrier::Initialize(uint32 mode)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 flags, secondary_flags, left, right, upper, value, effect, green, blue;
    uint32 difference[3];
    sint32 count;
    FUNCTION_MARKER(0x80019ED0, "SLUS_003.73");
    flags = r_u16(object);
    secondary_flags = r_u16(object + 78);
    w_u16(object, flags | 1);
    w_u16(object + 78, secondary_flags & 0xFFEF);
    AddToList((uint32 *)psx_addr(0x800FF4E8, 4));
    left = r_u32(object + 4);
    right = r_u32(object + 296);
    upper = right;
    if ((sint32)left < (sint32)right)
        w_u32(object + 308, left);
    else
    {
        upper = r_u32(object + 4);
        w_u32(object + 308, right);
    }
    w_u32(object + 312, upper);
    left = r_u32(object + 12);
    right = r_u32(object + 304);
    upper = right;
    if ((sint32)left < (sint32)right)
        w_u32(object + 316, left);
    else
    {
        upper = r_u32(object + 12);
        w_u32(object + 316, right);
    }
    w_u32(object + 320, upper);
    value = r_u32(object + 308);
    w_u32(object + 308, value - 0x100000);
    left = r_u32(object + 312);
    right = r_u32(object + 320);
    w_u32(object + 312, left + 0x100000);
    value = r_u32(object + 316);
    w_u32(object + 320, right + 0x100000);
    w_u32(object + 316, value - 0x100000);
    xport_draft_host_sub_8006C3AC_p1(difference, object + 296, object + 4);
    count = (sint32)xport_draft_host_sub_8006BF04_p1(difference) / 512;
    if (count < 4)
        count = 4;
    green = mode ? 32 : 128;
    blue = mode ? 128 : 32;
    effect = sub_80032DC0(164);
    if (effect)
    {
        void *dram_addr = AllocateNativeObjectStorage(effect, sizeof(EndpointBeam));
        new (dram_addr) EndpointBeam(32, green, blue, (uint32)count, 700, 32, 48, 440, 110, 800, 147);
    }
    w_u32(object + 328, effect);
    EndpointBeam::Attach(effect).SetEndpoints((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 296, 12));
    w_u8(r_u32(object + 328) + 66, 1);
    w_u16(object + 58, 404);
    SetMode(mode);
    return 404;
}

BeamBarrier::BeamBarrier(const uint32 *data, uint32 index, uint32 mode) : VisualObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 value, cursor;
    FUNCTION_MARKER(0x80019DA0, "SLUS_003.73");
    w_u32(object + 68, 0x800A0740);
    value = ReadBeamBarrierWord(data, 0);
    w_u32(object + 4, value << 12);
    value = ReadBeamBarrierWord(data, 1);
    w_u32(object + 8, value << 12);
    value = ReadBeamBarrierWord(data, 2);
    w_u16(object + 214, index);
    w_u32(object + 12, value << 12);
    cursor = sub_80066088(index);
    sub_8006613C(object + 296, r_u16(cursor + 2));
    Initialize(mode);
}

BeamBarrier::BeamBarrier(const uint32 *position, const uint32 *target) : VisualObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z;
    FUNCTION_MARKER(0x80019E48, "SLUS_003.73");
    w_u32(object + 68, 0x800A0740);
    x = ReadBeamBarrierWord(position, 0);
    y = ReadBeamBarrierWord(position, 1);
    z = ReadBeamBarrierWord(position, 2);
    w_u32(object + 4, x);
    w_u32(object + 8, y);
    w_u32(object + 12, z);
    x = ReadBeamBarrierWord(target, 0);
    y = ReadBeamBarrierWord(target, 1);
    z = ReadBeamBarrierWord(target, 2);
    w_u32(object + 296, x);
    w_u32(object + 300, y);
    w_u32(object + 304, z);
    Initialize(1);
}

uint32 BeamBarrier::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, receiver, target;
    FUNCTION_MARKER(0x8001A0B4, "SLUS_003.73");
    w_u32(object + 68, 0x800A0740);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    child = r_u32(object + 328);
    if (child)
    {
        table = r_u32(child + 68);
        receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, receiver, 3);
    }
    VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 BeamBarrier::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 player, coordinate, bound, result, table, receiver, target, value, index;
    uint32 collision[4];
    uint16 angles[3];
    FUNCTION_MARKER(0x8001A14C, "SLUS_003.73");
    // The current virtual caller supplies the callback target in V0
    result = 0x8001A14C;
    player = r_u32(0x800FF5A0);
    if (!player)
        return result;
    coordinate = r_u32(player + 4);
    bound = r_u32(object + 308);
    result = (sint32)bound < (sint32)coordinate;
    if (!result)
        return result;
    bound = r_u32(object + 312);
    result = (sint32)coordinate < (sint32)bound;
    if (!result)
        return result;
    coordinate = r_u32(player + 12);
    bound = r_u32(object + 316);
    result = (sint32)bound < (sint32)coordinate;
    if (!result)
        return result;
    bound = r_u32(object + 320);
    result = (sint32)coordinate < (sint32)bound;
    if (!result)
        return result;
    result = IntersectObjectSpheres((const uint32 *)psx_addr(object + 4, 12), (const uint32 *)psx_addr(object + 296, 12), collision, player, 0);
    player = r_u32(0x800FF5A0);
    if (result != player)
        return result;
    table = r_u32(result + 68);
    receiver = result + (uint32)(sint32)(sint16)r_u16(table + 48);
    target = r_u32(table + 52);
    apocalypse_object_virtual52(target, receiver, 10, 0x800A71CC, 0);
    result = r_u32(object + 324);
    if (!result)
        return result;
    Player::Attach(r_u32(0x800FF5A0)).Player::ApplyBeamContactImpulse(object);
    SetParticleDirection((const sint16 *)psx_addr(0x800FF5E4, 6));
    angles[0] = 512;
    angles[1] = 4096;
    angles[2] = 0;
    SetParticleSpread((const sint16 *)angles);
    SetParticleSize(2);
    SetParticleColor(32, 32, 128);
    SetParticleColorStep(1, 1, 4);
    w_u32(0x800FF3AC, 0);
    for (index = 0; index < 32; ++index)
    {
        value = AllocateEffectStorage(88);
        if (value)
        {
            uint32 position = r_u32(0x800FF5A0) + 4;
            void *dram_addr = AllocateNativeObjectStorage(value, sizeof(FadingParticle));
            new (dram_addr) FadingParticle((const uint32 *)psx_addr(position, 12), 16, 8192, 32);
        }
    }
    w_u32(0x800FF3AC, 1);
    return 1;
}

extern "C" uint32 sub_80019DA0(uint32 object, uint32 data, uint32 index, uint32 mode)
{
    FUNCTION_MARKER(0x80019DA0, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(BeamBarrier));
    new (storage) BeamBarrier((const uint32 *)psx_addr(data, 12), index, mode);
    return object;
}

extern "C" uint32 sub_80019E48(uint32 object, uint32 position, uint32 target)
{
    FUNCTION_MARKER(0x80019E48, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(BeamBarrier));
    new (dram_addr) BeamBarrier((const uint32 *)psx_addr(position, 12), (const uint32 *)psx_addr(target, 12));
    return object;
}

extern "C" uint32 sub_80019ED0(uint32 object, uint32 mode)
{
    FUNCTION_MARKER(0x80019ED0, "SLUS_003.73");
    return BeamBarrier::Attach(object).Initialize(mode);
}

extern "C" uint32 sub_8001A0B4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001A0B4, "SLUS_003.73");
    return BeamBarrier::Attach(object).BeamBarrier::Destroy(reason);
}

extern "C" void sub_8001A144(uint32 object, uint32 mode)
{
    FUNCTION_MARKER(0x8001A144, "SLUS_003.73");
    BeamBarrier::Attach(object).SetMode(mode);
}

extern "C" uint32 sub_8001A14C(uint32 object)
{
    FUNCTION_MARKER(0x8001A14C, "SLUS_003.73");
    return BeamBarrier::Attach(object).BeamBarrier::Update();
}

BeamBarrier &BeamBarrier::Attach(uint32 object)
{
    return *static_cast<BeamBarrier *>(FindNativeObjectStorage(object));
}

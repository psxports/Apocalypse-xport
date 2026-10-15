extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "ring_burst_effect.h"
#include "native_object_storage.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "effect_color.h"
#include <new>
#include <string.h>

RingBurstEffect::RingBurstEffect(const uint32 *position, uint32 segments, uint32 red, uint32 green, uint32 blue, uint32 center_fade, uint32 ring_red, uint32 ring_green, uint32 ring_blue, uint32 ring_fade, uint32 extent, uint32 angle_step, uint32 regenerate, uint32 odd_min, uint32 odd_range, uint32 even_min, uint32 even_range, uint32 decay, uint32 lifetime) : RingEffect(segments, 1)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8001CF9C, "SLUS_003.73");
    w_u32(object + 68, 0x800A1154);
    uint32 address = GuestMemoryAddress(position);
    uint32 x, y, z;
    if (address)
    {
        x = r_u32(address);
        y = r_u32(address + 4);
        z = r_u32(address + 8);
    }
    else
    {
        memcpy(&x, position, 4);
        memcpy(&y, (const uint8 *)position + 4, 4);
        memcpy(&z, (const uint8 *)position + 8, 4);
    }
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    SetCenterColor(red & 255, green & 255, blue & 255);
    w_u16(object + 104, center_fade);
    SetRingColors(ring_red & 255, ring_green & 255, ring_blue & 255);
    w_u16(object + 106, ring_fade);
    SetRowColors(0, 0, 0, 0);
    SetRowExtent(0, extent);
    w_u16(object + 108, angle_step);
    w_u8(object + 120, regenerate);
    w_u16(object + 110, odd_min);
    w_u16(object + 112, odd_range);
    w_u16(object + 114, even_min);
    w_u16(object + 116, even_range);
    w_u16(object + 118, decay);
    w_u16(object + 10, lifetime);
    RegenerateRadii();
}

RingBurstEffect &RingBurstEffect::Attach(uint32 object)
{
    return *static_cast<RingBurstEffect *>(FindNativeObjectStorage(object));
}

uint32 RingBurstEffect::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8001FCD4, "SLUS_003.73");
    w_u32(object + 68, 0x800A1154);
    uint32 result = RingEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 RingBurstEffect::RegenerateRadii()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index = 0, offset = 0, count, field, random, buffer, value, current, decrement;
    sint32 signed_current, signed_decrement;
    FUNCTION_MARKER(0x8001D114, "SLUS_003.73");
    count = r_u32(object + 80);
    if (count)
    {
        do
        {
            field = (index & 1) ? 112 : 116;
            random = RandomRange((uint32)(sint32)(sint16)r_u16(object + field));
            buffer = r_u32(object + 72);
            value = (uint32)(sint32)(sint16)r_u16(object + field - 2);
            w_u32(buffer + offset, value + random);
            count = r_u32(object + 80);
            ++index;
            offset += 8;
        } while (index < count);
    }
    for (field = 110; field <= 116; field += 2)
    {
        signed_current = (sint16)r_u16(object + field);
        signed_decrement = (sint16)r_u16(object + 118);
        current = r_u16(object + field);
        decrement = r_u16(object + 118);
        if (signed_current >= signed_decrement)
            w_u16(object + field, current - decrement);
    }
    random = RandomRange(1024);
    w_u16(object + 96, random);
    return random;
}

void RingBurstEffect::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index = 0, count, buffer, amount, lifetime, age;
    FUNCTION_MARKER(0x8001D240, "SLUS_003.73");
    count = r_u32(object + 80);
    if (count)
    {
        do
        {
            buffer = r_u32(object + 72);
            amount = (uint32)(sint32)(sint16)r_u16(object + 106);
            FadePackedColor((uint32 *)psx_addr(buffer + index * 8 + 4, 4), (sint32)amount);
            amount = (uint32)(sint32)(sint16)r_u16(object + 104);
            FadePackedColor((uint32 *)psx_addr(object + 88, 4), (sint32)amount);
            count = r_u32(object + 80);
            ++index;
        } while (index < count);
    }
    if (r_u8(object + 120))
        RegenerateRadii();
    lifetime = r_u16(object + 10);
    if (lifetime)
    {
        age = r_u16(object + 8);
        lifetime = r_u16(object + 10);
        age = (age + 1) & 65535;
        w_u16(object + 8, age);
        if ((sint32)lifetime < (sint16)age)
            Finish();
    }
    else if (!(r_u32(object + 88) & 0xFFFFFF))
        Finish();
}

uint32 SpawnExplosionRing(const uint32 *position, uint32 size, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 mode, uint32 extent)
{
    FUNCTION_MARKER(0x8001D320, "SLUS_003.73");
    uint32 object = r_u32(0x800FF738);
    if (object)
        return object;
    object = AllocateEffectStorage(124);
    if (!object)
        return object;
    uint32 half = (uint32)((sint32)size / 2);
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(RingBurstEffect));
    if (mode)
        new (dram_addr) RingBurstEffect(position, segments, red, green, blue, 2, red, green, blue, 20, extent, 0, 0, size, size, half, half, 0, 0);
    else
        new (dram_addr) RingBurstEffect(position, segments, red, green, blue, 2, red, green, blue, 20, extent, 0, 0, size, 0, size, 0, 0, 0);
    return object;
}

extern "C" uint32 sub_8001CF9C(uint32 object, uint32 position, uint32 segments, uint32 red, uint32 green, uint32 blue, uint32 center_fade, uint32 ring_red, uint32 ring_green, uint32 ring_blue, uint32 ring_fade, uint32 extent, uint32 angle_step, uint32 regenerate, uint32 odd_min, uint32 odd_range, uint32 even_min, uint32 even_range, uint32 decay, uint32 lifetime)
{
    FUNCTION_MARKER(0x8001CF9C, "SLUS_003.73");
    return xport_draft_host_sub_8001CF9C_p2(object, psx_addr(position, 12), segments, red, green, blue, center_fade, ring_red, ring_green, ring_blue, ring_fade, extent, angle_step, regenerate, odd_min, odd_range, even_min, even_range, decay, lifetime);
}

extern "C" uint32 xport_draft_host_sub_8001CF9C_p2(uint32 object, const void *position, uint32 segments, uint32 red, uint32 green, uint32 blue, uint32 center_fade, uint32 ring_red, uint32 ring_green, uint32 ring_blue, uint32 ring_fade, uint32 extent, uint32 angle_step, uint32 regenerate, uint32 odd_min, uint32 odd_range, uint32 even_min, uint32 even_range, uint32 decay, uint32 lifetime)
{
    FUNCTION_MARKER(0x8001CF9C, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(RingBurstEffect));
    new (dram_addr) RingBurstEffect((const uint32 *)position, segments, red, green, blue, center_fade, ring_red, ring_green, ring_blue, ring_fade, extent, angle_step, regenerate, odd_min, odd_range, even_min, even_range, decay, lifetime);
    return object;
}

extern "C" uint32 sub_8001D114(uint32 object)
{
    FUNCTION_MARKER(0x8001D114, "SLUS_003.73");
    return RingBurstEffect::Attach(object).RegenerateRadii();
}

extern "C" uint32 sub_8001FCD4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001FCD4, "SLUS_003.73");
    return RingBurstEffect::Attach(object).RingBurstEffect::Destroy(reason);
}

// Native update callbacks discard the original return register
extern "C" void sub_8001D240(uint32 object)
{
    FUNCTION_MARKER(0x8001D240, "SLUS_003.73");
    RingBurstEffect::Attach(object).RingBurstEffect::Update();
}

extern "C" uint32 sub_8001D320(uint32 position, uint32 size, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 mode, uint32 extent)
{
    FUNCTION_MARKER(0x8001D320, "SLUS_003.73");
    return SpawnExplosionRing((const uint32 *)psx_addr(position, 12), size, red, green, blue, segments, mode, extent);
}

extern "C" uint32 apocalypse_explosion_sprite_native(const void *position, uint32 size, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 mode, uint32 extent)
{
    return SpawnExplosionRing((const uint32 *)position, size, red, green, blue, segments, mode, extent);
}

extern "C" uint32 xport_draft_host_sub_8001D320_p1(const void *position, uint32 size, uint32 red, uint32 green, uint32 blue, uint32 segments, uint32 mode, uint32 extent)
{
    FUNCTION_MARKER(0x8001D320, "SLUS_003.73");
    return SpawnExplosionRing((const uint32 *)position, size, red, green, blue, segments, mode, extent);
}

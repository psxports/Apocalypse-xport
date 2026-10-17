extern "C"
{
#include "draft_first_signatures.h"
}
#include "oscillating_ring.h"
#include "native_object_storage.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include <new>

OscillatingRing::OscillatingRing(const uint32 *position, uint32 radius, uint32 extent, uint32 amplitude, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue) : RingEffect(position, radius, extent, red & 255, green & 255, blue & 255, tail_red & 255, tail_green & 255, tail_blue & 255)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 product, count, index = 0;
    FUNCTION_MARKER(0x80034C0C, "SLUS_003.73");
    product = radius * amplitude;
    w_u32(object + 68, 0x800A1CF0);
    w_u32(object + 172, radius);
    count = r_u32(object + 80);
    w_u32(object + 168, (uint32)((sint32)product / 256));
    if (count)
    {
        do
        {
            w_u32(object + 136 + index * 4, RandomRange(4096));
            w_u32(object + 104 + index * 4, RandomRange(50) + 50);
            count = r_u32(object + 80);
            ++index;
        } while (index < count);
    }
}

OscillatingRing &OscillatingRing::Attach(uint32 object)
{
    return *static_cast<OscillatingRing *>(FindNativeObjectStorage(object));
}

uint32 OscillatingRing::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8003A0C4, "SLUS_003.73");
    w_u32(object + 68, 0x800A1CF0);
    uint32 result = RingEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

void OscillatingRing::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, index = 0, phase, speed, sine, amplitude, product, buffer, output, radius;
    FUNCTION_MARKER(0x80034CF8, "SLUS_003.73");
    count = r_u32(object + 80);
    if (count)
    {
        do
        {
            phase = r_u32(object + 136 + index * 4);
            speed = r_u32(object + 104 + index * 4);
            phase += speed;
            w_u32(object + 136 + index * 4, phase);
            sine = (uint32)rsin(phase & 4095);
            amplitude = r_u32(object + 168);
            product = amplitude * sine;
            buffer = r_u32(object + 72);
            output = buffer + index * 8;
            radius = r_u32(object + 172);
            w_u32(output, radius + (uint32)((sint32)product / 4096));
            count = r_u32(object + 80);
            ++index;
        } while (index < count);
    }
}

extern "C" uint32 sub_80034C0C(uint32 object, uint32 position, uint32 radius, uint32 extent, uint32 amplitude, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue)
{
    FUNCTION_MARKER(0x80034C0C, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(OscillatingRing));
    new (dram_addr) OscillatingRing((const uint32 *)psx_addr(position, 12), radius, extent, amplitude, red, green, blue, tail_red, tail_green, tail_blue);
    return object;
}

extern "C" uint32 sub_8003A0C4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8003A0C4, "SLUS_003.73");
    return OscillatingRing::Attach(object).OscillatingRing::Destroy(reason);
}

extern "C" uint32 sub_80034CF8(uint32 object)
{
    FUNCTION_MARKER(0x80034CF8, "SLUS_003.73");
    OscillatingRing::Attach(object).OscillatingRing::Update();
    return 0;
}

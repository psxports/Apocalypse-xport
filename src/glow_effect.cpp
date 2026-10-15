extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "glow_effect.h"
#include "native_object_storage.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include <new>

static uint32 DivideGlowScalar(uint32 value, uint32 divisor)
{
    if (!divisor)
        return (sint32)value < 0 ? 1 : (uint32)-1;
    if (value == 0x80000000 && divisor == (uint32)-1)
        return value;
    return (uint32)((sint32)value / (sint32)divisor);
}

GlowEffect::GlowEffect(const uint32 *position, uint32 red, uint32 green, uint32 blue, uint32 growth, uint32 fade, uint32 size, uint32 segments) : RingEffect(segments, 2)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 r, g, b;
    FUNCTION_MARKER(0x8001E8B0, "SLUS_003.73");
    w_u32(object + 68, 0x800A113C);
    SetPosition(position);
    w_u8(object + 104, red);
    w_u8(object + 105, green);
    w_u8(object + 106, blue);
    w_u32(object + 108, fade);
    w_u32(object + 112, 0);
    w_u32(object + 116, growth);
    w_u8(object + 94, 1);
    SetCenterColor(0, 0, 0);
    SetRingColors(0, 0, 0);
    SetRadii(0);
    SetRowExtent(0, size);
    SetRowExtent(1, size);
    r = r_u8(object + 104);
    g = r_u8(object + 105);
    b = r_u8(object + 106);
    SetRowColors(0, r, g, b);
    SetRowColors(1, 0, 0, 0);
    w_u16(object + 96, RandomRange(1024));
}

GlowEffect &GlowEffect::Attach(uint32 object)
{
    return *static_cast<GlowEffect *>(FindNativeObjectStorage(object));
}

uint32 GlowEffect::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8001EA0C, "SLUS_003.73");
    w_u32(object + 68, 0x800A113C);
    uint32 result = RingEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

void GlowEffect::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 radius, growth, color, fade, axis, r, g, b;
    FUNCTION_MARKER(0x8001EA60, "SLUS_003.73");
    radius = r_u32(object + 112);
    growth = r_u32(object + 116);
    color = r_u8(object + 104);
    fade = r_u32(object + 108);
    w_u32(object + 112, radius + growth);
    if ((sint32)color < (sint32)fade)
        w_u8(object + 104, 0);
    else
        w_u8(object + 104, color - r_u8(object + 108));
    for (axis = 1; axis < 3; ++axis)
    {
        color = r_u8(object + 104 + axis);
        fade = r_u32(object + 108);
        if ((sint32)color < (sint32)fade)
            w_u8(object + 104 + axis, 0);
        else
            w_u8(object + 104 + axis, color - r_u8(object + 108));
    }
    r = r_u8(object + 104);
    g = r_u8(object + 105);
    b = r_u8(object + 106);
    if (!(r | g | b))
        Finish();
    SetRadii(r_u32(object + 112));
    r = r_u8(object + 104);
    g = r_u8(object + 105);
    b = r_u8(object + 106);
    SetRowColors(0, r, g, b);
}

ExpandingGlow::ExpandingGlow(const uint32 *position, uint32 red, uint32 green, uint32 blue, uint32 size, uint32 duration) : GlowEffect(position, red & 255, green & 255, blue & 255, 0, 0, 0, 6)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 step, maximum;
    FUNCTION_MARKER(0x8001EB64, "SLUS_003.73");
    step = DivideGlowScalar(size, duration);
    w_u32(object + 68, 0x800A1124);
    w_u32(object + 120, step);
    maximum = (red & 255) < (blue & 255) ? (blue & 255) : (red & 255);
    if (maximum < (green & 255))
        maximum = green & 255;
    w_u32(object + 108, DivideGlowScalar(maximum, duration));
}

ExpandingGlow &ExpandingGlow::Attach(uint32 object)
{
    return *static_cast<ExpandingGlow *>(FindNativeObjectStorage(object));
}

uint32 ExpandingGlow::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8001EC58, "SLUS_003.73");
    w_u32(object + 68, 0x800A1124);
    uint32 result = GlowEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

void ExpandingGlow::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 buffer, extent, step;
    FUNCTION_MARKER(0x8001ECAC, "SLUS_003.73");
    GlowEffect::Update();
    buffer = r_u32(object + 76);
    extent = r_u32(buffer);
    step = r_u32(object + 120);
    SetRowExtent(0, extent + step);
    SetRowExtent(1, 50);
}

extern "C" uint32 sub_8001E8B0(uint32 object, uint32 position, uint32 red, uint32 green, uint32 blue, uint32 growth, uint32 fade, uint32 size, uint32 segments)
{
    FUNCTION_MARKER(0x8001E8B0, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(GlowEffect));
    new (dram_addr) GlowEffect((const uint32 *)psx_addr(position, 12), red, green, blue, growth, fade, size, segments);
    return object;
}

extern "C" uint32 apocalypse_expanding_glow_native(uint32 object, const void *position, uint32 red, uint32 green, uint32 blue, uint32 size, uint32 duration)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ExpandingGlow));
    new (dram_addr) ExpandingGlow((const uint32 *)position, red, green, blue, size, duration);
    return object;
}

extern "C" uint32 sub_8001EB64(uint32 object, uint32 position, uint32 red, uint32 green, uint32 blue, uint32 size, uint32 duration)
{
    FUNCTION_MARKER(0x8001EB64, "SLUS_003.73");
    return apocalypse_expanding_glow_native(object, psx_addr(position, 12), red, green, blue, size, duration);
}

extern "C" uint32 sub_8001EA0C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001EA0C, "SLUS_003.73");
    return GlowEffect::Attach(object).GlowEffect::Destroy(reason);
}
extern "C" uint32 sub_8001EC58(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001EC58, "SLUS_003.73");
    return ExpandingGlow::Attach(object).ExpandingGlow::Destroy(reason);
}
extern "C" void sub_8001EA60(uint32 object)
{
    FUNCTION_MARKER(0x8001EA60, "SLUS_003.73");
    GlowEffect::Attach(object).GlowEffect::Update();
}
extern "C" void sub_8001ECAC(uint32 object)
{
    FUNCTION_MARKER(0x8001ECAC, "SLUS_003.73");
    ExpandingGlow::Attach(object).ExpandingGlow::Update();
}

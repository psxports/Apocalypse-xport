#include "visual_object.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "effect_splash.h"
#include "expanding_ring.h"
#include "game_random.h"
#include "glow_effect.h"
#include "native_object_storage.h"
#include "vector_arithmetic.h"
#include "effect_lifecycle.h"
#include <new>

static uint32 ReadSplashPosition(uint32 guest, const uint32 *native, uint32 axis)
{
    return native ? native[axis] : r_u32(guest + axis * 4);
}

Droplet::Droplet(const uint32 *position, uint32 angle, uint32 red, uint32 green, uint32 blue)
    : Sprite()
{
    uint32 trig_angle;
    uint32 object = NativeObjectGuestAddress(this);
    uint32 guest_position = GuestMemoryAddress(position);
    const uint32 *native_position = guest_position ? 0 : position;
    uint32 x, y, z, random, coefficient, value, camera;
    FUNCTION_MARKER(0x8001F0D8, "SLUS_003.73");
    w_u32(object + 68, 0x800A10DC);
    x = ReadSplashPosition(guest_position, native_position, 0);
    y = ReadSplashPosition(guest_position, native_position, 1);
    z = ReadSplashPosition(guest_position, native_position, 2);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    w_u32(object + 112, ReadSplashPosition(guest_position, native_position, 1));
    SelectResource((const char *)psx_addr(0x800FF1F4, 1));
    SetFrame(0);
    EnableSemiTransparency();
    w_u8(object + 116, red);
    w_u8(object + 117, green);
    w_u8(object + 118, blue);
    SetColors(red & 255, green & 255, blue & 255);
    w_u16(object + 94, 700);
    AddFlags(8);
    random = RandomRange(50);
    trig_angle = angle & 4095;
    coefficient = (uint32)rsin(trig_angle);
    w_u32(object + 36, coefficient * random);
    random = RandomRange(30);
    w_u32(object + 40, 0 - ((random + 20) << 12));
    random = RandomRange(50);
    coefficient = (uint32)rcos(trig_angle);
    value = coefficient * random;
    w_u32(object + 52, 24576);
    w_u8(object + 60, 3);
    w_u32(object + 44, value);
    w_u8(object + 61, 3);
    w_u8(object + 62, 3);
    camera = visual_lists.cameras;
    value = (uint32)(sint32)(sint16)r_u16(camera + 18);
    value = (angle + 2048 - value) & 4095;
    w_u16(object + 98, value < 2048 ? 137 : (uint32)-137);
    w_u16(object + 96, RandomRange(4096));
}

uint32 CreateSplash(const uint32 *position)
{
    uint32 trig_angle;
    uint32 guest_position = GuestMemoryAddress(position);
    const uint32 *native_position = guest_position ? 0 : position;
    uint32 point[3], effect, index, angle, first, second, x, z;
    sint32 quotient;
    FUNCTION_MARKER(0x8001F52C, "SLUS_003.73");
    point[0] = ReadSplashPosition(guest_position, native_position, 0);
    point[1] = ReadSplashPosition(guest_position, native_position, 1);
    point[2] = ReadSplashPosition(guest_position, native_position, 2);
    effect = AllocateEffectStorage(124);
    if (effect)
    {
        void *dram_addr = AllocateNativeObjectStorage(effect, sizeof(ExpandingGlow));
        new (dram_addr) ExpandingGlow(position, 52, 102, 82, 90, 15);
    }
    // TODO The original writes through zero when glow allocation fails
    w_u32(effect + 100, (uint32)-505);
    w_u16(effect + 96, 0);
    for (index = 0; index < 30; ++index)
    {
        angle = RandomRange(4096);
        trig_angle = angle & 4095;
        first = (uint32)rsin(trig_angle) * 100;
        second = (uint32)rcos(trig_angle) * 100;
        x = ReadSplashPosition(guest_position, native_position, 0);
        quotient = (sint32)first / 4096;
        point[0] = x + ((uint32)quotient << 12);
        z = ReadSplashPosition(guest_position, native_position, 2);
        quotient = (sint32)second / 4096;
        point[2] = z + ((uint32)quotient << 12);
        effect = AllocateEffectStorage(120);
        if (effect)
        {
            void *storage = AllocateNativeObjectStorage(effect, sizeof(Droplet));
            new (storage) Droplet(point, angle, 16, 48, 16);
        }
    }
    return 0;
}





extern "C" uint32 apocalypse_splash_native(const uint32 position[3])
{
    return CreateSplash(position);
}

uint32 Droplet::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8001F280, "SLUS_003.73");
    w_u32(object + 68, 0x800A10DC);
    result = Sprite::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(object);
    return result;
}

uint32 Droplet::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 age, angle, velocity_x, velocity_y, velocity_z, shift_y, shift_x, shift_z;
    uint32 x, y, z, acceleration, red, green, blue, effect, result;
    sint32 point[3];
    FUNCTION_MARKER(0x8001F2D4, "SLUS_003.73");
    angle = r_u16(object + 96);
    acceleration = r_u16(object + 98);
    age = (uint32)(sint32)(sint16)r_u16(object + 8);
    w_u16(object + 96, angle + acceleration);
    if (age)
    {
        x = r_u32(object + 24);
        velocity_x = r_u32(object + 36);
        velocity_y = r_u32(object + 40);
        velocity_z = r_u32(object + 44);
        shift_y = r_u8(object + 61);
        w_u32(object + 24, x + velocity_x);
        y = r_u32(object + 28);
        z = r_u32(object + 32);
        w_u32(object + 28, y + velocity_y);
        velocity_x = r_u32(object + 36);
        acceleration = r_u32(object + 48);
        w_u32(object + 32, z + velocity_z);
        velocity_y = r_u32(object + 40);
        velocity_y += r_u32(object + 52);
        w_u32(object + 36, velocity_x + acceleration);
        velocity_z = r_u32(object + 44);
        acceleration = r_u32(object + 56);
        shift_x = r_u8(object + 60);
        w_u32(object + 40, velocity_y);
        velocity_x = r_u32(object + 36);
        velocity_z += acceleration;
        velocity_y = r_u32(object + 40);
        w_u32(object + 36, velocity_x - (uint32)((sint32)velocity_x >> (shift_x & 31)));
        shift_z = r_u8(object + 62);
        w_u32(object + 44, velocity_z);
        w_u32(object + 40, velocity_y - (uint32)((sint32)velocity_y >> (shift_y & 31)));
        w_u32(object + 44, velocity_z - (uint32)((sint32)velocity_z >> (shift_z & 31)));
    }
    age = (uint32)(sint32)(sint16)r_u16(object + 8);
    red = green = blue = 0;
    if ((sint32)age < 20)
    {
        result = 20 - age;
        red = r_u8(object + 116) * result;
        green = r_u8(object + 117) * result;
        blue = r_u8(object + 118) * result;
        red = (uint32)((sint32)red / 20) & 255;
        green = (uint32)((sint32)green / 20) & 255;
        blue = (uint32)((sint32)blue / 20) & 255;
    }
    SetColors(red, green, blue);
    y = r_u32(object + 28);
    if ((sint32)r_u32(object + 112) < (sint32)y)
    {
        if (!RandomRange(6))
        {
            point[0] = (sint32)r_u32(object + 24);
            point[1] = (sint32)r_u32(object + 28);
            point[2] = (sint32)r_u32(object + 32);
            point[1] = (sint32)r_u32(object + 112);
            effect = AllocateEffectStorage(116);
            if (effect)
            {
                red = r_u8(object + 116);
                green = r_u8(object + 117);
                blue = r_u8(object + 118);
                void *dram_addr = AllocateNativeObjectStorage(effect, sizeof(ExpandingRing));
                new (dram_addr) ExpandingRing(point, 100, red, green, blue, 10, 3);
            }
        }
        Finish();
    }
    if ((sint16)r_u16(object + 8) >= 21)
        Finish();
    result = r_u16(object + 8) + 1;
    w_u16(object + 8, result);
    return result;
}

extern "C" uint32 sub_8001F280(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8001F280, "SLUS_003.73");
    return Droplet::Attach(object).Droplet::Destroy(reason);
}

extern "C" uint32 sub_8001F2D4(uint32 object)
{
    FUNCTION_MARKER(0x8001F2D4, "SLUS_003.73");
    return Droplet::Attach(object).Droplet::Update();
}

// TODO Original constructor boundary is inline; retain only its shared initialization block
ImpactRing::ImpactRing(const uint32 velocity[3]) : MeshRingEffect(12, 0xCCE0187C)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 x = velocity[0], y = velocity[1], z = velocity[2];
    w_u32(object + 68, 0x800A2B88);
    w_u32(object + 36, x);
    w_u32(object + 40, y);
    w_u32(object + 44, z);
}

extern "C" void apocalypse_impact_ring_construct_native(uint32 object, const uint32 velocity[3])
{
    void *storage = AllocateNativeObjectStorage(object, sizeof(ImpactRing));
    new (storage) ImpactRing(velocity);
}

uint32 ImpactRing::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 age, lifetime, first, second, value, color;
    FUNCTION_MARKER(0x8005C47C, "SLUS_003.73");
    age = r_u16(object + 8) + 1;
    lifetime = r_u16(object + 10);
    w_u16(object + 8, age);
    if ((sint32)lifetime < (sint16)age)
        return Finish();
    AddVectors((uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 36, 12));
    first = (uint32)(sint32)(sint16)r_u16(object + 116);
    value = r_u32(object + 92);
    second = (uint32)(sint32)(sint16)r_u16(object + 118);
    color = r_u32(object + 96);
    SetGeometry((const uint32 *)psx_addr(object + 24, 12), value + first, color + second);
    lifetime = r_u16(object + 10);
    age = (uint32)(sint32)(sint16)r_u16(object + 8);
    value = lifetime - age;
    value = (value << 8) - value;
    if (lifetime)
        color = (uint32)((sint32)value / (sint32)lifetime);
    else
        color = (sint32)value < 0 ? 1 : (uint32)-1;
    return SetColors(color, color, (uint32)((sint32)color >> 1));
}

uint32 ImpactRing::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8005C538, "SLUS_003.73");
    result = MeshRingEffect::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(object);
    return result;
}

extern "C" uint32 sub_8005C47C(uint32 object)
{
    FUNCTION_MARKER(0x8005C47C, "SLUS_003.73");
    return ImpactRing::Attach(object).ImpactRing::Update();
}

extern "C" uint32 sub_8005C538(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8005C538, "SLUS_003.73");
    return ImpactRing::Attach(object).ImpactRing::Destroy(reason);
}

Droplet &Droplet::Attach(uint32 object)
{
    return *static_cast<Droplet *>(FindNativeObjectStorage(object));
}

ImpactRing &ImpactRing::Attach(uint32 object)
{
    return *static_cast<ImpactRing *>(FindNativeObjectStorage(object));
}

extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "fading_particle.h"
#include "particle_settings.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>

FadingParticle::FadingParticle(const uint32 *position, uint32 speed, uint32 acceleration, uint32 lifetime_range) : PrimitiveParticle()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 kind, header, mapped, x, y, z, value, emission;
    FUNCTION_MARKER(0x80035124, "SLUS_003.73");
    kind = r_u32(0x800FF424);
    w_u32(object + 68, 0x800A1C90);
    kind &= 15;
    if (kind == 1)
    {
        w_u8(object + 79, 104);
        header = 0x02000000;
    }
    else
    {
        w_u8(object + 79, 96);
        kind = r_u32(0x800FF424);
        header = 0x03000000;
    }
    w_u32(object + 72, header);
    w_u32(object + 80, kind);
    value = r_u8(0x800FF42C);
    w_u8(object + 76, value);
    value = r_u8(0x800FF42D);
    w_u8(object + 77, value);
    value = r_u8(0x800FF42E);
    w_u8(object + 78, value);
    value = r_u8(0x800FF430);
    w_u8(object + 84, value);
    value = r_u8(0x800FF431);
    w_u8(object + 85, value);
    value = r_u8(0x800FF432);
    w_u8(object + 86, value);
    mapped = GuestMemoryAddress(position);
    if (mapped)
    {
        x = r_u32(mapped);
        y = r_u32(mapped + 4);
        z = r_u32(mapped + 8);
    }
    else
    {
        memcpy(&x, position, 4);
        memcpy(&y, position + 1, 4);
        memcpy(&z, position + 2, 4);
    }
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    BuildParticleVelocity((uint32 *)psx_addr(object + 36, 12), speed);
    w_u32(object + 52, acceleration);
    w_u8(object + 60, 3);
    w_u8(object + 61, 3);
    w_u8(object + 62, 3);
    value = RandomRange(lifetime_range);
    emission = r_u32(0x800FF428);
    w_u16(object + 10, value);
    if (emission)
    {
        value = r_u8(object + 79);
        w_u8(object + 79, value | 2);
    }
}

uint32 FadingParticle::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8003525C, "SLUS_003.73");
    w_u32(object + 68, 0x800A1C90);
    result = PrimitiveParticle::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 FadingParticle::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, vx, vy, vz, ax, ay, az, age, lifetime;
    uint32 red, green, blue, fade_x, fade_y, fade_z, result;
    FUNCTION_MARKER(0x800352B0, "SLUS_003.73");
    x = r_u32(object + 24);
    vx = r_u32(object + 36);
    y = r_u32(object + 28);
    vy = r_u32(object + 40);
    x += vx;
    y += vy;
    z = r_u32(object + 32);
    vz = r_u32(object + 44);
    ax = r_u32(object + 48);
    w_u32(object + 24, x);
    z += vz;
    vx += ax;
    vy = r_u32(object + 40);
    ay = r_u32(object + 52);
    vz = r_u32(object + 44);
    az = r_u32(object + 56);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    vy += ay;
    vz += az;
    vx -= (uint32)((sint32)vx >> 3);
    vy -= (uint32)((sint32)vy >> 3);
    vz -= (uint32)((sint32)vz >> 3);
    age = r_u16(object + 8);
    lifetime = r_u16(object + 10);
    w_u32(object + 36, vx);
    w_u32(object + 40, vy);
    w_u32(object + 44, vz);
    age = (age + 1) & 0xFFFF;
    w_u16(object + 8, age);
    if ((sint16)age >= (sint32)lifetime)
        Finish();
    red = r_u8(object + 76);
    fade_x = r_u8(object + 84);
    fade_y = r_u8(object + 85);
    fade_z = r_u8(object + 86);
    w_u8(object + 76, red - fade_x);
    green = r_u8(object + 77);
    blue = r_u8(object + 78);
    result = green - fade_y;
    blue -= fade_z;
    w_u8(object + 77, result);
    w_u8(object + 78, blue);
    return result;
}

FadingParticle &FadingParticle::Attach(uint32 object)
{
    return *static_cast<FadingParticle *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_80035124(uint32 object, uint32 position, uint32 speed, uint32 lifetime, uint32 random_range)
{
    FUNCTION_MARKER(0x80035124, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(FadingParticle));
    new (dram_addr) FadingParticle((const uint32 *)psx_addr(position, 12), speed, lifetime, random_range);
    return object;
}

extern "C" uint32 xport_draft_host_sub_80035124_p2(uint32 object, const void *position, uint32 speed, uint32 lifetime, uint32 random_range)
{
    FUNCTION_MARKER(0x80035124, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(FadingParticle));
    new (dram_addr) FadingParticle((const uint32 *)position, speed, lifetime, random_range);
    return object;
}

extern "C" uint32 sub_8003525C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8003525C, "SLUS_003.73");
    return FadingParticle::Attach(object).FadingParticle::Destroy(reason);
}
extern "C" uint32 sub_800352B0(uint32 object)
{
    FUNCTION_MARKER(0x800352B0, "SLUS_003.73");
    return FadingParticle::Attach(object).FadingParticle::Update();
}

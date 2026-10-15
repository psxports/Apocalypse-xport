extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "effect_launch_explosion.h"
#include "effect_sprite.h"
#include "native_object_storage.h"
#include "audio_session.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include <new>

LaunchExplosionEffect::LaunchExplosionEffect(const uint32 *position) : EffectController()
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_position = GuestMemoryAddress(position);
    uint32 x, y, z;
    FUNCTION_MARKER(0x80035B60, "SLUS_003.73");
    w_u32(object + 68, 0x800A1C18);
    x = guest_position ? r_u32(guest_position) : position[0];
    y = guest_position ? r_u32(guest_position + 4) : position[1];
    z = guest_position ? r_u32(guest_position + 8) : position[2];
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    w_u16(object + 10, 50);
    PlayPositionedSound(1, (const uint32 *)psx_addr(object + 24, 12), 0);
}

uint32 LaunchExplosionEffect::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80035BD0, "SLUS_003.73");
    w_u32(object + 68, 0x800A1C18);
    uint32 result = EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 LaunchExplosionEffect::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, value, position[3], age, lifetime;
    FUNCTION_MARKER(0x80035C24, "SLUS_003.73");
    if (!r_u16(object + 8))
    {
        child = AllocateEffectStorage(120);
        if (child)
        {
            value = RandomRange(500);
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(AnimatedSprite));
            new (dram_addr) AnimatedSprite((const uint32 *)psx_addr(object + 24, 12), (const char *)psx_addr(0x800FF3B0, 1), (value + 2000) & 65535, 1, 1, (uint32)-1);
        }
        AnimatedSprite::Attach(child).SetBrightness(100);
        w_u16(child + 64, 300);
    }
    if ((sint16)r_u16(object + 8) == 5)
    {
        position[0] = r_u32(object + 24);
        position[1] = r_u32(object + 28);
        position[2] = r_u32(object + 32);
        value = RandomRange(100);
        position[1] += (value + 50) << 12;
        child = AllocateEffectStorage(112);
        if (child)
        {
            value = RandomRange(60);
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(MovingSprite));
            new (dram_addr) MovingSprite(position, (const uint32 *)psx_addr(0x800A71CC, 12), (const char *)psx_addr(0x800FF3B8, 1), 3000, value + 30, 0);
        }
        MovingSprite::Attach(child).SetBrightness(100);
        w_u16(child + 64, 200);
        MovingSprite::Attach(child).RandomizeFrame();
    }
    age = r_u16(object + 8);
    lifetime = r_u16(object + 10);
    ++age;
    w_u16(object + 8, age);
    age = (uint32)(sint32)(sint16)age;
    return (sint32)lifetime < (sint32)age ? Finish() : age;
}

LaunchExplosionEffect &LaunchExplosionEffect::Attach(uint32 object)
{
    return *static_cast<LaunchExplosionEffect *>(FindNativeObjectStorage(object));
}

extern "C" uint32 sub_80035B60(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80035B60, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(LaunchExplosionEffect));
    new (dram_addr) LaunchExplosionEffect((const uint32 *)psx_addr(position, 12));
    return object;
}

extern "C" uint32 sub_80035BD0(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80035BD0, "SLUS_003.73");
    return LaunchExplosionEffect::Attach(object).LaunchExplosionEffect::Destroy(reason);
}

extern "C" uint32 sub_80035C24(uint32 object)
{
    FUNCTION_MARKER(0x80035C24, "SLUS_003.73");
    return LaunchExplosionEffect::Attach(object).LaunchExplosionEffect::Update();
}

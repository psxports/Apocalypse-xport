extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "smoke_emitter.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "effect_sprite.h"
#include "native_object_storage.h"
#include <new>

SmokeEmitter::SmokeEmitter(const uint32 *position, uint32 duration, uint32 count, uint32 red, uint32 green, uint32 blue, uint32 speed_min, uint32 speed_range, uint32 size_min, uint32 size_range) : EffectController()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800325B0, "SLUS_003.73");
    w_u32(object + 68, 0x800A1B20);
    SetPosition(position);
    w_u16(object + 10, duration);
    w_u32(object + 72, count);
    w_u8(object + 76, red);
    w_u8(object + 77, green);
    w_u8(object + 78, blue);
    w_u32(object + 80, speed_min);
    w_u32(object + 84, speed_range);
    w_u32(object + 88, size_min);
    w_u32(object + 92, size_range);
}

SmokeEmitter &SmokeEmitter::Attach(uint32 object)
{
    return *static_cast<SmokeEmitter *>(FindNativeObjectStorage(object));
}

uint32 SmokeEmitter::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80032694, "SLUS_003.73");
    w_u32(object + 68, 0x800A1B20);
    uint32 result = EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 SmokeEmitter::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 velocity[3] = {0, 0, 0};
    uint32 index = 0, random, size, child, red, green, blue, age, duration;
    FUNCTION_MARKER(0x800326E8, "SLUS_003.73");
    while ((sint32)index < (sint32)r_u32(object + 72))
    {
        random = RandomRange(r_u32(object + 84));
        velocity[1] = ((uint32)0 - (r_u32(object + 80) + random)) << 12;
        child = AllocateEffectStorage(112);
        if (child)
        {
            random = RandomRange(r_u32(object + 92));
            size = r_u32(object + 88) + random;
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(MovingSprite));
            new (dram_addr) MovingSprite((const uint32 *)psx_addr(object + 24, 12), velocity, (const char *)psx_addr(0x800FF398, 1), size, 0, 10);
        }
        // TODO: Resolve original null allocation before moving-sprite attachment
        w_u16(child + 64, 100);
        random = RandomRange(4096);
        w_u16(child + 96, random);
        red = r_u8(object + 76);
        green = r_u8(object + 77);
        blue = r_u8(object + 78);
        ++index;
        MovingSprite::Attach(child).SetColors(red, green, blue);
    }
    age = r_u16(object + 8);
    duration = r_u16(object + 10);
    age = (age + 1) & 65535;
    w_u16(object + 8, age);
    if ((sint32)(sint16)age == (sint32)duration)
    {
        if (age != 65535)
            return Finish();
        return 65535;
    }
    return (uint32)(sint32)(sint16)age;
}

extern "C" uint32 sub_800325B0(uint32 object, uint32 position, uint32 duration, uint32 count, uint32 red, uint32 green, uint32 blue, uint32 speed_min, uint32 speed_range, uint32 size_min, uint32 size_range)
{
    FUNCTION_MARKER(0x800325B0, "SLUS_003.73");
    return apocalypse_effect_construct_325B0(object, (const sint32 *)psx_addr(position, 12), duration, count, red, green, blue, speed_min, speed_range, size_min, size_range);
}

extern "C" uint32 sub_80032694(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80032694, "SLUS_003.73");
    return SmokeEmitter::Attach(object).SmokeEmitter::Destroy(reason);
}

extern "C" uint32 sub_800326E8(uint32 object)
{
    FUNCTION_MARKER(0x800326E8, "SLUS_003.73");
    return SmokeEmitter::Attach(object).SmokeEmitter::Update();
}

extern "C" uint32 apocalypse_effect_construct_325B0(uint32 object, const sint32 position[3], uint32 duration, uint32 count, uint32 red, uint32 green, uint32 blue, uint32 speed_min, uint32 speed_range, uint32 size_min, uint32 size_range)
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(SmokeEmitter));
    new (dram_addr) SmokeEmitter((const uint32 *)position, duration, count, red, green, blue, speed_min, speed_range, size_min, size_range);
    return object;
}

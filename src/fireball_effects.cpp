#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "fireball_effects.h"
#include "game_random.h"
#include "game_heap.h"
#include "model_attachment.h"
#include "native_object_storage.h"
#include "effect_lifecycle.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Fireball::Fireball(uint32 source, sint16 bone) : Sprite()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 reference[2], random, intensity, y;
    FUNCTION_MARKER(0x8004B428, "SLUS_003.73");
    w_u32(object + 68, 0x800A27A8);
    if (!TryCaptureHeapReference(reference, source))
    {
        // TODO Define the original uninitialized generation for invalid sources
        GameDiagnostic("Unsupported invalid source reference in 8006BE38: %08X\n", source);
        abort();
    }
    w_u32(object + 112, reference[0]);
    w_u32(object + 116, reference[1]);
    w_u16(object + 126, bone);
    SelectResource((const char *)psx_addr(0x800A2748, 1));
    EnableSemiTransparency();
    random = RandomRange(5);
    w_u8(object + 90, random);
    intensity = (RandomRange(64) + 64) & 255;
    w_u8(object + 129, intensity);
    SetColors(128, intensity, intensity);
    random = RandomRange(2);
    w_u8(object + 128, random + 5);
    random = RandomRange(512);
    w_u16(object + 94, random + 1200);
    GetModelBonePosition((uint32 *)psx_addr(object + 24, 12), source, (const sint16 *)psx_addr(object + 120, 8));
    y = r_u32(object + 28);
    w_u32(object + 28, y - 0x40000);
}

uint32 Fireball::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8004B51C, "SLUS_003.73");
    w_u32(object + 68, 0x800A27A8);
    result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 Fireball::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 age, child, source, speed, y, intensity, decrement;
    FUNCTION_MARKER(0x8004B570, "SLUS_003.73");
    age = r_u16(object + 8) + 1;
    w_u16(object + 8, age);
    if ((sint16)age >= 9 && !(r_u32(0x800FF2F0) & 3) && !r_u32(0x800FF738))
    {
        w_u32(0x800FF3AC, 0);
        child = AllocateEffectStorage(116);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(Smoke));
            new (dram_addr) Smoke((const uint32 *)psx_addr(object + 24, 12));
        }
        w_u32(0x800FF3AC, 1);
    }
    source = ResolveHeapReference((uint32 *)psx_addr(object + 112, 8));
    if (!source || (r_u16(source + 78) & 64))
        return Finish();
    speed = r_u8(object + 128);
    y = r_u32(object + 28);
    intensity = r_u8(object + 129);
    w_u32(object + 28, y - (speed << 12));
    if (intensity < 8)
        return Finish();
    decrement = (r_u8(0x800FF2F0) & 1) + 4;
    intensity = (intensity - decrement) & 255;
    w_u8(object + 129, intensity);
    return SetColors(128, intensity, intensity);
}

Smoke::Smoke(const uint32 *position) : Sprite()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 mapped, x, y, z, random;
    FUNCTION_MARKER(0x8004B68C, "SLUS_003.73");
    w_u32(object + 68, 0x800A2790);
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
    SelectResource((const char *)psx_addr(0x800FF4F0, 1));
    EnableSemiTransparency();
    w_u8(object + 109, 64);
    random = RandomRange(3);
    w_u8(object + 90, random);
    SetColors(32, 32, 32);
    w_u16(object + 92, 85);
    random = RandomRange(3);
    w_u8(object + 112, random + 7);
    w_u16(object + 94, 2048);
}

uint32 Smoke::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8004B744, "SLUS_003.73");
    w_u32(object + 68, 0x800A2790);
    result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 Smoke::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 age, speed, y, count;
    sint32 frame;
    FUNCTION_MARKER(0x8004B798, "SLUS_003.73");
    age = r_u16(object + 8);
    w_u16(object + 8, age + 1);
    speed = r_u8(object + 112);
    y = r_u32(object + 28);
    w_u32(object + 28, y - (speed << 12));
    AdvanceFrame();
    frame = (sint8)r_u8(object + 90);
    count = r_u8(object + 89);
    return frame < (sint32)count ? 1 : Finish();
}

Fireball &Fireball::Attach(uint32 object)
{
    return *static_cast<Fireball *>(FindNativeObjectStorage(object));
}

Smoke &Smoke::Attach(uint32 object)
{
    return *static_cast<Smoke *>(FindNativeObjectStorage(object));
}



extern "C" uint32 sub_8004B51C(uint32 object, uint32 flags)
{
    FUNCTION_MARKER(0x8004B51C, "SLUS_003.73");
    return Fireball::Attach(object).Fireball::Destroy(flags);
}
extern "C" uint32 sub_8004B570(uint32 object)
{
    FUNCTION_MARKER(0x8004B570, "SLUS_003.73");
    return Fireball::Attach(object).Fireball::Update();
}



extern "C" uint32 sub_8004B744(uint32 object, uint32 flags)
{
    FUNCTION_MARKER(0x8004B744, "SLUS_003.73");
    return Smoke::Attach(object).Smoke::Destroy(flags);
}
extern "C" uint32 sub_8004B798(uint32 object)
{
    FUNCTION_MARKER(0x8004B798, "SLUS_003.73");
    return Smoke::Attach(object).Smoke::Update();
}

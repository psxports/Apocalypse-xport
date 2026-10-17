extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "panel_particle.h"
#include "native_object_storage.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include <new>
#include <string.h>

PanelParticle::PanelParticle(const uint32 *position) : Sprite()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 mapped, x, y, z, velocity_x, velocity_y, random, floor;
    FUNCTION_MARKER(0x8005C11C, "SLUS_003.73");
    w_u32(object + 68, 0x800A27C0);
    w_u8(object + 60, 4);
    w_u8(object + 61, 4);
    w_u8(object + 62, 4);
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
    velocity_x = (RandomRange(16) - 8) << 12;
    velocity_y = (0xFFFFFFFE - RandomRange(2)) << 12;
    random = RandomRange(16);
    w_u32(object + 36, velocity_x);
    w_u32(object + 40, velocity_y);
    w_u32(object + 44, (random - 8) << 12);
    random = RandomRange(1);
    w_u32(object + 48, 0);
    w_u32(object + 52, (~random) << 12);
    w_u32(object + 56, 0);
    y = r_u32(object + 28);
    floor = r_u32(0x800FF37C);
    if ((sint32)y < (sint32)floor)
    {
        w_u32(object + 28, floor);
        w_u32(object + 40, 0);
        w_u32(object + 52, 0);
    }
    SelectResource(VFX_PANEL);
    SetFrame(2);
    random = RandomRange(1024);
    SetSize((random + 512) & 0xFFFF);
    EnableSemiTransparency();
    random = RandomRange(64);
    w_u8(object + 112, random + 64);
}

uint32 PanelParticle::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8005C264, "SLUS_003.73");
    w_u32(object + 68, 0x800A27C0);
    result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 PanelParticle::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 intensity, y, floor;
    FUNCTION_MARKER(0x8005C2B8, "SLUS_003.73");
    intensity = r_u8(object + 112);
    if (intensity < 5)
        return Finish();
    intensity = (intensity - 4) & 255;
    w_u8(object + 112, intensity);
    SetBrightness(intensity);
    y = r_u32(object + 28);
    floor = r_u32(0x800FF37C);
    if ((sint32)y < (sint32)floor)
    {
        w_u32(object + 28, floor);
        w_u32(object + 40, 0);
        w_u32(object + 52, 0);
    }
    return IntegrateMotion();
}

PanelParticle &PanelParticle::Attach(uint32 object)
{
    return *static_cast<PanelParticle *>(FindNativeObjectStorage(object));
}



extern "C" uint32 apocalypse_particle_construct_5C11C(uint32 object, const uint32 position[3])
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(PanelParticle));
    new (dram_addr) PanelParticle(position);
    return object;
}

extern "C" uint32 sub_8005C264(uint32 object, uint32 flags)
{
    FUNCTION_MARKER(0x8005C264, "SLUS_003.73");
    return PanelParticle::Attach(object).PanelParticle::Destroy(flags);
}
extern "C" uint32 sub_8005C2B8(uint32 object)
{
    FUNCTION_MARKER(0x8005C2B8, "SLUS_003.73");
    return PanelParticle::Attach(object).PanelParticle::Update();
}

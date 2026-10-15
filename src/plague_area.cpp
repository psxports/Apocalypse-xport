#include "plague_area.h"
#include "native_object_storage.h"
#include "effect_lifecycle.h"
#include "vector_distance.h"
#include "object_damage.h"
#include "game_random.h"
#include "panel_particle.h"
#include <new>

// Recovered inline base construction and table assignment at 800464EC..80046504
PlagueArea::PlagueArea() : TexturedQuad()
{
    w_u32(NativeObjectGuestAddress(this) + 68, 0x800A2514);
}

PlagueArea &PlagueArea::Attach(uint32 object)
{
    return *static_cast<PlagueArea *>(FindNativeObjectStorage(object));
}

uint32 PlagueArea::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80047A54, "SLUS_003.73");
    uint32 result = TexturedQuad::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(object);
    return result;
}

void PlagueArea::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 age, player, distance, radius, lifetime, count, random, child;
    uint32 position[3];
    sint16 direction[3];
    FUNCTION_MARKER(0x80047844, "SLUS_003.73");
    age = r_u16(object + 8);
    player = r_u32(0x800FF5A0);
    w_u16(object + 8, age + 1);
    if (player)
    {
        distance = MeasureHorizontalDistance((const uint32 *)psx_addr(object + 156, 12), (const uint32 *)psx_addr(player + 4, 12));
        radius = r_u32(object + 168);
        radius = (uint32)((sint32)(radius << 7) >> 7);
        if (distance < radius)
        {
            player = r_u32(0x800FF5A0);
            if (r_u16(player + 216) & 2)
                ApplyObjectDamage(player, 1, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
        }
    }
    age = (uint32)(sint32)(sint16)r_u16(object + 8);
    if ((sint32)age < 38)
    {
        radius = r_u32(object + 168);
        w_u32(object + 168, radius + 4);
        direction[2] = 0;
        direction[0] = 0;
        direction[1] = -4096;
        radius = r_u32(object + 168);
        uint32 second_scale = radius;
        uint32 first_scale = r_u32(object + 168);
        SetPosition((const uint32 *)psx_addr(object + 156, 12), direction, first_scale, second_scale);
    }
    age = (uint32)(sint32)(sint16)r_u16(object + 8);
    lifetime = r_u16(object + 10);
    if ((sint32)age >= (sint32)lifetime)
    {
        Finish();
        return;
    }
    if ((sint32)age >= (sint32)(lifetime - 30))
    {
        radius = r_u32(object + 168);
        if ((sint32)radius < 9)
        {
            Finish();
            return;
        }
        w_u32(object + 168, radius - 6);
        direction[2] = 0;
        direction[0] = 0;
        direction[1] = -4096;
        radius = r_u32(object + 168);
        uint32 second_scale = radius;
        uint32 first_scale = r_u32(object + 168);
        SetPosition((const uint32 *)psx_addr(object + 156, 12), direction, first_scale, second_scale);
        return;
    }
    count = RandomRange(3);
    while (count)
    {
        position[0] = r_u32(object + 156);
        position[1] = r_u32(object + 160);
        position[2] = r_u32(object + 164);
        random = RandomRange(140);
        position[0] += (random - 70) << 12;
        random = RandomRange(140);
        position[2] += (random - 70) << 12;
        child = AllocateEffectStorage(116);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(PanelParticle));
            new (dram_addr) PanelParticle(position);
        }
        count -= 1;
    }
}

extern "C" uint32 sub_80047A54(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80047A54, "SLUS_003.73");
    return PlagueArea::Attach(object).PlagueArea::Destroy(reason);
}

extern "C" void sub_80047844(uint32 object)
{
    FUNCTION_MARKER(0x80047844, "SLUS_003.73");
    PlagueArea::Attach(object).PlagueArea::Update();
}

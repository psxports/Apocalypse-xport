#include <new>

#include "game_heap.h"
#include "death_fading_ring.h"
#include "effect_lifecycle.h"
#include "ring_effect.h"
#include "mesh_ring_effect.h"
#include "effect_object.h"

void DeathFadingRing::Update()
{
    FUNCTION_MARKER(0x80049E38, "SLUS_003.73");
    ++age;
    if (age >= 32)
    {
        Finish();
        return;
    }
    uint32 r = 240 - age * 6;
    uint32 g = 96 - age * 3;
    radius += 1;
    expansion += 7;
    SetColors(r, g, g);
    SetGeometry(&pos, radius, expansion);
}

uint32 DeathFadingRing::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80049EDC, "SLUS_003.73");
    uint32 result = MeshRingEffect::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

DeathFadingRing::DeathFadingRing(const Vector3 *pos) : MeshRingEffect(12, 0xCCE0187C), age(0)
{
    SetGeometry(pos, 32, 32);
    SetColors(240, 96, 96);
}

void DeathFadingRing::Spawn(const Vector3 *pos)
{
    GameHeapBlock *object = EffectObject::AllocateStorage(116);
    if (object)
        new (object) DeathFadingRing(pos);
}

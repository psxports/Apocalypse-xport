#include <new>

#include "game_heap.h"
#include "death_persistent_ring.h"
#include "effect_lifecycle.h"
#include "ring_effect.h"
#include "mesh_ring_effect.h"
#include "effect_object.h"

void DeathPersistentRing::Update()
{
    FUNCTION_MARKER(0x80049D54, "SLUS_003.73");
    ++age;
    if (age < 30)
    {
        radius += 3;
        expansion += 7;
    }
    else
    {
        if (age < 61)
            return;
        if (age >= 90)
        {
            Finish();
            return;
        }
        radius -= 3;
        expansion -= 7;
    }
    SetGeometry(&pos, radius, expansion);
}

uint32 DeathPersistentRing::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80049DF0, "SLUS_003.73");
    uint32 result = MeshRingEffect::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

DeathPersistentRing::DeathPersistentRing(const Vector3 *pos) : MeshRingEffect(12, 0xCCE0187C), age(0)
{
    SetGeometry(pos, 32, 32);
    SetColors(64, 64, 240);
}

void DeathPersistentRing::Spawn(const Vector3 *pos)
{
    GameHeapBlock *object = EffectObject::AllocateStorage(116);
    if (object)
        new (object) DeathPersistentRing(pos);
}

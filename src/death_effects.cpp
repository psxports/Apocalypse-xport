#include <new>

#include "game_heap.h"
#include "death_effects.h"
#include "effect_lifecycle.h"

// Recovered inline construction at 80049574..800495B0
DeathPersistentRing::DeathPersistentRing(const uint32 *position) : MeshRingEffect(12, 0xCCE0187C), age(0)
{
    SetGeometry(position, 32, 32);
    SetColors(64, 64, 240);
}

// Recovered inline construction at 8004911C..80049158
DeathFadingRing::DeathFadingRing(const uint32 *position) : MeshRingEffect(12, 0xCCE0187C), age(0)
{
    SetGeometry(position, 32, 32);
    SetColors(240, 96, 96);
}

// Native factories extract the original allocation and inline construction
void SpawnDeathPersistentRing(const uint32 *position)
{
    GameHeapBlock *object = AllocateEffectStorage(116);
    if (object)
    {
        new (object) DeathPersistentRing(position);
    }
}

void SpawnDeathFadingRing(const uint32 *position)
{
    GameHeapBlock *object = AllocateEffectStorage(116);
    if (object)
    {
        new (object) DeathFadingRing(position);
    }
}

void DeathPersistentRing::Update()
{
    FUNCTION_MARKER(0x80049D54, "SLUS_003.73");
    ++age;
    if ((sint16)age < 30)
    {
        radius += 3;
        expansion += 7;
    }
    else
    {
        if ((sint16)age < 61)
            return;
        if ((sint16)age >= 90)
        {
            Finish();
            return;
        }
        radius -= 3;
        expansion -= 7;
    }
    SetGeometry(pos.words, radius, expansion);
}

void DeathFadingRing::Update()
{
    FUNCTION_MARKER(0x80049E38, "SLUS_003.73");
    ++age;
    if ((sint16)age >= 32)
    {
        Finish();
        return;
    }
    uint32 tick = (sint32)(sint16)age;
    uint32 red = 240 - tick * 6;
    uint32 green = 96 - tick * 3;
    radius += 1;
    expansion += 7;
    SetColors(red, green, green);
    SetGeometry(pos.words, radius, expansion);
}

uint32 DeathPersistentRing::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80049DF0, "SLUS_003.73");
    uint32 result = MeshRingEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

uint32 DeathFadingRing::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80049EDC, "SLUS_003.73");
    uint32 result = MeshRingEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

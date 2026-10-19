#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "effect_object.h"

#include "game_diagnostics.h"
#include "projectile_trail.h"
#include "ring_effect.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "effect_lifecycle.h"

TrailProjectile::TrailProjectile(const uint32 *position, const uint32 *target, const uint16 *angles, uint32 lifetime, uint32 speed, uint32 scale, uint32 red, uint32 green, uint32 blue, uint32 tail_red, uint32 tail_green, uint32 tail_blue) : EffectController(), age(0), lifetime(lifetime), ring()
{
    FUNCTION_MARKER(0x8004B08C, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        angular_velocity.words[axis] = 0;
        angular_acceleration.words[axis] = 0;
    }
    uint32 x = position[0];
    uint32 y = position[1];
    uint32 z = position[2];
    pos.words[0] = x;
    pos.words[1] = y;
    pos.words[2] = z;
    x = target[0];
    y = target[1];
    z = target[2];
    this->target.words[0] = x;
    this->target.words[1] = y;
    this->target.words[2] = z;
    CopyAngleVector(this->angles.words, angles);
    this->speed = speed;
    GameHeapBlock *allocation = AllocateEffectStorage(104);
    if (!allocation)
    {
        GameDiagnostic("Trail projectile ring allocation failed\n");
        abort();
    }
    ring = new (allocation) RingEffect(4, 1);
    ring->SetCenterColor(red & 255, green & 255, blue & 255);
    ring->SetRingColors(tail_red & 255, tail_green & 255, tail_blue & 255);
    ring->SetRowColors(0, 0, 0, 0);
    ring->SetOddRadii(scale);
    ring->SetEvenRadii(scale);
    ring->SetRowExtent(0, scale << 1);
    ring->SetPosition(pos.words);
    ring->persistent = 1;
}

TrailProjectile::~TrailProjectile()
{
    if (ring)
        ring->Destroy(3);
}

uint32 TrailProjectile::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8004B23C, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

void TrailProjectile::Update()
{
    FUNCTION_MARKER(0x8004B2B8, "SLUS_003.73");
    ShortVector3 desired;
    CalculateDirectionAngles(desired.words, pos.words, target.words);
    UpdateAngleAcceleration(angles, angular_velocity, angular_acceleration, desired, 4);
    BuildDirectionVector(velocity.words, speed, angles.words);
    AddVectors(pos.words, velocity.words);
    if (ring)
        ring->SetPosition(pos.words);
    AddAngleVector(angles.words, angular_velocity.words);
    NormalizeAngles(angles.words);
    angular_velocity.words[0] += angular_acceleration.words[0];
    angular_velocity.words[1] += angular_acceleration.words[1];
    angular_velocity.words[0] -= (sint16)angular_velocity.words[0] >> 2;
    angular_velocity.words[1] -= (sint16)angular_velocity.words[1] >> 2;
    SnapSmallAngles(angular_velocity.words);
    ++age;
    if (lifetime < (sint16)age)
        Finish();
}

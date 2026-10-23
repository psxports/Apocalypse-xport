#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "effect_object.h"
#include "effect_controller.h"
#include "game_diagnostics.h"
#include "trail_projectile.h"
#include "ring_effect.h"
#include "angle_math.h"
#include "global.h"
#include "effect_lifecycle.h"

TrailProjectile::TrailProjectile(const Vector3 *src_pos, const Vector3 *target, const ShortVector3 *angles, uint16 lifetime, uint32 speed, uint32 scale, uint8 r, uint8 g, uint8 b, uint8 tail_r, uint8 tail_g, uint8 tail_b) : EffectController(), age(0), lifetime(lifetime), ring()
{
    FUNCTION_MARKER(0x8004B08C, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        angular_velocity.comp[axis] = 0;
        angular_acceleration.comp[axis] = 0;
    }
    pos = *src_pos;
    this->target = *target;
    this->angles = *angles;
    this->speed = speed;
    GameHeapBlock *allocation = EffectObject::AllocateStorage(104);
    if (!allocation)
    {
        GameDiagnostic(("Trail projectile ring allocation failed\n"));
        abort();
    }
    ring = new (allocation) RingEffect(4, 1);
    ring->SetCenterColor(r, g, b);
    ring->SetRingColors(tail_r, tail_g, tail_b);
    ring->SetRowColors(0, 0, 0, 0);
    ring->SetOddRadii(scale);
    ring->SetEvenRadii(scale);
    ring->SetRowExtent(0, scale << 1);
    ring->SetPos(&pos);
    ring->persistent = true;
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
    return reason & 1 ? ReleaseStorage() : 0;
}

void TrailProjectile::Update()
{
    FUNCTION_MARKER(0x8004B2B8, "SLUS_003.73");
    ShortVector3 desired;
    desired.SetDir(pos, target);
    UpdateAngularAcceleration(angles, angular_velocity, angular_acceleration, desired, 4);
    velocity.SetDir(speed, angles);
    pos.Add(velocity);
    if (ring)
        ring->SetPos(&pos);
    angles.AddAngles(angular_velocity);
    angles.NormalizeAngles();
    angular_velocity.x = (uint16)angular_velocity.x + ((uint16)angular_acceleration.x);
    angular_velocity.y = (uint16)angular_velocity.y + ((uint16)angular_acceleration.y);
    angular_velocity.x = (uint16)angular_velocity.x - (angular_velocity.x >> 2);
    angular_velocity.y = (uint16)angular_velocity.y - (angular_velocity.y >> 2);
    angular_velocity.SnapAngles();
    ++age;
    if (lifetime < (sint16)age)
        Finish();
}

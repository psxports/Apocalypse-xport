#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "model_bank.h"
#include "collision.h"
#include "global.h"
#include "game_diagnostics.h"
#include "projectile_base.h"
#include "sprite_trail.h"
#include "ring_effect.h"
#include "game_object_allocation.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "game_heap.h"
#include "object_damage.h"
#include "launch_explosion_effect.h"
#include "explosion_effects.h"
#include "tracking_projectile.h"
#include "mesh_ring_effect.h"
#include "effect_object.h"
#include "explosion_ring.h"

void ExplosionRing::Update()
{
    FUNCTION_MARKER(0x8002E008, "SLUS_003.73");
    ++age;
    if (lifetime < age)
    {
        Finish();
        return;
    }
    pos.Add(velocity);
    SetGeometry(&pos, radius + radius_step, expansion + width_step);
    sint32 color = DivideSignedWord(255 * (lifetime - age), lifetime);
    SetColors(color, color, color >> 1);
}

uint32 ExplosionRing::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8002E0C4, "SLUS_003.73");
    result = MeshRingEffect::Destroy(0);
    if (reason & 1)
        return ReleaseStorage();
    return result;
}

ExplosionRing::ExplosionRing() : MeshRingEffect(12, 0xCCE0187C), age(0), lifetime(0), radius_step(0), width_step(0)
{
    // Original constructor is inlined in ProjectileBase::Explode
    velocity.x = 0;
    velocity.y = -32768;
    velocity.z = 0;
}

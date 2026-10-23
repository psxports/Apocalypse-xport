#include <new>
#include <string.h>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "effect_object.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "angle_math.h"
#include "ring_burst_effect.h"
#include "collision.h"
#include "object_damage.h"
#include "global.h"
#include "ring_effect.h"
#include "projectile_line.h"
#include "model_projection.h"
#include "animated_sprite.h"
#include "particle_sprite.h"
#include "smoke_emitter.h"
#include "effect_lifecycle.h"

void CreateProjectileImpact(uint32 flags, const Vector3 *pos)
{
    FUNCTION_MARKER(0x8002289C, "SLUS_003.73");
    GameHeapBlock *effect;
    if (flags & 4)
    {
        AnimatedSprite *spr = 0;
        effect = EffectObject::AllocateStorage(120);
        if (effect)
        {
            spr = new (effect) AnimatedSprite(pos, VFX_EXP_FIRE, 744, true, true, -1);
        }
        if (!spr)
        {
            GameDiagnostic(("Projectile impact sprite allocation failed\n"));
            abort();
        }
        spr->frame = RandomRange(spr->frame_count - 1);
        spr->angle = RandomRange(4096);
        spr->depth_bias = 100;
    }
    if (flags & 8)
    {
        for (uint32 idx = 0; idx < 4; ++idx)
        {
            effect = EffectObject::AllocateStorage(112);
            if (effect)
            {
                new (effect) ParticleSprite(pos, 128, 128, 128, 6, 1000, 1, 40, 0x3000, 20);
            }
        }
    }
    if (flags & 16)
    {
        effect = EffectObject::AllocateStorage(96);
        if (effect)
        {
            sint32 count = RandomRange(3) + 1;
            new (effect) SmokeEmitter(pos, 1, count, 128, 128, 128, 8, 5, 700, 700);
        }
    }
}

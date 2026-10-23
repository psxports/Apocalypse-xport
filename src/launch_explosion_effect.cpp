#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "object_initializers.h"
#include "game_diagnostics.h"
#include "launch_explosion_effect.h"
#include "animated_sprite.h"
#include "moving_sprite.h"
#include "audio_session.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "effect_object.h"
#include "effect_controller.h"

LaunchExplosionEffect::LaunchExplosionEffect(const Vector3 *src_pos) : EffectController(), age(0), lifetime(50)
{
    FUNCTION_MARKER(0x80035B60, "SLUS_003.73");
    pos = *src_pos;
    PlayPositionedSound(1, &pos, 0);
}

uint32 LaunchExplosionEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80035BD0, "SLUS_003.73");
    uint32 result = EffectController::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void LaunchExplosionEffect::Update()
{
    GameHeapBlock *child;
    uint32 val;
    Vector3 tmp_pos;
    FUNCTION_MARKER(0x80035C24, "SLUS_003.73");
    if (!age)
    {
        AnimatedSprite *spr = 0;
        child = EffectObject::AllocateStorage(120);
        if (child)
        {
            val = RandomRange(500);
            spr = new (child) AnimatedSprite(&pos, "ExpFire", val + 2000, 1, 1, -1);
        }
        if (!spr)
        {
            GameDiagnostic(("Launch explosion sprite allocation failed\n"));
            abort();
        }
        spr->SetBrightness(100);
        spr->depth_bias = 300;
    }
    if ((sint16)age == 5)
    {
        tmp_pos = pos;
        val = RandomRange(100);
        tmp_pos.y = (uint32)tmp_pos.y + ((val + 50) << 12);
        MovingSprite *spr = 0;
        child = EffectObject::AllocateStorage(112);
        if (child)
        {
            val = RandomRange(60);
            spr = new (child) MovingSprite(&tmp_pos, &zero_vector, "Fire", 3000, val + 30, 0);
        }
        if (!spr)
        {
            GameDiagnostic(("Launch explosion fire allocation failed\n"));
            abort();
        }
        spr->SetBrightness(100);
        spr->depth_bias = 200;
        spr->RandomizeFrame();
    }
    ++age;
    if ((sint16)age > lifetime)
        Finish();
}

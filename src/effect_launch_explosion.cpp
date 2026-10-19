#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "object_initializers.h"
#include "game_diagnostics.h"

#include "effect_launch_explosion.h"
#include "effect_sprite.h"
#include "audio_session.h"
#include "game_random.h"
#include "effect_lifecycle.h"

LaunchExplosionEffect::LaunchExplosionEffect(const uint32 *position) : EffectController(), age(0), lifetime(50)
{
    const uint32 x = position[0];
    const uint32 y = position[1];
    const uint32 z = position[2];
    FUNCTION_MARKER(0x80035B60, "SLUS_003.73");
    pos.words[0] = x;
    pos.words[1] = y;
    pos.words[2] = z;
    PlayPositionedSound(1, pos.words, 0);
}

uint32 LaunchExplosionEffect::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80035BD0, "SLUS_003.73");
    uint32 result = EffectController::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void LaunchExplosionEffect::Update()
{
    GameHeapBlock *child;
    uint32 value;
    Vector3 position;
    FUNCTION_MARKER(0x80035C24, "SLUS_003.73");
    if (!age)
    {
        AnimatedSprite *sprite = 0;
        child = AllocateEffectStorage(120);
        if (child)
        {
            value = RandomRange(500);
            sprite = new (child) AnimatedSprite(pos.words, "ExpFire", (value + 2000) & 65535, 1, 1, (uint32)-1);
        }
        if (!sprite)
        {
            GameDiagnostic("Launch explosion sprite allocation failed\n");
            abort();
        }
        sprite->SetBrightness(100);
        sprite->depth_bias = 300;
    }
    if ((sint16)age == 5)
    {
        position = pos;
        value = RandomRange(100);
        position.words[1] += (value + 50) << 12;
        MovingSprite *sprite = 0;
        child = AllocateEffectStorage(112);
        if (child)
        {
            value = RandomRange(60);
            sprite = new (child) MovingSprite(position.words, zero_vector.words, "Fire", 3000, value + 30, 0);
        }
        if (!sprite)
        {
            GameDiagnostic("Launch explosion fire allocation failed\n");
            abort();
        }
        sprite->SetBrightness(100);
        sprite->depth_bias = 200;
        sprite->RandomizeFrame();
    }
    ++age;
    if ((sint16)age > (sint32)lifetime)
        Finish();
}

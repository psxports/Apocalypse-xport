#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_diagnostics.h"
#include "model_render_view.h"

#include "exp_act_burst.h"
#include "effect_sprite.h"
#include "effect_lifecycle.h"
#include "vector_arithmetic.h"
#include "effect_color.h"
#include "angle_math.h"
#include "game_random.h"

ExpActBurst::ExpActBurst(const uint32 *position, uint32 count, uint32 spread, uint32 jitter_y, uint32 upward_speed, uint32 gravity, uint32 drag_y, uint32 speed, uint32 acceleration, uint32 drag_xz, uint32 angle_speed, uint32 angle_acceleration, uint32 size_step, uint32 color_step, uint32 fade_start, uint32 frame_step) : EffectController(), age(0), count(count), fade_start(fade_start)
{
    Vector3 origin, target, direction, tmp, child_position;
    uint32 pair;
    uint32 side;
    GameHeapBlock *child;
    uint32 scalar;
    uint32 random;
    uint32 idx = 0;
    ShortVector3 angles;
    FUNCTION_MARKER(0x8001D484, "SLUS_003.73");
    if ((sint32)count > 20)
    {
        GameDiagnostic("ExpActBurst exceeds its twenty sprite slots\n");
        abort();
    }
    SetPosition(position);
    origin.words[0] = GetModelRenderView().pos.words[0] << 12;
    origin.words[2] = GetModelRenderView().pos.words[2] << 12;
    origin.words[1] = 0;
    target.words[0] = position[0];
    target.words[2] = position[2];
    target.words[1] = 0;
    CalculateDirectionAngles(angles.words, origin.words, target.words);
    angles.components[1] = (sint16)((uint32)(uint16)angles.components[1] + 1024);
    BuildDirectionVector(direction.words, 1, angles.words);
    uint32 velocity_y = ((uint32)0 - upward_speed) << 12;
    uint32 acceleration_y = (uint32)0 - gravity;
    uint32 vertical_range = jitter_y * 2 + 1;
    for (pair = 0; (sint32)pair < (sint32)count / 2; ++pair)
    {
        for (side = 0; side < 2; ++side)
        {
            scalar = RandomRange(spread);
            MultiplyVector(tmp.words, direction.words, scalar);
            if (!side)
                AddVectors(child_position.words, this->pos.words, tmp.words);
            else
                SubtractVectors(child_position.words, this->pos.words, tmp.words);
            random = RandomRange(vertical_range);
            child_position.words[1] += (random - jitter_y) << 12;
            AnimatedSprite *sprite = 0;
            child = AllocateEffectStorage(120);
            if (child)
            {
                sprite = new (child) AnimatedSprite(child_position.words, VFX_EXP_ACT, 1000, 1, 0, (uint32)-2);
            }
            if (!sprite)
            {
                GameDiagnostic("ExpActBurst sprite allocation failed\n");
                abort();
            }
            sprite->persistent = 1;
            sprites[idx++] = sprite;
            scalar = side ? (uint32)0 - speed : speed;
            MultiplyVector(tmp.words, direction.words, scalar);
            sprite->velocity = tmp;
            sprite->velocity.words[1] = velocity_y;
            scalar = side ? (uint32)0 - acceleration : acceleration;
            MultiplyVector(tmp.words, direction.words, scalar);
            sprite->acceleration = tmp;
            sprite->acceleration.words[1] = acceleration_y;
            sprite->drag[0] = drag_xz;
            sprite->drag[2] = drag_xz;
            sprite->drag[1] = drag_y;
            random = RandomRange(4096);
            sprite->angle = random;
            sprite->angle_speed = side ? (uint32)0 - angle_speed : angle_speed;
            sprite->angle_accel = side ? (uint32)0 - angle_acceleration : angle_acceleration;
            sprite->color_step = color_step;
            sprite->shrink_step = size_step;
            sprite->SetFrameStep((uint32)(sint32)(sint16)frame_step);
            sprite->AddFlags(8);
        }
    }
}

ExpActBurst::~ExpActBurst()
{
    for (uint32 idx = 0; (sint32)idx < (sint32)count; ++idx)
        if (sprites[idx])
            sprites[idx]->Destroy(3);
}

uint32 ExpActBurst::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001D918, "SLUS_003.73");
    for (uint32 idx = 0; (sint32)idx < (sint32)count; ++idx)
    {
        if (sprites[idx])
            sprites[idx]->Destroy(3);
        sprites[idx] = 0;
    }
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

void ExpActBurst::Update()
{
    FUNCTION_MARKER(0x8001D9D0, "SLUS_003.73");
    ObjectBorrow object_borrow;
    ++age;
    if ((sint16)age == 1)
        return;
    bool empty = true;
    for (uint32 idx = 0; (sint32)idx < (sint32)count; ++idx)
    {
        AnimatedSprite *sprite = sprites[idx].Get();
        if (!sprite)
            continue;
        AddVectors(sprite->pos.words, sprite->velocity.words);
        AddVectors(sprite->velocity.words, sprite->acceleration.words);
        DampVector(sprite->velocity.words, sprite->drag);
        sprite->angle += sprite->angle_speed;
        sprite->angle_speed += sprite->angle_accel;
        sprite->size += sprite->shrink_step;
        empty = false;
        if ((sint32)fade_start < (sint16)age)
        {
            FadePackedColor(&sprite->color, sprite->color_step);
            if (!(sprite->color & 0xFFFFFF))
            {
                sprite->Destroy(3);
                sprites[idx] = 0;
            }
        }
    }
    if (empty)
        Finish();
}

#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_diagnostics.h"
#include "model_render_view.h"
#include "exp_act_burst.h"
#include "animated_sprite.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "angle_math.h"
#include "effect_object.h"
#include "effect_controller.h"

ExpActBurst::ExpActBurst(const Vector3 *pos, sint32 count, uint32 spread, uint32 jitter_y, uint32 upward_speed, uint32 gravity, uint8 drag_y, uint32 speed, uint32 acceleration, uint8 drag_xz, uint16 angle_speed, uint16 angle_acceleration, uint16 size_step, sint16 color_step, sint32 fade_start, uint16 frame_step) : EffectController(), age(0), count(count), fade_start(fade_start)
{
    Vector3 origin, target, dir, tmp, child_pos;
    sint32 pair;
    uint32 side;
    GameHeapBlock *child;
    uint32 scalar;
    uint32 random;
    uint32 idx = 0;
    ShortVector3 angles;
    FUNCTION_MARKER(0x8001D484, "SLUS_003.73");
    if (count > 20)
    {
        GameDiagnostic(("ExpActBurst exceeds its twenty sprite slots\n"));
        abort();
    }
    SetPos(pos);
    origin.x = (uint32)GetModelRenderView().pos.x << 12;
    origin.z = (uint32)GetModelRenderView().pos.z << 12;
    origin.y = 0;
    target.x = pos->x;
    target.z = pos->z;
    target.y = 0;
    angles.SetDir(origin, target);
    angles.y += 1024;
    dir.SetDir(1, angles);
    uint32 velocity_y = -(upward_speed << 12);
    uint32 acceleration_y = -gravity;
    uint32 vertical_range = jitter_y * 2 + 1;
    for (pair = 0; pair < count / 2; ++pair)
    {
        for (side = 0; side < 2; ++side)
        {
            scalar = RandomRange(spread);
            tmp.Multiply(dir, scalar);
            if (!side)
                child_pos.Add(this->pos, tmp);
            else
                child_pos.Subtract(this->pos, tmp);
            random = RandomRange(vertical_range);
            child_pos.y = (uint32)child_pos.y + ((random - jitter_y) << 12);
            AnimatedSprite *spr = 0;
            child = EffectObject::AllocateStorage(120);
            if (child)
            {
                spr = new (child) AnimatedSprite(&child_pos, VFX_EXP_ACT, 1000, 1, 0, -2);
            }
            if (!spr)
            {
                GameDiagnostic(("ExpActBurst sprite allocation failed\n"));
                abort();
            }
            spr->persistent = true;
            sprs[idx++] = spr;
            scalar = side ? -speed : speed;
            tmp.Multiply(dir, scalar);
            spr->velocity = tmp;
            spr->velocity.y = velocity_y;
            scalar = side ? -acceleration : acceleration;
            tmp.Multiply(dir, scalar);
            spr->acceleration = tmp;
            spr->acceleration.y = acceleration_y;
            spr->drag[0] = drag_xz;
            spr->drag[2] = drag_xz;
            spr->drag[1] = drag_y;
            random = RandomRange(4096);
            spr->angle = random;
            spr->angle_speed = side ? -angle_speed : angle_speed;
            spr->angle_accel = side ? -angle_acceleration : angle_acceleration;
            spr->color_step = color_step;
            spr->shrink_step = size_step;
            spr->SetFrameStep(frame_step);
            spr->AddFlags(8);
        }
    }
}

ExpActBurst::~ExpActBurst()
{
    for (sint32 idx = 0; idx < count; ++idx)
        if (sprs[idx])
            sprs[idx]->Destroy(3);
}

uint32 ExpActBurst::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001D918, "SLUS_003.73");
    for (sint32 idx = 0; idx < count; ++idx)
    {
        if (sprs[idx])
            sprs[idx]->Destroy(3);
        sprs[idx] = 0;
    }
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

void ExpActBurst::Update()
{
    FUNCTION_MARKER(0x8001D9D0, "SLUS_003.73");
    ObjectBorrow object_borrow;
    ++age;
    if (age == 1)
        return;
    bool empty = true;
    for (sint32 idx = 0; idx < count; ++idx)
    {
        AnimatedSprite *spr = sprs[idx].Get();
        if (!spr)
            continue;
        spr->pos.Add(spr->velocity);
        spr->velocity.Add(spr->acceleration);
        spr->velocity.Damp(spr->drag);
        spr->angle += spr->angle_speed;
        spr->angle_speed += spr->angle_accel;
        spr->size += spr->shrink_step;
        empty = false;
        if (fade_start < age)
        {
            FadePackedColor(spr->color, spr->color_step);
            if (!(spr->color & 0xFFFFFF))
            {
                spr->Destroy(3);
                sprs[idx] = 0;
            }
        }
    }
    if (empty)
        Finish();
}

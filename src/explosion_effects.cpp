#include <new>
#include <string.h>

#include "global.h"
#include "audio_session.h"
#include "effect_lifecycle.h"
#include "game_heap.h"
#include "level_session.h"
#include "game_tick.h"
#include "visual_object.h"
#include "model_render_view.h"
#include "game_diagnostics.h"
#include "explosion_effects.h"
#include "screen_flash.h"
#include "camera.h"
#include "particle_sprite.h"
#include "exp_act_burst.h"
#include "ring_burst_effect.h"
#include "effect_object.h"

ExplosionQueue explosion_queue = {0};

void ResetExplosionState()
{
    memset(&explosion_queue, 0, sizeof(explosion_queue));
}

static void MarkExplosionObjects(ModelInstance *instance)
{
    FUNCTION_MARKER(0x80067338, "SLUS_003.73");
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (!(object.lifecycle_flags & 64))
            object.activation_flags |= 4;
        instance = instance->next;
    }
}

void ResetExplosionQueue()
{
    FUNCTION_MARKER(0x8006737C, "SLUS_003.73");
    explosion_queue.count = 0;
}

void QueueExplosion(const Vector3 *pos, uint32 radius, uint32 strength, sint32 priority)
{
    uint32 count, idx;
    FUNCTION_MARKER(0x80067388, "SLUS_003.73");
    count = explosion_queue.count;
    bool append = (sint32)count < 3;
    if (append)
        idx = count;
    else
    {
        for (idx = 0; idx < 3; ++idx)
            if (explosion_queue.entries[idx].priority < priority)
                break;
        if (idx == 3)
            return;
    }
    Vector3 pos_copy = *pos;
    ExplosionEvent *event = &explosion_queue.entries[idx];
    event->pos = pos_copy;
    if (append)
    {
        count = explosion_queue.count;
        idx = count;
        event = &explosion_queue.entries[idx];
    }
    event->radius = radius;
    event->strength = strength;
    event->priority = priority;
    if (append)
        explosion_queue.count = count + 1;
    MarkExplosionObjects(visual_lists.actors);
    MarkExplosionObjects(visual_lists.solids);
}

uint32 SpawnExplosionParticles(const Vector3 *pos, uint32 count, uint32 resource, uint16 size, uint32 spread)
{
    uint32 result;
    uint32 idx = 0;
    GameHeapBlock *child;
    FUNCTION_MARKER(0x8001E4C8, "SLUS_003.73");
    result = IsGameHeapUnderPressure();
    if (result)
        return result;
    while ((sint32)idx < (sint32)count)
    {
        child = EffectObject::AllocateStorage(112);
        if (child)
        {
            new (child) ParticleSprite(pos, 128, 128, 128, resource, size, 1, spread, 0x3000, 30);
        }
        ++idx;
        result = (sint32)idx < (sint32)count;
    }
    return result;
}

void HandleExplosionProximity(const Vector3 *pos, uint32 kind)
{
    Vector3 listener;
    GameHeapBlock *child;
    uint32 sound;
    sint32 distance;
    FUNCTION_MARKER(0x8001E1B8, "SLUS_003.73");
    if (IsGameHeapUnderPressure())
        return;
    listener.x = (uint32)GetModelRenderView().pos.x << 12;
    listener.y = (uint32)GetModelRenderView().pos.y << 12;
    listener.z = (uint32)GetModelRenderView().pos.z << 12;
    distance = pos->Distance(listener);
    if (kind != 0 && kind != 1)
        return;
    sound = (game_clock.tick & 1) ? 1 : 2;
    PlayPositionedSound(sound, pos, 0);
    child = EffectObject::AllocateStorage(160);
    if (child)
    {
        if (kind)
            new (child) ExpActBurst(pos, 20, 200, 150, 10, 5000, 10, 0, 0, 1, 0, 7, 250, 5, 20, 65);
        else
            new (child) ExpActBurst(pos, 10, 100, 70, 10, 3000, 10, 0, 0, 1, 0, 10, 250, 14, 10, 128);
    }
    if (kind && distance < 6000)
        SetScrFlash(70, 30, 0, 20, 0, 0);
    if (distance < 4000)
    {
        Camera *camera = static_cast<Camera *>(visual_lists.cameras->owner);
        uint32 mode = kind ? (distance < 2000 ? 0 : 1) : (distance < 2000 ? 1 : 2);
        camera->ApplyExplosionShake(mode);
    }
}

void SpawnExplosion(const Vector3 *pos, bool large)
{
    uint32 count;
    if (large)
    {
        FUNCTION_MARKER(0x8001E598, "SLUS_003.73");
    }
    else
    {
        FUNCTION_MARKER(0x8001E650, "SLUS_003.73");
    }
    if (level_environment.area_kind != 9)
        QueueExplosion(pos, 512, 32, 256);
    if (IsGameHeapUnderPressure())
        return;
    count = RandomRange(20);
    SpawnExplosionParticles(pos, count, 10, 750, 80);
    HandleExplosionProximity(pos, large);
    RingBurstEffect::SpawnExplosion(pos, large ? 200 : 70, 240, 200, 0, 5, 0, 100);
}

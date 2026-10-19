#include <new>
#include <string.h>

#include "vector_distance.h"
#include "audio_session.h"
#include "effect_lifecycle.h"
#include "game_heap.h"
#include "level_session.h"
#include "game_tick.h"

#include "visual_object.h"
#include "model_render_view.h"
#include "game_diagnostics.h"
#include "explosion_effects.h"
#include "game_random.h"
#include "screen_flash.h"
#include "camera.h"
#include "effect_sprite.h"
#include "exp_act_burst.h"
#include "ring_burst_effect.h"

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

void QueueExplosion(const uint32 *pos, uint32 radius, uint32 strength, uint32 priority)
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
            if ((sint32)explosion_queue.entries[idx].priority < (sint32)priority)
                break;
        if (idx == 3)
            return;
    }
    Vector3 pos_copy;
    memcpy(pos_copy.words, pos, sizeof(pos_copy.words));
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

uint32 SpawnExplosionParticles(const uint32 *position, uint32 count, uint32 resource, uint32 size, uint32 spread)
{
    uint32 result;
    uint32 index = 0;
    GameHeapBlock *child;
    FUNCTION_MARKER(0x8001E4C8, "SLUS_003.73");
    result = IsGameHeapUnderPressure();
    if (result)
        return result;
    size &= 65535;
    while ((sint32)index < (sint32)count)
    {
        child = AllocateEffectStorage(112);
        if (child)
        {
            new (child) ParticleSprite(position, 128, 128, 128, resource, size, 1, spread, 0x3000, 30);
        }
        ++index;
        result = (uint32)((sint32)index < (sint32)count);
    }
    return result;
}

uint32 HandleExplosionProximity(const uint32 *position, uint32 kind)
{
    uint32 result;
    Vector3 listener;
    GameHeapBlock *child;
    uint32 sound;
    sint32 distance;
    FUNCTION_MARKER(0x8001E1B8, "SLUS_003.73");
    result = IsGameHeapUnderPressure();
    if (result)
        return result;
    listener.words[0] = GetModelRenderView().pos.words[0] << 12;
    listener.words[1] = GetModelRenderView().pos.words[1] << 12;
    listener.words[2] = GetModelRenderView().pos.words[2] << 12;
    distance = (sint32)MeasurePositionDistance(position, listener.words);
    if (kind != 0 && kind != 1)
        return 1;
    sound = (game_clock.tick & 1) ? 1 : 2;
    PlayPositionedSound(sound, position, 0);
    child = AllocateEffectStorage(160);
    if (child)
    {
        if (kind)
            new (child) ExpActBurst(position, 20, 200, 150, 10, 5000, 10, 0, 0, 1, 0, 7, 250, 5, 20, 65);
        else
            new (child) ExpActBurst(position, 10, 100, 70, 10, 3000, 10, 0, 0, 1, 0, 10, 250, 14, 10, 128);
    }
    if (kind && distance < 6000)
        SetScreenFlash(70, 30, 0, 20, 0, 0);
    result = (uint32)(distance < 2000);
    if (distance < 4000)
    {
        Camera *camera = static_cast<Camera *>(visual_lists.cameras->owner);
        uint32 mode = kind ? (distance < 2000 ? 0 : 1) : (distance < 2000 ? 1 : 2);
        return camera->ApplyExplosionShake(mode);
    }
    return result;
}

void SpawnExplosion(const uint32 *position, uint32 large)
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
        QueueExplosion(position, 512, 32, 256);
    if (IsGameHeapUnderPressure())
        return;
    count = RandomRange(20);
    SpawnExplosionParticles(position, count, 10, 750, 80);
    HandleExplosionProximity(position, large ? 1 : 0);
    SpawnExplosionRing(position, large ? 200 : 70, 240, 200, 0, 5, 0, 100);
}

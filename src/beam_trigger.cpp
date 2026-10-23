#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "beam_trigger.h"
#include "effect_lifecycle.h"
#include "record_data.h"
#include "collision.h"
#include "record_factory.h"
#include "audio_session.h"
#include "global.h"
#include "point_segments.h"
#include "effect_object.h"

BeamTrigger::BeamTrigger(uint16 idx) : VisualObject(), triggered(false), timer(0), spawn_rec(0), effect()
{
    RecordDataBorrow rec_borrow;
    uint16 reference;
    RecordCursor cursor;
    RecordCursor rec;
    FUNCTION_MARKER(0x8001A2E8, "SLUS_003.73");
    model.flags |= 1;
    lifecycle_flags &= 0xFFEF;
    AddToList(visual_lists.actors);
    triggered = false;
    ReadRecPos(&model.pos, idx);
    cursor = GetRecPayload(idx);
    reference = (cursor + 2).Read16();
    rec = GetLevelRec(reference);
    if (rec.Read16() == 3)
    {
        spawn_rec = (cursor + 4).Read16();
        reference = (cursor + 2).Read16();
    }
    else
    {
        spawn_rec = reference;
        reference = (cursor + 4).Read16();
    }
    ReadRecPos(&target, reference);
    rgb[0] = 32;
    rgb[1] = 0;
    rgb[2] = 0;
    timer = 0;
    InitializeEffect();
}

uint32 BeamTrigger::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001A3E8, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    if (effect)
        effect->Destroy(3);
    effect = 0;
    VisualObject::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

uint8 BeamTrigger::InitializeEffect()
{
    FUNCTION_MARKER(0x8001A478, "SLUS_003.73");
    GameHeapBlock *allocation = EffectObject::AllocateStorage(96);
    if (!allocation)
    {
        GameDiagnostic(("Beam trigger point allocation failed\n"));
        abort();
    }
    effect = new (allocation) PointSegments(1);
    effect->origin = model.pos;
    effect->persistent = true;
    effect->EnableSegs();
    effect->points[0].pos = target;
    effect->points[0].rgbc.r = rgb[0];
    effect->points[0].rgbc.g = rgb[1];
    effect->points[0].rgbc.b = rgb[2];
    return rgb[2];
}

uint32 BeamTrigger::Update()
{
    FUNCTION_MARKER(0x8001A550, "SLUS_003.73");
    ModelInstance *player = visual_lists.players;
    bool active = false;
    if (player && CollisionRay::TraceSeg(model.pos, target, player))
    {
        active = true;
        if (!triggered)
        {
            PlaySoundEffect(41, 8192, 0);
            // The script may destroy this trigger or its effect
            SpawnRec(spawn_rec);
            triggered = true;
        }
        if (effect)
        {
            effect->points[0].rgbc.r = rgb[0] ? 255 : 0;
            effect->points[0].rgbc.g = rgb[1] ? 255 : 0;
            effect->points[0].rgbc.b = rgb[2] ? 255 : 0;
        }
        timer = 0;
    }
    uint16 flags = activation_flags;
    uint32 result = flags & 0xFFFE;
    if (flags & 1)
    {
        activation_flags = result;
        result = 1;
        triggered = true;
    }
    if (active)
        return result;
    if (triggered)
    {
        ++timer;
        if (timer >= 61)
            return Finish();
        if (!RandomRange(2))
        {
            for (uint32 axis = 0; axis < 3; ++axis)
            {
                result = rgb[axis] >> 2;
                if (effect)
                {
                    RGBC &color = effect->points[0].rgbc;
                    if (axis == 0)
                        color.r = result;
                    else if (axis == 1)
                        color.g = result;
                    else
                        color.b = result;
                }
            }
            return result;
        }
    }
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        uint32 val = rgb[axis];
        uint32 random = RandomRange(val >> 1);
        val = rgb[axis];
        result = random - (val >> 2);
        if (effect)
        {
            RGBC &color = effect->points[0].rgbc;
            if (axis == 0)
                color.r = val + result;
            else if (axis == 1)
                color.g = val + result;
            else
                color.b = val + result;
        }
    }
    return result;
}

BeamTrigger::~BeamTrigger()
{
    if (effect)
        effect->Destroy(3);
}

#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "beam_trigger.h"
#include "effect_lifecycle.h"
#include "record_data.h"
#include "game_object_allocation.h"
#include "collision.h"
#include "record_factory.h"
#include "audio_session.h"
#include "game_random.h"
#include "point_segments.h"

uint32 BeamTrigger::InitializeEffect()
{
    FUNCTION_MARKER(0x8001A478, "SLUS_003.73");
    GameHeapBlock *allocation = AllocateEffectStorage(96);
    if (!allocation)
    {
        GameDiagnostic("Beam trigger point allocation failed\n");
        abort();
    }
    effect = new (allocation) PointSegments(1);
    effect->origin.words[0] = model.pos.words[0];
    effect->origin.words[1] = model.pos.words[1];
    effect->origin.words[2] = model.pos.words[2];
    effect->persistent = 1;
    effect->EnableSegments();
    effect->points[0].pos.words[0] = target.words[0];
    effect->points[0].pos.words[1] = target.words[1];
    effect->points[0].pos.words[2] = target.words[2];
    effect->points[0].rgbc.r = rgb[0];
    effect->points[0].rgbc.g = rgb[1];
    effect->points[0].rgbc.b = rgb[2];
    return rgb[2];
}

BeamTrigger::BeamTrigger(const uint8 *, uint32 index) : VisualObject(), triggered(0), timer(0), spawn_rec(0), effect()
{
    RecordDataBorrow record_borrow;
    uint32 flags, secondary_flags, reference, value;
    RecordCursor cursor;
    RecordCursor record;
    FUNCTION_MARKER(0x8001A2E8, "SLUS_003.73");
    flags = this->model.flags;
    secondary_flags = this->lifecycle_flags;
    this->model.flags = flags | 1;
    this->lifecycle_flags = secondary_flags & 0xFFEF;
    AddToList(visual_lists.actors);
    index &= 65535;
    triggered = 0;
    ReadRecordPosition(this->model.pos.words, index);
    cursor = GetRecordPayload(index);
    reference = (cursor + 2).Read16();
    record = GetLevelRecord(reference);
    value = (record).Read16();
    if ((sint16)value == 3)
    {
        value = (cursor + 4).Read16();
        spawn_rec = value;
        reference = (cursor + 2).Read16();
    }
    else
    {
        spawn_rec = reference;
        reference = (cursor + 4).Read16();
    }
    ReadRecordPosition(target.words, reference);
    rgb[0] = 32;
    rgb[1] = 0;
    rgb[2] = 0;
    timer = 0;
    InitializeEffect();
}

BeamTrigger::~BeamTrigger()
{
    if (effect)
        effect->Destroy(3);
}

uint32 BeamTrigger::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001A3E8, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    if (effect)
        effect->Destroy(3);
    effect = 0;
    VisualObject::Destroy(0);
    return reason & 1 ? ReleaseObjectStorage(this) : 0;
}

uint32 BeamTrigger::Update()
{
    FUNCTION_MARKER(0x8001A550, "SLUS_003.73");
    ModelInstance *player = visual_lists.players;
    uint32 active = 0;
    if (player && TraceSegmentCollision(model.pos.words, target.words, player))
    {
        active = 1;
        if (!triggered)
        {
            PlaySoundEffect(41, 8192, 0);
            SpawnRecord(spawn_rec);
            triggered = 1;
        }
        if (effect)
        {
            effect->points[0].rgbc.r = rgb[0] ? 255 : 0;
            effect->points[0].rgbc.g = rgb[1] ? 255 : 0;
            effect->points[0].rgbc.b = rgb[2] ? 255 : 0;
        }
        timer = 0;
    }
    uint32 flags = activation_flags;
    uint32 result = flags & 0xFFFE;
    if (flags & 1)
    {
        activation_flags = result;
        result = 1;
        triggered = 1;
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

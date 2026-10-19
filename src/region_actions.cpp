
#include "effect_lifecycle.h"

#include "effect_object.h"
#include "model_resources.h"
#include "game_diagnostics.h"
#include "region_actions.h"
#include "visual_object.h"

static uint32 ApplyRegionToObjects(const uint32 *lower, const uint32 *upper, ModelInstance *instance, uint32 trigger)
{
    ModelBorrow model_borrow;
    uint32 count = 0;
    FUNCTION_MARKER(0x80067ABC, "SLUS_003.73");
    while (instance)
    {
        ModelInstance *next = instance->next;
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (object.IsAlive() && !object.IsFinished())
        {
            uint32 x = instance->pos.words[0];
            uint32 y = instance->pos.words[1];
            uint32 z = instance->pos.words[2];
            if ((sint32)x >= (sint32)lower[0] && (sint32)upper[0] >= (sint32)x && (sint32)z >= (sint32)lower[2] && (sint32)upper[2] >= (sint32)z && (sint32)y >= (sint32)lower[1] && (sint32)upper[1] >= (sint32)y)
            {
                if (trigger)
                    object.Finish();
                else
                    object.Destroy(3);
                ++count;
            }
        }
        instance = next;
    }
    return count;
}

static uint32 RemoveRegionEffects(const uint32 *lower, const uint32 *upper)
{
    ModelBorrow model_borrow;
    EffectObject *object, *next;
    uint32 kind, axis, value, count = 0;
    FUNCTION_MARKER(0x8001F7A0, "SLUS_003.73");
    object = effect_lists.controllers;
    while (object)
    {
        kind = object->kind;
        next = object->next;
        if (object->IsAlive() && kind == 7)
        {
            for (axis = 0; axis < 3; ++axis)
            {
                value = object->pos.words[axis];
                if ((sint32)value < (sint32)lower[axis])
                    break;
                if ((sint32)upper[axis] < (sint32)value)
                    break;
            }
            if (axis == 3)
            {
                ++count;
                object->Destroy(3);
            }
        }
        object = next;
    }
    return count;
}

uint32 ClearRegion(const uint32 *lower, const uint32 *upper)
{
    uint32 count;
    FUNCTION_MARKER(0x80067C20, "SLUS_003.73");
    count = ApplyRegionToObjects(lower, upper, visual_lists.actors, 1);
    count += ApplyRegionToObjects(lower, upper, visual_lists.solids, 1);
    count += ApplyRegionToObjects(lower, upper, visual_lists.pickups, 1);
    count += ApplyRegionToObjects(lower, upper, visual_lists.suspended, 0);
    return count + RemoveRegionEffects(lower, upper);
}

void SetRegionObjectsEnabled(const uint32 *lower, const uint32 *upper, uint32 enabled)
{
    ModelInstance *object;
    Vector3 position;
    uint32 order[3] = {0, 2, 1}, index, axis, result;
    FUNCTION_MARKER(0x80067CCC, "SLUS_003.73");
    object = GetModelSceneObjects();
    while (object)
    {
        result = GetModelBank(object->bank).loaded;
        if (result)
        {
            position = object->pos;
            for (index = 0; index < 3; ++index)
            {
                axis = order[index];
                result = (sint32)position.words[axis] < (sint32)lower[axis];
                if (result)
                    break;
                result = (sint32)upper[axis] < (sint32)position.words[axis];
                if (result)
                    break;
            }
            if (index == 3)
            {
                result = object->flags;
                result = enabled ? result & 65534 : result | 1;
                object->flags = result;
            }
        }
        object = object->next;
    }
}

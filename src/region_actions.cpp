#include "effect_lifecycle.h"
#include "effect_object.h"
#include "model_bank.h"
#include "game_diagnostics.h"
#include "region_actions.h"
#include "visual_object.h"

static uint32 ApplyRegionToObjects(const Vector3 *lower, const Vector3 *upper, ModelInstance *instance, uint32 trigger)
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
            sint32 x = instance->pos.x;
            sint32 y = instance->pos.y;
            sint32 z = instance->pos.z;
            if (x >= lower->x && upper->x >= x && z >= lower->z && upper->z >= z && y >= lower->y && upper->y >= y)
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

static uint32 RemoveRegionEffects(const Vector3 *lower, const Vector3 *upper)
{
    ModelBorrow model_borrow;
    EffectObject *object, *next;
    uint8 kind;
    uint32 axis, count = 0;
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
                sint32 val = object->pos.comp[axis];
                if (val < lower->comp[axis])
                    break;
                if (upper->comp[axis] < val)
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

uint32 ClearRegion(const Vector3 *lower, const Vector3 *upper)
{
    uint32 count;
    FUNCTION_MARKER(0x80067C20, "SLUS_003.73");
    count = ApplyRegionToObjects(lower, upper, visual_lists.actors, 1);
    count += ApplyRegionToObjects(lower, upper, visual_lists.solids, 1);
    count += ApplyRegionToObjects(lower, upper, visual_lists.pickups, 1);
    count += ApplyRegionToObjects(lower, upper, visual_lists.suspended, 0);
    return count + RemoveRegionEffects(lower, upper);
}

void SetRegionObjectsEnabled(const Vector3 *lower, const Vector3 *upper, uint32 enabled)
{
    ModelInstance *object;
    FUNCTION_MARKER(0x80067CCC, "SLUS_003.73");
    object = ModelBank::GetSceneObjects();
    while (object)
    {
        if (ModelBank::Get(object->bank).loaded)
        {
            Vector3 pos = object->pos;
            if (pos.x >= lower->x && upper->x >= pos.x && pos.z >= lower->z && upper->z >= pos.z && pos.y >= lower->y && upper->y >= pos.y)
            {
                if (enabled)
                    object->flags &= 65534;
                else
                    object->flags |= 1;
            }
        }
        object = object->next;
    }
}

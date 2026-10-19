#include <string.h>

#include "record_data.h"
#include "game_diagnostics.h"
#include "enemy_list.h"
#include "visual_object.h"
#include "vector_distance.h"
#include "record_factory.h"

uint32 FinishSelectedEnemies()
{
    uint32 result = 0x800A0000;
    FUNCTION_MARKER(0x8005C338, "SLUS_003.73");
    ModelInstance *instance = visual_lists.actors;
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        uint32 kind = object.kind;
        ModelInstance *next = instance->next;
        uint32 relative = kind - 200;
        if (relative < 11)
        {
            switch (kind)
            {
                case 200:
                case 201:
                case 202:
                case 204:
                case 205:
                case 207:
                case 208:
                case 209:
                case 210:
                    result = object.Finish();
                    break;
                default:
                    result = 0x8005C3A4;
                    break;
            }
        }
        else
            result = relative << 2;
        instance = next;
    }
    return result;
}

void ActivateObjectsByIndex(ModelInstance *instance, uint32 index)
{
    FUNCTION_MARKER(0x80064E10, "SLUS_003.73");
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (object.record_idx == index)
            object.activation_flags |= 1;
        instance = instance->next;
    }
}

uint32 ActivateEnemyIndices(RecordCursor indices)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x80064E50, "SLUS_003.73");
    uint32 count = indices.Read16();
    indices.Require(2 + count * 2);
    for (uint32 index = 0; index < count; ++index)
    {
        uint32 identifier = (indices + 2 + index * 2).Read16();
        sint32 kind = (sint16)GetLevelRecord(identifier).Read16();
        if (kind == 1 || kind == 7)
        {
            ActivateObjectsByIndex(visual_lists.actors, identifier);
            ActivateObjectsByIndex(visual_lists.solids, identifier);
        }
    }
    return 0;
}

uint32 SpawnEnemyIndices(RecordCursor indices)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x80064A08, "SLUS_003.73");
    uint32 count = indices.Read16();
    indices.Require(2 + count * 2);
    for (uint32 index = 0; index < count; ++index)
        SpawnRecord((indices + 2 + index * 2).Read16());
    return 0;
}

void ClearObjectActivationFlags(ModelInstance *instance)
{
    FUNCTION_MARKER(0x80063738, "SLUS_003.73");
    while (instance)
    {
        ModelInstance *next = instance->next;
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        object.activation_flags &= 1;
        instance = next;
    }
}

void RestoreNearbySuspendedObjects()
{
    FUNCTION_MARKER(0x800638A4, "SLUS_003.73");
    ModelInstance *instance = visual_lists.suspended;
    while (instance)
    {
        ModelInstance *next = instance->next;
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        uint32 flags = object.lifecycle_flags;
        if (flags & 2)
        {
            uint32 distance = MeasurePositionDistance(visual_lists.players->pos.words, instance->pos.words);
            if (visual_activation_distance >= distance)
                object.RestoreList();
        }
        instance = next;
    }
}

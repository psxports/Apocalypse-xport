#include "object_initializers.h"
#include "effect_object.h"
#include "proximity_ring_emitter.h"
#include "visual_object.h"
#include "object_damage.h"
#include "effect_lifecycle.h"
#include "game_diagnostics.h"
#include "record_groups.h"
#include "record_data.h"
#include "model_resources.h"

uint32 SetRecordGroupEnabled(uint32 group, uint32 enabled)
{
    RecordDataBorrow record_borrow;
    uint32 count, index, kind, flags, identifier;
    RecordCursor cursor;
    RecordCursor record;
    RecordCursor payload;
    ModelInstance *object;
    FUNCTION_MARKER(0x80064B24, "SLUS_003.73");
    cursor = GetRecordPayload(group);
    count = (cursor).Read16();
    cursor.Require(2 + count * 2);
    cursor += 2;
    for (index = 0; index < count; ++index)
    {
        identifier = (cursor).Read16();
        record = GetLevelRecord(identifier);
        kind = (uint32)(sint32)(sint16)(record).Read16();
        cursor += 2;
        if (kind == 2 || kind == 9)
        {
            payload = record + 4 + (sint16)(record + 2).Read16() * 2;
            payload = AlignRecordData(payload);
            object = FindModelObject((payload).Read32());
            if (object)
            {
                flags = object->flags;
                object->flags = enabled ? flags & 65534 : flags | 1;
            }
        }
    }
    return 0;
}

static void SuspendObjectsByRecord(ModelInstance *&head, uint32 record)
{
    FUNCTION_MARKER(0x80064F08, "SLUS_003.73");
    ModelInstance *instance = head;
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (object.record_idx == record)
            object.MoveToTemporaryList(head);
        instance = instance->next;
    }
}

static void ResumeObjectsByRecord(ModelInstance *instance, uint32 record)
{
    FUNCTION_MARKER(0x80064F70, "SLUS_003.73");
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (object.record_idx == record)
            object.RestoreList();
        instance = instance->next;
    }
}

uint32 SetObjectRecordGroupMode(RecordCursor group, uint32 mode)
{
    RecordDataBorrow record_borrow;
    RecordCursor cursor(group + 2);
    uint32 count, index, record, kind;
    FUNCTION_MARKER(0x80064FC8, "SLUS_003.73");
    count = (group).Read16();
    group.Require(2 + count * 2);
    for (index = 0; index < count; ++index)
    {
        record = (cursor).Read16();
        kind = (uint32)(sint32)(sint16)(GetLevelRecord(record)).Read16();
        if (kind == 1 || kind == 7)
        {
            if (mode == 5)
            {
                SuspendObjectsByRecord(visual_lists.actors, record);
                SuspendObjectsByRecord(visual_lists.solids, record);
            }
            else
            {
                ResumeObjectsByRecord(visual_lists.actors, record);
                ResumeObjectsByRecord(visual_lists.solids, record);
            }
        }
        cursor += 2;
    }
    return 0;
}

static void ApplyObjectListRecordAction(uint32 identifier, ModelInstance *instance, uint32 mode)
{
    FUNCTION_MARKER(0x80064A68, "SLUS_003.73");
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (object.record_idx == identifier)
        {
            if (!mode)
                object.Finish();
            else if (mode == 1)
            {
                uint32 damage = (uint32)(sint32)(sint16)(uint16)object.health;
                object.Damage(damage, zero_vector.components, 0);
            }
        }
        instance = instance->next;
    }
}

uint32 ApplyRecordGroupAction(uint32 group, uint32 mode)
{
    RecordDataBorrow record_borrow;
    uint32 count, index, identifier, flags;
    RecordCursor list;
    RecordCursor descriptor;
    RecordCursor pointer;
    ModelInstance *head;
    sint32 type;
    FUNCTION_MARKER(0x80064C1C, "SLUS_003.73");
    list = GetRecordPayload(group);
    count = (list).Read16();
    list.Require(2 + count * 2);
    list += 2;
    for (index = 0; index < count; ++index)
    {
        identifier = (list).Read16();
        descriptor = GetLevelRecord(identifier);
        type = (sint16)(descriptor).Read16();
        list += 2;
        if (type == 1)
        {
            head = visual_lists.actors;
            ApplyObjectListRecordAction(identifier, head, mode);
            head = visual_lists.solids;
            ApplyObjectListRecordAction(identifier, head, mode);
        }
        else if (type == 2 || type == 9)
        {
            pointer = descriptor + 2;
            pointer += (sint16)(pointer).Read16() * 2 + 2;
            pointer = AlignRecordData(pointer);
            ModelInstance *scene_object = FindModelObject((pointer).Read32());
            if (scene_object)
            {
                if (mode == 1)
                    DamageLevelObject(scene_object, 0, 65535);
                else
                {
                    flags = scene_object->flags;
                    scene_object->flags = flags | 1;
                }
            }
        }
        else if (type == 10)
            DestroyEffectsByIdentifier(identifier);
        else if (type == 500 || type == 501)
        {
            EffectObject *effect = effect_lists.controllers;
            while (effect && (!effect->IsAlive() || effect->kind != 2 || static_cast<ProximityRingEmitter *>(effect)->record_idx != identifier))
                effect = effect->next;
            if (effect)
                effect->Finish();
        }
    }
    return 0;
}

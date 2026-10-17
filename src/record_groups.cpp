#include "visual_object.h"
#include "object_damage.h"
#include "effect_lifecycle.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "record_groups.h"
#include "record_data.h"
#include "model_resources.h"

uint32 SetRecordGroupEnabled(uint32 group, uint32 enabled)
{
    uint32 cursor, count, index, record, kind, payload, flags, identifier, table;
    ModelInstance *object;
    FUNCTION_MARKER(0x80064B24, "SLUS_003.73");
    cursor = GetRecordPayload(group);
    count = r_u16(cursor);
    cursor += 2;
    for (index = 0; index < count; ++index)
    {
        identifier = r_u16(cursor);
        table = r_u32(0x800FF624);
        record = r_u32(table + identifier * 4);
        kind = (uint32)(sint32)(sint16)r_u16(record);
        cursor += 2;
        if (kind == 2 || kind == 9)
        {
            payload = record + 4 + ((uint32)(sint32)(sint16)r_u16(record + 2) << 1);
            if (payload & 2)
                payload += 2;
            object = FindModelObject(r_u32(payload));
            if (object)
            {
                flags = object->flags;
                object->flags = enabled ? flags & 65534 : flags | 1;
            }
        }
    }
    return 0;
}

void SuspendObjectsByRecord(ModelInstance *&head, uint32 record)
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

void ResumeObjectsByRecord(ModelInstance *instance, uint32 record)
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

uint32 SetObjectRecordGroupMode(uint32 group, uint32 mode)
{
    uint32 cursor = group + 2, count, index, record, kind;
    FUNCTION_MARKER(0x80064FC8, "SLUS_003.73");
    count = r_u16(group);
    for (index = 0; index < count; ++index)
    {
        record = r_u16(cursor);
        kind = (uint32)(sint32)(sint16)r_u16(r_u32(r_u32(0x800FF624) + record * 4));
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






void ApplyObjectListRecordAction(uint32 identifier, ModelInstance *instance, uint32 mode)
{
    FUNCTION_MARKER(0x80064A68, "SLUS_003.73");
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        const uint32 address = NativeObjectGuestAddress(&object);
        if (object.record_idx == identifier)
        {
            if (!mode)
                object.Finish();
            else if (mode == 1)
            {
                uint32 damage = (uint32)(sint32)(sint16)r_u16(address + 218);
                object.Damage(damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
            }
        }
        instance = instance->next;
    }
}

uint32 ApplyRecordGroupAction(uint32 group, uint32 mode)
{
    uint32 list, count, index, identifier, table, descriptor, pointer, object, flags;
    ModelInstance *head;
    sint32 type;
    FUNCTION_MARKER(0x80064C1C, "SLUS_003.73");
    list = GetRecordPayload(group);
    count = r_u16(list);
    list += 2;
    for (index = 0; index < count; ++index)
    {
        identifier = r_u16(list);
        table = r_u32(0x800FF624);
        descriptor = r_u32(table + identifier * 4);
        type = (sint16)r_u16(descriptor);
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
            pointer += ((uint32)(sint32)(sint16)r_u16(pointer) << 1) + 2;
            if (pointer & 2)
                pointer += 2;
            ModelInstance *scene_object = FindModelObject(r_u32(pointer));
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
            object = r_u32(0x800FF434);
            while (object && (r_u8(object + 67) != 2 || r_u16(object + 10) != identifier))
                object = r_u32(object + 4);
            if (object)
                sub_80032ED8(object);
        }
    }
    return 0;
}




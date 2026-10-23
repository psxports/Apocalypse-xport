#include "object_initializers.h"
#include "effect_object.h"
#include "proximity_ring_emitter.h"
#include "visual_object.h"
#include "object_damage.h"
#include "effect_lifecycle.h"
#include "record_groups.h"
#include "record_data.h"
#include "model_bank.h"
#include "path_particle_emitter.h"

static void ApplyObjectListRecAction(uint32 identifier, ModelInstance *instance, uint32 mode)
{
    FUNCTION_MARKER(0x80064A68, "SLUS_003.73");
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (object.rec_idx == identifier)
        {
            if (!mode)
                object.Finish();
            else if (mode == 1)
            {
                object.Damage(object.health, &zero_vector, 0);
            }
        }
        instance = instance->next;
    }
}

uint32 SetRecGrpEnabled(uint32 grp, bool enabled)
{
    RecordDataBorrow rec_borrow;
    uint32 idx;
    uint16 count, identifier, kind;
    RecordCursor cursor;
    RecordCursor rec;
    RecordCursor payload;
    ModelInstance *object;
    FUNCTION_MARKER(0x80064B24, "SLUS_003.73");
    cursor = GetRecPayload(grp);
    count = cursor.Read16();
    cursor.Require(2 + count * 2);
    cursor += 2;
    for (idx = 0; idx < count; ++idx)
    {
        identifier = cursor.Read16();
        rec = GetLevelRec(identifier);
        kind = rec.Read16();
        cursor += 2;
        if (kind == 2 || kind == 9)
        {
            payload = rec + 4 + (sint16)(rec + 2).Read16() * 2;
            payload = payload.Aligned(4);
            object = ModelBank::FindObject(payload.Read32());
            if (object)
            {
                if (enabled)
                    object->flags &= 65534;
                else
                    object->flags |= 1;
            }
        }
    }
    return 0;
}

uint32 ApplyRecGrpAction(uint32 grp, uint32 mode)
{
    RecordDataBorrow rec_borrow;
    uint32 idx;
    uint16 count, identifier, type;
    RecordCursor list;
    RecordCursor desc;
    RecordCursor pointer;
    FUNCTION_MARKER(0x80064C1C, "SLUS_003.73");
    list = GetRecPayload(grp);
    count = list.Read16();
    list.Require(2 + count * 2);
    list += 2;
    for (idx = 0; idx < count; ++idx)
    {
        identifier = list.Read16();
        desc = GetLevelRec(identifier);
        type = desc.Read16();
        list += 2;
        if (type == 1)
        {
            ApplyObjectListRecAction(identifier, visual_lists.actors, mode);
            ApplyObjectListRecAction(identifier, visual_lists.solids, mode);
        }
        else if (type == 2 || type == 9)
        {
            pointer = desc + 2;
            pointer += (sint16)pointer.Read16() * 2 + 2;
            pointer = pointer.Aligned(4);
            ModelInstance *scene_object = ModelBank::FindObject(pointer.Read32());
            if (scene_object)
            {
                if (mode == 1)
                    DamageLevelObject(scene_object, 0, 65535);
                else
                {
                    scene_object->flags |= 1;
                }
            }
        }
        else if (type == 10)
            PathParticleEmitter::DestroyByIdentifier(identifier);
        else if (type == 500 || type == 501)
        {
            EffectObject *effect = effect_lists.controllers;
            while (effect && (!effect->IsAlive() || effect->kind != 2 || static_cast<ProximityRingEmitter *>(effect)->rec_idx != identifier))
                effect = effect->next;
            if (effect)
                effect->Finish();
        }
    }
    return 0;
}

static void SuspendObjectsByRec(ModelInstance *&head, uint32 rec)
{
    FUNCTION_MARKER(0x80064F08, "SLUS_003.73");
    ModelInstance *instance = head;
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (object.rec_idx == rec)
            object.MoveToTmpList(head);
        instance = instance->next;
    }
}

static void ResumeObjectsByRec(ModelInstance *instance, uint32 rec)
{
    FUNCTION_MARKER(0x80064F70, "SLUS_003.73");
    while (instance)
    {
        VisualObject &object = *static_cast<VisualObject *>(instance->owner);
        if (object.rec_idx == rec)
            object.RestoreList();
        instance = instance->next;
    }
}

uint32 SetObjectRecGrpMode(RecordCursor grp, uint32 mode)
{
    RecordDataBorrow rec_borrow;
    RecordCursor cursor(grp + 2);
    uint32 idx;
    uint16 count, rec, kind;
    FUNCTION_MARKER(0x80064FC8, "SLUS_003.73");
    count = grp.Read16();
    grp.Require(2 + count * 2);
    for (idx = 0; idx < count; ++idx)
    {
        rec = cursor.Read16();
        kind = GetLevelRec(rec).Read16();
        if (kind == 1 || kind == 7)
        {
            if (mode == 5)
            {
                SuspendObjectsByRec(visual_lists.actors, rec);
                SuspendObjectsByRec(visual_lists.solids, rec);
            }
            else
            {
                ResumeObjectsByRec(visual_lists.actors, rec);
                ResumeObjectsByRec(visual_lists.solids, rec);
            }
        }
        cursor += 2;
    }
    return 0;
}

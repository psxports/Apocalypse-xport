#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "object_initializers.h"
#include "pickup.h"
#include "game_diagnostics.h"
#include "visual_object.h"
#include "game_object_allocation.h"
#include "scripted_effect_controller.h"
#include "floating_hazard.h"
#include "one_shot_activator.h"
#include "resetting_activator.h"
#include "traversal_region.h"
#include "beam_trigger.h"
#include "beam_barrier.h"
#include "scripted_level_object.h"
#include "traversal_rail.h"
#include "flexible_traversal_rail.h"
#include "plague.h"
#include "death.h"
#include "war.h"
#include "the_beast.h"
#include "croc.h"
#include "bat.h"
#include "hound_rat.h"
#include "mini_tank.h"
#include "turret.h"
#include "tank.h"
#include "items.h"
#include "chopper.h"
#include "hovr_swat.h"
#include "swat.h"
#include "resource_script.h"
#include "record_commands.h"
#include "effect_lifecycle.h"
#include "record_data.h"
#include "record_factory.h"
#include "proximity_ring_emitter.h"
#include "activation_record.h"
#include "path_particle_emitter.h"
#include "effect_object.h"

// Original800FF640 activates the record group before terminating
static const uint8 grp_activation_script[4] = {3, 0, 255, 255};

void SpawnRec(uint32 idx)
{
    FUNCTION_MARKER(0x80064874, "SLUS_003.73");
    RecordCursor desc(GetLevelRec(idx));
    sint32 type = (sint16)desc.Read16();
    RecordCursor rec;
    GameHeapBlock *child;
    Vector3 pos;
    uint32 parameters[9];
    uint32 proximity_range;
    uint8 count;
    if (type == 6)
    {
        ResourceScriptBorrow borrow;
        ResourceScript *script = FindScriptRec(idx);
        count = script->spawn_count;
        RecordCursor commands = script->script;
        script->spawn_count = count + 1;
        ExecuteRecCommands(commands, idx, false);
        return;
    }
    if (type < 7)
    {
        if (type == 1 || type == 5)
        {
            CreateObjectFromRec(idx);
            return;
        }
        return;
    }
    if (type == 10)
    {
        PathParticleEmitter::Spawn(idx);
        return;
    }
    if (type < 11)
    {
        if (type == 7)
        {
            CreateObjectFromRec(idx);
            return;
        }
        return;
    }
    if (type != 501)
        return;
    rec = ReadRecPos(&pos, idx);
    use_small_effect_pool = false;
    child = EffectObject::AllocateStorage(100);
    if (child)
    {
        parameters[0] = (rec + 2).Read16();
        proximity_range = rec.Read16();
        parameters[1] = (rec + 4).Read16();
        parameters[2] = (rec + 6).Read16();
        parameters[3] = (rec + 8)[0];
        parameters[4] = (rec + 10)[0];
        parameters[5] = (rec + 12)[0];
        parameters[6] = (rec + 14)[0];
        parameters[7] = (rec + 16)[0];
        parameters[8] = (rec + 18)[0];
        new (child) ProximityRingEmitter(&pos, idx, proximity_range, parameters[0], parameters[1], parameters[2], parameters[3], parameters[4], parameters[5], parameters[6], parameters[7], parameters[8]);
    }
    use_small_effect_pool = true;
    return;
}

uint32 InitializeLevelRecs()
{
    RecordDataBorrow rec_borrow;
    uint32 idx = 0;
    GameHeapBlock *object;
    Vector3 pos;
    uint32 parameters[9];
    uint32 proximity_range;
    RecordCursor desc;
    RecordCursor payload;
    RecordCursor cursor;
    uint16 type;
    FUNCTION_MARKER(0x80065DE8, "SLUS_003.73");
    while (idx < GetLevelRecCount())
    {
        desc = GetLevelRec(idx);
        type = desc.Read16();
        if (type == 6 || type == 2 || type == 9)
        {
            payload = desc + 4 + (sint16)(desc + 2).Read16() * 2;
            payload = payload.Aligned(4);
            RegisterResourceScript(payload.Read32(), idx, type == 6 ? RecordCursor(payload + 4) : RecordCursor(grp_activation_script, sizeof(grp_activation_script)));
        }
        else if (type == 1 || type == 7)
        {
            payload = desc + 8 + (sint16)(desc + 6).Read16() * 2;
            if (RecListContains(1, payload))
            {
                if (type == 1)
                    CreateObjectFromRec(idx);
                else
                {
                    new ActivationRecord(idx, -1);
                }
            }
        }
        else if (type == 5)
        {
            cursor = ReadRecPos(&pos, idx);
            if (cursor.Read16() == 1)
            {
                new ActivationRecord(idx, -1);
            }
        }
        else if (type == 500)
        {
            cursor = ReadRecPos(&pos, idx);
            use_small_effect_pool = false;
            object = EffectObject::AllocateStorage(100);
            if (object)
            {
                parameters[0] = (cursor + 2).Read16();
                proximity_range = cursor.Read16();
                parameters[1] = (cursor + 4).Read16();
                parameters[2] = (cursor + 6).Read16();
                parameters[3] = (cursor + 8)[0];
                parameters[4] = (cursor + 10)[0];
                parameters[5] = (cursor + 12)[0];
                parameters[6] = (cursor + 14)[0];
                parameters[7] = (cursor + 16)[0];
                parameters[8] = (cursor + 18)[0];
                new (object) ProximityRingEmitter(&pos, idx, proximity_range, parameters[0], parameters[1], parameters[2], parameters[3], parameters[4], parameters[5], parameters[6], parameters[7], parameters[8]);
            }
            use_small_effect_pool = true;
        }
        ++idx;
    }
    return 0;
}

VisualObject *CreateObjectFromRec(uint32 idx)
{
    RecordDataBorrow rec_borrow;
    RecordCursor rec;
    RecordCursor cursor;
    RecordCursor data;
    GameHeapBlock *object = 0;
    VisualObject *created = 0;
    ModelInstance **list_head = 0;
    bool preserve_list = false, clear_flag = false, special_flag = false;
    uint16 type, kind;
    Vector3 pos;
    FUNCTION_MARKER(0x800641E8, "SLUS_003.73");
    rec = GetLevelRec(idx);
    type = rec.Read16();
    cursor = rec + 2;
    if (type == 5)
    {
        uint32 resource = cursor.Read16();
        RecordCursor info(ReadRecPos(&pos, idx));
        uint32 mode, lifetime;
        preserve_list = true;
        special_flag = true;
        mode = (info + 2).Read16();
        lifetime = (info + 4).Read16();
        mode = mode == 0 ? 4 : 0;
        created = Pickup::Spawn(resource, &pos, mode, lifetime, &zero_vector);
        goto finish;
    }
    if (type == 7)
        special_flag = true;
    else if (type != 1)
        return 0;
    kind = cursor.Read16();
    cursor += 2;
    cursor.Read16();
    cursor += 2;
    cursor += 2 + (sint16)cursor.Read16() * 2;
    preserve_list = RecListContains(2, cursor);
    clear_flag = RecListContains(4, cursor);
    data = SkipRecList(cursor).Aligned(4);
    switch (kind)
    {
        case 200:
            object = AllocateZeroedObject(624);
            if (!object)
                return 0;
            created = new (object) Swat(data, idx);
            break;
        case 201:
            object = AllocateZeroedObject(572);
            if (!object)
                return 0;
            created = new (object) HovrSwat(data, idx);
            break;
        case 202:
            object = AllocateZeroedObject(544);
            if (!object)
                return 0;
            created = new (object) Chopper(data, idx);
            break;
        case 203:
            object = AllocateZeroedObject(520);
            if (!object)
                return 0;
            created = new (object) ScriptedEffectController(data, idx);
            break;
        case 204:
            object = AllocateZeroedObject(328);
            if (!object)
                return 0;
            created = new (object) Items(data, idx);
            break;
        case 205:
            object = AllocateZeroedObject(576);
            if (!object)
                return 0;
            created = new (object) Tank(data, idx);
            break;
        case 206:
            object = AllocateZeroedObject(556);
            if (!object)
                return 0;
            created = new (object) Turret(data, idx);
            break;
        case 207:
            object = AllocateZeroedObject(528);
            if (!object)
                return 0;
            created = new (object) MiniTank(data, idx);
            break;
        case 208:
            object = AllocateZeroedObject(496);
            if (!object)
                return 0;
            created = new (object) HoundRat(data, idx, 1);
            break;
        case 209:
            object = AllocateZeroedObject(504);
            if (!object)
                return 0;
            created = new (object) Bat(data, idx);
            break;
        case 210:
            object = AllocateZeroedObject(496);
            if (!object)
                return 0;
            created = new (object) HoundRat(data, idx, 0);
            break;
        case 212:
            object = AllocateZeroedObject(548);
            if (!object)
                return 0;
            created = new (object) Croc(data, idx);
            break;
        case 220:
            object = AllocateZeroedObject(576);
            if (!object)
                return 0;
            created = new (object) TheBeast(data, idx);
            break;
        case 221:
            object = AllocateZeroedObject(568);
            if (!object)
                return 0;
            created = new (object) War(data, idx);
            break;
        case 222:
            object = AllocateZeroedObject(684);
            if (!object)
                return 0;
            created = new (object) Death(data, idx);
            break;
        case 223:
            object = AllocateZeroedObject(656);
            if (!object)
                return 0;
            created = new (object) Plague(data, idx);
            break;
        case 400:
            use_small_effect_pool = false;
            object = AllocateZeroedObject(348);
            if (object)
            {
                data.Require(12);
                created = new (object) TraversalRail(data, idx);
            }
            use_small_effect_pool = true;
            break;
        case 401:
            use_small_effect_pool = false;
            object = AllocateZeroedObject(644);
            if (object)
            {
                data.Require(12);
                created = new (object) FlexibleTraversalRail(data, idx);
            }
            use_small_effect_pool = true;
            break;
        case 402:
            object = AllocateZeroedObject(540);
            if (!object)
                return 0;
            created = new (object) ScriptedLevelObject(data, idx);
            break;
        case 403:
            object = AllocateZeroedObject(312);
            if (!object)
                return 0;
            created = new (object) OneShotActivator(data, idx);
            break;
        case 404:
            use_small_effect_pool = false;
            object = AllocateZeroedObject(332);
            if (object)
            {
                data.Require(12);
                created = new (object) BeamBarrier(data, idx, true);
            }
            use_small_effect_pool = true;
            break;
        case 405:
            use_small_effect_pool = false;
            object = AllocateZeroedObject(324);
            if (object)
                created = new (object) BeamTrigger(idx);
            use_small_effect_pool = true;
            break;
        case 406:
            object = AllocateZeroedObject(336);
            if (!object)
                return 0;
            created = new (object) TraversalRegion(data, idx);
            break;
        case 407:
            object = AllocateZeroedObject(336);
            if (!object)
                return 0;
            created = new (object) ResettingActivator(data, idx);
            break;
        case 408:
            use_small_effect_pool = false;
            object = AllocateZeroedObject(332);
            if (object)
            {
                data.Require(12);
                created = new (object) BeamBarrier(data, idx, false);
            }
            use_small_effect_pool = true;
            break;
        case 409:
            object = AllocateZeroedObject(504);
            if (!object)
                return 0;
            created = new (object) FloatingHazard(data, idx);
            break;
        default:
            break;
    }
finish:
    if (!created)
        return 0;
    created->rec_idx = idx;
    if (clear_flag)
        created->lifecycle_flags &= 0xFFFD;
    if (special_flag)
        created->lifecycle_flags |= 4;
    if (!preserve_list)
    {
        if (&created->model == visual_lists.actors)
            list_head = &visual_lists.actors;
        if (&created->model == visual_lists.solids)
            list_head = &visual_lists.solids;
        if (&created->model == visual_lists.pickups)
            list_head = &visual_lists.pickups;
        if (!list_head)
        {
            GameDiagnostic(("Record object has no supported list head: index=%u type=%d\n", idx, type));
            abort();
        }
        created->MoveToTmpList(*list_head);
    }
    created->FlagNearRingAttack();
    return created;
}

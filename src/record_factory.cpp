#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "object_initializers.h"
#include "pickup_factory.h"
#include "pickup.h"
#include "game_diagnostics.h"
#include "visual_object.h"
#include "game_object_allocation.h"
#include "scripted_effect_controller.h"
#include "level_hazards.h"
#include "level_interactions.h"
#include "beam_trigger.h"
#include "beam_barrier.h"
#include "level_interactions.h"
#include "scripted_level_object.h"
#include "traversal_rail.h"
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
#include "record_scripts.h"
#include "record_commands.h"
#include "effect_lifecycle.h"
#include "record_data.h"

#include "record_factory.h"
#include "proximity_ring_emitter.h"
#include "activation_record.h"
#include "path_particle_emitter.h"

// Original800FF640 activates the record group before terminating
static const uint8 group_activation_script[4] = {3, 0, 255, 255};

void SpawnRecord(uint32 index)
{
    FUNCTION_MARKER(0x80064874, "SLUS_003.73");
    RecordCursor descriptor(GetLevelRecord(index));
    sint32 type = (sint16)(descriptor).Read16();
    RecordCursor record;
    GameHeapBlock *child;
    Vector3 position;
    uint32 parameters[9];
    uint32 first;
    uint32 count;
    if (type == 6)
    {
        ResourceScriptBorrow borrow;
        ResourceScript *script = FindScriptRecord(index);
        count = script->spawn_count;
        RecordCursor commands = script->script;
        script->spawn_count = count + 1;
        ExecuteRecordCommands(commands, index, 0);
        return;
    }
    if (type < 7)
    {
        if (type == 1 || type == 5)
        {
            CreateObjectFromRecord(index);
            return;
        }
        return;
    }
    if (type == 10)
    {
        SpawnPathParticleEmitter(index);
        return;
    }
    if (type < 11)
    {
        if (type == 7)
        {
            CreateObjectFromRecord(index);
            return;
        }
        return;
    }
    if (type != 501)
        return;
    record = ReadRecordPosition(position.words, index);
    use_small_effect_pool = false;
    child = AllocateEffectStorage(100);
    if (child)
    {
        parameters[0] = (record + 2).Read16();
        first = (record).Read16();
        parameters[1] = (record + 4).Read16();
        parameters[2] = (record + 6).Read16();
        parameters[3] = (record + 8)[0];
        parameters[4] = (record + 10)[0];
        parameters[5] = (record + 12)[0];
        parameters[6] = (record + 14)[0];
        parameters[7] = (record + 16)[0];
        parameters[8] = (record + 18)[0];
        new (child) ProximityRingEmitter(position.words, index, first, parameters[0], parameters[1], parameters[2], parameters[3], parameters[4], parameters[5], parameters[6], parameters[7], parameters[8]);
    }
    use_small_effect_pool = true;
    return;
}

uint32 InitializeLevelRecords()
{
    RecordDataBorrow record_borrow;
    uint32 index = 0;
    GameHeapBlock *object;
    Vector3 position;
    uint32 parameters[9];
    uint32 first;
    RecordCursor descriptor;
    RecordCursor payload;
    RecordCursor cursor;
    sint32 type;
    FUNCTION_MARKER(0x80065DE8, "SLUS_003.73");
    while ((sint32)index < (sint32)GetLevelRecordCount())
    {
        descriptor = GetLevelRecord(index);
        type = (sint16)(descriptor).Read16();
        if (type == 6 || type == 2 || type == 9)
        {
            payload = descriptor + 4 + (sint16)(descriptor + 2).Read16() * 2;
            payload = AlignRecordData(payload);
            RegisterResourceScript((payload).Read32(), index & 0xFFFF, type == 6 ? RecordCursor(payload + 4) : RecordCursor(group_activation_script, sizeof(group_activation_script)));
        }
        else if (type == 1 || type == 7)
        {
            payload = descriptor + 8 + (sint16)(descriptor + 6).Read16() * 2;
            if (RecordListContains(1, payload) & 0xFF)
            {
                if (type == 1)
                    CreateObjectFromRecord(index);
                else
                {
                    new ActivationRecord(index & 65535, 0xFFFFFFFF);
                }
            }
        }
        else if (type == 5)
        {
            cursor = ReadRecordPosition(position.words, index);
            if ((cursor).Read16() == 1)
            {
                new ActivationRecord(index & 65535, 0xFFFFFFFF);
            }
        }
        else if (type == 500)
        {
            cursor = ReadRecordPosition(position.words, index);
            use_small_effect_pool = false;
            object = AllocateEffectStorage(100);
            if (object)
            {
                parameters[0] = (cursor + 2).Read16();
                first = (cursor).Read16();
                parameters[1] = (cursor + 4).Read16();
                parameters[2] = (cursor + 6).Read16();
                parameters[3] = (cursor + 8)[0];
                parameters[4] = (cursor + 10)[0];
                parameters[5] = (cursor + 12)[0];
                parameters[6] = (cursor + 14)[0];
                parameters[7] = (cursor + 16)[0];
                parameters[8] = (cursor + 18)[0];
                new (object) ProximityRingEmitter(position.words, index, first, parameters[0], parameters[1], parameters[2], parameters[3], parameters[4], parameters[5], parameters[6], parameters[7], parameters[8]);
            }
            use_small_effect_pool = true;
        }
        ++index;
    }
    return 0;
}

VisualObject *CreateObjectFromRecord(uint32 index)
{
    RecordDataBorrow record_borrow;
    RecordCursor record;
    RecordCursor cursor;
    RecordCursor data;
    GameHeapBlock *object = 0;
    VisualObject *created = 0;
    ModelInstance **list_head = 0;
    uint32 preserve_list = 0, clear_flag = 0, special_flag = 0;
    sint32 type, kind;
    Vector3 position;
    FUNCTION_MARKER(0x800641E8, "SLUS_003.73");
    record = GetLevelRecord(index);
    type = (sint16)(record).Read16();
    cursor = record + 2;
    if (type == 5)
    {
        uint32 resource = (cursor).Read16();
        RecordCursor info(ReadRecordPosition(position.words, index));
        uint32 mode, lifetime;
        preserve_list = 1;
        special_flag = 1;
        mode = (info + 2).Read16();
        lifetime = (info + 4).Read16();
        mode = mode == 0 ? 4 : 0;
        created = ::SpawnPickup(resource, position.words, mode, lifetime, zero_vector.words);
        goto finish;
    }
    if (type == 7)
        special_flag = 1;
    else if (type != 1)
        return 0;
    kind = (sint16)(cursor).Read16();
    cursor += 2;
    cursor.Read16();
    cursor += 2;
    cursor += 2 + (sint16)(cursor).Read16() * 2;
    preserve_list = (uint8)RecordListContains(2, cursor);
    clear_flag = (uint8)RecordListContains(4, cursor);
    data = AlignRecordData(SkipRecordList(cursor));
    switch (kind)
    {
        case 200:
            object = AllocateZeroedObject(624);
            if (!object)
                return 0;
            {
                created = new (object) Swat(data, index);
            }
            break;
        case 201:
            object = AllocateZeroedObject(572);
            if (!object)
                return 0;
            {
                created = new (object) HovrSwat(data, index);
            }
            break;
        case 202:
            object = AllocateZeroedObject(544);
            if (!object)
                return 0;
            {
                created = new (object) Chopper(data, index);
            }
            break;
        case 203:
            object = AllocateZeroedObject(520);
            if (!object)
                return 0;
            {
                created = new (object) ScriptedEffectController(data, index);
            }
            break;
        case 204:
            object = AllocateZeroedObject(328);
            if (!object)
                return 0;
            {
                created = new (object) Items(data, index);
            }
            break;
        case 205:
            object = AllocateZeroedObject(576);
            if (!object)
                return 0;
            {
                created = new (object) Tank(data, index);
            }
            break;
        case 206:
            object = AllocateZeroedObject(556);
            if (!object)
                return 0;
            {
                created = new (object) Turret(data, index);
            }
            break;
        case 207:
            object = AllocateZeroedObject(528);
            if (!object)
                return 0;
            {
                created = new (object) MiniTank(data, index);
            }
            break;
        case 208:
            object = AllocateZeroedObject(496);
            if (!object)
                return 0;
            {
                created = new (object) HoundRat(data, index, 1);
            }
            break;
        case 209:
            object = AllocateZeroedObject(504);
            if (!object)
                return 0;
            {
                created = new (object) Bat(data, index);
            }
            break;
        case 210:
            object = AllocateZeroedObject(496);
            if (!object)
                return 0;
            {
                created = new (object) HoundRat(data, index, 0);
            }
            break;
        case 212:
            object = AllocateZeroedObject(548);
            if (!object)
                return 0;
            {
                created = new (object) Croc(data, index);
            }
            break;
        case 220:
            object = AllocateZeroedObject(576);
            if (!object)
                return 0;
            {
                created = new (object) TheBeast(data, index);
            }
            break;
        case 221:
            object = AllocateZeroedObject(568);
            if (!object)
                return 0;
            {
                created = new (object) War(data, index);
            }
            break;
        case 222:
            object = AllocateZeroedObject(684);
            if (!object)
                return 0;
            {
                created = new (object) Death(data, index);
            }
            break;
        case 223:
            object = AllocateZeroedObject(656);
            if (!object)
                return 0;
            {
                created = new (object) Plague(data, index);
            }
            break;
        case 400:
            use_small_effect_pool = false;
            object = AllocateZeroedObject(348);
            if (object)
            {
                data.Require(12);
                created = new (object) TraversalRail((const uint32 *)data.Get(), index);
            }
            use_small_effect_pool = true;
            break;
        case 401:
            use_small_effect_pool = false;
            object = AllocateZeroedObject(644);
            if (object)
            {
                data.Require(12);
                created = new (object) FlexibleTraversalRail((const uint32 *)data.Get(), index);
            }
            use_small_effect_pool = true;
            break;
        case 402:
            object = AllocateZeroedObject(540);
            if (!object)
                return 0;
            {
                created = new (object) ScriptedLevelObject(data, index);
            }
            break;
        case 403:
            object = AllocateZeroedObject(312);
            if (!object)
                return 0;
            {
                created = new (object) OneShotActivator(data, index);
            }
            break;
        case 404:
            use_small_effect_pool = false;
            object = AllocateZeroedObject(332);
            if (object)
            {
                data.Require(12);
                created = new (object) BeamBarrier((const uint32 *)data.Get(), index, 1);
            }
            use_small_effect_pool = true;
            break;
        case 405:
            use_small_effect_pool = false;
            object = AllocateZeroedObject(324);
            if (object)
            {
                created = new (object) BeamTrigger(0, index & 0xFFFF);
            }
            use_small_effect_pool = true;
            break;
        case 406:
            object = AllocateZeroedObject(336);
            if (!object)
                return 0;
            {
                created = new (object) TraversalRegion(data, index);
            }
            break;
        case 407:
            object = AllocateZeroedObject(336);
            if (!object)
                return 0;
            {
                created = new (object) ResettingActivator(data, index);
            }
            break;
        case 408:
            use_small_effect_pool = false;
            object = AllocateZeroedObject(332);
            if (object)
            {
                data.Require(12);
                created = new (object) BeamBarrier((const uint32 *)data.Get(), index, 0);
            }
            use_small_effect_pool = true;
            break;
        case 409:
            object = AllocateZeroedObject(504);
            if (!object)
                return 0;
            {
                created = new (object) FloatingHazard(data, index);
            }
            break;
        default:
            break;
    }
finish:
    if (!created)
        return 0;
    created->record_idx = index;
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
            GameDiagnostic("Record object has no supported list head: index=%u type=%d\n", index, type);
            abort();
        }
        created->MoveToTemporaryList(*list_head);
    }
    FlagObjectNearReference(*created);
    return created;
}

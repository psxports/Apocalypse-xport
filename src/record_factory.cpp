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
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "record_factory.h"
#include "proximity_ring_emitter.h"
#include "activation_record.h"
#include "path_particle_emitter.h"
#include "native_object_storage.h"
#include <new>

uint32 SpawnRecord(uint32 index)
{
    FUNCTION_MARKER(0x80064874, "SLUS_003.73");
    uint32 table = r_u32(0x800FF624);
    uint32 descriptor = r_u32(table + index * 4);
    sint32 type = (sint16)r_u16(descriptor);
    uint32 record, value, child, position[3], parameters[9], first;
    uint32 count;
    if (type == 6)
    {
        record = FindScriptRecord(index);
        count = r_u8(record + 7);
        value = r_u32(record);
        w_u8(record + 7, count + 1);
        return ExecuteRecordCommands(value, index, 0);
    }
    if (type < 7)
    {
        if (type == 1 || type == 5)
            return CreateObjectFromRecord(index);
        return 5;
    }
    if (type == 10)
        return SpawnPathParticleEmitter(index);
    if (type < 11)
    {
        if (type == 7)
            return CreateObjectFromRecord(index);
        return 7;
    }
    if (type != 501)
        return 501;
    record = ReadRecordPosition(position, index);
    w_u32(0x800FF3AC, 0);
    child = AllocateEffectStorage(100);
    if (child)
    {
        parameters[0] = r_u16(record + 2);
        first = r_u16(record);
        parameters[1] = r_u16(record + 4);
        parameters[2] = r_u16(record + 6);
        parameters[3] = r_u8(record + 8);
        parameters[4] = r_u8(record + 10);
        parameters[5] = r_u8(record + 12);
        parameters[6] = r_u8(record + 14);
        parameters[7] = r_u8(record + 16);
        parameters[8] = r_u8(record + 18);
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ProximityRingEmitter));
        new (dram_addr) ProximityRingEmitter(position, index, first, parameters[0], parameters[1], parameters[2], parameters[3], parameters[4], parameters[5], parameters[6], parameters[7], parameters[8]);
    }
    w_u32(0x800FF3AC, 1);
    return 1;
}



uint32 InitializeLevelRecords()
{
    uint32 index = 0, descriptor, payload, cursor, object, position[3], parameters[9], first;
    sint32 type;
    FUNCTION_MARKER(0x80065DE8, "SLUS_003.73");
    while ((sint32)index < (sint32)r_u32(0x800FF628))
    {
        descriptor = r_u32(r_u32(0x800FF624) + index * 4);
        type = (sint16)r_u16(descriptor);
        if (type == 6 || type == 2 || type == 9)
        {
            payload = descriptor + 4 + ((uint32)(sint32)(sint16)r_u16(descriptor + 2) << 1);
            if (payload & 2)
                payload += 2;
            RegisterResourceScript(r_u32(payload), index & 0xFFFF, type == 6 ? payload + 4 : 0x800FF640);
        }
        else if (type == 1 || type == 7)
        {
            payload = descriptor + 8 + ((uint32)(sint32)(sint16)r_u16(descriptor + 6) << 1);
            if (RecordListContains(1, payload) & 0xFF)
            {
                if (type == 1)
                    CreateObjectFromRecord(index);
                else
                {
                    object = sub_8002FED8(24);
                    if (object)
                    {
                        void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ActivationRecord));
                        new (dram_addr) ActivationRecord(index & 65535, 0xFFFFFFFF);
                    }
                }
            }
        }
        else if (type == 5)
        {
            cursor = ReadRecordPosition(position, index);
            if (r_u16(cursor) == 1)
            {
                object = sub_8002FED8(24);
                if (object)
                {
                    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ActivationRecord));
                    new (dram_addr) ActivationRecord(index & 65535, 0xFFFFFFFF);
                }
            }
        }
        else if (type == 500)
        {
            cursor = ReadRecordPosition(position, index);
            w_u32(0x800FF3AC, 0);
            object = AllocateEffectStorage(100);
            if (object)
            {
                parameters[0] = r_u16(cursor + 2);
                first = r_u16(cursor);
                parameters[1] = r_u16(cursor + 4);
                parameters[2] = r_u16(cursor + 6);
                parameters[3] = r_u8(cursor + 8);
                parameters[4] = r_u8(cursor + 10);
                parameters[5] = r_u8(cursor + 12);
                parameters[6] = r_u8(cursor + 14);
                parameters[7] = r_u8(cursor + 16);
                parameters[8] = r_u8(cursor + 18);
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ProximityRingEmitter));
                new (dram_addr) ProximityRingEmitter(position, index, first, parameters[0], parameters[1], parameters[2], parameters[3], parameters[4], parameters[5], parameters[6], parameters[7], parameters[8]);
            }
            w_u32(0x800FF3AC, 1);
        }
        ++index;
    }
    return 0;
}



uint32 CreateObjectFromRecord(uint32 index)
{
    uint32 record, cursor, data, object = 0;
    ModelInstance **list_head = 0;
    uint32 preserve_list = 0, clear_flag = 0, special_flag = 0;
    uint16 tag = 0xFFFF;
    sint32 type, kind;
    uint32 position[3];
    FUNCTION_MARKER(0x800641E8, "SLUS_003.73");
    record = r_u32(r_u32(0x800FF624) + index * 4);
    type = (sint16)r_u16(record);
    cursor = record + 2;
    if (type == 5)
    {
        uint32 resource = r_u16(cursor);
        uint32 info = ReadRecordPosition(position, index);
        uint32 mode, yaw;
        preserve_list = 1;
        special_flag = 1;
        mode = r_u16(info + 2);
        yaw = r_u16(info + 4);
        mode = mode == 0 ? 4 : 0;
        object = xport_draft_host_sub_80020EF8_p2(resource, position, mode, yaw, 0x800A71CC);
        goto finish;
    }
    if (type == 7)
        special_flag = 1;
    else if (type != 1)
        return 0;
    kind = (sint16)r_u16(cursor);
    cursor += 2;
    tag = r_u16(cursor);
    cursor += 2;
    cursor += 2 + (uint32)(sint32)(sint16)r_u16(cursor) * 2;
    preserve_list = (uint8)RecordListContains(2, cursor);
    clear_flag = (uint8)RecordListContains(4, cursor);
    data = (SkipRecordList(cursor) + 3) & 0xFFFFFFFC;
    switch (kind)
    {
        case 200:
            object = AllocateZeroedObject(624);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Swat));
                new (dram_addr) Swat((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 201:
            object = AllocateZeroedObject(572);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(HovrSwat));
                new (dram_addr) HovrSwat((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 202:
            object = AllocateZeroedObject(544);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Chopper));
                new (dram_addr) Chopper((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 203:
            object = AllocateZeroedObject(520);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ScriptedEffectController));
                new (dram_addr) ScriptedEffectController((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 204:
            object = AllocateZeroedObject(328);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Items));
                new (dram_addr) Items((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 205:
            object = AllocateZeroedObject(576);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Tank));
                new (dram_addr) Tank((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 206:
            object = AllocateZeroedObject(556);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Turret));
                new (dram_addr) Turret((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 207:
            object = AllocateZeroedObject(528);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(MiniTank));
                new (dram_addr) MiniTank((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 208:
            object = AllocateZeroedObject(496);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(HoundRat));
                new (dram_addr) HoundRat((const uint8 *)psx_addr(data, 1), index, 1);
            }
            break;
        case 209:
            object = AllocateZeroedObject(504);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Bat));
                new (dram_addr) Bat((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 210:
            object = AllocateZeroedObject(496);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(HoundRat));
                new (dram_addr) HoundRat((const uint8 *)psx_addr(data, 1), index, 0);
            }
            break;
        case 212:
            object = AllocateZeroedObject(548);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Croc));
                new (dram_addr) Croc((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 220:
            object = AllocateZeroedObject(576);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(TheBeast));
                new (dram_addr) TheBeast((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 221:
            object = AllocateZeroedObject(568);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(War));
                new (dram_addr) War((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 222:
            object = AllocateZeroedObject(684);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Death));
                new (dram_addr) Death((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 223:
            object = AllocateZeroedObject(656);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(Plague));
                new (dram_addr) Plague((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 400:
            w_u32(0x800FF3AC, 0);
            object = AllocateZeroedObject(348);
            if (object)
            {
                {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(TraversalRail));
                new (dram_addr) TraversalRail((const uint32 *)psx_addr(data, 12), index);
            }
            }
            w_u32(0x800FF3AC, 1);
            break;
        case 401:
            w_u32(0x800FF3AC, 0);
            object = AllocateZeroedObject(644);
            if (object)
            {
                {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(FlexibleTraversalRail));
                new (dram_addr) FlexibleTraversalRail((const uint32 *)psx_addr(data, 12), index);
            }
            }
            w_u32(0x800FF3AC, 1);
            break;
        case 402:
            object = AllocateZeroedObject(540);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ScriptedLevelObject));
                new (dram_addr) ScriptedLevelObject((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 403:
            object = AllocateZeroedObject(312);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(OneShotActivator));
                new (dram_addr) OneShotActivator((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 404:
            w_u32(0x800FF3AC, 0);
            object = AllocateZeroedObject(332);
            if (object)
            {
                {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(BeamBarrier));
                new (dram_addr) BeamBarrier((const uint32 *)psx_addr(data, 12), index, 1);
            }
            }
            w_u32(0x800FF3AC, 1);
            break;
        case 405:
            w_u32(0x800FF3AC, 0);
            object = AllocateZeroedObject(324);
            if (object)
            {
                {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(BeamTrigger));
                new (dram_addr) BeamTrigger(0, index & 0xFFFF);
            }
            }
            w_u32(0x800FF3AC, 1);
            break;
        case 406:
            object = AllocateZeroedObject(336);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(TraversalRegion));
                new (dram_addr) TraversalRegion((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 407:
            object = AllocateZeroedObject(336);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ResettingActivator));
                new (dram_addr) ResettingActivator((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        case 408:
            w_u32(0x800FF3AC, 0);
            object = AllocateZeroedObject(332);
            if (object)
            {
                {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(BeamBarrier));
                new (dram_addr) BeamBarrier((const uint32 *)psx_addr(data, 12), index, 0);
            }
            }
            w_u32(0x800FF3AC, 1);
            break;
        case 409:
            object = AllocateZeroedObject(504);
            if (!object)
                return 0;
            {
                void *dram_addr = AllocateNativeObjectStorage(object, sizeof(FloatingHazard));
                new (dram_addr) FloatingHazard((const uint8 *)psx_addr(data, 1), index);
            }
            break;
        default:
            break;
    }
finish:
    if (!object)
        return 0;
    VisualObject &visual = VisualObject::Attach(object);
    visual.record_idx = index;
    w_u16(object + 170, tag);
    if (clear_flag)
        visual.lifecycle_flags &= 0xFFFD;
    if (special_flag)
        visual.lifecycle_flags |= 4;
    if (!preserve_list)
    {
        if (&VisualObject::Attach(object).model == visual_lists.actors)
            list_head = &visual_lists.actors;
        if (&VisualObject::Attach(object).model == visual_lists.solids)
            list_head = &visual_lists.solids;
        if (&VisualObject::Attach(object).model == visual_lists.pickups)
            list_head = &visual_lists.pickups;
        VisualObject::Attach(object).VisualObject::MoveToTemporaryList(*list_head);
    }
    FlagObjectNearReference(object);
    return object;
}



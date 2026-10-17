#include "visual_object.h"
#include "game_sdk_bindings.h"
#include "audio_session.h"
#include "game_object_allocation.h"
#include "vector_distance.h"
#include "model_resources.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "enemy_list.h"
#include "record_data.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "level_interactions.h"
#include "native_object_storage.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

// Native views retain the original record field widths

#include "level_interactions.h"

// Native bridge to the original mapped record parser

static uint32 ReadOneShotActivatorHalf(const uint8 *data)
{
    uint32 address = GuestMemoryAddress(data);
    if (address)
        return r_u16(address);
    return (uint32)data[0] | ((uint32)data[1] << 8);
}

static uint32 ReadOneShotActivatorWord(const uint8 *data)
{
    uint32 address = GuestMemoryAddress(data);
    if (address)
        return r_u32(address);
    return (uint32)data[0] | ((uint32)data[1] << 8) | ((uint32)data[2] << 16) | ((uint32)data[3] << 24);
}

OneShotActivator &OneShotActivator::Attach(uint32 object)
{
    return *static_cast<OneShotActivator *>(FindNativeObjectStorage(object));
}

static uint32 RecordInteractionDataAddress(const uint8 *data)
{
    uint32 address = GuestMemoryAddress(data);
    if (!address)
    {
        // TODO Bind native record data without inventing parser storage
        GameDiagnostic("TODO Native interaction record data\n");
        abort();
    }
    return address;
}

TraversalRegion &TraversalRegion::Attach(uint32 object)
{
    return *static_cast<TraversalRegion *>(FindNativeObjectStorage(object));
}

ResettingActivator &ResettingActivator::Attach(uint32 object)
{
    return *static_cast<ResettingActivator *>(FindNativeObjectStorage(object));
}

OneShotActivator::OneShotActivator(const uint8 *record_data, uint32 index) : VisualObject()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 bank, angle, model, key, flags;
    const uint8 *cursor;
    FUNCTION_MARKER(0x80061BD4, "SLUS_003.73");
    bank = r_u32(0x800FF62C);
    w_u32(object + 68, 0x800A3264);
    SetModelBank((const char *)psx_addr(bank, 1));
    AddToList(visual_lists.actors);
    cursor = ReadPosition(record_data);
    angle = ReadOneShotActivatorHalf(cursor);
    model = this->model.bank;
    this->model.angles[0] = angle;
    angle = ReadOneShotActivatorHalf(cursor + 2);
    this->model.angles[1] = angle;
    angle = ReadOneShotActivatorHalf(cursor + 4);
    this->model.angles[2] = angle;
    uint32 cursor_address = GuestMemoryAddress(cursor);
    if (cursor_address)
        key = r_u32((cursor_address + 9) & ~3);
    else
    {
        uint32 alignment = (uint32)((uintptr_t)(cursor + 9) & 3);
        key = ReadOneShotActivatorWord(cursor + 9 - alignment);
    }
    this->model.model_idx = FindModelResourceIndex(key, model);
    uint32 *offset = (uint32 *)psx_addr(object + 296, 12);
    BuildDirectionVector(offset, (uint32)-96, (const uint16 *)this->model.angles);
    AddVectors(offset, (const uint32 *)this->model.pos);
    flags = this->lifecycle_flags;
    w_u16(object + 212, 0);
    w_u32(object + 308, 0);
    this->record_idx = index;
    this->lifecycle_flags = flags & 0xFFEF;
}

uint32 OneShotActivator::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80061CBC, "SLUS_003.73");
    w_u32(object + 68, 0x800A3264);
    RemoveFromList(visual_lists.actors);
    result = VisualObject::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(object);
    return result;
}

uint32 OneShotActivator::Update()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result, player;
    FUNCTION_MARKER(0x80061D20, "SLUS_003.73");
    result = r_u32(object + 308);
    if (result)
        return result;
    player = visual_lists.players;
    if (!player)
        return result;
    result = (sint32)MeasurePositionDistance((const uint32 *)psx_addr(player + 4, 12), (const uint32 *)psx_addr(object + 296, 12)) < 192;
    if (result)
    {
        result = visual_lists.players;
        w_u32(result + 516, object);
    }
    return result;
}

uint32 OneShotActivator::Activate()
{
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80061D80, "SLUS_003.73");
    w_u32(object + 308, 1);
    return 1;
}

uint32 OneShotActivator::SpawnObjects()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index, list;
    FUNCTION_MARKER(0x80061D8C, "SLUS_003.73");
    index = this->record_idx;
    list = GetRecordPayload(index);
    SpawnEnemyIndices((const uint16 *)psx_addr(list, 2));
    return PlaySoundEffect(47, 0x2000, 0);
}

ResettingActivator::ResettingActivator(const uint8 *record_data, uint32 index) : VisualObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    RecordInteractionDataAddress(record_data);
    uint32 bank, cursor, x, y, z, value, data, flags;
    FUNCTION_MARKER(0x80061DC8, "SLUS_003.73");
    bank = r_u32(0x800FF62C);
    w_u32(object + 68, 0x800A3224);
    SetModelBank((const char *)psx_addr(bank, 1));
    AddToList(visual_lists.actors);
    cursor = GuestMemoryAddress(ReadPosition(record_data));
    x = this->model.pos[0];
    y = this->model.pos[1];
    z = this->model.pos[2];
    w_u32(object + 296, x);
    w_u32(object + 300, y);
    w_u32(object + 304, z);
    value = r_u16(cursor);
    bank = this->model.bank;
    this->model.angles[0] = value;
    this->model.angles[1] = r_u16(cursor + 2);
    this->model.angles[2] = r_u16(cursor + 4);
    data = (cursor + 9) & 0xFFFFFFFC;
    value = r_u32(data);
    w_u32(object + 332, value);
    value = ::FindModelResourceIndex(value, bank);
    data = (cursor + 13) & 0xFFFFFFFC;
    this->model.model_idx = value;
    value = r_u32(data);
    w_u32(object + 328, value);
    sub_800667CC(object + 308, (uint32)-128, object + 16);
    sub_8006C0B8(object + 308, object + 4);
    flags = this->lifecycle_flags;
    w_u16(object + 212, 0);
    w_u32(object + 320, 0);
    this->record_idx = index;
    this->lifecycle_flags = flags & 65519;
}

uint32 ResettingActivator::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80061EF0, "SLUS_003.73");
    w_u32(object + 68, 0x800A3224);
    RemoveFromList(visual_lists.actors);
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : result;
}

uint32 ResettingActivator::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result, value, player, x, y, z, resource, bank;
    FUNCTION_MARKER(0x80061F54, "SLUS_003.73");
    if (this->activation_flags & 1)
    {
        BeginActivation();
        Activate();
        this->activation_flags = this->activation_flags & 65534;
    }
    result = r_u32(object + 320);
    if (result)
    {
        value = r_u32(object + 324) + 1;
        w_u32(object + 324, value);
        result = (sint32)value < 129;
        if (!result)
        {
            w_u32(object + 320, 0);
            x = r_u32(object + 296);
            y = r_u32(object + 300);
            z = r_u32(object + 304);
            this->model.pos[0] = x;
            this->model.pos[1] = y;
            this->model.pos[2] = z;
            resource = r_u32(object + 332);
            bank = this->model.bank;
            result = ::FindModelResourceIndex(resource, bank);
            this->model.model_idx = result;
        }
    }
    else
    {
        player = visual_lists.players;
        result = player;
        if (player)
        {
            result = (sint32)sub_8006696C(player + 4, object + 308) < 192;
            if (result)
            {
                result = visual_lists.players;
                w_u32(result + 520, object);
            }
        }
    }
    return result;
}

uint32 ResettingActivator::BeginActivation()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80062034, "SLUS_003.73");
    w_u32(object + 320, 1);
    w_u32(object + 324, 0);
    return 1;
}

uint32 ResettingActivator::Activate()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 descriptor, resource, bank, result;
    uint32 offset[4];
    FUNCTION_MARKER(0x80062044, "SLUS_003.73");
    descriptor = sub_80066088(this->record_idx);
    SpawnEnemyIndices((const uint16 *)psx_addr(descriptor, 2));
    sub_80069DF0(47, 8192, 0);
    xport_draft_host_sub_800667CC_p1(offset, 16, object + 16);
    xport_draft_host_sub_8006C0B8_p2(object + 4, offset);
    resource = r_u32(object + 328);
    bank = this->model.bank;
    result = ::FindModelResourceIndex(resource, bank);
    this->model.model_idx = result;
    return result;
}

TraversalRegion::TraversalRegion(const uint8 *record_data, uint32 index) : VisualObject()
{
    const uint32 object = NativeObjectGuestAddress(this);
    RecordInteractionDataAddress(record_data);
    FUNCTION_MARKER(0x800620B8, "SLUS_003.73");
    uint32 data, words, descriptor, delta_z, delta_x, value;
    sint32 heading, current;
    w_u32(object + 68, 0x800A31E4);
    value = this->lifecycle_flags;
    current = this->model.flags;
    w_u16(object + 212, 0);
    this->record_idx = index;
    this->lifecycle_flags = value & 0xFFEF;
    this->model.flags = (uint32)current | 1;
    AddToList(visual_lists.actors);
    data = GuestMemoryAddress(ReadPosition(record_data));
    this->model.angles[0] = r_u16(data);
    this->model.angles[1] = r_u16(data + 2);
    this->model.angles[2] = r_u16(data + 4);
    descriptor = sub_80066088(index);
    sub_8006613C(object + 324, r_u16(descriptor + 2));
    words = (data + 9) & 0xFFFFFFFC;
    value = r_u32(words);
    delta_z = r_u32(object + 332) - this->model.pos[2];
    delta_x = r_u32(object + 324);
    w_u32(object + 296, value << 12);
    value = r_u32(words + 4);
    delta_x -= this->model.pos[0];
    w_u32(object + 300, value << 12);
    w_u32(object + 304, r_u32(words + 8) << 12);
    w_u32(object + 308, r_u32(words + 12) << 12);
    w_u32(object + 312, r_u32(words + 16) << 12);
    w_u32(object + 316, r_u32(words + 20) << 12);
    heading = (3072 - (uint32)ratan2((sint32)delta_z >> 12, (sint32)delta_x >> 12)) & 0xFFF;
    current = (sint16)this->model.angles[1];
    if (current < heading)
        w_u8(object + 320, heading - current < 2048 ? 1 : 0);
    else if (heading < current)
        w_u8(object + 320, current - heading >= 2048 ? 1 : 0);
}

uint32 TraversalRegion::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 player;
    FUNCTION_MARKER(0x80062254, "SLUS_003.73");
    w_u32(object + 68, 0x800A31E4);
    RemoveFromList(visual_lists.actors);
    player = visual_lists.players;
    if (player && r_u32(player + 524) == object)
        w_u32(player + 524, 0);
    VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

uint32 TraversalRegion::Update()
{
    FUNCTION_MARKER(0x800622D8, "SLUS_003.73");
    return 0;
}



extern "C" uint32 sub_80061CBC(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80061CBC, "SLUS_003.73");
    return OneShotActivator::Attach(object).OneShotActivator::Destroy(reason);
}

extern "C" uint32 sub_80061D20(uint32 object)
{
    FUNCTION_MARKER(0x80061D20, "SLUS_003.73");
    return OneShotActivator::Attach(object).OneShotActivator::Update();
}







extern "C" uint32 sub_80062254(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80062254, "SLUS_003.73");
    return TraversalRegion::Attach(object).TraversalRegion::Destroy(reason);
}

extern "C" void sub_800622D8(uint32 object)
{
    FUNCTION_MARKER(0x800622D8, "SLUS_003.73");
    TraversalRegion::Attach(object).TraversalRegion::Update();
}



extern "C" uint32 sub_80061EF0(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x80061EF0, "SLUS_003.73");
    return ResettingActivator::Attach(object).ResettingActivator::Destroy(reason);
}

extern "C" uint32 sub_80061F54(uint32 object)
{
    FUNCTION_MARKER(0x80061F54, "SLUS_003.73");
    return ResettingActivator::Attach(object).ResettingActivator::Update();
}



extern "C" uint32 sub_80062044(uint32 object)
{
    FUNCTION_MARKER(0x80062044, "SLUS_003.73");
    return ResettingActivator::Attach(object).ResettingActivator::Activate();
}

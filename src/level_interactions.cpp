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
    AddToList((uint32 *)psx_addr(0x800FF4E8, 4));
    cursor = ReadPosition(record_data);
    angle = ReadOneShotActivatorHalf(cursor);
    model = r_u8(object + 27);
    w_u16(object + 16, angle);
    angle = ReadOneShotActivatorHalf(cursor + 2);
    w_u16(object + 18, angle);
    angle = ReadOneShotActivatorHalf(cursor + 4);
    w_u16(object + 20, angle);
    uint32 cursor_address = GuestMemoryAddress(cursor);
    if (cursor_address)
        key = r_u32((cursor_address + 9) & ~3);
    else
    {
        uint32 alignment = (uint32)((uintptr_t)(cursor + 9) & 3);
        key = ReadOneShotActivatorWord(cursor + 9 - alignment);
    }
    w_u16(object + 22, FindModelResourceIndex(key, model));
    uint32 *offset = (uint32 *)psx_addr(object + 296, 12);
    BuildDirectionVector(offset, (uint32)-96, (const uint16 *)psx_addr(object + 16, 6));
    AddVectors(offset, (const uint32 *)psx_addr(object + 4, 12));
    flags = r_u16(object + 78);
    w_u16(object + 212, 0);
    w_u32(object + 308, 0);
    w_u16(object + 214, index);
    w_u16(object + 78, flags & 0xFFEF);
}

uint32 OneShotActivator::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80061CBC, "SLUS_003.73");
    w_u32(object + 68, 0x800A3264);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
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
    player = r_u32(0x800FF5A0);
    if (!player)
        return result;
    result = (sint32)MeasurePositionDistance((const uint32 *)psx_addr(player + 4, 12), (const uint32 *)psx_addr(object + 296, 12)) < 192;
    if (result)
    {
        result = r_u32(0x800FF5A0);
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
    index = r_u16(object + 214);
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
    AddToList((uint32 *)psx_addr(0x800FF4E8, 4));
    cursor = GuestMemoryAddress(ReadPosition(record_data));
    x = r_u32(object + 4);
    y = r_u32(object + 8);
    z = r_u32(object + 12);
    w_u32(object + 296, x);
    w_u32(object + 300, y);
    w_u32(object + 304, z);
    value = r_u16(cursor);
    bank = r_u8(object + 27);
    w_u16(object + 16, value);
    w_u16(object + 18, r_u16(cursor + 2));
    w_u16(object + 20, r_u16(cursor + 4));
    data = (cursor + 9) & 0xFFFFFFFC;
    value = r_u32(data);
    w_u32(object + 332, value);
    value = sub_8006E080(value, bank);
    data = (cursor + 13) & 0xFFFFFFFC;
    w_u16(object + 22, value);
    value = r_u32(data);
    w_u32(object + 328, value);
    sub_800667CC(object + 308, (uint32)-128, object + 16);
    sub_8006C0B8(object + 308, object + 4);
    flags = r_u16(object + 78);
    w_u16(object + 212, 0);
    w_u32(object + 320, 0);
    w_u16(object + 214, index);
    w_u16(object + 78, flags & 65519);
}

uint32 ResettingActivator::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x80061EF0, "SLUS_003.73");
    w_u32(object + 68, 0x800A3224);
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : result;
}

uint32 ResettingActivator::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result, value, player, x, y, z, resource, bank;
    FUNCTION_MARKER(0x80061F54, "SLUS_003.73");
    if (r_u16(object + 76) & 1)
    {
        BeginActivation();
        Activate();
        w_u16(object + 76, r_u16(object + 76) & 65534);
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
            w_u32(object + 4, x);
            w_u32(object + 8, y);
            w_u32(object + 12, z);
            resource = r_u32(object + 332);
            bank = r_u8(object + 27);
            result = sub_8006E080(resource, bank);
            w_u16(object + 22, result);
        }
    }
    else
    {
        player = r_u32(0x800FF5A0);
        result = player;
        if (player)
        {
            result = (sint32)sub_8006696C(player + 4, object + 308) < 192;
            if (result)
            {
                result = r_u32(0x800FF5A0);
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
    descriptor = sub_80066088(r_u16(object + 214));
    SpawnEnemyIndices((const uint16 *)psx_addr(descriptor, 2));
    sub_80069DF0(47, 8192, 0);
    xport_draft_host_sub_800667CC_p1(offset, 16, object + 16);
    xport_draft_host_sub_8006C0B8_p2(object + 4, offset);
    resource = r_u32(object + 328);
    bank = r_u8(object + 27);
    result = sub_8006E080(resource, bank);
    w_u16(object + 22, result);
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
    value = r_u16(object + 78);
    current = r_u16(object);
    w_u16(object + 212, 0);
    w_u16(object + 214, index);
    w_u16(object + 78, value & 0xFFEF);
    w_u16(object, (uint32)current | 1);
    AddToList((uint32 *)psx_addr(0x800FF4E8, 4));
    data = GuestMemoryAddress(ReadPosition(record_data));
    w_u16(object + 16, r_u16(data));
    w_u16(object + 18, r_u16(data + 2));
    w_u16(object + 20, r_u16(data + 4));
    descriptor = sub_80066088(index);
    sub_8006613C(object + 324, r_u16(descriptor + 2));
    words = (data + 9) & 0xFFFFFFFC;
    value = r_u32(words);
    delta_z = r_u32(object + 332) - r_u32(object + 12);
    delta_x = r_u32(object + 324);
    w_u32(object + 296, value << 12);
    value = r_u32(words + 4);
    delta_x -= r_u32(object + 4);
    w_u32(object + 300, value << 12);
    w_u32(object + 304, r_u32(words + 8) << 12);
    w_u32(object + 308, r_u32(words + 12) << 12);
    w_u32(object + 312, r_u32(words + 16) << 12);
    w_u32(object + 316, r_u32(words + 20) << 12);
    heading = (3072 - (uint32)ratan2((sint32)delta_z >> 12, (sint32)delta_x >> 12)) & 0xFFF;
    current = (sint16)r_u16(object + 18);
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
    RemoveFromList((uint32 *)psx_addr(0x800FF4E8, 4));
    player = r_u32(0x800FF5A0);
    if (player && r_u32(player + 524) == object)
        w_u32(player + 524, 0);
    VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

void TraversalRegion::Update()
{
    FUNCTION_MARKER(0x800622D8, "SLUS_003.73");
}

extern "C" uint32 sub_80061BD4(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x80061BD4, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(OneShotActivator));
    new (storage) OneShotActivator((const uint8 *)psx_addr(data, 1), index);
    return object;
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

extern "C" uint32 sub_80061D80(uint32 object)
{
    FUNCTION_MARKER(0x80061D80, "SLUS_003.73");
    return OneShotActivator::Attach(object).Activate();
}

extern "C" uint32 sub_80061D8C(uint32 object)
{
    FUNCTION_MARKER(0x80061D8C, "SLUS_003.73");
    return OneShotActivator::Attach(object).SpawnObjects();
}

extern "C" uint32 sub_800620B8(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x800620B8, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(TraversalRegion));
    new (dram_addr) TraversalRegion((const uint8 *)psx_addr(data, 1), index);
    return object;
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

extern "C" uint32 sub_80061DC8(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x80061DC8, "SLUS_003.73");
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(ResettingActivator));
    new (dram_addr) ResettingActivator((const uint8 *)psx_addr(data, 1), index);
    return object;
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

extern "C" uint32 sub_80062034(uint32 object)
{
    FUNCTION_MARKER(0x80062034, "SLUS_003.73");
    return ResettingActivator::Attach(object).ResettingActivator::BeginActivation();
}

extern "C" uint32 sub_80062044(uint32 object)
{
    FUNCTION_MARKER(0x80062044, "SLUS_003.73");
    return ResettingActivator::Attach(object).ResettingActivator::Activate();
}

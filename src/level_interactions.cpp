#include <new>
#include <stdio.h>
#include <stdlib.h>

#include "record_scripts.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "audio_session.h"
#include "game_object_allocation.h"
#include "vector_distance.h"
#include "model_resources.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "enemy_list.h"
#include "record_data.h"

#include "level_interactions.h"
#include "player.h"

OneShotActivator::OneShotActivator(const RecordCursor &record_data, uint32 index) : VisualObject(), activated(0)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x80061BD4, "SLUS_003.73");
    SetModelBank(GetRecordModelBankName());
    AddToList(visual_lists.actors);
    RecordCursor cursor(ReadPosition(record_data));
    for (uint32 axis = 0; axis < 3; ++axis)
        model.angles.words[axis] = (cursor + axis * 2).Read16();
    uint32 key = (AlignRecordData(cursor + 6)).Read32();
    model.model_idx = FindModelResourceIndex(key, model.bank);
    BuildDirectionVector(activation_pos.words, (uint32)-96, model.angles.words);
    AddVectors(activation_pos.words, model.pos.words);
    collision_radius = 0;
    record_idx = index;
    lifecycle_flags &= 0xFFEF;
}

uint32 OneShotActivator::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80061CBC, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    result = VisualObject::Destroy(0);
    if (reason & 1)
        result = ReleaseObjectStorage(this);
    return result;
}

uint32 OneShotActivator::Update()
{
    FUNCTION_MARKER(0x80061D20, "SLUS_003.73");
    if (activated)
        return activated;
    if (!visual_lists.players)
        return 0;
    Player &player = *static_cast<Player *>(visual_lists.players->owner);
    uint32 near = (sint32)MeasurePositionDistance(player.model.pos.words, activation_pos.words) < 192;
    if (near)
        player.one_shot_activator = this;
    return near;
}

uint32 OneShotActivator::Activate()
{
    FUNCTION_MARKER(0x80061D80, "SLUS_003.73");
    activated = 1;
    return 1;
}

uint32 OneShotActivator::SpawnObjects()
{
    RecordDataBorrow record_borrow;
    ObjectBorrow borrow;
    uint32 index;
    RecordCursor list;
    FUNCTION_MARKER(0x80061D8C, "SLUS_003.73");
    index = this->record_idx;
    list = GetRecordPayload(index);
    SpawnEnemyIndices(list);
    return PlaySoundEffect(47, 0x2000, 0);
}

ResettingActivator::ResettingActivator(const RecordCursor &record_data, uint32 index) : VisualObject(), activated(0), timer(0), active_model(0), idle_model(0)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x80061DC8, "SLUS_003.73");
    SetModelBank(GetRecordModelBankName());
    AddToList(visual_lists.actors);
    RecordCursor cursor(ReadPosition(record_data));
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        rest_pos.words[axis] = model.pos.words[axis];
        model.angles.words[axis] = (cursor + axis * 2).Read16();
    }
    const uint8 *keys = AlignRecordData(cursor + 6);
    idle_model = xport_load_le32(keys);
    model.model_idx = FindModelResourceIndex(idle_model, model.bank);
    active_model = xport_load_le32(keys + 4);
    BuildDirectionVector(activation_pos.words, (uint32)-128, model.angles.words);
    AddVectors(activation_pos.words, model.pos.words);
    collision_radius = 0;
    record_idx = index;
    lifecycle_flags &= 0xFFEF;
}

uint32 ResettingActivator::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80061EF0, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : result;
}

uint32 ResettingActivator::Update()
{
    ObjectBorrow borrow;
    FUNCTION_MARKER(0x80061F54, "SLUS_003.73");
    if (activation_flags & 1)
    {
        BeginActivation();
        Activate();
        activation_flags &= 65534;
    }
    if (activated)
    {
        ++timer;
        if ((sint32)timer < 129)
            return 1;
        activated = 0;
        for (uint32 axis = 0; axis < 3; ++axis)
            model.pos.words[axis] = rest_pos.words[axis];
        uint32 result = FindModelResourceIndex(idle_model, model.bank);
        model.model_idx = result;
        return result;
    }
    if (!visual_lists.players)
        return 0;
    Player &player = *static_cast<Player *>(visual_lists.players->owner);
    uint32 near = (sint32)MeasurePositionDistance(player.model.pos.words, activation_pos.words) < 192;
    if (near)
        player.resetting_activator = this;
    return near;
}

uint32 ResettingActivator::BeginActivation()
{
    FUNCTION_MARKER(0x80062034, "SLUS_003.73");
    activated = 1;
    timer = 0;
    return 1;
}

uint32 ResettingActivator::Activate()
{
    RecordDataBorrow record_borrow;
    ObjectBorrow borrow;
    Vector3 offset;
    FUNCTION_MARKER(0x80062044, "SLUS_003.73");
    SpawnEnemyIndices(GetRecordPayload(record_idx));
    PlaySoundEffect(47, 8192, 0);
    BuildDirectionVector(offset.words, 16, model.angles.words);
    AddVectors(model.pos.words, offset.words);
    uint32 result = FindModelResourceIndex(active_model, model.bank);
    model.model_idx = result;
    return result;
}

TraversalRegion::TraversalRegion(const RecordCursor &record_data, uint32 index) : VisualObject(), reverse(0)
{
    RecordDataBorrow record_borrow;
    FUNCTION_MARKER(0x800620B8, "SLUS_003.73");
    collision_radius = 0;
    record_idx = index;
    lifecycle_flags &= 0xFFEF;
    model.flags |= 1;
    AddToList(visual_lists.actors);
    RecordCursor cursor(ReadPosition(record_data));
    for (uint32 axis = 0; axis < 3; ++axis)
        model.angles.words[axis] = (cursor + axis * 2).Read16();
    RecordCursor descriptor(GetRecordPayload(index));
    ReadRecordPosition(target.words, (descriptor + 2).Read16());
    RecordCursor words(AlignRecordData(cursor + 6));
    uint32 delta_z = target.words[2] - model.pos.words[2];
    uint32 delta_x = target.words[0] - model.pos.words[0];
    for (uint32 corner = 0; corner < 2; ++corner)
        for (uint32 axis = 0; axis < 3; ++axis)
            bounds[corner].words[axis] = (words + (corner * 3 + axis) * 4).Read32() << 12;
    sint32 heading = (3072 - (uint32)ratan2((sint32)delta_z >> 12, (sint32)delta_x >> 12)) & 4095;
    sint32 current = (sint16)model.angles.words[1];
    if (current < heading)
        reverse = heading - current < 2048;
    else if (heading < current)
        reverse = current - heading >= 2048;
}

uint32 TraversalRegion::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80062254, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    if (visual_lists.players)
    {
        Player &player = *static_cast<Player *>(visual_lists.players->owner);
        if (player.traversal_region.Get() == this)
            player.traversal_region = 0;
    }
    VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(this) : 0;
}

uint32 TraversalRegion::Update()
{
    FUNCTION_MARKER(0x800622D8, "SLUS_003.73");
    return 0;
}

#include "resource_script.h"
#include "visual_object.h"
#include "audio_session.h"
#include "model_bank.h"
#include "enemy_list.h"
#include "record_data.h"
#include "resetting_activator.h"
#include "player.h"

ResettingActivator::ResettingActivator(const RecordCursor &rec_data, uint16 idx) : VisualObject(), activated(false), timer(0), active_model_key(0), idle_model_key(0)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x80061DC8, "SLUS_003.73");
    SetModelBank(GetRecModelBankName());
    AddToList(visual_lists.actors);
    RecordCursor cursor(ReadPos(rec_data));
    rest_pos = model.pos;
    for (uint32 axis = 0; axis < 3; ++axis)
        model.angles.comp[axis] = (cursor + axis * 2).Read16();
    RecordCursor keys((cursor + 6).Aligned(4));
    idle_model_key = keys.Read32();
    model.model_idx = ModelBank::FindResourceIdx(idle_model_key, model.bank);
    active_model_key = (keys + 4).Read32();
    activation_pos.SetDir(-128, model.angles);
    activation_pos.Add(model.pos);
    col_radius = 0;
    rec_idx = idx;
    lifecycle_flags &= 0xFFEF;
}

uint32 ResettingActivator::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80061EF0, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    result = VisualObject::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
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
        if (timer < 129)
            return 1;
        activated = false;
        model.pos = rest_pos;
        uint32 result = ModelBank::FindResourceIdx(idle_model_key, model.bank);
        model.model_idx = result;
        return result;
    }
    if (!visual_lists.players)
        return 0;
    Player &player = *static_cast<Player *>(visual_lists.players->owner);
    sint32 distance = player.model.pos.Distance(activation_pos);
    bool near = distance < 192;
    if (near)
        player.resetting_activator = this;
    return near;
}

uint32 ResettingActivator::BeginActivation()
{
    FUNCTION_MARKER(0x80062034, "SLUS_003.73");
    activated = true;
    timer = 0;
    return 1;
}

uint32 ResettingActivator::Activate()
{
    RecordDataBorrow rec_borrow;
    ObjectBorrow borrow;
    Vector3 offset;
    FUNCTION_MARKER(0x80062044, "SLUS_003.73");
    SpawnEnemyIndices(GetRecPayload(rec_idx));
    PlaySoundEffect(47, 8192, 0);
    offset.SetDir(16, model.angles);
    model.pos.Add(offset);
    uint32 result = ModelBank::FindResourceIdx(active_model_key, model.bank);
    model.model_idx = result;
    return result;
}

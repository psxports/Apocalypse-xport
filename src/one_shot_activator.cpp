#include "resource_script.h"
#include "visual_object.h"
#include "audio_session.h"
#include "model_bank.h"
#include "enemy_list.h"
#include "record_data.h"
#include "one_shot_activator.h"
#include "player.h"

OneShotActivator::OneShotActivator(const RecordCursor &rec_data, uint16 idx) : VisualObject(), activated(false)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x80061BD4, "SLUS_003.73");
    SetModelBank(GetRecModelBankName());
    AddToList(visual_lists.actors);
    RecordCursor cursor(ReadPos(rec_data));
    for (uint32 axis = 0; axis < 3; ++axis)
        model.angles.comp[axis] = (cursor + axis * 2).Read16();
    uint32 key = (cursor + 6).Aligned(4).Read32();
    model.model_idx = ModelBank::FindResourceIdx(key, model.bank);
    activation_pos.SetDir(-96, model.angles);
    activation_pos.Add(model.pos);
    col_radius = 0;
    rec_idx = idx;
    lifecycle_flags &= 0xFFEF;
}

uint32 OneShotActivator::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80061CBC, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    result = VisualObject::Destroy(0);
    if (reason & 1)
        result = ReleaseStorage();
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
    sint32 distance = player.model.pos.Distance(activation_pos);
    bool near = distance < 192;
    if (near)
        player.one_shot_activator = this;
    return near;
}

uint32 OneShotActivator::Activate()
{
    FUNCTION_MARKER(0x80061D80, "SLUS_003.73");
    activated = true;
    return 1;
}

uint32 OneShotActivator::SpawnObjects()
{
    RecordDataBorrow rec_borrow;
    ObjectBorrow borrow;
    uint32 idx;
    RecordCursor list;
    FUNCTION_MARKER(0x80061D8C, "SLUS_003.73");
    idx = rec_idx;
    list = GetRecPayload(idx);
    SpawnEnemyIndices(list);
    return PlaySoundEffect(47, 0x2000, 0);
}

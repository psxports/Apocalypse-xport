#include "resource_script.h"
#include "visual_object.h"
#include "audio_session.h"
#include "model_bank.h"
#include "enemy_list.h"
#include "record_data.h"
#include "traversal_region.h"
#include "player.h"

TraversalRegion::TraversalRegion(const RecordCursor &rec_data, uint32 idx) : VisualObject(), reverse(false)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x800620B8, "SLUS_003.73");
    col_radius = 0;
    rec_idx = idx;
    lifecycle_flags &= 0xFFEF;
    model.flags |= 1;
    AddToList(visual_lists.actors);
    RecordCursor cursor(ReadPos(rec_data));
    for (uint32 axis = 0; axis < 3; ++axis)
        model.angles.comp[axis] = (cursor + axis * 2).Read16();
    RecordCursor desc(GetRecPayload(idx));
    ReadRecPos(&target, (desc + 2).Read16());
    RecordCursor words((cursor + 6).Aligned(4));
    sint32 delta_z = (uint32)target.z - model.pos.z;
    sint32 delta_x = (uint32)target.x - model.pos.x;
    for (uint32 corner = 0; corner < 2; ++corner)
        for (uint32 axis = 0; axis < 3; ++axis)
            bounds[corner].comp[axis] = (words + (corner * 3 + axis) * 4).Read32() << 12;
    sint32 heading = (3072 - ratan2(delta_z >> 12, delta_x >> 12)) & 4095;
    sint16 cur = model.angles.y;
    if (cur < heading)
        reverse = heading - cur < 2048;
    else if (heading < cur)
        reverse = cur - heading >= 2048;
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
    return (reason & 1) ? ReleaseStorage() : 0;
}

uint32 TraversalRegion::Update()
{
    FUNCTION_MARKER(0x800622D8, "SLUS_003.73");
    return 0;
}

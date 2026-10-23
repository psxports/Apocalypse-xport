#include <string.h>

#include "record_data.h"
#include "game_diagnostics.h"
#include "enemy_list.h"
#include "visual_object.h"
#include "global.h"
#include "record_factory.h"

uint32 ActivateEnemyIndices(RecordCursor indices)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x80064E50, "SLUS_003.73");
    uint32 count = indices.Read16();
    indices.Require(2 + count * 2);
    for (uint32 idx = 0; idx < count; ++idx)
    {
        uint32 identifier = (indices + 2 + idx * 2).Read16();
        uint16 kind = GetLevelRec(identifier).Read16();
        if (kind == 1 || kind == 7)
        {
            VisualObject::ActivateByIdx(visual_lists.actors, identifier);
            VisualObject::ActivateByIdx(visual_lists.solids, identifier);
        }
    }
    return 0;
}

uint32 SpawnEnemyIndices(RecordCursor indices)
{
    RecordDataBorrow rec_borrow;
    FUNCTION_MARKER(0x80064A08, "SLUS_003.73");
    uint32 count = indices.Read16();
    indices.Require(2 + count * 2);
    for (uint32 idx = 0; idx < count; ++idx)
        SpawnRec((indices + 2 + idx * 2).Read16());
    return 0;
}

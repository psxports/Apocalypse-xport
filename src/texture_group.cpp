#include <stdlib.h>

#include "texture_group.h"
#include "texture_group_block.h"
#include "game_diagnostics.h"

const TextureGroup *TextureGroup::Find(const char *name)
{
    FUNCTION_MARKER(0x8006F164, "SLUS_003.73");
    for (TextureGroupBlock *block = TextureGroupBlock::group_list; block; block = block->next)
        for (uint32 i = 0; i < block->count; ++i)
        {
            const TextureGroup &grp = block->grps[i];
            uint32 len = 0;
            while (len < 8)
            {
                const uint8 left = name[len] & 0xDF;
                const uint8 right = grp.name[len] & 0xDF;
                if (left != right)
                    break;
                if (!left)
                    return &grp;
                ++len;
            }
            if (len == 8)
                return &grp;
        }
    return 0;
}

const TextureFrame &TextureGroup::GetFrame(const TextureGroup *grp, sint32 idx)
{
    if (!grp || idx < 0 || (uint32)idx >= grp->count)
    {
        GameDiagnostic(("Invalid texture group frame %d\n", idx));
        abort();
    }
    return grp->frames[idx];
}

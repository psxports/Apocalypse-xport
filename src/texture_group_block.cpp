#include <stdlib.h>
#include <string.h>

#include "texture_group_block.h"
#include "texture_resources.h"
#include "game_diagnostics.h"

struct TextureGroupFile
{
    char name[8];
    uint32 count;
};

typedef char TextureGroupFileSize[sizeof(TextureGroupFile) == 12 ? 1 : -1];

struct TextureFrameFile
{
    uint8 ox;
    uint8 oy;
    uint8 width;
    uint8 height;
    uint32 reference;
};

typedef char TextureFrameFileSize[sizeof(TextureFrameFile) == 8 ? 1 : -1];

TextureGroupBlock *TextureGroupBlock::group_list;

static void ClearTexGrpBlock(TextureGroupBlock &block)
{
    for (uint32 i = 0; i < block.count; ++i)
        free(block.grps[i].frames);
    free(block.grps);
    block.grps = 0;
    block.count = 0;
}

static bool DecodeTexGrps(TextureGroupBlock &block, const TextureReferences &refs, const uint8 *data, uint32 bytes)
{
    if (bytes < 4)
        return false;
    const uint32 count = *(const uint32 *)data;
    if (count > (bytes - 4) / 12 || count > 0xFFFFFFFF / sizeof(TextureGroup))
        return false;
    uint32 pos = 4;
    for (uint32 i = 0; i < count; ++i)
    {
        if (bytes - pos < 12)
            return false;
        const TextureGroupFile &file = *(const TextureGroupFile *)(data + pos);
        const uint32 frames = file.count;
        if (frames > (bytes - pos - 12) / 8 || frames > 0xFFFFFFFF / sizeof(TextureFrame))
            return false;
        const TextureFrameFile *frame_files = (const TextureFrameFile *)(data + pos + 12);
        for (uint32 frame = 0; frame < frames; ++frame)
            if (frame_files[frame].reference >= refs.count)
                return false;
        pos += 12 + frames * 8;
    }
    if (pos != bytes)
        return false;
    block.grps = count ? (TextureGroup *)malloc(count * sizeof(TextureGroup)) : 0;
    if (count && !block.grps)
        return false;
    block.count = count;
    for (uint32 i = 0; i < count; ++i)
        block.grps[i].frames = 0;
    pos = 4;
    for (uint32 i = 0; i < count; ++i)
    {
        TextureGroup &grp = block.grps[i];
        const TextureGroupFile &file = *(const TextureGroupFile *)(data + pos);
        memcpy(grp.name, file.name, sizeof(file.name));
        grp.count = file.count;
        grp.frames = grp.count ? (TextureFrame *)malloc(grp.count * sizeof(TextureFrame)) : 0;
        if (grp.count && !grp.frames)
            return false;
        const TextureFrameFile *frame_files = (const TextureFrameFile *)(data + pos + 12);
        for (uint32 frame = 0; frame < grp.count; ++frame)
        {
            const TextureFrameFile &src = frame_files[frame];
            TextureFrame &dst = grp.frames[frame];
            dst.ox = src.ox;
            dst.oy = src.oy;
            dst.w = src.width;
            dst.h = src.height;
            dst.tex = &ReadTexReference(refs, src.reference);
        }
        pos += 12 + grp.count * 8;
    }
    return true;
}

void TextureGroupBlock::Register(TextureGroupBlock *&owner, const TextureReferences &refs, const uint8 *data, uint32 bytes)
{
    FUNCTION_MARKER(0x8006F048, "SLUS_003.73");
    TextureGroupBlock decoded = {0, 0, 0, 0, 0};
    if (!DecodeTexGrps(decoded, refs, data, bytes))
    {
        ClearTexGrpBlock(decoded);
        GameDiagnostic(("Invalid texture groups or allocation failure\n"));
        abort();
    }
    TextureGroupBlock *block = (TextureGroupBlock *)malloc(sizeof(TextureGroupBlock));
    if (!block)
    {
        ClearTexGrpBlock(decoded);
        GameDiagnostic(("Cannot allocate texture group block\n"));
        abort();
    }
    *block = decoded;
    block->owner_next = owner;
    owner = block;
    block->next = TextureGroupBlock::group_list;
    if (TextureGroupBlock::group_list)
        TextureGroupBlock::group_list->prev = block;
    TextureGroupBlock::group_list = block;
    for (uint32 i = 0; i < block->count; ++i)
        for (uint32 frame = 0; frame < block->grps[i].count; ++frame)
        {
            TextureResource &tex = *block->grps[i].frames[frame].tex;
            tex.tpage |= 32;
            ++tex.uv[1].u;
            ++tex.uv[2].v;
            ++tex.uv[3].u;
            ++tex.uv[3].v;
        }
}

void TextureGroupBlock::ReleaseAll(TextureGroupBlock *&owner)
{
    while (owner)
    {
        TextureGroupBlock *block = owner;
        owner = block->owner_next;
        if (block->next)
            block->next->prev = block->prev;
        if (block->prev)
            block->prev->next = block->next;
        if (TextureGroupBlock::group_list == block)
            TextureGroupBlock::group_list = block->next;
        for (uint32 i = 0; i < block->count; ++i)
            InvalidateTexBindings(&block->grps[i]);
        ClearTexGrpBlock(*block);
        free(block);
    }
}

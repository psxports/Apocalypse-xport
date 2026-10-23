#ifndef TEXTURE_GROUP_BLOCK_H
#define TEXTURE_GROUP_BLOCK_H

#include "texture_group.h"

struct TextureReferences;

struct TextureGroupBlock
{
    uint32 count;
    TextureGroup *grps;
    TextureGroupBlock *next;
    TextureGroupBlock *prev;
    TextureGroupBlock *owner_next;

private:
    friend struct TextureGroup;
    friend struct TextureResource;
    static TextureGroupBlock *group_list;

public:
    static void Register(TextureGroupBlock *&owner, const TextureReferences &refs, const uint8 *data, uint32 bytes);
    static void ReleaseAll(TextureGroupBlock *&owner);
};

#endif

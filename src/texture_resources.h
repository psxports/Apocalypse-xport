#ifndef TEXTURE_RESOURCES_H
#define TEXTURE_RESOURCES_H

#include "texture_group_block.h"
#include "texture_group.h"
#include "texture_resource.h"
#include "palette_resource.h"

struct TextureReferences;

struct TextureReferences
{
    uint32 count;
    TextureResource **entries;
};

void InvalidateTexBindings(const TextureResource *tex);
void InvalidateTexBindings(const TextureGroup *grp);
void LoadTexReferences(TextureReferences &refs, const uint8 *data, uint32 bytes);
void ReleaseTexReferences(TextureReferences &refs);
TextureResource &ReadTexReference(const TextureReferences &refs, uint32 idx);

#endif

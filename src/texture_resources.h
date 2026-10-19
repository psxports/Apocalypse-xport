#ifndef TEXTURE_RESOURCES_H
#define TEXTURE_RESOURCES_H

#include "palette_resources.h"

struct TextureResource
{
    UV uv[4];
    uint16 clut;
    uint16 tpage;
    uint32 window;
    uint16 refs;
    uint8 loaded;
    uint32 key;
    PaletteResource *pal;
    sint16 x;
    sint16 y;
    TextureResource *next;
    TextureResource *prev;
};

struct TextureReferences
{
    uint32 count;
    TextureResource **entries;
};

struct TextureFrame
{
    sint8 ox;
    sint8 oy;
    uint8 w;
    uint8 h;
    TextureResource *tex;
};

struct TextureGroup
{
    char name[8];
    uint32 count;
    TextureFrame *frames;
};

struct TextureGroupBlock
{
    uint32 count;
    TextureGroup *groups;
    TextureGroupBlock *next;
    TextureGroupBlock *prev;
    TextureGroupBlock *owner_next;
};

TextureResource *FindModelResourceEntry(uint32 key);
TextureResource &GetTextureResource(uint32 key);
uint32 PackTextureUv(const TextureResource &tex, uint32 word);
void ReleaseUnusedModelResources();
void ResetTextureResources();
void InvalidateTextureBindings(const TextureResource *tex);
void InvalidateTextureBindings(const TextureGroup *grp);
void LoadTextureReferences(TextureReferences &refs, const uint8 *data, uint32 bytes);
void ReleaseTextureReferences(TextureReferences &refs);
TextureResource &ReadTextureReference(const TextureReferences &refs, uint32 idx);
void RegisterTextureGroups(TextureGroupBlock *&owner, const TextureReferences &refs, const uint8 *data, uint32 bytes);
void ReleaseTextureGroups(TextureGroupBlock *&owner);
const TextureGroup *FindModelTextureGroup(const char *name);
const TextureFrame &GetTextureFrame(const TextureGroup *grp, sint32 idx);

#endif

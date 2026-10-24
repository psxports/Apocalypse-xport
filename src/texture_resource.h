#ifndef TEXTURE_RESOURCE_H
#define TEXTURE_RESOURCE_H

#include "palette_resource.h"
#include "texture_region.h"

struct TextureReferences;

struct TextureResource
{
public:
    UV uv[4];
    uint16 clut;
    uint16 tpage;
    uint32 window;
    TextureRegion region;
    uint16 refs;
    bool loaded;
    uint32 key;
    PaletteResource *pal;
    sint16 x;
    sint16 y;
    TextureResource *next;
    TextureResource *prev;

private:
    friend void LoadTexReferences(TextureReferences &refs, const uint8 *data, uint32 bytes);

private:
    static TextureResource *Allocate(uint32 key);
    void Release();

public:
    static TextureResource *Find(uint32 key);

private:
    static TextureResource *BeginTraversal();
    static TextureResource *NextEntry();

public:
    static void ReleaseUnused();
    static TextureResource &Get(uint32 key);
    static void ResetAll();
    uint32 PackUv(uint32 word) const;
    uint32 HdTexelFlags(uint16 selected_clut) const;
};

#endif

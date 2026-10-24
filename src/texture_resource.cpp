#include <stdlib.h>

#include "texture_resource.h"
#include "texture_resources.h"
#include "texture_group_block.h"
#include "model_vram.h"
#include "game_diagnostics.h"
#include "hd.h"

uint32 TextureResource::HdTexelFlags(uint16 selected_clut) const
{
    if (!pal || pal->clut != selected_clut || (tpage & 0x180) >= 0x100)
        return 0;
    if (pal->texel_classes == 1)
        return HD_OPAQUE_TEXELS_ONLY;
    if (pal->texel_classes == 2)
        return HD_STP_TEXELS_ONLY;
    return 0;
}

static TextureResource *tex_buckets[512];
static uint32 tex_bucket;
static TextureResource *tex_cursor;

TextureResource *TextureResource::Allocate(uint32 key)
{
    FUNCTION_MARKER(0x8006E178, "SLUS_003.73");
    TextureResource *tex = (TextureResource *)malloc(sizeof(TextureResource));
    if (!tex)
    {
        GameDiagnostic(("Cannot allocate texture resource\n"));
        abort();
    }
    TextureResource *&head = tex_buckets[key & 511];
    tex->prev = 0;
    tex->next = head;
    head = tex;
    if (tex->next)
        tex->next->prev = tex;
    tex->key = key;
    tex->loaded = false;
    tex->region.x = tex->region.y = 0;
    tex->region.width = tex->region.height = 0;
    tex->refs = 0;
    return tex;
}

void TextureResource::Release()
{
    FUNCTION_MARKER(0x8006E1EC, "SLUS_003.73");
    if (next)
        next->prev = prev;
    if (prev)
        prev->next = next;
    TextureResource *&head = tex_buckets[key & 511];
    if (head == this)
        head = next;
    InvalidateTexBindings(this);
    free(this);
}

TextureResource *TextureResource::Find(uint32 key)
{
    FUNCTION_MARKER(0x8006E278, "SLUS_003.73");
    for (TextureResource *tex = tex_buckets[key & 511]; tex; tex = tex->next)
        if (tex->key == key)
            return tex;
    return 0;
}

TextureResource &TextureResource::Get(uint32 key)
{
    TextureResource *tex = TextureResource::Find(key);
    if (!tex)
    {
        GameDiagnostic(("Missing texture resource %08X\n", key));
        abort();
    }
    return *tex;
}

uint32 TextureResource::PackUv(uint32 word) const
{
    switch (word)
    {
        case 0:
            return uv[0].uv | ((uint32)clut << 16);
        case 1:
            return uv[1].uv | ((uint32)tpage << 16);
        case 2:
            return uv[2].uv | ((uint32)uv[3].uv << 16);
    }
    GameDiagnostic(("Invalid packed texture UV word %u\n", word));
    abort();
    return 0;
}

TextureResource *TextureResource::BeginTraversal()
{
    FUNCTION_MARKER(0x8006E2C4, "SLUS_003.73");
    tex_bucket = 0;
    tex_cursor = tex_buckets[0];
    return tex_cursor;
}

TextureResource *TextureResource::NextEntry()
{
    FUNCTION_MARKER(0x8006E2DC, "SLUS_003.73");
    while (tex_bucket < 512 && !tex_cursor)
    {
        ++tex_bucket;
        if (tex_bucket < 512)
            tex_cursor = tex_buckets[tex_bucket];
    }
    if (tex_bucket == 512)
        return 0;
    TextureResource *tex = tex_cursor;
    tex_cursor = tex->next;
    return tex;
}

void TextureResource::ReleaseUnused()
{
    FUNCTION_MARKER(0x8006E414, "SLUS_003.73");
    TextureResource::BeginTraversal();
    TextureResource *tex;
    while ((tex = TextureResource::NextEntry()) != 0)
        if (tex->loaded && !tex->refs)
        {
            --tex->pal->refs;
            ReleaseUsedVramRect(tex->x, tex->y);
            tex->Release();
        }
}

void TextureResource::ResetAll()
{
    if (TextureGroupBlock::group_list)
    {
        GameDiagnostic(("Texture groups must be released before resource reset\n"));
        abort();
    }
    for (uint32 i = 0; i < 512; ++i)
        while (tex_buckets[i])
            tex_buckets[i]->Release();
    tex_bucket = 0;
    tex_cursor = 0;
}

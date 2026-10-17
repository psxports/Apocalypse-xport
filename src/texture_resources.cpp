#include "game_sdk_bindings.h"
#include "texture_resources.h"
#include "model_vram.h"
#include <stdlib.h>
#include <string.h>

static TextureResource *tex_buckets[512];
static uint32 tex_bucket;
static TextureResource *tex_cursor;
static TextureGroupBlock *tex_groups;

TextureResource *AllocateModelResourceEntry(uint32 key)
{
    FUNCTION_MARKER(0x8006E178, "SLUS_003.73");
    TextureResource *tex = (TextureResource *)malloc(sizeof(TextureResource));
    if (!tex)
    {
        GameDiagnostic("Cannot allocate texture resource\n");
        abort();
    }
    TextureResource *&head = tex_buckets[key & 511];
    tex->prev = 0;
    tex->next = head;
    head = tex;
    if (tex->next)
        tex->next->prev = tex;
    tex->key = key;
    tex->loaded = 0;
    tex->refs = 0;
    return tex;
}

static void ReleaseModelResourceEntry(TextureResource *tex)
{
    FUNCTION_MARKER(0x8006E1EC, "SLUS_003.73");
    if (tex->next)
        tex->next->prev = tex->prev;
    if (tex->prev)
        tex->prev->next = tex->next;
    TextureResource *&head = tex_buckets[tex->key & 511];
    if (head == tex)
        head = tex->next;
    InvalidateTextureBindings(tex);
    free(tex);
}

TextureResource *FindModelResourceEntry(uint32 key)
{
    FUNCTION_MARKER(0x8006E278, "SLUS_003.73");
    for (TextureResource *tex = tex_buckets[key & 511]; tex; tex = tex->next)
        if (tex->key == key)
            return tex;
    return 0;
}

TextureResource &GetTextureResource(uint32 key)
{
    TextureResource *tex = FindModelResourceEntry(key);
    if (!tex)
    {
        GameDiagnostic("Missing texture resource %08X\n", key);
        abort();
    }
    return *tex;
}

uint32 PackTextureUv(const TextureResource &tex, uint32 word)
{
    switch (word)
    {
        case 0: return tex.uv[0].uv | ((uint32)tex.clut << 16);
        case 1: return tex.uv[1].uv | ((uint32)tex.tpage << 16);
        case 2: return tex.uv[2].uv | ((uint32)tex.uv[3].uv << 16);
    }
    GameDiagnostic("Invalid packed texture UV word %u\n", word);
    abort();
    return 0;
}

static TextureResource *BeginModelResourceTraversal()
{
    FUNCTION_MARKER(0x8006E2C4, "SLUS_003.73");
    tex_bucket = 0;
    tex_cursor = tex_buckets[0];
    return tex_cursor;
}

static TextureResource *NextModelResourceEntry()
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

void ReleaseUnusedModelResources()
{
    FUNCTION_MARKER(0x8006E414, "SLUS_003.73");
    BeginModelResourceTraversal();
    TextureResource *tex;
    while ((tex = NextModelResourceEntry()) != 0)
        if (tex->loaded && !tex->refs)
        {
            --tex->pal->refs;
            ReleaseUsedVramRectangle((uint32)(sint32)tex->x, (uint32)(sint32)tex->y);
            ReleaseModelResourceEntry(tex);
        }
}

void ResetTextureResources()
{
    if (tex_groups)
    {
        GameDiagnostic("Texture groups must be released before resource reset\n");
        abort();
    }
    for (uint32 i = 0; i < 512; ++i)
        while (tex_buckets[i])
            ReleaseModelResourceEntry(tex_buckets[i]);
    tex_bucket = 0;
    tex_cursor = 0;
}

TextureResource &ReadTextureReference(const TextureReferences &refs, uint32 idx)
{
    if (idx >= refs.count || !refs.entries || !refs.entries[idx])
    {
        GameDiagnostic("Invalid model texture reference %u\n", idx);
        abort();
    }
    return *refs.entries[idx];
}

void ReleaseTextureReferences(TextureReferences &refs)
{
    for (uint32 i = 0; i < refs.count; ++i)
        --refs.entries[i]->refs;
    free(refs.entries);
    refs.entries = 0;
    refs.count = 0;
}

void LoadTextureReferences(TextureReferences &refs, const uint8 *data, uint32 bytes)
{
    if (bytes < 4 || refs.count || refs.entries)
    {
        GameDiagnostic("Invalid texture reference input/owner\n");
        abort();
    }
    const uint32 count = xport_load_le32(data);
    if (count > (bytes - 4) / 4 || count > 0xFFFFFFFF / sizeof(TextureResource *))
    {
        GameDiagnostic("Truncated texture references\n");
        abort();
    }
    TextureResource **entries = count ? (TextureResource **)malloc(count * sizeof(TextureResource *)) : 0;
    if (count && !entries)
    {
        GameDiagnostic("Cannot allocate texture references\n");
        abort();
    }
    for (uint32 i = 0; i < count; ++i)
    {
        const uint32 key = xport_load_le32(data + 4 + i * 4);
        TextureResource *tex = FindModelResourceEntry(key);
        if (!tex)
            tex = AllocateModelResourceEntry(key);
        ++tex->refs;
        entries[i] = tex;
    }
    refs.count = count;
    refs.entries = entries;
}

static void ClearTextureGroupBlock(TextureGroupBlock &block)
{
    for (uint32 i = 0; i < block.count; ++i)
        free(block.groups[i].frames);
    free(block.groups);
    block.groups = 0;
    block.count = 0;
}

static bool DecodeTextureGroups(TextureGroupBlock &block, const TextureReferences &refs, const uint8 *data, uint32 bytes)
{
    if (bytes < 4)
        return false;
    const uint32 count = xport_load_le32(data);
    if (count > (bytes - 4) / 12 || count > 0xFFFFFFFF / sizeof(TextureGroup))
        return false;
    uint32 pos = 4;
    for (uint32 i = 0; i < count; ++i)
    {
        if (bytes - pos < 12)
            return false;
        const uint32 frames = xport_load_le32(data + pos + 8);
        if (frames > (bytes - pos - 12) / 8 || frames > 0xFFFFFFFF / sizeof(TextureFrame))
            return false;
        for (uint32 frame = 0; frame < frames; ++frame)
            if (xport_load_le32(data + pos + 16 + frame * 8) >= refs.count)
                return false;
        pos += 12 + frames * 8;
    }
    if (pos != bytes)
        return false;
    block.groups = count ? (TextureGroup *)malloc(count * sizeof(TextureGroup)) : 0;
    if (count && !block.groups)
        return false;
    block.count = count;
    for (uint32 i = 0; i < count; ++i)
        block.groups[i].frames = 0;
    pos = 4;
    for (uint32 i = 0; i < count; ++i)
    {
        TextureGroup &grp = block.groups[i];
        memcpy(grp.name, data + pos, 8);
        grp.count = xport_load_le32(data + pos + 8);
        grp.frames = grp.count ? (TextureFrame *)malloc(grp.count * sizeof(TextureFrame)) : 0;
        if (grp.count && !grp.frames)
            return false;
        for (uint32 frame = 0; frame < grp.count; ++frame)
        {
            const uint8 *src = data + pos + 12 + frame * 8;
            TextureFrame &dst = grp.frames[frame];
            dst.ox = (sint8)src[0];
            dst.oy = (sint8)src[1];
            dst.w = src[2];
            dst.h = src[3];
            dst.tex = &ReadTextureReference(refs, xport_load_le32(src + 4));
        }
        pos += 12 + grp.count * 8;
    }
    return true;
}

void RegisterTextureGroups(TextureGroupBlock *&owner, const TextureReferences &refs, const uint8 *data, uint32 bytes)
{
    FUNCTION_MARKER(0x8006F048, "SLUS_003.73");
    TextureGroupBlock decoded = {0, 0, 0, 0, 0};
    if (!DecodeTextureGroups(decoded, refs, data, bytes))
    {
        ClearTextureGroupBlock(decoded);
        GameDiagnostic("Invalid texture groups or allocation failure\n");
        abort();
    }
    TextureGroupBlock *block = (TextureGroupBlock *)malloc(sizeof(TextureGroupBlock));
    if (!block)
    {
        ClearTextureGroupBlock(decoded);
        GameDiagnostic("Cannot allocate texture group block\n");
        abort();
    }
    *block = decoded;
    block->owner_next = owner;
    owner = block;
    block->next = tex_groups;
    if (tex_groups)
        tex_groups->prev = block;
    tex_groups = block;
    for (uint32 i = 0; i < block->count; ++i)
        for (uint32 frame = 0; frame < block->groups[i].count; ++frame)
        {
            TextureResource &tex = *block->groups[i].frames[frame].tex;
            tex.tpage |= 32;
            ++tex.uv[1].u;
            ++tex.uv[2].v;
            ++tex.uv[3].u;
            ++tex.uv[3].v;
        }
}

void ReleaseTextureGroups(TextureGroupBlock *&owner)
{
    while (owner)
    {
        TextureGroupBlock *block = owner;
        owner = block->owner_next;
        if (block->next)
            block->next->prev = block->prev;
        if (block->prev)
            block->prev->next = block->next;
        if (tex_groups == block)
            tex_groups = block->next;
        for (uint32 i = 0; i < block->count; ++i)
            InvalidateTextureBindings(&block->groups[i]);
        ClearTextureGroupBlock(*block);
        free(block);
    }
}

const TextureGroup *FindModelTextureGroup(const char *name)
{
    FUNCTION_MARKER(0x8006F164, "SLUS_003.73");
    for (TextureGroupBlock *block = tex_groups; block; block = block->next)
        for (uint32 i = 0; i < block->count; ++i)
        {
            const TextureGroup &grp = block->groups[i];
            uint32 len = 0;
            while (len < 8)
            {
                const uint8 left = (uint8)name[len] & 0xDF;
                const uint8 right = (uint8)grp.name[len] & 0xDF;
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

const TextureFrame &GetTextureFrame(const TextureGroup *grp, sint32 idx)
{
    if (!grp || idx < 0 || (uint32)idx >= grp->count)
    {
        GameDiagnostic("Invalid texture group frame %d\n", idx);
        abort();
    }
    return grp->frames[idx];
}

const TextureGroup *FindModelTextureGroup(uint32 name)
{
    // Incoming script names remain at the string migration boundary
    return FindModelTextureGroup((const char *)psx_addr(name, 1));
}

#include <stdlib.h>
#include <string.h>

#include "game_diagnostics.h"
#include "texture_resources.h"
#include "model_vram.h"

TextureResource &ReadTexReference(const TextureReferences &refs, uint32 idx)
{
    if (idx >= refs.count || !refs.entries || !refs.entries[idx])
    {
        GameDiagnostic(("Invalid model texture reference %u\n", idx));
        abort();
    }
    return *refs.entries[idx];
}

void ReleaseTexReferences(TextureReferences &refs)
{
    for (uint32 i = 0; i < refs.count; ++i)
        --refs.entries[i]->refs;
    free(refs.entries);
    refs.entries = 0;
    refs.count = 0;
}

void LoadTexReferences(TextureReferences &refs, const uint8 *data, uint32 bytes)
{
    if (bytes < 4 || refs.count || refs.entries)
    {
        GameDiagnostic(("Invalid texture reference input/owner\n"));
        abort();
    }
    const uint32 count = *(const uint32 *)data;
    if (count > (bytes - 4) / 4 || count > 0xFFFFFFFF / sizeof(TextureResource *))
    {
        GameDiagnostic(("Truncated texture references\n"));
        abort();
    }
    TextureResource **entries = count ? (TextureResource **)malloc(count * sizeof(*entries)) : 0;
    if (count && !entries)
    {
        GameDiagnostic(("Cannot allocate texture references\n"));
        abort();
    }
    const uint32 *keys = (const uint32 *)(data + 4);
    for (uint32 i = 0; i < count; ++i)
    {
        const uint32 key = keys[i];
        TextureResource *tex = TextureResource::Find(key);
        if (!tex)
            tex = TextureResource::Allocate(key);
        ++tex->refs;
        entries[i] = tex;
    }
    refs.count = count;
    refs.entries = entries;
}

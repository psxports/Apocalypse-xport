#include <string.h>
#include <stdlib.h>

#include "game_diagnostics.h"
#include "model_keys.h"
#include "resource_file_layouts.h"

void ReleaseModelKeys(ModelKeys &keys)
{
    free(keys.values);
    keys.values = 0;
    keys.count = 0;
}

bool DecodeModelFileLayout(ModelFileLayout &layout, const uint8 *data, uint32 bytes)
{
    FUNCTION_MARKER(0x8006E360, "SLUS_003.73");
    if (!data || bytes < 16)
        return false;
    ModelFileLayout decoded;
    const ModelFileHeader &header = *(const ModelFileHeader *)data;
    decoded.scene_count = header.scene_count;
    if (decoded.scene_count > (bytes - 16) / 36)
        return false;
    decoded.model_table_offset = 16 + decoded.scene_count * 36;
    decoded.model_count = ((const uint32 *)data)[decoded.model_table_offset / 4 - 1];
    if (decoded.model_count > (bytes - decoded.model_table_offset) / 4)
        return false;
    decoded.blocks_offset = header.blocks_offset;
    uint32 offset = decoded.blocks_offset;
    if (offset < decoded.model_table_offset + decoded.model_count * 4 || offset > bytes)
        return false;
    for (;;)
    {
        if (bytes - offset < 4)
            return false;
        if (*(const uint32 *)(data + offset) == 0xFFFFFFFF)
        {
            offset += 4;
            break;
        }
        if (bytes - offset < 8)
            return false;
        const ResourceBlockFile &block = *(const ResourceBlockFile *)(data + offset);
        const uint32 size = block.bytes;
        if (size > bytes - offset - 8)
            return false;
        offset += 8 + size;
    }
    if (decoded.model_count > (bytes - offset) / 4)
        return false;
    decoded.keys_offset = offset;
    layout = decoded;
    return true;
}

static bool DecodeModelKeys(ModelKeys &keys, ModelFileLayout &layout, const uint8 *data, uint32 bytes)
{
    ModelFileLayout decoded;
    if (!DecodeModelFileLayout(decoded, data, bytes))
        return false;
    const uint32 count = decoded.model_count;
    uint32 *values = count ? (uint32 *)malloc(count * sizeof(*values)) : 0;
    if (count && !values)
        return false;
    if (count)
        memcpy(values, data + decoded.keys_offset, count * sizeof(*values));
    keys.count = count;
    keys.values = values;
    layout = decoded;
    return true;
}

ModelFileLayout LoadModelKeys(ModelKeys &keys, const uint8 *data, uint32 bytes)
{
    ModelKeys decoded = {0, 0};
    ModelFileLayout layout;
    if (!DecodeModelKeys(decoded, layout, data, bytes))
    {
        ReleaseModelKeys(decoded);
        GameDiagnostic(("Invalid model key table or allocation failure\n"));
        abort();
    }
    ReleaseModelKeys(keys);
    keys = decoded;
    return layout;
}

uint32 ReadModelKey(const ModelKeys &keys, uint32 idx)
{
    if (!keys.values || idx >= keys.count)
    {
        GameDiagnostic(("Invalid model/script key index %u\n", idx));
        abort();
    }
    return keys.values[idx];
}

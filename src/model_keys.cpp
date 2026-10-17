#include "game_sdk_bindings.h"
#include "model_keys.h"
#include <stdlib.h>

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
    decoded.scene_count = xport_load_le32(data + 8);
    if (decoded.scene_count > (bytes - 16) / 36)
        return false;
    decoded.model_table_offset = 16 + decoded.scene_count * 36;
    decoded.model_count = xport_load_le32(data + decoded.model_table_offset - 4);
    if (decoded.model_count > (bytes - decoded.model_table_offset) / 4)
        return false;
    decoded.blocks_offset = xport_load_le32(data + 4);
    uint32 offset = decoded.blocks_offset;
    if (offset < decoded.model_table_offset + decoded.model_count * 4 || offset > bytes)
        return false;
    for (;;)
    {
        if (bytes - offset < 4)
            return false;
        if (xport_load_le32(data + offset) == 0xFFFFFFFF)
        {
            offset += 4;
            break;
        }
        if (bytes - offset < 8)
            return false;
        const uint32 size = xport_load_le32(data + offset + 4);
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
    uint32 *values = count ? (uint32 *)malloc(count * sizeof(uint32)) : 0;
    if (count && !values)
        return false;
    for (uint32 i = 0; i < count; ++i)
        values[i] = xport_load_le32(data + decoded.keys_offset + i * 4);
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
        GameDiagnostic("Invalid model key table or allocation failure\n");
        abort();
    }
    ReleaseModelKeys(keys);
    keys = decoded;
    return layout;
}

uint32 ReadModelKey(const ModelKeys &keys, uint32 index)
{
    if (!keys.values || index >= keys.count)
    {
        GameDiagnostic("Invalid model/script key index %u\n", index);
        abort();
    }
    return keys.values[index];
}

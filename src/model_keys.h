#ifndef MODEL_KEYS_H
#define MODEL_KEYS_H

extern "C"
{
#include "psx.h"
}

struct ModelKeys
{
    uint32 count;
    uint32 *values;
};

struct ModelFileLayout
{
    uint32 blocks_offset;
    uint32 scene_count;
    uint32 model_count;
    uint32 model_table_offset;
    uint32 keys_offset;
};

bool DecodeModelFileLayout(ModelFileLayout &layout, const uint8 *data, uint32 bytes);
void ReleaseModelKeys(ModelKeys &keys);
ModelFileLayout LoadModelKeys(ModelKeys &keys, const uint8 *data, uint32 bytes);
uint32 ReadModelKey(const ModelKeys &keys, uint32 index);

#endif

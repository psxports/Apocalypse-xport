#ifndef RESOURCE_FILE_LAYOUTS_H
#define RESOURCE_FILE_LAYOUTS_H

// Shared model file records use native little-endian representation
struct ModelFileHeader
{
    uint32 kind;
    uint32 blocks_offset;
    uint32 scene_count;
};

typedef char ModelFileHeaderSize[sizeof(ModelFileHeader) == 12 ? 1 : -1];

struct ResourceBlockFile
{
    uint32 kind;
    uint32 bytes;
};

typedef char ResourceBlockFileSize[sizeof(ResourceBlockFile) == 8 ? 1 : -1];

#endif

#ifndef TEXTURE_GROUP_H
#define TEXTURE_GROUP_H

#include "texture_resource.h"

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

public:
    static const TextureGroup *Find(const char *name);
    static const TextureFrame &GetFrame(const TextureGroup *grp, sint32 idx);
};

#endif

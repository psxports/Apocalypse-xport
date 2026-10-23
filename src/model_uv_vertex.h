#ifndef MODEL_UV_VERTEX_H
#define MODEL_UV_VERTEX_H

#include "psx.h"

struct ModelUvVertex
{
    uint8 u;
    uint8 v;
    uint8 wave_x;
    uint8 wave_y;

public:
    sint16 ReadHorizontalOffset(uint32 phase) const;
    sint16 ReadVerticalOffset(uint32 phase) const;
};

#endif

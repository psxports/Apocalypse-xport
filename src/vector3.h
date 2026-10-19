#ifndef VECTOR3_H
#define VECTOR3_H

extern "C"
{
#include "psx.h"
}

struct Vector3
{
    // Word view preserves wrapping arithmetic without changing signed components
    union
    {
        struct
        {
            sint32 x, y, z;
        };

        uint32 words[3];
        sint32 components[3];
    };

    // Component indices are limited to 0..2
    sint32 &operator[](uint32 idx)
    {
        return idx == 0 ? x : idx == 1 ? y : z;
    }

    sint32 operator[](uint32 idx) const
    {
        return idx == 0 ? x : idx == 1 ? y : z;
    }
};

struct ShortVector3
{
    // Word view preserves angle truncation and masked arithmetic
    union
    {
        struct
        {
            sint16 x, y, z;
        };

        uint16 words[3];
        sint16 components[3];
    };

    // Component indices are limited to 0..2
    sint16 &operator[](uint32 idx)
    {
        return idx == 0 ? x : idx == 1 ? y : z;
    }

    sint16 operator[](uint32 idx) const
    {
        return idx == 0 ? x : idx == 1 ? y : z;
    }
};

#endif

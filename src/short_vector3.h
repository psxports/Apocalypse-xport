#ifndef SHORT_VECTOR3_H
#define SHORT_VECTOR3_H

#include "psx.h"

struct Vector3;

struct ShortVector3
{
    union
    {
        struct
        {
            sint16 x, y, z;
        };

        sint16 comp[3];
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

public:
    uint32 SetDir(const Vector3 &origin, const Vector3 &target);
    uint32 NormalizeAngles();
    bool SnapAngles();
    ShortVector3 &AddAngles(const ShortVector3 &src);
    ShortVector3 &SubtractAngles(const ShortVector3 &src);
    ShortVector3 &DivideAngles(const uint32 &divisor);
    ShortVector3 &DampAngles(const uint8 *shifts);
    ShortVector3 &AddAngles(const ShortVector3 &left, const ShortVector3 &right);
    ShortVector3 &SubtractAngles(const ShortVector3 &left, const ShortVector3 &right);
    ShortVector3 &Scale(const uint32 &scalar, const ShortVector3 &vector);
    ShortVector3 &ShiftRight(const ShortVector3 &vector, const uint32 &shift);
    void BuildRotationMatrix(MATRIX &matrix) const;
    void BuildRotationMatrixf(MATRIXF &matrix) const;
};

#endif

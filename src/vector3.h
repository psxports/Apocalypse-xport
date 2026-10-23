#ifndef VECTOR3_H
#define VECTOR3_H

#include "short_vector3.h"

extern "C"
{
#include "psx.h"
}

struct ShortVector3;

struct Vector3
{
    union
    {
        struct
        {
            sint32 x, y, z;
        };

        sint32 comp[3];
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

public:
    uint32 RotateYaw(const Vector3 &input, uint32 angle);
    uint32 Normalize(const Vector3 &input);
    uint32 SetDir(uint32 scale, const ShortVector3 &angles);
    uint32 EuclideanDistance(const Vector3 &target) const;
    uint32 HorizontalDistance(const Vector3 &target) const;
    uint32 Distance(const Vector3 &target) const;
    uint32 Len() const;
    uint32 LenSq() const;
    bool SnapSmall();
    Vector3 &Add(const Vector3 &src);
    Vector3 &Subtract(const Vector3 &src);
    Vector3 &Divide(const uint32 &scalar);
    Vector3 &ShiftRight(const uint32 &shift);
    Vector3 &ShiftLeft(const uint32 &shift);
    Vector3 &Damp(const uint8 *shifts);
    bool Differs(const Vector3 &right) const;
    Vector3 &Add(const Vector3 &left, const Vector3 &right);
    Vector3 &Subtract(const Vector3 &left, const Vector3 &right);
    Vector3 &Multiply(const Vector3 &vector, const uint32 &scalar);
    Vector3 &Scale(const uint32 &scalar, const Vector3 &vector);
    Vector3 &Divide(const Vector3 &vector, const uint32 &scalar);
    Vector3 &ShiftRight(const Vector3 &vector, const uint32 &shift);
    Vector3 &ShiftLeft(const Vector3 &vector, const uint32 &shift);
    void LoadGteTranslation() const;

private:
    static Vector3 &CombineVectors(Vector3 &output, const Vector3 &left, const Vector3 &right, bool subtract);
    static Vector3 &ApplyVectorScalar(Vector3 &output, const Vector3 &vector, const uint32 &scalar, bool shift);
    static Vector3 &ShiftVectorInPlace(Vector3 &vector, const uint32 &shift, bool right);
    static Vector3 &AccumulateVector(Vector3 &vector, const Vector3 &src, bool subtract);
    static uint32 SqShiftedVector(const Vector3 &vector);
};

#endif

#ifndef VECTOR_ARITHMETIC_H
#define VECTOR_ARITHMETIC_H

extern "C"
{
#include "psx.h"
}

uint32 MultiplyDivideWord(uint32 first, uint32 second, uint32 divisor);

uint16 *MultiplyScalarShortVector(uint16 *output, const uint32 *scalar, const uint16 *vector);
uint16 *ShiftShortVectorRight(uint16 *output, const uint16 *vector, const uint32 *shift);
uint32 VectorsDiffer(const uint32 *left, const uint32 *right);
uint32 SnapSmallVector(uint32 *vector);
uint32 *ShiftVectorLeft(uint32 *vector, const uint32 *shift);
uint32 *ShiftVectorRight(uint32 *vector, const uint32 *shift);
uint32 *ShiftVectorLeft(uint32 *output, const uint32 *vector, const uint32 *shift);
uint32 *DampVector(uint32 *vector, const uint8 *shifts);
uint32 *DivideVector(uint32 *vector, const uint32 *scalar);
uint32 *DivideVector(uint32 *output, const uint32 *vector, const uint32 *scalar);
uint32 *MultiplyScalarVector(uint32 *output, const uint32 *scalar, const uint32 *vector);
uint32 *MultiplyVector(uint32 *output, const uint32 *vector, const uint32 *scalar);
uint32 *ShiftVectorRight(uint32 *output, const uint32 *vector, const uint32 *shift);
uint32 *AddVectors(uint32 *vector, const uint32 *source);
uint32 *SubtractVectors(uint32 *vector, const uint32 *source);
uint32 *AddVectors(uint32 *output, const uint32 *left, const uint32 *right);
uint32 *SubtractVectors(uint32 *output, const uint32 *left, const uint32 *right);
uint32 NormalizeFixedVector(uint32 *output, const uint32 *input);

#endif

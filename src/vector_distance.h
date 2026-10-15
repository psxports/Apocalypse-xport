#ifndef VECTOR_DISTANCE_H
#define VECTOR_DISTANCE_H

extern "C"
{
#include "psx.h"
}

uint32 LookupDistanceRoot(uint32 value);
uint32 MeasureVectorLength(const uint32 *vector);
uint32 MeasureSquaredVectorLength(const uint32 *vector);
uint32 MeasureHorizontalDistance(const uint32 *origin, const uint32 *target);
uint32 MeasurePositionDistance(const uint32 *origin, const uint32 *target);

uint32 MeasureEuclideanDistance(const uint32 *origin, const uint32 *target);

#endif

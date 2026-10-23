#ifndef REGION_ACTIONS_H
#define REGION_ACTIONS_H

#include "vector3.h"
#include "psx.h"

uint32 ClearRegion(const Vector3 *lower, const Vector3 *upper);
void SetRegionObjectsEnabled(const Vector3 *lower, const Vector3 *upper, uint32 enabled);

#endif

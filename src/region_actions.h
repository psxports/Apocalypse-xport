#ifndef REGION_ACTIONS_H
#define REGION_ACTIONS_H

#include "psx.h"

uint32 ClearRegion(const uint32 *lower, const uint32 *upper);
void SetRegionObjectsEnabled(const uint32 *lower, const uint32 *upper, uint32 enabled);

#endif

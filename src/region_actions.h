#ifndef REGION_ACTIONS_H
#define REGION_ACTIONS_H

#include "psx.h"

#ifdef __cplusplus
uint32 ApplyRegionToObjects(const uint32 *lower, const uint32 *upper, uint32 object, uint32 trigger);
uint32 RemoveRegionEffects(const uint32 *lower, const uint32 *upper);
uint32 ClearRegion(const uint32 *lower, const uint32 *upper);
uint32 SetRegionObjectsEnabled(const uint32 *lower, const uint32 *upper, uint32 enabled);
extern "C" {
#endif
uint32 apocalypse_clear_region_native(const uint32 *lower, const uint32 *upper);
uint32 apocalypse_region_enabled_native(const uint32 *lower, const uint32 *upper, uint32 enabled);
#ifdef __cplusplus
}
#endif

#endif

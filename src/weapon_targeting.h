#ifndef WEAPON_TARGETING_H
#define WEAPON_TARGETING_H
#include "model_instance.h"

extern "C"
{
#include "psx.h"
}
ModelInstance *SelectWeaponTarget(const uint32 *position, const sint16 *original_angles, ModelInstance *candidate, uint32 range, uint32 aperture, uint32 excluded_flags);
ModelInstance *SelectAimTarget(sint16 output[3], const uint32 origin[3], const sint16 angles[3], uint32 distance_limit, uint32 yaw_limit, uint32 pitch_limit, uint32 scale, uint32 exclude_mask, ModelInstance *list);
#endif

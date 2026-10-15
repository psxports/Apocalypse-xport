#ifndef WEAPON_TARGETING_H
#define WEAPON_TARGETING_H
extern "C"
{
#include "psx.h"
}
uint32 SelectWeaponTarget(const uint32 *position, const sint16 *original_angles, uint32 candidate, uint32 range, uint32 aperture, uint32 excluded_flags);
uint32 SelectAimTarget(sint16 output[3], const uint32 origin[3], const sint16 angles[3], uint32 distance_limit, uint32 yaw_limit, uint32 pitch_limit, uint32 scale, uint32 exclude_mask, uint32 list);
#endif

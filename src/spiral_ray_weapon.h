#ifndef SPIRAL_RAY_WEAPON_H
#define SPIRAL_RAY_WEAPON_H
extern "C"
{
#include "psx.h"
}
#include "weapon_nodes.h"

// Descriptive native name for a hitscan ray with spiralling particle graphics
class SpiralRayWeapon : public WeaponNode
{
public:
    SpiralRayWeapon(ModelInstance **target);

    virtual ~SpiralRayWeapon()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const uint32 *position, const sint16 *angles);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);

private:
    void Configure(uint32 pitch_spread, uint32 yaw_spread, uint32 fire_rate, uint32 range, uint32 sound_id, uint32 flash_scale);

    uint32 pitch_spread;
    uint32 yaw_spread;
    uint8 fire_rate;
    uint32 range;
    uint32 sound_id;
    uint32 flash_scale;
    uint32 light_pending;
    static uint32 DivideBy256(uint32 value);
};

#endif

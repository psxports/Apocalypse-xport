#ifndef SPIRAL_RAY_WEAPON_H
#define SPIRAL_RAY_WEAPON_H
extern "C"
{
#include "psx.h"
}
#include "weapon_type.h"
#include "weapon_node.h"

// Descriptive native name for a hitscan ray with spiralling particle graphics
class SpiralRayWeapon : public WeaponNode
{
public:
    virtual ~SpiralRayWeapon()
    {
    }

private:
    uint32 pitch_spread;
    uint32 yaw_spread;
    uint8 fire_rate;
    uint32 range;
    sint32 sound_id;
    uint32 flash_scale;
    bool light_pending;

public:
    SpiralRayWeapon(ModelInstance **target);
    virtual uint32 Destroy(uint32 reason);

private:
    void Configure(uint32 pitch_spread, uint32 yaw_spread, uint8 fire_rate, uint32 range, sint32 sound_id, uint32 flash_scale);

public:
    virtual uint32 Fire(const Vector3 &input_pos, const ShortVector3 &input_angles);
    virtual uint32 Update(const Vector3 *pos);
    virtual uint32 ReadLight(uint8 *r, uint8 *g, uint8 *b);
};

#endif

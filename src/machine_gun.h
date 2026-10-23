#ifndef MACHINE_GUN_H
#define MACHINE_GUN_H

#include "weapon_type.h"
#include "weapon_node.h"

// Default player hitscan weapon with tracer and ricochet effects
class MachineGun : public WeaponNode
{
private:
    uint32 pitch_spread;
    uint32 yaw_spread;
    bool muzzle_effect;
    uint8 fire_rate;
    uint32 range;
    sint32 sound_id;
    uint32 flash_scale;
    bool light_pending;

public:
    explicit MachineGun(ModelInstance **list);
    virtual uint32 Destroy(uint32 reason);
    void Configure(uint32 pitch_spread, uint32 yaw_spread, uint8 fire_rate, uint32 range, uint8 muzzle_effect, sint32 sound_id, uint32 flash_scale);
    virtual uint32 Fire(const Vector3 &input_pos, const ShortVector3 &input_angles);
    virtual uint32 Update(const Vector3 *pos);
    virtual uint32 ReadLight(uint8 *r, uint8 *g, uint8 *b);
};

#endif

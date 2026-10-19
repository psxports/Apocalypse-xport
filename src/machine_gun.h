#ifndef MACHINE_GUN_H
#define MACHINE_GUN_H

#include "weapon_nodes.h"

// Default player hitscan weapon with tracer and ricochet effects
class MachineGun : public WeaponNode
{
public:
    explicit MachineGun(ModelInstance **list);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Fire(const uint32 *position, const sint16 *rotation);
    virtual uint32 Update(const uint32 *position, const sint16 *angles);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    void Configure(uint32 pitch_spread, uint32 yaw_spread, uint8 fire_rate, uint32 range, uint8 muzzle_effect, uint32 sound_id, uint32 flash_scale);

private:
    uint32 pitch_spread;
    uint32 yaw_spread;
    uint8 muzzle_effect;
    uint8 fire_rate;
    uint32 range;
    uint32 sound_id;
    uint32 flash_scale;
    uint32 light_pending;
};

#endif

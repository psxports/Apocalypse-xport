#ifndef MACHINE_GUN_H
#define MACHINE_GUN_H

#include "weapon_nodes.h"

// Default player hitscan weapon with tracer and ricochet effects
class MachineGun : public WeaponNode
{
public:
    explicit MachineGun(uint32 list);
    static MachineGun &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Fire(const uint32 *position, const sint16 *rotation);
    virtual uint32 Update(const sint32 *position, const sint16 *angles);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    uint32 Configure(uint32 field60, uint32 field64, uint32 field70, uint32 field72, uint32 field68, uint32 field69, uint32 field76, uint32 field80);
};

#endif

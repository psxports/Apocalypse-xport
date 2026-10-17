#ifndef FLAMETHROWER_H
#define FLAMETHROWER_H

#include "weapon_nodes.h"

class Flamethrower : public WeaponNode
{
public:
    Flamethrower(ModelInstance **list, ModelInstance **secondary_list);
    static Flamethrower &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    uint32 Configure(uint32 amount);
    void ClearBeam();
    virtual uint32 Fire(const uint32 *position, const sint16 *angles);
    virtual uint32 Update(const sint32 *position, const sint16 *angles);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
};

#endif

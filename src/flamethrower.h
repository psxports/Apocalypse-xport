#ifndef FLAMETHROWER_H
#define FLAMETHROWER_H

#include "weapon_nodes.h"

class RibbonSegments;

class Flamethrower : public WeaponNode
{
public:
    Flamethrower(ModelInstance **list, ModelInstance **secondary_list);
    virtual ~Flamethrower();
    virtual uint32 Destroy(uint32 reason);
    uint32 Configure(uint32 amount);
    void ClearBeam();
    virtual uint32 Fire(const uint32 *position, const sint16 *angles);
    virtual uint32 Update(const uint32 *position, const sint16 *angles);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);

private:
    uint32 impact_count;
    uint32 segment_len;
    ObjectReference<RibbonSegments> beam;
};

#endif

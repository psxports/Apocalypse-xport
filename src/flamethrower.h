#ifndef FLAMETHROWER_H
#define FLAMETHROWER_H

#include "object_reference.h"
#include "weapon_type.h"
#include "weapon_node.h"

class RibbonSegments;

class Flamethrower : public WeaponNode
{
private:
    ObjectReference<RibbonSegments> beam;
    uint32 impact_count;
    sint32 seg_len;

public:
    Flamethrower(ModelInstance **list, ModelInstance **secondary_list);
    virtual uint32 Destroy(uint32 reason);
    sint32 Configure(sint32 range);

private:
    void ClearBeam();
    static uint8 ReadShade(sint32 val);

public:
    virtual uint32 Fire(const Vector3 &input_pos, const ShortVector3 &input_angles);
    virtual uint32 Update(const Vector3 *pos);
    virtual uint32 ReadLight(uint8 *r, uint8 *g, uint8 *b);
    virtual ~Flamethrower();
};

#endif

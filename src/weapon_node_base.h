#ifndef WEAPON_NODE_BASE_H
#define WEAPON_NODE_BASE_H

#include "vector3.h"
#include "object.h"
#include "model_instance.h"
#include "weapon_type.h"

class EffectObject;
class RingEffect;
class BeamSegments;

// TODO Recover the original base class name
class WeaponNodeBase : public Object
{
public:
    virtual ~WeaponNodeBase()
    {
    }

protected:
    // Native base binding adds no original memory writes
    WeaponNodeBase()
    {
    }

public:
    virtual uint32 Destroy(uint32 reason);
};

#endif

#ifndef HOMING_MISSILES_H
#define HOMING_MISSILES_H

#include "vector3.h"
#include "object.h"
#include "model_instance.h"
#include "weapon_type.h"
#include "weapon_node.h"

class EffectObject;
class RingEffect;
class BeamSegments;

class HomingMissiles : public WeaponNode
{
private:
    uint32 fire_delay;
    uint32 fire_timer;

public:
    HomingMissiles(ModelInstance **list);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Fire(const Vector3 &input_pos, const ShortVector3 &input_angles);
    virtual uint32 Update(const Vector3 *pos);
    virtual uint32 ReadLight(uint8 *r, uint8 *g, uint8 *b);
};

#endif

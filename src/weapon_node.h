#ifndef WEAPON_NODE_H
#define WEAPON_NODE_H

#include "object_reference.h"
#include "vector3.h"
#include "object.h"
#include "model_instance.h"
#include "weapon_type.h"
#include "weapon_node_base.h"

class EffectObject;
class RingEffect;
class BeamSegments;

class WeaponNode : public WeaponNodeBase
{
public:
    uint32 type;

protected:
    uint32 fire_active;
    uint32 last_fire_tick;
    uint32 start_tick;
    uint32 child_tick;
    ObjectReference<EffectObject> child;

public:
    uint32 damage;

protected:
    ModelInstance **owner_list;
    ModelInstance **secondary_list;
    uint32 sound;

public:
    uint32 ammo;

protected:
    Vector3 muzzle_pos;

public:
    // Native cleanup returns zero while timer callbacks retain their carriers

protected:
    WeaponNode(ModelInstance **list, ModelInstance **secondary_list);

public:
    virtual uint32 Destroy(uint32 reason);

protected:
    void ClearChild();
    uint32 ConsumeAmmo(uint32 amount);
    Vector3 &OffsetByAngles(Vector3 &pos, const ShortVector3 &angles);

public:
    virtual uint32 ReadLight(uint8 *r, uint8 *g, uint8 *b);
    virtual uint32 Update(const Vector3 *pos);
    virtual uint32 Fire(const Vector3 &input_pos, const ShortVector3 &input_angles);
    static ModelInstance *SelectTarget(const Vector3 *pos, const ShortVector3 *original_angles, ModelInstance *candidate, sint32 range, sint32 aperture, uint16 excluded_flags);
    static ModelInstance *SelectAimTarget(ShortVector3 &output, const Vector3 &origin, const ShortVector3 &original_angles, sint32 distance_limit, sint32 yaw_limit, sint32 pitch_limit, uint32 scale, uint16 exclude_mask, ModelInstance *list);
};

#endif

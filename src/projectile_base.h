#ifndef PROJECTILE_BASE_H
#define PROJECTILE_BASE_H

#include "explosion_ring.h"
#include "object_reference.h"
#include "vector3.h"
#include "visual_object.h"
#include "mesh_ring_effect.h"
#include "ribbon_trail.h"

struct ModelPolygon;

class RibbonTrail;
class RingEffect;

class ProjectileBase : public VisualObject
{
protected:
    bool trace_movement;
    uint32 age;
    uint32 lifetime;
    ModelInstance **owner_list;
    uint16 damage;
    uint16 radius;
    uint32 damage_kind;
    ObjectReference<RingEffect> glow_object;
    sint32 glow_distance;
    Vector3 glow_pos;
    ObjectReference<RibbonTrail> trail_object;
    bool impact_pending;
    ModelReference impact_target;
    bool impact_dynamic;
    uint8 trail_r, trail_g, trail_b;
    uint8 trail_kind;

public:
    ProjectileBase();
    virtual uint32 Destroy(uint32 reason);

protected:
    void PrepareTrajectory(uint32 scale);

public:
    virtual uint32 Explode();

protected:
    uint32 SyncChild();
    uint32 AdvanceMotion();
    void ApplyBlast(ModelInstance *list, sint32 radius, uint32 impulse, uint32 kind);
    void DamageNearbyObjects(sint32 radius);

public:
    uint32 SetTrailColor(uint8 kind, uint8 r, uint8 g, uint32 b);
    static void ResetPopulation();
    virtual ~ProjectileBase();
    void SetDamage(uint16 damage);
    void SetBlastRadius(uint16 radius);
};

#endif

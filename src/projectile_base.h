#ifndef PROJECTILE_BASE_H
#define PROJECTILE_BASE_H

#include "vector3.h"

#include "visual_object.h"

struct ModelPolygon;
void ResetProjectilePopulation();

class RibbonTrail;
class RingEffect;
#include "mesh_ring_effect.h"

struct ProjectileImpact
{
    uint32 pending;
    ModelReference hit;
    Vector3 pos;
    uint8 dynamic;

    ProjectileImpact() : pending(0), dynamic(0)
    {
        pos.words[0] = pos.words[1] = pos.words[2] = 0;
    }
};

struct ProjectileState
{
    bool trace_movement;
    uint32 age;
    uint32 lifetime;
    ModelInstance **owner_list;
    uint16 damage;
    uint16 radius;
    uint32 damage_kind;
    ObjectReference<RingEffect> glow_object;
    uint32 glow_distance;
    Vector3 glow_pos;
    ObjectReference<RibbonTrail> trail_object;
    ProjectileImpact impact;
    RGBC trail_color;
    uint8 trail_kind;

    ProjectileState() : trace_movement(false), age(0), lifetime(0), owner_list(0), damage(0), radius(0), damage_kind(0), glow_object(), glow_distance(0), trail_object(), trail_kind(0)
    {
        glow_pos.words[0] = glow_pos.words[1] = glow_pos.words[2] = 0;
        trail_color.rgbc = 0;
    }
};

// TODO Recover the original name for the 0x800A15F8 effect family
class ExplosionRing : public MeshRingEffect
{
public:
    ExplosionRing();

    virtual ~ExplosionRing()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    uint16 age;
    uint16 lifetime;
    sint16 radius_step;
    sint16 width_step;
};

class ProjectileBase : public VisualObject
{
public:
    ProjectileBase();
    virtual ~ProjectileBase();
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Explode();
    void PrepareTrajectory(uint32 scale);
    uint32 SyncChild();
    uint32 AdvanceMotion();
    void ApplyBlast(ModelInstance *list, uint32 radius, uint32 impulse, uint32 kind);
    void DamageNearbyObjects(uint32 radius);
    uint32 SetTrailColor(uint32 kind, uint32 red, uint32 green, uint32 blue);
    void SetDamage(uint32 damage);
    void SetBlastRadius(uint32 radius);

protected:
    ProjectileState projectile;
};

#endif

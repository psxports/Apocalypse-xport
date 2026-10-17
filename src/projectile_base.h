#ifndef PROJECTILE_BASE_H
#define PROJECTILE_BASE_H

#include "visual_object.h"

struct ModelPolygon;
#include "mesh_ring_effect.h"

struct ProjectileImpact
{
    uint32 pending;
    ModelReference hit;
    uint32 pos[3];
    uint8 dynamic;

    ProjectileImpact() : pending(0), dynamic(0)
    {
        pos[0] = pos[1] = pos[2] = 0;
    }
};

struct ProjectileState
{
    uint32 age;
    uint32 lifetime;
    ModelInstance **owner_list;
    uint16 damage;
    uint16 radius;
    uint32 damage_kind;
    uint32 glow_object;
    uint32 glow_distance;
    uint32 glow_pos[3];
    uint32 trail_object;
    ProjectileImpact impact;
    uint8 trail_rgb[3];
    uint8 trail_kind;

    ProjectileState() : age(0), lifetime(0), owner_list(0), damage(0), radius(0), damage_kind(0), glow_object(0), glow_distance(0), trail_object(0), trail_kind(0)
    {
        glow_pos[0] = glow_pos[1] = glow_pos[2] = 0;
        trail_rgb[0] = trail_rgb[1] = trail_rgb[2] = 0;
    }
};

// TODO Recover the original name for the 0x800A15F8 effect family
class ExplosionRing : public MeshRingEffect
{
public:
    ExplosionRing();
    virtual ~ExplosionRing() {}
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
    static ExplosionRing &Attach(uint32 object);
};

class ProjectileBase : public VisualObject
{
public:
    ProjectileBase();
    virtual ~ProjectileBase() {}
    static ProjectileBase &Attach(uint32 object);
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
    static uint32 ReadAnglesWord(uint32 address);
};

#endif

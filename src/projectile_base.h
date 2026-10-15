#ifndef PROJECTILE_BASE_H
#define PROJECTILE_BASE_H

#include "visual_object.h"
#include "mesh_ring_effect.h"

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
    void ApplyBlast(uint32 list, uint32 radius, uint32 impulse, uint32 kind);
    void DamageNearbyObjects(uint32 radius);
    uint32 SetTrailColor(uint32 kind, uint32 red, uint32 green, uint32 blue);

protected:
    static uint32 ReadAnglesWord(uint32 address);
};

#endif

#ifndef DEATH_PERSISTENT_RING_H
#define DEATH_PERSISTENT_RING_H

#include "vector3.h"
#include "mesh_ring_effect.h"

// Death rings share the smtile texture
class DeathPersistentRing : public MeshRingEffect
{
public:
    virtual ~DeathPersistentRing()
    {
    }

private:
    sint16 age;

public:
    virtual void Update();
    virtual uint32 Destroy(uint32 reason);
    DeathPersistentRing(const Vector3 *pos);
    static void Spawn(const Vector3 *pos);
};

#endif

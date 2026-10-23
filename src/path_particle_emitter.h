#ifndef PATH_PARTICLE_EMITTER_H
#define PATH_PARTICLE_EMITTER_H

#include "effect_object.h"
#include "effect_controller.h"

// Emit smoke sprites and rings along linked record positions
class PathParticleEmitter : public EffectController
{
private:
    uint16 start_rec;
    uint16 smoke_frequency;
    uint16 ring_frequency;

public:
    uint32 identifier;

    virtual ~PathParticleEmitter()
    {
    }

public:
    static void Spawn(uint32 identifier);
    static void DestroyByIdentifier(uint32 identifier);
    PathParticleEmitter(uint32 identifier);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif

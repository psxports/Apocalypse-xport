#ifndef PATH_PARTICLE_EMITTER_H
#define PATH_PARTICLE_EMITTER_H

#include "effect_object.h"

// Emit smoke sprites and rings along linked record positions
class PathParticleEmitter : public EffectController
{
public:
    uint32 start_rec;
    uint16 smoke_frequency;
    uint16 ring_frequency;
    uint32 identifier;

    PathParticleEmitter(uint32 identifier);

    virtual ~PathParticleEmitter()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

void SpawnPathParticleEmitter(uint32 identifier);

#endif

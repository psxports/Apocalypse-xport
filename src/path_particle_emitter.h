#ifndef PATH_PARTICLE_EMITTER_H
#define PATH_PARTICLE_EMITTER_H

#include "effect_object.h"

// Emit smoke sprites and rings along linked record positions
class PathParticleEmitter : public EffectController
{
public:
    PathParticleEmitter(uint32 identifier);
    virtual ~PathParticleEmitter() {}
    static PathParticleEmitter &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

uint32 SpawnPathParticleEmitter(uint32 identifier);

#endif

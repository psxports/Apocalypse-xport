#ifndef PRIMITIVE_PARTICLE_H
#define PRIMITIVE_PARTICLE_H

#include "vector3.h"
#include "texture_resources.h"
#include "object.h"
#include "effect_object.h"

struct HD_SPRITE_HISTORY;

// Shared defaults for primitive particles
struct ParticleSettings
{
    ShortVector3 angles;
    ShortVector3 spread;
    uint32 packed_size;
    bool semi_transparent;
    RGBC color;
    RGBC color_step;
};

// Base for primitive particles rendered from the particle list
class PrimitiveParticle : public EffectObject
{
public:
    virtual ~PrimitiveParticle();

    TILE primitive;
    mutable HD_SPRITE_HISTORY *hd_history;

protected:
    PrimitiveParticle();

public:
    virtual uint32 Destroy(uint32 reason);
    static void SetAngles(const ShortVector3 *angles);
    static void SetSpread(const ShortVector3 *angles);
    static uint32 BuildVelocity(Vector3 *output, uint32 scale);
    static uint32 SetSize(uint32 size);
    static void SetColor(uint8 r, uint8 g, uint8 b);
    static void SetColorStep(uint8 r, uint8 g, uint8 b);
    static void ResetSettings();
    static const ParticleSettings &GetSettings();
};

#endif

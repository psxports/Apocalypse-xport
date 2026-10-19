#ifndef PARTICLE_SETTINGS_H
#define PARTICLE_SETTINGS_H

extern "C"
{
#include "psx.h"
}

#include "vector3.h"

struct ParticleSettings
{
    ShortVector3 angles;
    ShortVector3 spread;
    uint32 packed_size;
    uint32 semi_transparent;
    RGBC color;
    RGBC color_step;
};

void ResetParticleSettings();
const ParticleSettings &GetParticleSettings();
void SetParticleAngles(const sint16 *angles);
void SetParticleSpread(const sint16 *angles);
uint32 BuildParticleVelocity(uint32 *output, uint32 scale);
uint32 SetParticleSize(uint32 size);
void SetParticleColor(uint8 red, uint8 green, uint8 blue);
void SetParticleColorStep(uint8 red, uint8 green, uint8 blue);

#endif

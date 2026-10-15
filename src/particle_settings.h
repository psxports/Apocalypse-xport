#ifndef PARTICLE_SETTINGS_H
#define PARTICLE_SETTINGS_H

extern "C"
{
#include "psx.h"
}

uint32 SetParticleDirection(const sint16 *angles);
uint32 SetParticleSpread(const sint16 *angles);
uint32 BuildParticleVelocity(uint32 *output, uint32 scale);
uint32 SetParticleSize(uint32 size);
void SetParticleColor(uint8 red, uint8 green, uint8 blue);
void SetParticleColorStep(uint8 red, uint8 green, uint8 blue);

#endif

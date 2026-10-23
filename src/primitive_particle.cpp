#include <string.h>

#include "primitive_particle.h"
#include "effect_lifecycle.h"
#include "global.h"

static ParticleSettings particle_settings;

PrimitiveParticle::PrimitiveParticle() : EffectObject()
{
    FUNCTION_MARKER(0x80034EF4, "SLUS_003.73");
    memset(&primitive, 0, sizeof(primitive));
    AddToList(effect_lists.particles);
}

uint32 PrimitiveParticle::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80034F38, "SLUS_003.73");
    RemoveFromList(effect_lists.particles);
    result = EffectObject::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void PrimitiveParticle::SetAngles(const ShortVector3 *angles)
{
    FUNCTION_MARKER(0x80034F9C, "SLUS_003.73");
    particle_settings.angles = *angles;
}

void PrimitiveParticle::SetSpread(const ShortVector3 *angles)
{
    FUNCTION_MARKER(0x80034FC4, "SLUS_003.73");
    particle_settings.spread = *angles;
}

uint32 PrimitiveParticle::BuildVelocity(Vector3 *output, uint32 scale)
{
    ShortVector3 angles;
    FUNCTION_MARKER(0x80034FEC, "SLUS_003.73");
    angles = particle_settings.angles;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        sint16 range = particle_settings.spread[axis];
        if (range)
        {
            const uint32 random = RandomRange(range);
            range = particle_settings.spread[axis];
            angles.comp[axis] = (uint16)angles.comp[axis] + (random - (range >> 1));
        }
    }
    return output->SetDir(scale, angles);
}

uint32 PrimitiveParticle::SetSize(uint32 size)
{
    FUNCTION_MARKER(0x800350E8, "SLUS_003.73");
    particle_settings.packed_size = (size << 16) | size;
    return particle_settings.packed_size;
}

void PrimitiveParticle::SetColor(uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x800350FC, "SLUS_003.73");
    particle_settings.color.r = r;
    particle_settings.color.g = g;
    particle_settings.color.b = b;
}

void PrimitiveParticle::SetColorStep(uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x80035110, "SLUS_003.73");
    particle_settings.color_step.r = r;
    particle_settings.color_step.g = g;
    particle_settings.color_step.b = b;
}

void PrimitiveParticle::ResetSettings()
{
    memset(&particle_settings, 0, sizeof(particle_settings));
    particle_settings.packed_size = 1;
    particle_settings.semi_transparent = true;
    particle_settings.color.rgbc = 0x00808080;
    particle_settings.color_step.rgbc = 0x08040404;
}

const ParticleSettings &PrimitiveParticle::GetSettings()
{
    return particle_settings;
}

#include <string.h>

#include "game_random.h"
extern "C"
{
#include "psx.h"
}
#include "particle_settings.h"
#include "angle_math.h"

static ParticleSettings particle_settings;

void ResetParticleSettings()
{
    memset(&particle_settings, 0, sizeof(particle_settings));
    particle_settings.packed_size = 1;
    particle_settings.semi_transparent = 1;
    particle_settings.color.rgbc = 0x00808080;
    particle_settings.color_step.rgbc = 0x08040404;
}

const ParticleSettings &GetParticleSettings()
{
    return particle_settings;
}

void SetParticleAngles(const sint16 *angles)
{
    FUNCTION_MARKER(0x80034F9C, "SLUS_003.73");
    const ShortVector3 input = {{{angles[0], angles[1], angles[2]}}};
    particle_settings.angles = input;
}

void SetParticleSpread(const sint16 *angles)
{
    FUNCTION_MARKER(0x80034FC4, "SLUS_003.73");
    const ShortVector3 input = {{{angles[0], angles[1], angles[2]}}};
    particle_settings.spread = input;
}

uint32 BuildParticleVelocity(uint32 *output, uint32 scale)
{
    ShortVector3 angles;
    FUNCTION_MARKER(0x80034FEC, "SLUS_003.73");
    angles = particle_settings.angles;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        sint32 range = particle_settings.spread[axis];
        if (range)
        {
            const uint32 random = RandomRange((uint32)range);
            range = particle_settings.spread[axis];
            angles.words[axis] = (uint32)angles.words[axis] + random - (uint32)(range >> 1);
        }
    }
    return BuildDirectionVector(output, scale, angles.words);
}

uint32 SetParticleSize(uint32 size)
{
    uint32 result;
    FUNCTION_MARKER(0x800350E8, "SLUS_003.73");
    result = (size << 16) | size;
    particle_settings.packed_size = result;
    return result;
}

void SetParticleColor(uint8 red, uint8 green, uint8 blue)
{
    FUNCTION_MARKER(0x800350FC, "SLUS_003.73");
    particle_settings.color.r = red;
    particle_settings.color.g = green;
    particle_settings.color.b = blue;
}

void SetParticleColorStep(uint8 red, uint8 green, uint8 blue)
{
    FUNCTION_MARKER(0x80035110, "SLUS_003.73");
    particle_settings.color_step.r = red;
    particle_settings.color_step.g = green;
    particle_settings.color_step.b = blue;
}

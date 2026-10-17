#include "game_random.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "particle_settings.h"
#include "angle_math.h"
#include "native_object_storage.h"

uint32 SetParticleDirection(const sint16 *angles)
{
    FUNCTION_MARKER(0x80034F9C, "SLUS_003.73");
    return CopyAngleVector((uint16 *)psx_addr(0x800FF414, 6), (const uint16 *)angles);
}

uint32 SetParticleSpread(const sint16 *angles)
{
    FUNCTION_MARKER(0x80034FC4, "SLUS_003.73");
    return CopyAngleVector((uint16 *)psx_addr(0x800FF41C, 6), (const uint16 *)angles);
}

uint32 BuildParticleVelocity(uint32 *output, uint32 scale)
{
    uint32 axis, random, packed;
    sint32 range;
    uint16 angles[3];
    FUNCTION_MARKER(0x80034FEC, "SLUS_003.73");
    range = (sint16)r_u16(0x800FF41C);
    // Preserve both aligned reads corresponding to the original LWL and LWR
    r_u32(0x800FF414);
    packed = r_u32(0x800FF414);
    angles[2] = r_u16(0x800FF418);
    angles[0] = (uint16)packed;
    angles[1] = (uint16)(packed >> 16);
    for (axis = 0; axis < 3; ++axis)
    {
        if (axis)
            range = (sint16)r_u16(0x800FF41C + axis * 2);
        if (range)
        {
            random = RandomRange((uint32)range);
            range = (sint16)r_u16(0x800FF41C + axis * 2);
            angles[axis] = (uint16)((uint32)angles[axis] + random - (uint32)(range >> 1));
        }
    }
    return BuildDirectionVector(output, scale, angles);
}

uint32 SetParticleSize(uint32 size)
{
    uint32 result;
    FUNCTION_MARKER(0x800350E8, "SLUS_003.73");
    result = (size << 16) | size;
    w_u32(0x800FF424, result);
    return result;
}

void SetParticleColor(uint8 red, uint8 green, uint8 blue)
{
    FUNCTION_MARKER(0x800350FC, "SLUS_003.73");
    w_u8(0x800FF42C, red);
    w_u8(0x800FF42D, green);
    w_u8(0x800FF42E, blue);
}

void SetParticleColorStep(uint8 red, uint8 green, uint8 blue)
{
    FUNCTION_MARKER(0x80035110, "SLUS_003.73");
    w_u8(0x800FF430, red);
    w_u8(0x800FF431, green);
    w_u8(0x800FF432, blue);
}

extern "C" uint32 sub_80034F9C(uint32 angles)
{
    FUNCTION_MARKER(0x80034F9C, "SLUS_003.73");
    return SetParticleDirection((const sint16 *)psx_addr(angles, 6));
}

extern "C" uint32 apocalypse_effect_angles_native(const void *angles) { return SetParticleSpread((const sint16 *)angles); }


extern "C" void sub_800350FC(uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x800350FC, "SLUS_003.73");
    SetParticleColor((uint8)red, (uint8)green, (uint8)blue);
}
extern "C" void sub_80035110(uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x80035110, "SLUS_003.73");
    SetParticleColorStep((uint8)red, (uint8)green, (uint8)blue);
}

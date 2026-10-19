#include <new>

#include "angle_math.h"
#include "game_lighting.h"
#include "model_render_view.h"
#include "game_diagnostics.h"
#include "orbit_camera.h"

OrbitCamera::OrbitCamera(const uint32 *position, uint32 radius, uint32 height) : phase(0)
{
    FUNCTION_MARKER(0x80011904, "SLUS_003.73");
    center.words[0] = position[0] << 12;
    center.words[1] = position[1] << 12;
    uint32 z = position[2];
    this->radius = radius;
    this->height = height;
    center.words[2] = z << 12;
    Update();
    game_lighting.background_color.b = 128;
    game_lighting.background_color.g = 128;
    game_lighting.background_color.r = 128;
}

uint32 OrbitCamera::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80011990, "SLUS_003.73");
    if (reason & 1)
        ReleaseStorage();
    return 0;
}

uint32 OrbitCamera::Advance()
{
    FUNCTION_MARKER(0x800119C4, "SLUS_003.73");
    phase += 4;
    return Update();
}

uint32 OrbitCamera::Update()
{
    FUNCTION_MARKER(0x800119F0, "SLUS_003.73");
    uint32 angle = phase & 4095;
    uint32 s = (uint32)rsin(angle);
    uint32 x = center.words[0] + radius * s;
    uint32 vertical = height << 12;
    uint32 y = center.words[1] - vertical;
    angle = phase & 4095;
    uint32 c = (uint32)rcos(angle);
    uint32 z = center.words[2];
    GetModelRenderView().pos.words[0] = (uint32)((sint32)x >> 12);
    GetModelRenderView().pos.words[1] = (uint32)((sint32)y >> 12);
    z += radius * c;
    GetModelRenderView().pos.words[2] = (uint32)((sint32)z >> 12);
    vertical = height << 12;
    sint32 slope;
    if (!radius)
        slope = (sint32)vertical < 0 ? 1 : -1;
    else if (vertical == 0x80000000 && radius == 0xFFFFFFFF)
        slope = (sint32)vertical;
    else
        slope = (sint32)vertical / (sint32)radius;
    GetModelRenderView().angles.words[0] = (uint16)(0 - (uint32)CalculateAngleFromSlope(slope));
    uint32 heading = (uint16)phase;
    GetModelRenderView().angles.words[2] = 0;
    uint32 result = heading + 2048;
    GetModelRenderView().angles.words[1] = (uint16)result;
    return result;
}

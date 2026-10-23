#include "angle_math.h"
#include "game_lighting.h"
#include "model_render_view.h"
#include "orbit_camera.h"
#include "global.h"
#include "camera.h"

OrbitCamera::OrbitCamera(const Vector3 *pos, uint32 radius, uint32 height) : phase(0)
{
    FUNCTION_MARKER(0x80011904, "SLUS_003.73");
    center.x = (uint32)pos->x << 12;
    center.y = (uint32)pos->y << 12;
    uint32 z = pos->z;
    this->radius = radius;
    this->height = height;
    center.z = z << 12;
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
    ModelRenderView &view = GetModelRenderView();
    uint32 angle = phase & 4095;
    sint32 s = rsin(angle);
    uint32 x = (uint32)center.x + radius * s;
    uint32 vertical = height << 12;
    uint32 y = (uint32)center.y - vertical;
    sint32 c = rcos(phase & 4095);
    uint32 z = (uint32)center.z + radius * c;
    view.pos.x = (sint32)x >> 12;
    view.pos.y = (sint32)y >> 12;
    view.pos.z = (sint32)z >> 12;
    sint32 slope = DivideSignedWord(vertical, radius);
    view.angles.x = -catan(slope);
    uint16 heading = phase;
    view.angles.z = 0;
    uint32 result = heading + 2048;
    view.angles.y = result;
    return result;
}

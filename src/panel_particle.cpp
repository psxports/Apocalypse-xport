#include "level_session.h"
#include "panel_particle.h"
#include "global.h"
#include "sprite.h"

PanelParticle::PanelParticle(const Vector3 *pos) : Sprite()
{
    sint32 velocity_x, velocity_y, random, floor;
    FUNCTION_MARKER(0x8005C11C, "SLUS_003.73");
    drag[0] = 4;
    drag[1] = 4;
    drag[2] = 4;
    this->pos = *pos;
    velocity_x = RandomRange(16);
    velocity_x = (velocity_x - 8) * ONE;
    velocity_y = RandomRange(2);
    velocity_y = -(velocity_y + 2) * ONE;
    random = RandomRange(16);
    velocity.x = velocity_x;
    velocity.y = velocity_y;
    velocity.z = (random - 8) * ONE;
    random = RandomRange(1);
    acceleration.x = 0;
    acceleration.y = -(random + 1) * ONE;
    acceleration.z = 0;
    floor = level_environment.water_y;
    if (this->pos.y < floor)
    {
        this->pos.y = floor;
        velocity.y = 0;
        acceleration.y = 0;
    }
    SelectResource(VFX_PANEL);
    SetFrame(2);
    random = RandomRange(1024);
    SetSize(random + 512);
    EnableSemiTransparency();
    random = RandomRange(64);
    intensity = random + 64;
}

uint32 PanelParticle::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8005C264, "SLUS_003.73");
    result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void PanelParticle::Update()
{
    uint8 intensity;
    sint32 floor;
    FUNCTION_MARKER(0x8005C2B8, "SLUS_003.73");
    intensity = this->intensity;
    if (intensity < 5)
    {
        Finish();
        return;
    }
    intensity -= 4;
    this->intensity = intensity;
    SetBrightness(intensity);
    floor = level_environment.water_y;
    if (pos.y < floor)
    {
        pos.y = floor;
        velocity.y = 0;
        acceleration.y = 0;
    }
    IntegrateMotion();
}

#include <new>

#include "level_session.h"
#include "game_diagnostics.h"
#include "panel_particle.h"
#include "effect_lifecycle.h"
#include "game_random.h"

PanelParticle::PanelParticle(const uint32 *position) : Sprite()
{
    uint32 x, y, z, velocity_x, velocity_y, random, floor;
    FUNCTION_MARKER(0x8005C11C, "SLUS_003.73");
    this->drag[0] = 4;
    this->drag[1] = 4;
    this->drag[2] = 4;
    x = position[0];
    y = position[1];
    z = position[2];
    this->pos.words[0] = x;
    this->pos.words[1] = y;
    this->pos.words[2] = z;
    velocity_x = (RandomRange(16) - 8) << 12;
    velocity_y = (0xFFFFFFFE - RandomRange(2)) << 12;
    random = RandomRange(16);
    this->velocity.words[0] = velocity_x;
    this->velocity.words[1] = velocity_y;
    this->velocity.words[2] = (random - 8) << 12;
    random = RandomRange(1);
    this->acceleration.words[0] = 0;
    this->acceleration.words[1] = (~random) << 12;
    this->acceleration.words[2] = 0;
    y = this->pos.words[1];
    floor = (uint32)level_environment.water_y;
    if ((sint32)y < (sint32)floor)
    {
        this->pos.words[1] = floor;
        this->velocity.words[1] = 0;
        this->acceleration.words[1] = 0;
    }
    SelectResource(VFX_PANEL);
    SetFrame(2);
    random = RandomRange(1024);
    SetSize((random + 512) & 0xFFFF);
    EnableSemiTransparency();
    random = RandomRange(64);
    intensity = random + 64;
}

uint32 PanelParticle::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8005C264, "SLUS_003.73");
    result = Sprite::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void PanelParticle::Update()
{
    uint32 intensity, y, floor;
    FUNCTION_MARKER(0x8005C2B8, "SLUS_003.73");
    intensity = this->intensity;
    if (intensity < 5)
    {
        Finish();
        return;
    }
    intensity = (intensity - 4) & 255;
    this->intensity = intensity;
    SetBrightness(intensity);
    y = this->pos.words[1];
    floor = (uint32)level_environment.water_y;
    if ((sint32)y < (sint32)floor)
    {
        this->pos.words[1] = floor;
        this->velocity.words[1] = 0;
        this->acceleration.words[1] = 0;
    }
    IntegrateMotion();
}

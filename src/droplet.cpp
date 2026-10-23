#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_diagnostics.h"
#include "visual_object.h"
#include "effect_splash.h"
#include "droplet.h"
#include "expanding_ring.h"
#include "global.h"
#include "glow_effect.h"
#include "effect_lifecycle.h"
#include "ring_effect.h"
#include "mesh_ring_effect.h"
#include "effect_object.h"
#include "sprite.h"

Droplet::Droplet(const Vector3 *pos, uint32 angle, uint8 r, uint8 g, uint8 b) : Sprite()
{
    FUNCTION_MARKER(0x8001F0D8, "SLUS_003.73");
    this->pos = *pos;
    floor_y = pos->y;
    SelectResource("droplet");
    SetFrame(0);
    EnableSemiTransparency();
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
    SetColors(r, g, b);
    size = 700;
    AddFlags(8);
    sint32 random = RandomRange(50);
    uint32 trig_angle = angle & 4095;
    velocity.x = rsin(trig_angle) * random;
    random = RandomRange(30);
    velocity.y = -(random + 20) * ONE;
    random = RandomRange(50);
    acceleration.y = 24576;
    velocity.z = rcos(trig_angle) * random;
    drag[0] = 3;
    drag[1] = 3;
    drag[2] = 3;
    uint32 val = (angle + 2048 - visual_lists.cameras->angles.y) & 4095;
    angle_speed = val < 2048 ? 137 : -137;
    this->angle = RandomRange(4096);
}

uint32 Droplet::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8001F280, "SLUS_003.73");
    result = Sprite::Destroy(0);
    if (reason & 1)
        return ReleaseStorage();
    return result;
}

void Droplet::Update()
{
    FUNCTION_MARKER(0x8001F2D4, "SLUS_003.73");
    sint16 age = this->age;
    angle += angle_speed;
    if (age)
        IntegrateMotion();
    uint8 r = 0, g = 0, b = 0;
    if (age < 20)
    {
        sint32 remaining = 20 - age;
        r = rgb[0] * remaining / 20;
        g = rgb[1] * remaining / 20;
        b = rgb[2] * remaining / 20;
    }
    SetColors(r, g, b);
    if (floor_y < pos.y)
    {
        if (!RandomRange(6))
        {
            Vector3 point = pos;
            point.y = floor_y;
            GameHeapBlock *effect = EffectObject::AllocateStorage(116);
            if (effect)
                new (effect) ExpandingRing(&point, 100, rgb[0], rgb[1], rgb[2], 10, 3);
        }
        Finish();
    }
    if (age >= 21)
        Finish();
    ++this->age;
}

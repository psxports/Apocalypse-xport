#include "opaque_fragment.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "effect_object.h"
#include "hd.h"

OpaqueFragment::OpaqueFragment(const Vector3 &pos, const Vector3 &velocity, sint32 floor, uint8 r, uint8 g, uint8 b, uint32 extent_x, uint32 extent_y, uint32 extent_z) : EffectObject(), bounce_count(0), lifetime(0), hd_history(0), floor_y(0), fade_step(0)
{
    uint32 extents[3] = {extent_x, extent_y, extent_z};
    base_color.rgbc = 0;
    draw_color.rgbc = 0;
    uint32 vertex, axis, random;
    FUNCTION_MARKER(0x80036D48, "SLUS_003.73");
    AddToList(effect_lists.opaque_fragments);
    SetPos(&pos);
    for (vertex = 0; vertex < 3; ++vertex)
        for (axis = 0; axis < 3; ++axis)
        {
            random = RandomRange(extents[axis] * 2 + 1);
            vertices[vertex].comp[axis] = (uint32)pos.comp[axis] + ((random - extents[axis]) << 12);
        }
    this->velocity = velocity;
    floor_y = floor;
    r >>= 2;
    g >>= 2;
    b >>= 2;
    base_color.r = r;
    base_color.g = g;
    draw_color.r = r;
    draw_color.g = g;
    base_color.b = b;
    draw_color.b = b;
    lifetime = RandomRange(30) + 30;
    fade_step = 4;
}

OpaqueFragment::~OpaqueFragment()
{
    hd_polygon_chain_free(hd_history);
}

uint32 OpaqueFragment::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80036F98, "SLUS_003.73");
    RemoveFromList(effect_lists.opaque_fragments);
    uint32 result = EffectObject::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void OpaqueFragment::Update()
{
    FUNCTION_MARKER(0x80036FFC, "SLUS_003.73");
    for (uint32 vertex = 0; vertex < 3; ++vertex)
        vertices[vertex].Add(velocity);
    velocity.y = (uint32)velocity.y + (10000);
    if (floor_y < vertices[0].y)
    {
        if (bounce_count < 2)
        {
            uint32 random = RandomRange(50);
            vertices[0].y = floor_y - (random << 12);
            random = RandomRange(50);
            vertices[1].y = floor_y - (random << 12);
            random = RandomRange(50);
            uint32 last_y = floor_y - (random << 12);
            // Reflect the low word before the signed shift
            sint32 reflected = -(uint32)velocity.y;
            vertices[2].y = last_y;
            velocity.y = reflected >> 2;
            ++bounce_count;
        }
        else
            velocity.y = 0;
        velocity.x >>= 1;
        velocity.z >>= 1;
    }
    if (bounce_count >= 2 || RandomRange(5))
        draw_color = base_color;
    else
    {
        draw_color.r = 48;
        draw_color.g = 48;
        draw_color.b = 64;
    }
    if (lifetime)
        --lifetime;
    if (!lifetime)
    {
        base_color.r = fade_step < base_color.r ? base_color.r - fade_step : 0;
        base_color.g = fade_step < base_color.g ? base_color.g - fade_step : 0;
        base_color.b = fade_step < base_color.b ? base_color.b - fade_step : 0;
    }
    if (!(base_color.r | base_color.g | base_color.b))
        Finish();
}

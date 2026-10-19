#include "opaque_fragment.h"
#include "game_random.h"
#include "effect_lifecycle.h"

OpaqueFragment::OpaqueFragment(const Vector3 &position, const Vector3 &velocity, uint32 floor, uint32 red, uint32 green, uint32 blue, uint32 extent_x, uint32 extent_y, uint32 extent_z) : EffectObject(), bounce_count(0), lifetime(0), floor_y(0), fade_step(0)
{
    uint32 extents[3] = {extent_x, extent_y, extent_z};
    base_color.rgbc = 0;
    draw_color.rgbc = 0;
    uint32 vertex, axis, random, value, x, y, z;
    FUNCTION_MARKER(0x80036D48, "SLUS_003.73");
    AddToList(effect_lists.opaque_fragments);
    SetPosition(position.words);
    for (vertex = 0; vertex < 3; ++vertex)
        for (axis = 0; axis < 3; ++axis)
        {
            random = RandomRange(extents[axis] * 2 + 1);
            value = position.words[axis] + ((random - extents[axis]) << 12);
            switch (axis)
            {
                case 0:
                    vertices[vertex].x = value;
                    break;
                case 1:
                    vertices[vertex].y = value;
                    break;
                case 2:
                    vertices[vertex].z = value;
                    break;
            }
        }
    x = velocity.words[0];
    y = velocity.words[1];
    z = velocity.words[2];
    this->velocity.words[0] = x;
    this->velocity.words[1] = y;
    this->velocity.words[2] = z;
    floor_y = floor;
    red = (red & 255) >> 2;
    green = (green & 255) >> 2;
    blue = (blue & 255) >> 2;
    base_color.r = red;
    base_color.g = green;
    draw_color.r = red;
    draw_color.g = green;
    base_color.b = blue;
    draw_color.b = blue;
    this->lifetime = RandomRange(30) + 30;
    fade_step = 4;
}

uint32 OpaqueFragment::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80036F98, "SLUS_003.73");
    RemoveFromList(effect_lists.opaque_fragments);
    uint32 result = EffectObject::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void OpaqueFragment::Update()
{
    uint32 velocity_y, position_y, floor, random, last_y, bounce, x, z, red, green, blue, lifetime, step;
    FUNCTION_MARKER(0x80036FFC, "SLUS_003.73");
    for (uint32 vertex = 0; vertex < 3; ++vertex)
    {
        vertices[vertex].x = (uint32)vertices[vertex].x + velocity.words[0];
        vertices[vertex].y = (uint32)vertices[vertex].y + velocity.words[1];
        vertices[vertex].z = (uint32)vertices[vertex].z + velocity.words[2];
    }
    velocity_y = this->velocity.words[1];
    position_y = vertices[0].y;
    floor = floor_y;
    this->velocity.words[1] = velocity_y + 10000;
    if ((sint32)floor < (sint32)position_y)
    {
        if ((sint16)bounce_count < 2)
        {
            random = RandomRange(50);
            vertices[0].y = (uint32)floor_y - (random << 12);
            random = RandomRange(50);
            vertices[1].y = (uint32)floor_y - (random << 12);
            random = RandomRange(50);
            floor = floor_y;
            velocity_y = this->velocity.words[1];
            last_y = floor - (random << 12);
            bounce = bounce_count;
            velocity_y = (uint32)((sint32)((uint32)0 - velocity_y) >> 2);
            vertices[2].y = last_y;
            this->velocity.words[1] = velocity_y;
            bounce_count = bounce + 1;
        }
        else
            this->velocity.words[1] = 0;
        x = this->velocity.words[0];
        z = this->velocity.words[2];
        this->velocity.words[0] = (sint32)x >> 1;
        this->velocity.words[2] = (sint32)z >> 1;
    }
    if ((sint16)bounce_count >= 2 || RandomRange(5))
    {
        red = base_color.r;
        green = base_color.g;
        blue = base_color.b;
        draw_color.r = red;
        draw_color.g = green;
        draw_color.b = blue;
    }
    else
    {
        draw_color.r = 48;
        draw_color.g = 48;
        draw_color.b = 64;
    }
    lifetime = this->lifetime;
    if (lifetime)
    {
        lifetime = (lifetime - 1) & 65535;
        this->lifetime = lifetime;
    }
    if (!lifetime)
    {
        red = base_color.r;
        step = fade_step;
        red = step < red ? red - step : 0;
        green = base_color.g;
        step = fade_step;
        base_color.r = red;
        green = step < green ? green - step : 0;
        blue = base_color.b;
        step = fade_step;
        base_color.g = green;
        blue = step < blue ? blue - step : 0;
        base_color.b = blue;
    }
    red = base_color.r;
    green = base_color.g;
    blue = base_color.b;
    if (!(red | green | blue))
        Finish();
}

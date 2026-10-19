#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_diagnostics.h"
#include "visual_object.h"

#include "effect_splash.h"
#include "expanding_ring.h"
#include "game_random.h"
#include "glow_effect.h"
#include "vector_arithmetic.h"
#include "effect_lifecycle.h"

Droplet::Droplet(const uint32 *position, uint32 angle, uint32 red, uint32 green, uint32 blue) : Sprite()
{
    uint32 trig_angle;
    uint32 x, y, z, random, coefficient, value;
    FUNCTION_MARKER(0x8001F0D8, "SLUS_003.73");
    x = position[0];
    y = position[1];
    z = position[2];
    this->pos.words[0] = x;
    this->pos.words[1] = y;
    this->pos.words[2] = z;
    floor_y = position[1];
    SelectResource("droplet");
    SetFrame(0);
    EnableSemiTransparency();
    rgb[0] = red;
    rgb[1] = green;
    rgb[2] = blue;
    SetColors(red & 255, green & 255, blue & 255);
    this->size = 700;
    AddFlags(8);
    random = RandomRange(50);
    trig_angle = angle & 4095;
    coefficient = (uint32)rsin(trig_angle);
    this->velocity.words[0] = coefficient * random;
    random = RandomRange(30);
    this->velocity.words[1] = 0 - ((random + 20) << 12);
    random = RandomRange(50);
    coefficient = (uint32)rcos(trig_angle);
    value = coefficient * random;
    this->acceleration.words[1] = 24576;
    this->drag[0] = 3;
    this->velocity.words[2] = value;
    this->drag[1] = 3;
    this->drag[2] = 3;
    value = (uint32)(sint32)(sint16)visual_lists.cameras->angles.words[1];
    value = (angle + 2048 - value) & 4095;
    angle_speed = value < 2048 ? 137 : (uint32)-137;
    this->angle = RandomRange(4096);
}

uint32 CreateSplash(const uint32 *position)
{
    uint32 trig_angle;
    Vector3 point;
    GameHeapBlock *effect;
    uint32 index;
    uint32 angle;
    uint32 first;
    uint32 second;
    uint32 x;
    uint32 z;
    sint32 quotient;
    FUNCTION_MARKER(0x8001F52C, "SLUS_003.73");
    point.words[0] = position[0];
    point.words[1] = position[1];
    point.words[2] = position[2];
    ExpandingGlow *glow = 0;
    effect = AllocateEffectStorage(124);
    if (effect)
    {
        glow = new (effect) ExpandingGlow(position, 52, 102, 82, 90, 15);
    }
    if (!glow)
    {
        GameDiagnostic("Splash glow allocation failed\n");
        abort();
    }
    glow->seg_mask = (uint32)-505;
    glow->angle = 0;
    for (index = 0; index < 30; ++index)
    {
        angle = RandomRange(4096);
        trig_angle = angle & 4095;
        first = (uint32)rsin(trig_angle) * 100;
        second = (uint32)rcos(trig_angle) * 100;
        x = position[0];
        quotient = (sint32)first / 4096;
        point.words[0] = x + ((uint32)quotient << 12);
        z = position[2];
        quotient = (sint32)second / 4096;
        point.words[2] = z + ((uint32)quotient << 12);
        effect = AllocateEffectStorage(120);
        if (effect)
        {
            new (effect) Droplet(point.words, angle, 16, 48, 16);
        }
    }
    return 0;
}

uint32 Droplet::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8001F280, "SLUS_003.73");
    result = Sprite::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(this);
    return result;
}

void Droplet::Update()
{
    uint32 age, angle, velocity_x, velocity_y, velocity_z, shift_y, shift_x, shift_z;
    uint32 x;
    uint32 y;
    uint32 z;
    uint32 acceleration;
    uint32 red;
    uint32 green;
    uint32 blue;
    GameHeapBlock *effect;
    uint32 result;
    Vector3 point;
    FUNCTION_MARKER(0x8001F2D4, "SLUS_003.73");
    angle = this->angle;
    acceleration = angle_speed;
    age = (uint32)(sint32)(sint16)this->age;
    this->angle = angle + acceleration;
    if (age)
    {
        x = this->pos.words[0];
        velocity_x = this->velocity.words[0];
        velocity_y = this->velocity.words[1];
        velocity_z = this->velocity.words[2];
        shift_y = this->drag[1];
        this->pos.words[0] = x + velocity_x;
        y = this->pos.words[1];
        z = this->pos.words[2];
        this->pos.words[1] = y + velocity_y;
        velocity_x = this->velocity.words[0];
        acceleration = this->acceleration.words[0];
        this->pos.words[2] = z + velocity_z;
        velocity_y = this->velocity.words[1];
        velocity_y += this->acceleration.words[1];
        this->velocity.words[0] = velocity_x + acceleration;
        velocity_z = this->velocity.words[2];
        acceleration = this->acceleration.words[2];
        shift_x = this->drag[0];
        this->velocity.words[1] = velocity_y;
        velocity_x = this->velocity.words[0];
        velocity_z += acceleration;
        velocity_y = this->velocity.words[1];
        this->velocity.words[0] = velocity_x - (uint32)((sint32)velocity_x >> (shift_x & 31));
        shift_z = this->drag[2];
        this->velocity.words[2] = velocity_z;
        this->velocity.words[1] = velocity_y - (uint32)((sint32)velocity_y >> (shift_y & 31));
        this->velocity.words[2] = velocity_z - (uint32)((sint32)velocity_z >> (shift_z & 31));
    }
    age = (uint32)(sint32)(sint16)this->age;
    red = green = blue = 0;
    if ((sint32)age < 20)
    {
        result = 20 - age;
        red = rgb[0] * result;
        green = rgb[1] * result;
        blue = rgb[2] * result;
        red = (uint32)((sint32)red / 20) & 255;
        green = (uint32)((sint32)green / 20) & 255;
        blue = (uint32)((sint32)blue / 20) & 255;
    }
    SetColors(red, green, blue);
    y = this->pos.words[1];
    if ((sint32)floor_y < (sint32)y)
    {
        if (!RandomRange(6))
        {
            point.components[0] = (sint32)this->pos.words[0];
            point.components[1] = (sint32)this->pos.words[1];
            point.components[2] = (sint32)this->pos.words[2];
            point.components[1] = (sint32)floor_y;
            effect = AllocateEffectStorage(116);
            if (effect)
            {
                red = rgb[0];
                green = rgb[1];
                blue = rgb[2];
                new (effect) ExpandingRing(point.components, 100, red, green, blue, 10, 3);
            }
        }
        Finish();
    }
    if ((sint16)this->age >= 21)
        Finish();
    result = this->age + 1;
    this->age = result;
    return;
}

// TODO Original constructor boundary is inline; retain only its shared initialization block
ImpactRing::ImpactRing(const uint32 velocity[3]) : MeshRingEffect(12, 0xCCE0187C), age(0), lifetime(0), radius_step(0), width_step(0)
{
    uint32 x = velocity[0], y = velocity[1], z = velocity[2];
    this->velocity.words[0] = x;
    this->velocity.words[1] = y;
    this->velocity.words[2] = z;
}

void ImpactRing::Update()
{
    FUNCTION_MARKER(0x8005C47C, "SLUS_003.73");
    ++age;
    if (lifetime < (sint16)age)
    {
        Finish();
        return;
    }
    AddVectors(pos.words, velocity.words);
    SetGeometry(pos.words, radius + (uint32)(sint32)radius_step, expansion + (uint32)(sint32)width_step);
    uint32 value = lifetime - (uint32)(sint32)(sint16)age;
    value = (value << 8) - value;
    uint32 color = lifetime ? (uint32)((sint32)value / (sint32)lifetime) : ((sint32)value < 0 ? 1 : (uint32)-1);
    SetColors(color, color, (sint32)color >> 1);
}

uint32 ImpactRing::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8005C538, "SLUS_003.73");
    result = MeshRingEffect::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(this);
    return result;
}

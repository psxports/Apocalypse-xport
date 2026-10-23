extern "C"
{
#include "psx.h"
}
#include "fading_particle.h"
#include "effect_object.h"
#include "primitive_particle.h"
#include "global.h"
#include "effect_lifecycle.h"

FadingParticle::FadingParticle(const Vector3 *src_pos, uint32 speed, sint32 vertical_accel, uint32 lifetime_range) : PrimitiveParticle(), age(0), lifetime(0)
{
    const ParticleSettings &settings = PrimitiveParticle::GetSettings();
    uint32 size = settings.packed_size & 15;
    FUNCTION_MARKER(0x80035124, "SLUS_003.73");
    if (size == 1)
        setTile1(&primitive);
    else
    {
        setTile(&primitive);
        size = settings.packed_size;
    }
    primitive.w = size;
    primitive.h = size >> 16;
    primitive.rgbc.r = settings.color.r;
    primitive.rgbc.g = settings.color.g;
    primitive.rgbc.b = settings.color.b;
    fade_step.rgbc = 0;
    fade_step.r = settings.color_step.r;
    fade_step.g = settings.color_step.g;
    fade_step.b = settings.color_step.b;
    pos = *src_pos;
    PrimitiveParticle::BuildVelocity(&velocity, speed);
    acceleration.y = vertical_accel;
    drag[0] = 3;
    drag[1] = 3;
    drag[2] = 3;
    lifetime = RandomRange(lifetime_range);
    if (settings.semi_transparent)
        setSemiTrans(&primitive, 1);
}

uint32 FadingParticle::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8003525C, "SLUS_003.73");
    result = PrimitiveParticle::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void FadingParticle::Update()
{
    FUNCTION_MARKER(0x800352B0, "SLUS_003.73");
    pos.Add(velocity);
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        const sint32 speed = (uint32)velocity.comp[axis] + acceleration.comp[axis];
        velocity.comp[axis] = speed - (speed >> 3);
    }
    ++age;
    if (age >= lifetime)
        Finish();
    primitive.rgbc.r -= fade_step.r;
    primitive.rgbc.g -= fade_step.g;
    primitive.rgbc.b -= fade_step.b;
}

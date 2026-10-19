#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"

#include "effect_object.h"
#include "game_diagnostics.h"
#include "beam_endpoint.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "vector_arithmetic.h"
#include "angle_math.h"

EndpointBeam::EndpointBeam(uint32 red, uint32 green, uint32 blue, uint32 count, uint32 duration, uint32 radius0, uint32 radius1, uint32 point_phase0, uint32 tick_phase0, uint32 point_phase1, uint32 tick_phase1) : BeamSegments(count + 1, 0), child(), phase(0)
{
    FUNCTION_MARKER(0x80035DC8, "SLUS_003.73");
    wave[0].radius = radius0;
    wave[1].radius = radius1;
    wave[0].point_phase = point_phase0;
    wave[1].point_phase = point_phase1;
    wave[0].tick_phase = tick_phase0;
    wave[1].tick_phase = tick_phase1;
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        origin.words[axis] = 0;
        step.words[axis] = 0;
        basis[0].words[axis] = 0;
        basis[1].words[axis] = 0;
    }
    phase = RandomRange(4096);
    red &= 255;
    blue &= 255;
    green &= 255;
    SetColors(red, green, blue);
    points[point_count - 1].rgbc.r = red >> 1;
    points[0].rgbc.r = red >> 1;
    points[point_count - 1].rgbc.g = green >> 1;
    points[0].rgbc.g = green >> 1;
    points[point_count - 1].rgbc.b = blue >> 1;
    points[0].rgbc.b = blue >> 1;
    sint32 interval = (sint32)duration;
    SetSize((uint32)(interval / 20) & 65535);
    points[point_count - 1].size = interval / 40;
    points[0].size = interval / 40;
    red *= 2;
    green *= 2;
    blue *= 2;
    if (red >= 256)
        red = 255;
    if (green >= 256)
        green = 255;
    if (blue >= 256)
        blue = 255;
    SetChildColors(red, green, blue, interval / 80);
}

EndpointBeam::~EndpointBeam()
{
    if (child)
        child->Destroy(3);
}

uint32 EndpointBeam::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80035FC0, "SLUS_003.73");
    if (child)
        child->Destroy(3);
    child = 0;
    BeamSegments::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

uint32 EndpointBeam::SetChildColors(uint32 red, uint32 green, uint32 blue, uint32 size)
{
    FUNCTION_MARKER(0x8003603C, "SLUS_003.73");
    if (child)
        child->Destroy(3);
    child = 0;
    GameHeapBlock *allocation = AllocateEffectStorage(84);
    if (!allocation)
    {
        GameDiagnostic("Endpoint beam child allocation failed\n");
        abort();
    }
    child = new (allocation) BeamSegments(point_count, 0);
    child->persistent = 1;
    child->SetColors(red & 255, green & 255, blue & 255);
    return child->SetSize(size & 65535);
}

uint32 EndpointBeam::SetEndpoints(const uint32 *origin, const uint32 *target)
{
    FUNCTION_MARKER(0x800360FC, "SLUS_003.73");
    Vector3 point, delta, part;
    for (uint32 axis = 0; axis < 3; ++axis)
        point.words[axis] = origin[axis];
    this->origin = point;
    SubtractVectors(delta.words, target, origin);
    uint32 divisor = point_count - 1;
    DivideVector(part.words, delta.words, divisor);
    step = part;
    ShortVector3 angles;
    CalculateDirectionAngles(angles.words, origin, target);
    angles.words[1] = (angles.words[1] + 1024) & 4095;
    BuildDirectionVector(basis[0].words, wave[0].radius, angles.words);
    angles.words[1] = (angles.words[1] - 1024) & 4095;
    angles.words[0] = (angles.words[0] + 1024) & 4095;
    return BuildDirectionVector(basis[1].words, wave[1].radius, angles.words);
}

static uint32 ReadEndpointSine(uint32 angle)
{
    return (uint32)rsin(angle & 4095);
}

void EndpointBeam::Update()
{
    FUNCTION_MARKER(0x80036200, "SLUS_003.73");
    uint32 last = point_count - 1;
    for (uint32 axis = 0; axis < 3; ++axis)
        points[0].pos.words[axis] = origin.words[axis];
    Vector3 current;
    for (uint32 axis = 0; axis < 3; ++axis)
        points[last].pos.words[axis] = origin.words[axis] + step.words[axis] * last;
    for (uint32 axis = 0; axis < 3; ++axis)
        current.words[axis] = origin.words[axis] + step.words[axis];
    for (uint32 i = 1; i < last; ++i)
    {
        if (!RandomRange(15))
            phase = RandomRange(4096);
        Vector3 shifted;
        for (uint32 axis = 0; axis < 3; ++axis)
            shifted.words[axis] = (sint32)step.words[axis] >> 6;
        uint32 tick = game_clock.tick;
        uint32 scalar = ReadEndpointSine(phase + tick * 130);
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            uint32 product = shifted.words[axis] * scalar;
            points[i].pos.words[axis] = current.words[axis] + (uint32)((sint32)product / 64);
        }
        Vector3 offset[2];
        uint32 value = i * wave[0].point_phase;
        uint32 angle = game_clock.tick * (wave[0].tick_phase * 2);
        scalar = ReadEndpointSine(value + angle + phase);
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            uint32 product = basis[0].words[axis] * scalar;
            offset[0].words[axis] = (sint32)product / 4096;
        }
        value = i * wave[1].point_phase;
        angle = game_clock.tick * (wave[1].tick_phase * 2);
        scalar = ReadEndpointSine(value + angle + phase);
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            uint32 product = basis[1].words[axis] * scalar;
            offset[1].words[axis] = (sint32)product / 4096;
        }
        for (uint32 axis = 0; axis < 3; ++axis)
            points[i].pos.words[axis] += offset[0].words[axis] + offset[1].words[axis];
        for (uint32 axis = 0; axis < 3; ++axis)
            current.words[axis] += step.words[axis];
    }
    if (child)
    {
        for (uint32 i = 0; i < point_count; ++i)
        {
            uint32 x = points[i].pos.words[0];
            uint32 y = points[i].pos.words[1];
            uint32 z = points[i].pos.words[2];
            child->points[i].pos.words[0] = x;
            child->points[i].pos.words[1] = y;
            child->points[i].pos.words[2] = z;
        }
    }
}

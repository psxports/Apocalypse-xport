#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"
#include "effect_object.h"
#include "game_diagnostics.h"
#include "endpoint_beam.h"
#include "effect_lifecycle.h"
#include "global.h"

EndpointBeam::EndpointBeam(uint8 r, uint8 g, uint8 b, uint32 count, sint32 width, uint32 radius0, uint32 radius1, uint32 point_phase0, uint32 tick_phase0, uint32 point_phase1, uint32 tick_phase1) : BeamSegments(count + 1, 0), child(), phase(0)
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
        origin.comp[axis] = 0;
        step.comp[axis] = 0;
        basis[0].comp[axis] = 0;
        basis[1].comp[axis] = 0;
    }
    phase = RandomRange(4096);
    SetColors(r, g, b);
    points[point_count - 1].rgbc.r = r >> 1;
    points[0].rgbc.r = r >> 1;
    points[point_count - 1].rgbc.g = g >> 1;
    points[0].rgbc.g = g >> 1;
    points[point_count - 1].rgbc.b = b >> 1;
    points[0].rgbc.b = b >> 1;
    SetSize(width / 20);
    points[point_count - 1].size = width / 40;
    points[0].size = width / 40;
    uint16 child_rgb[3];
    child_rgb[0] = r * 2;
    child_rgb[1] = g * 2;
    child_rgb[2] = b * 2;
    for (uint32 i = 0; i < 3; ++i)
    {
        if (child_rgb[i] >= 256)
            child_rgb[i] = 255;
    }
    RecreateChild(child_rgb[0], child_rgb[1], child_rgb[2], width / 80);
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
    return reason & 1 ? ReleaseStorage() : 0;
}

uint32 EndpointBeam::RecreateChild(uint8 r, uint8 g, uint8 b, uint16 size)
{
    FUNCTION_MARKER(0x8003603C, "SLUS_003.73");
    if (child)
        child->Destroy(3);
    child = 0;
    GameHeapBlock *allocation = EffectObject::AllocateStorage(84);
    if (!allocation)
    {
        GameDiagnostic(("Endpoint beam child allocation failed\n"));
        abort();
    }
    child = new (allocation) BeamSegments(point_count, 0);
    child->persistent = true;
    child->SetColors(r, g, b);
    return child->SetSize(size);
}

uint32 EndpointBeam::SetEndpoints(const Vector3 *origin, const Vector3 *target)
{
    FUNCTION_MARKER(0x800360FC, "SLUS_003.73");
    Vector3 delta;
    this->origin = *origin;
    delta.Subtract(*target, *origin);
    delta.Divide(point_count - 1);
    step = delta;
    ShortVector3 angles;
    angles.SetDir(*origin, *target);
    angles.y = ((uint16)angles.y + 1024) & 4095;
    basis[0].SetDir(wave[0].radius, angles);
    angles.y = ((uint16)angles.y - 1024) & 4095;
    angles.x = ((uint16)angles.x + 1024) & 4095;
    return basis[1].SetDir(wave[1].radius, angles);
}

void EndpointBeam::Update()
{
    FUNCTION_MARKER(0x80036200, "SLUS_003.73");
    sint32 last = point_count - 1;
    points[0].pos = origin;
    Vector3 endpoint, cur;
    endpoint.Scale(last, step);
    endpoint.Add(origin, endpoint);
    points[last].pos = endpoint;
    cur.Add(origin, step);
    for (sint32 i = 1; i < last; ++i)
    {
        if (!RandomRange(15))
            phase = RandomRange(4096);
        Vector3 shifted;
        shifted.ShiftRight(step, 6);
        uint32 tick = game_clock.tick;
        sint32 scalar = rsin((phase + tick * 130) & 4095);
        shifted.Multiply(shifted, scalar);
        shifted.Divide(64);
        points[i].pos.Add(cur, shifted);
        Vector3 offset[2];
        uint32 val = i * wave[0].point_phase;
        uint32 angle = game_clock.tick * (wave[0].tick_phase * 2);
        scalar = rsin((val + angle + phase) & 4095);
        offset[0].Multiply(basis[0], scalar);
        offset[0].Divide(ONE);
        val = i * wave[1].point_phase;
        angle = game_clock.tick * (wave[1].tick_phase * 2);
        scalar = rsin((val + angle + phase) & 4095);
        offset[1].Multiply(basis[1], scalar);
        offset[1].Divide(ONE);
        offset[0].Add(offset[1]);
        points[i].pos.Add(offset[0]);
        cur.Add(step);
    }
    if (child)
    {
        for (sint32 i = 0; i < point_count; ++i)
            child->points[i].pos = points[i].pos;
    }
}

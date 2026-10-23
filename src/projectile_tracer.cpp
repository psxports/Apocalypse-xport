#include <new>
#include <string.h>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "effect_object.h"
#include "line_effect.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "angle_math.h"
#include "ring_burst_effect.h"
#include "collision.h"
#include "object_damage.h"
#include "global.h"
#include "ring_effect.h"
#include "projectile_line.h"
#include "projectile_tracer.h"
#include "model_projection.h"
#include "vector3.h"
#include "smoke_emitter.h"
#include "effect_lifecycle.h"

ProjectileTracer::ProjectileTracer(const Vector3 *start, const Vector3 *end) : LineEffect(), age(0)
{
    Vector3 end_pos, dir, scaled, delta, movement;
    sint32 distance;
    uint32 offset;
    FUNCTION_MARKER(0x800234CC, "SLUS_003.73");
    end_pos = *end;
    dir.Subtract(end_pos, *start);
    distance = dir.Len();
    if (distance)
        dir.Divide(distance);
    else
        Finish();
    if (distance >= 3001)
    {
        distance = 3000;
        scaled.Scale(distance, dir);
        end_pos.Add(*start, scaled);
    }
    offset = RandomRange(distance / 3);
    scaled.Multiply(dir, offset);
    endpoint[0].Add(*start, scaled);
    delta.Subtract(end_pos, endpoint[0]);
    movement.Divide(delta, 5);
    endpoint[1].Add(endpoint[0], movement);
    velocity.ShiftLeft(movement, 1);
    color[0] = (color[0] & 0xFF000000) | 0x2000000;
}

void ProjectileTracer::Update()
{
    FUNCTION_MARKER(0x800236D4, "SLUS_003.73");
    if (age)
    {
        endpoint[0].Add(velocity);
        endpoint[1].Add(velocity);
    }
    ++age;
    if (age == 4)
        Finish();
    FadePackedColor(color[1], 16);
}

uint32 ProjectileTracer::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002BC44, "SLUS_003.73");
    uint32 result = LineEffect::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

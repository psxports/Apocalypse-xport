#include <new>

#include "game_tick.h"
#include "game_diagnostics.h"
#include "bouncing_point_effect.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "game_random.h"
#include "audio_session.h"
#include "collision.h"

BouncingPointEffect::BouncingPointEffect(const uint32 *position, const uint16 *angles) : PointSegments(1)
{
    CollisionRay &shared_ray = GetSharedCollisionRay();
    Vector3 direction, point, probe;
    uint32 random, value;
    ShortVector3 rotation;
    FUNCTION_MARKER(0x8002374C, "SLUS_003.73");
    points[0].rgbc.r = 64;
    points[0].rgbc.g = 64;
    points[0].rgbc.b = 30;
    BuildDirectionVector(this->dir.words, 3, angles);
    BuildDirectionVector(direction.words, (uint32)-45, angles);
    AddVectors(point.words, position, direction.words);
    this->pos = point;
    AddVectors(point.words, this->pos.words, this->dir.words);
    this->origin = point;

    SubtractVectors(point.words, this->pos.words, this->dir.words);
    points[0].pos = point;
    CopyAngleVector(rotation.words, angles);
    rotation.words[1] = (uint16)(rotation.words[1] + 1024);
    random = RandomRange(4);
    BuildDirectionVector(this->velocity.words, random + 10, rotation.words);
    value = this->velocity.words[1];
    this->velocity.words[1] = value - 40960;
    BuildDirectionVector(probe.words, 145, rotation.words);
    AddVectors(probe.words, position);
    shared_ray.endpoints[0].words[0] = (probe.words[0]);
    shared_ray.endpoints[0].words[1] = (probe.words[1]);
    shared_ray.endpoints[1].words[1] = (probe.words[1]);
    shared_ray.endpoints[0].words[2] = (probe.words[2]);
    shared_ray.endpoints[1].words[0] = (probe.words[0]);
    shared_ray.endpoints[1].words[2] = (probe.words[2]);
    shared_ray.endpoints[1].words[1] = (probe.words[1] + 4096000);
    InitializeCollisionRay(shared_ray);
    QueryCollisionRay(shared_ray, 1);
    if (shared_ray.hit.GetObject())
        value = shared_ray.hit_pos.words[1];
    else
        value = 0x7FFFFFFF;
    this->floor_y = value;
    value = game_clock.active_frame;
    this->spawn_tick = value;
}

uint32 BouncingPointEffect::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8002BBF0, "SLUS_003.73");
    result = PointSegments::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void BouncingPointEffect::Update()
{
    FUNCTION_MARKER(0x8002396C, "SLUS_003.73");
    uint32 frame = game_clock.active_frame;
    if (frame - spawn_tick >= 120)
    {
        frame = game_clock.active_frame;
        if (frame - spawn_tick >= 121)
            Finish();
        return;
    }
    AddVectors(pos.words, velocity.words);
    if ((sint32)floor_y < (sint32)pos.words[1])
    {
        pos.words[1] = floor_y;
        velocity.words[1] = 0 - velocity.words[1];
        uint32 shift = 1;
        ShiftVectorRight(velocity.words, shift);
        if (velocity.words[1] + 8192 >= 16385)
        {
            ShortVector3 angles;
            angles.words[2] = 0;
            angles.words[0] = 0;
            angles.words[1] = RandomRange(4096);
            uint32 random = RandomRange(4);
            if (!random)
            {
                random = RandomRange(3);
                PlayPositionedSound((sint32)(random + 12), pos.words, 0);
            }
            BuildDirectionVector(dir.words, 3, angles.words);
            random = RandomRange(1025);
            Vector3 rotated;
            RotateVectorYaw(rotated.words, velocity.words, random - 512);
            velocity = rotated;
        }
    }
    Vector3 point;
    AddVectors(point.words, pos.words, dir.words);
    origin = point;
    SubtractVectors(point.words, pos.words, dir.words);
    points[0].pos = point;
    velocity.words[1] += 14792;
}

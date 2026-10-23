#include "collision_ray.h"
#include "game_tick.h"
#include "bouncing_point_effect.h"
#include "global.h"
#include "audio_session.h"
#include "collision.h"

BouncingPointEffect::BouncingPointEffect(const Vector3 *pos, const ShortVector3 *angles) : PointSegments(1)
{
    CollisionRay &shared_ray = CollisionRay::GetShared();
    Vector3 dir, probe;
    uint32 random;
    ShortVector3 rotation;
    FUNCTION_MARKER(0x8002374C, "SLUS_003.73");
    points[0].rgbc.r = 64;
    points[0].rgbc.g = 64;
    points[0].rgbc.b = 30;
    this->dir.SetDir(3, *angles);
    dir.SetDir(-45, *angles);
    this->pos.Add(*pos, dir);
    origin.Add(this->pos, this->dir);
    points[0].pos.Subtract(this->pos, this->dir);
    rotation = *angles;
    rotation.y = (uint16)rotation.y + (1024);
    random = RandomRange(4);
    velocity.SetDir(random + 10, rotation);
    velocity.y = (uint32)velocity.y - (40960);
    probe.SetDir(145, rotation);
    probe.Add(*pos);
    shared_ray.endpoints[0].x = probe.x;
    shared_ray.endpoints[0].y = probe.y;
    shared_ray.endpoints[1].y = probe.y;
    shared_ray.endpoints[0].z = probe.z;
    shared_ray.endpoints[1].x = probe.x;
    shared_ray.endpoints[1].z = probe.z;
    shared_ray.endpoints[1].y = (uint32)probe.y + 4096000;
    shared_ray.Initialize();
    shared_ray.Query(1);
    if (shared_ray.hit.GetObject())
        floor_y = shared_ray.hit_pos.y;
    else
        floor_y = 0x7FFFFFFF;
    spawn_tick = game_clock.active_frame;
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
    pos.Add(velocity);
    if (floor_y < pos.y)
    {
        pos.y = floor_y;
        velocity.y = -(uint32)velocity.y;
        velocity.ShiftRight(1);
        if ((uint32)velocity.y + 8192 >= 16385)
        {
            ShortVector3 angles;
            angles.z = 0;
            angles.x = 0;
            angles.y = RandomRange(4096);
            uint32 random = RandomRange(4);
            if (!random)
            {
                random = RandomRange(3);
                PlayPositionedSound(random + 12, &pos, 0);
            }
            dir.SetDir(3, angles);
            random = RandomRange(1025);
            Vector3 rotated;
            rotated.RotateYaw(velocity, random - 512);
            velocity = rotated;
        }
    }
    origin.Add(pos, dir);
    points[0].pos.Subtract(pos, dir);
    velocity.y = (uint32)velocity.y + (14792);
}

uint32 BouncingPointEffect::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8002BBF0, "SLUS_003.73");
    result = PointSegments::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

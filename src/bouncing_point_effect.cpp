extern "C"
{
#include "draft_first_signatures.h"
}
#include "bouncing_point_effect.h"
#include "native_object_storage.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "game_random.h"
#include "audio_session.h"
#include "object_collision_events.h"
#include <new>

BouncingPointEffect::BouncingPointEffect(const uint32 *position, const uint16 *angles) : PointSegments(1)
{
    CollisionRay &shared_ray = GetSharedCollisionRay();
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 direction[3], point[3], probe[3], child, random, value;
    uint16 rotation[3];
    FUNCTION_MARKER(0x8002374C, "SLUS_003.73");
    w_u32(object + 68, 0x800A1518);
    child = r_u32(object + 80);
    w_u8(child + 12, 64);
    child = r_u32(object + 80);
    w_u8(child + 13, 64);
    child = r_u32(object + 80);
    w_u8(child + 14, 30);
    BuildDirectionVector((uint32 *)psx_addr(object + 96, 12), 3, angles);
    BuildDirectionVector(direction, (uint32)-45, angles);
    AddVectors(point, position, direction);
    w_u32(object + 24, point[0]);
    w_u32(object + 28, point[1]);
    w_u32(object + 32, point[2]);
    AddVectors(point, (const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 96, 12));
    w_u32(object + 84, point[0]);
    w_u32(object + 88, point[1]);
    w_u32(object + 92, point[2]);
    child = r_u32(object + 80);
    SubtractVectors(point, (const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 96, 12));
    w_u32(child, point[0]);
    w_u32(child + 4, point[1]);
    w_u32(child + 8, point[2]);
    CopyAngleVector(rotation, angles);
    rotation[1] = (uint16)(rotation[1] + 1024);
    random = RandomRange(4);
    BuildDirectionVector((uint32 *)psx_addr(object + 36, 12), random + 10, rotation);
    value = r_u32(object + 40);
    w_u32(object + 40, value - 40960);
    BuildDirectionVector(probe, 145, rotation);
    AddVectors(probe, position);
    shared_ray.pos[0][0] = (probe[0]);
    shared_ray.pos[0][1] = (probe[1]);
    shared_ray.pos[1][1] = (probe[1]);
    shared_ray.pos[0][2] = (probe[2]);
    shared_ray.pos[1][0] = (probe[0]);
    shared_ray.pos[1][2] = (probe[2]);
    shared_ray.pos[1][1] = (probe[1] + 4096000);
    InitializeCollisionRay(shared_ray);
    QueryCollisionRay(shared_ray, 1);
    if (shared_ray.hit.GetObject())
        value = shared_ray.hit_pos[1];
    else
        value = 0x7FFFFFFF;
    w_u32(object + 108, value);
    value = r_u32(0x800FF650);
    w_u32(object + 112, value);
}

uint32 BouncingPointEffect::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result;
    FUNCTION_MARKER(0x8002BBF0, "SLUS_003.73");
    w_u32(object + 68, 0x800A1518);
    result = PointSegments::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(object) : result;
}

uint32 BouncingPointEffect::Update()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 frame, created, elapsed, y, floor, velocity, shift = 1, random, point[3], rotated[3], child, result;
    uint16 angles[3];
    FUNCTION_MARKER(0x8002396C, "SLUS_003.73");
    frame = r_u32(0x800FF650);
    created = r_u32(object + 112);
    elapsed = frame - created;
    if (elapsed >= 120)
    {
        frame = r_u32(0x800FF650);
        result = frame - created < 121;
        return result ? result : Finish();
    }
    AddVectors((uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 36, 12));
    y = r_u32(object + 28);
    floor = r_u32(object + 108);
    if ((sint32)floor < (sint32)y)
    {
        velocity = r_u32(object + 40);
        w_u32(object + 28, floor);
        w_u32(object + 40, 0 - velocity);
        ShiftVectorRight((uint32 *)psx_addr(object + 36, 12), &shift);
        velocity = r_u32(object + 40);
        if (velocity + 8192 >= 16385)
        {
            angles[2] = 0;
            angles[0] = 0;
            angles[1] = (uint16)RandomRange(4096);
            random = RandomRange(4);
            if (!random)
            {
                random = RandomRange(3);
                PlayPositionedSound((sint32)(random + 12), (const uint32 *)psx_addr(object + 24, 12), 0);
            }
            BuildDirectionVector((uint32 *)psx_addr(object + 96, 12), 3, angles);
            random = RandomRange(1025);
            RotateVectorYaw(rotated, (const uint32 *)psx_addr(object + 36, 12), random - 512);
            w_u32(object + 36, rotated[0]);
            w_u32(object + 40, rotated[1]);
            w_u32(object + 44, rotated[2]);
        }
    }
    AddVectors(point, (const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 96, 12));
    w_u32(object + 84, point[0]);
    w_u32(object + 88, point[1]);
    w_u32(object + 92, point[2]);
    child = r_u32(object + 80);
    SubtractVectors(point, (const uint32 *)psx_addr(object + 24, 12), (const uint32 *)psx_addr(object + 96, 12));
    w_u32(child, point[0]);
    w_u32(child + 4, point[1]);
    w_u32(child + 8, point[2]);
    result = r_u32(object + 40) + 14792;
    w_u32(object + 40, result);
    return result;
}

BouncingPointEffect &BouncingPointEffect::Attach(uint32 object)
{
    return *static_cast<BouncingPointEffect *>(FindNativeObjectStorage(object));
}



extern "C" uint32 apocalypse_projectile_construct_2374C(uint32 object, uint32 position, const sint16 angles[3])
{
    void *dram_addr = AllocateNativeObjectStorage(object, sizeof(BouncingPointEffect));
    new (dram_addr) BouncingPointEffect((const uint32 *)psx_addr(position, 12), (const uint16 *)angles);
    return object;
}

extern "C" uint32 sub_8002BBF0(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002BBF0, "SLUS_003.73");
    return BouncingPointEffect::Attach(object).BouncingPointEffect::Destroy(reason);
}

extern "C" uint32 sub_8002396C(uint32 object)
{
    FUNCTION_MARKER(0x8002396C, "SLUS_003.73");
    return BouncingPointEffect::Attach(object).BouncingPointEffect::Update();
}

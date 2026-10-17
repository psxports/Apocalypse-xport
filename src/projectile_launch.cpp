#include "visual_object.h"
#include "object_collision_events.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "projectile_launch.h"
#include "glow_effect.h"
#include "ring_burst_effect.h"
#include "explosion_effects.h"
#include "native_object_storage.h"
#include "game_object_allocation.h"
#include "model_resources.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "audio_session.h"
#include <new>

static uint32 DivideLaunchScalar(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

// Native input adapter for the original position word loads
static uint32 ReadLaunchPositionWord(const uint32 *position, uint32 index)
{
    uint32 guest_position = GuestMemoryAddress(position);
    return guest_position ? r_u32(guest_position + index * 4) : position[index];
}

uint32 CalculateLaunchAngle(uint32 input);

uint32 LaunchedProjectile::Configure(ModelInstance **owner, uint32 radius, uint32 model, uint32 byte5, uint32 byte6)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 tag, tick, child, random;
    FUNCTION_MARKER(0x8002A7F8, "SLUS_003.73");
    AddToList(visual_lists.projectiles);
    if (model)
        tag = r_u8(0x800FF644);
    else
    {
        tag = r_u8(0x800FF645);
        model = 0x51E46AAF;
    }
    this->model.model_idx = FindModelResourceIndex(model, tag);
    tick = r_u32(0x800FF650);
    projectile.damage_kind = (uint32)(26);
    w_u16(object + 204, 300);
    projectile.owner_list = owner;
    projectile.radius = (uint16)(radius);
    launch.explode_on_contact = (uint8)(byte5);
    launch.config_byte = (uint8)(byte6);
    launch.spawn_tick = (uint32)(tick);
    this->kind = owner == &visual_lists.players ? 63 : 64;
    if (projectile.owner_list == &visual_lists.players)
        this->lifecycle_flags = this->lifecycle_flags & 0xFFEF;
    child = AllocateEffectStorage(104);
    if (child)
    {
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingEffect));
        new (dram_addr) RingEffect(4, 2);
    }
    launch.glow_object = (uint32)(child);
    w_u8(child + 94, 1);
    child = launch.glow_object;
    RingEffect::Attach(child).SetRowExtent( 0, 16);
    child = launch.glow_object;
    RingEffect::Attach(child).SetRowExtent( 1, 16);
    child = launch.glow_object;
    RingEffect::Attach(child).SetCenterColor( 0, 0, 0);
    child = launch.glow_object;
    RingEffect::Attach(child).SetRingColors( 0, 0, 0);
    launch.glow_rgb[0] = (uint8)(100);
    launch.glow_rgb[1] = (uint8)(50);
    launch.glow_rgb[2] = (uint8)(0);
    child = launch.glow_object;
    RingEffect::Attach(child).SetRowColors( 0, 100, 50, 0);
    child = launch.glow_object;
    RingEffect::Attach(child).SetRowColors( 1, 0, 0, 0);
    random = RandomRange(1024);
    child = launch.glow_object;
    w_u16(child + 96, random);
    child = launch.glow_object;
    RingEffect::Attach(child).SetRadii( 0);
    child = launch.glow_object;
    RingEffect::Attach(child).SetPosition((const uint32 *)this->model.pos);
    child = launch.glow_object;
    w_u8(child + 66, 1);
    return child;
}

uint32 LaunchedProjectile::SetGlowColor(uint32 red, uint32 green, uint32 blue)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child;
    FUNCTION_MARKER(0x8002A9F0, "SLUS_003.73");
    launch.glow_rgb[2] = (uint8)(blue);
    launch.glow_rgb[0] = (uint8)(red);
    launch.glow_rgb[1] = (uint8)(green);
    child = launch.glow_object;
    return RingEffect::Attach(child).SetRowColors( 0, red & 255, green & 255, blue & 255);
}

LaunchedProjectile::LaunchedProjectile(const uint32 *position, const sint16 *angles, uint32 speed, uint32 damage, uint32 radius, ModelInstance **list, uint32 model, uint32 first, uint32 second)
    : ProjectileBase()
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_position = GuestMemoryAddress(position);
    const uint32 *native_position = guest_position ? 0 : position;
    uint32 x, y, z;
    FUNCTION_MARKER(0x8002AA38, "SLUS_003.73");
    w_u32(object + 68, 0x800A1338);
    projectile.damage = (uint16)(damage);
    Configure(list, radius, model, first & 255, second & 255);
    if (native_position)
    {
        x = native_position[0];
        y = native_position[1];
        z = native_position[2];
    }
    else
    {
        x = r_u32(guest_position);
        y = r_u32(guest_position + 4);
        z = r_u32(guest_position + 8);
    }
    this->model.pos[0] = x;
    this->model.pos[1] = y;
    this->model.pos[2] = z;
    BuildDirectionVector((uint32 *)motion.velocity, speed, (const uint16 *)angles);
}

LaunchedProjectile::LaunchedProjectile(const uint32 *position, const uint32 *target, uint32 speed, uint32 damage, uint32 radius, ModelInstance **owner, uint32 model, uint32 first, uint32 second)
    : ProjectileBase()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8002AB28, "SLUS_003.73");
    uint32 x, y, z, origin, aim, dx, dz, distance, qx, qz, gravity, dy, vertical, scale;
    w_u32(object + 68, 0x800A1338);
    projectile.damage = (uint16)(damage);
    Configure(owner, radius, model, first & 255, second & 255);
    x = ReadLaunchPositionWord(position, 0);
    y = ReadLaunchPositionWord(position, 1);
    z = ReadLaunchPositionWord(position, 2);
    this->model.pos[0] = x;
    this->model.pos[1] = y;
    this->model.pos[2] = z;
    origin = ReadLaunchPositionWord(position, 0);
    aim = ReadLaunchPositionWord(target, 0);
    dx = (uint32)((sint32)(aim - origin) >> 12);
    aim = ReadLaunchPositionWord(target, 2);
    origin = ReadLaunchPositionWord(position, 2);
    dz = (uint32)((sint32)(aim - origin) >> 12);
    distance = LookupDistanceRoot(dx * dx + dz * dz);
    dx <<= 12;
    if (!distance)
    {
        motion.velocity[0] = 0;
        motion.velocity[1] = 0xFFFEC000;
        motion.velocity[2] = 0;
    }
    else
    {
        qx = DivideLaunchScalar(dx, distance);
        qz = DivideLaunchScalar(dz << 12, distance);
        gravity = DivideLaunchScalar(distance << 3, speed << 1);
        origin = ReadLaunchPositionWord(position, 1);
        aim = ReadLaunchPositionWord(target, 1);
        dy = (uint32)((sint32)(aim - origin) >> 12);
        vertical = DivideLaunchScalar(speed * dy, distance);
        scale = (uint32)((sint32)(speed * 9) / 10);
        motion.velocity[0] = qx * scale;
        motion.velocity[1] = (vertical - gravity) << 12;
        motion.velocity[2] = qz * scale;
    }
}

uint32 LaunchedProjectile::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 child, table, target, result;
    sint32 adjustment;
    FUNCTION_MARKER(0x8002AD10, "SLUS_003.73");
    child = launch.glow_object;
    w_u32(object + 68, 0x800A1338);
    if (child)
    {
        table = r_u32(child + 68);
        adjustment = (sint16)r_u16(table + 8);
        target = r_u32(table + 12);
        apocalypse_object_cleanup(target, child + (uint32)adjustment, 3);
    }
    RemoveFromList(visual_lists.projectiles);
    ProjectileBase::Destroy(0);
    result = reason & 1;
    if (result)
        result = ReleaseObjectStorage(object);
    return result;
}

uint32 LaunchedProjectile::Update()
{
    CollisionRay &shared_ray = GetSharedCollisionRay();
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 before[3], x, y, z, speed, dot, normal, product, value, one = 1;
    uint32 explode = 0, child, elapsed, limit, red, green, blue, height;
    ModelInstance *hit;
    ModelInstance *head;
    ModelInstance **table;
    uint32 now, start_time;
    FUNCTION_MARKER(0x8002AD9C, "SLUS_003.73");
    this->model.pos[1] = this->model.pos[1] + 81920;
    before[0] = this->model.pos[0];
    before[1] = this->model.pos[1];
    before[2] = this->model.pos[2];
    if (!launch.settled)
    {
        x = this->model.pos[0];
        shared_ray.pos[0][0] = (x);
        y = this->model.pos[1];
        shared_ray.pos[0][1] = (y);
        z = this->model.pos[2];
        shared_ray.pos[0][2] = (z);
        shared_ray.pos[1][0] = (x + (motion.velocity[0] << 1));
        shared_ray.pos[1][1] = (y + (motion.velocity[1] << 1));
        shared_ray.pos[1][2] = (z + (motion.velocity[2] << 1));
        speed = MeasureVectorLength((const uint32 *)motion.velocity);
        InitializeCollisionRay(shared_ray);
        w_u32(0x800FF974, 1);
        QueryCollisionRay(shared_ray, 1);
        w_u32(0x800FF974, 0);
        AddVectors((uint32 *)this->model.pos, (const uint32 *)motion.velocity);
        motion.velocity[1] = motion.velocity[1] + 32768;
        if (shared_ray.hit.GetObject() && (sint32)speed >= (sint32)shared_ray.distance)
        {
            this->model.pos[0] = shared_ray.hit_pos[0];
            this->model.pos[1] = shared_ray.hit_pos[1];
            this->model.pos[2] = shared_ray.hit_pos[2];
            PlayPositionedSound(22, (const uint32 *)this->model.pos, 0);
            x = motion.velocity[0];
            normal = (uint32)(sint32)(sint16)(uint16)shared_ray.n.vx;
            dot = (uint32)((sint32)x >> 6) * normal;
            y = motion.velocity[1];
            dot += (uint32)((sint32)y >> 6) * (uint32)(sint32)(sint16)(uint16)shared_ray.n.vy;
            z = motion.velocity[2];
            dot += (uint32)((sint32)z >> 6) * (uint32)(sint32)(sint16)(uint16)shared_ray.n.vz;
            dot = (uint32)((sint32)dot >> 12);
            product = dot * normal;
            motion.velocity[0] = x - (uint32)((sint32)product >> 6);
            normal = (uint32)(sint32)(sint16)(uint16)shared_ray.n.vy;
            y = motion.velocity[1];
            product = dot * normal;
            motion.velocity[1] = y - (uint32)((sint32)product >> 6);
            normal = (uint32)(sint32)(sint16)(uint16)shared_ray.n.vz;
            explode = launch.explode_on_contact != 0;
            z = motion.velocity[2];
            product = dot * normal;
            motion.velocity[2] = z - (uint32)((sint32)product >> 6);
            ShiftVectorRight((uint32 *)motion.velocity, &one);
            normal = (uint32)(sint32)(sint16)(uint16)shared_ray.n.vx;
            x = motion.velocity[0];
            product = dot * normal;
            motion.velocity[0] = x - (uint32)((sint32)product >> 7);
            normal = (uint32)(sint32)(sint16)(uint16)shared_ray.n.vy;
            y = motion.velocity[1];
            product = dot * normal;
            motion.velocity[1] = y - (uint32)((sint32)product >> 7);
            normal = (uint32)(sint32)(sint16)(uint16)shared_ray.n.vz;
            z = motion.velocity[2];
            product = dot * normal;
            motion.velocity[2] = z - (uint32)((sint32)product >> 7);
            value = (RandomRange(11) - 5) << 12;
            motion.velocity[0] = motion.velocity[0] + value;
            value = (RandomRange(11) - 5) << 12;
            z = motion.velocity[2];
            y = motion.velocity[1];
            motion.velocity[2] = z + value;
            if ((sint32)y > -61440 && (sint16)(uint16)shared_ray.n.vy < -3500)
                launch.settled = (uint8)(1);
        }
        product = 410 * MeasureVectorLength((const uint32 *)motion.velocity);
        value = this->model.angles[0];
        this->model.angles[0] = value + (uint32)((sint32)product / 128);
        x = motion.velocity[0];
        z = (uint32)((sint32)motion.velocity[2] >> 12);
        x = (uint32)((sint32)x >> 12);
        if (!z)
            this->model.angles[1] = (sint32)x > 0 ? -1024 : 1024;
        else if ((sint32)z > 0)
            this->model.angles[1] = 2048 - CalculateLaunchAngle(DivideLaunchScalar(x << 12, 0 - z));
        else
            this->model.angles[1] = CalculateLaunchAngle(DivideLaunchScalar(x << 12, z));
    }
    shared_ray.pos[0][0] = (this->model.pos[0]);
    shared_ray.pos[0][1] = (this->model.pos[1]);
    shared_ray.pos[0][2] = (this->model.pos[2]);
    shared_ray.pos[1][0] = (this->model.pos[0]);
    y = this->model.pos[1];
    shared_ray.pos[1][1] = (y);
    z = this->model.pos[2];
    shared_ray.pos[1][1] = (y + 12288000);
    shared_ray.pos[1][2] = (z);
    InitializeCollisionRay(shared_ray);
    QueryCollisionRay(shared_ray, 1);
    if (shared_ray.hit.GetObject())
    {
        w_u32(object + 184, shared_ray.hit_pos[0]);
        y = shared_ray.hit_pos[1];
        w_u32(object + 188, y);
        value = this->model.pos[1];
        z = shared_ray.hit_pos[2];
        height = (uint32)((sint32)(y - value) >> 12);
        product = (uint32)(21 * (sint32)(sint16)height);
        w_u32(object + 192, z);
        w_u16(object + 202, height);
        value = 50 - (uint32)((sint32)product / 128);
        w_u16(object + 200, value);
        if (value & 65535)
            EnableShadow();
        else
            RemoveShadow();
    }
    else
        RemoveShadow();
    now = r_u32(0x800FF650);
    start_time = launch.spawn_tick;
    elapsed = now - start_time;
    limit = 60 * (uint32)(sint32)(sint16)r_u16(0x800EC7FE);
    if ((sint32)(limit - 30) < (sint32)elapsed)
    {
        if (r_u32(0x800FF2F0) & 2)
        {
            value = launch.glow_rgb[0];
            red = launch.glow_rgb[0] * 2;
            if ((sint8)value < 0)
                red = 255;
            value = launch.glow_rgb[1];
            green = launch.glow_rgb[1] * 2;
            if ((sint8)value < 0)
                green = 255;
            value = launch.glow_rgb[2];
            blue = launch.glow_rgb[2] * 2;
            if ((sint8)value < 0)
                blue = 255;
            red &= 255;
            green &= 255;
            blue &= 255;
        }
        else
        {
            blue = launch.glow_rgb[2];
            red = launch.glow_rgb[0];
            green = launch.glow_rgb[1];
        }
        child = launch.glow_object;
        RingEffect::Attach(child).SetRowColors(0, red, green, blue);
    }
    limit = 60 * (uint32)(sint32)(sint16)r_u16(0x800EC7FE);
    if ((sint32)limit < (sint32)elapsed)
        explode = 1;
    head = *projectile.owner_list;
    hit = TraceSegmentCollision(before, (const uint32 *)this->model.pos, head);
    if (hit && (!(hit->flags & 16) || static_cast<VisualObject *>(hit->owner)->kind == 205))
        explode = 1;
    if (explode)
    {
        table = projectile.owner_list;
        value = projectile.radius;
        head = *table;
        ApplyBlast(head, value, 10, 26);
        head = visual_lists.solids;
        value = projectile.radius;
        ApplyBlast(head, value, 10, 26);
        DamageNearbyObjects(projectile.radius);
        child = AllocateEffectStorage(124);
        if (child)
        {
            value = projectile.radius;
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ExpandingGlow));
            new (dram_addr) ExpandingGlow((const uint32 *)this->model.pos, 128, 100, 0, value, 7);
        }
        value = RandomRange(2);
        PlayPositionedSound(value ? 1 : 2, (const uint32 *)this->model.pos, 0);
        SpawnExplosionParticles((const uint32 *)this->model.pos, 20, 10, 750, 80);
        child = AllocateEffectStorage(124);
        if (child)
        {
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingBurstEffect));
            new (dram_addr) RingBurstEffect((const uint32 *)this->model.pos, 5, 255, 255, 255, 1, 128, 100, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 4);
        }
        Finish();
    }
    y = this->model.pos[1];
    child = launch.glow_object;
    this->model.pos[1] = y - 81920;
    return RingEffect::Attach(child).SetPosition((const uint32 *)this->model.pos);
}


uint32 CalculateLaunchAngle(uint32 input)
{
    FUNCTION_MARKER(0x8008702C, "SLUS_003.73");
    return (uint32)CalculateAngleFromSlope((sint32)input);
}

LaunchedProjectile &LaunchedProjectile::Attach(uint32 object)
{
    return *static_cast<LaunchedProjectile *>(FindNativeObjectStorage(object));
}











extern "C" uint32 sub_8002AD10(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002AD10, "SLUS_003.73");
    return LaunchedProjectile::Attach(object).LaunchedProjectile::Destroy(reason);
}

extern "C" uint32 sub_8002AD9C(uint32 object)
{
    FUNCTION_MARKER(0x8002AD9C, "SLUS_003.73");
    return LaunchedProjectile::Attach(object).LaunchedProjectile::Update();
}






#include <new>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_heap.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "effect_object.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "angle_math.h"
#include "ring_burst_effect.h"
#include "collision.h"
#include "object_damage.h"
#include "vector_distance.h"
#include "ring_effect.h"

#include "projectile_line.h"
#include "model_projection.h"
#include "effect_sprite.h"
#include "smoke_emitter.h"
#include "vector_arithmetic.h"
#include "game_random.h"
#include "effect_color.h"
#include "effect_lifecycle.h"

uint32 LineProjectile::DivideLifetime(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return numerator & 0x80000000 ? 1 : 0xFFFFFFFF;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

LineProjectile::LineProjectile(const uint32 *input_position, const sint16 *input_angles, uint32 speed, uint32 damage, ModelInstance **owner_list, uint32 red, uint32 green, uint32 blue, uint32 mode) : RenderableEffect(), age(0), lifetime(0), owner_list(0), damage(0), speed(0), ring(), mode(0)
{
    ShortVector3 rotation;
    Vector3 vector, scaled;
    uint32 x, y, z, scalar;
    ModelInstance *player, *owner;
    FUNCTION_MARKER(0x800256F0, "SLUS_003.73");
    player = visual_lists.players;
    this->speed = speed;
    this->damage = damage;
    this->owner_list = owner_list;
    owner = *owner_list;
    if (owner == player)
        this->kind = 4;
    CopyAngleVector(rotation.words, (const uint16 *)input_angles);
    scalar = RandomRange(21);
    rotation.words[0] = (uint16)((rotation.words[0] + scalar - 10) & 4095);
    speed = this->speed;
    BuildDirectionVector(this->velocity.words, speed, rotation.words);
    DivideVector(vector.words, this->velocity.words, this->speed);
    this->dir = vector;
    scalar = 100;
    MultiplyVector(scaled.words, this->dir.words, scalar);
    AddVectors(vector.words, input_position, scaled.words);
    this->pos = vector;
    x = this->pos.words[0];
    y = this->pos.words[1];
    z = this->pos.words[2];
    this->origin.words[0] = x;
    this->origin.words[1] = y;
    this->origin.words[2] = z;
    scalar = RandomRange(100);
    MultiplyVector(vector.words, this->dir.words, scalar);
    AddVectors(this->pos.words, vector.words);
    x = this->pos.words[0];
    y = this->pos.words[1];
    z = this->pos.words[2];
    this->tail_pos.words[0] = x;
    this->tail_pos.words[1] = y;
    this->tail_pos.words[2] = z;
    InitializeTrace();
    this->rgb.code = 0;
    this->rgb.r = red;
    this->rgb.g = green;
    this->rgb.b = blue;
    this->mode = mode;
    ring = SpawnProjectileRingTrail(pos.words, red & 255, green & 255, blue & 255);
}

LineProjectile::~LineProjectile()
{
    if (ring)
        ring->Destroy(3);
}

uint32 LineProjectile::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800258F8, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    RenderableEffect::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

uint32 LineProjectile::Finish()
{
    GameHeapBlock *particle;
    uint32 red;
    uint32 green;
    uint32 blue;
    FUNCTION_MARKER(0x80025974, "SLUS_003.73");
    particle = AllocateEffectStorage(124);
    if (particle)
    {
        red = ((59 * this->rgb.r) >> 7) & 255;
        green = ((59 * this->rgb.g) >> 7) & 255;
        blue = ((59 * this->rgb.b) >> 7) & 255;
        new (particle) RingBurstEffect(this->pos.words, 5, 255, 255, 255, 1, red, green, blue, 1, 60, 0, 1, 50, 80, 25, 40, 10, 5);
    }
    return EffectObject::Finish();
}

uint32 LineProjectile::InitializeTrace()
{
    uint32 dx, dy, dz, x, y, z, distance, speed, result;
    ModelInstance *hit;
    FUNCTION_MARKER(0x80025A74, "SLUS_003.73");
    dx = this->dir.words[0];
    dy = this->dir.words[1];
    x = this->pos.words[0];
    trace.endpoints[0].words[0] = x - 300 * dx;
    y = this->pos.words[1];
    dz = this->dir.words[2];
    trace.endpoints[0].words[1] = y - 300 * dy;
    z = this->pos.words[2];
    dx = this->dir.words[0];
    trace.endpoints[0].words[2] = z - 300 * dz;
    x = this->pos.words[0];
    dy = this->dir.words[1];
    trace.endpoints[1].words[0] = x + 10000 * dx;
    y = this->pos.words[1];
    dz = this->dir.words[2];
    trace.endpoints[1].words[1] = y + 10000 * dy;
    z = this->pos.words[2];
    trace.endpoints[1].words[2] = z + 10000 * dz;
    InitializeCollisionRay(trace);
    collision_query.reject_surface40 = true;
    QueryCollisionRay(trace, 0);
    hit = trace.hit.GetObject();
    collision_query.reject_surface40 = false;
    distance = hit ? trace.distance : 10000;
    speed = this->speed;
    result = DivideLifetime(distance, speed);
    this->lifetime = result;
    return result;
}

uint32 LineProjectile::Render()
{
    const ModelProjectionConfig &proj_cfg = GetModelProjectionConfig();
    uint8 *packet, *limit, *next;
    uint32 red, green, blue, color, screen, depth, result;
    uint32 draw_mode, draw_word;
    FUNCTION_MARKER(0x80025BEC, "SLUS_003.73");
    packet = scene_render.cursor;
    limit = scene_render.limit;
    result = limit - packet < (sint32)(sizeof(LINE_G2) + sizeof(DR_TPAGE));
    if (result)
        return result;
    next = packet + sizeof(LINE_G2) + sizeof(DR_TPAGE);
    scene_render.cursor = next;
    LINE_G2 *line = (LINE_G2 *)packet;
    line->tag = 0;
    setLineG2(line);
    red = this->rgb.r;
    green = this->rgb.g;
    blue = this->rgb.b;
    color = (((197 * blue) >> 8) << 16) | ((197 * green) & 0xFF00) | 0x52000000 | ((197 * red) >> 8);
    line->p[0].rgbc.rgbc = color;
    red = this->rgb.r;
    green = this->rgb.g;
    blue = this->rgb.b;
    color = (((49 * blue) >> 7) << 16) | (((49 * green) >> 7) << 8) | ((49 * red) >> 7);
    line->p[1].rgbc.rgbc = color;
    depth = ProjectWorldPoint(pos.words, &screen);
    line->p[1].xy.xy = screen;
    ProjectWorldPoint(tail_pos.words, &screen);
    line->p[0].xy.xy = screen;
    uint32 depth_limit = proj_cfg.near_depth;
    result = (sint32)depth < (sint32)depth_limit;
    if (!result)
    {
        depth_limit = proj_cfg.max_distance;
        result = (sint32)depth_limit < (sint32)depth;
    }
    if (result)
    {
        scene_render.cursor = packet;
        return result;
    }
    uint32 *bucket = scene_render.cur->ot + ((depth & 0x3FFC) >> 2);
    setaddr(line, getaddr(bucket));
    setaddr(bucket, gpu_packet_link((uintptr_t)line));
    draw_mode = GetEffectDrawMode().tag;
    draw_word = GetEffectDrawMode().code[0];
    DR_TPAGE *state = (DR_TPAGE *)(packet + sizeof(LINE_G2));
    state->tag = draw_mode;
    state->code[0] = draw_word;
    setaddr(state, getaddr(bucket));
    setaddr(bucket, gpu_packet_link((uintptr_t)state));
    return *bucket;
}

void LineProjectile::Update()
{
    Vector3 scaled, position;
    uint32 value, distance, scalar;
    ModelInstance *hit, *head;
    uint32 damage;
    uint32 mode;
    GameHeapBlock *particle;
    uint32 release = 0, age, lifetime, result, x, y, z;
    FUNCTION_MARKER(0x80025E2C, "SLUS_003.73");
    AddVectors(this->pos.words, this->velocity.words);
    if (ring)
    {
        ring->SetPosition(pos.words);
        ring->angle += 200;
    }
    age = this->age;
    this->age = age + 1;
    distance = MeasureEuclideanDistance(this->pos.words, this->origin.words);
    if ((sint32)distance >= 601)
        distance = 600;
    scalar = distance;
    MultiplyScalarVector(scaled.words, scalar, this->dir.words);
    SubtractVectors(position.words, this->pos.words, scaled.words);
    this->tail_pos = position;
    value = this->mode;
    head = *owner_list;
    if (value)
        hit = TraceSegmentCollision(this->tail_pos.words, this->pos.words, head);
    else
        hit = IntersectObjectSpheres(this->tail_pos.words, this->pos.words, position.words, head, 0);
    if (hit)
    {
        release = 1;
        damage = this->damage;
        static_cast<VisualObject *>(hit->owner)->Damage(damage, this->dir.components, 9);
        mode = level_environment.area_kind;
        if (mode == 6)
        {
            if (owner_list == &visual_lists.players)
            {
                EffectObject *effect = effect_lists.renderables;
                while (effect)
                {
                    EffectObject *next_effect = effect->next;
                    if (effect->IsAlive() && effect != this && effect->kind == 4)
                        static_cast<LineProjectile *>(effect)->LineProjectile::Finish();
                    effect = next_effect;
                }
                goto CheckLifetime;
            }
            mode = level_environment.area_kind;
        }
        if (mode == 3 && this->kind == 4)
        {
            particle = AllocateEffectStorage(124);
            if (particle)
            {
                new (particle) RingBurstEffect(this->pos.words, 5, 255, 255, 255, 1, 64, 32, 160, 1, 60, 0, 1, 100, 120, 80, 100, 10, 8);
            }
        }
    }
CheckLifetime:
    age = (uint32)(sint32)(sint16)this->age;
    lifetime = this->lifetime;
    result = (sint32)age < (sint32)lifetime;
    if (!result)
    {
        hit = trace.hit.GetObject();
        release = 1;
        if (hit)
        {
            damage = this->damage;
            DamageLevelObject(hit, trace.hit.GetPolygon(), (sint32)damage);
            x = trace.hit_pos.words[0];
            y = trace.hit_pos.words[1];
            z = trace.hit_pos.words[2];
            this->pos.words[0] = x;
            this->pos.words[1] = y;
            this->pos.words[2] = z;
        }
    }
    if (release)
    {
        Finish();
        return;
    }
    return;
}

ProjectileTracer::ProjectileTracer(const uint32 *start, const uint32 *end) : LineEffect(), age(0)
{
    Vector3 end_pos, direction, scaled, point, delta, movement;
    uint32 distance, offset, divisor = 5, shift = 1, axis;
    FUNCTION_MARKER(0x800234CC, "SLUS_003.73");
    for (axis = 0; axis < 3; ++axis)
        end_pos.words[axis] = end[axis];
    SubtractVectors(direction.words, end_pos.words, start);
    distance = MeasureVectorLength(direction.words);
    if (distance)
        DivideVector(direction.words, distance);
    else
        Finish();
    if ((sint32)distance >= 3001)
    {
        distance = 3000;
        MultiplyScalarVector(scaled.words, distance, direction.words);
        AddVectors(point.words, start, scaled.words);
        end_pos = point;
    }
    offset = RandomRange((uint32)((sint32)distance / 3));
    MultiplyVector(scaled.words, direction.words, offset);
    AddVectors(point.words, start, scaled.words);
    endpoint[0] = point;
    SubtractVectors(delta.words, end_pos.words, endpoint[0].words);
    DivideVector(movement.words, delta.words, divisor);
    AddVectors(point.words, endpoint[0].words, movement.words);
    endpoint[1] = point;
    ShiftVectorLeft(point.words, movement.words, shift);
    this->velocity = point;
    color[0] = (color[0] & 0xFF000000) | 0x2000000;
}

void ProjectileTracer::Update()
{
    FUNCTION_MARKER(0x800236D4, "SLUS_003.73");
    if (age)
    {
        AddVectors(endpoint[0].words, velocity.words);
        AddVectors(endpoint[1].words, velocity.words);
    }
    ++age;
    if (age == 4)
        Finish();
    FadePackedColor(&color[1], 16);
}

uint32 ProjectileTracer::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8002BC44, "SLUS_003.73");
    uint32 result = LineEffect::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void CreateProjectileImpact(uint32 flags, const uint32 *position)
{
    FUNCTION_MARKER(0x8002289C, "SLUS_003.73");
    GameHeapBlock *effect;
    if (flags & 4)
    {
        AnimatedSprite *sprite = 0;
        effect = AllocateEffectStorage(120);
        if (effect)
        {
            sprite = new (effect) AnimatedSprite(position, (uint32)0, 744, 1, 1, 0xFFFFFFFF);
        }
        if (!sprite)
        {
            GameDiagnostic("Projectile impact sprite allocation failed\n");
            abort();
        }
        sprite->frame = RandomRange(sprite->frame_count - 1);
        sprite->angle = RandomRange(4096);
        sprite->depth_bias = 100;
    }
    if (flags & 8)
    {
        for (uint32 index = 0; index < 4; ++index)
        {
            effect = AllocateEffectStorage(112);
            if (effect)
            {
                new (effect) ParticleSprite(position, 128, 128, 128, 6, 1000, 1, 40, 0x3000, 20);
            }
        }
    }
    if (flags & 16)
    {
        effect = AllocateEffectStorage(96);
        if (effect)
        {
            uint32 count = RandomRange(3) + 1;
            new (effect) SmokeEmitter(position, 1, count, 128, 128, 128, 8, 5, 700, 700);
        }
    }
}

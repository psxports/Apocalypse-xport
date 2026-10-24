#include <new>
#include <string.h>
#include <stdlib.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "scene_lifecycle.h"
#include "level_session.h"
#include "effect_object.h"
#include "renderable_effect.h"
#include "visual_object.h"
#include "game_diagnostics.h"
#include "angle_math.h"
#include "ring_burst_effect.h"
#include "collision.h"
#include "object_damage.h"
#include "global.h"
#include "ring_effect.h"
#include "projectile_line.h"
#include "line_projectile.h"
#include "model_projection.h"
#include "vector3.h"
#include "smoke_emitter.h"
#include "effect_lifecycle.h"
#include "hd.h"

LineProjectile::LineProjectile(const Vector3 *input_pos, const ShortVector3 *input_angles, uint32 speed, uint16 damage, ModelInstance **owner_list, uint8 r, uint8 g, uint8 b, uint8 mode) : RenderableEffect(), age(0), lifetime(0), owner_list(0), damage(0), speed(0), ring(), mode(false), hd_history(0)
{
    ShortVector3 rotation;
    Vector3 vector, scaled;
    uint32 scalar;
    ModelInstance *player;
    FUNCTION_MARKER(0x800256F0, "SLUS_003.73");
    player = visual_lists.players;
    this->speed = speed;
    this->damage = damage;
    this->owner_list = owner_list;
    if (*owner_list == player)
        kind = 4;
    rotation = *input_angles;
    scalar = RandomRange(21);
    rotation.x = ((uint16)rotation.x + scalar - 10) & 4095;
    velocity.SetDir(this->speed, rotation);
    dir.Divide(velocity, this->speed);
    scaled.Multiply(dir, 100);
    pos.Add(*input_pos, scaled);
    origin = pos;
    scalar = RandomRange(100);
    vector.Multiply(dir, scalar);
    pos.Add(vector);
    tail_pos = pos;
    InitializeTrace();
    rgb.code = 0;
    rgb.r = r;
    rgb.g = g;
    rgb.b = b;
    this->mode = mode;
    ring = RingEffect::SpawnProjectileTrail(&pos, r, g, b);
}

uint32 LineProjectile::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800258F8, "SLUS_003.73");
    if (ring)
        ring->Destroy(3);
    ring = 0;
    RenderableEffect::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

uint32 LineProjectile::Finish()
{
    GameHeapBlock *particle;
    uint8 r, g, b;
    FUNCTION_MARKER(0x80025974, "SLUS_003.73");
    particle = EffectObject::AllocateStorage(124);
    if (particle)
    {
        r = (59 * rgb.r) >> 7;
        g = (59 * rgb.g) >> 7;
        b = (59 * rgb.b) >> 7;
        new (particle) RingBurstEffect(&pos, 5, 255, 255, 255, 1, r, g, b, 1, 60, 0, 1, 50, 80, 25, 40, 10, 5);
    }
    return EffectObject::Finish();
}

uint32 LineProjectile::InitializeTrace()
{
    uint32 result;
    sint32 distance;
    ModelInstance *hit;
    FUNCTION_MARKER(0x80025A74, "SLUS_003.73");
    for (uint32 axis = 0; axis < 3; ++axis)
        trace.endpoints[0].comp[axis] = (uint32)pos.comp[axis] - 300 * (uint32)dir.comp[axis];
    for (uint32 axis = 0; axis < 3; ++axis)
        trace.endpoints[1].comp[axis] = (uint32)pos.comp[axis] + 10000 * (uint32)dir.comp[axis];
    trace.Initialize();
    col_query.reject_surface40 = true;
    trace.Query(0);
    hit = trace.hit.GetObject();
    col_query.reject_surface40 = false;
    distance = hit ? trace.distance : 10000;
    result = DivideSignedWord(distance, speed);
    lifetime = result;
    return result;
}

bool LineProjectile::CaptureHd(bool logic)
{
    const ModelProjectionConfig &proj_cfg = GetModelProjConfig();
    HD_LINE_SAMPLE line = {0};
    line.page = EffectObject::GetDrawMode().code[0] & 0x1FF;
    line.flags = HD_SEMITRANSPARENT;
    line.sort_endpoint = 2;
    line.points[0].vx = tail_pos.x / 4096.0f;
    line.points[0].vy = tail_pos.y / 4096.0f;
    line.points[0].vz = tail_pos.z / 4096.0f;
    line.points[1].vx = pos.x / 4096.0f;
    line.points[1].vy = pos.y / 4096.0f;
    line.points[1].vz = pos.z / 4096.0f;
    line.colors[0] = ((197 * rgb.r) >> 8) | (((197 * rgb.g) >> 8) << 8) | (((197 * rgb.b) >> 8) << 16);
    line.colors[1] = ((49 * rgb.r) >> 7) | (((49 * rgb.g) >> 7) << 8) | (((49 * rgb.b) >> 7) << 16);
    line.near_depth = proj_cfg.near_depth;
    line.far_depth = proj_cfg.max_distance;
    line.clip = 1;
    return (logic ? hd_line_tick(&hd_history, &line) : hd_line_capture(&hd_history, &line)) != 0;
}

bool LineProjectile::CaptureHdTick()
{
    return CaptureHd(true);
}

uint32 LineProjectile::Render()
{
    const ModelProjectionConfig &proj_cfg = GetModelProjConfig();
    uint8 *packet, *limit;
    bool rejected;
    sint32 depth;
    const sint32 packet_size = sizeof(LINE_G2) + sizeof(DR_TPAGE);
    XY scr;
    FUNCTION_MARKER(0x80025BEC, "SLUS_003.73");
    if (hd_ui_collecting())
    {
        if (!CaptureHd(false))
        {
            GameDiagnostic(("Cannot capture HD line projectile\n"));
            abort();
        }
        return 0;
    }
    packet = scene_render.cursor;
    limit = scene_render.limit;
    rejected = limit - packet < packet_size;
    if (rejected)
        return rejected;
    scene_render.cursor = packet + packet_size;
    LINE_G2 *line = (LINE_G2 *)packet;
    line->tag = 0;
    setLineG2(line);
    setSemiTrans(line, 1);
    line->p[0].rgbc.r = (197 * rgb.r) >> 8;
    line->p[0].rgbc.g = (197 * rgb.g) >> 8;
    line->p[0].rgbc.b = (197 * rgb.b) >> 8;
    line->p[1].rgbc.r = (49 * rgb.r) >> 7;
    line->p[1].rgbc.g = (49 * rgb.g) >> 7;
    line->p[1].rgbc.b = (49 * rgb.b) >> 7;
    line->p[1].rgbc.code = 0;
    depth = ProjectWorldPoint(&pos, &scr);
    line->p[1].xy = scr;
    ProjectWorldPoint(&tail_pos, &scr);
    line->p[0].xy = scr;
    sint32 depth_limit = proj_cfg.near_depth;
    rejected = depth < depth_limit;
    if (!rejected)
    {
        depth_limit = proj_cfg.max_distance;
        rejected = depth_limit < depth;
    }
    if (rejected)
    {
        scene_render.cursor = packet;
        return rejected;
    }
    uint32 *bucket = scene_render.cur->ot + ((depth & 0x3FFC) >> 2);
    setaddr(line, getaddr(bucket));
    setaddr(bucket, gpu_packet_link((uintptr_t)line));
    DR_TPAGE *state = (DR_TPAGE *)(line + 1);
    *state = EffectObject::GetDrawMode();
    setaddr(state, getaddr(bucket));
    setaddr(bucket, gpu_packet_link((uintptr_t)state));
    return *bucket;
}

void LineProjectile::Update()
{
    Vector3 scaled, tmp_pos;
    sint32 distance;
    ModelInstance *hit, *head;
    uint32 area_kind;
    GameHeapBlock *particle;
    bool release = false;
    FUNCTION_MARKER(0x80025E2C, "SLUS_003.73");
    this->pos.Add(velocity);
    if (ring)
    {
        ring->SetPos(&pos);
        ring->angle += 200;
    }
    ++age;
    distance = this->pos.EuclideanDistance(origin);
    if (distance >= 601)
        distance = 600;
    scaled.Scale(distance, dir);
    tail_pos.Subtract(this->pos, scaled);
    head = *owner_list;
    if (mode)
        hit = CollisionRay::TraceSeg(tail_pos, this->pos, head);
    else
        hit = CollisionRay::IntersectSpheres(tail_pos, this->pos, &tmp_pos, head, 0);
    if (hit)
    {
        release = true;
        static_cast<VisualObject *>(hit->owner)->Damage(damage, &dir, 9);
        area_kind = level_environment.area_kind;
        if (area_kind == 6)
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
            area_kind = level_environment.area_kind;
        }
        if (area_kind == 3 && kind == 4)
        {
            particle = EffectObject::AllocateStorage(124);
            if (particle)
            {
                new (particle) RingBurstEffect(&this->pos, 5, 255, 255, 255, 1, 64, 32, 160, 1, 60, 0, 1, 100, 120, 80, 100, 10, 8);
            }
        }
    }
CheckLifetime:
    if (age >= lifetime)
    {
        hit = trace.hit.GetObject();
        release = true;
        if (hit)
        {
            DamageLevelObject(hit, trace.hit.GetPolygon(), damage);
            this->pos = trace.hit_pos;
        }
    }
    if (release)
        Finish();
}

LineProjectile::~LineProjectile()
{
    hd_line_free(hd_history);
    if (ring)
        ring->Destroy(3);
}

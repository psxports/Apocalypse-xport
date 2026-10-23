#include "collision_ray.h"
#include "game_tick.h"
#include "visual_object.h"
#include "waving_beam.h"
#include "collision.h"
#include "global.h"
#include "ribbon_segments.h"

WavingBeam::WavingBeam(const Vector3 *pos, const ShortVector3 *angles, sint32 len, uint16 lifetime, uint16 damage, uint32 r, uint32 g, uint32 b, uint8 scroll_step, uint32 material) : RibbonSegments(10, 0), scroll_step(scroll_step), damage_actors(false), damage(damage), damage_timer(0), length(len), lifetime(lifetime)
{
    FUNCTION_MARKER(0x80010D3C, "SLUS_003.73");
    this->angles = *angles;
    SetPos(pos);
    if (damage >= 1000)
    {
        damage_actors = true;
        this->damage -= 1000;
    }
    SetMaterial(material);
    for (uint32 i = 0; i < 10; ++i)
    {
        RibbonPoint *point = points + i;
        rest_x[i] = (uint32)point->pos.x;
        phase[i] = RandomRange(4096);
        amplitude[i] = i ? len / 80 : 0;
        width[i].tick = RandomRange(16);
        if (i)
            width[i].target = 148 * width[i - 1].target / 128;
        else
            width[i].target = RandomRange(60) + 20;
        point->width = width[i].target;
        point->height = point->width + 40;
        uint32 colors[3];
        colors[0] = (sint32)((9 - i) * r) / 9;
        colors[1] = (sint32)((9 - i) * g) / 9;
        colors[2] = (sint32)((9 - i) * b) / 9;
        point->center.r = colors[0];
        point->center.g = colors[1];
        point->center.b = colors[2];
        point->edge.r = (sint32)(colors[0] * 48) / 256;
        point->edge.g = (sint32)(colors[1] * 48) / 256;
        point->edge.b = (sint32)(colors[2] * 48) / 256;
    }
}

uint32 WavingBeam::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80011058, "SLUS_003.73");
    result = RibbonSegments::Destroy(0);
    if (reason & 1)
        result = ReleaseStorage();
    return result;
}

uint32 WavingBeam::SetPos(const Vector3 *pos)
{
    FUNCTION_MARKER(0x800110AC, "SLUS_003.73");
    Vector3 step, point;
    step.SetDir(length, angles);
    step.Divide(9);
    point = *pos;
    for (uint32 i = 0; i < 10; ++i)
    {
        points[i].pos = point;
        point.Add(step);
    }
    return 0;
}

void WavingBeam::Update()
{
    FUNCTION_MARKER(0x8001116C, "SLUS_003.73");
    if (lifetime == 65535)
    {
        ModelInstance *player = visual_lists.players;
        sint32 x = (uint32)points[0].pos.x - player->pos.x;
        x >>= 12;
        if (x < 0)
            x = -x;
        sint32 z = (uint32)points[0].pos.z - player->pos.z;
        z >>= 12;
        if (z < 0)
            z = -z;
        sint32 distance = x < z ? z + x / 2 : x + z / 2;
        if (distance >= 10001)
        {
            hidden = 1;
            return;
        }
    }
    else
    {
        uint16 before = lifetime;
        if (before)
            --lifetime;
        if (!before || before == 1)
            Finish();
    }
    hidden = 0;
    if (tex->key != 0x68B7B136)
    {
        uint32 tick = game_clock.tick;
        sint32 wave = rsin(((tick << 6) & 0x3FC0) >> 2);
        u_scroll = wave / 128;
    }
    uint32 tick = game_clock.tick;
    v_scroll += scroll_step;
    for (uint32 i = 0; i < 10; ++i)
    {
        uint32 val = 80 * tick + phase[i];
        sint32 wave = rsin(val & 4095);
        sint32 product = (uint32)amplitude[i] * wave;
        points[i].pos.x = rest_x[i] + ((uint32)(product / ONE) << 12);
    }
    points[0].width = 20;
    points[0].height = 60;
    width[0].target = RandomRange(60) + 20;
    for (uint32 i = 1; i < 10; ++i)
    {
        RibbonPoint *point = points + i;
        point->width += (width[i].target - point->width) / 8;
        point->height = point->width + 40;
        tick = game_clock.tick;
        if ((tick & 15) == width[i].tick)
        {
            width[i].tick = RandomRange(16);
            width[i].target = width[i - 1].target * 148 / 128;
        }
    }
    if (damage_timer)
    {
        --damage_timer;
        if (damage_timer)
            return;
    }
    if (!damage)
        return;
    Vector3 origin, endpoint, dir, hit_rec;
    origin = points[0].pos;
    endpoint = points[9].pos;
    dir.Subtract(endpoint, origin);
    dir.Normalize(dir);
    ModelInstance *hit = CollisionRay::IntersectSpheres(origin, endpoint, &hit_rec, visual_lists.players, 0);
    if (hit)
    {
        static_cast<VisualObject *>(hit->owner)->Damage(damage, &dir, 31);
        damage_timer = 15;
    }
    if (damage_actors)
    {
        hit = CollisionRay::IntersectSpheres(origin, endpoint, &hit_rec, visual_lists.actors, 0);
        if (hit)
        {
            static_cast<VisualObject *>(hit->owner)->Damage(damage, &dir, 31);
            damage_timer = 15;
        }
    }
}

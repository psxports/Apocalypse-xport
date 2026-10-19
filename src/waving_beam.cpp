#include <new>
#include <string.h>

#include "game_tick.h"

#include "visual_object.h"
#include "object_damage.h"
#include "game_diagnostics.h"
#include "waving_beam.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include "collision.h"
#include "effect_lifecycle.h"
#include "game_random.h"

WavingBeam::WavingBeam(const uint32 *position, const uint16 *angles, uint32 length, uint32 height, uint32 lifetime, uint32 red, uint32 green, uint32 blue, uint32 mode, uint32 material) : RibbonSegments(10, 0), scroll_step(mode), damage_actors(0), damage(lifetime), damage_timer(0), length(length), lifetime(height)
{
    FUNCTION_MARKER(0x80010D3C, "SLUS_003.73");
    memcpy(this->angles.words, angles, sizeof(this->angles.words));
    SetPosition(position);
    if ((lifetime & 0xFFFF) >= 1000)
    {
        damage_actors = 1;
        damage -= 1000;
    }
    SetMaterial(material);
    for (uint32 i = 0; i < 10; ++i)
    {
        rest_x[i] = points[i].pos.words[0];
        phase[i] = RandomRange(4096);
        phase_step[i] = 80;
        amplitude[i] = i ? (uint32)((sint32)length / 80) : 0;
        width[i].tick = RandomRange(16);
        if (i)
        {
            uint32 product = 148 * width[i - 1].target;
            width[i].target = (sint32)product / 128;
        }
        else
            width[i].target = RandomRange(60) + 20;
        points[i].width = width[i].target;
        points[i].height = points[i].width + 40;
        uint32 colors[3];
        colors[0] = (sint32)((9 - i) * red) / 9;
        colors[1] = (sint32)((9 - i) * green) / 9;
        colors[2] = (sint32)((9 - i) * blue) / 9;
        points[i].center.r = colors[0];
        points[i].center.g = colors[1];
        points[i].center.b = colors[2];
        points[i].edge.r = (sint32)(colors[0] * 48) / 256;
        points[i].edge.g = (sint32)(colors[1] * 48) / 256;
        points[i].edge.b = (sint32)(colors[2] * 48) / 256;
    }
}

uint32 WavingBeam::SetPosition(const uint32 *position)
{
    FUNCTION_MARKER(0x800110AC, "SLUS_003.73");
    Vector3 step, point;
    uint32 divisor = 9;
    BuildDirectionVector(step.words, length, angles.words);
    DivideVector(step.words, divisor);
    point.words[0] = position[0];
    point.words[1] = position[1];
    point.words[2] = position[2];
    for (uint32 i = 0; i < 10; ++i)
    {
        points[i].pos = point;
        AddVectors(point.words, step.words);
    }
    return 0;
}

uint32 WavingBeam::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80011058, "SLUS_003.73");
    result = RibbonSegments::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(this);
    return result;
}

void WavingBeam::Update()
{
    FUNCTION_MARKER(0x8001116C, "SLUS_003.73");
    if (lifetime == 65535)
    {
        ModelInstance *player = visual_lists.players;
        uint32 x = (sint32)(points[0].pos.words[0] - player->pos.words[0]) >> 12;
        if ((sint32)x < 0)
            x = 0 - x;
        uint32 z = (sint32)(points[0].pos.words[2] - player->pos.words[2]) >> 12;
        if ((sint32)z < 0)
            z = 0 - z;
        uint32 distance = x < z ? z + x / 2 : x + z / 2;
        if ((sint32)distance >= 10001)
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
    uint32 scroll = v_scroll;
    uint32 tick = game_clock.tick;
    v_scroll = scroll + scroll_step;
    for (uint32 i = 0; i < 10; ++i)
    {
        uint32 value = phase_step[i] * tick + phase[i];
        sint32 wave = rsin(value & 4095);
        uint32 product = amplitude[i] * (uint32)wave;
        points[i].pos.words[0] = rest_x[i] + ((uint32)((sint32)product / 4096) << 12);
    }
    points[0].width = 20;
    points[0].height = 60;
    width[0].target = RandomRange(60) + 20;
    for (uint32 i = 1; i < 10; ++i)
    {
        uint32 value = points[i].width;
        uint32 product = width[i].target - value;
        points[i].width = value + (uint32)((sint32)product / 8);
        points[i].height = points[i].width + 40;
        tick = game_clock.tick;
        if ((tick & 15) == width[i].tick)
        {
            width[i].tick = RandomRange(16);
            product = width[i - 1].target * 148;
            width[i].target = (sint32)product / 128;
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
    Vector3 origin, endpoint, direction, hit_rec;
    origin = points[0].pos;
    endpoint = points[9].pos;
    SubtractVectors(direction.words, endpoint.words, origin.words);
    NormalizeFixedVector(direction.words, direction.words);
    ModelInstance *hit = IntersectObjectSpheres(origin.words, endpoint.words, hit_rec.words, visual_lists.players, 0);
    if (hit)
    {
        static_cast<VisualObject *>(hit->owner)->Damage(damage, direction.components, 31);
        damage_timer = 15;
    }
    if (damage_actors)
    {
        hit = IntersectObjectSpheres(origin.words, endpoint.words, hit_rec.words, visual_lists.actors, 0);
        if (hit)
        {
            static_cast<VisualObject *>(hit->owner)->Damage(damage, direction.components, 31);
            damage_timer = 15;
        }
    }
}

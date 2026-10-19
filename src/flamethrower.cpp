#include <new>
#include <string.h>
#include <stdlib.h>

#include "game_heap.h"
#include "game_tick.h"

#include "effect_object.h"

#include "visual_object.h"
#include "game_diagnostics.h"
#include "flamethrower.h"
#include "audio_voice.h"
#include "angle_math.h"
#include "ribbon_segments.h"
#include "effect_sprite.h"
#include "audio_session.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "collision.h"
#include "object_damage.h"
#include "effect_lifecycle.h"
#include "game_random.h"

Flamethrower::Flamethrower(ModelInstance **list, ModelInstance **secondary_list) : WeaponNode(list, secondary_list), impact_count(0), segment_len(0), beam()
{
    FUNCTION_MARKER(0x80024428, "SLUS_003.73");
    this->type = WPN_FLAMETHROWER;
    Configure(3000);
}

Flamethrower::~Flamethrower()
{
    ClearBeam();
}

uint32 Flamethrower::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80024470, "SLUS_003.73");
    ClearBeam();
    uint32 result = WeaponNode::Destroy(0);
    return (reason & 1) ? ReleaseWeaponStorage(this) : result;
}

// TODO Recover original units for configuration field68
uint32 Flamethrower::Configure(uint32 amount)
{
    FUNCTION_MARKER(0x800244CC, "SLUS_003.73");
    uint32 result = (uint32)((sint32)amount / 20);
    this->segment_len = result;
    return result;
}

void Flamethrower::ClearBeam()
{
    FUNCTION_MARKER(0x800244F0, "SLUS_003.73");
    if (beam)
        beam->Destroy(3);
    beam = 0;
    if (sound)
    {
        StopSoundVoices(sound);
        sound = 0;
    }
}

// Original beam color attenuation helper
static uint32 ReadFlamethrowerShade(uint32 value)
{
    FUNCTION_MARKER(0x8002455C, "SLUS_003.73");
    if ((sint32)value < 0 || (sint32)value >= 2049)
        return 0;
    return 255 - ((255 * value) >> 11);
}

// Native helper retains the original signed DIV result words
static uint32 DivideFlamethrowerDistance(uint32 numerator, uint32 denominator)
{
    sint32 n = (sint32)numerator, d = (sint32)denominator;
    if (!d)
        return n < 0 ? 1 : (uint32)-1;
    if (n == (sint32)0x80000000 && d == -1)
        return 0x80000000;
    return (uint32)(n / d);
}

uint32 Flamethrower::Fire(const uint32 *position, const sint16 *angles)
{
    ModelBorrow model_borrow;
    CollisionRay &shared_ray = GetSharedCollisionRay();
    FUNCTION_MARKER(0x80024598, "SLUS_003.73");
    Vector3 step, direction, point, endpoint, start, scaled, tmp, difference, impact, fireball_velocity;
    uint32 value, shade, index, previous, count, scale;
    RibbonPoint *buffer;
    ModelInstance *target;
    uint32 object_hit = 0, geometry_hit = 0, distance = 0;
    ModelInstance **list;
    uint32 first_distance;
    uint32 second_distance;
    uint32 damage;
    GameHeapBlock *fragment;
    sint32 wave;
    ConsumeAmmo(4);
    scale = this->segment_len;
    BuildDirectionVector(step.words, scale, (const uint16 *)angles);
    DivideVector(direction.words, step.words, this->segment_len);
    if (!beam)
    {
        ClearBeam();
        GameHeapBlock *allocation = AllocateEffectStorage(108);
        if (!allocation)
        {
            GameDiagnostic("Flamethrower ribbon allocation failed\n");
            abort();
        }
        beam = new (allocation) RibbonSegments(20, 1);
        beam->persistent = 1;
        beam->SetMaterial(0x68B7B136);
        beam->waving = 1;
        // Original leaves the first point edge RGB untouched
        buffer = beam->points;
        buffer->center.r = 0;
        buffer->center.g = 0;
        buffer->center.b = 0;
        for (index = 1; index < 20; ++index)
        {
            value = (index << 10) / 10;
            shade = ReadFlamethrowerShade(value);
            buffer = beam->points;
            buffer[index].center.r = shade;
            shade = ReadFlamethrowerShade(value);
            buffer = beam->points;
            buffer[index].center.g = shade;
            shade = ReadFlamethrowerShade(value);
            buffer = beam->points;
            buffer[index].center.b = shade;
            value = LookupDistanceRoot(((value * (index << 10)) / 10) + 0x100000);
            shade = ReadFlamethrowerShade(value);
            buffer = beam->points;
            buffer[index].edge.r = shade;
            shade = ReadFlamethrowerShade(value);
            buffer = beam->points;
            buffer[index].edge.g = shade;
            shade = ReadFlamethrowerShade(value);
            buffer = beam->points;
            buffer[index].edge.b = shade;
        }
        beam->v_step = 8;
        point.words[0] = (position)[0];
        point.words[1] = (position)[1];
        point.words[2] = (position)[2];
        for (index = 0; index < 20; ++index)
        {
            buffer = beam->points;
            buffer[index].pos = point;
            AddVectors(point.words, step.words);
        }
        value = PlaySoundEffect(18, 8192, 0);
        this->sound = value;
    }
    index = beam->point_count - 1;
    if ((sint32)index > 0)
    {
        do
        {
            buffer = beam->points;
            RibbonPoint *point_dst = buffer + index;
            scale = 1;
            MultiplyVector(tmp.words, step.words, scale);
            buffer = beam->points;
            AddVectors(point.words, buffer[index - 1].pos.words, tmp.words);
            point_dst->pos = point;
            --index;
        } while ((sint32)index > 0);
    }
    point.words[0] = (position)[0];
    point.words[1] = (position)[1];
    point.words[2] = (position)[2];
    buffer = beam->points;
    buffer->pos = point;
    AddVectors(point.words, step.words);
    value = 2100 - game_clock.tick * 800;
    for (index = 2, count = 12; index < 20; ++index, count += 6, value += 1050)
    {
        wave = rsin(value & 4095);
        buffer = beam->points + index;
        wave = (6 * wave) / 4096;
        buffer->width = (uint32)wave + 12 + count;
        buffer = beam->points + index;
        buffer->height = buffer->width + 50;
    }
    buffer = beam->points;
    buffer->width = 2;
    buffer = beam->points;
    buffer->height = 15;

    value = game_clock.tick;
    buffer = beam->points;
    buffer[1].width = 15;
    wave = rsin(((960 * value) & 0x3FC0) >> 2);

    buffer = beam->points;
    buffer[1].height = (uint32)((9 * wave) / 4096) + 45;

    value = beam->v_scroll;
    scale = 20;
    beam->v_scroll = value + 8;
    MultiplyScalarVector(scaled.words, scale, step.words);
    AddVectors(endpoint.words, position, scaled.words);
    start.words[0] = (position)[0];
    start.words[1] = (position)[1];
    start.words[2] = (position)[2];
    OffsetPositionByAngles(start.words, (const uint16 *)angles);
    shared_ray.endpoints[0] = start;
    shared_ray.endpoints[1] = endpoint;
    InitializeCollisionRay(shared_ray);
    collision_query.reject_surface40 = true;
    QueryCollisionRay(shared_ray, 1);
    point.words[0] = (position)[0];
    point.words[1] = (position)[1];
    point.words[2] = (position)[2];
    list = this->owner_list;
    collision_query.reject_surface40 = false;
    target = IntersectObjectSpheres(point.words, endpoint.words, start.words, *list, 0);
    if (!target)
    {
        list = this->secondary_list;
        if (list)
            target = IntersectObjectSpheres(point.words, endpoint.words, start.words, *list, 0);
    }
    if (target && target == visual_lists.players)
    {
        first_distance = MeasurePositionDistance(target->pos.words, position);
        scale = this->segment_len;
        if (19 * scale < first_distance)
            target = 0;
    }
    if (shared_ray.hit.GetObject())
    {
        endpoint.words[0] = shared_ray.hit_pos.words[0];
        endpoint.words[1] = shared_ray.hit_pos.words[1];
        distance = shared_ray.distance - 100;
        endpoint.words[2] = shared_ray.hit_pos.words[2];
        if (!target)
            geometry_hit = 1;
        else
        {
            first_distance = MeasurePositionDistance(position, target->pos.words);
            second_distance = MeasurePositionDistance(position, endpoint.words);
            if (first_distance >= second_distance)
                geometry_hit = 1;
            else
                object_hit = 1;
        }
    }
    else if (target)
        object_hit = 1;
    if (object_hit)
    {
        endpoint.words[0] = (target->pos.words)[0];
        endpoint.words[1] = (target->pos.words)[1];
        endpoint.words[2] = (target->pos.words)[2];
        distance = MeasureEuclideanDistance(position, target->pos.words);
        damage = this->damage;
        static_cast<VisualObject *>(target->owner)->Damage(damage, direction.components, 5);
    }
    if (geometry_hit)
    {
        ModelInstance *level_target = shared_ray.hit.GetObject();
        damage = this->damage;
        DamageLevelObject(level_target, shared_ray.hit.GetPolygon(), damage);
        level_target = shared_ray.hit.GetObject();
        if (level_target && (level_target->flags & 16))
        {
            if (static_cast<VisualObject *>(level_target->owner)->kind != 205 || this->owner_list != &visual_lists.players)
            {
                damage = this->damage;
                static_cast<VisualObject *>(level_target->owner)->Damage(damage, direction.components, 5);
            }
        }
    }
    if (object_hit || geometry_hit)
    {
        scale = this->segment_len;
        index = DivideFlamethrowerDistance(distance, scale);
        buffer = beam->points;
        previous = (sint32)index < 19 ? index : index - 1;
        value = (sint32)index < 19 ? index + 1 : index;
        SubtractVectors(difference.words, buffer[value].pos.words, buffer[previous].pos.words);
        scale = this->segment_len;
        value = distance - index * scale;
        MultiplyVector(scaled.words, difference.words, value);
        DivideVector(impact.words, scaled.words, this->segment_len);
        buffer = beam->points;
        AddVectors(impact.words, buffer[index].pos.words);
        for (++index; (sint32)index < 20; ++index)
        {
            value = index;
            buffer = beam->points;
            buffer[value].pos = impact;
            buffer = beam->points;
            buffer[value].width = 0;
            buffer = beam->points;
            buffer[value].height = 0;
        }
    }
    else
        impact = beam->points[18].pos;
    value = this->impact_count + 1;
    this->impact_count = value;
    if (value & 1)
    {
        fireball_velocity.words[0] = 0;
        value = RandomRange(20);
        fireball_velocity.words[1] = (0 - 10 - value) << 12;
        fireball_velocity.words[2] = 0;
        MovingSprite *sprite = 0;
        fragment = AllocateEffectStorage(112);
        if (fragment)
        {
            value = RandomRange(1200) + 2900;
            sprite = new (fragment) MovingSprite(impact.words, fireball_velocity.words, "Fireball", value, 0, 10);
        }
        if (!sprite)
        {
            GameDiagnostic("Flamethrower impact sprite allocation failed\n");
            abort();
        }
        sprite->depth_bias = 300;
        sprite->AddFlags(8);
    }
    return 1;
}

uint32 Flamethrower::Update(const uint32 *, const sint16 *)
{
    FUNCTION_MARKER(0x80024FDC, "SLUS_003.73");
    ClearBeam();
    return 0;
}

uint32 Flamethrower::ReadLight(uint8 *red, uint8 *green, uint8 *blue)
{
    FUNCTION_MARKER(0x80024FFC, "SLUS_003.73");
    if (!beam)
        return 0;
    if (ReadWeaponLightRandom() >= 16384)
        return 0;
    *red = 255;
    *green = 128;
    *blue = 0;
    return 1;
}

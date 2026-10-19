#include <stdlib.h>
#include <new>
#include <string.h>

#include "game_heap.h"
#include "model_render_view.h"
#include "model_resources.h"
#include "angle_math.h"
#include "model_matrix.h"
#include "game_random.h"
#include "vector_distance.h"
#include "effect_lifecycle.h"
#include "collision.h"
#include "vector_arithmetic.h"
#include "game_diagnostics.h"
#include "audio_session.h"
#include "object_fragments.h"
#include "model_materials.h"
#include "model_geometry.h"
#include "textured_fragment.h"
#include "opaque_fragment.h"
#include "ring_burst_effect.h"

struct FragmentState
{
    uint16 clut;
    uint16 tpage;
    RGBC color;
    ShortVector3 normal;
    Vector3 origin;
};

static FragmentState fragment_state;

void ResetFragmentState()
{
    fragment_state.clut = 0;
    fragment_state.tpage = 0;
    fragment_state.color.rgbc = 0;
    fragment_state.normal.x = fragment_state.normal.y = fragment_state.normal.z = 0;
    fragment_state.origin.x = fragment_state.origin.y = fragment_state.origin.z = 0;
}

const Vector3 &GetFragmentOrigin()
{
    return fragment_state.origin;
}

const ShortVector3 &GetFragmentNormal()
{
    return fragment_state.normal;
}

static void TransformFragmentVertex(Vector3 &output, const ModelGeometry &geometry, const ModelPolygon &polygon, uint32 index)
{
    const SVECTOR *input = geometry.vertices + polygon.v[index];
    uint32 axis;
    uint32 shift = 12;
    FUNCTION_MARKER(0x80021ADC, "SLUS_003.73");
    xport_gte_write_data(0, ((uint16)input->vx | ((uint32)(uint16)input->vy << 16)));
    xport_gte_write_data(1, ((uint16)input->vz | ((uint32)(uint16)input->pad << 16)));
    xport_gte_execute(0x480012);
    for (axis = 0; axis < 3; ++axis)
        output.words[axis] = xport_gte_read_data(25 + axis);
    ShiftVectorLeft(output.words, shift);
}

static void SetFragmentColor(uint32 indexed, uint32 colors, uint32 count, const ModelPalette *palette)
{
    RGBC color[4];
    uint32 red, green, blue, result;
    FUNCTION_MARKER(0x80021B3C, "SLUS_003.73");
    if (indexed)
    {
        color[0].rgbc = ReadModelPaletteColor(palette, colors & 255);
        color[1].rgbc = ReadModelPaletteColor(palette, (colors >> 8) & 255);
        color[2].rgbc = ReadModelPaletteColor(palette, (colors >> 16) & 255);
        red = color[0].r + color[1].r + color[2].r;
        green = color[0].g + color[1].g + color[2].g;
        blue = color[0].b + color[1].b + color[2].b;
        if (count == 4)
        {
            color[3].rgbc = ReadModelPaletteColor(palette, colors >> 24);
            red += color[3].r;
            green += color[3].g;
            blue += color[3].b;
            fragment_state.color.r = red / 4;
            fragment_state.color.g = green / 4;
            result = blue / 4;
        }
        else
        {
            // Byte-channel sums are nonnegative and at most 765
            fragment_state.color.r = red / 3;
            fragment_state.color.g = green / 3;
            result = blue / 3;
        }
    }
    else
    {
        fragment_state.color.g = colors >> 8;
        result = colors >> 16;
        fragment_state.color.r = colors;
    }
    fragment_state.color.b = result;
}

static uint32 SpawnTexturedFragments(const Vector3 &vertex0, const Vector3 &vertex1, const Vector3 &vertex2, uint32 u0, uint32 v0, uint32 u1, uint32 v1, uint32 u2, uint32 v2, uint32 texture, uint32 subdivisions)
{
    Vector3 vector, difference, scaled, midpoint, points[3], center;
    FUNCTION_MARKER(0x80021358, "SLUS_003.73");
    ShortVector3 vertices[3], jitter, shifted_jitter;
    sint32 length, candidate;
    uint32 factor, shift;
    uint32 edge = 0;
    GameHeapBlock *object;
    uint32 i;
    uint32 component;
    uint32 u, v, result = IsGameHeapUnderPressure();
    if (result != 0)
        return result;
    if (subdivisions != 0)
    {
        SubtractVectors(vector.words, vertex1.words, vertex2.words);
        length = (sint32)MeasureVectorLength(vector.words);
        SubtractVectors(vector.words, vertex0.words, vertex2.words);
        candidate = (sint32)MeasureVectorLength(vector.words);
        if (length < candidate)
        {
            length = candidate;
            edge = 1;
        }
        SubtractVectors(vector.words, vertex0.words, vertex1.words);
        if (length < (sint32)MeasureVectorLength(vector.words))
            edge = 2;
        factor = RandomRange(9);
        if (edge == 1)
            SubtractVectors(difference.words, vertex2.words, vertex0.words);
        else if (edge == 2)
            SubtractVectors(difference.words, vertex1.words, vertex0.words);
        else
            SubtractVectors(difference.words, vertex2.words, vertex1.words);
        MultiplyVector(scaled.words, difference.words, factor);
        shift = 3;
        ShiftVectorRight(midpoint.words, scaled.words, shift);
        AddVectors(vector.words, edge == 0 ? vertex1.words : vertex0.words, midpoint.words);
        if (edge == 1)
        {
            u = u0 + (uint32)((sint32)((u2 - u0) * (uint32)factor) >> 3);
            v = v0 + (uint32)((sint32)((v2 - v0) * (uint32)factor) >> 3);
            SpawnTexturedFragments(vertex1, vertex0, vector, u1, v1, u0, v0, u, v, texture, subdivisions - 1);
            return SpawnTexturedFragments(vertex1, vertex2, vector, u1, v1, u2, v2, u, v, texture, subdivisions - 1);
        }
        if (edge == 2)
        {
            u = u0 + (uint32)((sint32)((u1 - u0) * (uint32)factor) >> 3);
            v = v0 + (uint32)((sint32)((v1 - v0) * (uint32)factor) >> 3);
            SpawnTexturedFragments(vertex2, vertex0, vector, u2, v2, u0, v0, u, v, texture, subdivisions - 1);
            return SpawnTexturedFragments(vertex2, vertex1, vector, u2, v2, u1, v1, u, v, texture, subdivisions - 1);
        }
        u = u1 + (uint32)((sint32)((u2 - u1) * (uint32)factor) >> 3);
        v = v1 + (uint32)((sint32)((v2 - v1) * (uint32)factor) >> 3);
        SpawnTexturedFragments(vertex0, vertex1, vector, u0, v0, u1, v1, u, v, texture, subdivisions - 1);
        return SpawnTexturedFragments(vertex0, vertex2, vector, u0, v0, u2, v2, u, v, texture, subdivisions - 1);
    }
    shift = 12;
    ShiftVectorRight(points[0].words, vertex0.words, shift);
    ShiftVectorRight(points[1].words, vertex1.words, shift);
    ShiftVectorRight(points[2].words, vertex2.words, shift);
    for (component = 0; component < 3; ++component)
    {
        center.words[component] = (sint32)((uint32)points[0].words[component] + (uint32)points[1].words[component] + (uint32)points[2].words[component]) / 3;
        for (i = 0; i < 3; ++i)
            vertices[i][component] = points[i].words[component] - center.words[component];
    }
    for (i = 0; i < 3; ++i)
    {
        factor = RandomRange(100);
        const ShortVector3 &normal = GetFragmentNormal();
        MultiplyScalarShortVector(jitter, factor, normal);
        ShiftShortVectorRight(shifted_jitter, jitter, shift);
        for (component = 0; component < 3; ++component)
            vertices[i][component] = (uint16)vertices[i][component] + (uint16)shifted_jitter[component];
    }
    ShiftVectorLeft(center.words, shift);
    use_small_effect_pool = false;
    object = AllocateEffectStorage(204);
    TexturedFragment *fragment = 0;
    if (object != 0)
    {
        fragment = new (object) TexturedFragment(vertices[0], vertices[1], vertices[2], center);
    }
    uint32 red = fragment_state.color.r;
    uint32 green = fragment_state.color.g;
    uint32 blue = fragment_state.color.b;
    use_small_effect_pool = true;
    if (!fragment)
    {
        GameDiagnostic("Textured fragment allocation failed\n");
        abort();
    }
    fragment->SetColors(red, green, blue);
    uint32 clut = fragment_state.clut;
    uint32 tpage = fragment_state.tpage;
    result = fragment->SetTextureCoordinates(clut, tpage, u0, v0, u1, v1, u2, v2);
    fragment->texture_window = texture;
    return result;
}

static uint32 SpawnOpaqueFragments(uint32 count, const Vector3 &vertex0, const Vector3 &vertex1, const Vector3 &vertex2, const Vector3 &normal, uint32 red, uint32 green, uint32 blue)
{
    Vector3 midpoint, sum, edge0, edge1, tmp, position, velocity, scaled[2], player;
    uint32 axis;
    uint32 index;
    uint32 value;
    uint32 floor = 0x7FFFFFFF;
    uint32 factor0;
    uint32 factor1;
    GameHeapBlock *object;
    uint32 distance;
    uint32 result;
    CollisionRay collision;
    uint32 shift = 1;
    FUNCTION_MARKER(0x80022454, "SLUS_003.73");
    red &= 255;
    green &= 255;
    blue &= 255;
    AddVectors(sum.words, vertex1.words, vertex2.words);
    ShiftVectorRight(midpoint.words, sum.words, shift);
    for (axis = 0; axis < 3; ++axis)
    {
        value = normal.words[axis];
        collision.endpoints[0].words[axis] = midpoint.words[axis] + value * 500;
    }
    collision.endpoints[1].words[0] = collision.endpoints[0].words[0];
    collision.endpoints[1].words[1] = collision.endpoints[0].words[1] + 12288000;
    collision.endpoints[1].words[2] = collision.endpoints[0].words[2];
    InitializeCollisionRay(collision);
    QueryCollisionRay(collision, 1);
    if (collision.hit.GetObject())
        floor = collision.hit_pos.words[1];
    shift = 12;
    SubtractVectors(tmp.words, vertex2.words, vertex0.words);
    ShiftVectorRight(edge0.words, tmp.words, shift);
    SubtractVectors(tmp.words, vertex1.words, vertex0.words);
    ShiftVectorRight(edge1.words, tmp.words, shift);
    for (index = 0; (sint32)index < (sint32)count; ++index)
    {
        factor0 = RandomRange(4096);
        factor1 = RandomRange(4096);
        MultiplyScalarVector(scaled[0].words, factor0, edge0.words);
        MultiplyScalarVector(scaled[1].words, factor1, edge1.words);
        AddVectors(tmp.words, scaled[0].words, scaled[1].words);
        ShiftVectorRight(position.words, tmp.words, shift);
        ShiftVectorLeft(scaled[0].words, position.words, shift);
        AddVectors(tmp.words, scaled[0].words, vertex0.words);
        position = tmp;
        SubtractVectors(velocity.words, position.words, midpoint.words);
        NormalizeFixedVector(velocity.words, velocity.words);
        factor0 = RandomRange(40);
        MultiplyVector(scaled[0].words, velocity.words, factor0);
        factor1 = RandomRange(40);
        MultiplyVector(scaled[1].words, normal.words, factor1);
        AddVectors(tmp.words, scaled[0].words, scaled[1].words);
        velocity = tmp;
        object = AllocateEffectStorage(120);
        if (object)
        {
            new (object) OpaqueFragment(position, velocity, floor, red, green, blue, 100, 100, 100);
        }
    }
    if (!positioned_sound_event.pending)
    {
        positioned_sound_event.pos = midpoint;
        positioned_sound_event.pending = 1;
        return 1;
    }
    player.words[0] = GetModelRenderView().pos.words[0];
    player.words[1] = GetModelRenderView().pos.words[1];
    player.words[2] = GetModelRenderView().pos.words[2];
    ShiftVectorLeft(player.words, shift);
    distance = MeasurePositionDistance(player.words, midpoint.words);
    result = MeasurePositionDistance(player.words, positioned_sound_event.pos.words);
    if (distance < result)
    {
        positioned_sound_event.pos = midpoint;
    }
    return result;
}

uint32 FragmentLevelPolygon(ModelInstance *object, ModelPolygon *polygon, uint32 parameter, uint32 effect, uint32 test, uint32 mark, uint32 render)
{
    Vector3 vertices[4], offset;
    ModelBorrow borrow;
    const ModelPalette *palette;
    uint32 bank, index, flags, touched, count, axis, vertex;
    uint32 result = 1, texture, color, red, green, blue, u0, v0, u1, v1, u2, v2;
    MATRIX matrix;
    FUNCTION_MARKER(0x80021CA8, "SLUS_003.73");
    bank = object->bank;
    index = object->model_idx;
    const ModelGeometry &geometry = GetModelGeometry(bank, index);
    if (test)
    {
        touched = polygon->surface;
        if (!(touched & 8) || (touched & 1))
            return 0;
        if (mark)
        {
            flags = polygon->flags;
            polygon->surface = (uint16)(touched | 1);
            polygon->flags = (uint16)(flags & 0xFE7F);
            if (!render && ((flags & 64) || !(flags & 1)))
                polygon->flags = (uint16)(flags & 0xFE3F);
        }
    }
    if (!render)
        return 1;
    flags = polygon->flags;
    if (flags & 1)
    {
        fragment_state.clut = polygon->clut;
        fragment_state.tpage = polygon->tpage;
    }
    const SVECTOR &source_normal = geometry.normals[polygon->n];
    fragment_state.normal.x = source_normal.vx;
    fragment_state.normal.y = source_normal.vy;
    fragment_state.normal.z = source_normal.vz;
    BuildModelRotationMatrix(object->angles.words, matrix);
    SetRotMatrix(&matrix);
    LoadFixedGteTranslation(object->pos.words);
    for (vertex = 0; vertex < 3; ++vertex)
        TransformFragmentVertex(vertices[vertex], geometry, *polygon, vertex);
    flags = polygon->flags;
    if (flags & 16)
    {
        count = 3;
        bank = object->bank;
        color = polygon->color;
        palette = &GetModelBank(bank).colors;
    }
    else
    {
        count = 4;
        TransformFragmentVertex(vertices[3], geometry, *polygon, 3);
        color = polygon->color;
        bank = object->bank;
        flags = polygon->flags;
        palette = &GetModelBank(bank).colors;
    }
    SetFragmentColor(flags & 2048, color, count, palette);
    for (axis = 0; axis < 3; ++axis)
    {
        uint32 sum = 0;
        sint32 center;
        for (vertex = 0; vertex < count; ++vertex)
            sum += (uint32)((sint32)vertices[vertex].words[axis] >> 12);
        center = count == 3 ? (sint32)sum / 3 : (sint32)sum >> 2;
        uint32 coordinate = (uint32)center << 12;
        switch (axis)
        {
            case 0:
                fragment_state.origin.x = coordinate;
                break;
            case 1:
                fragment_state.origin.y = coordinate;
                break;
            case 2:
                fragment_state.origin.z = coordinate;
                break;
        }
    }
    flags = polygon->flags;
    if (!(flags & 64) && (flags & 1))
    {
        texture = (flags & 32) ? GetModelPolygonWindow(*polygon) : 0;
        if (!IsGameHeapUnderPressure())
        {
            v0 = polygon->uv[0].v;
            u0 = polygon->uv[0].u;
            u1 = polygon->uv[1].u;
            v1 = polygon->uv[1].v;
            u2 = polygon->uv[2].u;
            v2 = polygon->uv[2].v;
            SpawnTexturedFragments(vertices[0], vertices[1], vertices[2], u0, v0, u1, v1, u2, v2, texture, parameter);
            if (count == 4)
            {
                v0 = polygon->uv[1].v;
                u0 = polygon->uv[1].u;
                u1 = polygon->uv[2].u;
                v1 = polygon->uv[2].v;
                u2 = polygon->uv[3].u;
                v2 = polygon->uv[3].v;
                SpawnTexturedFragments(vertices[1], vertices[2], vertices[3], u0, v0, u1, v1, u2, v2, texture, parameter);
            }
        }
    }
    else
    {
        if (!IsGameHeapUnderPressure())
        {
            const ShortVector3 &normal = GetFragmentNormal();
            if (count == 3)
                blue = fragment_state.color.b;
            offset.words[0] = (uint32)(sint32)normal.x;
            offset.words[1] = (uint32)(sint32)normal.y;
            offset.words[2] = (uint32)(sint32)normal.z;
            red = fragment_state.color.r;
            if (count == 3)
                green = fragment_state.color.g;
            else
            {
                blue = fragment_state.color.b;
                green = blue;
            }
            result = 2;
            SpawnOpaqueFragments(count == 3 ? 15 : 30, vertices[0], vertices[1], vertices[2], offset, red, green, blue);
        }
        if (mark)
        {
            flags = polygon->flags;
            polygon->flags = (uint16)(flags & 0xFFBF);
        }
    }
    if (effect && !IsGameHeapUnderPressure())
    {
        red = fragment_state.color.r;
        green = fragment_state.color.g;
        blue = fragment_state.color.b;
        SpawnExplosionRing(fragment_state.origin.words, 100, red, green, blue, 4, 1, 100);
    }
    return result;
}

uint32 FragmentLevelObject(ModelInstance *object, uint32 test, uint32 mark)
{
    ModelBorrow borrow;
    uint32 bank, index, drawn = 0, any = 0, result, random, flags;
    FUNCTION_MARKER(0x80022318, "SLUS_003.73");
    bank = object->bank;
    index = object->model_idx;
    const ModelGeometry &geometry = GetModelGeometry(bank, index);
    ModelPolygon *polygon = geometry.polys;
    index = 0;
    while ((sint32)index < (sint32)geometry.poly_count)
    {
        random = RandomRange(5);
        result = FragmentLevelPolygon(object, polygon, random == 0, 0, test, mark, (sint32)drawn < 6);
        if (result)
        {
            any = 1;
            if (result == 1)
                ++drawn;
        }
        ++index;
        polygon = polygon->next;
    }
    if (mark && any)
    {
        flags = object->flags;
        object->flags = flags | 1;
    }
    return any;
}

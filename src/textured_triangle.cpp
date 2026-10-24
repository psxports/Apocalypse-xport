#include <new>

#include "game_heap.h"
#include "effect_object.h"
#include "object_fragments.h"
#include "game_diagnostics.h"
#include "textured_fragment.h"
#include "textured_triangle.h"
#include "angle_math.h"
#include "model_matrix.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "sprite_trail.h"
#include "hd.h"

TexturedTriangle::TexturedTriangle(const ShortVector3 &v0, const ShortVector3 &v1, const ShortVector3 &v2) : EffectObject(), hd_history(0), uv_clut(0), uv_tpage(0), uv_tail(0), texture_window(0)
{
    const ShortVector3 *inputs[3] = {&v0, &v1, &v2};
    Vector3 edge[2];
    uint32 axis, idx, factor;
    Vector3 cross, unit_n;
    FUNCTION_MARKER(0x800340A4, "SLUS_003.73");
    texture_region.x = texture_region.y = 0;
    texture_region.width = texture_region.height = 0;
    angles.x = angles.y = angles.z = 0;
    base_color.rgbc = 0;
    for (idx = 0; idx < 3; ++idx)
        colors[idx].rgbc = 0;
    for (idx = 0; idx < 4; ++idx)
        transformed[idx].x = transformed[idx].y = transformed[idx].z = 0;
    kind = 6;
    for (idx = 0; idx < 3; ++idx)
    {
        vertices[idx] = *inputs[idx];
    }
    for (axis = 0; axis < 3; ++axis)
        edge[0].comp[axis] = v2[axis] - v0[axis];
    for (axis = 0; axis < 3; ++axis)
        edge[1].comp[axis] = v1[axis] - v0[axis];
    cross.x = (uint32)edge[0].y * edge[1].z - (uint32)edge[0].z * edge[1].y;
    cross.y = (uint32)edge[0].z * edge[1].x - (uint32)edge[0].x * edge[1].z;
    cross.z = (uint32)edge[0].x * edge[1].y - (uint32)edge[0].y * edge[1].x;
    unit_n.Normalize(cross);
    factor = RandomRange(96) + 128;
    for (axis = 0; axis < 3; ++axis)
    {
        sint32 product = (uint32)unit_n.comp[axis] * factor;
        n.comp[axis] = product >> 12;
    }
    AddToList(effect_lists.textured_triangles);
}

TexturedTriangle::~TexturedTriangle()
{
    hd_polygon_chain_free(hd_history);
}

uint32 TexturedTriangle::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800342B0, "SLUS_003.73");
    RemoveFromList(effect_lists.textured_triangles);
    uint32 result = EffectObject::Destroy(0);
    return (reason & 1) ? ReleaseStorage() : result;
}

void TexturedTriangle::SetColors(uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x80034314, "SLUS_003.73");
    base_color.r = r;
    base_color.g = g;
    base_color.b = b;
    base_color.code = 0;
    for (uint32 idx = 0; idx < 3; ++idx)
    {
        uint32 factor = RandomRange(4096);
        colors[idx].b = (b * factor) >> 12;
        colors[idx].g = (g * factor) >> 12;
        colors[idx].r = (r * factor) >> 12;
        colors[idx].code = 0;
    }
}

uint32 TexturedTriangle::SetTexCoords(uint16 clut, uint16 tpage, uint8 u0, uint8 v0, uint8 u1, uint8 v1, uint8 u2, uint8 v2)
{
    FUNCTION_MARKER(0x8003445C, "SLUS_003.73");
    uv_clut = ((uint32)clut << 16) | (v0 << 8) | u0;
    uv_tpage = ((uint32)tpage << 16) | (v1 << 8) | u1;
    uv_tail = u2 | (v2 << 8);
    return uv_tail;
}

void TexturedTriangle::TransformVertices()
{
    MATRIX matrix;
    FUNCTION_MARKER(0x800344BC, "SLUS_003.73");
    angles.BuildRotationMatrix(matrix);
    SetRotMatrix(&matrix);
    pos.LoadGteTranslation();
    // Original fourth input is transformed with the same translation as the corners
    for (uint32 vertex = 0; vertex < 4; ++vertex)
    {
        const ShortVector3 *input = vertex < 3 ? vertices + vertex : &n;
        XY xy;
        xy.x = input->x;
        xy.y = input->y;
        uint32 z = (uint16)input->z;
        xport_gte_write_data(0, xy.xy);
        xport_gte_write_data(1, z);
        xport_gte_execute(0x480012);
        transformed[vertex].x = xport_gte_read_data(25);
        transformed[vertex].y = xport_gte_read_data(26);
        transformed[vertex].z = xport_gte_read_data(27);
    }
}

void TexturedTriangle::Update()
{
    FUNCTION_MARKER(0x8003A144, "SLUS_003.73");
}

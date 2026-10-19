#include <new>

#include "game_heap.h"
#include "effect_object.h"

#include "object_fragments.h"
#include "game_diagnostics.h"
#include "textured_fragment.h"
#include "angle_math.h"
#include "model_matrix.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "game_random.h"
#include "effect_lifecycle.h"
#include "sprite_trail.h"

TexturedTriangle::TexturedTriangle(const ShortVector3 &vertex0, const ShortVector3 &vertex1, const ShortVector3 &vertex2) : EffectObject(), uv_clut(0), uv_tpage(0), uv_tail(0), texture_window(0)
{
    const ShortVector3 *inputs[3] = {&vertex0, &vertex1, &vertex2};
    Vector3 edge[2];
    uint32 axis, index, factor;
    Vector3 cross, unit_normal;
    FUNCTION_MARKER(0x800340A4, "SLUS_003.73");
    angles.x = angles.y = angles.z = 0;
    base_color.rgbc = 0;
    for (index = 0; index < 3; ++index)
        colors[index].rgbc = 0;
    for (index = 0; index < 4; ++index)
        transformed[index].x = transformed[index].y = transformed[index].z = 0;
    kind = 6;
    for (index = 0; index < 3; ++index)
    {
        vertices[index] = *inputs[index];
    }
    for (axis = 0; axis < 3; ++axis)
    {
        uint32 a = vertex2[axis];
        uint32 b = vertex0[axis];
        edge[0].words[axis] = a - b;
    }
    for (axis = 0; axis < 3; ++axis)
    {
        uint32 a = vertex1[axis];
        uint32 b = vertex0[axis];
        edge[1].words[axis] = a - b;
    }
    cross.words[0] = edge[0].words[1] * edge[1].words[2] - edge[0].words[2] * edge[1].words[1];
    cross.words[1] = edge[0].words[2] * edge[1].words[0] - edge[0].words[0] * edge[1].words[2];
    cross.words[2] = edge[0].words[0] * edge[1].words[1] - edge[0].words[1] * edge[1].words[0];
    NormalizeFixedVector(unit_normal.words, cross.words);
    factor = RandomRange(96) + 128;
    normal.x = (sint32)(unit_normal.words[0] * factor) >> 12;
    normal.y = (sint32)(unit_normal.words[1] * factor) >> 12;
    normal.z = (sint32)(unit_normal.words[2] * factor) >> 12;
    AddToList(effect_lists.textured_triangles);
}

uint32 TexturedTriangle::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800342B0, "SLUS_003.73");
    RemoveFromList(effect_lists.textured_triangles);
    uint32 result = EffectObject::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : result;
}

void TexturedTriangle::Update()
{
    FUNCTION_MARKER(0x8003A144, "SLUS_003.73");
}

void TexturedTriangle::TransformVertices()
{
    MATRIX matrix;
    FUNCTION_MARKER(0x800344BC, "SLUS_003.73");
    BuildModelRotationMatrix(angles.words, matrix);
    SetRotMatrix(&matrix);
    LoadFixedGteTranslation(pos.words);
    // Original fourth input is transformed with the same translation as the corners
    for (uint32 vertex = 0; vertex < 4; ++vertex)
    {
        const ShortVector3 *input = vertex < 3 ? vertices + vertex : &normal;
        uint32 xy = input->words[0] | ((uint32)input->words[1] << 16);
        uint32 z = input->words[2];
        xport_gte_write_data(0, xy);
        xport_gte_write_data(1, z);
        xport_gte_execute(0x480012);
        transformed[vertex].x = xport_gte_read_data(25);
        transformed[vertex].y = xport_gte_read_data(26);
        transformed[vertex].z = xport_gte_read_data(27);
    }
}

void TexturedTriangle::SetColors(uint32 red, uint32 green, uint32 blue)
{
    uint32 index, factor, r, g, b;
    FUNCTION_MARKER(0x80034314, "SLUS_003.73");
    red &= 255;
    green &= 255;
    blue &= 255;
    base_color.rgbc = (blue << 16) | (green << 8) | red;
    for (index = 0; index < 3; ++index)
    {
        factor = RandomRange(4096);
        b = (uint32)((sint32)(blue * factor) >> 12);
        g = (uint32)((sint32)(green * factor) >> 12);
        r = (uint32)((sint32)(red * factor) >> 12);
        colors[index].rgbc = (b << 16) | (g << 8) | r;
    }
}

uint32 TexturedTriangle::SetTextureCoordinates(uint32 clut, uint32 tpage, uint32 u0, uint32 v0, uint32 u1, uint32 v1, uint32 u2, uint32 v2)
{
    uint32 result;
    FUNCTION_MARKER(0x8003445C, "SLUS_003.73");
    uv_clut = (clut << 16) | ((v0 & 255) << 8) | (u0 & 255);
    uv_tpage = (tpage << 16) | ((v1 & 255) << 8) | (u1 & 255);
    result = (u2 & 255) | ((v2 & 255) << 8);
    uv_tail = result;
    return result;
}

TexturedFragment::TexturedFragment(const ShortVector3 &vertex0, const ShortVector3 &vertex1, const ShortVector3 &vertex2, const Vector3 &position) : TexturedTriangle(vertex0, vertex1, vertex2), lifetime(0), trail(0)
{
    GameHeapBlock *child;
    uint32 angle;
    FUNCTION_MARKER(0x80020FB4, "SLUS_003.73");
    if (RandomRange(2))
    {
        angle = RandomRange(160) + 80;
        angular_velocity.x = angle;
        angular_velocity.y = 0;
        angular_velocity.z = 0;
    }
    else
    {
        angle = RandomRange(160) + 80;
        angular_velocity.x = 0;
        angular_velocity.y = angle;
        angular_velocity.z = 0;
    }
    lifetime = RandomRange(30) + 45;
    InitializeVelocity(position);
    TransformVertices();
    if (RandomRange(2) && !IsGameHeapUnderPressure())
    {
        child = AllocateEffectStorage(100);
        if (child)
        {
            trail = new (child) RibbonTrail(this->pos.words, 4, 128, 128, 128);
        }
        // TODO Original allocation failure still leaves no usable trail
        if (!trail)
        {
            GameDiagnostic("Effect trail allocation failed\n");
            abort();
        }
        trail->persistent = 1;
        trail->SetWidth(1200);
        trail->SetColors(32, 32, 32);
    }
}

uint32 TexturedFragment::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800210D8, "SLUS_003.73");
    if (trail)
    {
        trail->Destroy(3);
        trail = 0;
    }
    TexturedTriangle::Destroy(0);
    return (reason & 1) ? ReleaseEffectStorage(this) : 0;
}

void TexturedFragment::InitializeVelocity(const Vector3 &position)
{
    Vector3 difference, scaled, velocity;
    uint32 distance, factor, axis, value, result;
    uint32 shift = 1;
    FUNCTION_MARKER(0x80021154, "SLUS_003.73");
    SetPosition(position.words);
    SubtractVectors(difference.words, position.words, GetFragmentOrigin().words);
    distance = MeasureVectorLength(difference.words);
    if (distance)
    {
        factor = RandomRange(48);
        MultiplyScalarVector(scaled.words, factor, difference.words);
        DivideVector(velocity.words, scaled.words, distance);
        this->velocity = velocity;
    }
    else
        Finish();
    factor = RandomRange(45);
    const ShortVector3 &normal = GetFragmentNormal();
    for (axis = 0; axis < 3; ++axis)
    {
        value = normal[axis];
        value = this->velocity.words[axis] + factor * value;
        this->velocity.words[axis] = value;
    }
    value = RandomRange(175);
    this->velocity.words[1] = this->velocity.words[1] - (value << 12);
    if (!RandomRange(20))
        ShiftVectorLeft(this->velocity.words, shift);
    result = this->velocity.words[1];
    if ((sint32)result > 0)
        this->velocity.words[1] = 0;
    if (trail)
        trail->SetPosition(pos.words);
}

void TexturedFragment::Update()
{
    uint32 lifetime;
    FUNCTION_MARKER(0x800212C8, "SLUS_003.73");
    AddVectors(this->pos.words, this->velocity.words);
    this->velocity.words[1] = this->velocity.words[1] + 20500;
    AddAngleVector(angles.words, angular_velocity.words);
    if (trail)
        trail->SetPosition(pos.words);
    TransformVertices();
    lifetime = this->lifetime;
    if (!lifetime)
    {
        Finish();
        return;
    }
    this->lifetime = lifetime - 1;
}

TexturedFragment::~TexturedFragment()
{
    if (trail)
        trail->Destroy(3);
}

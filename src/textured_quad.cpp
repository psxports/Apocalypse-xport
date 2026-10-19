#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_diagnostics.h"
#include "textured_quad.h"
#include "model_resources.h"
#include "vector_arithmetic.h"
#include "vector_distance.h"
#include "effect_lifecycle.h"

TexturedQuad::TexturedQuad() : EffectObject(), color(0), flags(0), tex(0)
{
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        saved_pos.words[axis] = 0;
        for (uint32 corner = 0; corner < 3; ++corner)
        {
            corners[corner].words[axis] = 0;
            saved_corners[corner].words[axis] = 0;
        }
    }
    FUNCTION_MARKER(0x80033688, "SLUS_003.73");
    this->color = 0x2C808080;
    AddToList(effect_lists.quads);
}

uint32 TexturedQuad::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800336D8, "SLUS_003.73");
    RemoveFromList(effect_lists.quads);
    uint32 result = EffectObject::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(this);
    return result;
}

void TexturedQuad::SelectResource(const char *name, uint32 index)
{
    FUNCTION_MARKER(0x80033764, "SLUS_003.73");
    tex = GetTextureFrame(FindModelTextureGroup(name), (sint32)index).tex;
}

const TextureResource *TexturedQuad::GetTexture() const
{
    return tex;
}

uint32 TexturedQuad::AddFlags(uint32 flags)
{
    FUNCTION_MARKER(0x8003384C, "SLUS_003.73");
    uint32 result = this->flags | flags;
    this->flags = result;
    return result;
}

uint32 TexturedQuad::EnableSemiTransparency()
{
    FUNCTION_MARKER(0x80033874, "SLUS_003.73");
    uint32 result = this->color | 0x2000000;
    this->color = result;
    return result;
}

uint32 TexturedQuad::SetBrightness(uint32 brightness)
{
    FUNCTION_MARKER(0x800338A0, "SLUS_003.73");
    uint32 result = this->color & 0xFF000000;
    brightness &= 255;
    result |= (brightness << 16) | (brightness << 8) | brightness;
    this->color = result;
    return result;
}

void TexturedQuad::SelectResource(uint32 key)
{
    FUNCTION_MARKER(0x800337B4, "SLUS_003.73");
    tex = FindModelResourceEntry(key);
}

uint32 TexturedQuad::SetColors(uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x800338CC, "SLUS_003.73");
    uint32 result = (this->color & 0xFF000000) | ((blue & 255) << 16) | ((green & 255) << 8) | (red & 255);
    this->color = result;
    return result;
}

void TexturedQuad::SetPosition(const uint32 *position, const sint16 *direction, uint32 scale0, uint32 scale1)
{
    FUNCTION_MARKER(0x80033900, "SLUS_003.73");
    Vector3 basis[2], tmp[2], out;
    uint32 len[2];
    uint32 x, y, z, product;
    basis[0].words[0] = 0;
    z = (uint32)(sint32)direction[2];
    basis[0].words[1] = 0 - z;
    y = (uint32)(sint32)direction[1];
    basis[0].words[2] = y;
    y = (uint32)(sint32)direction[1];
    product = y * y;
    z = (uint32)(sint32)direction[2];
    basis[1].words[0] = product + z * z;
    x = (uint32)(sint32)direction[0];
    y = (uint32)(sint32)direction[1];
    basis[1].words[1] = (0 - x) * y;
    z = (uint32)(sint32)direction[2];
    x = (uint32)(sint32)direction[0];
    basis[1].words[2] = (0 - z) * x;
    len[0] = MeasureVectorLength(basis[0].words);
    len[1] = MeasureVectorLength(basis[1].words);
    MultiplyVector(tmp[0].words, basis[0].words, scale0);
    DivideVector(out.words, tmp[0].words, len[0]);
    basis[0] = out;
    MultiplyVector(tmp[1].words, basis[1].words, scale1);
    DivideVector(out.words, tmp[1].words, len[1]);
    basis[1] = out;
    SubtractVectors(tmp[0].words, position, basis[0].words);
    SubtractVectors(out.words, tmp[0].words, basis[1].words);
    this->pos = out;
    AddVectors(tmp[0].words, position, basis[0].words);
    SubtractVectors(out.words, tmp[0].words, basis[1].words);
    this->corners[0] = out;
    SubtractVectors(tmp[0].words, position, basis[0].words);
    AddVectors(out.words, tmp[0].words, basis[1].words);
    this->corners[1] = out;
    AddVectors(tmp[0].words, position, basis[0].words);
    AddVectors(out.words, tmp[0].words, basis[1].words);
    this->corners[2] = out;
}

static void SmoothQuadPosition(Vector3 &position, Vector3 &history)
{
    Vector3 current, previous_quarter, current_quarter, weighted, output;
    uint32 shift = 2, weight = 3;
    current = position;
    ShiftVectorRight(previous_quarter.words, history.words, shift);
    ShiftVectorRight(current_quarter.words, current.words, shift);
    MultiplyScalarVector(weighted.words, weight, current_quarter.words);
    AddVectors(output.words, previous_quarter.words, weighted.words);
    position = output;
    history = current;
}

void SmoothTexturedQuadPositions(void)
{
    FUNCTION_MARKER(0x800303F4, "SLUS_003.73");
    EffectObject *object = effect_lists.quads;
    while (object)
    {
        TexturedQuad &quad = *static_cast<TexturedQuad *>(object);
        SmoothQuadPosition(quad.pos, quad.saved_pos);
        for (uint32 i = 0; i < 3; ++i)
            SmoothQuadPosition(quad.corners[i], quad.saved_corners[i]);
        object = object->next;
    }
}

uint32 SaveTexturedQuadPositions(void)
{
    FUNCTION_MARKER(0x800306D8, "SLUS_003.73");
    EffectObject *object = effect_lists.quads;
    while (object)
    {
        TexturedQuad &quad = *static_cast<TexturedQuad *>(object);
        quad.saved_pos = quad.pos;
        for (uint32 i = 0; i < 3; ++i)
            quad.saved_corners[i] = quad.corners[i];
        object = object->next;
    }
    return 0;
}

uint32 RestoreTexturedQuadPositions(void)
{
    FUNCTION_MARKER(0x80030764, "SLUS_003.73");
    EffectObject *object = effect_lists.quads;
    while (object)
    {
        TexturedQuad &quad = *static_cast<TexturedQuad *>(object);
        quad.pos = quad.saved_pos;
        for (uint32 i = 0; i < 3; ++i)
            quad.corners[i] = quad.saved_corners[i];
        object = object->next;
    }
    return 0;
}

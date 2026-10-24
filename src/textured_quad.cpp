#include "textured_quad.h"
#include "model_bank.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "effect_object.h"
#include "texture_resources.h"
#include "hd.h"

void TexturedQuad::SmoothPos(Vector3 &pos, Vector3 &history)
{
    const Vector3 cur = pos;
    Vector3 prev_quarter, cur_quarter, weighted;
    prev_quarter.ShiftRight(history, 2);
    cur_quarter.ShiftRight(cur, 2);
    weighted.Scale(3, cur_quarter);
    pos.Add(prev_quarter, weighted);
    history = cur;
}

void TexturedQuad::SmoothListPos(void)
{
    FUNCTION_MARKER(0x800303F4, "SLUS_003.73");
    EffectObject *object = effect_lists.quads;
    while (object)
    {
        TexturedQuad &quad = *static_cast<TexturedQuad *>(object);
        TexturedQuad::SmoothPos(quad.pos, quad.saved_pos);
        for (uint32 i = 0; i < 3; ++i)
            TexturedQuad::SmoothPos(quad.corners[i], quad.saved_corners[i]);
        object = object->next;
    }
}

uint32 TexturedQuad::SaveListPos(void)
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

uint32 TexturedQuad::RestoreListPos(void)
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

TexturedQuad::TexturedQuad() : EffectObject(), hd_history(0), color(0), flags(0), tex(0)
{
    for (uint32 axis = 0; axis < 3; ++axis)
    {
        saved_pos.comp[axis] = 0;
        for (uint32 corner = 0; corner < 3; ++corner)
        {
            corners[corner].comp[axis] = 0;
            saved_corners[corner].comp[axis] = 0;
        }
    }
    FUNCTION_MARKER(0x80033688, "SLUS_003.73");
    color = 0x2C808080;
    AddToList(effect_lists.quads);
}

TexturedQuad::~TexturedQuad()
{
    hd_polygon_chain_free(hd_history);
}

uint32 TexturedQuad::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x800336D8, "SLUS_003.73");
    RemoveFromList(effect_lists.quads);
    uint32 result = EffectObject::Destroy(0);
    if (reason & 1)
        return ReleaseStorage();
    return result;
}

void TexturedQuad::SelectResource(const char *name, sint32 idx)
{
    FUNCTION_MARKER(0x80033764, "SLUS_003.73");
    tex = TextureGroup::GetFrame(TextureGroup::Find(name), idx).tex;
}

void TexturedQuad::SelectResource(uint32 key)
{
    FUNCTION_MARKER(0x800337B4, "SLUS_003.73");
    tex = TextureResource::Find(key);
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
    uint32 result = color | 0x2000000;
    color = result;
    return result;
}

uint32 TexturedQuad::SetBrightness(uint8 brightness)
{
    FUNCTION_MARKER(0x800338A0, "SLUS_003.73");
    uint32 result = color & 0xFF000000;
    result |= (brightness << 16) | (brightness << 8) | brightness;
    color = result;
    return result;
}

uint32 TexturedQuad::SetColors(uint8 r, uint8 g, uint8 b)
{
    FUNCTION_MARKER(0x800338CC, "SLUS_003.73");
    uint32 result = (color & 0xFF000000) | (b << 16) | (g << 8) | r;
    color = result;
    return result;
}

void TexturedQuad::SetPos(const Vector3 *pos, const ShortVector3 *dir, uint32 scale0, uint32 scale1)
{
    FUNCTION_MARKER(0x80033900, "SLUS_003.73");
    Vector3 basis[2], tmp[2];
    uint32 len[2];
    sint32 x = dir->x, y = dir->y, z = dir->z;
    basis[0].x = 0;
    basis[0].y = -z;
    basis[0].z = y;
    uint32 y_sq = y * y;
    basis[1].x = y_sq + z * z;
    basis[1].y = -x * y;
    basis[1].z = -z * x;
    len[0] = basis[0].Len();
    len[1] = basis[1].Len();
    tmp[0].Multiply(basis[0], scale0);
    basis[0].Divide(tmp[0], len[0]);
    tmp[1].Multiply(basis[1], scale1);
    basis[1].Divide(tmp[1], len[1]);
    tmp[0].Subtract(*pos, basis[0]);
    this->pos.Subtract(tmp[0], basis[1]);
    tmp[0].Add(*pos, basis[0]);
    corners[0].Subtract(tmp[0], basis[1]);
    tmp[0].Subtract(*pos, basis[0]);
    corners[1].Add(tmp[0], basis[1]);
    tmp[0].Add(*pos, basis[0]);
    corners[2].Add(tmp[0], basis[1]);
}

const TextureResource *TexturedQuad::GetTex() const
{
    return tex;
}

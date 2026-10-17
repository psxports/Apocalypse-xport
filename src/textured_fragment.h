#ifndef TEXTURED_FRAGMENT_H
#define TEXTURED_FRAGMENT_H

#include "effect_object.h"

// TODO Recover the original names for the triangle and fragment owners
class TexturedTriangle : public EffectObject
{
public:
    TexturedTriangle(const sint16 *vertex0, const sint16 *vertex1, const sint16 *vertex2);
    virtual ~TexturedTriangle() {}
    virtual uint32 Destroy(uint32 reason);
    static TexturedTriangle &Attach(uint32 object);
    uint32 TransformVertices();
    uint32 SetColors(uint32 red, uint32 green, uint32 blue);
    uint32 SetTextureCoordinates(uint32 clut, uint32 tpage, uint32 u0, uint32 v0, uint32 u1, uint32 v1, uint32 u2, uint32 v2);
    virtual void Update();
};

class TexturedFragment : public TexturedTriangle
{
public:
    TexturedFragment(const sint16 *vertex0, const sint16 *vertex1, const sint16 *vertex2, const uint32 *position);
    virtual ~TexturedFragment() {}
    static TexturedFragment &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
    uint32 InitializeVelocity(const uint32 *position);
};

#endif

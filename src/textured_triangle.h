#ifndef TEXTURED_TRIANGLE_H
#define TEXTURED_TRIANGLE_H

#include "effect_object.h"
#include "vector3.h"
#include "textured_fragment.h"

class RibbonTrail;

// TODO Recover the original names for the triangle and fragment owners
class TexturedTriangle : public EffectObject
{
private:
    ShortVector3 vertices[3];
    ShortVector3 n;

public:
    Vector3 transformed[4];

protected:
    ShortVector3 angles;

public:
    uint32 uv_clut;
    uint32 uv_tpage;
    uint16 uv_tail;
    RGBC base_color;
    RGBC colors[3];
    uint32 texture_window;

    virtual ~TexturedTriangle()
    {
    }

public:
    TexturedTriangle(const ShortVector3 &v0, const ShortVector3 &v1, const ShortVector3 &v2);
    virtual uint32 Destroy(uint32 reason);
    void SetColors(uint8 r, uint8 g, uint8 b);
    uint32 SetTexCoords(uint16 clut, uint16 tpage, uint8 u0, uint8 v0, uint8 u1, uint8 v1, uint8 u2, uint8 v2);

protected:
    void TransformVertices();

public:
    virtual void Update();
};

#endif

#ifndef TEXTURED_FRAGMENT_H
#define TEXTURED_FRAGMENT_H

#include "effect_object.h"
#include "vector3.h"

class RibbonTrail;

// TODO Recover the original names for the triangle and fragment owners
class TexturedTriangle : public EffectObject
{
public:
    ShortVector3 vertices[3];
    ShortVector3 normal;
    Vector3 transformed[4];
    ShortVector3 angles;
    uint32 uv_clut;
    uint32 uv_tpage;
    uint16 uv_tail;
    RGBC base_color;
    RGBC colors[3];
    uint32 texture_window;

    TexturedTriangle(const ShortVector3 &vertex0, const ShortVector3 &vertex1, const ShortVector3 &vertex2);

    virtual ~TexturedTriangle()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    void SetColors(uint32 red, uint32 green, uint32 blue);
    uint32 SetTextureCoordinates(uint32 clut, uint32 tpage, uint32 u0, uint32 v0, uint32 u1, uint32 v1, uint32 u2, uint32 v2);
    virtual void Update();

protected:
    void TransformVertices();
};

class TexturedFragment : public TexturedTriangle
{
public:
    TexturedFragment(const ShortVector3 &vertex0, const ShortVector3 &vertex1, const ShortVector3 &vertex2, const Vector3 &position);
    virtual ~TexturedFragment();
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    void InitializeVelocity(const Vector3 &position);
    uint16 lifetime;
    ShortVector3 angular_velocity;
    ObjectReference<RibbonTrail> trail;
};

#endif

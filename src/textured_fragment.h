#ifndef TEXTURED_FRAGMENT_H
#define TEXTURED_FRAGMENT_H

#include "object_reference.h"
#include "effect_object.h"
#include "vector3.h"
#include "ribbon_trail.h"
#include "textured_triangle.h"

class RibbonTrail;

class TexturedFragment : public TexturedTriangle
{
private:
    uint16 lifetime;
    ShortVector3 angular_velocity;
    ObjectReference<RibbonTrail> trail;

public:
    TexturedFragment(const ShortVector3 &v0, const ShortVector3 &v1, const ShortVector3 &v2, const Vector3 &pos);
    virtual uint32 Destroy(uint32 reason);

private:
    void InitializeVelocity(const Vector3 &src_pos);

public:
    virtual void Update();
    virtual ~TexturedFragment();
};

#endif

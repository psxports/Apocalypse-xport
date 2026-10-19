#ifndef PLAGUE_AREA_H
#define PLAGUE_AREA_H

#include "textured_quad.h"
#include "vector3.h"

// TODO Recover the resource name corresponding to key 019D23F9
class PlagueArea : public TexturedQuad
{
public:
    PlagueArea();

    virtual ~PlagueArea()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

private:
    friend class Plague;
    uint16 age;
    uint16 lifetime;
    Vector3 center;
    uint32 radius;
};

#endif

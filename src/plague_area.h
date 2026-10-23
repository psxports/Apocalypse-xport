#ifndef PLAGUE_AREA_H
#define PLAGUE_AREA_H

#include "textured_quad.h"
#include "vector3.h"

// Texture key 019D23F9 has no recovered resource name
class PlagueArea : public TexturedQuad
{
public:
    virtual ~PlagueArea()
    {
    }

private:
    friend class Plague;
    sint16 age;
    uint16 lifetime;
    Vector3 center;
    uint32 radius;

public:
    virtual void Update();
    virtual uint32 Destroy(uint32 reason);
    PlagueArea();
};

#endif

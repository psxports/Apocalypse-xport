#ifndef PLAGUE_AREA_H
#define PLAGUE_AREA_H

#include "textured_quad.h"

// TODO Recover the resource name corresponding to key 019D23F9
class PlagueArea : public TexturedQuad
{
public:
    PlagueArea();
    virtual ~PlagueArea() {}
    static PlagueArea &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();
};

#endif

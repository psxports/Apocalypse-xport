#ifndef WAR_EFFECTS_H
#define WAR_EFFECTS_H

#include "effect_object.h"

class WarImpact : public EffectController
{
public:
    WarImpact(const uint32 *position);
    virtual ~WarImpact() {}
    static WarImpact &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();

};

#endif

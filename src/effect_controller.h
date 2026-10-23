#ifndef EFFECT_CONTROLLER_H
#define EFFECT_CONTROLLER_H

#include "vector3.h"
#include "texture_resources.h"
#include "object.h"
#include "effect_object.h"

// Update-only list owner whose children provide rendering
class EffectController : public EffectObject
{
public:
    virtual ~EffectController()
    {
    }

protected:
    EffectController();

public:
    virtual uint32 Destroy(uint32 reason);
};

#endif

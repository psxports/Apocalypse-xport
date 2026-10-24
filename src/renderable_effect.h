#ifndef RENDERABLE_EFFECT_H
#define RENDERABLE_EFFECT_H

#include "vector3.h"
#include "texture_resources.h"
#include "object.h"
#include "effect_object.h"

class RenderableEffect : public EffectObject
{
public:
    virtual ~RenderableEffect()
    {
    }

protected:
    RenderableEffect();

public:
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Render();
    virtual bool CaptureHdTick();
};

#endif

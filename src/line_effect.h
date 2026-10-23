#ifndef LINE_EFFECT_H
#define LINE_EFFECT_H

#include "vector3.h"
#include "texture_resources.h"
#include "object.h"
#include "effect_object.h"

// TODO: Recover the original name of the line-effect base at 800A1B08
class LineEffect : public EffectObject
{
public:
    uint32 color[2];
    Vector3 endpoint[2];

    virtual ~LineEffect()
    {
    }

protected:
    LineEffect();

public:
    virtual uint32 Destroy(uint32 reason);
};

#endif

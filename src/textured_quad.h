#ifndef TEXTURED_QUAD_H
#define TEXTURED_QUAD_H
#include "effect_object.h"

void SmoothTexturedQuadPositions(void);
uint32 SaveTexturedQuadPositions(void);
uint32 RestoreTexturedQuadPositions(void);

class TexturedQuad : public EffectObject
{
public:
    TexturedQuad();
    virtual ~TexturedQuad() {}
    static TexturedQuad &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    uint32 SelectResource(const char *name, uint32 index);
    uint32 SelectResource(uint32 key);
    uint32 SetColors(uint32 red, uint32 green, uint32 blue);
    void SetPosition(const uint32 *position, const sint16 *direction, uint32 first_scale, uint32 second_scale);
    uint32 AddFlags(uint32 flags);
    uint32 EnableSemiTransparency();
    uint32 SetBrightness(uint32 brightness);
};

#endif

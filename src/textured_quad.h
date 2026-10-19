#ifndef TEXTURED_QUAD_H
#define TEXTURED_QUAD_H

#include "vector3.h"
#include "effect_object.h"
#include "texture_resources.h"

void SmoothTexturedQuadPositions(void);
uint32 SaveTexturedQuadPositions(void);
uint32 RestoreTexturedQuadPositions(void);

class TexturedQuad : public EffectObject
{
public:
    Vector3 corners[3];
    Vector3 saved_pos;
    Vector3 saved_corners[3];
    uint32 color;
    uint8 flags;

    TexturedQuad();

    virtual ~TexturedQuad()
    {
    }

    virtual uint32 Destroy(uint32 reason);
    void SelectResource(const char *name, uint32 index);
    void SelectResource(uint32 key);
    uint32 SetColors(uint32 red, uint32 green, uint32 blue);
    void SetPosition(const uint32 *position, const sint16 *direction, uint32 scale0, uint32 scale1);
    uint32 AddFlags(uint32 flags);
    uint32 EnableSemiTransparency();
    uint32 SetBrightness(uint32 brightness);
    const TextureResource *GetTexture() const;

private:
    TextureResource *tex;
};

#endif

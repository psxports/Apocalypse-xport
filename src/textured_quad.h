#ifndef TEXTURED_QUAD_H
#define TEXTURED_QUAD_H

#include "vector3.h"
#include "effect_object.h"
#include "texture_resources.h"

class TexturedQuad : public EffectObject
{
public:
    Vector3 corners[3];

private:
    Vector3 saved_pos;
    Vector3 saved_corners[3];

public:
    uint32 color;
    uint8 flags;

    virtual ~TexturedQuad()
    {
    }

private:
    const TextureResource *tex;

public:
    static void SmoothListPos(void);
    static uint32 SaveListPos(void);
    static uint32 RestoreListPos(void);
    TexturedQuad();
    virtual uint32 Destroy(uint32 reason);
    void SelectResource(const char *name, sint32 idx);
    void SelectResource(uint32 key);
    uint32 AddFlags(uint32 flags);
    uint32 EnableSemiTransparency();
    uint32 SetBrightness(uint8 brightness);
    uint32 SetColors(uint8 r, uint8 g, uint8 b);
    void SetPos(const Vector3 *pos, const ShortVector3 *dir, uint32 scale0, uint32 scale1);
    const TextureResource *GetTex() const;

private:
    static void SmoothPos(Vector3 &pos, Vector3 &history);
};

#endif

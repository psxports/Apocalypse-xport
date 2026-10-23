#ifndef TEXTURED_EFFECT_H
#define TEXTURED_EFFECT_H

#include "vector3.h"
#include "texture_resources.h"
#include "object.h"
#include "effect_object.h"

class TexturedEffect : public EffectObject
{
public:
    sint16 color_step;
    uint16 shrink_step;
    uint32 color;

private:
    const TextureGroup *tex_grp;
    sint32 tex_frame;

public:
    uint8 flags;
    uint8 frame_count;
    sint8 frame;
    uint8 frame_fraction;
    uint16 frame_step;
    uint16 size;

    virtual ~TexturedEffect()
    {
    }

protected:
    TexturedEffect();

public:
    virtual uint32 Destroy(uint32 reason);
    uint32 AddFlags(uint32 flags);
    void SetFrameStep(uint16 val);
    void SetSize(uint16 size);
    uint32 EnableSemiTransparency();
    uint32 SetBrightness(uint8 intensity);
    uint32 SetColors(uint8 r, uint8 g, uint8 b);
    void SetColorStep(sint16 step);
    uint8 SelectResource(const char *name);
    uint8 SelectResource(uint32 resource);
    uint32 SetFrame(sint8 frame);
    uint32 AdvanceFrame();
    uint32 AdvanceLoopingFrame();
    uint32 RandomizeFrame();
    uint32 Shrink(bool finish_when_empty);
    uint32 FadeColors(bool finish_when_empty);
    const TextureFrame &GetTexFrame() const;
};

#endif

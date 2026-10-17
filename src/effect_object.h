#ifndef EFFECT_OBJECT_H
#define EFFECT_OBJECT_H
extern "C"
{
#include "psx.h"
}

#include "texture_resources.h"
#include "object.h"

// Original texture slots initialized from the table at 800A67CC
enum EffectResource
{
    VFX_EXP_FIRE = 0,
    VFX_FIREBALL = 1,
    VFX_FIRE = 2,
    VFX_FONT = 3,
    VFX_FONTSMLL = 4,
    VFX_SHADOW = 5,
    VFX_ROCK = 6,
    VFX_SMOKE = 7,
    VFX_PANEL = 8,
    VFX_EXP_ACT = 9,
    VFX_ORBIT = 10,
    VFX_RIBBON = 11,
    VFX_FIRETILE = 12,
    VFX_SMTILE = 13,
    VFX_FIRECYCL = 14
};

class EffectObject : public Object
{
public:
    virtual ~EffectObject() {}
    virtual uint32 Destroy(uint32 reason);
    uint32 Finish();
    uint32 IntegrateMotion();
    uint32 SetPosition(const uint32 *position);
    uint32 AddToList(uint32 *head);
    uint32 RemoveFromList(uint32 *head);

protected:
    EffectObject();
};

// TODO: Recover the original name of the line-effect base at 800A1B08
class LineEffect : public EffectObject
{
public:
    virtual ~LineEffect() {}
    virtual uint32 Destroy(uint32 reason);
    virtual void Update();

protected:
    LineEffect();
};

class RenderableEffect : public EffectObject
{
public:
    virtual ~RenderableEffect() {}
    virtual uint32 Destroy(uint32 reason);

protected:
    RenderableEffect();
};

// Base for primitive particles rendered from the particle list
class PrimitiveParticle : public EffectObject
{
public:
    PrimitiveParticle();
    virtual ~PrimitiveParticle() {}
    static PrimitiveParticle &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update();
};

// Update-only list owner whose children provide rendering
class EffectController : public EffectObject
{
public:
    virtual ~EffectController() {}
    virtual uint32 Destroy(uint32 reason);

protected:
    EffectController();
};

class TexturedEffect : public EffectObject
{
public:
    static TexturedEffect &Attach(uint32 object);
    const TextureFrame &GetTextureFrame() const;
    virtual ~TexturedEffect() {}
    virtual uint32 Destroy(uint32 reason);
    uint32 SelectResource(const char *name);
    uint32 SelectResource(uint32 resource);
    uint32 SetColors(uint32 red, uint32 green, uint32 blue);
    uint32 SetBrightness(uint32 intensity);
    uint32 RandomizeFrame();
    uint32 EnableSemiTransparency();
    uint32 AddFlags(uint32 flags);
    void SetSize(uint32 size);
    void SetFrameStep(uint32 value);
    void SetColorStep(uint32 step);
    uint32 SetFrame(uint32 frame);
    uint32 AdvanceFrame();
    uint32 AdvanceLoopingFrame();
    uint32 Shrink(uint32 finish_when_empty);
    uint32 FadeColors(uint32 finish_when_empty);

protected:
    TexturedEffect();

private:
    const TextureGroup *tex_group;
    sint32 tex_frame;
};

#endif

#ifndef EFFECT_OBJECT_H
#define EFFECT_OBJECT_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}

#include "texture_resources.h"
#include "object.h"
#include "model_reference.h"

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
    EffectObject *prev;
    EffectObject *next;
    Vector3 pos;
    Vector3 velocity;
    Vector3 acceleration;
    uint8 drag[3];
    bool finished;
    uint16 depth_bias;
    bool persistent;
    uint8 kind;
    ModelReference hd_attachment_owner;
    Vector3 hd_attachment_origin;
    uint32 hd_attachment_tick;
    bool hd_attachment_birth_only;
    mutable VECTORF hd_release_offset;
    mutable bool hd_release_ready;

private:
    EffectObject **list_head;

public:
    static void InitializeResources();

private:
    static void DestroyTransientList(EffectObject *object);

public:
    static void DestroyTransient();

private:
    static void DestroyFinishedList(EffectObject *object);

public:
    static void DestroyFinished();
    static GameHeapBlock *AllocateStorage(uint32 size);

protected:
    EffectObject *AddToList(EffectObject *&head);
    EffectObject *RemoveFromList(EffectObject *&head);

public:
    uint32 Finish();
    uint32 SetPos(const Vector3 *src_pos);

protected:
    EffectObject();

public:
    virtual uint32 Destroy(uint32 reason);

protected:
    void IntegrateMotion();

private:
    static void UpdateList(EffectObject *node);

public:
    static void UpdateAll(void);
    virtual void Update();
    static const TextureResource *GetShadowTex();
    static const TextureGroup *GetButtonResourceGrp();
    static const TextureGroup *GetResourceGrp(uint32 resource);
    static const DR_TPAGE &GetDrawMode();
    static void ForgetTex(const TextureGroup *grp);
    static void ForgetTex(const TextureResource *tex);
    static void ResetDrawMode();
    virtual ~EffectObject();
};

#endif

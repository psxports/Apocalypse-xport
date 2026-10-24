#ifndef MODEL_INSTANCE_H
#define MODEL_INSTANCE_H

#include "model_reference.h"
#include "vector3.h"
#include "game_lighting.h"

extern "C"
{
#include "psx.h"
}

#include "object.h"

void BeginModelBorrow();
void EndModelBorrow();
void RetireModelMemory(void *memory);

class ModelBorrow
{
public:
    ModelBorrow()
    {
        BeginObjectBorrow();
        BeginModelBorrow();
    }

    ~ModelBorrow()
    {
        EndObjectBorrow();
        EndModelBorrow();
    }

private:
    ModelBorrow(const ModelBorrow &);
    ModelBorrow &operator=(const ModelBorrow &);
};

class ScaledObject;
struct CollisionRay;
struct ModelInstance;
struct ModelPolygon;
struct ObjectFlash;
struct ModelAttachmentTable;
struct HD_OBJECT_STATE;
struct HD_MODEL_HISTORY;
struct GameLighting;

// Common model values follow original offsets; list identity has a separate lifetime
struct ModelInstance
{
private:
    friend void DamageLevelObject(ModelInstance *object, ModelPolygon *polygon, sint32 damage);

public:
    uint16 flags;
    uint16 generation;
    Vector3 pos;
    ShortVector3 angles;
    uint16 model_idx;
    uint8 frame;
    uint8 flash;
    uint8 animation;
    uint8 bank;
    uint32 color;
    ModelInstance *next;
    ModelInstance *prev;
    ShortVector3 scale;
    uint32 render_mode;
    ScaledObject *owner;
    bool active;
    GameLighting hd_lighting;
    bool hd_lighting_valid;
    mutable HD_MODEL_HISTORY *hd_history;

    ModelInstance() : flags(0), generation(0), model_idx(0), frame(0), flash(0), animation(0), bank(0), color(0), next(0), prev(0), render_mode(0), owner(0), active(true), hd_lighting_valid(false), hd_history(0), references(0)
    {
        for (uint32 axis = 0; axis < 3; ++axis)
        {
            pos.comp[axis] = 0;
            angles.comp[axis] = 0;
            scale.comp[axis] = ONE;
        }
    }

    void SetAnglesXY(uint32 packed)
    {
        angles.x = packed;
        angles.y = packed >> 16;
    }

private:
    friend class ModelReference;
    ModelReference *references;
    ModelInstance &operator=(const ModelInstance &);

private:
    ModelInstance(const ModelInstance &);

public:
    static uint32 InitializeFlashes();

private:
    static void ReleaseFlash(ObjectFlash *flash);

public:
    static void UpdateFlashes();

private:
    static ObjectFlash *AcquireFlash(ModelInstance *object);
    uint32 FlashColor(uint8 r, uint8 g, uint8 b, uint32 duration);
    uint32 FadeToBlack(uint32 duration);
    uint32 DestroyLevelObject();
    uint32 FragmentPolygon(ModelPolygon *polygon, uint32 subdiv, bool effect, bool test, bool mark, bool render);

public:
    uint32 Fragment(bool test, bool mark);
    uint32 IntersectCol(CollisionRay &rec);
    void SetAttachmentTable(const ModelAttachmentTable &table) const;
    static void RenderVisibleList(ModelInstance *owner);
    static void RenderList(ModelInstance *object);
    static void CullList(ModelInstance *object);
    void ScaleRotationMatrix(MATRIX &matrix) const;
    static void ReleaseFlash(uint8 idx);
    ~ModelInstance();
    void InvalidateReferences();
    static void ResetDestruction();
    static void ForgetFlashes(const ModelInstance *objects, uint32 count);
    void PrepareVisibleDraw(bool update_lighting);
    bool CaptureHdPrepared(bool update_effects) const;
    bool CaptureHdTick() const;
    bool CaptureHdRegistry() const;
    bool CaptureHdState(HD_OBJECT_STATE &state, const GameLighting &lighting, uint32 mask, const VECTORF &constant) const;
    void ResetHdHistory();
};

#endif

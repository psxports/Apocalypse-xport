#include <string.h>
#include <stdlib.h>

#include "texture_resources.h"
#include "game_heap.h"
#include "path_particle_emitter.h"
#include "game_diagnostics.h"
#include "effect_lifecycle.h"
#include "model_bank.h"
#include "global.h"
#include "effect_object.h"
#include "object_initializers.h"

static DR_TPAGE draw_mode = {0};

uint32 effect_count;

bool use_small_effect_pool = true;

EffectLists effect_lists;

static EffectObject **const list_heads[13] = {&effect_lists.controllers, &effect_lists.sprs, &effect_lists.endpoint_sprs, &effect_lists.particles, &effect_lists.points, &effect_lists.segments, &effect_lists.quads, &effect_lists.polygons, &effect_lists.textured_triangles, &effect_lists.rings, &effect_lists.opaque_fragments, &effect_lists.lines, &effect_lists.renderables};

static EffectObject **const update_heads[13] = {&effect_lists.particles, &effect_lists.sprs, &effect_lists.opaque_fragments, &effect_lists.polygons, &effect_lists.textured_triangles, &effect_lists.quads, &effect_lists.rings, &effect_lists.controllers, &effect_lists.endpoint_sprs, &effect_lists.points, &effect_lists.segments, &effect_lists.lines, &effect_lists.renderables};

static const TextureGroup *grps[15];

static const TextureGroup *button_grp;

static const TextureResource *shadow_tex;

static const char *const grp_names[15] = {"ExpFire", "Fireball", "Fire", "Font", "Fontsmll", "Shadow", "Rock", "Smoke", "Panel", "ExpAct", "orbit", "Ribbon", "firetile", "smtile", "firecycl"};

void EffectObject::InitializeResources()
{
    FUNCTION_MARKER(0x80032A0C, "SLUS_003.73");
    effect_count = 0;
    for (uint32 i = 0; i < 13; ++i)
        *list_heads[i] = 0;
    setDrawMode1(&draw_mode, 1, 1, 0x20);
    for (uint32 i = 0; i < 15; ++i)
        grps[i] = TextureGroup::Find(grp_names[i]);
    button_grp = TextureGroup::Find("Buttons");
    shadow_tex = TextureResource::Find(0x2A3B2550);
}

void EffectObject::DestroyTransientList(EffectObject *object)
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x80032AF4, "SLUS_003.73");
    while (object)
    {
        EffectObject *next = object->next;
        if (object->IsAlive() && !object->persistent)
            object->Destroy(3);
        object = next;
    }
}

void EffectObject::DestroyTransient()
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x80032B64, "SLUS_003.73");
    for (uint32 idx = 0; idx < 13; ++idx)
        EffectObject::DestroyTransientList(*list_heads[idx]);
}

void EffectObject::DestroyFinishedList(EffectObject *object)
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x80032C18, "SLUS_003.73");
    while (object)
    {
        EffectObject *next = object->next;
        if (object->IsAlive() && object->finished)
            object->Destroy(3);
        object = next;
    }
}

void EffectObject::DestroyFinished()
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x80032C88, "SLUS_003.73");
    for (uint32 idx = 0; idx < 13; ++idx)
        EffectObject::DestroyFinishedList(*list_heads[idx]);
}

GameHeapBlock *EffectObject::AllocateStorage(uint32 size)
{
    FUNCTION_MARKER(0x80032DC0, "SLUS_003.73");
    return AllocateHeapBlock(size, use_small_effect_pool ? -1 : 0, 1);
}

EffectObject *EffectObject::AddToList(EffectObject *&head)
{
    FUNCTION_MARKER(0x80032E50, "SLUS_003.73");
    list_head = &head;
    prev = 0;
    next = head;
    head = this;
    if (next)
        next->prev = this;
    return next;
}

EffectObject *EffectObject::RemoveFromList(EffectObject *&head)
{
    FUNCTION_MARKER(0x80032E7C, "SLUS_003.73");
    if (next)
        next->prev = prev;
    if (prev)
        prev->next = next;
    if (head == this)
        head = next;
    if (list_head == &head)
        list_head = 0;
    return head;
}

EffectObject::~EffectObject()
{
    if (list_head)
        RemoveFromList(*list_head);
}

void EffectObject::ResetDrawMode()
{
    draw_mode.tag = 0;
    draw_mode.code[0] = 0;
}

const DR_TPAGE &EffectObject::GetDrawMode()
{
    return draw_mode;
}

void EffectObject::ForgetTex(const TextureGroup *grp)
{
    for (uint32 i = 0; i < 15; ++i)
        if (grps[i] == grp)
            grps[i] = 0;
    if (button_grp == grp)
        button_grp = 0;
}

void EffectObject::ForgetTex(const TextureResource *tex)
{
    if (shadow_tex == tex)
        shadow_tex = 0;
}

const TextureGroup *EffectObject::GetResourceGrp(uint32 resource)
{
    if (resource >= 15)
    {
        GameDiagnostic(("Invalid effect resource group %u\n", resource));
        abort();
    }
    return grps[resource];
}

const TextureGroup *EffectObject::GetButtonResourceGrp()
{
    return button_grp;
}

const TextureResource *EffectObject::GetShadowTex()
{
    return shadow_tex;
}

uint32 EffectObject::Finish()
{
    FUNCTION_MARKER(0x80032ED8, "SLUS_003.73");
    finished = true;
    return 1;
}

uint32 EffectObject::SetPos(const Vector3 *src_pos)
{
    FUNCTION_MARKER(0x80032EE4, "SLUS_003.73");
    const Vector3 position = *src_pos;
    pos = position;
    return position.x;
}

EffectObject::EffectObject() : prev(0), next(0), finished(false), depth_bias(0), persistent(false), kind(0), list_head(0)
{
    pos = zero_vector;
    velocity = zero_vector;
    acceleration = zero_vector;
    FUNCTION_MARKER(0x80032F7C, "SLUS_003.73");
    drag[0] = 1;
    drag[1] = 1;
    drag[2] = 1;
    ++effect_count;
}

uint32 EffectObject::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80032FB8, "SLUS_003.73");
    --effect_count;
    MarkDestroyed();
    if (reason & 1)
        return ReleaseStorage();
    return 0x800A1DB8;
}

void EffectObject::IntegrateMotion()
{
    FUNCTION_MARKER(0x80032FFC, "SLUS_003.73");
    pos.Add(velocity);
    velocity.Add(acceleration);
    velocity.Damp(drag);
}

void EffectObject::UpdateList(EffectObject *node)
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x80037258, "SLUS_003.73");
    while (node)
    {
        if (node->IsAlive() && !node->finished)
            node->Update();
        node = node->next;
    }
}

void EffectObject::UpdateAll(void)
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x800372B8, "SLUS_003.73");
    for (uint32 i = 0; i < 13; ++i)
        EffectObject::UpdateList(*update_heads[i]);
}

void EffectObject::Update()
{
    FUNCTION_MARKER(0x8003A144, "SLUS_003.73");
}

#include <stdlib.h>

#include "effect_object.h"
#include "path_particle_emitter.h"
#include "game_diagnostics.h"

#include "effect_lifecycle.h"
#include "model_resources.h"
#include "game_heap.h"

static DR_TPAGE effect_draw_mode = {0};

void ResetEffectDrawMode()
{
    effect_draw_mode.tag = 0;
    effect_draw_mode.code[0] = 0;
}

const DR_TPAGE &GetEffectDrawMode()
{
    return effect_draw_mode;
}

uint32 effect_count;
bool use_small_effect_pool = true;

EffectLists effect_lists;

static EffectObject **const effect_list_heads[13] = {&effect_lists.controllers, &effect_lists.sprites, &effect_lists.endpoint_sprites, &effect_lists.particles, &effect_lists.points, &effect_lists.segments, &effect_lists.quads, &effect_lists.polygons, &effect_lists.textured_triangles, &effect_lists.rings, &effect_lists.opaque_fragments, &effect_lists.lines, &effect_lists.renderables};

static const TextureGroup *effect_groups[15];
static const TextureGroup *button_group;
static TextureResource *shadow_tex;
static const char *const effect_group_names[15] = {"ExpFire", "Fireball", "Fire", "Font", "Fontsmll", "Shadow", "Rock", "Smoke", "Panel", "ExpAct", "orbit", "Ribbon", "firetile", "smtile", "firecycl"};

void ForgetEffectTexture(const TextureGroup *grp)
{
    for (uint32 i = 0; i < 15; ++i)
        if (effect_groups[i] == grp)
            effect_groups[i] = 0;
    if (button_group == grp)
        button_group = 0;
}

void ForgetEffectTexture(const TextureResource *tex)
{
    if (shadow_tex == tex)
        shadow_tex = 0;
}

void ClearEffectResourceBindings()
{
    for (uint32 i = 0; i < 15; ++i)
        effect_groups[i] = 0;
    button_group = 0;
    shadow_tex = 0;
}

const TextureGroup *GetEffectResourceGroup(uint32 resource)
{
    if (resource >= 15)
    {
        GameDiagnostic("Invalid effect resource group %u\n", resource);
        abort();
    }
    return effect_groups[resource];
}

const TextureGroup *GetButtonResourceGroup()
{
    return button_group;
}

const TextureResource *GetShadowTexture()
{
    return shadow_tex;
}

void InitializeEffectResources()
{
    FUNCTION_MARKER(0x80032A0C, "SLUS_003.73");
    effect_count = 0;
    for (uint32 i = 0; i < 13; ++i)
        *effect_list_heads[i] = 0;
    setDrawMode1(&effect_draw_mode, 1, 1, 0x20);
    for (uint32 i = 0; i < 15; ++i)
        effect_groups[i] = FindModelTextureGroup(effect_group_names[i]);
    button_group = FindModelTextureGroup("Buttons");
    shadow_tex = FindModelResourceEntry(0x2A3B2550);
}

static void DestroyTransientEffectList(EffectObject *object)
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

void DestroyTransientEffects()
{
    ObjectBorrow object_borrow;
    uint32 index;
    EffectObject *head;
    FUNCTION_MARKER(0x80032B64, "SLUS_003.73");
    for (index = 0; index < 13; ++index)
    {
        head = *effect_list_heads[index];
        DestroyTransientEffectList(head);
    }
}

static void DestroyFinishedEffectList(EffectObject *object)
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

void DestroyFinishedEffects()
{
    ObjectBorrow object_borrow;
    uint32 index;
    EffectObject *head;
    FUNCTION_MARKER(0x80032C88, "SLUS_003.73");
    for (index = 0; index < 13; ++index)
    {
        head = *effect_list_heads[index];
        DestroyFinishedEffectList(head);
    }
}

GameHeapBlock *AllocateEffectStorage(uint32 size)
{
    FUNCTION_MARKER(0x80032DC0, "SLUS_003.73");
    return AllocateHeapBlock(size, use_small_effect_pool ? -1 : 0, 1);
}

uint32 ReleaseEffectStorage(Object *object)
{
    FUNCTION_MARKER(0x80032E30, "SLUS_003.73");
    return object->ReleaseStorage();
}

static void UpdateEffectList(EffectObject *head)
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x80037258, "SLUS_003.73");
    EffectObject *node = head;
    while (node)
    {
        if (node->IsAlive() && !node->finished)
            node->Update();
        node = node->next;
    }
}

void UpdateEffects(void)
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x800372B8, "SLUS_003.73");
    EffectObject **const heads[13] = {&effect_lists.particles, &effect_lists.sprites, &effect_lists.opaque_fragments, &effect_lists.polygons, &effect_lists.textured_triangles, &effect_lists.quads, &effect_lists.rings, &effect_lists.controllers, &effect_lists.endpoint_sprites, &effect_lists.points, &effect_lists.segments, &effect_lists.lines, &effect_lists.renderables};
    for (uint32 i = 0; i < 13; ++i)
        UpdateEffectList(*heads[i]);
}

void DestroyEffectsByIdentifier(uint32 identifier)
{
    ObjectBorrow object_borrow;
    FUNCTION_MARKER(0x8001F718, "SLUS_003.73");
    EffectObject *object = effect_lists.controllers;
    while (object)
    {
        uint32 kind = object->kind;
        EffectObject *next = object->next;
        if (object->IsAlive() && kind == 7 && static_cast<PathParticleEmitter *>(object)->identifier == identifier)
            object->Destroy(3);
        object = next;
    }
}

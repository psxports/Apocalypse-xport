#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_diagnostics.h"
#include "visual_object.h"
#include "player.h"
#include "fire_cycle.h"
#include "palette_resource.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "animated_sprite.h"
#include "effect_object.h"
#include "effect_controller.h"

FireCycle::FireCycle(const Vector3 *pos, uint16 lifetime, uint8 enabled, uint8 damage) : EffectController(), age(0), lifetime(lifetime), spawn_age(0), damage(damage), enabled(enabled)
{
    FUNCTION_MARKER(0x8001DB68, "SLUS_003.73");
    SetPos(pos);
    SpawnSpr(0);
}

FireCycle::~FireCycle()
{
    for (uint32 idx = 0; idx < 20; ++idx)
        if (sprs[idx])
            sprs[idx]->Destroy(3);
}

uint32 FireCycle::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001DBFC, "SLUS_003.73");
    for (uint32 idx = 0; idx < 20; ++idx)
    {
        if (sprs[idx])
            sprs[idx]->Destroy(3);
        sprs[idx] = 0;
    }
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

void FireCycle::SpawnSpr(uint32 idx)
{
    FUNCTION_MARKER(0x8001DCA8, "SLUS_003.73");
    spawn_age = age;
    Vector3 tmp_pos = pos;
    sint32 offset = RandomRange(200);
    tmp_pos.x = (uint32)tmp_pos.x + ((offset - 100) * ONE);
    offset = RandomRange(200);
    use_small_effect_pool = false;
    tmp_pos.z = (uint32)tmp_pos.z + ((offset - 100) * ONE);
    GameHeapBlock *allocation = EffectObject::AllocateStorage(120);
    AnimatedSprite *spr = 0;
    if (allocation)
        spr = new (allocation) AnimatedSprite(&tmp_pos, "firecycl", 3072, 1, 0, -1);
    use_small_effect_pool = true;
    if (!spr)
    {
        GameDiagnostic(("FireCycle sprite allocation failed\n"));
        abort();
    }
    spr->persistent = true;
    spr->frame_step = 0;
    spr->scale[0] = RandomRange(2048) + 3072;
    spr->scale[1] = RandomRange(1024) + 1024;
    spr->AddFlags(8);
    spr->angle = RandomRange(4096);
    const uint16 clut_y = PaletteResource::GetLargePos().y;
    spr->lifetime = 4096;
    spr->clut = getClut(PaletteResource::GetLargePos().x, clut_y);
    spr->color &= 0xFF000000;
    sprs[idx] = spr;
}

void FireCycle::Update()
{
    FUNCTION_MARKER(0x8001DE24, "SLUS_003.73");
    ObjectBorrow object_borrow;
    if (enabled)
    {
        ModelInstance *player = visual_lists.players;
        if (player && player->pos.y < pos.y)
        {
            if (pos.HorizontalDistance(player->pos) < 256)
            {
                player = visual_lists.players;
                const Vector3 zero = {{{0, 0, 0}}};
                static_cast<Player *>(player->owner)->Damage(damage, &zero, 0);
            }
        }
    }
    sint32 empty = -1;
    for (uint32 idx = 0; idx < 20; ++idx)
    {
        AnimatedSprite *spr = sprs[idx].Get();
        if (!spr)
        {
            if (empty == -1)
                empty = idx;
            continue;
        }
        if (spr->lifetime >= 97)
        {
            sint32 elapsed = 4096 - spr->lifetime;
            if (elapsed < 1024)
            {
                uint32 shade = elapsed >> 3;
                spr->color = (spr->color & 0xFF000000) | (shade << 16) | (shade << 8) | shade;
            }
            uint32 random = RandomRange(64);
            spr->lifetime -= random + 32;
            uint32 base = getClut(PaletteResource::GetLargePos().x, PaletteResource::GetLargePos().y);
            sint32 delta = spr->clut - base;
            if (delta < 2)
            {
                if (!RandomRange(9))
                    ++spr->clut;
            }
            else if (delta < 15 && !RandomRange(3))
            {
                ++spr->clut;
                if (delta == 12)
                    spr->blend_mode = 64;
            }
        }
        else
        {
            uint8 shade = spr->color;
            if (shade < 5)
            {
                spr->Destroy(3);
                sprs[idx] = 0;
                continue;
            }
            shade -= 2 + RandomRange(3);
            spr->color = (spr->color & 0xFF000000) | (shade << 16) | (shade << 8) | shade;
        }
        uint16 width = spr->scale[0];
        uint16 height = spr->scale[1];
        if (width > 1024)
            width -= 83;
        if (height < 2048)
            height += 33;
        spr->pos.y = (uint32)spr->pos.y - (49152);
        spr->scale[0] = width;
        spr->scale[1] = height;
        spr->angle += idx & 1 ? 123 : -123;
    }
    ++age;
    if (lifetime < age)
    {
        Finish();
        return;
    }
    if (RandomRange(8) && age - spawn_age < 9)
        return;
    if (empty != -1)
        SpawnSpr(empty);
}

FireCycle *FireCycle::Spawn(const Vector3 *pos, uint16 lifetime, uint8 enabled, uint8 damage)
{
    FUNCTION_MARKER(0x8001E400, "SLUS_003.73");
    GameHeapBlock *allocation = EffectObject::AllocateStorage(156);
    if (!allocation)
        return 0;
    return new (allocation) FireCycle(pos, lifetime, enabled, damage);
}

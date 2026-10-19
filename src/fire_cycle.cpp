#include <new>
#include <stdlib.h>

#include "game_heap.h"
#include "game_diagnostics.h"
#include "visual_object.h"
#include "player.h"
#include "fire_cycle.h"
#include "palette_resources.h"
#include "effect_lifecycle.h"
#include "game_random.h"
#include "vector_distance.h"
#include "effect_sprite.h"

FireCycle::FireCycle(const uint32 *position, uint32 size, uint32 enabled, uint32 count) : EffectController(), age(0), lifetime(size), spawn_age(0), damage(count), enabled(enabled)
{
    FUNCTION_MARKER(0x8001DB68, "SLUS_003.73");
    SetPosition(position);
    SpawnSprite(0);
}

FireCycle::~FireCycle()
{
    for (uint32 idx = 0; idx < 20; ++idx)
        if (sprites[idx])
            sprites[idx]->Destroy(3);
}

uint32 FireCycle::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x8001DBFC, "SLUS_003.73");
    for (uint32 idx = 0; idx < 20; ++idx)
    {
        if (sprites[idx])
            sprites[idx]->Destroy(3);
        sprites[idx] = 0;
    }
    EffectController::Destroy(0);
    return reason & 1 ? ReleaseEffectStorage(this) : 0;
}

void FireCycle::SpawnSprite(uint32 idx)
{
    FUNCTION_MARKER(0x8001DCA8, "SLUS_003.73");
    spawn_age = age;
    Vector3 position = pos;
    position.words[0] += (RandomRange(200) - 100) << 12;
    uint32 random = RandomRange(200);
    use_small_effect_pool = false;
    position.words[2] += (random - 100) << 12;
    GameHeapBlock *allocation = AllocateEffectStorage(120);
    AnimatedSprite *sprite = 0;
    if (allocation)
    {
        sprite = new (allocation) AnimatedSprite(position.words, "firecycl", 3072, 1, 0, (uint32)-1);
    }
    use_small_effect_pool = true;
    if (!sprite)
    {
        GameDiagnostic("FireCycle sprite allocation failed\n");
        abort();
    }
    sprite->persistent = 1;
    sprite->frame_step = 0;
    sprite->scale[0] = RandomRange(2048) + 3072;
    sprite->scale[1] = RandomRange(1024) + 1024;
    sprite->AddFlags(8);
    sprite->angle = RandomRange(4096);
    uint32 clut = (uint16)GetLargePalettePos().y;
    sprite->lifetime = 4096;
    uint32 texture = GetLargePalettePos().x;
    sprite->clut = (clut << 6) | ((uint32)((sint32)texture >> 4) & 63);
    sprite->color &= 0xFF000000;
    sprites[idx] = sprite;
}

void FireCycle::Update()
{
    FUNCTION_MARKER(0x8001DE24, "SLUS_003.73");
    ObjectBorrow object_borrow;
    if (enabled)
    {
        ModelInstance *player = visual_lists.players;
        if (player && (sint32)player->pos.words[1] < (sint32)pos.words[1])
        {
            if (MeasureHorizontalDistance(pos.words, player->pos.words) < 256)
            {
                player = visual_lists.players;
                const sint32 zero[3] = {0, 0, 0};
                static_cast<Player *>(player->owner)->Damage(damage, zero, 0);
            }
        }
    }
    sint32 empty = -1;
    for (uint32 idx = 0; idx < 20; ++idx)
    {
        AnimatedSprite *sprite = sprites[idx].Get();
        if (!sprite)
        {
            if (empty == -1)
                empty = idx;
            continue;
        }
        uint32 value = sprite->lifetime;
        if (value >= 97)
        {
            value = 4096 - sprite->lifetime;
            if ((sint32)value < 1024)
            {
                value = (uint32)((sint32)value >> 3);
                sprite->color = (sprite->color & 0xFF000000) | (value << 16) | (value << 8) | value;
            }
            uint32 random = RandomRange(64);
            sprite->lifetime -= random + 32;
            uint32 clut = GetLargePalettePos().y;
            uint32 texture = GetLargePalettePos().x;
            uint32 base = (clut << 6) | ((uint32)((sint32)texture >> 4) & 63);
            sint32 delta = (sint32)((uint32)sprite->clut - base);
            if (delta < 2)
            {
                if (!RandomRange(9))
                    ++sprite->clut;
            }
            else if (delta < 15 && !RandomRange(3))
            {
                ++sprite->clut;
                if (delta == 12)
                    sprite->blend_mode = 64;
            }
        }
        else
        {
            value = sprite->color & 255;
            if (value < 5)
            {
                sprite->Destroy(3);
                sprites[idx] = 0;
                continue;
            }
            value -= 2 + RandomRange(3);
            sprite->color = (sprite->color & 0xFF000000) | (value << 16) | (value << 8) | value;
        }
        uint32 width = sprite->scale[0];
        uint32 height = sprite->scale[1];
        if (width > 1024)
            width -= 83;
        if (height < 2048)
            height += 33;
        sprite->pos.words[1] -= 49152;
        sprite->scale[0] = width;
        sprite->scale[1] = height;
        sprite->angle += idx & 1 ? 123 : -123;
    }
    ++age;
    if ((sint32)lifetime < (sint16)age)
    {
        Finish();
        return;
    }
    if (RandomRange(8) && (sint32)((uint32)(sint32)(sint16)age - spawn_age) < 9)
        return;
    if (empty != -1)
        SpawnSprite(empty);
}

FireCycle *SpawnFireCycle(const uint32 *position, uint32 size, uint32 enabled, uint32 count)
{
    FUNCTION_MARKER(0x8001E400, "SLUS_003.73");
    GameHeapBlock *allocation = AllocateEffectStorage(156);
    if (!allocation)
        return 0;
    return new (allocation) FireCycle(position, size, enabled, count);
}

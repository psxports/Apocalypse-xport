#include "visual_object.h"
#include "game_sdk_bindings.h"
#include "items.h"
#include "plague.h"
#include "the_beast.h"
#include "war.h"
#include "death.h"
#include "scripted_level_object.h"
#include "level_hazards.h"
#include "record_scripts.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "object_damage.h"
#include "object_fragments.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"
#include "glow_effect.h"
#include "native_object_storage.h"
#include "audio_session.h"
#include "game_random.h"
#include "vector_distance.h"
#include "effect_lifecycle.h"
#include "model_resources.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

void DamageLevelObject(ModelInstance *object, ModelPolygon *polygon, sint32 damage)
{
    ModelBorrow borrow;
    ModelInstance *node;
    uint32 bank, index, flags, health, remaining;
    FUNCTION_MARKER(0x8001E740, "SLUS_003.73");
    if (!object)
        return;
    if (object->flags & 1)
        return;
    node = GetModelSceneObjects();
    if (!node)
        return;
    while (node != object)
    {
        node = node->next;
        if (!node)
            return;
    }
    bank = object->bank;
    index = object->model_idx;
    ModelGeometry &model = GetModelGeometry(bank, index);
    flags = model.flags;
    health = flags >> 24;
    if (flags & 2)
    {
        remaining = health;
        if (health || damage == 65535)
        {
            FlashLevelObject(object, 255, 255, 255, 3);
            remaining = health - (uint32)damage;
            if (damage >= (sint32)health)
            {
                remaining = 0;
                DestroyLevelObject(object);
            }
            flags = model.flags;
        }
        model.flags = (flags & 0xFFFFFF) | (remaining << 24);
    }
    else if (polygon)
        FragmentLevelPolygon(object, polygon, 1, 1, 1, 1, 1);
}



// Native helper models the original signed DIV carriers
static uint32 DivideFlashScalar(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

struct ObjectFlash
{
    uint32 color;
    uint16 step[3];
    uint8 fraction[3];
    uint8 duration;
    ModelInstance *object;
    ObjectFlash *next;
};

static ObjectFlash object_flashes[256];
static ObjectFlash *active_object_flashes;
static ObjectFlash *free_object_flashes;

uint32 InitializeObjectFlashes()
{
    FUNCTION_MARKER(0x8001BAC0, "SLUS_003.73");
    active_object_flashes = 0;
    free_object_flashes = object_flashes;
    for (uint32 i = 0; i < 256; ++i)
    {
        object_flashes[i].next = i + 1 < 256 ? object_flashes + i + 1 : 0;
        object_flashes[i].object = 0;
    }
    return 0;
}

static void ReleaseObjectFlash(ObjectFlash *flash)
{
    FUNCTION_MARKER(0x8001BB14, "SLUS_003.73");
    ObjectFlash *cur = active_object_flashes, *prev = 0;
    while (cur)
    {
        ObjectFlash *next = cur->next;
        if (cur == flash)
        {
            cur->next = free_object_flashes;
            free_object_flashes = cur;
            if (prev)
                prev->next = next;
            else
                active_object_flashes = next;
            return;
        }
        prev = cur;
        cur = next;
    }
}

void ReleaseObjectFlash(uint8 index)
{
    ReleaseObjectFlash(object_flashes + index);
}

void ForgetModelObjectFlashes(const ModelInstance *objects, uint32 count)
{
    ObjectFlash *flash = active_object_flashes;
    while (flash)
    {
        ObjectFlash *next = flash->next;
        for (uint32 i = 0; i < count; ++i)
            if (flash->object == objects + i)
            {
                ReleaseObjectFlash(flash);
                break;
            }
        flash = next;
    }
}

void UpdateObjectFlashes()
{
    FUNCTION_MARKER(0x8001BB68, "SLUS_003.73");
    ObjectFlash *flash = active_object_flashes;
    while (flash)
    {
        ObjectFlash *next = flash->next;
        if (flash->duration)
        {
            uint32 first = (uint16)flash->color;
            uint32 red = (flash->fraction[0] | (first << 8)) + flash->step[0];
            flash->fraction[0] = (uint8)red;
            red = (red >> 8) & 255;
            uint32 green = (flash->fraction[1] | (first & 0xFF00)) + flash->step[1];
            flash->fraction[1] = (uint8)green;
            green &= 0xFF00;
            uint32 blue = (flash->fraction[2] | ((flash->color >> 8) & 0xFF00)) + flash->step[2];
            flash->fraction[2] = (uint8)blue;
            blue = (uint32)(sint32)(sint8)((blue >> 8) & 255);
            flash->color = (blue << 16) | green | red;
            flash->object->color = flash->color;
            --flash->duration;
        }
        else
        {
            flash->object->flags &= 0xFBFF;
            ReleaseObjectFlash(flash);
        }
        flash = next;
    }
}

static ObjectFlash *AcquireObjectFlash(ModelInstance *object)
{
    FUNCTION_MARKER(0x8001BC58, "SLUS_003.73");
    ObjectFlash *flash = 0;
    // Original rejects even an existing flash when the free list is empty
    if (!free_object_flashes)
        return 0;
    if (object->flags & 1024)
    {
        flash = active_object_flashes;
        while (flash && flash->object != object)
            flash = flash->next;
    }
    if (!flash)
    {
        flash = free_object_flashes;
        free_object_flashes = flash->next;
        flash->next = active_object_flashes;
        active_object_flashes = flash;
    }
    return flash;
}

uint32 FlashLevelObject(ModelInstance *object, uint32 red, uint32 green, uint32 blue, uint32 duration)
{
    FUNCTION_MARKER(0x8001BCDC, "SLUS_003.73");
    ObjectFlash *flash = AcquireObjectFlash(object);
    if (!flash)
        return 0;
    red &= 255;
    green &= 255;
    blue &= 255;
    flash->object = object;
    flash->color = (blue << 16) | (green << 8) | red;
    flash->fraction[2] = flash->fraction[1] = flash->fraction[0] = 0;
    flash->duration = (uint8)duration;
    flash->step[0] = (uint16)DivideFlashScalar(32768 - (red << 8), duration);
    flash->step[1] = (uint16)DivideFlashScalar(32768 - (green << 8), duration);
    flash->step[2] = (uint16)DivideFlashScalar(32768 - (blue << 8), duration);
    object->flags |= 1024;
    object->color = flash->color;
    return flash->color;
}

uint32 FadeLevelObjectToBlack(ModelInstance *object, uint32 duration)
{
    FUNCTION_MARKER(0x8001BDC4, "SLUS_003.73");
    ObjectFlash *flash = AcquireObjectFlash(object);
    if (!flash)
        return 32768;
    uint32 step = 0 - DivideFlashScalar(32768, duration);
    flash->object = object;
    flash->color = 0x808080;
    flash->fraction[2] = flash->fraction[1] = flash->fraction[0] = 0;
    flash->duration = (uint8)duration;
    flash->step[0] = flash->step[1] = flash->step[2] = (uint16)step;
    object->flags |= 1024;
    object->color = flash->color;
    return flash->color;
}

uint32 DestroyLevelObject(ModelInstance *object)
{
    ModelBorrow borrow;
    uint32 previous, count, result, bank, index, table, resource, record, type, explosive = 0;
    ModelInstance *node;
    uint32 position[3], child, radius = 512, flags, distance, selected;
    sint32 height;
    FUNCTION_MARKER(0x8001CC1C, "SLUS_003.73");
    previous = r_u32(0x800FF1F0);
    count = previous + 1;
    w_u32(0x800FF1F0, count);
    result = (sint32)count < 10;
    if (!result)
    {
        w_u32(0x800FF1F0, previous);
        return result;
    }
    if (FragmentLevelObject(object, 1, 1))
    {
        bank = FindActiveModelBankSlot(object->bank);
        index = object->model_idx;
        resource = ReadModelKey(GetActiveModelScriptKeys(bank), index);
        record = ActivateResourceScript(resource);
        if (record)
        {
            index = r_u16(record + 10);
            table = r_u32(0x800FF624);
            type = r_u32(table + index * 4);
            explosive = (sint16)r_u16(type) == 9;
        }
        if (!r_u32(0x800FF738))
        {
            position[0] = object->pos[0];
            position[1] = object->pos[1];
            position[2] = object->pos[2];
            if (explosive)
            {
                sint32 upper = (sint16)r_u16(0x800EC51C);
                sint32 lower = (sint16)r_u16(0x800EC51A);
                QueueExplosion(position, 0, (uint32)upper, (uint32)lower, 1);
                selected = r_u32(0x800FF2F0) & 1;
                PlayPositionedSound(selected ? 1 : 2, position, 0);
                child = AllocateEffectStorage(124);
                if (child)
                {
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(ExpandingGlow));
                    new (dram_addr) ExpandingGlow(position, 64, 128, 0, 512, 10);
                }
                SpawnExplosionParticles(position, 20, 10, 750, 80);
                child = AllocateEffectStorage(124);
                if (child)
                {
                    void *dram_addr = AllocateNativeObjectStorage(child, sizeof(RingBurstEffect));
                    new (dram_addr) RingBurstEffect(position, 5, 255, 255, 255, 1, 16, 64, 0, 1, 60, 0, 1, 100, 120, 80, 100, 10, 4);
                }
            }
            else if (!RandomRange(5))
                SpawnExplosion(position, 1);
            else
                SpawnExplosion(position, 0);
        }
        if (explosive)
            radius = 1024;
        node = GetModelSceneObjects();
        while (node)
        {
            if (node != object)
            {
                bank = node->bank;
                index = node->model_idx;
                flags = GetModelGeometry(bank, index).flags;
                if ((flags & 2) && (flags >> 24))
                {
                    selected = 0;
                    distance = MeasurePositionDistance(object->pos, node->pos);
                    if (explosive)
                        selected = distance < radius;
                    else if (distance < radius)
                    {
                        uint32 source_y = object->pos[1];
                        uint32 neighbor_y = node->pos[1];
                        height = (sint32)(source_y - neighbor_y);
                        if (height > 409600 && height <= 1638399)
                            selected = 1;
                    }
                    if (selected)
                        DestroyLevelObject(node);
                }
            }
            node = node->next;
        }
    }
    result = r_u32(0x800FF1F0) - 1;
    w_u32(0x800FF1F0, result);
    return result;
}





// Native virtual damage dispatch through the current object identity
uint32 ApplyObjectDamage(uint32 object, uint32 damage, const sint32 *vector, uint32 reason)
{
    return VisualObject::Attach(object).Damage(damage, vector, reason);
}



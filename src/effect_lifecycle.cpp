extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
uint32 apocalypse_object_cleanup(uint32 target, uint32 receiver, uint32 reason);
}
#include "effect_lifecycle.h"
#include "model_resources.h"
#include "game_heap.h"

static const uint32 effect_list_heads[13] =
{
    0x800FF434, 0x800FF438, 0x800FF43C, 0x800FF440,
    0x800FF45C, 0x800FF460, 0x800FF444, 0x800FF448,
    0x800FF44C, 0x800FF450, 0x800FF454, 0x800FF464,
    0x800FF458
};

uint32 InitializeEffectResources()
{
    uint32 index, name, resource;
    FUNCTION_MARKER(0x80032A0C, "SLUS_003.73");
    w_u32(0x800FF3A8, 0);
    for (index = 0; index < 13; ++index)
        w_u32(effect_list_heads[index], 0);
    // Preserve the existing native GetTPage and SetDrawTPage packet adaptation
    w_u8(0x800FF46B, 1);
    w_u32(0x800FF46C, 0xE1000620);
    for (index = 0; index < 15; ++index)
    {
        name = r_u32(0x800A67CC + index * 4);
        resource = FindModelTextureGroup(name);
        w_u32(0x800A6808 + index * 4, resource);
    }
    resource = FindModelTextureGroup(0x800FF40C);
    w_u32(0x800FF01C, resource);
    resource = FindModelResourceEntry(0x2A3B2550);
    w_u32(0x800FF408, resource);
    return resource;
}

void DestroyTransientEffectList(uint32 object)
{
    uint32 next, table, target, receiver;
    sint32 adjustment;
    FUNCTION_MARKER(0x80032AF4, "SLUS_003.73");
    while (object)
    {
        next = r_u32(object + 4);
        if (!r_u8(object + 66))
        {
            table = r_u32(object + 68);
            adjustment = (sint16)r_u16(table + 8);
            target = r_u32(table + 12);
            receiver = object + (uint32)adjustment;
            apocalypse_object_cleanup(target, receiver, 3);
        }
        object = next;
    }
}

void DestroyTransientEffects()
{
    uint32 index, head;
    FUNCTION_MARKER(0x80032B64, "SLUS_003.73");
    for (index = 0; index < 13; ++index)
    {
        head = r_u32(effect_list_heads[index]);
        DestroyTransientEffectList(head);
    }
}

void DestroyFinishedEffectList(uint32 object)
{
    uint32 next, table, target, receiver;
    sint32 adjustment;
    FUNCTION_MARKER(0x80032C18, "SLUS_003.73");
    while (object)
    {
        next = r_u32(object + 4);
        if (r_u8(object + 63))
        {
            table = r_u32(object + 68);
            adjustment = (sint16)r_u16(table + 8);
            target = r_u32(table + 12);
            receiver = object + (uint32)adjustment;
            apocalypse_object_cleanup(target, receiver, 3);
        }
        object = next;
    }
}

void DestroyFinishedEffects()
{
    uint32 index, head;
    FUNCTION_MARKER(0x80032C88, "SLUS_003.73");
    for (index = 0; index < 13; ++index)
    {
        head = r_u32(effect_list_heads[index]);
        DestroyFinishedEffectList(head);
    }
}

uint32 AllocateEffectStorage(uint32 size)
{
    uint32 allocation, words, index, destination;
    sint32 heap;
    FUNCTION_MARKER(0x80032DC0, "SLUS_003.73");
    heap = r_u32(0x800FF3AC) ? -1 : 0;
    allocation = AllocateHeapBlock(size, heap, 1);
    words = (size + 3) >> 2;
    destination = allocation;
    for (index = 0; index < words; ++index)
    {
        w_u32(destination, 0);
        destination += 4;
    }
    return allocation;
}

uint32 ReleaseEffectStorage(uint32 object)
{
    FUNCTION_MARKER(0x80032E30, "SLUS_003.73");
    return ReleaseHeapBlock(object);
}

extern "C" uint32 sub_80032A0C(void)
{
    FUNCTION_MARKER(0x80032A0C, "SLUS_003.73");
    return InitializeEffectResources();
}
extern "C" void sub_80032AF4(uint32 object)
{
    FUNCTION_MARKER(0x80032AF4, "SLUS_003.73");
    DestroyTransientEffectList(object);
}
extern "C" void sub_80032B64(void)
{
    FUNCTION_MARKER(0x80032B64, "SLUS_003.73");
    DestroyTransientEffects();
}
extern "C" void sub_80032C18(uint32 object)
{
    FUNCTION_MARKER(0x80032C18, "SLUS_003.73");
    DestroyFinishedEffectList(object);
}
extern "C" void sub_80032C88(void)
{
    FUNCTION_MARKER(0x80032C88, "SLUS_003.73");
    DestroyFinishedEffects();
}

extern "C" uint32 sub_80032DC0(uint32 size)
{
    FUNCTION_MARKER(0x80032DC0, "SLUS_003.73");
    return AllocateEffectStorage(size);
}
extern "C" uint32 sub_80032E30(uint32 object)
{
    FUNCTION_MARKER(0x80032E30, "SLUS_003.73");
    return ReleaseEffectStorage(object);
}

void UpdateEffectList(uint32 head)
{
    FUNCTION_MARKER(0x80037258, "SLUS_003.73");
    uint32 node = head;
    while (node)
    {
        if (!r_u8(node + 63))
        {
            uint32 table = r_u32(node + 68);
            sint32 adjustment = (sint16)r_u16(table + 16);
            uint32 target = r_u32(table + 20);
            apocalypse_object_virtual20(target, node + (uint32)adjustment);
        }
        node = r_u32(node + 4);
    }
}

void UpdateEffects(void)
{
    FUNCTION_MARKER(0x800372B8, "SLUS_003.73");
    const uint32 heads[13] =
    {
        0x800FF440, 0x800FF438, 0x800FF454, 0x800FF448,
        0x800FF44C, 0x800FF444, 0x800FF450, 0x800FF434,
        0x800FF43C, 0x800FF45C, 0x800FF460, 0x800FF464,
        0x800FF458
    };
    for (uint32 index = 0; index < 13; ++index)
        UpdateEffectList(r_u32(heads[index]));
}

extern "C" void sub_80037258(uint32 head)
{
    FUNCTION_MARKER(0x80037258, "SLUS_003.73");
    UpdateEffectList(head);
}

extern "C" void sub_800372B8(void)
{
    FUNCTION_MARKER(0x800372B8, "SLUS_003.73");
    UpdateEffects();
}


void DestroyEffectsByIdentifier(uint32 identifier)
{
    uint32 object = r_u32(0x800FF434), next, kind, table, receiver, target;
    FUNCTION_MARKER(0x8001F718, "SLUS_003.73");
    while (object)
    {
        kind = r_u8(object + 67);
        next = r_u32(object + 4);
        if (kind == 7 && r_u32(object + 84) == identifier)
        {
            table = r_u32(object + 68);
            receiver = object + (uint32)(sint32)(sint16)r_u16(table + 8);
            target = r_u32(table + 12);
            apocalypse_object_cleanup(target, receiver, 3);
        }
        object = next;
    }
}

extern "C" void sub_8001F718(uint32 identifier)
{
    FUNCTION_MARKER(0x8001F718, "SLUS_003.73");
    DestroyEffectsByIdentifier(identifier);
}

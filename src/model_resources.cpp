#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include <stdlib.h>
#include <stdio.h>
#include "game_vram_startup.h"
}
#include "model_resources.h"
#include "game_string.h"
#include "native_object_storage.h"
#include "resource_io.h"
#include "game_heap.h"
#include "palette_resources.h"
#include <string.h>

uint32 InitializeModelResources()
{
    uint32 address = 0x800EAEE8;
    FUNCTION_MARKER(0x8006DFA0, "SLUS_003.73");
    for (uint32 item = 0; item < 20; ++item, address -= 16)
        w_u8(address + 13, 0);
    w_u32(0x800FF79C, 0);
    w_u32(0x800FF7A0, 0);
    address = 0x800EAEF8;
    for (uint32 item = 0; item < 40; ++item, address += 64)
    {
        w_u8(address, 0);
        w_u8(address + 11, 0);
        w_u8(address + 10, 0);
        w_u32(address + 16, 0);
        w_u32(address + 20, 0);
        w_u32(address + 24, 0);
        w_u32(address + 40, 0);
        w_u32(address + 36, 0);
        w_u32(address + 28, 0);
    }
    address = 0x800EC0F4;
    for (uint32 item = 0; item < 512; ++item, address -= 4)
        w_u32(address, 0);
    w_u32(0x800FF7A4, 0);
    w_u32(0x800FF778, 0xFFFFFFFF);
    w_u32(0x800FF77C, 0xFFFFFFFF);
    w_u32(0x800FF780, 0);
    w_u32(0x800FF784, 0);
    w_u32(0x800FF794, 0);
    w_u32(0x800FF798, 0);
    return 0xFFFFFFFF;
}

uint32 FindModelResourceIndex(uint32 key, uint32 bank)
{
    uint32 entry, header, count, keys;
    FUNCTION_MARKER(0x8006E080, "SLUS_003.73");
    entry = 0x800EAEF8 + (bank << 6);
    header = r_u32(entry + 16);
    count = r_u32(header - 4);
    keys = r_u32(entry + 12);
    // Original returns the header when empty and zero when absent
    if (!count)
        return header;
    for (uint32 index = 0; index < count; ++index, keys += 4)
        if (r_u32(keys) == key)
            return index;
    return 0;
}

uint32 FindModelObject(uint32 key)
{
    uint32 object, bank, previous_bank = 0xFFFFFFFF, keys = 0, index;
    FUNCTION_MARKER(0x8006E0D0, "SLUS_003.73");
    object = r_u32(0x800FF794);
    while (object)
    {
        bank = r_u8(object + 27);
        if (bank != previous_bank)
        {
            previous_bank = bank;
            keys = r_u32(0x800EAEF8 + (bank << 6) + 12);
        }
        index = r_u16(object + 22);
        if (r_u32(keys + index * 4) == key)
            return object;
        object = r_u32(object + 28);
    }
    return 0;
}

uint32 AllocateModelResourceEntry(uint32 key)
{
    uint32 entry, bucket, previous;
    FUNCTION_MARKER(0x8006E178, "SLUS_003.73");
    entry = sub_8006B864(40, 0, 1);
    bucket = 0x800EB8F8 + (key & 511) * 4;
    previous = r_u32(bucket);
    // TODO Establish allocator success invariant before adding a failure path
    w_u32(entry + 36, 0);
    w_u32(entry + 32, previous);
    w_u32(bucket, entry);
    previous = r_u32(entry + 32);
    if (previous)
        w_u32(previous + 36, entry);
    w_u32(entry + 20, key);
    w_u8(entry + 18, 0);
    w_u16(entry + 16, 0);
    return entry;
}

// TODO Establish receiver ownership before making this an instance method
uint32 ReleaseModelResourceEntry(uint32 entry)
{
    uint32 next, previous, bucket;
    FUNCTION_MARKER(0x8006E1EC, "SLUS_003.73");
    next = r_u32(entry + 32);
    if (next)
        w_u32(next + 36, r_u32(entry + 36));
    previous = r_u32(entry + 36);
    if (previous)
        w_u32(previous + 32, r_u32(entry + 32));
    bucket = 0x800EB8F8 + (r_u32(entry + 20) & 511) * 4;
    if (r_u32(bucket) == entry)
        w_u32(bucket, r_u32(entry + 32));
    return sub_8006BC20(entry);
}

extern "C" uint32 sub_8006DFA0(void)
{
    FUNCTION_MARKER(0x8006DFA0, "SLUS_003.73");
    return InitializeModelResources();
}
extern "C" uint32 sub_8006E080(uint32 key, uint32 bank)
{
    FUNCTION_MARKER(0x8006E080, "SLUS_003.73");
    return FindModelResourceIndex(key, bank);
}
extern "C" uint32 sub_8006E0D0(uint32 key)
{
    FUNCTION_MARKER(0x8006E0D0, "SLUS_003.73");
    return FindModelObject(key);
}
extern "C" uint32 sub_8006E178(uint32 key)
{
    FUNCTION_MARKER(0x8006E178, "SLUS_003.73");
    return AllocateModelResourceEntry(key);
}
extern "C" uint32 sub_8006E1EC(uint32 entry)
{
    FUNCTION_MARKER(0x8006E1EC, "SLUS_003.73");
    return ReleaseModelResourceEntry(entry);
}

uint32 FindModelResourceEntry(uint32 key)
{
    uint32 entry;
    FUNCTION_MARKER(0x8006E278, "SLUS_003.73");
    entry = r_u32(0x800EB8F8 + (key & 511) * 4);
    while (entry)
    {
        if (r_u32(entry + 20) == key)
            return entry;
        entry = r_u32(entry + 32);
    }
    return 0;
}

uint32 BeginModelResourceTraversal()
{
    uint32 entry;
    FUNCTION_MARKER(0x8006E2C4, "SLUS_003.73");
    entry = r_u32(0x800EB8F8);
    w_u32(0x800FFC60, 0);
    w_u32(0x800FFC64, entry);
    return entry;
}

uint32 NextModelResourceEntry()
{
    uint32 index, entry;
    FUNCTION_MARKER(0x8006E2DC, "SLUS_003.73");
    if ((sint32)r_u32(0x800FFC60) >= 512)
        return 0;
    entry = r_u32(0x800FFC64);
    while (!entry)
    {
        index = r_u32(0x800FFC60) + 1;
        // Original reads bucket 512 before terminating traversal
        entry = r_u32(0x800EB8F8 + index * 4);
        w_u32(0x800FFC60, index);
        w_u32(0x800FFC64, entry);
        if ((sint32)index >= 512)
            return 0;
    }
    w_u32(0x800FFC64, r_u32(entry + 32));
    return entry;
}

uint32 FindModelResourceTrailer(uint32 data)
{
    uint32 offset, cursor;
    FUNCTION_MARKER(0x8006E360, "SLUS_003.73");
    offset = r_u32(data + 4);
    for (;;)
    {
        cursor = data + offset;
        if (r_u32(cursor) == 0xFFFFFFFF)
            return cursor + 4;
        offset = r_u32(cursor + 4);
        data = cursor + 8;
    }
}

uint32 DecrementModelResourceReferences(uint32 bank)
{
    uint32 entry, header, data, count, cursor, references, resource;
    FUNCTION_MARKER(0x8006E394, "SLUS_003.73");
    entry = 0x800EAEF8 + (bank << 6);
    header = r_u32(entry + 16);
    data = r_u32(entry + 20);
    count = r_u32(header - 4);
    cursor = FindModelResourceTrailer(data) + (count << 2) + 4;
    references = r_u32(cursor - 4);
    if (!references)
        return cursor;
    for (uint32 index = 0; index < references; ++index, cursor += 4)
    {
        resource = r_u32(cursor);
        w_u16(resource + 16, r_u16(resource + 16) - 1);
    }
    return 0;
}

uint32 ReleaseUnusedModelResources()
{
    uint32 entry, owner, first, second;
    FUNCTION_MARKER(0x8006E414, "SLUS_003.73");
    BeginModelResourceTraversal();
    for (;;)
    {
        entry = NextModelResourceEntry();
        if (!entry)
            return 0;
        if (!r_u8(entry + 18))
            continue;
        if (r_u16(entry + 16))
            continue;
        owner = r_u32(entry + 24);
        w_u16(owner + 4, r_u16(owner + 4) - 1);
        first = (uint32)(sint32)(sint16)r_u16(entry + 28);
        second = (uint32)(sint32)(sint16)r_u16(entry + 30);
        ReleaseUsedVramRectangle(first, second);
        ReleaseModelResourceEntry(entry);
    }
}

extern "C" uint32 sub_8006E278(uint32 key)
{
    FUNCTION_MARKER(0x8006E278, "SLUS_003.73");
    return FindModelResourceEntry(key);
}
extern "C" uint32 sub_8006E2C4(void)
{
    FUNCTION_MARKER(0x8006E2C4, "SLUS_003.73");
    return BeginModelResourceTraversal();
}
extern "C" uint32 sub_8006E2DC(void)
{
    FUNCTION_MARKER(0x8006E2DC, "SLUS_003.73");
    return NextModelResourceEntry();
}
extern "C" uint32 sub_8006E360(uint32 data)
{
    FUNCTION_MARKER(0x8006E360, "SLUS_003.73");
    return FindModelResourceTrailer(data);
}
extern "C" uint32 sub_8006E394(uint32 bank)
{
    FUNCTION_MARKER(0x8006E394, "SLUS_003.73");
    return DecrementModelResourceReferences(bank);
}
extern "C" uint32 sub_8006E414(void)
{
    FUNCTION_MARKER(0x8006E414, "SLUS_003.73");
    return ReleaseUnusedModelResources();
}

uint32 LoadModelResourceData(uint32 bank)
{
    uint32 entry, data, models, model_count, keys, references, reference_count;
    uint32 cursor, resource, model, vertices, normals, primitive_count, primitive;
    uint32 retained_size, count, texture_count, textures, texture_data, flags, format, palette_key;
    uint32 width, height, pixels, key, rounded_width, depth, palette, x, y, u, right, bottom, mask;
    uint32 texture_xy[2];
    sint16 upload_rectangle[4];
    uint32 block_kind, block_size, block_data, trim = 0;
    FUNCTION_MARKER(0x8006E4A0, "SLUS_003.73");
    entry = 0x800EAEF8 + (bank << 6);
    data = r_u32(entry + 20);
    models = data + r_u32(data + 8) * 36 + 16;
    model_count = r_u32(models - 4);
    w_u32(0x800FF788, 0);
    cursor = models;
    for (uint32 index = 0; index < model_count; ++index, cursor += 4)
        w_u32(cursor, r_u32(cursor) + data);
    w_u32(entry + 16, models);
    keys = FindModelResourceTrailer(data);
    references = keys + (model_count << 2) + 4;
    w_u32(entry + 12, keys);
    cursor = references;
    reference_count = r_u32(references - 4);
    uint32 reference_index = 0;
    for (;;)
    {
        if (reference_index >= reference_count)
            break;
        resource = FindModelResourceEntry(r_u32(cursor));
        if (!resource)
            resource = AllocateModelResourceEntry(r_u32(cursor));
        ++reference_index;
        w_u16(resource + 16, r_u16(resource + 16) + 1);
        w_u32(cursor, resource);
        cursor += 4;
    }
    cursor = models;
    for (uint32 index = 0; index < model_count; ++index, cursor += 4)
    {
        model = r_u32(cursor);
        vertices = r_u32(model + 4);
        normals = r_u32(model + 8);
        primitive_count = r_u32(model + 12);
        primitive = model + (vertices << 3) + 32 + (normals << 3);
        for (uint32 item = 0; item < primitive_count; ++item)
        {
            if (r_u32(primitive) & 1)
                w_u32(primitive + 20, r_u32(references + (r_u32(primitive + 20) << 2)));
            primitive += r_u16(primitive + 2) & 0xFFFC;
        }
    }
    ReleaseUnusedModelResources();
    ReleaseUnusedPaletteResources();
    retained_size = FindModelResourceTrailer(data) - data + (model_count << 2) + (reference_count << 2) + 4;
    cursor = data + retained_size;
    count = r_u32(cursor);
    cursor += 4;
    for (uint32 index = 0; index < count; ++index, cursor += 36)
        if (!FindPaletteResource(r_u32(cursor)))
            LoadPaletteResource(r_u32(cursor), cursor + 4, 1);
    count = r_u32(cursor);
    cursor += 4;
    for (uint32 index = 0; index < count; ++index, cursor += 516)
        if (!FindPaletteResource(r_u32(cursor)))
            LoadPaletteResource(r_u32(cursor), cursor + 4, 2);
    w_u32(0x800FF674, 0);
    if (r_u8(entry) == 98 && r_u8(entry + 1) == 105 && r_u8(entry + 2) == 116 && r_u8(entry + 3) == 115 && !r_u8(entry + 4))
        w_u32(0x800FF674, 1);
    texture_count = r_u32(cursor);
    textures = cursor + 4;
    for (uint32 index = 0; index < texture_count; ++index, textures += 4)
    {
        texture_data = data + r_u32(textures);
        resource = r_u32(texture_data + 12);
        flags = r_u32(texture_data);
        format = r_u32(texture_data + 4);
        palette_key = r_u32(texture_data + 8);
        resource = r_u32(references + (resource << 2));
        if (!resource || r_u8(resource + 18))
            continue;
        key = r_u32(resource + 20);
        if (key == 0xCCE0187C || key == 0x68B7B136)
            flags |= 1;
        width = r_u16(texture_data + 16);
        height = r_u16(texture_data + 18);
        pixels = texture_data + 20;
        if (format == 256 || format == 16)
        {
            rounded_width = format == 256 ? (width + 1) & 0xFFFFFFFE : (width + 3) & 0xFFFFFFFC;
            depth = format == 256 ? 8 : 4;
            AllocateModelTextureRectangle(0, rounded_width, height, texture_xy, &texture_xy[1], depth, flags & 1 ? 2 : 0, r_u32(resource + 20));
            x = texture_xy[0] & 0xFFFF;
            y = texture_xy[1] & 0xFFFF;
            w_u16(resource + 28, x);
            w_u16(resource + 30, y);
            upload_rectangle[0] = (sint16)x;
            upload_rectangle[1] = (sint16)y;
            upload_rectangle[2] = (sint16)(format == 256 ? rounded_width >> 1 : rounded_width >> 2);
            upload_rectangle[3] = (sint16)height;
            xport_draft_host_sub_800885A4_p1(upload_rectangle, pixels);
        }
        palette = FindPaletteResource(palette_key);
        if (format != 256 && format != 16)
        {
            // TODO Original coordinates are undefined for unsupported texture formats
            GameDiagnostic("Unsupported model texture format %u at %08X\n", format, texture_data);
            abort();
        }
        x = texture_xy[0];
        y = texture_xy[1];
        w_u16(palette + 4, r_u16(palette + 4) + 1);
        w_u32(resource + 24, palette);
        w_u16(resource + 2, r_u16(palette));
        w_u16(resource + 6, GetTPage(format == 256, 0, x, y));
        u = texture_xy[0] & 63;
        u <<= format == 256 ? 1 : 2;
        w_u8(resource, u);
        right = width + u - 1;
        y = texture_xy[1] & 255;
        w_u8(resource + 18, 1);
        w_u8(resource + 4, right);
        w_u8(resource + 10, right);
        w_u8(resource + 1, y);
        bottom = height + y - 1;
        x = r_u8(resource);
        w_u8(resource + 9, bottom);
        u = r_u8(resource);
        w_u8(resource + 5, y);
        w_u8(resource + 8, x);
        mask = ((y & 248) << 12) | ((u & 248) << 7) | 0xE2000000;
        mask |= ((0 - height) & 248) << 2;
        mask |= ((0 - width) & 248) >> 3;
        bottom = r_u8(resource + 9);
        w_u32(resource + 12, mask);
        w_u8(resource + 11, bottom);
    }
    w_u32(0x800FF674, 0);
    ResizeHeapBlock(data, retained_size);
    PrepareModelBankGeometry(bank);
    w_u8(entry + 9, 0);
    w_u32(entry + 36, 0);
    w_u32(entry + 40, 0);
    cursor = data + r_u32(data + 4);
    for (;;)
    {
        block_kind = r_u32(cursor);
        if (block_kind == 0xFFFFFFFF)
            break;
        block_size = r_u32(cursor + 4);
        block_data = cursor + 8;
        switch (block_kind)
        {
            case 6:
                w_u32(entry + 36, block_data);
                w_u32(0x800FF788, block_data - 8);
                break;
            case 7:
                w_u32(entry + 40, block_data);
                break;
            case 10:
                resource = FindActiveModelBankSlot(bank);
                InitializeModelBankGrid(resource, block_data);
                break;
            case 42:
                w_u32(entry + 24, block_data);
                w_u8(entry + 9, 1);
                break;
            case 69:
                RegisterModelTextureBlock(data, block_data);
                trim = 1;
                break;
            case 0x73424752:
                w_u32(entry + 32, block_data);
                break;
            default:
                break;
        }
        cursor = block_data + block_size;
    }
    cursor += 4;
    if (trim)
        ResizeHeapBlock(data, cursor - data);
    w_u8(0x800EAEF8 + (bank << 6) + 10, 1);
    return 1;
}

extern "C" uint32 sub_8006E4A0(uint32 bank)
{
    FUNCTION_MARKER(0x8006E4A0, "SLUS_003.73");
    return LoadModelResourceData(bank);
}

uint32 AttachModelBankObjects(uint32 bank)
{
    uint32 head, node, tail, old, result;
    FUNCTION_MARKER(0x8006EBF4, "SLUS_003.73");
    old = r_u32(0x800FF794);
    head = r_u32(0x800EAEF8 + (bank << 6) + 20) + 12;
    if (old)
    {
        tail = head;
        if (r_u32(head + 28))
        {
            do
            {
                tail = r_u32(tail + 28);
            } while (r_u32(tail + 28));
        }
        old = r_u32(0x800FF794);
        w_u32(0x800FF794, head);
        w_u32(tail + 28, old);
    }
    else
        w_u32(0x800FF794, head);
    node = FindActiveModelBankSlot(bank);
    result = 0x800FF78C + (node << 2);
    w_u32(result, r_u32(0x800FF788));
    return result;
}

uint32 DetachModelBankObjects(uint32 slot)
{
    uint32 bank, target, other_bank, remaining = 0, result = 0xFFFFFFFF;
    FUNCTION_MARKER(0x8006ECAC, "SLUS_003.73");
    slot &= 255;
    w_u32(0x800FF78C + slot * 4, 0);
    bank = r_u32(0x800FF778 + slot * 4);
    target = r_u32(0x800EAEF8 + (bank << 6) + 20);
    other_bank = r_u32(0x800FF778 + ((slot ^ 1) & 255) * 4);
    target += 12;
    if (other_bank != 0xFFFFFFFF)
    {
        result = r_u32(0x800EAEF8 + (other_bank << 6) + 20);
        remaining = result + 12;
    }
    if (!remaining)
    {
        w_u32(0x800FF794, 0);
        return result;
    }
    result = r_u32(0x800FF794);
    if (target == result)
    {
        w_u32(0x800FF794, remaining);
        return result;
    }
    // TODO Establish original list-membership invariant before adding a failure path
    for (;;)
    {
        result = r_u32(remaining + 28);
        if (result == target)
            break;
        remaining = r_u32(remaining + 28);
    }
    w_u32(remaining + 28, 0);
    return result;
}

uint32 FindModelBankByName(const char *name)
{
    uint32 entry = 0x800EAEF8;
    FUNCTION_MARKER(0x8006ED70, "SLUS_003.73");
    for (uint32 bank = 0; bank < 40; ++bank, entry += 64)
        if (NamesEqualIgnoringCase(name, (const char *)psx_addr(entry, 1)))
            return bank;
    return 0xFFFFFFFF;
}

uint32 FindModelBankByName(uint32 name)
{
    return FindModelBankByName(name ? (const char *)psx_addr(name, 1) : 0);
}

void UnloadModelBankByName(uint32 name)
{
    uint32 bank;
    FUNCTION_MARKER(0x8006EDD4, "SLUS_003.73");
    bank = FindModelBankByName(name);
    // TODO Original probes the preceding entry when the name is absent
    if (r_u8(0x800EAEF8 + (bank << 6) + 10))
    {
        bank = FindModelBankByName(name);
        UnloadModelBank(bank, 1);
    }
}

uint32 UnloadUnprotectedModelBanks()
{
    uint32 entry = 0x800EAEF8;
    FUNCTION_MARKER(0x8006EE2C, "SLUS_003.73");
    WaitForModelBankLoads();
    UnloadActiveModelBanks();
    for (uint32 bank = 0; bank < 40; ++bank, entry += 64)
        if (!r_u8(entry + 11))
            UnloadModelBank(bank, 1);
    return ReleaseUnusedModelResources();
}

void UnloadModelBank(uint32 bank, uint32 release_unused)
{
    uint32 entry, slot;
    FUNCTION_MARKER(0x8006EE94, "SLUS_003.73");
    if (bank == 0xFFFFFFFF)
        return;
    entry = 0x800EAEF8 + (bank << 6);
    if (!r_u8(entry))
        return;
    DecrementModelResourceReferences(bank);
    w_u32(entry + 24, 0);
    w_u32(entry + 40, 0);
    w_u32(entry + 36, 0);
    w_u8(entry + 11, 0);
    w_u8(entry + 10, 0);
    w_u32(entry + 28, 0);
    w_u32(entry + 16, 0);
    w_u8(entry, 0);
    slot = FindActiveModelBankSlot(bank);
    if (slot != 0xFFFFFFFF)
    {
        ClearModelBankGrid((sint32)slot);
        DetachModelBankObjects(slot & 255);
        w_u32(0x800FF778 + slot * 4, 0xFFFFFFFF);
    }
    sub_8006BC20(r_u32(entry + 20));
    w_u32(entry + 20, 0);
    if (release_unused)
        ReleaseUnusedModelResources();
}

extern "C" uint32 sub_8006EBF4(uint32 bank)
{
    FUNCTION_MARKER(0x8006EBF4, "SLUS_003.73");
    return AttachModelBankObjects(bank);
}
extern "C" uint32 sub_8006ECAC(uint32 slot)
{
    FUNCTION_MARKER(0x8006ECAC, "SLUS_003.73");
    return DetachModelBankObjects(slot);
}
extern "C" uint32 sub_8006ED70(uint32 name)
{
    FUNCTION_MARKER(0x8006ED70, "SLUS_003.73");
    return FindModelBankByName(name);
}
extern "C" void sub_8006EDD4(uint32 name)
{
    FUNCTION_MARKER(0x8006EDD4, "SLUS_003.73");
    UnloadModelBankByName(name);
}
extern "C" uint32 sub_8006EE2C(void)
{
    FUNCTION_MARKER(0x8006EE2C, "SLUS_003.73");
    return UnloadUnprotectedModelBanks();
}
extern "C" void sub_8006EE94(uint32 bank, uint32 release_unused)
{
    FUNCTION_MARKER(0x8006EE94, "SLUS_003.73");
    UnloadModelBank(bank, release_unused);
}

uint32 UnloadInactiveModelBanks()
{
    uint32 entry = 0x800EAEF8;
    FUNCTION_MARKER(0x8006EF74, "SLUS_003.73");
    for (uint32 bank = 0; bank < 40; ++bank, entry += 64)
        if (r_u32(entry + 20) && bank != r_u32(0x800FF778) && bank != r_u32(0x800FF77C) && !r_u8(entry + 11))
            UnloadModelBank(bank, 0);
    return ReleaseUnusedModelResources();
}

uint32 UnloadActiveModelBanks()
{
    uint32 result;
    FUNCTION_MARKER(0x8006F004, "SLUS_003.73");
    UnloadModelBank(r_u32(0x800FF778), 0);
    UnloadModelBank(r_u32(0x800FF77C), 0);
    w_u32(0x800FF778, 0xFFFFFFFF);
    w_u32(0x800FF77C, 0xFFFFFFFF);
    result = ReleaseUnusedModelResources();
    w_u32(0x800FF798, 0);
    return result;
}

uint32 RegisterModelTextureBlock(uint32 data, uint32 block)
{
    uint32 textures, node, previous, count, cursor, items, item_count, resource;
    FUNCTION_MARKER(0x8006F048, "SLUS_003.73");
    textures = FindModelResourceTrailer(data) + 4;
    node = AllocateHeapBlock(8, 0, 1);
    previous = r_u32(0x800FF7A4);
    // TODO Establish allocator success invariant before adding a failure path
    w_u32(node, block);
    w_u32(0x800FF7A4, node);
    w_u32(node + 4, previous);
    count = r_u32(block);
    cursor = block + 4;
    for (uint32 group = 0; group < count; ++group)
    {
        cursor += 8;
        item_count = r_u32(cursor);
        cursor += 4;
        items = cursor + 4;
        for (uint32 item = 0; item < item_count; ++item, items += 8)
        {
            resource = r_u32(textures + (r_u32(items) << 2));
            w_u32(items, resource);
            w_u16(resource + 6, r_u16(resource + 6) | 32);
            resource = r_u32(items);
            w_u8(resource + 4, r_u8(resource + 4) + 1);
            resource = r_u32(items);
            w_u8(resource + 9, r_u8(resource + 9) + 1);
            resource = r_u32(items);
            cursor += 8;
            w_u8(resource + 10, r_u8(resource + 10) + 1);
            resource = r_u32(items);
            w_u8(resource + 11, r_u8(resource + 11) + 1);
        }
    }
    return 0;
}

uint32 FindActiveModelBankSlot(uint32 bank)
{
    FUNCTION_MARKER(0x8006F4D0, "SLUS_003.73");
    if (bank == r_u32(0x800FF778))
        return 0;
    if (bank == r_u32(0x800FF77C))
        return 1;
    return 0xFFFFFFFF;
}

extern "C" uint32 sub_8006EF74(void)
{
    FUNCTION_MARKER(0x8006EF74, "SLUS_003.73");
    return UnloadInactiveModelBanks();
}
extern "C" uint32 sub_8006F004(void)
{
    FUNCTION_MARKER(0x8006F004, "SLUS_003.73");
    return UnloadActiveModelBanks();
}
extern "C" uint32 sub_8006F048(uint32 data, uint32 block)
{
    FUNCTION_MARKER(0x8006F048, "SLUS_003.73");
    return RegisterModelTextureBlock(data, block);
}
extern "C" uint32 sub_8006F4D0(uint32 bank)
{
    FUNCTION_MARKER(0x8006F4D0, "SLUS_003.73");
    return FindActiveModelBankSlot(bank);
}

uint32 FindModelTextureGroup(const char *name)
{
    uint32 name_address = GuestMemoryAddress(name);
    uint32 node, block, group, count, length, left, right;
    FUNCTION_MARKER(0x8006F164, "SLUS_003.73");
    node = r_u32(0x800FF7A4);
    while (node)
    {
        block = r_u32(node);
        group = block + 4;
        count = r_u32(block);
        for (uint32 index = 0; index < count; ++index)
        {
            length = 0;
            for (;;)
            {
                left = (name_address ? r_u8(name_address + length) : (uint8)name[length]) & 0xDF;
                right = r_u8(group + length) & 0xDF;
                if (left != right || !left || length >= 8)
                    break;
                ++length;
            }
            // Original accepts a mismatch at the ninth character
            if ((!left && !right) || length == 8)
                return group + 12;
            group += (r_u32(group + 8) << 3) + 12;
        }
        node = r_u32(node + 4);
    }
    return 0;
}

uint32 FindModelTextureGroup(uint32 name)
{
    return FindModelTextureGroup((const char *)psx_addr(name, 1));
}

uint32 WriteModelLoadFilename(uint32 name, uint32 suffix)
{
    uint32 index, cursor, character;
    FUNCTION_MARKER(0x8006F238, "SLUS_003.73");
    index = r_u32(0x800FF7A0);
    character = r_u8(name);
    cursor = 0x800EADB8 + (index << 4);
    // TODO Original queue filename copy has no length bound
    while (character)
    {
        ++name;
        w_u8(cursor, character);
        character = r_u8(name);
        ++cursor;
    }
    character = r_u8(suffix);
    while (character)
    {
        ++suffix;
        w_u8(cursor, character);
        character = r_u8(suffix);
        ++cursor;
    }
    w_u8(cursor, 0);
    return character;
}

uint32 QueueModelBankLoad(uint32 name, uint32 active)
{
    uint32 entry = 0x800EAEF8, queue, source, target, left, right, index, bank = 0xFFFFFFFF;
    bool matches;
    FUNCTION_MARKER(0x8006F29C, "SLUS_003.73");
    for (index = 0; index < 40; ++index, entry += 64)
        if (r_u8(entry) && sub_80067724(name, entry))
            return index;
    for (queue = 0x800EADB8; queue < 0x800EAEF8; queue += 16)
    {
        if (!r_u8(queue + 13))
            continue;
        source = name;
        target = queue;
        matches = true;
        if (r_u8(name))
        {
            for (;;)
            {
                left = r_u8(source);
                right = r_u8(target);
                ++source;
                if (left != right)
                {
                    matches = false;
                    break;
                }
                left = r_u8(source);
                ++target;
                if (!left)
                    break;
            }
        }
        if (matches && r_u8(target) == 46)
            return r_u8(queue + 15);
    }
    entry = 0x800EAEF8;
    for (index = 0; index < 40; ++index, entry += 64)
        if (!r_u8(entry))
        {
            bank = index;
            break;
        }
    if (!active)
    {
        // TODO Original writes the preceding entry if all banks are occupied
        target = 0x800EAEF8 + (bank << 6);
        source = name;
        if (r_u8(name))
        {
            for (index = 0; index < 9; ++index)
            {
                left = r_u8(source);
                ++source;
                w_u8(target, left);
                left = r_u8(source);
                ++target;
                if (!left)
                    break;
            }
        }
        w_u8(target, 0);
    }
    WriteModelLoadFilename(name, 0x800FF7A8);
    queue = 0x800EADB8 + (r_u32(0x800FF7A0) << 4);
    w_u8(queue + 14, active != 0);
    queue = 0x800EADB8 + (r_u32(0x800FF7A0) << 4);
    w_u8(queue + 15, bank);
    queue = 0x800EADB8 + (r_u32(0x800FF7A0) << 4);
    w_u8(queue + 13, 1);
    index = r_u32(0x800FF7A0) + 1;
    w_u32(0x800FF7A0, (uint32)((sint32)index % 20));
    return bank;
}

uint32 ProcessModelBankLoadQueue()
{
    uint32 queue, pending, state, size, mode, first, second, slot, bank, source, target;
    uint32 character, storage, entry, trailer, index, quotient;
    sint32 allocation_kind;
    FUNCTION_MARKER(0x8006F500, "SLUS_003.73");
    queue = 0x800EADB8 + (r_u32(0x800FF79C) << 4);
    pending = r_u8(queue + 13);
    if (!pending)
        return pending;
    state = r_u32(0x800FF7B0);
    if (state == 1)
    {
        if (!r_u32(0x800FF700))
            w_u32(0x800FF7B0, 2);
        return 2;
    }
    if ((sint32)state < 2)
    {
        if (state)
            return 1;
        size = sub_8006B04C(queue);
        mode = r_u8(queue + 14);
        allocation_kind = 1;
        if (mode)
        {
            if (mode != 1)
            {
                w_u32(0x800FF7B0, 1);
                return 1;
            }
            first = r_u32(0x800FF778);
            if (first != 0xFFFFFFFF)
            {
                second = r_u32(0x800FF77C);
                if (second != 0xFFFFFFFF)
                {
                    slot = r_u32(0x800FF798);
                    bank = r_u8(0x800FF778 + (slot << 2));
                    w_u8(queue + 15, bank);
                    UnloadModelBank(bank, 1);
                    slot = r_u32(0x800FF798);
                    bank = r_u8(queue + 15);
                    w_u32(0x800FF778 + (slot << 2), bank);
                }
                else if (first != second)
                    w_u32(0x800FF77C, r_u8(queue + 15));
                else
                    w_u32(0x800FF778, r_u8(queue + 15));
            }
            else
                w_u32(0x800FF778, r_u8(queue + 15));
            target = 0x800EAEF8 + ((uint32)r_u8(queue + 15) << 6);
            source = queue;
            if (r_u8(source) != 46)
            {
                for (index = 0; index < 9; ++index)
                {
                    character = r_u8(source);
                    ++source;
                    w_u8(target, character);
                    character = r_u8(source);
                    ++target;
                    if (character == 46)
                        break;
                }
            }
            w_u8(target, 0);
            allocation_kind = -2;
        }
        storage = sub_8006B864(size, (uint32)allocation_kind, 1);
        entry = 0x800EAEF8 + ((uint32)r_u8(queue + 15) << 6);
        w_u32(entry + 20, storage);
        entry = 0x800EAEF8 + ((uint32)r_u8(queue + 15) << 6);
        BeginResourceRead(r_u32(entry + 20));
        w_u32(0x800FF7B0, 1);
        return 1;
    }
    if (state != 2)
        return 2;
    mode = r_u8(queue + 14);
    if (!mode)
        LoadModelResourceData(r_u8(queue + 15));
    else if (mode == 1)
    {
        LoadModelResourceData(r_u8(queue + 15));
        AttachModelBankObjects(r_u8(queue + 15));
        slot = FindActiveModelBankSlot(r_u8(queue + 15));
        entry = 0x800EAEF8 + ((uint32)r_u8(queue + 15) << 6);
        trailer = FindModelResourceTrailer(r_u32(entry + 20));
        w_u32(0x800FF7B4 + (slot << 2), trailer);
    }
    w_u8(queue + 13, 0);
    index = r_u32(0x800FF79C) + 1;
    w_u32(0x800FF7B0, 0);
    quotient = (uint32)((sint32)index / 20);
    quotient *= 20;
    w_u32(0x800FF79C, index - quotient);
    return quotient;
}

extern "C" uint32 sub_8006F164(uint32 name)
{
    FUNCTION_MARKER(0x8006F164, "SLUS_003.73");
    return FindModelTextureGroup(name);
}
extern "C" uint32 sub_8006F238(uint32 name, uint32 suffix)
{
    FUNCTION_MARKER(0x8006F238, "SLUS_003.73");
    return WriteModelLoadFilename(name, suffix);
}
extern "C" uint32 sub_8006F29C(uint32 name, uint32 active)
{
    FUNCTION_MARKER(0x8006F29C, "SLUS_003.73");
    return QueueModelBankLoad(name, active);
}
extern "C" uint32 sub_8006F500(void)
{
    FUNCTION_MARKER(0x8006F500, "SLUS_003.73");
    return ProcessModelBankLoadQueue();
}

uint32 WaitForModelBankLoads()
{
    uint32 busy, pending;
    FUNCTION_MARKER(0x8006F7D8, "SLUS_003.73");
    busy = r_u32(0x800FF700);
    for (;;)
    {
        if (!busy)
        {
            pending = r_u8(0x800EADB8 + (r_u32(0x800FF79C) << 4) + 13);
            if (!pending)
                return pending;
        }
        ProcessModelBankLoadQueue();
        ProcessResourceRead();
        busy = r_u32(0x800FF700);
    }
}

extern "C" uint32 sub_8006F7D8(void)
{
    FUNCTION_MARKER(0x8006F7D8, "SLUS_003.73");
    return WaitForModelBankLoads();
}

uint32 PrepareModelBankGeometry(uint32 bank)
{
    FUNCTION_MARKER(0x8007D8E8, "SLUS_003.73");
    uint32 entry = 0x800EAEF8 + (bank << 6);
    uint32 table = r_u32(entry + 16);
    uint32 records = r_u32(entry + 20);
    uint32 geometry = r_u32(table);
    sint32 geometry_count = (sint32)r_u32(table - 4);
    uint32 record_count = r_u32(records + 8);
    records += 12;
    for (sint32 index = 0; index < geometry_count; ++index)
    {
        uint32 flags = r_u32(geometry);
        uint32 vertices = r_u32(geometry + 4);
        uint32 normals = r_u32(geometry + 8);
        uint32 polygons = r_u32(geometry + 12);
        uint32 combined_flags = 0;
        uint32 common_flags = 0xFFFFFFFF;
        if (!(flags & 8))
        {
            uint32 size_x = (uint32)((sint32)(r_u32(geometry + 20) + 4095) >> 12);
            uint32 size_y = r_u32(geometry + 24);
            w_u32(geometry + 20, ((0 - size_x) << 16) | size_x);
            size_y = (uint32)((sint32)(size_y + 4095) >> 12);
            uint32 size_z = r_u32(geometry + 28);
            w_u32(geometry + 24, ((0 - size_y) << 16) | size_y);
            flags = r_u32(geometry);
            size_z = (uint32)((sint32)(size_z + 4095) >> 12);
            w_u32(geometry + 28, ((0 - size_z) << 16) | size_z);
            w_u32(geometry, flags | 8);
        }
        uint32 vertex = geometry + 32;
        for (uint32 vertex_index = 0; vertex_index < vertices; ++vertex_index, vertex += 8)
        {
            if (r_u16(vertex + 6) & 2)
            {
                uint32 offset = r_u16(vertex + 2) << 3;
                w_u16(vertex + 2, offset);
                w_u16(vertex, offset);
                w_u16(vertex + 2, 0);
            }
        }
        uint32 polygon = vertex + (normals << 3);
        for (uint32 polygon_index = 0; polygon_index < polygons; ++polygon_index)
        {
            for (uint32 corner = 0; corner < 4; ++corner)
                w_u16(polygon + 4 + corner * 2, r_u16(polygon + 4 + corner * 2) << 3);
            uint32 normal = r_u16(polygon + 16);
            flags = r_u32(polygon);
            w_u16(polygon + 16, normal << 3);
            if (!(flags & 64))
                w_u32(polygon, flags ^ 128);
            flags = r_u32(polygon);
            if (!(flags & 2048))
                w_u32(polygon + 12, r_u32(polygon + 12) & 0xFFFFFF);
            flags = r_u32(polygon);
            if ((flags & 3) == 3)
            {
                uint32 uv0 = r_u16(polygon + 24);
                uint32 uv1 = r_u16(polygon + 26);
                uint32 uv2 = r_u16(polygon + 28);
                uint32 texture = r_u32(polygon + 20);
                uint32 uv3 = r_u16(polygon + 30);
                uint32 clut = r_u16(texture + 2);
                uint32 page = r_u16(texture + 6);
                if (flags & 32)
                    w_u32(polygon + 32, r_u32(texture + 12));
                else
                {
                    uint32 origin = r_u16(texture);
                    uv0 += origin;
                    uv1 += origin;
                    uv2 += origin;
                    uv3 += origin;
                }
                w_u32(polygon + 20, (uv0 & 65535) | (clut << 16));
                w_u32(polygon + 24, (uv1 & 65535) | (page << 16));
                w_u32(polygon + 28, (uv2 & 65535) | (uv3 << 16));
            }
            flags = r_u32(polygon);
            uint32 surface = r_u16(polygon + 18);
            combined_flags |= flags;
            common_flags &= surface;
            if (flags & 16)
                w_u16(polygon + 10, r_u16(polygon + 8));
            polygon += r_u16(polygon + 2) & 0xFFFC;
        }
        if (common_flags & 1)
            w_u32(geometry, r_u32(geometry) | 16);
        if (!(combined_flags & 192))
            w_u32(geometry, r_u32(geometry) | 32);
        if (combined_flags & 4096)
            w_u32(geometry, r_u32(geometry) | 64);
        geometry = polygon;
    }
    for (uint32 index = 0; index < record_count; ++index, records += 36)
    {
        w_u32(records + 28, index < record_count - 1 ? records + 36 : 0);
        w_u8(records + 27, bank);
        uint32 model_index = r_u16(records + 22);
        w_u8(records + 26, 0);
        table = r_u32(0x800EAEF8 + ((bank & 255) << 6) + 16);
        geometry = r_u32(table + (model_index << 2));
        if (r_u32(geometry) & 16)
            w_u16(records, r_u16(records) | 32);
    }
    return 1;
}

extern "C" uint32 sub_8007D8E8(uint32 bank)
{
    FUNCTION_MARKER(0x8007D8E8, "SLUS_003.73");
    return PrepareModelBankGeometry(bank);
}

uint32 RebuildModelVramAllocations()
{
    FUNCTION_MARKER(0x800691B8, "SLUS_003.73");
    uint32 node = r_u32(0x800FF67C);
    while (node)
    {
        uint32 current = node;
        node = r_u32(node + 8);
        ReleaseVramRecord(current);
    }
    w_u32(0x800FF67C, 0);
    CreateFreeVramRectangle(512, 256, 512, 256);
    uint32 result = CreateFreeVramRectangle(512, 0, 512, 256);
    uint32 copies = 0;
    node = r_u32(0x800FF678);
    while (node)
    {
        result = AllocateHeapBlock(20, 0, 1);
        uint32 first = r_u32(node);
        uint32 second = r_u32(node + 4);
        uint32 third = r_u32(node + 8);
        uint32 fourth = r_u32(node + 12);
        w_u32(result, first);
        w_u32(result + 4, second);
        w_u32(result + 8, third);
        w_u32(result + 12, fourth);
        uint32 dimensions = r_u32(node + 16);
        w_u32(result + 16, dimensions);
        w_u32(result + 8, copies);
        node = r_u32(node + 8);
        copies = result;
    }
    node = r_u32(0x800FF678);
    while (node)
    {
        uint32 current = node;
        node = r_u32(node + 8);
        result = ReleaseVramRecord(current);
    }
    w_u32(0x800FF678, 0);
    uint32 coordinates[2];
    for (node = copies; node; node = r_u32(node + 8))
    {
        uint32 allocation_mode = r_u8(node + 13);
        uint32 width = r_u16(node + 16);
        uint32 height = r_u16(node + 18);
        w_u32(0x800FF674, allocation_mode);
        uint32 owner = (uint32)(sint32)(sint8)r_u8(node + 12);
        uint32 depth = r_u8(node + 14);
        uint32 flags = r_u8(node + 15);
        result = AllocateModelTextureRectangle(owner, width, height, &coordinates[0], &coordinates[1], depth, flags, 0);
    }
    while (copies)
    {
        node = copies;
        copies = r_u32(copies + 8);
        result = ReleaseHeapBlock(node);
    }
    return result;
}

extern "C" uint32 sub_800691B8(void)
{
    FUNCTION_MARKER(0x800691B8, "SLUS_003.73");
    return RebuildModelVramAllocations();
}

uint32 AllocateVramRecord()
{
    FUNCTION_MARKER(0x80068240, "SLUS_003.73");
    uint32 record = AllocateHeapBlock(20, 0, 1);
    uint32 count = r_u32(0x800FF670);
    w_u32(0x800FF670, count + 1);
    return record;
}

uint32 ReleaseVramRecord(uint32 record)
{
    FUNCTION_MARKER(0x80068270, "SLUS_003.73");
    ReleaseHeapBlock(record);
    uint32 count = r_u32(0x800FF670) - 1;
    w_u32(0x800FF670, count);
    return count;
}

uint32 PrependVramRecord(uint32 *head)
{
    FUNCTION_MARKER(0x80068298, "SLUS_003.73");
    uint32 record = AllocateVramRecord();
    uint32 address = GuestMemoryAddress(head);
    uint32 previous = address ? r_u32(address) : xport_load_le32(head);
    w_u32(record + 8, previous);
    if (address)
        w_u32(address, record);
    else
        xport_store_le32(head, record);
    return record;
}

static uint32 SetVramRectangle(uint32 record, uint32 x, uint32 y, uint32 width, uint32 height)
{
    w_u16(record, x);
    w_u16(record + 2, y);
    w_u16(record + 4, width);
    w_u16(record + 6, height);
    return record;
}

uint32 CreateFreeVramRectangle(uint32 x, uint32 y, uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x800682CC, "SLUS_003.73");
    uint32 record = PrependVramRecord((uint32 *)psx_addr(0x800FF67C, 4));
    return SetVramRectangle(record, x, y, width, height);
}

uint32 CreateUsedVramRectangle(uint32 x, uint32 y, uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x8006832C, "SLUS_003.73");
    uint32 record = PrependVramRecord((uint32 *)psx_addr(0x800FF678, 4));
    return SetVramRectangle(record, x, y, width, height);
}

extern "C" uint32 sub_80068240(void)
{
    FUNCTION_MARKER(0x80068240, "SLUS_003.73");
    return AllocateVramRecord();
}
extern "C" uint32 sub_80068270(uint32 record)
{
    FUNCTION_MARKER(0x80068270, "SLUS_003.73");
    return ReleaseVramRecord(record);
}
extern "C" uint32 sub_80068298(uint32 head)
{
    FUNCTION_MARKER(0x80068298, "SLUS_003.73");
    return PrependVramRecord((uint32 *)psx_addr(head, 4));
}
extern "C" uint32 sub_800682CC(uint32 x, uint32 y, uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x800682CC, "SLUS_003.73");
    return CreateFreeVramRectangle(x, y, width, height);
}
extern "C" uint32 sub_8006832C(uint32 x, uint32 y, uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x8006832C, "SLUS_003.73");
    return CreateUsedVramRectangle(x, y, width, height);
}

uint32 RemoveVramRecord(uint32 *head, uint32 record)
{
    FUNCTION_MARKER(0x8006838C, "SLUS_003.73");
    uint32 address = GuestMemoryAddress(head);
    uint32 previous = address ? r_u32(address) : xport_load_le32(head);
    if (previous == record)
    {
        uint32 next = r_u32(record + 8);
        if (address)
            w_u32(address, next);
        else
            xport_store_le32(head, next);
    }
    else
    {
        // TODO Original assumes the record belongs to a nonempty list
        while (r_u32(previous + 8) != record)
            previous = r_u32(previous + 8);
        uint32 next = r_u32(record + 8);
        w_u32(previous + 8, next);
    }
    return ReleaseVramRecord(record);
}

extern "C" uint32 sub_8006838C(uint32 head, uint32 record)
{
    FUNCTION_MARKER(0x8006838C, "SLUS_003.73");
    return RemoveVramRecord((uint32 *)psx_addr(head, 4), record);
}

static bool AreVramRectanglesAdjacent(uint32 record, uint32 candidate)
{
    uint32 record_y = r_u16(record + 2);
    uint32 candidate_y = r_u16(candidate + 2);
    bool horizontal = record_y == candidate_y;
    if (horizontal)
    {
        uint32 record_height = r_u16(record + 6);
        uint32 candidate_height = r_u16(candidate + 6);
        horizontal = record_height == candidate_height;
    }
    if (horizontal)
    {
        uint32 candidate_x = r_u16(candidate);
        uint32 candidate_width = r_u16(candidate + 4);
        uint32 record_x = r_u16(record);
        if (candidate_x + candidate_width == record_x)
            return true;
        uint32 record_width = r_u16(record + 4);
        return record_x + record_width == candidate_x;
    }
    uint32 record_x = r_u16(record);
    uint32 candidate_x = r_u16(candidate);
    if (record_x != candidate_x)
        return false;
    uint32 record_width = r_u16(record + 4);
    uint32 candidate_width = r_u16(candidate + 4);
    if (record_width != candidate_width)
        return false;
    candidate_y = r_u16(candidate + 2);
    record_y = r_u16(record + 2);
    if ((candidate_y & 256) != (record_y & 256))
        return false;
    candidate_y = r_u16(candidate + 2);
    uint32 candidate_height = r_u16(candidate + 6);
    record_y = r_u16(record + 2);
    if (candidate_y + candidate_height == record_y)
        return true;
    uint32 record_height = r_u16(record + 6);
    return record_y + record_height == candidate_y;
}

uint32 CoalesceFreeVramRectangles(uint32 record)
{
    FUNCTION_MARKER(0x800690A0, "SLUS_003.73");
    uint32 result = 0;
    bool merged;
    do
    {
        uint32 candidate = r_u32(0x800FF67C);
        merged = false;
        while (candidate)
        {
            if (AreVramRectanglesAdjacent(record, candidate))
            {
                record = MergeFreeVramRectangles(candidate, record);
                merged = true;
                result = 1;
                break;
            }
            candidate = r_u32(candidate + 8);
        }
    } while (merged);
    return result;
}

extern "C" uint32 sub_800690A0(uint32 record)
{
    FUNCTION_MARKER(0x800690A0, "SLUS_003.73");
    return CoalesceFreeVramRectangles(record);
}

uint32 ReleaseUsedVramRectangle(uint32 x, uint32 y)
{
    FUNCTION_MARKER(0x80068DC4, "SLUS_003.73");
    uint32 record, record_x, record_y, width, height;
    x = (uint32)(sint32)(sint16)x;
    y = (uint32)(sint32)(sint16)y;
    record = r_u32(0x800FF678);
    while (record)
    {
        record_x = r_u16(record);
        if (record_x == x)
        {
            record_y = r_u16(record + 2);
            if (record_y == y)
            {
                width = r_u16(record + 4);
                height = r_u16(record + 6);
                RemoveVramRecord((uint32 *)psx_addr(0x800FF678, 4), record);
                record = CreateFreeVramRectangle(record_x, record_y, width, height);
                return CoalesceFreeVramRectangles(record);
            }
        }
        record = r_u32(record + 8);
    }
    return record;
}

extern "C" uint32 sub_80068DC4(uint32 x, uint32 y)
{
    FUNCTION_MARKER(0x80068DC4, "SLUS_003.73");
    return ReleaseUsedVramRectangle(x, y);
}

uint32 MergeFreeVramRectangles(uint32 first, uint32 second)
{
    FUNCTION_MARKER(0x80068E80, "SLUS_003.73");
    uint32 first_y = r_u16(first + 2);
    uint32 second_y = r_u16(second + 2);
    uint32 result, x, y, width, height;
    if (first_y == second_y)
    {
        uint32 first_x = r_u16(first);
        uint32 second_x = r_u16(second);
        result = second_x < first_x;
        if (first_x < second_x)
        {
            uint32 first_width = r_u16(first + 4);
            result = first_x + first_width;
            if (result != second_x)
                return result;
            uint32 first_height = r_u16(first + 6);
            result = r_u16(second + 6);
            if (first_height != result)
                return result;
            x = first_x;
            y = r_u16(first + 2);
            uint32 second_width = r_u16(second + 4);
            height = r_u16(first + 6);
            width = first_width + second_width;
        }
        else if (second_x < first_x)
        {
            second_x = r_u16(second);
            uint32 second_width = r_u16(second + 4);
            first_x = r_u16(first);
            result = second_x + second_width;
            if (result != first_x)
                return result;
            uint32 first_height = r_u16(first + 6);
            result = r_u16(second + 6);
            if (first_height != result)
                return result;
            x = second_x;
            y = r_u16(second + 2);
            uint32 first_width = r_u16(first + 4);
            height = r_u16(first + 6);
            width = first_width + second_width;
        }
        else
            return result;
    }
    else
    {
        uint32 first_x = r_u16(first);
        result = r_u16(second);
        if (first_x != result)
            return result;
        result = second_y < first_y;
        if (first_y < second_y)
        {
            first_y = r_u16(first + 2);
            uint32 first_height = r_u16(first + 6);
            second_y = r_u16(second + 2);
            result = first_y + first_height;
            if (result != second_y)
                return result;
            uint32 first_width = r_u16(first + 4);
            result = r_u16(second + 4);
            if (first_width != result)
                return result;
            y = first_y;
            x = r_u16(first);
            uint32 second_height = r_u16(second + 6);
            width = r_u16(first + 4);
            height = first_height + second_height;
        }
        else if (second_y < first_y)
        {
            second_y = r_u16(second + 2);
            uint32 second_height = r_u16(second + 6);
            first_y = r_u16(first + 2);
            result = second_y + second_height;
            if (result != first_y)
                return result;
            uint32 first_width = r_u16(first + 4);
            result = r_u16(second + 4);
            if (first_width != result)
                return result;
            y = second_y;
            x = r_u16(second);
            uint32 first_height = r_u16(first + 6);
            width = r_u16(first + 4);
            height = first_height + second_height;
        }
        else
            return result;
    }
    RemoveVramRecord((uint32 *)psx_addr(0x800FF67C, 4), first);
    RemoveVramRecord((uint32 *)psx_addr(0x800FF67C, 4), second);
    return CreateFreeVramRectangle(x, y, width, height);
}

extern "C" uint32 sub_80068E80(uint32 first, uint32 second)
{
    FUNCTION_MARKER(0x80068E80, "SLUS_003.73");
    return MergeFreeVramRectangles(first, second);
}


uint32 InitializeModelVramRectangles()
{
    FUNCTION_MARKER(0x80068404, "SLUS_003.73");
    w_u32(0x800FF670, 0);
    w_u32(0x800FF67C, 0);
    w_u32(0x800FF678, 0);
    CreateFreeVramRectangle(512, 256, 512, 256);
    return CreateFreeVramRectangle(512, 0, 512, 256);
}

extern "C" uint32 sub_80068404(void)
{
    FUNCTION_MARKER(0x80068404, "SLUS_003.73");
    return InitializeModelVramRectangles();
}


static void StoreModelVramCoordinate(uint32 *output, uint32 value)
{
    uint32 address = GuestMemoryAddress(output);
    if (address)
        w_u32(address, value);
    else
        memcpy(output, &value, 4);
}

static bool IsVramRecordAreaSmaller(uint32 candidate, uint32 selected)
{
    uint32 width, height, selected_width, selected_height;
    width = r_u16(candidate + 4);
    height = r_u16(candidate + 6);
    selected_width = r_u16(selected + 4);
    selected_height = r_u16(selected + 6);
    return (sint32)(width * height) < (sint32)(selected_width * selected_height);
}

uint32 AllocateModelTextureRectangle(uint32 owner, uint32 width, uint32 height, uint32 *x, uint32 *y, uint32 depth, uint32 flags, uint32 name)
{
    sint32 v12;
    sint32 v14;
    uint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    unsigned short v20;
    unsigned short v21;
    sint32 v22;
    uint32 v23;
    uint32 v24;
    uint32 v25;
    uint32 v26;
    sint32 result;
    sint8 v28;
    unsigned short v29;
    uint32 v30;
    uint32 v31;
    sint32 v32;
    signed int v33;
    signed int v34;
    sint32 v35;
    sint32 v36;
    uint32 v37;
    sint32 v38;
    signed int v39;
    sint32 v40;
    uint32 v41;
    sint32 v42;
    uint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    uint32 v52;
    sint32 v53;
    uint32 v54;
    sint32 v55;
    sint32 v56;
    sint32 v57;
    uint32 v58;
    sint32 v59;
    sint32 v60;
    sint32 v61;
    sint32 v62;
    sint32 v63;
    uint32 v64;
    sint32 v65;
    sint32 v66;
    sint32 v67;
    uint32 v68;
    sint8 v69;
    short v70;
    sint32 v71;
    sint32 v72;
    sint32 v73;
    sint32 v74;
    FUNCTION_MARKER(0x80068450, "SLUS_003.73");
    // TODO Audit exact candidate read order and extreme signed dimensions
    v12 = width;
    v73 = width;
    v14 = width;
    v74 = height;
    if (!flags)
        goto LABEL_28;
    if (((width == 256) && (height == 256)))
        flags = 0;
    if (!flags)
    {
    LABEL_28:
        if ((depth != 16))
            v14 = (((sint32)(((uint32)(width) + (uint32)(1)))) / 2);

        if ((depth == 4))
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) / 2);
        v31 = r_u32(0x800FF67C);
        v30 = 0;
        if (v31)
        {
            do
            {
                v32 = ((sint32)r_u16(v31));
                v33 = ((sint32)(((uint32)(v32) + (uint32)(v14))));
                if ((depth == 8))
                    v34 = ((uint32)((v32 & 0xFFFFFFC0)) + (uint32)(128));
                else
                    v34 = ((uint32)((v32 & 0xFFFFFFC0)) + (uint32)(64));
                v35 = (((sint32)(v34)) < ((sint32)(v33)));
                if (((!r_u32(0x800FF674) || ((((sint32)r_u16(v31 + 2)) + height) & 255)) && ((((sint32)(owner)) < 0) || !v35)))
                {
                    v36 = ((sint32)r_u16((v31 + (2) * 2)));
                    if (((v36 == v14) && (((sint32)r_u16((v31 + (3) * 2))) == height)))
                    {
                        v30 = v31;
                        break;
                    }
                    if (((((((!v30 || ((((sint32)r_u16((v30 + (2) * 2))) != v14) && (((sint32)r_u16((v30 + (3) * 2))) != height))) || (v36 == v14)) || (((sint32)r_u16((v31 + (3) * 2))) == height)) && (((sint32)r_u16((v31 + (2) * 2))) >= ((sint32)(v14)))) && (((sint32)r_u16((v31 + (3) * 2))) >= ((sint32)(height)))) && (!v30 || IsVramRecordAreaSmaller(v31, v30))))
                    {
                        v30 = v31;
                    }
                }
                v31 = ((uint32)(r_u32((((uint32)(v31)) + (2) * 4))));
            } while (v31);
        }
        if (!v30)
        {
            v37 = r_u32(0x800FF67C);
            if (v37)
            {
                do
                {
                    v38 = ((uint32)(((sint32)r_u16(v37))) + (uint32)(((sint32)r_u16((v37 + (2) * 2)))));
                    if ((depth == 8))
                        v39 = ((uint32)((((sint32)(((uint32)(v38) - (uint32)(v14)))) & 0xFFFFFFC0)) + (uint32)(128));
                    else
                        v39 = ((uint32)((((sint32)(((uint32)(v38) - (uint32)(v14)))) & 0xFFFFFFC0)) + (uint32)(64));
                    v40 = (((sint32)(v39)) < ((sint32)(v38)));
                    if ((((!r_u32(0x800FF674) || ((((sint32)r_u16(v37 + 2)) + height) & 255)) && ((((sint32)(owner)) < 0) || !v40)) && (((((sint32)r_u16((v37 + (2) * 2))) == v14) && (((sint32)r_u16((v37 + (3) * 2))) == height)) || ((((((!v30 || ((((sint32)r_u16((v30 + (2) * 2))) != v14) && (((sint32)r_u16((v30 + (3) * 2))) != height))) || (((sint32)r_u16((v37 + (2) * 2))) == v14)) || (((sint32)r_u16((v37 + (3) * 2))) == height)) && (((sint32)r_u16((v37 + (2) * 2))) >= ((sint32)(v14)))) && (((sint32)r_u16((v37 + (3) * 2))) >= ((sint32)(height)))) && (!v30 || IsVramRecordAreaSmaller(v37, v30))))))
                    {
                        v30 = v37;
                    }
                    v37 = ((uint32)(r_u32((((uint32)(v37)) + (2) * 4))));
                } while (v37);
            }
            if (!v30)
            {
                v41 = r_u32(0x800FF67C);
                if (v41)
                {
                    do
                    {
                        if ((!r_u32(0x800FF674) || ((((sint32)r_u16(v41 + 2)) + height) & 255)) && (!v30 || (((sint32)r_u16(v30 + 4)) != v14 && ((sint32)r_u16(v30 + 6)) != height) || ((sint32)r_u16(v41 + 4)) == v14 || ((sint32)r_u16(v41 + 6)) == height) && (sint32)(((sint32)r_u16(v41 + 4)) - (((((sint32)r_u16(v41)) + 64) & 0xFFFFFFC0) - ((sint32)r_u16(v41)))) >= v14 && (sint32)((sint32)r_u16(v41 + 6)) >= (sint32)height && (!v30 || (sint32)((uint32)((sint32)r_u16(v41 + 4)) * ((sint32)r_u16(v41 + 6))) < (sint32)((uint32)((sint32)r_u16(v30 + 4)) * ((sint32)r_u16(v30 + 6)))))
                        {
                            v30 = v41;
                        }
                        v41 = ((uint32)(r_u32((((uint32)(v41)) + (2) * 4))));
                    } while (v41);
                }
                if (!v30)
                {
                    GameDiagnostic("Out of VRAM whilst packing %x, (%d,%d)\n", name, v73, v74);
                    return 0;
                }
                v42 = ((sint32)r_u16(v30));
                v43 = (((uint32)(v42) + (uint32)(64)) & 0xFFFFFFC0);
                v44 = ((sint32)r_u16((v30 + (2) * 2)));
                v45 = ((uint32)(v43) - (uint32)(v42));
                v46 = ((sint32)(((uint32)(v44) - (uint32)(v45))));
                if ((((sint32)(((sint32)(((uint32)(v44) - (uint32)(v45)))))) > 0))
                {
                    CreateFreeVramRectangle(v43, ((sint32)r_u16((v30 + (1) * 2))), ((sint32)(((uint32)(v44) - (uint32)(v45)))), ((sint32)r_u16((v30 + (3) * 2))));
                    w_u16(v30 + 4, r_u16(v30 + 4) - (uint32)v46);
                    width = v12;
                }
                result = AllocateModelTextureRectangle(owner, width, height, x, y, depth, 0, name);
                w_u8(((uint32)(((uint32)(result) + (uint32)(14)))), depth);
                w_u8(((uint32)(((uint32)(result) + (uint32)(15)))), flags);
                w_u8(((uint32)(((uint32)(result) + (uint32)(12)))), owner);
                goto LABEL_114;
            }
            v47 = ((sint32)(v30));
            v48 = ((sint32)r_u16(v30));
            v49 = ((sint32)r_u16((v30 + (1) * 2)));
            v50 = ((sint32)r_u16(((uint32)(((uint32)(v47) + (uint32)(4))))));
            v51 = ((sint32)r_u16(((uint32)(((uint32)(v47) + (uint32)(6))))));
            RemoveVramRecord((uint32 *)psx_addr(0x800FF67C, 4), v47);
            v52 = CreateUsedVramRectangle(((sint32)(((uint32)(((sint32)(((uint32)(v48) + (uint32)(v50))))) - (uint32)(v14)))), v49, v14, height);
            if ((v50 == v14))
            {
                v53 = ((sint32)(((uint32)(v49) + (uint32)(height))));
                if ((v51 == height))
                {
                LABEL_103:
                    StoreModelVramCoordinate(x, ((sint32)(((uint32)(((sint32)(((uint32)(v48) + (uint32)(v50))))) - (uint32)(v14)))));

                LABEL_113:
                    StoreModelVramCoordinate(y, v49);

                    w_u8((v52 + (14) * 1), depth);
                    result = ((sint32)(v52));
                    w_u8((v52 + (15) * 1), flags);
                    w_u8((v52 + (12) * 1), owner);
                LABEL_114:
                    v69 = r_u8(0x800FF674);

                    w_u16(((uint32)(((uint32)(result) + (uint32)(16)))), v73);
                    v70 = v74;
                    w_u8(((uint32)(((uint32)(result) + (uint32)(13)))), v69);
                    w_u16(((uint32)(((uint32)(result) + (uint32)(18)))), v70);
                    return result;
                }
                v55 = v48;
                v56 = v50;
            }
            else
            {
                v53 = v49;
                if ((v51 == height))
                {
                    v55 = v48;
                    v56 = ((sint32)(((uint32)(v50) - (uint32)(v14))));
                    v57 = v51;
                LABEL_102:
                    v58 = CreateFreeVramRectangle(v55, v53, v56, v57);

                    CoalesceFreeVramRectangles(v58);
                    goto LABEL_103;
                }
                v54 = CreateFreeVramRectangle(v48, v49, ((sint32)(((uint32)(v50) - (uint32)(v14)))), v51);
                CoalesceFreeVramRectangles(v54);
                v55 = ((sint32)(((uint32)(((sint32)(((uint32)(v48) + (uint32)(v50))))) - (uint32)(v14))));
                v53 = ((sint32)(((uint32)(v49) + (uint32)(height))));
                v56 = v14;
            }
            v57 = ((sint32)(((uint32)(v51) - (uint32)(height))));
            goto LABEL_102;
        }
        v59 = ((sint32)(v30));
        v60 = ((sint32)r_u16(v30));
        v49 = ((sint32)r_u16((v30 + (1) * 2)));
        v61 = ((sint32)r_u16(((uint32)(((uint32)(v59) + (uint32)(4))))));
        v62 = ((sint32)r_u16(((uint32)(((uint32)(v59) + (uint32)(6))))));
        RemoveVramRecord((uint32 *)psx_addr(0x800FF67C, 4), v59);
        v52 = CreateUsedVramRectangle(v60, v49, v14, height);
        if ((v61 == v14))
        {
            v63 = ((sint32)(((uint32)(v49) + (uint32)(height))));
            if ((v62 == height))
            {
            LABEL_112:
                StoreModelVramCoordinate(x, v60);

                goto LABEL_113;
            }
            v65 = v60;
            v66 = v61;
        }
        else
        {
            v63 = v49;
            if ((v62 == height))
            {
                v65 = ((sint32)(((uint32)(v60) + (uint32)(v14))));
                v66 = ((sint32)(((uint32)(v61) - (uint32)(v14))));
                v67 = v62;
            LABEL_111:
                v68 = CreateFreeVramRectangle(v65, v63, v66, v67);

                CoalesceFreeVramRectangles(v68);
                goto LABEL_112;
            }
            v64 = CreateFreeVramRectangle(((sint32)(((uint32)(v60) + (uint32)(v14)))), v49, ((sint32)(((uint32)(v61) - (uint32)(v14)))), v62);
            CoalesceFreeVramRectangles(v64);
            v65 = v60;
            v63 = ((sint32)(((uint32)(v49) + (uint32)(height))));
            v66 = v14;
        }
        v67 = ((sint32)(((uint32)(v62) - (uint32)(height))));
        goto LABEL_111;
    }
    if (((width == 256) || (height == 256)))
    {
        v15 = ((uint32)(AllocateModelTextureRectangle(owner, 256, 256, (uint32 *)&v71, (uint32 *)&v72, depth, 0, 0x800A3A08)));
        v16 = 128;
        v17 = 256;
        if ((depth != 16))
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) >> 1);
        if ((depth == 4))
        {
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) >> 1);
            v16 = 64;
        }
        v18 = ((sint32)r_u16(v15));
        v19 = ((sint32)r_u16((v15 + (1) * 2)));
    }
    else
    {
        v15 = ((uint32)(AllocateModelTextureRectangle(owner, 2 * width, 2 * height, (uint32 *)&v71, (uint32 *)&v72, depth, 0, name)));
        if ((depth != 16))
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) / 2);
        v16 = ((uint32)(2) * (uint32)(v14));
        if ((depth == 4))
        {
            v14 = (((sint32)(((uint32)(v14) + (uint32)(1)))) / 2);
            v16 = ((uint32)(2) * (uint32)(v14));
        }
        v17 = ((uint32)(2) * (uint32)(height));
        v18 = ((sint32)r_u16(v15));
        v19 = ((sint32)r_u16((v15 + (1) * 2)));
        v71 = (((uint32)(((sint32)(((uint32)(v71) + (uint32)(v14))))) - (uint32)(1)) & (0 - (uint32)v14));
        v72 = (((uint32)(((sint32)(((uint32)(v72) + (uint32)(height))))) - (uint32)(1)) & (0 - height));
    }
    v20 = v71;
    v21 = v72;
    v22 = v71;
    w_u16((v15 + (2) * 2), v14);
    w_u16((v15 + (3) * 2), height);
    w_u16(v15, v20);
    w_u16((v15 + (1) * 2), v21);
    StoreModelVramCoordinate(x, v22);
    StoreModelVramCoordinate(y, v72);
    if ((((sint32)(v18)) < ((sint32)(v71))))
    {
        v23 = CreateFreeVramRectangle(v18, v19, ((sint32)(((uint32)(v71) - (uint32)(v18)))), v17);
        CoalesceFreeVramRectangles(v23);
        v16 = (sint32)((uint32)v16 - ((uint32)v71 - (uint32)v18));
    }
    if ((((sint32)(v14)) < ((sint32)(v16))))
    {
        v24 = CreateFreeVramRectangle(((sint32)(((uint32)(v71) + (uint32)(v14)))), v19, ((sint32)(((uint32)(v16) - (uint32)(v14)))), v17);
        CoalesceFreeVramRectangles(v24);
    }
    if ((((sint32)(v19)) < ((sint32)(v72))))
    {
        v25 = CreateFreeVramRectangle(v71, v19, v14, ((sint32)(((uint32)(v72) - (uint32)(v19)))));
        CoalesceFreeVramRectangles(v25);
        v17 = (sint32)((uint32)v17 - ((uint32)v72 - (uint32)v19));
    }
    if ((((sint32)(height)) < ((sint32)(v17))))
    {
        v26 = CreateFreeVramRectangle(v71, ((sint32)(((uint32)(v72) + (uint32)(height)))), v14, ((sint32)(((uint32)(v17) - (uint32)(height)))));
        CoalesceFreeVramRectangles(v26);
    }
    w_u8((((uint32)(v15)) + (14) * 1), depth);
    result = ((sint32)(v15));
    w_u8((((uint32)(v15)) + (15) * 1), flags);
    w_u8((((uint32)(v15)) + (12) * 1), owner);
    v28 = r_u8(0x800FF674);
    w_u16((v15 + (8) * 2), v73);
    v29 = v74;
    w_u8((((uint32)(v15)) + (13) * 1), v28);
    w_u16((v15 + (9) * 2), v29);
    return result;
}

extern "C" uint32 xport_draft_host_sub_80068450_p45(uint32 owner, uint32 width, uint32 height, void *x, void *y, uint32 depth, uint32 flags, uint32 name)
{
    FUNCTION_MARKER(0x80068450, "SLUS_003.73");
    return AllocateModelTextureRectangle(owner, width, height, (uint32 *)x, (uint32 *)y, depth, flags, name);
}
extern "C" uint32 apocalypse_allocate_vram(uint32 owner, uint32 width, uint32 height, uint32 x, uint32 y, uint32 depth, uint32 flags, uint32 name) { return AllocateModelTextureRectangle(owner, width, height, (uint32 *)psx_addr(x, 4), (uint32 *)psx_addr(y, 4), depth, flags, name); }


extern "C" uint32 sub_80068450(uint32 owner, uint32 width, uint32 height, uint32 x, uint32 y, uint32 depth, uint32 flags, uint32 name)
{
    FUNCTION_MARKER(0x80068450, "SLUS_003.73");
    return AllocateModelTextureRectangle(owner, width, height, (uint32 *)psx_addr(x, 4), (uint32 *)psx_addr(y, 4), depth, flags, name);
}

uint32 ClearModelBankGrid(sint32 slot)
{
    FUNCTION_MARKER(0x8007DC5C, "SLUS_003.73");
    if (slot >= 2)
        return 0;
    uint32 base = 0x800EDA30 + 1632 * (uint32)slot;
    w_u32(base, 0);
    for (uint32 row = 0; row < 20; ++row)
        for (uint32 column = 0; column < 20; ++column)
            w_u32(base + 32 + row * 80 + column * 4, 0);
    return 0;
}

uint32 ClearModelBankGrids()
{
    FUNCTION_MARKER(0x8007DCDC, "SLUS_003.73");
    ClearModelBankGrid(0);
    return ClearModelBankGrid(1);
}

extern "C" uint32 sub_8007DC5C(uint32 slot)
{
    FUNCTION_MARKER(0x8007DC5C, "SLUS_003.73");
    return ClearModelBankGrid((sint32)slot);
}
extern "C" uint32 sub_8007DCDC(void)
{
    FUNCTION_MARKER(0x8007DCDC, "SLUS_003.73");
    return ClearModelBankGrids();
}

uint32 InitializeModelBankGrid(uint32 slot, uint32 data)
{
    FUNCTION_MARKER(0x8007E41C, "SLUS_003.73");
    uint32 first = r_u32(data);
    uint32 cursor = data + 4;
    uint32 grid_offset = slot * 1632;
    uint32 grid = 0x800EDA30 + grid_offset;
    w_u32(grid + 4, first);
    uint32 value = r_u32(cursor);
    cursor += 4;
    w_u32(grid + 8, value);
    value = r_u32(cursor);
    cursor += 4;
    w_u32(grid + 12, value);
    value = r_u32(cursor);
    uint32 last = r_u32(grid + 12);
    cursor += 4;
    w_u32(grid + 16, value);
    uint32 columns = r_u16(cursor);
    w_u16(grid + 28, columns);
    sint32 divisor = (sint16)r_u16(grid + 28);
    sint32 dividend = (sint32)(last - first);
    uint32 step;
    if (!divisor)
        step = dividend < 0 ? 1 : 0xFFFFFFFF;
    else if (dividend == (sint32)0x80000000 && divisor == -1)
        step = 0x80000000;
    else
        step = (uint32)(dividend / divisor);
    cursor += 2;
    uint32 rows = r_u16(cursor);
    cursor += 2;
    w_u32(grid, 1);
    w_u16(grid + 30, rows);
    w_u32(grid + 20, step);
    sint32 row = 0;
    for (;;)
    {
        sint32 row_count = (sint16)r_u16(grid + 30);
        if (row >= row_count)
            return 0;
        sint32 column = 0;
        uint32 cell_offset = (uint32)row * 4;
        for (;;)
        {
            sint32 column_count = (sint16)r_u16(grid + 28);
            if (column >= column_count)
                break;
            cursor += 8;
            sint32 count = (sint32)r_u32(cursor);
            cursor += 4;
            w_u32(0x800EDA30 + grid_offset + cell_offset + 32, cursor);
            uint32 bank = r_u32(0x800FF778 + slot * 4);
            uint32 records = r_u32(0x800EAEF8 + bank * 64 + 20);
            if (count > 0)
            {
                do
                {
                    uint32 index = r_u32(cursor);
                    w_u32(cursor, records + index * 36 + 12);
                    cursor += 4;
                    --count;
                } while (count > 0);
            }
            cursor += 4;
            cell_offset += 80;
            ++column;
        }
        ++row;
    }
}

extern "C" uint32 sub_8007E41C(uint32 slot, uint32 data)
{
    FUNCTION_MARKER(0x8007E41C, "SLUS_003.73");
    return InitializeModelBankGrid(slot, data);
}

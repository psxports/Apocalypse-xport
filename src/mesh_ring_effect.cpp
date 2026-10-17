#include "model_render_view.h"
extern "C"
{
#include "draft_first_signatures.h"
}
#include "mesh_ring_effect.h"
#include "native_object_storage.h"
#include "game_heap.h"
#include "model_resources.h"
#include "effect_lifecycle.h"
#include "angle_math.h"
#include "vector_arithmetic.h"
#include <new>
#include <string.h>

static uint32 ReadMeshRingPosition(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

MeshRingEffect::MeshRingEffect(uint32 segments, uint32 resource) : RenderableEffect()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 buffer, count, cursor, index, texture;
    FUNCTION_MARKER(0x80028FB8, "SLUS_003.73");
    w_u32(object + 68, 0x800A1398);
    buffer = AllocateHeapBlock(140 * segments, 0, 1);
    w_u32(object + 80, buffer);
    w_u32(object + 76, segments);
    tex = &GetTextureResource(resource);
    count = r_u32(object + 76);
    buffer = r_u32(object + 80);
    index = 0;
    if ((sint32)count > 0)
    {
        cursor = buffer + 26;
        do
        {
            w_u8(cursor + 29, 12);
            w_u8(cursor + 33, 62);
            texture = tex->clut;
            w_u16(cursor + 40, texture);
            texture = tex->tpage;
            w_u8(cursor - 23, 12);
            w_u8(cursor - 19, 62);
            w_u16(cursor + 52, texture);
            texture = tex->clut;
            w_u16(cursor - 12, texture);
            texture = tex->tpage;
            index += 1;
            w_u16(cursor, texture);
            count = r_u32(object + 76);
            cursor += 140;
        } while ((sint32)index < (sint32)count);
    }
    SetColors(128, 128, 128);
    SetTextureOffsets(0, 0, 2);
    w_u32(object + 108, -1);
}

uint32 MeshRingEffect::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 buffer, result;
    FUNCTION_MARKER(0x800290F4, "SLUS_003.73");
    buffer = r_u32(object + 80);
    w_u32(object + 68, 0x800A1398);
    ReleaseHeapBlock(buffer);
    result = RenderableEffect::Destroy(0);
    if (reason & 1)
        return ReleaseEffectStorage(object);
    return result;
}

uint32 MeshRingEffect::Update()
{
    // Original 0x8003A144 has no body or defined return carrier
    return 0;
}

uint32 MeshRingEffect::SetTextureOffsets(uint32 u, uint32 v, uint32 repeats)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, buffer, step, cursor, index, next_u, middle_v, bottom_v, result;
    sint32 numerator, denominator;
    FUNCTION_MARKER(0x80029324, "SLUS_003.73");
    w_u32(object + 100, u);
    w_u32(object + 104, v);
    u = tex->uv[0].u + (u & 63);
    v = tex->uv[0].v + (v & 63);
    count = r_u32(object + 76);
    numerator = (sint32)(repeats << 6);
    denominator = (sint32)count;
    if (!denominator)
        step = numerator < 0 ? 1 : (uint32)-1;
    else if ((uint32)numerator == 0x80000000 && denominator == -1)
        step = 0x80000000;
    else
        step = (uint32)(numerator / denominator);
    buffer = r_u32(object + 80);
    result = count;
    index = 0;
    if ((sint32)count > 0)
    {
        middle_v = v + 32;
        bottom_v = v + 64;
        cursor = buffer + 101;
        do
        {
            next_u = u + step;
            w_u8(cursor - 89, u);
            w_u8(cursor - 65, u);
            w_u8(cursor - 37, u);
            w_u8(cursor - 13, u);
            u = next_u & 63;
            w_u8(cursor - 77, next_u);
            w_u8(cursor - 88, v);
            w_u8(cursor - 76, v);
            w_u8(cursor - 53, next_u);
            w_u8(cursor - 64, middle_v);
            w_u8(cursor - 52, middle_v);
            w_u8(cursor - 25, next_u);
            w_u8(cursor - 36, middle_v);
            w_u8(cursor - 24, middle_v);
            w_u8(cursor - 1, next_u);
            w_u8(cursor - 12, bottom_v);
            w_u8(cursor, bottom_v);
            count = r_u32(object + 76);
            index += 1;
            result = (sint32)index < (sint32)count;
            cursor += 140;
        } while (result);
    }
    return result;
}

uint32 MeshRingEffect::SetColors(uint32 red, uint32 green, uint32 blue)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, buffer, cursor, index, result;
    FUNCTION_MARKER(0x800293D8, "SLUS_003.73");
    if ((sint32)red < 0)
        red = 0;
    if ((sint32)green < 0)
        green = 0;
    if ((sint32)blue < 0)
        blue = 0;
    count = r_u32(object + 76);
    buffer = r_u32(object + 80);
    result = count;
    index = 0;
    if ((sint32)count > 0)
    {
        cursor = buffer + 92;
        do
        {
            w_u8(cursor - 86, 0);
            w_u8(cursor - 87, 0);
            w_u8(cursor - 88, 0);
            w_u8(cursor - 74, 0);
            w_u8(cursor - 75, 0);
            w_u8(cursor - 76, 0);
            w_u8(cursor - 64, red);
            w_u8(cursor - 52, red);
            w_u8(cursor - 63, green);
            w_u8(cursor - 51, green);
            w_u8(cursor - 62, blue);
            w_u8(cursor - 50, blue);
            w_u8(cursor - 36, red);
            w_u8(cursor - 24, red);
            w_u8(cursor - 35, green);
            w_u8(cursor - 23, green);
            w_u8(cursor - 34, blue);
            w_u8(cursor - 22, blue);
            w_u8(cursor - 10, 0);
            w_u8(cursor - 11, 0);
            w_u8(cursor - 12, 0);
            w_u8(cursor + 2, 0);
            w_u8(cursor + 1, 0);
            w_u8(cursor, 0);
            count = r_u32(object + 76);
            index += 1;
            result = (sint32)index < (sint32)count;
            cursor += 140;
        } while (result);
    }
    return result;
}

MeshRingEffect &MeshRingEffect::Attach(uint32 object)
{
    return *static_cast<MeshRingEffect *>(FindNativeObjectStorage(object));
}

uint32 MeshRingEffect::SetGeometry(const uint32 *position, uint32 radius, uint32 expansion)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 x, y, z, buffer, count, increment, index = 0;
    uint32 direction[3], offset[3], first[3], second[3];
    uint16 angles[3];
    FUNCTION_MARKER(0x80029198, "SLUS_003.73");
    x = ReadMeshRingPosition(position, 0);
    y = ReadMeshRingPosition(position, 1);
    z = ReadMeshRingPosition(position, 2);
    w_u32(object + 24, x);
    w_u32(object + 28, y);
    w_u32(object + 32, z);
    w_u32(object + 96, expansion);
    w_u32(object + 92, radius);
    CopyAngleVector(angles, (const uint16 *)psx_addr(object + 84, 6));
    buffer = r_u32(object + 80);
    count = r_u32(object + 76);
    // Original signed DIV has no BREAK guard for a zero count
    increment = count ? (uint32)(4096 / (sint32)count) : (uint32)-1;
    while ((sint32)index < (sint32)r_u32(object + 76))
    {
        BuildDirectionVector(direction, 1, angles);
        MultiplyVector(offset, direction, (const uint32 *)psx_addr(object + 92, 4));
        AddVectors(first, (const uint32 *)psx_addr(object + 24, 12), offset);
        w_u32(buffer + 104, first[0]);
        w_u32(buffer + 108, first[1]);
        w_u32(buffer + 112, first[2]);
        MultiplyVector(first, direction, (const uint32 *)psx_addr(object + 96, 4));
        AddVectors(second, (const uint32 *)psx_addr(buffer + 104, 12), first);
        w_u32(buffer + 116, second[0]);
        w_u32(buffer + 120, second[1]);
        w_u32(buffer + 124, second[2]);
        AddVectors(offset, (const uint32 *)psx_addr(buffer + 116, 12), first);
        w_u32(buffer + 128, offset[0]);
        w_u32(buffer + 132, offset[1]);
        w_u32(buffer + 136, offset[2]);
        buffer += 140;
        index += 1;
        angles[1] = (uint16)(angles[1] + increment);
    }
    return 0;
}



extern "C" uint32 sub_800290F4(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x800290F4, "SLUS_003.73");
    return MeshRingEffect::Attach(object).MeshRingEffect::Destroy(reason);
}





extern "C" uint32 sub_80029198(uint32 object, uint32 position, uint32 radius, uint32 expansion)
{
    FUNCTION_MARKER(0x80029198, "SLUS_003.73");
    return MeshRingEffect::Attach(object).SetGeometry((const uint32 *)psx_addr(position, 12), radius, expansion);
}

extern "C" uint32 apocalypse_ring_position_native(uint32 object, const uint32 position[3], uint32 radius, uint32 expansion)
{
    return MeshRingEffect::Attach(object).SetGeometry(position, radius, expansion);
}

static sint32 ReadMeshRingCameraOffset(uint32 address, uint32 axis)
{
    uint32 position = r_u32(address);
    uint32 camera = GetModelRenderView().pos[axis];
    return (sint32)((uint32)((sint32)position >> 12) - camera);
}

static uint32 LinkMeshRingRenderPacket(uint32 packet, uint32 ordering_entry)
{
    uint32 header = r_u32(packet);
    uint32 link = r_u32(ordering_entry);
    w_u32(packet, (header & 0xFF000000) | (link & 0xFFFFFF));
    link = r_u32(ordering_entry);
    uint32 result = (link & 0xFF000000) | (packet & 0xFFFFFF);
    w_u32(ordering_entry, result);
    return result;
}

uint32 MeshRingEffect::Render()
{
    FUNCTION_MARKER(0x8002948C, "SLUS_003.73");
    const uint32 a1 = NativeObjectGuestAddress(this);
    sint32 gte_T4;
    sint32 v2;
    sint32 v3;
    uint32 v4;
    sint32 v5;
    uint32 v6;
    sint32 v8;
    short v9;
    sint32 v11;
    sint32 v12;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    short v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    sint32 v21;
    uint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 result;
    sint32 v29;
    uint32 v30;
    uint32 v31;
    uint32 v32;
    uint32 v33;
    uint32 v34;
    uint32 v35;
    sint32 v36;
    sint8 v37;
    uint32 v38;
    sint32 v39;
    uint32 v40;
    sint32 v41;
    sint32 v42;
    sint32 v43;
    sint32 v44;
    uint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    int v52[2];
    short v53;
    short v54;
    short v55;
    short v56;
    sint32 v57;
    v2 = 528482304;
    v3 = 0;
    v4 = r_u32(((uint32)((a1 + 80))));
    v5 = 528482326;
    v6 = ((uint32)((v4 + 112)));
    while (v3 < (sint32)r_u32(a1 + 76))
    {
        w_u16(((uint32)(v5)), 0);
        v46 = ReadMeshRingCameraOffset(v6+16, 0);
        v49 = ReadMeshRingCameraOffset(v6+20, 1);
        v52[0] = ReadMeshRingCameraOffset(v6+24, 2);
        gte_T4 = (sint32)((uint32)(uint16)v46 | ((uint32)(uint16)v49 << 16));
        xport_gte_write_data(0, gte_T4);
        xport_gte_write_data(1, v52[0]);
        xport_gte_execute(0x180001);
        w_u32(v2, xport_gte_read_data(14));
        v57 = xport_gte_read_data(27);
        v8 = r_u32(((uint32)(v2)));
        if (((((((v8 == 67044351) || (v8 == 67107840)) || (v8 == -67107841)) || (v8 == -67044352)) || (v57 < -20000)) || (v57 >= 20001)))
        {
            w_u16(((uint32)((v5 - 18))), 0);
        }
        else
        {
            v9 = v57;
            w_u16(((uint32)((v5 - 18))), 1);
            w_u16(((uint32)(v5)), v9);
        }
        v47 = ReadMeshRingCameraOffset(v6+4, 0);
        v50 = ReadMeshRingCameraOffset(v6+8, 1);
        v52[0] = ReadMeshRingCameraOffset(v6+12, 2);
        gte_T4 = (sint32)((uint32)(uint16)v47 | ((uint32)(uint16)v50 << 16));
        xport_gte_write_data(0, gte_T4);
        xport_gte_write_data(1, v52[0]);
        xport_gte_execute(0x180001);
        w_u32((v5 - 14), xport_gte_read_data(14));
        v57 = xport_gte_read_data(27);
        v11 = r_u32(((uint32)((v5 - 14))));
        if (((((((v11 == 67044351) || (v11 == 67107840)) || (v11 == -67107841)) || (v11 == -67044352)) || (v57 < -20000)) || (v57 >= 20001)))
        {
            w_u16(((uint32)((v5 - 10))), 0);
        }
        else
        {
            v12 = (v57 >= ((sint16)(r_u16(((uint32)(v5))))));
            w_u16(((uint32)((v5 - 10))), 1);
            if (!v12)
                w_u16(((uint32)(v5)), v57);
        }
        v48 = ReadMeshRingCameraOffset(v6-8, 0);
        v51 = ReadMeshRingCameraOffset(v6-4, 1);
        v52[0] = ReadMeshRingCameraOffset(v6, 2);
        gte_T4 = (sint32)((uint32)(uint16)v48 | ((uint32)(uint16)v51 << 16));
        xport_gte_write_data(0, gte_T4);
        xport_gte_write_data(1, v52[0]);
        xport_gte_execute(0x180001);
        w_u32((v5 - 6), xport_gte_read_data(14));
        v57 = xport_gte_read_data(27);
        v14 = r_u32(((uint32)((v5 - 6))));
        if (((((((v14 == 67044351) || (v14 == 67107840)) || (v14 == -67107841)) || (v14 == -67044352)) || (v57 < -20000)) || (v57 >= 20001)))
        {
            w_u16(((uint32)((v5 - 2))), 0);
        }
        else
        {
            v12 = (v57 >= ((sint16)(r_u16(((uint32)(v5))))));
            w_u16(((uint32)((v5 - 2))), 1);
            if (!v12)
                w_u16(((uint32)(v5)), v57);
        }
        v6 += (35) * 4;
        v5 += 24;
        v2 += 24;
        ++v3;
    }

    v15 = 1;
    v53 = tex->uv[0].u;
    v16 = 528482350;
    v17 = tex->uv[0].v;
    v55 = 64;
    v56 = 64;
    v54 = v17;
    v18 = r_u32(0x1F800000);
    v19 = r_u32(0x1F800008);
    v20 = r_u32(0x1F800010);
    v21 = 528482328;
    v23 = (sint16)r_u16(0x1F800004);
    v24 = (sint16)r_u16(0x1F80000C);
    v25 = (sint16)r_u16(0x1F800014);
    v26 = (sint16)r_u16(0x1F800016);
    v22 = r_u32(a1 + 80);
    while (1)
    {
        v27 = r_u32(((uint32)((a1 + 76))));
        result = (v27 < v15);
        if ((v27 < v15))
            break;
        if ((v15 == v27))
        {
            v16 = 528482326;
            v21 = 528482304;
        }
        sint32 next_depth = (sint16)r_u16(v16);
        if (next_depth < v26)
            v26 = next_depth;
        uint32 forced_depth = r_u16(a1 + 114);
        if ((forced_depth || v26) && ((r_u32(a1 + 108) >> ((uint32)v15 & 31)) & 1))
        {
            if (forced_depth)
                v29 = 4 * r_u16(a1 + 114);
            else
                v29 = ((uint32)v26 - r_u16(a1 + 64)) & 0x3FFC;
            uint32 frame = r_u32(0x800FF660);
            v30 = frame + (uint32)v29 + 112;
            v31 = r_u32(0x800FF668);
            uint32 limit = r_u32(0x800FF374);
            v32 = v31 + 12;
            uint32 next_packet = v31 + 32;
            result = limit < next_packet;
            v33 = v31 + 24;
            if (result)
                return result;
            w_u32(0x800FF668, next_packet);
            w_u8(((uint32)((v32 + 3))), 2);
            w_u32(((uint32)((v32 + 8))), 0);
            w_u32(((uint32)((v32 + 4))), -503316480);
            w_u8(((uint32)((v31 + 3))), 2);
            v34 = ((unsigned char)(v54));
            v35 = ((unsigned char)(v53));
            v36 = v56;
            v37 = v55;
            w_u32(((uint32)((v31 + 8))), 0);
            w_u32(((uint32)((v31 + 4))), ((((((v34 >> 3) << 15) | ((v35 >> 3) << 10)) | 0xE2000000) | ((-4 * v36) & 0x3E0)) | (((sint32)(((unsigned char)(-v37)))) >> 3)));
            v38 = 0;
            uint32 state_tag = r_u32(0x800FF468);
            v39 = r_u32(0x800FF46C);
            v40 = 0;
            w_u32(((uint32)((v31 + 24))), state_tag);
            w_u32(((uint32)((v33 + 4))), v39);
            LinkMeshRingRenderPacket(v32, v30);
            if ((((v23 && v24) && r_u16(((uint32)((v16 - 18))))) && r_u16(((uint32)((v16 - 10))))))
            {
                v38 = r_u32(0x800FF668);
                uint32 limit = r_u32(0x800FF374);
                uint32 next_packet = v38 + 52;
                result = limit < next_packet;
                if (result)
                    return result;
                w_u32(0x800FF668, next_packet);
                w_u32(((uint32)(v38)), r_u32(v22));
                w_u32(((uint32)((v38 + 4))), r_u32((v22 + (1) * 4)));
                w_u32(((uint32)((v38 + 12))), r_u32((v22 + (3) * 4)));
                w_u32(((uint32)((v38 + 16))), r_u32((v22 + (4) * 4)));
                w_u32(((uint32)((v38 + 24))), r_u32((v22 + (6) * 4)));
                w_u32(((uint32)((v38 + 28))), r_u32((v22 + (7) * 4)));
                w_u32(((uint32)((v38 + 36))), r_u32((v22 + (9) * 4)));
                w_u32(((uint32)((v38 + 40))), r_u32((v22 + (10) * 4)));
                v41 = r_u32((v22 + (12) * 4));
                w_u32(((uint32)((v38 + 8))), v18);
                w_u32(((uint32)((v38 + 48))), v41);
                v42 = r_u32(((uint32)(v21)));
                w_u32(((uint32)((v38 + 32))), v19);
                w_u32(((uint32)((v38 + 20))), v42);
                w_u32(((uint32)((v38 + 44))), r_u32(((uint32)((v16 - 14)))));
            }
            if ((((v25 && v24) && r_u16(((uint32)((v16 - 2))))) && r_u16(((uint32)((v16 - 10))))))
            {
                v40 = r_u32(0x800FF668);
                uint32 limit = r_u32(0x800FF374);
                uint32 next_packet = v40 + 52;
                result = limit < next_packet;
                if (result)
                    return result;
                w_u32(0x800FF668, next_packet);
                w_u32(((uint32)(v40)), r_u32((v22 + (13) * 4)));
                w_u32(((uint32)((v40 + 4))), r_u32((v22 + (14) * 4)));
                w_u32(((uint32)((v40 + 12))), r_u32((v22 + (16) * 4)));
                w_u32(((uint32)((v40 + 16))), r_u32((v22 + (17) * 4)));
                w_u32(((uint32)((v40 + 24))), r_u32((v22 + (19) * 4)));
                w_u32(((uint32)((v40 + 28))), r_u32((v22 + (20) * 4)));
                w_u32(((uint32)((v40 + 36))), r_u32((v22 + (22) * 4)));
                w_u32(((uint32)((v40 + 40))), r_u32((v22 + (23) * 4)));
                v43 = r_u32((v22 + (25) * 4));
                w_u32(((uint32)((v40 + 8))), v19);
                w_u32(((uint32)((v40 + 48))), v43);
                v44 = r_u32(((uint32)((v16 - 14))));
                w_u32(((uint32)((v40 + 32))), v20);
                w_u32(((uint32)((v40 + 20))), v44);
                w_u32(((uint32)((v40 + 44))), r_u32(((uint32)((v16 - 6)))));
            }
            if (v38)
            {
                LinkMeshRingRenderPacket(v38, v30);
            }
            if (v40)
            {
                LinkMeshRingRenderPacket(v40, v30);
            }
            v45 = LinkMeshRingRenderPacket(v31, v30);
            w_u32(((uint32)(v33)), ((r_u32(((uint32)(v33))) & 0xFF000000) | (v45 & 0xFFFFFF)));
            w_u32(v30, ((r_u32(v30) & 0xFF000000) | (v33 & 0xFFFFFF)));
        }
        v19 = r_u32(((uint32)((v16 - 14))));
        v20 = r_u32(((uint32)((v16 - 6))));
        v23 = ((sint16)(r_u16(((uint32)((v16 - 18))))));
        v24 = ((sint16)(r_u16(((uint32)((v16 - 10))))));
        v25 = ((sint16)(r_u16(((uint32)((v16 - 2))))));
        v26 = ((sint16)(r_u16(((uint32)(v16)))));
        v16 += 24;
        v18 = r_u32(((uint32)(v21)));
        v21 += 24;
        v15 = (sint32)((uint32)v15 + 1);
    }

    return result;
}

extern "C" uint32 sub_8002948C(uint32 object)
{
    FUNCTION_MARKER(0x8002948C, "SLUS_003.73");
    return MeshRingEffect::Attach(object).MeshRingEffect::Render();
}

extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "beam_segments.h"
#include "model_projection.h"
#include "native_object_storage.h"
#include "game_heap.h"
#include "effect_lifecycle.h"
#include <new>

BeamSegments::BeamSegments(uint32 count, uint32 mode) : RenderableEffect()
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 storage;
    FUNCTION_MARKER(0x800260FC, "SLUS_003.73");
    w_u32(object + 68, 0x800A15B8);
    w_u32(object + 76, count);
    storage = AllocateHeapBlock(count * 36, 0, 1);
    w_u32(object + 80, storage);
    w_u32(object + 72, mode);
}

uint32 BeamSegments::Destroy(uint32 reason)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 storage, result;
    FUNCTION_MARKER(0x8002616C, "SLUS_003.73");
    storage = r_u32(object + 80);
    w_u32(object + 68, 0x800A15B8);
    ReleaseHeapBlock(storage);
    result = RenderableEffect::Destroy(0);
    if (reason & 1)
        result = ReleaseEffectStorage(object);
    return result;
}

uint32 BeamSegments::SetColors(uint32 red, uint32 green, uint32 blue)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result, index = 0, offset = 0, storage;
    FUNCTION_MARKER(0x800261CC, "SLUS_003.73");
    result = r_u32(object + 76);
    if ((sint32)result > 0)
    {
        do
        {
            storage = r_u32(object + 80);
            w_u8(storage + offset + 12, red);
            storage = r_u32(object + 80);
            w_u8(storage + offset + 13, green);
            storage = r_u32(object + 80);
            ++index;
            w_u8(storage + offset + 14, blue);
            result = (sint32)index < (sint32)r_u32(object + 76);
            offset += 36;
        } while (result);
    }
    return result;
}

uint32 BeamSegments::SetSize(uint32 size)
{
    uint32 object = NativeObjectGuestAddress(this);
    uint32 result, index = 0, offset = 0, storage;
    FUNCTION_MARKER(0x8002622C, "SLUS_003.73");
    result = r_u32(object + 76);
    if ((sint32)result > 0)
    {
        do
        {
            storage = r_u32(object + 80);
            ++index;
            w_u16(storage + offset + 16, size);
            result = (sint32)index < (sint32)r_u32(object + 76);
            offset += 36;
        } while (result);
    }
    return result;
}

extern "C" uint32 sub_800260FC(uint32 object, uint32 count, uint32 mode)
{
    FUNCTION_MARKER(0x800260FC, "SLUS_003.73");
    void *storage = AllocateNativeObjectStorage(object, sizeof(BeamSegments));
    new (storage) BeamSegments(count, mode);
    return object;
}

extern "C" uint32 sub_8002616C(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8002616C, "SLUS_003.73");
    return BeamSegments::Attach(object).BeamSegments::Destroy(reason);
}

extern "C" uint32 sub_800261CC(uint32 object, uint32 red, uint32 green, uint32 blue)
{
    FUNCTION_MARKER(0x800261CC, "SLUS_003.73");
    return BeamSegments::Attach(object).SetColors(red, green, blue);
}

extern "C" uint32 sub_8002622C(uint32 object, uint32 size)
{
    FUNCTION_MARKER(0x8002622C, "SLUS_003.73");
    return BeamSegments::Attach(object).SetSize(size);
}

BeamSegments &BeamSegments::Attach(uint32 object)
{
    return *static_cast<BeamSegments *>(FindNativeObjectStorage(object));
}

static sint32 DivideBeamScalar(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : -1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return (sint32)numerator;
    return (sint32)numerator / (sint32)denominator;
}

static uint32 ReadBeamNormal(uint32 guest, const sint32 *native)
{
    return native ? (uint32)*native : r_u32(guest);
}

static void WriteBeamNormal(uint32 guest, sint32 *native, uint32 value)
{
    if (native)
        *native = (sint32)value;
    else
        w_u32(guest, value);
}

uint32 CalculateBeamNormal(uint32 vertices, uint32 guest_x, sint32 *native_x, uint32 guest_y, sint32 *native_y, uint32 minimum_length)
{
    uint32 result, current_x, current_y, next_x, next_y, horizontal, vertical, norm;
    FUNCTION_MARKER(0x80026370, "SLUS_003.73");
    result = r_u32(vertices + 24);
    if (!result)
    {
        result = r_u32(vertices + 52);
        if (!result)
        {
            current_x = (uint32)(sint32)(sint16)r_u16(vertices);
            current_y = (uint32)(sint32)(sint16)r_u16(vertices + 2);
            next_x = (uint32)(sint32)(sint16)r_u16(vertices + 28);
            next_y = (uint32)(sint32)(sint16)r_u16(vertices + 30);
            horizontal = (uint32)DivideBeamScalar((next_x - current_x) * 320, 512);
            vertical = next_y - current_y;
            WriteBeamNormal(guest_x, native_x, vertical);
            WriteBeamNormal(guest_y, native_y, 0 - horizontal);
            if ((sint32)horizontal < 0)
                horizontal = 0 - horizontal;
            if ((sint32)vertical < 0)
                vertical = 0 - vertical;
            if ((sint32)vertical < (sint32)horizontal)
                norm = horizontal + (uint32)((sint32)vertical / 2);
            else
                norm = vertical + (uint32)((sint32)horizontal / 2);
            result = (sint32)norm < (sint32)minimum_length;
            if (!result)
            {
                result = (uint32)DivideBeamScalar(ReadBeamNormal(guest_x, native_x) << 6, norm);
                WriteBeamNormal(guest_x, native_x, result);
                result = (uint32)DivideBeamScalar(ReadBeamNormal(guest_y, native_y) << 6, norm);
                WriteBeamNormal(guest_y, native_y, result);
                result = (uint32)DivideBeamScalar(ReadBeamNormal(guest_x, native_x) << 9, 320);
                WriteBeamNormal(guest_x, native_x, result);
                return result;
            }
        }
    }
    WriteBeamNormal(guest_y, native_y, 0);
    WriteBeamNormal(guest_x, native_x, 0);
    return result;
}

uint32 BeamSegments::Render()
{
    uint32 a1 = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80026488, "SLUS_003.73");
    uint32 v2;
    uint32 v3;
    sint32 v4;
    uint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    uint32 v9;
    uint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    sint32 v15;
    uint32 v16;
    uint32 v17;
    uint32 v18;
    uint32 v19;
    uint32 v20;
    sint32 v21;
    uint32 v22;
    uint32 v23;
    sint32 v24;
    sint32 v25;
    uint32 delta_x, delta_y, previous_x, previous_y, width, current_x, depth, bias, draw_word;
    sint32 v27;
    sint32 v28;
    sint32 v29;
    uint32 v30;
    sint32 v31;
    sint32 v32;
    uint32 v33;
    uint32 v34;
    uint32 v35;
    uint32 v36;
    uint32 v37;
    sint32 v38;
    sint32 v39;
    sint32 v40;
    sint32 v41;
    sint32 v42;
    uint32 v43;
    sint32 v44;
    sint32 result;
    uint32 v46;
    sint32 v47;
    uint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    sint32 v52;
    sint32 v53;
    sint32 v54;
    v2 = 528482304;
    depth = r_u32(a1 + 76);
    v3 = r_u32(((uint32)((a1 + 80))));
    v4 = 0;
    if ((sint32)depth > 0)
    {
        v5 = 528482320;
        do
        {
            v6 = (sint32)ProjectWorldPoint((const uint32 *)psx_addr((uint32)v3, 12), (uint32 *)psx_addr((uint32)v2, 4));
            v7 = (v6 < (sint32)r_u32(0x800FFAE8));
            w_u32(((uint32)((v5 - 4))), v6);
            if ((v7 || ((sint32)r_u32(0x800FFAD8) < v6)))
            {
                w_u32(((uint32)((v5 + 8))), 1);
            }
            else
            {
                w_u32(((uint32)((v5 + 8))), 0);
                w_u32(((uint32)(v5)), DivideBeamScalar(400 * r_u16((uint32)v3 + 16), r_u32((uint32)v5 - 4)));
            }
            v5 += 28;
            v2 += 28;
            ++v4;
            v3 += 36;
        } while ((v4 < (sint32)r_u32(a1 + 76)));
    }
    CalculateBeamNormal(0x1F800000, 0, &v50, 0, &v51, 2);
    v8 = 1;
    v9 = 528482340;
    w_u32(0x1F800004, v50);
    w_u32(0x1F800008, v51);
    v10 = 528482332;
    while ((v8 < (sint32)(r_u32(a1 + 76) - 1)))
    {
        CalculateBeamNormal((uint32)v10, 0, &v52, 0, &v53, 2);
        v10 += 28;
        ++v8;
        v11 = v52;
        v12 = v53;
        v13 = v51;
        w_u32(((uint32)((v9 - 4))), ((sint32)((uint32)v52 + (uint32)v50) / 2));
        w_u32(((uint32)(v9)), ((sint32)((uint32)v12 + (uint32)v13) / 2));
        v9 += 28;
        v50 = v11;
        v51 = v12;
    }

    v14 = v51;
    w_u32(((uint32)((v10 + 4))), v50);
    w_u32(((uint32)((v10 + 8))), v14);
    v15 = 1;
    v16 = 528482304;
    v17 = 528482348;
    v18 = 528482312;
    v19 = 528482332;
    delta_x = r_u32(0x1F800004);
    width = r_u32(0x1F800010);
    v21 = (sint32)(delta_x * width) >> 6;
    delta_y = r_u32(0x1F800008);
    v24 = (sint32)(delta_y * width) >> 6;
    current_x = (uint32)(sint32)(sint16)r_u16(0x1F800000);
    v29 = (sint16)r_u16(0x1F800002);
    v20 = r_u32(a1 + 80);
    v22 = ((uint32)((v20 + 48)));
    v23 = ((uint32)((v20 + 12)));
    v54 = (((uint32)v29 + (uint32)v24) << 16) | ((current_x + (uint32)v21) & 65535);
    v25 = (((uint32)v29 - (uint32)v24) << 16) | ((current_x - (uint32)v21) & 65535);
    while ((v15 < (sint32)r_u32(a1 + 76)))
    {
        delta_x = r_u32(v17 - 12);
        delta_y = r_u32(v17 - 8);
        if (delta_x | delta_y)
        {
            previous_x = r_u32(v18 - 4);
            previous_y = r_u32(v18);
            if (previous_x | previous_y)
            {
                width = r_u32(v17);
                v28 = (sint32)(delta_x * width) >> 6;
                current_x = (uint32)(sint32)(sint16)r_u16(v19);
                v27 = (sint32)(delta_y * width);
                bias = r_u16(a1 + 64);
                depth = r_u32(v17 - 4);
                v29 = (sint32)(short)r_u16((uint32)v19 + 2);
                v30 = r_u32(0x800FF660) + ((depth - bias) & 0x3FFC) + 112;
                v31 = ((((uint32)v29 + (uint32)(v27 >> 6)) << 16) | ((unsigned short)((current_x + v28))));
                v32 = ((((uint32)v29 - (uint32)(v27 >> 6)) << 16) | ((unsigned short)((current_x - v28))));
                v33 = r_u32(0x800FF668);
                v34 = r_u32(0x800FF374);
                v35 = v33 + 36;
                v36 = v33 + 80;
                v37 = v33 + 72;
                if (v34 < v36)
                    break;
                draw_word = r_u32(0x800FF468);
                v38 = r_u32(0x800FF46C);
                w_u32(v37, draw_word);
                w_u32(v37 + 4, v38);
                v39 = r_u32(((uint32)((a1 + 72))));
                w_u32(0x800FF668, v36);
                if ((v39 >= 3))
                {
                    if ((v34 < (v33 + 116)))
                        break;
                    w_u32(0x800FF668, (v33 + 116));
                    w_u32(((uint32)((v33 + 80))), 0x8000000);
                    w_u32(((uint32)((v36 + 4))), 973078528);
                    w_u32(((uint32)((v36 + 12))), 0);
                    w_u32(((uint32)((v36 + 28))), r_u32(v23));
                    w_u32(((uint32)((v36 + 20))), r_u32(v22));
                    w_u32(((uint32)((v36 + 16))), r_u32((v23 + (4) * 4)));
                    w_u32(((uint32)((v36 + 8))), r_u32((v22 + (4) * 4)));
                    w_u32(((uint32)((v36 + 32))), r_u32(((uint32)(v16))));
                    w_u32(((uint32)((v36 + 24))), r_u32(((uint32)(v19))));
                    depth = r_u32(v33 + 80);
                    draw_word = r_u32(v30);
                    w_u32(v33 + 80, (depth & 0xFF000000) | (draw_word & 0xFFFFFF));
                    w_u32(v30, ((r_u32(v30) & 0xFF000000) | (v36 & 0xFFFFFF)));
                }
                w_u32(((uint32)((v33 + 36))), 0x8000000);
                w_u32(((uint32)(v33)), 0x8000000);
                w_u32(((uint32)((v35 + 4))), 973078528);
                w_u32(((uint32)((v33 + 4))), 973078528);
                w_u32(((uint32)((v33 + 12))), 0);
                draw_word = r_u32(v23);
                v40 = r_u32(((uint32)(v33)));
                w_u32(((uint32)((v33 + 28))), draw_word);
                v41 = r_u32(v22);
                w_u32(((uint32)((v33 + 8))), v31);
                w_u32(((uint32)((v33 + 16))), v54);
                w_u32(((uint32)((v33 + 20))), v41);
                w_u32(((uint32)((v33 + 24))), r_u32(((uint32)(v19))));
                w_u32(((uint32)((v33 + 32))), r_u32(((uint32)(v16))));
                w_u32(((uint32)(v33)), ((v40 & 0xFF000000) | (r_u32(v30) & 0xFFFFFF)));
                w_u32(v30, ((r_u32(v30) & 0xFF000000) | (v33 & 0xFFFFFF)));
                w_u32(((uint32)((v35 + 12))), 0);
                w_u32(((uint32)((v35 + 28))), r_u32(v23));
                v42 = r_u32(v22);
                w_u32(((uint32)((v35 + 8))), v32);
                w_u32(((uint32)((v35 + 16))), v25);
                w_u32(((uint32)((v35 + 20))), v42);
                w_u32(((uint32)((v35 + 24))), r_u32(((uint32)(v19))));
                v25 = v32;
                w_u32(((uint32)((v35 + 32))), r_u32(((uint32)(v16))));
                depth = r_u32(v33 + 36);
                draw_word = r_u32(v30);
                w_u32(v33 + 36, (depth & 0xFF000000) | (draw_word & 0xFFFFFF));
                v43 = ((r_u32(v30) & 0xFF000000) | (v35 & 0xFFFFFF));
                w_u32(v30, v43);
                v44 = r_u32(((uint32)((v33 + 72))));
                v54 = v31;
                w_u32(((uint32)((v33 + 72))), ((v44 & 0xFF000000) | (v43 & 0xFFFFFF)));
                w_u32(v30, ((r_u32(v30) & 0xFF000000) | (v37 & 0xFFFFFF)));
            }
        }
        v18 += 28;
        v16 += 28;
        v17 += 28;
        v19 += 28;
        v23 += (9) * 4;
        v22 += (9) * 4;
        ++v15;
    }

    result = r_u32(((uint32)((a1 + 72))));
    v46 = 528482304;
    if (result)
    {
        v47 = 0;
        depth = r_u32(a1 + 76);
        v48 = r_u32(a1 + 80) + 24;
        if ((sint32)depth > 0)
        {
            do
            {
                if ((r_u32(((uint32)((a1 + 72)))) == 1))
                {
                    v49 = r_u32(((uint32)(v46)));
                    w_u32(v48, v49);
                    w_u32((v48 + (1) * 4), v49);
                }
                else
                {
                    w_u32((v48 + (1) * 4), r_u32(v48));
                    v49 = r_u32(v46);
                    w_u32(v48, v49);
                }
                v48 += (9) * 4;
                ++v47;
                v46 += 28;
            } while ((v47 < (sint32)r_u32(a1 + 76)));
        }
        result = (r_u32(((uint32)((a1 + 72)))) + 1);
        w_u32(((uint32)((a1 + 72))), result);
    }
    return result;
}

extern "C" uint32 sub_80026488(uint32 object)
{
    FUNCTION_MARKER(0x80026488, "SLUS_003.73");
    return BeamSegments::Attach(object).BeamSegments::Render();
}

extern "C" uint32 apocalypse_segment_normal(uint32 vertices, sint32 *x, sint32 *y, uint32 minimum_length)
{
    return CalculateBeamNormal(vertices, 0, x, 0, y, minimum_length);
}

extern "C" uint32 sub_80026370(uint32 vertices, uint32 x, uint32 y, uint32 minimum_length)
{
    FUNCTION_MARKER(0x80026370, "SLUS_003.73");
    return CalculateBeamNormal(vertices, x, 0, y, 0, minimum_length);
}

extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
#include "game_mdec.h"
#include "game_movie_startup.h"
#include "game_gpu_present.h"
#include "game_str_vlc.h"
}
#include "movie_lifecycle.h"
#include "startup_resources.h"
#include "game_random.h"
#include "vector_distance.h"
#include "cd_voice_event.h"
#include <stdio.h>
#include <stdlib.h>
#include "model_resources.h"
#include "native_object_storage.h"
#include "game_heap.h"
#include "psx_spu.h"

uint32 ResetMoviePlayback()
{
    uint8 volume[8], location[8];
    sint16 rectangle[4];
    uint32 pending, active, index, limit, pointer, x, y, width, height;
    FUNCTION_MARKER(0x8002F130, "SLUS_003.73");
    pending = r_u32(0x800FF254);
    w_u32(0x800FF294, 0);
    if (pending)
        w_u32(0x800FF254, 0);
    active = r_u32(0x800FF250);
    if (!active)
        return active;
    volume[0] = 0;
    volume[1] = 0;
    volume[2] = 0;
    volume[3] = 0;
    xport_draft_host_sub_800981B4_p1(volume);
    w_u32(0x800FF250, 0);
    sub_8009BED8(0);
    sub_8009AD7C();
    xport_draft_host_sub_80098258_p2(r_u32(0x800FF278), location);
    xport_draft_host_sub_80097DF8_p2(22, location, 0);
    if (r_u32(0x800FFB84))
        w_u32(0x800FFB84, 0);
    if (r_u32(0x800FFB88))
        w_u32(0x800FFB88, 0);
    limit = r_u8(0x800FF288);
    for (index = 0; index < limit; ++index)
    {
        pointer = 0x800A5F68 + index * 4;
        if (r_u32(pointer))
            w_u32(pointer, 0);
    }
    for (index = 0; index < 2; ++index)
    {
        pointer = 0x800FF2A4 + index * 4;
        if (r_u32(pointer))
            w_u32(pointer, 0);
    }
    x = r_u16(0x800FF28A);
    y = r_u16(0x800FF28C);
    width = r_u16(0x800FF290);
    height = r_u16(0x800FF292);
    rectangle[0] = (sint16)x;
    rectangle[1] = (sint16)y;
    rectangle[2] = (sint16)width;
    rectangle[3] = (sint16)height;
    return xport_draft_host_sub_8008847C_p1(rectangle, 1, 1, 1);
}

void ReleaseMovieFrameBuffers()
{
    uint32 allocation;
    FUNCTION_MARKER(0x8002E2B8, "SLUS_003.73");
    allocation = r_u32(0x800FF230);
    if (allocation)
    {
        ReleaseHeapBlock(allocation);
        w_u32(0x800FF230, 0);
        w_u32(0x800FF234, 0);
    }
}

extern "C" uint32 sub_8002F130(void)
{
    FUNCTION_MARKER(0x8002F130, "SLUS_003.73");
    return ResetMoviePlayback();
}

extern "C" void sub_8002E2B8(void)
{
    FUNCTION_MARKER(0x8002E2B8, "SLUS_003.73");
    ReleaseMovieFrameBuffers();
}

uint32 StartMovieStream(void)
{
    FUNCTION_MARKER(0x8002FA48, "SLUS_003.73");
    CdlLOC location;
    uint8 mode = 0x80;
    CdIntToPos((sint32)r_u32(0x800FF278), &location);
    uint32 result;
    do
    {
        while (!xport_draft_host_sub_80097DF8_p2(2, &location, 0))
            ;
        while (!xport_draft_host_sub_80097DF8_p2(14, &mode, 0))
            ;
        result = apocalypse_str_start(480);
    } while (!result);
    return result;
}

extern "C" uint32 sub_8002FA48(void)
{
    FUNCTION_MARKER(0x8002FA48, "SLUS_003.73");
    return StartMovieStream();
}

static uint32 ReadMovieInputByte(const void *input, uint32 offset)
{
    uint32 address = GuestMemoryAddress(input);
    return address ? r_u8(address + offset) : ((const uint8 *)input)[offset];
}

static uint32 ReadMovieInputHalf(const void *input, uint32 offset)
{
    uint32 address = GuestMemoryAddress(input);
    return address ? r_u16(address + offset) : xport_load_le16((const uint8 *)input + offset);
}

static uint32 ReadMovieInputWord(const void *input, uint32 offset)
{
    uint32 address = GuestMemoryAddress(input);
    return address ? r_u32(address + offset) : xport_load_le32((const uint8 *)input + offset);
}

uint32 RemapMovieTexturePolygons(uint32 tpage, const uint8 *texture)
{
    FUNCTION_MARKER(0x8002F50C, "SLUS_003.73");
    for (uint32 slot = 0; slot < 2; ++slot)
    {
        uint32 model = r_u32(0x800FF778 + slot * 4);
        if (model == 0xFFFFFFFF)
            continue;
        uint32 objects = r_u32(0x800EAEF8 + (model << 6) + 16);
        uint32 count = r_u32(objects - 4);
        for (uint32 index = 0; index < count; ++index, objects += 4)
        {
            uint32 geometry = r_u32(objects);
            uint32 vertices = r_u32(geometry + 4);
            uint32 polygons = r_u32(geometry + 12);
            uint32 normals = r_u32(geometry + 8);
            uint32 polygon = geometry + (vertices << 3) + 32 + (normals << 3);
            for (uint32 item = 0; item < polygons; ++item)
            {
                uint32 flags = r_u32(polygon);
                if ((flags & 1) && (flags & 2))
                {
                    uint32 page = r_u16(polygon + 26);
                    uint32 resource_page = ReadMovieInputHalf(texture, 6);
                    if (page == resource_page)
                    {
                        uint32 clut = r_u16(polygon + 22);
                        uint32 resource_clut = ReadMovieInputHalf(texture, 2);
                        if (clut == resource_clut)
                        {
                            uint32 u = r_u8(polygon + 20);
                            uint32 min_u = ReadMovieInputByte(texture, 0);
                            if (u >= min_u)
                            {
                                uint32 max_u = ReadMovieInputByte(texture, 4);
                                if (u <= max_u)
                                {
                                    u = r_u8(polygon + 24);
                                    if (u >= min_u && u <= max_u)
                                    {
                                        u = r_u8(polygon + 28);
                                        if (u >= min_u && u <= max_u)
                                        {
                                            u = r_u8(polygon + 30);
                                            if (u >= min_u && u <= max_u)
                                            {
                                                uint32 v = r_u8(polygon + 21);
                                                uint32 min_v = ReadMovieInputByte(texture, 1);
                                                if (v >= min_v)
                                                {
                                                    uint32 max_v = ReadMovieInputByte(texture, 9);
                                                    if (v <= max_v)
                                                    {
                                                        v = r_u8(polygon + 25);
                                                        if (v >= min_v && v <= max_v)
                                                        {
                                                            v = r_u8(polygon + 29);
                                                            if (v >= min_v && v <= max_v)
                                                            {
                                                                v = r_u8(polygon + 31);
                                                                if (v >= min_v && v <= max_v)
                                                                {
                                                                    w_u16(polygon + 26, tpage);
                                                                    w_u8(polygon + 20, (r_u8(polygon + 20) + 1) >> 1);
                                                                    uint32 u1 = r_u8(polygon + 24);
                                                                    uint32 u2 = r_u8(polygon + 28);
                                                                    w_u8(polygon + 24, (u1 + 1) >> 1);
                                                                    uint32 u3 = r_u8(polygon + 30);
                                                                    w_u8(polygon + 28, (u2 + 1) >> 1);
                                                                    w_u8(polygon + 30, (u3 + 1) >> 1);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                polygon += r_u16(polygon + 2) & 0xFFFC;
            }
        }
    }
    return 8;
}

uint32 PrepareMovieTexture(uint32 resource)
{
    FUNCTION_MARKER(0x8002E600, "SLUS_003.73");
    uint32 active = r_u32(0x800FF250);
    if (active)
        return active;
    uint32 texture = FindModelResourceEntry(resource);
    uint32 x = r_u16(texture + 28);
    uint32 y = r_u16(texture + 30);
    uint32 width = r_u16(0x800FF290);
    uint32 height = r_u16(0x800FF292);
    sint16 rectangle[4];
    rectangle[2] = (sint16)width;
    rectangle[3] = (sint16)height;
    w_u16(0x800FF28A, x);
    w_u16(0x800FF28C, y);
    rectangle[0] = (sint16)x;
    rectangle[1] = (sint16)y;
    xport_draft_host_sub_8008847C_p1(rectangle, 1, 1, 1);
    uint32 wide = r_u8(0x800FF298);
    x = r_u16(0x800FF28A);
    width = r_u16(0x800FF290);
    if (wide)
        width = (3 * width) >> 1;
    y = r_u16(0x800FF28C);
    w_u16(0x800FF28E, x + width);
    x = r_u16(0x800FF28A);
    uint32 tpage = ((y & 0x100) >> 4) | ((x & 0x3FF) >> 6) | 0x100 | ((y & 0x200) << 2);
    return RemapMovieTexturePolygons(tpage, (const uint8 *)psx_addr(texture, 10));
}

uint32 QueueMoviePosition(const uint32 *position)
{
    FUNCTION_MARKER(0x8002E6D8, "SLUS_003.73");
    uint32 result = r_u32(0x800FF250);
    if (result)
        return result;
    uint32 count = r_u32(0x800FF294);
    result = (sint32)count < 8;
    if (!result)
        return result;
    // TODO: Original negative counts address before the position array
    uint32 target = 0x800A5B2C + count * 12;
    uint32 x = ReadMovieInputWord(position, 0);
    uint32 y = ReadMovieInputWord(position, 4);
    uint32 z = ReadMovieInputWord(position, 8);
    w_u32(target, x);
    w_u32(target + 4, y);
    w_u32(target + 8, z);
    result = count + 1;
    w_u32(0x800FF294, result);
    return result;
}

extern "C" uint32 sub_8002E600(uint32 resource)
{
    FUNCTION_MARKER(0x8002E600, "SLUS_003.73");
    return PrepareMovieTexture(resource);
}
extern "C" uint32 sub_8002E6D8(uint32 position)
{
    FUNCTION_MARKER(0x8002E6D8, "SLUS_003.73");
    return QueueMoviePosition((const uint32 *)psx_addr(position, 12));
}
extern "C" uint32 sub_8002F50C(uint32 tpage, uint32 texture)
{
    FUNCTION_MARKER(0x8002F50C, "SLUS_003.73");
    return RemapMovieTexturePolygons(tpage, (const uint8 *)psx_addr(texture, 10));
}

uint32 PrepareMoviePlaybackBuffers(void)
{
    FUNCTION_MARKER(0x8002EF68, "SLUS_003.73");
    uint32 result = r_u32(0x800A5B8C + 28 * r_u8(0x800FF299) + 24);
    uint32 remaining, base, cursor, size, stride, table, count, i, x, y, wide, height;
    if (!result)
        return result;
    result = r_u32(0x800FF250);
    if (result)
        return result;
    x = r_u16(0x800FF28A);
    y = r_u16(0x800FF28C);
    wide = r_u8(0x800FF298);
    w_u16(0x800FF29C, x);
    w_u16(0x800FF29E, y);
    height = r_u16(0x800FF292);
    w_u16(0x800FF2A0, wide ? 24 : 16);
    w_u32(0x800FF26C, 0);
    w_u32(0x800FF248, 0);
    w_u32(0x800FF258, 0);
    w_u32(0x800FF25C, 0);
    w_u32(0x800FF260, 0);
    w_u16(0x800FF2A2, height);
    base = FindTemporaryHeapSpace(&remaining);
    if (remaining <= 0x11000)
        goto exhausted;
    w_u32(0x800FFB88, base);
    cursor = base + 0x11000;
    table = 0x800FF2A4;
    size = r_u32(0x800FF280);
    stride = size & 0xFFFFFFFC;
    remaining -= 0x11000;
    for (i = 0; i < 2; ++i)
    {
        if (size >= remaining)
            goto exhausted;
        w_u32(table, cursor);
        cursor += stride;
        table += 4;
        remaining -= size;
    }
    size = r_u32(0x800FF284) << 11;
    if (size >= remaining)
        goto exhausted;
    w_u32(0x800FFB84, cursor);
    cursor += size;
    count = (r_u16(0x800FF290) >> 4) & 255;
    w_u8(0x800FF288, count);
    remaining -= size;
    table = 0x800A5F68;
    stride = count ? r_u16(0x800FF292) << 5 : 0;
    for (i = 0; i < count; ++i)
    {
        if (stride >= remaining)
            goto exhausted;
        w_u32(table, cursor);
        cursor += stride;
        remaining -= stride;
        table += 4;
    }
    sub_8009C65C(r_u32(0x800FFB88));
    w_u32(0x800FF250, 1);
    w_u32(0x800FF24C, 0);
    InitializeMovieDecoder();
    return DecodeNextMovieFrame();
exhausted:
    w_u32(0x800FF254, 1);
    return 1;
}

uint32 InitializeMovieDecoder(void)
{
    FUNCTION_MARKER(0x8002F7C8, "SLUS_003.73");
    sub_8009BC3C(0);
    sub_8009BED8(0x8002F84C);
    uint32 ring = r_u32(0x800FFB84);
    uint32 sectors = r_u32(0x800FF284);
    sub_80097B1C(ring, sectors);
    sub_8009AEEC(r_u8(0x800FF298), 0, 0xFFFFFFFF, 0, 0);
    w_u32(0x800FF2B8, 0);
    w_u32(0x800FF2BC, 1);
    w_u32(0x800FF2C0, 0);
    StartMovieStream();
    return sub_80097F34(12, 0);
}

uint32 NextMovieFrame(void)
{
    FUNCTION_MARKER(0x8002F990, "SLUS_003.73");
    uint32 frame, header;
    if (apocalypse_str_next(&frame, &header))
        return 0;
    uint32 last_frame = r_u32(0x800FF27C);
    uint32 number = r_u32(header + 8);
    w_u32(0x800FF274, number);
    if ((sint32)number >= (sint32)last_frame)
        w_u32(0x800FF24C, 1);
    return frame;
}

uint32 DecodeNextMovieFrame(void)
{
    FUNCTION_MARKER(0x8002F9E4, "SLUS_003.73");
    uint32 frame = NextMovieFrame();
    if (!frame)
        return 0;
    uint32 index = r_u32(0x800FF25C);
    uint32 table = r_u32(0x800FFB88);
    uint32 output = r_u32(0x800FF2A4 + index * 4);
    apocalypse_str_vlc_decode(frame, output, table);
    apocalypse_str_free(frame);
    return 1;
}

extern "C" uint32 sub_8002EF68(void)
{
    FUNCTION_MARKER(0x8002EF68, "SLUS_003.73");
    return PrepareMoviePlaybackBuffers();
}
extern "C" uint32 sub_8002F7C8(void)
{
    FUNCTION_MARKER(0x8002F7C8, "SLUS_003.73");
    return InitializeMovieDecoder();
}
extern "C" uint32 sub_8002F990(void)
{
    FUNCTION_MARKER(0x8002F990, "SLUS_003.73");
    return NextMovieFrame();
}
extern "C" uint32 sub_8002F9E4(void)
{
    FUNCTION_MARKER(0x8002F9E4, "SLUS_003.73");
    return DecodeNextMovieFrame();
}

uint32 OutputMovieSlice(void)
{
    FUNCTION_MARKER(0x8002F84C, "SLUS_003.73");
    if (r_u8(0x800FF298) && r_u32(0x8010581C))
    {
        apocalypse_str_pump();
        w_u32(0x8010581C, 0);
    }
    uint32 index = r_u32(0x800FF258);
    uint32 slices = r_u8(0x800FF288);
    uint32 rectangle[2];
    rectangle[0] = r_u32(0x800FF29C);
    rectangle[1] = r_u32(0x800FF2A0);
    uint32 next_index = index + 1;
    w_u32(0x800FF258, next_index);
    uint32 pixels = r_u32(0x800A5F68 + index * 4);
    if ((sint32)next_index >= (sint32)slices)
        w_u32(0x800FF258, 0);
    uint32 x = r_u16(0x800FF29C);
    uint32 width = r_u16(0x800FF2A0);
    uint32 end_x = r_u16(0x800FF28E);
    x += width;
    w_u16(0x800FF29C, x);
    if ((sint32)(sint16)x < (sint32)end_x)
    {
        uint32 height = (uint32)(sint32)(sint16)r_u16(0x800FF2A2);
        uint32 product = (uint32)(sint32)(sint16)width * height;
        next_index = r_u32(0x800FF258);
        uint32 output = r_u32(0x800A5F68 + next_index * 4);
        uint32 words = (uint32)((sint32)(product + (product >> 31)) >> 1);
        sub_8009BE1C(output, words);
    }
    else
    {
        uint32 origin_x = r_u16(0x800FF28A);
        uint32 double_buffer = r_u32(0x800FF248);
        w_u16(0x800FF29C, origin_x);
        if (double_buffer)
        {
            sint32 signed_y = (sint16)r_u16(0x800FF29E);
            uint32 y = r_u16(0x800FF29E);
            w_u16(0x800FF29E, signed_y < 256 ? y + 256 : y - 256);
        }
    }
    return xport_draft_host_sub_800885A4_p1((const sint16 *)rectangle, pixels);
}

extern "C" uint32 sub_8002F84C(void)
{
    FUNCTION_MARKER(0x8002F84C, "SLUS_003.73");
    return OutputMovieSlice();
}

uint32 UpdateMoviePlayback(void)
{
    FUNCTION_MARKER(0x8002F284, "SLUS_003.73");
    uint32 result = r_u32(0x800FF250);
    if (!result)
        return result;
    if (!r_u32(0x800FF26C))
    {
        if (r_u32(0x800FF24C))
        {
            uint32 double_buffer = r_u32(0x800FF248);
            w_u32(0x800FF24C, 0);
            if (!double_buffer && !r_u32(0x800FF378))
            {
                uint32 selected = RandomRange(4) == 1 ? 31 : RandomRange(11) + 4;
                uint32 entry;
                w_u8(0x800FF299, selected);
                entry = 0x800A5B8C + 28 * r_u8(0x800FF299);
                uint32 last_frame = r_u16(entry + 8);
                uint32 sector = r_u32(entry + 24);
                w_u32(0x800FF27C, last_frame);
                w_u32(0x800FF278, sector);
            }
            StartMovieStream();
        }
        if (r_u32(0x800FF260))
        {
            uint32 words;
            uint32 output;
            sub_8009BDA0(r_u32(0x800FF2A4 + r_u32(0x800FF25C) * 4), 0);
            uint32 width = (uint32)(sint32)(sint16)r_u16(0x800FF2A0);
            uint32 height = (uint32)(sint32)(sint16)r_u16(0x800FF2A2);
            words = width * height;
            output = r_u32(0x800A5F68 + r_u32(0x800FF258) * 4);
            w_u32(0x800FF25C, 1 - r_u32(0x800FF25C));
            sub_8009BE1C(output, (uint32)((sint32)(words + (words >> 31)) >> 1));
        }
        uint32 decoded = DecodeNextMovieFrame();
        uint32 timer = r_u32(0x800FF270);
        w_u32(0x800FF260, decoded);
        if (timer)
        {
            uint32 next = timer - 1;
            w_u32(0x800FF270, next);
            if (!next)
            {
                uint32 index = 0, selected = 0, point = 0x800A5B2C;
                sint32 best = 0xFFFFFF;
                while ((sint32)index < (sint32)r_u32(0x800FF294))
                {
                    sint32 distance = (sint32)MeasureEuclideanDistance((const uint32 *)psx_addr(r_u32(0x800FF904) + 4, 12), (const uint32 *)psx_addr(point, 12));
                    if (distance < best)
                    {
                        best = distance;
                        selected = index;
                    }
                    point += 12;
                    ++index;
                }
                UpdateCdVoiceBalance((const uint32 *)psx_addr(0x800A5B2C + 12 * selected, 12));
                w_u32(0x800FF270, 4);
            }
        }
    }
    if (r_u32(0x800FF008) || r_u32(0x800FF300))
    {
        if (r_u32(0x800FF26C))
            return 1;
        w_u32(0x800FF26C, 1);
        sub_8009B12C(1, 0, 0xFFFFFFFF);
        return apocalypse_str_pause();
    }
    result = r_u32(0x800FF26C);
    if (result)
    {
        w_u32(0x800FF26C, 0);
        sub_8009B12C(0, 0, 0xFFFFFFFF);
        return apocalypse_str_resume(480);
    }
    return result;
}

extern "C" uint32 sub_8002F284(void)
{
    FUNCTION_MARKER(0x8002F284, "SLUS_003.73");
    return UpdateMoviePlayback();
}

uint32 SelectMovieRecord(uint32 movie)
{
    FUNCTION_MARKER(0x8002EE7C, "SLUS_003.73");
    uint32 result = r_u32(0x800FF250);
    if (result)
        return result;
    movie &= 255;
    if (movie == 1)
        movie = 4;
    else if (movie == 26)
    {
        uint32 choice = RandomRange(5);
        movie = 0;
        if (choice == 1)
            movie = 27;
        else if (choice == 2)
            movie = 29;
        else if (choice == 3)
            movie = 30;
        else if (choice)
            movie = 32;
    }
    uint32 entry = 0x800A5B8C + 28 * movie;
    uint32 x = r_u16(entry + 4);
    uint32 y = r_u16(entry + 6);
    uint32 end_frame = r_u16(entry + 8);
    uint32 first_position = r_u32(entry + 12);
    uint32 second_position = r_u32(entry + 16);
    w_u16(0x800FF290, x);
    uint32 start_frame = r_u32(entry + 24);
    w_u8(0x800FF299, movie);
    w_u16(0x800FF292, y);
    w_u32(0x800FF27C, end_frame);
    w_u32(0x800FF280, first_position);
    w_u32(0x800FF284, second_position);
    result = r_u8(entry + 20);
    w_u32(0x800FF278, start_frame);
    w_u8(0x800FF298, result);
    return result;
}

extern "C" uint32 sub_8002EE7C(uint32 movie)
{
    FUNCTION_MARKER(0x8002EE7C, "SLUS_003.73");
    return SelectMovieRecord(movie);
}

uint32 PlayMovie(uint32 movie)
{
    DISPENV environment;
    uint8 volume[4], status[8];
    uint32 i, result = r_u32(0x800FF250);
    sint32 timeout;
    uint16 screen_width;
    uint32 width, height, left, top, mode, volume_setting;
    FUNCTION_MARKER(0x8002E814, "SLUS_003.73");
    if (result)
        return result;
    movie &= 255;
    w_u32(0x800FF248, 1);
    SelectMovieRecord(movie);
    width = r_u16(0x800FF290);
    w_u16(0x800FF28A, (320 - (sint32)width) / 2);
    mode = r_u8(0x800FF298);
    left = r_u16(0x800FF28A);
    height = r_u16(0x800FF292);
    i = r_u8(0x800FF299);
    w_u16(0x800FF28E, left + (mode ? 3 * width / 2 : width));
    w_u32(0x800FF24C, 0);
    top = (uint32)((240 - (sint32)height) / 2);
    result = r_u32(0x800A5B8C + 28 * i + 24);
    w_u16(0x800FF28C, top);
    if (!result)
        return result;
    left = r_u16(0x800FF28A);
    mode = r_u8(0x800FF298);
    w_u16(0x800FF29E, top);
    w_u16(0x800FF2A2, height);
    w_u16(0x800FF29C, left);
    screen_width = r_u16(0x800A7308);
    w_u16(0x800FF2A0, mode ? 24 : 16);
    w_u32(0x800FF258, 0);
    w_u32(0x800FF25C, 0);
    w_u32(0x800FF260, 0);
    w_u32(0x800FFB88, sub_8006B864(69632, 1, 0));
    if (!r_u32(0x800FFB88))
        goto cleanup;
    for (i = 0; i < 2; ++i)
    {
        w_u32(0x800FF2A4 + 4 * i, sub_8006B864(r_u32(0x800FF280), 1, 0));
        if (!r_u32(0x800FF2A4 + 4 * i))
            goto cleanup;
    }
    w_u32(0x800FFB84, sub_8006B864(r_u32(0x800FF284) << 11, 1, 0));
    if (!r_u32(0x800FFB84))
        goto cleanup;
    w_u8(0x800FF288, 2);
    for (i = 0; i < r_u8(0x800FF288); ++i)
    {
        w_u32(0x800A5F68 + 4 * i, sub_8006B864((r_u8(0x800FF298) ? 48 : 32) * r_u16(0x800FF292), 1, 0));
        if (!r_u32(0x800A5F68 + 4 * i))
            goto cleanup;
    }
    sub_8009C65C(r_u32(0x800FFB88));
    w_u32(0x800FF250, 1);
    ClearStartupImageArea();
    SetDefDispEnv(&environment, 0, 256, 320, 240);
    VSync(0);
    PutDispEnv(&environment);
    if (movie == 3)
        volume_setting = 192;
    else
    {
        sint32 setting = (sint16)r_u16(0x800ECC78);
        volume_setting = setting >= 64 ? (uint32)(63 * (setting - 64) / 64 + 192) : (uint32)(3 * setting);
    }
    volume[0] = volume[1] = (uint8)volume_setting;
    volume[2] = volume[3] = 0;
    xport_draft_host_sub_800981B4_p1(volume);
    sub_8002F7C8();
    sub_80070748();
    w_u8(0x800EC1D9, 0);
    w_u8(0x800EC129, 0);
    if (r_u32(0x800FF310))
    {
        w_u8(0x800EC1E9, 0);
        for (i = 0; i < 8; ++i)
            if (i != 3)
                w_u8(0x800EC0F9 + 16 * i, 0);
    }
    while (!sub_8002F9E4())
    {
    }
    for (;;)
    {
        sub_8009BDA0(r_u32(0x800FF2A4 + 4 * r_u32(0x800FF25C)), r_u8(0x800FF298) != 0);
        sub_8009BE1C(r_u32(0x800A5F68 + 4 * r_u32(0x800FF258)), (sint16)r_u16(0x800FF2A0) * (sint16)r_u16(0x800FF2A2) / 2);
        xport_draft_host_sub_80097DF8_p3(1, 0, status);
        if (status[0] & 16)
        {
            WaitForMovieCdStop();
            break;
        }
        timeout = 0x800000;
        while (!sub_8002F9E4())
            if (timeout-- == 0)
                goto finish;
        if (!timeout)
            break;
        timeout = 0x800000;
        // Native callback progress requires fresh coordinates instead of the original fixed snapshot
        while ((sint16)r_u16(0x800FF29C) != r_u16(0x800FF28A))
            if (--timeout == 0)
                goto finish;
        SetDefDispEnv(&environment, 0, (sint16)r_u16(0x800FF29E) >= 256 ? (sint16)r_u16(0x800FF29E) - 256 : (sint16)r_u16(0x800FF29E) + 256, r_u8(0x800FF298) ? 480 : 320, 240);
        if (r_u8(0x800FF298))
        {
            environment.isrgb24 = 1;
            environment.disp.w = 2 * environment.disp.w / 3;
        }
        VSync(0);
        PutDispEnv(&environment);
        // Native scanout follows the completed movie buffer switch
        apocalypse_gpu_present();
        if (r_u32(0x800FF24C))
            break;
        sub_80070748();
        if (r_u8(0x800EC1D9) || r_u8(0x800EC129))
            break;
        if (r_u32(0x800FF310))
        {
            if (r_u8(0x800EC1E9))
                break;
            for (i = 0; i < 8; ++i)
                if (i != 3 && r_u8(0x800EC0F9 + 16 * i))
                    break;
            if (i < 8)
                break;
        }
    }
finish:
    sub_80070100(0x800EC0F8);
    w_u32(0x800FF250, 0);
    ClearStartupImageArea();
    environment.isrgb24 = 0;
    environment.disp.w = 512;
    environment.screen.w = screen_width;
    VSync(0);
    PutDispEnv(&environment);
    volume[0] = volume[1] = volume[2] = volume[3] = 0;
    xport_draft_host_sub_800981B4_p1(volume);
    sub_8009BED8(0);
    apocalypse_str_stop();
    xport_draft_host_sub_80097DF8_p2(9, 0, 0);
cleanup:
    if (r_u32(0x800FFB84))
    {
        sub_8006BC20(r_u32(0x800FFB84));
        w_u32(0x800FFB84, 0);
    }
    if (r_u32(0x800FFB88))
    {
        sub_8006BC20(r_u32(0x800FFB88));
        w_u32(0x800FFB88, 0);
    }
    for (i = 0; i < 2; ++i)
        if (r_u32(0x800FF2A4 + 4 * i))
        {
            sub_8006BC20(r_u32(0x800FF2A4 + 4 * i));
            w_u32(0x800FF2A4 + 4 * i, 0);
        }
    result = r_u8(0x800FF288);
    for (i = 0; i < r_u8(0x800FF288); ++i)
    {
        if (r_u32(0x800A5F68 + 4 * i))
        {
            sub_8006BC20(r_u32(0x800A5F68 + 4 * i));
            w_u32(0x800A5F68 + 4 * i, 0);
        }
        result = i + 1 < r_u8(0x800FF288);
    }
    return result;
}

extern "C" uint32 apocalypse_play_movie(uint32 movie) { return PlayMovie(movie); }
extern "C" uint32 sub_8002E814(uint32 movie)
{
    FUNCTION_MARKER(0x8002E814, "SLUS_003.73");
    return PlayMovie(movie);
}

uint32 WaitForMovieCdStop()
{
    FUNCTION_MARKER(0x8002F758, "SLUS_003.73");
    uint8 status[8];
    do
    {
        xport_draft_host_sub_80097DF8_p3(1, 0, status);
    } while (status[0] & 16);
    xport_draft_host_sub_80097DF8_p2(19, 0, 0);
    while (!(status[0] & 2))
        xport_draft_host_sub_80097DF8_p3(1, 0, status);
    return status[0] & 2;
}

extern "C" uint32 sub_8002F758(void)
{
    FUNCTION_MARKER(0x8002F758, "SLUS_003.73");
    return WaitForMovieCdStop();
}


uint32 InitializeMovieResourceTable()
{
    uint8 location[24];
    uint32 index, entry, value;
    FUNCTION_MARKER(0x8002E738, "SLUS_003.73");
    for (index = 0; index < 34; ++index)
    {
        entry = 0x800A5B8C + 28 * r_u8(0x800A5F44 + index);
        if (r_u32(entry + 24) != 0)
        {
            if (xport_draft_host_sub_80099B3C_p1(location, r_u32(entry)) != 0)
                value = xport_draft_host_sub_8009835C_p1(location);
            else
                value = 0;
            entry = 0x800A5B8C + 28 * r_u8(0x800A5F44 + index);
            w_u32(entry + 24, value);
        }
    }
    return 0;
}

extern "C" uint32 sub_8002E738(void)
{
    FUNCTION_MARKER(0x8002E738, "SLUS_003.73");
    return InitializeMovieResourceTable();
}

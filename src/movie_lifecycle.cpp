#include <stdio.h>
#include <stdlib.h>

#include "game_image_resource.h"
#include "game_diagnostics.h"
#include "level_session.h"
#include "scene_lifecycle.h"
#include "game_settings.h"

#include "game_pad_poll.h"

#include "visual_object.h"
extern "C"
{
#include "game_gpu_present.h"
}
#include "psx_press.h"
#include "movie_stream.h"
#include "movie_lifecycle.h"
static uint32 WaitForMovieCdStop();
static uint32 OutputMovieSlice(void);
static uint32 DecodeNextMovieFrame(void);
static uint32 InitializeMovieDecoder(void);
#include "startup_resources.h"
#include "game_random.h"
#include "vector_distance.h"
#include "cd_voice_event.h"
#include "model_resources.h"
#include "game_heap.h"
#include "record_commands.h"
#include "psx_spu.h"
#include "game_data_io.h"

struct MovieResource
{
    const char *name;
    uint16 width;
    uint16 height;
    uint16 last_frame;
    uint32 decode_bytes;
    uint32 ring_sectors;
    uint8 rgb24;
    uint32 initial_sector;
};

static const MovieResource movie_resources[MovieRecordCount] = {{"\\SHRTPOE.STR;1", 96, 64, 1840, 16384, 16, 0, 1},
                                                                {"DUMMY", 0, 0, 0, 0, 0, 0, 0},
                                                                {"\\CINEMAS\\INTRO.STR;1", 320, 240, 1955, 153600, 48, 1, 1},
                                                                {"\\CINEMAS\\ATVILOGO.STR;1", 320, 240, 208, 153600, 48, 1, 1},
                                                                {"\\WDN1B.STR;1", 96, 64, 259, 16384, 16, 0, 1},
                                                                {"\\WDN2A.STR;1", 96, 64, 207, 16384, 16, 0, 1},
                                                                {"\\WDN3B.STR;1", 96, 64, 247, 16384, 16, 0, 1},
                                                                {"\\WDN4B.STR;1", 96, 64, 301, 16384, 16, 0, 1},
                                                                {"\\WDN5B.STR;1", 96, 64, 143, 16384, 16, 0, 1},
                                                                {"\\WDN6.STR;1", 96, 64, 149, 16384, 16, 0, 1},
                                                                {"\\WDN7B.STR;1", 96, 64, 149, 16384, 16, 0, 1},
                                                                {"\\WDN8B.STR;1", 96, 64, 255, 16384, 16, 0, 1},
                                                                {"\\WDN9B.STR;1", 96, 64, 297, 16384, 16, 0, 1},
                                                                {"\\WDN10B.STR;1", 96, 64, 269, 16384, 16, 0, 1},
                                                                {"\\WDN12A.STR;1", 96, 64, 131, 16384, 16, 0, 1},
                                                                {"\\CINEMAS\\HOVER.STR;1", 320, 240, 191, 153600, 48, 1, 1},
                                                                {"\\CINEMAS\\SEWER.STR;1", 320, 240, 496, 153600, 48, 1, 1},
                                                                {"\\CINEMAS\\DEATH.STR;1", 320, 240, 874, 153600, 48, 1, 1},
                                                                {"\\CINEMAS\\ROOFTOPS.STR;1", 320, 240, 671, 153600, 48, 1, 1},
                                                                {"\\CINEMAS\\CEMETARY.STR;1", 320, 240, 507, 153600, 48, 1, 1},
                                                                {"\\CINEMAS\\POE.STR;1", 320, 240, 963, 153600, 48, 1, 1},
                                                                {"\\CINEMAS\\FACTORY.STR;1", 320, 240, 447, 153600, 48, 1, 1},
                                                                {"\\CINEMAS\\WAR.STR;1", 320, 240, 427, 153600, 48, 1, 1},
                                                                {"\\CINEMAS\\WHITEHOU.STR;1", 320, 240, 169, 153600, 48, 1, 1},
                                                                {"\\CINEMAS\\OVAL.STR;1", 320, 240, 433, 153600, 48, 1, 1},
                                                                {"\\CINEMAS\\END.STR;1", 320, 240, 602, 153600, 48, 1, 1},
                                                                {"DUMMY", 0, 0, 0, 0, 0, 0, 0},
                                                                {"\\SNOT.STR;1", 96, 64, 1742, 16384, 16, 0, 1},
                                                                {"\\CINEMAS\\MONTAGE.STR;1", 320, 240, 626, 153600, 48, 1, 1},
                                                                {"\\CIRRUS.STR;1", 96, 64, 899, 16384, 16, 0, 1},
                                                                {"\\GEARWHOR.STR;1", 96, 64, 2232, 16384, 16, 0, 1},
                                                                {"\\ALERT.STR;1", 96, 64, 1106, 16384, 16, 0, 1},
                                                                {"\\SOFADOWN.STR;1", 96, 64, 2449, 16384, 16, 0, 1},
                                                                {"\\CINEMAS\\MCREDITS.STR;1", 320, 240, 1037, 153600, 48, 1, 1}};

static const uint8 movie_lookup_order[MovieRecordCount] = {0, 1, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 26, 27, 29, 30, 31, 32, 2, 3, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 28, 33};

static uint32 movie_sectors[MovieRecordCount];
MoviePlaybackState movie_playback;

enum
{
    MovieSliceBuffers = 6
};

struct MovieBufferStorage
{
    uint32 *slices[MovieSliceBuffers];
    uint32 *decoded[2];
    uint32 *ring;
    uint16 *vlc;
    uint8 *workspace_data;
    ScriptWorkspace *workspace;
};

static MovieBufferStorage movie_buffers;

static void CompleteMovieOutput()
{
    OutputMovieSlice();
}

static void ReleaseMovieStorage()
{
    if (!movie_buffers.workspace && !movie_buffers.workspace_data && !movie_buffers.vlc)
        return;
    DecDCToutCallback(0);
    StopMovieRead();
    // Cancel device borrowers before releasing their native backing storage
    DecDCTReset(1);
    if (movie_buffers.workspace)
        ReleaseScriptWorkspaceReference(movie_buffers.workspace);
    else
    {
        free(movie_buffers.ring);
        free(movie_buffers.vlc);
        for (uint32 index = 0; index < 2; ++index)
            free(movie_buffers.decoded[index]);
        for (uint32 index = 0; index < MovieSliceBuffers; ++index)
            free(movie_buffers.slices[index]);
    }
    MovieBufferStorage initial = {0};
    movie_buffers = initial;
}

void ResetMoviePlaybackState()
{
    ReleaseMovieStorage();
    MoviePlaybackState initial = {0};
    movie_playback = initial;
    movie_playback.balance_ticks = 4;
    for (uint32 index = 0; index < MovieRecordCount; ++index)
        movie_sectors[index] = movie_resources[index].initial_sector;
}

uint32 ResetMoviePlayback()
{
    FUNCTION_MARKER(0x8002F130, "SLUS_003.73");
    uint32 pending = movie_playback.refresh_pending;
    movie_playback.position_count = 0;
    if (pending)
        movie_playback.refresh_pending = 0;
    uint32 active = movie_playback.active;
    if (!active)
    {
        ReleaseMovieStorage();
        return active;
    }
    CdlATV volume = {0};
    CdMix(&volume);
    movie_playback.active = 0;
    ReleaseMovieStorage();
    CdlLOC location;
    CdIntToPos((sint32)movie_playback.start_sector, &location);
    ControlDataCd(22, &location, 0);
    PSX_RECT rectangle;
    rectangle.x = movie_playback.origin_x;
    rectangle.y = movie_playback.origin_y;
    rectangle.w = movie_playback.width;
    rectangle.h = movie_playback.height;
    return ClearImage(&rectangle, 1, 1, 1);
}

static uint32 StartMovieStream(void)
{
    FUNCTION_MARKER(0x8002FA48, "SLUS_003.73");
    CdlLOC location;
    uint8 mode = 0x80;
    CdIntToPos((sint32)movie_playback.start_sector, &location);
    uint32 result;
    do
    {
        while (!ControlDataCd(2, &location, 0))
            ;
        while (!ControlDataCd(14, &mode, 0))
            ;
        result = StartMovieRead(480);
    } while (!result);
    return result;
}

static uint32 RemapMovieTexturePolygons(uint32 tpage, const TextureResource &texture)
{
    FUNCTION_MARKER(0x8002F50C, "SLUS_003.73");
    for (uint32 slot = 0; slot < 2; ++slot)
    {
        const uint32 bank = GetActiveModelBank(slot);
        if (bank == 0xFFFFFFFF)
            continue;
        ModelGeometrySet &models = GetModelBank(bank).geometry;
        for (uint32 idx = 0; idx < models.count; ++idx)
        {
            ModelGeometry &model = models.models[idx];
            for (uint32 p = 0; p < model.poly_count; ++p)
            {
                ModelPolygon &poly = model.polys[p];
                if ((poly.flags & 3) != 3 || poly.tpage != texture.tpage || poly.clut != texture.clut)
                    continue;
                uint32 v;
                for (v = 0; v < 4; ++v)
                    if (poly.uv[v].u < texture.uv[0].u || poly.uv[v].u > texture.uv[1].u || poly.uv[v].v < texture.uv[0].v || poly.uv[v].v > texture.uv[2].v)
                        break;
                if (v != 4)
                    continue;
                poly.tpage = (uint16)tpage;
                for (v = 0; v < 4; ++v)
                    poly.uv[v].u = (poly.uv[v].u + 1) >> 1;
            }
        }
    }
    return 8;
}

uint32 PrepareMovieTexture(uint32 resource)
{
    FUNCTION_MARKER(0x8002E600, "SLUS_003.73");
    uint32 active = movie_playback.active;
    if (active)
        return active;
    const TextureResource &texture = GetTextureResource(resource);
    uint32 x = (uint16)texture.x;
    uint32 y = (uint16)texture.y;
    uint32 width = movie_playback.width;
    uint32 height = movie_playback.height;
    PSX_RECT rectangle;
    rectangle.w = (sint16)width;
    rectangle.h = (sint16)height;
    movie_playback.origin_x = x;
    movie_playback.origin_y = y;
    rectangle.x = (sint16)x;
    rectangle.y = (sint16)y;
    ClearImageRectangle(rectangle, 1, 1, 1);
    uint32 wide = movie_playback.rgb24;
    x = movie_playback.origin_x;
    width = movie_playback.width;
    if (wide)
        width = (3 * width) >> 1;
    y = movie_playback.origin_y;
    movie_playback.end_x = x + width;
    x = movie_playback.origin_x;
    uint32 tpage = ((y & 0x100) >> 4) | ((x & 0x3FF) >> 6) | 0x100 | ((y & 0x200) << 2);
    return RemapMovieTexturePolygons(tpage, texture);
}

uint32 QueueMoviePosition(const uint32 *position)
{
    FUNCTION_MARKER(0x8002E6D8, "SLUS_003.73");
    uint32 result = movie_playback.active;
    if (result)
        return result;
    uint32 count = movie_playback.position_count;
    result = (sint32)count < 8;
    if (!result)
        return result;
    Vector3 *target = movie_playback.positions + count;
    uint32 x = position[0];
    uint32 y = position[1];
    uint32 z = position[2];
    target->x = x;
    target->y = y;
    target->z = z;
    result = count + 1;
    movie_playback.position_count = result;
    return result;
}

uint32 PrepareMoviePlaybackBuffers(void)
{
    FUNCTION_MARKER(0x8002EF68, "SLUS_003.73");
    uint32 result = movie_sectors[movie_playback.selected];
    if (!result)
        return result;
    result = movie_playback.active;
    if (result)
        return result;
    movie_playback.slice_x = movie_playback.origin_x;
    movie_playback.slice_y = movie_playback.origin_y;
    movie_playback.slice_width = movie_playback.rgb24 ? 24 : 16;
    movie_playback.paused = 0;
    movie_playback.double_buffer = 0;
    movie_playback.slice_index = 0;
    movie_playback.decode_index = 0;
    movie_playback.frame_ready = 0;
    movie_playback.slice_height = movie_playback.height;
    ReleaseMovieStorage();
    uint32 remaining;
    movie_buffers.workspace = AcquireScriptWorkspace();
    remaining = GetScriptWorkspaceSize(movie_buffers.workspace);
    movie_buffers.workspace_data = GetScriptWorkspaceData(movie_buffers.workspace);
    if (remaining <= 0x11000)
        goto exhausted;
    if (!movie_buffers.workspace_data)
        goto exhausted;
    movie_buffers.vlc = (uint16 *)movie_buffers.workspace_data;
    {
        uint8 *cursor = movie_buffers.workspace_data + 0x11000;
        uint32 size = movie_playback.decode_bytes;
        uint32 stride = size & 0xFFFFFFFC;
        remaining -= 0x11000;
        for (uint32 index = 0; index < 2; ++index)
        {
            if (size >= remaining)
                goto exhausted;
            movie_buffers.decoded[index] = (uint32 *)cursor;
            cursor += stride;
            remaining -= size;
        }
        size = movie_playback.ring_sectors << 11;
        if (size >= remaining)
            goto exhausted;
        movie_buffers.ring = (uint32 *)cursor;
        cursor += size;
        uint32 count = (movie_playback.width >> 4) & 255;
        // TODO Establish that every screen record fits the original six pointer slots
        if (count > MovieSliceBuffers)
        {
            GameDiagnostic("Unsupported screen movie slice count %u\n", count);
            abort();
        }
        movie_playback.slice_count = count;
        remaining -= size;
        stride = count ? movie_playback.height << 5 : 0;
        for (uint32 index = 0; index < count; ++index)
        {
            if (stride >= remaining)
                goto exhausted;
            movie_buffers.slices[index] = (uint32 *)cursor;
            cursor += stride;
            remaining -= stride;
        }
    }
    DecDCTvlcBuild(movie_buffers.vlc);
    movie_playback.active = 1;
    movie_playback.at_end = 0;
    InitializeMovieDecoder();
    return DecodeNextMovieFrame();
exhausted:
    movie_playback.refresh_pending = 1;
    return 1;
}

static uint32 InitializeMovieDecoder(void)
{
    FUNCTION_MARKER(0x8002F7C8, "SLUS_003.73");
    DecDCTReset(0);
    DecDCToutCallback(CompleteMovieOutput);
    BindMovieRing(movie_buffers.ring, movie_playback.ring_sectors);
    StSetStream(movie_playback.rgb24, 0, 0xFFFFFFFF, 0, 0);
    cd_voice.active = 0;
    cd_voice.available = 1;
    cd_voice.prepared = 0;
    StartMovieStream();
    return FastControlDataCd(12, 0);
}

static uint32 *NextMovieFrame(void)
{
    FUNCTION_MARKER(0x8002F990, "SLUS_003.73");
    uint32 *frame, *header;
    if (ReadMovieFrame(&frame, &header))
        return 0;
    uint32 last_frame = movie_playback.last_frame;
    uint32 number = xport_load_le32((const uint8 *)header + 8);
    movie_playback.frame_number = number;
    if ((sint32)number >= (sint32)last_frame)
        movie_playback.at_end = 1;
    return frame;
}

static uint32 DecodeNextMovieFrame(void)
{
    FUNCTION_MARKER(0x8002F9E4, "SLUS_003.73");
    uint32 *frame = NextMovieFrame();
    if (!frame)
        return 0;
    uint32 index = movie_playback.decode_index;
    DecDCTvlc2(frame, movie_buffers.decoded[index], movie_buffers.vlc);
    StFreeRing(frame);
    return 1;
}

static uint32 OutputMovieSlice(void)
{
    FUNCTION_MARKER(0x8002F84C, "SLUS_003.73");
    if (movie_playback.rgb24 && StCdIntrFlag)
    {
        PumpMovieRead();
        StCdIntrFlag = 0;
    }
    uint32 index = movie_playback.slice_index;
    uint32 slices = movie_playback.slice_count;
    PSX_RECT rectangle;
    rectangle.x = movie_playback.slice_x;
    rectangle.y = movie_playback.slice_y;
    rectangle.w = movie_playback.slice_width;
    rectangle.h = movie_playback.slice_height;
    uint32 next_index = index + 1;
    movie_playback.slice_index = next_index;
    uint32 *pixels = movie_buffers.slices[index];
    if ((sint32)next_index >= (sint32)slices)
        movie_playback.slice_index = 0;
    uint32 x = movie_playback.slice_x;
    uint32 width = movie_playback.slice_width;
    uint32 end_x = movie_playback.end_x;
    x += width;
    movie_playback.slice_x = x;
    if ((sint32)(sint16)x < (sint32)end_x)
    {
        uint32 height = (uint32)(sint32)(sint16)movie_playback.slice_height;
        uint32 product = (uint32)(sint32)(sint16)width * height;
        next_index = movie_playback.slice_index;
        uint32 *output = movie_buffers.slices[next_index];
        uint32 words = (uint32)((sint32)(product + (product >> 31)) >> 1);
        DecDCTout(output, (sint32)words);
    }
    else
    {
        uint32 origin_x = movie_playback.origin_x;
        uint32 double_buffer = movie_playback.double_buffer;
        movie_playback.slice_x = origin_x;
        if (double_buffer)
        {
            sint32 signed_y = (sint16)movie_playback.slice_y;
            uint32 y = movie_playback.slice_y;
            movie_playback.slice_y = signed_y < 256 ? y + 256 : y - 256;
        }
    }
    return LoadImagePSX(&rectangle, pixels);
}

uint32 UpdateMoviePlayback(void)
{
    FUNCTION_MARKER(0x8002F284, "SLUS_003.73");
    uint32 result = movie_playback.active;
    if (!result)
        return result;
    if (!movie_playback.paused)
    {
        if (movie_playback.at_end)
        {
            uint32 double_buffer = movie_playback.double_buffer;
            movie_playback.at_end = 0;
            if (!double_buffer && !level_environment.area_kind)
            {
                uint32 selected = RandomRange(4) == 1 ? 31 : RandomRange(11) + 4;
                movie_playback.selected = selected;
                const MovieResource *entry = movie_resources + movie_playback.selected;
                uint32 last_frame = entry->last_frame;
                uint32 sector = movie_sectors[movie_playback.selected];
                movie_playback.last_frame = last_frame;
                movie_playback.start_sector = sector;
            }
            StartMovieStream();
        }
        if (movie_playback.frame_ready)
        {
            uint32 words;
            uint32 *output;
            DecDCTin(movie_buffers.decoded[movie_playback.decode_index], 0);
            uint32 width = (uint32)(sint32)(sint16)movie_playback.slice_width;
            uint32 height = (uint32)(sint32)(sint16)movie_playback.slice_height;
            words = width * height;
            output = movie_buffers.slices[movie_playback.slice_index];
            movie_playback.decode_index = 1 - movie_playback.decode_index;
            DecDCTout(output, (sint32)(words + (words >> 31)) >> 1);
        }
        uint32 decoded = DecodeNextMovieFrame();
        uint32 timer = movie_playback.balance_ticks;
        movie_playback.frame_ready = decoded;
        if (timer)
        {
            uint32 next = timer - 1;
            movie_playback.balance_ticks = next;
            if (!next)
            {
                uint32 index = 0, selected = 0;
                sint32 best = 0xFFFFFF;
                while ((sint32)index < (sint32)movie_playback.position_count)
                {
                    sint32 distance = (sint32)MeasureEuclideanDistance(visual_lists.cameras->pos.words, movie_playback.positions[index].words);
                    if (distance < best)
                    {
                        best = distance;
                        selected = index;
                    }
                    ++index;
                }
                UpdateCdVoiceBalance((const uint32 *)(movie_playback.positions + selected));
                movie_playback.balance_ticks = 4;
            }
        }
    }
    if (scene_frame.paused || scene_frame.camera_paused)
    {
        if (movie_playback.paused)
            return 1;
        movie_playback.paused = 1;
        StSetMask(1, 0, 0xFFFFFFFF);
        return PauseMovieRead();
    }
    result = movie_playback.paused;
    if (result)
    {
        movie_playback.paused = 0;
        StSetMask(0, 0, 0xFFFFFFFF);
        return ResumeMovieRead(480);
    }
    return result;
}

uint32 SelectMovieRecord(uint32 movie)
{
    FUNCTION_MARKER(0x8002EE7C, "SLUS_003.73");
    uint32 result = movie_playback.active;
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
    // TODO Establish the original record index domain for foreign script inputs
    if (movie >= MovieRecordCount)
    {
        GameDiagnostic("Invalid movie record %u\n", movie);
        abort();
    }
    const MovieResource *entry = movie_resources + movie;
    uint32 x = entry->width;
    uint32 y = entry->height;
    uint32 end_frame = entry->last_frame;
    uint32 first_position = entry->decode_bytes;
    uint32 second_position = entry->ring_sectors;
    movie_playback.width = x;
    uint32 start_frame = movie_sectors[movie];
    movie_playback.selected = movie;
    movie_playback.height = y;
    movie_playback.last_frame = end_frame;
    movie_playback.decode_bytes = first_position;
    movie_playback.ring_sectors = second_position;
    result = entry->rgb24;
    movie_playback.start_sector = start_frame;
    movie_playback.rgb24 = result;
    return result;
}

uint32 PlayMovie(uint32 movie)
{
    DISPENV environment;
    CdlATV volume;
    uint8 status[8];
    uint32 i, result = movie_playback.active;
    sint32 timeout;
    uint16 screen_width;
    uint32 width, height, left, top, mode, volume_setting;
    FUNCTION_MARKER(0x8002E814, "SLUS_003.73");
    if (result)
        return result;
    movie &= 255;
    movie_playback.double_buffer = 1;
    SelectMovieRecord(movie);
    width = movie_playback.width;
    movie_playback.origin_x = (320 - (sint32)width) / 2;
    mode = movie_playback.rgb24;
    left = movie_playback.origin_x;
    height = movie_playback.height;
    i = movie_playback.selected;
    movie_playback.end_x = left + (mode ? 3 * width / 2 : width);
    movie_playback.at_end = 0;
    top = (uint32)((240 - (sint32)height) / 2);
    result = movie_sectors[i];
    movie_playback.origin_y = top;
    if (!result)
        return result;
    left = movie_playback.origin_x;
    mode = movie_playback.rgb24;
    movie_playback.slice_y = top;
    movie_playback.slice_height = height;
    movie_playback.slice_x = left;
    screen_width = (uint16)scene_render.buffers[0].disp.screen.w;
    movie_playback.slice_width = mode ? 24 : 16;
    movie_playback.slice_index = 0;
    movie_playback.decode_index = 0;
    movie_playback.frame_ready = 0;
    ReleaseMovieStorage();
    movie_buffers.vlc = (uint16 *)malloc(69632);
    if (!movie_buffers.vlc)
        goto cleanup;
    for (i = 0; i < 2; ++i)
    {
        movie_buffers.decoded[i] = (uint32 *)malloc(movie_playback.decode_bytes);
        if (!movie_buffers.decoded[i])
            goto cleanup;
    }
    movie_buffers.ring = (uint32 *)malloc(movie_playback.ring_sectors << 11);
    if (!movie_buffers.ring)
        goto cleanup;
    movie_playback.slice_count = 2;
    for (i = 0; i < movie_playback.slice_count; ++i)
    {
        movie_buffers.slices[i] = (uint32 *)malloc((movie_playback.rgb24 ? 48 : 32) * movie_playback.height);
        if (!movie_buffers.slices[i])
            goto cleanup;
    }
    DecDCTvlcBuild(movie_buffers.vlc);
    movie_playback.active = 1;
    ClearStartupImageArea();
    SetDefDispEnv(&environment, 0, 256, 320, 240);
    VSync(0);
    PutDispEnv(&environment);
    if (movie == 3)
        volume_setting = 192;
    else
    {
        sint32 setting = (sint16)game_settings.music_volume;
        volume_setting = setting >= 64 ? (uint32)(63 * (setting - 64) / 64 + 192) : (uint32)(3 * setting);
    }
    volume.val0 = volume.val1 = (uint8)volume_setting;
    volume.val2 = volume.val3 = 0;
    CdMix(&volume);
    ::InitializeMovieDecoder();
    PollPadPackets();
    game_pads[0].buttons[14].pressed = 0;
    game_pads[0].buttons[3].pressed = 0;
    if (game_session.movie_any_button_skip)
    {
        game_pads[0].buttons[15].pressed = 0;
        for (i = 0; i < 8; ++i)
            if (i != 3)
                game_pads[0].buttons[i].pressed = 0;
    }
    while (!::DecodeNextMovieFrame())
    {
    }
    for (;;)
    {
        DecDCTin(movie_buffers.decoded[movie_playback.decode_index], movie_playback.rgb24 != 0);
        DecDCTout(movie_buffers.slices[movie_playback.slice_index], (sint16)movie_playback.slice_width * (sint16)movie_playback.slice_height / 2);
        ControlDataCd(1, 0, status);
        if (status[0] & 16)
        {
            WaitForMovieCdStop();
            break;
        }
        timeout = 0x800000;
        while (!::DecodeNextMovieFrame())
            if (timeout-- == 0)
                goto finish;
        if (!timeout)
            break;
        timeout = 0x800000;
        // Native callback progress requires fresh coordinates instead of the original fixed snapshot
        while ((sint16)movie_playback.slice_x != movie_playback.origin_x)
            if (--timeout == 0)
                goto finish;
        SetDefDispEnv(&environment, 0, (sint16)movie_playback.slice_y >= 256 ? (sint16)movie_playback.slice_y - 256 : (sint16)movie_playback.slice_y + 256, movie_playback.rgb24 ? 480 : 320, 240);
        if (movie_playback.rgb24)
        {
            environment.isrgb24 = 1;
            environment.disp.w = 2 * environment.disp.w / 3;
        }
        VSync(0);
        PutDispEnv(&environment);
        // Native scanout follows the completed movie buffer switch
        PresentGpuFrame();
        if (movie_playback.at_end)
            break;
        PollPadPackets();
        if (game_pads[0].buttons[14].pressed || game_pads[0].buttons[3].pressed)
            break;
        if (game_session.movie_any_button_skip)
        {
            if (game_pads[0].buttons[15].pressed)
                break;
            for (i = 0; i < 8; ++i)
                if (i != 3 && game_pads[0].buttons[i].pressed)
                    break;
            if (i < 8)
                break;
        }
    }
finish:
    game_pads[0].ClearPresses();
    movie_playback.active = 0;
    ClearStartupImageArea();
    environment.isrgb24 = 0;
    environment.disp.w = 512;
    environment.screen.w = screen_width;
    VSync(0);
    PutDispEnv(&environment);
    volume.val0 = volume.val1 = volume.val2 = volume.val3 = 0;
    CdMix(&volume);
    DecDCToutCallback(0);
    StopMovieRead();
    ControlDataCd(9, 0, 0);
cleanup:
    result = movie_playback.slice_count;
    for (i = 0; i < movie_playback.slice_count; ++i)
        result = i + 1 < movie_playback.slice_count;
    ReleaseMovieStorage();
    return result;
}

static uint32 WaitForMovieCdStop()
{
    FUNCTION_MARKER(0x8002F758, "SLUS_003.73");
    uint8 status[8];
    do
    {
        ControlDataCd(1, 0, status);
    } while (status[0] & 16);
    ControlDataCd(19, 0, 0);
    while (!(status[0] & 2))
        ControlDataCd(1, 0, status);
    return status[0] & 2;
}

uint32 InitializeMovieResourceTable()
{
    FUNCTION_MARKER(0x8002E738, "SLUS_003.73");
    CdlFILE file;
    uint32 value;
    for (uint32 index = 0; index < MovieRecordCount; ++index)
    {
        uint32 selected = movie_lookup_order[index];
        if (movie_sectors[selected] != 0)
        {
            if (FindDataCdFile(&file, movie_resources[selected].name))
                value = CdPosToInt(&file.pos);
            else
                value = 0;
            selected = movie_lookup_order[index];
            movie_sectors[selected] = value;
        }
    }
    return 0;
}

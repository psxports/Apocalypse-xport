#include <stdlib.h>

#include "psx.h"
#include "game_diagnostics.h"
#include "level_session.h"
#include "scene_lifecycle.h"
#include "game_settings.h"
#include "game_pad_input.h"
#include "visual_object.h"

extern "C"
{
#include "game_gpu_present.h"
}
#include "psx_press.h"
#include "psx_stream.h"
#include "movie_lifecycle.h"
#include "startup_resources.h"
#include "global.h"
#include "cd_voice_event.h"
#include "model_bank.h"
#include "game_heap.h"
#include "record_commands.h"
#include "psx_spu.h"
#include "psx_gpu.h"
#include "game_data_io.h"
#include "texture_resources.h"

struct MovieResource
{
    const char *name;
    uint16 width;
    uint16 height;
    uint16 last_frame;
    uint32 decode_bytes;
    uint32 ring_sectors;
    bool rgb24;
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
    uint32 ring_sectors;
    uint32 reading;
    bool ring_bound;
};

static MovieBufferStorage movie_buffers;

static uint32 RemapTexPolygons(uint16 tpage, const TextureResource &tex);
static uint32 WaitForCdStop();
static uint32 InitializeDecoder();
static void OutputSlice();
static uint32 DecodeNextFrame();
static uint32 StartStream();

static void BindRing(uint32 *ring, uint32 sectors)
{
    movie_buffers.ring_sectors = sectors;
    StSetRing(ring, sectors);
    movie_buffers.ring_bound = true;
}

static uint32 StartRead(uint32 mode)
{
    uint8 val = mode;
    if (!ControlDataCd(14, &val, 0))
        return 0;
    stream_set_read_mode(mode);
    movie_buffers.reading = ControlDataCd(27, 0, 0);
    return movie_buffers.reading;
}

static uint32 PauseRead()
{
    // Preserve complete queued frames, XA samples and decoder history
    cd_set_reading_native(0);
    movie_buffers.reading = 0;
    return 1;
}

static uint32 ResumeRead(uint32 mode)
{
    uint8 val = mode;
    if (!ControlDataCd(14, &val, 0))
        return 0;
    stream_set_read_mode(mode);
    cd_set_reading_native(1);
    movie_buffers.reading = 1;
    return 1;
}

static uint32 PumpRead()
{
    uint32 consumed = 0;
    if (!movie_buffers.reading)
        return 0;
    // Service the native CD clock while the caller waits for a complete frame
    psx_native_poll_events();
    for (uint32 remaining = movie_buffers.ring_sectors; remaining; --remaining)
    {
        uint16 status = StNextStatus(0, 0);
        if (status == StCOMPLETE || status == StLOCK || stream_pump() <= 0)
            break;
        ++consumed;
    }
    return consumed;
}

static uint32 ReadFrame(uint32 **frame, uint32 **header)
{
    PumpRead();
    return StGetNext(frame, header);
}

static void StopRead()
{
    movie_buffers.reading = 0;
    if (movie_buffers.ring_bound)
        StUnSetRing();
    movie_buffers.ring_bound = false;
    movie_buffers.ring_sectors = 0;
    CdReadyCallback(0);
}

static void ReleaseStorage()
{
    if (!movie_buffers.workspace && !movie_buffers.workspace_data && !movie_buffers.vlc)
        return;
    DecDCToutCallback(0);
    StopRead();
    // Cancel device borrowers before releasing their native backing storage
    DecDCTReset(1);
    if (movie_buffers.workspace)
        ReleaseScriptWorkspaceReference(movie_buffers.workspace);
    else
    {
        free(movie_buffers.ring);
        free(movie_buffers.vlc);
        for (uint32 idx = 0; idx < 2; ++idx)
            free(movie_buffers.decoded[idx]);
        for (uint32 idx = 0; idx < MovieSliceBuffers; ++idx)
            free(movie_buffers.slices[idx]);
    }
    MovieBufferStorage initial = {0};
    movie_buffers = initial;
}

void ResetMoviePlaybackState()
{
    ReleaseStorage();
    MoviePlaybackState initial = {0};
    movie_playback = initial;
    movie_playback.balance_ticks = 4;
    for (uint32 idx = 0; idx < MovieRecordCount; ++idx)
        movie_sectors[idx] = movie_resources[idx].initial_sector;
}

uint32 PrepareMovieTex(uint32 resource)
{
    FUNCTION_MARKER(0x8002E600, "SLUS_003.73");
    uint32 active = movie_playback.active;
    if (active)
        return active;
    const TextureResource &tex = TextureResource::Get(resource);
    uint16 x = tex.x;
    uint16 y = tex.y;
    uint32 width = movie_playback.width;
    uint16 height = movie_playback.height;
    PSX_RECT rect;
    rect.w = width;
    rect.h = height;
    movie_playback.origin_x = x;
    movie_playback.origin_y = y;
    rect.x = x;
    rect.y = y;
    ClearImage(&rect, 1, 1, 1);
    bool wide = movie_playback.rgb24;
    x = movie_playback.origin_x;
    width = movie_playback.width;
    if (wide)
        width = (3 * width) >> 1;
    y = movie_playback.origin_y;
    movie_playback.end_x = x + width;
    x = movie_playback.origin_x;
    uint16 tpage = getTPage(2, 0, x, y);
    return RemapTexPolygons(tpage, tex);
}

uint32 QueueMoviePos(const Vector3 *pos)
{
    FUNCTION_MARKER(0x8002E6D8, "SLUS_003.73");
    uint32 result = movie_playback.active;
    if (result)
        return result;
    sint32 count = movie_playback.pos_count;
    result = count < MoviePositionCount;
    if (!result)
        return result;
    movie_playback.positions[count] = *pos;
    movie_playback.pos_count = count + 1;
    return movie_playback.pos_count;
}

uint32 InitializeMovieResourceTable()
{
    FUNCTION_MARKER(0x8002E738, "SLUS_003.73");
    CdlFILE file;
    uint32 val;
    for (uint32 idx = 0; idx < MovieRecordCount; ++idx)
    {
        uint32 sel = movie_lookup_order[idx];
        if (movie_sectors[sel] != 0)
        {
            if (FindDataCdFile(&file, movie_resources[sel].name))
                val = CdPosToInt(&file.pos);
            else
                val = 0;
            movie_sectors[sel] = val;
        }
    }
    return 0;
}

uint32 PlayMovie(uint32 movie)
{
    DISPENV environment;
    CdlATV volume;
    uint8 status[8];
    uint32 i, result = movie_playback.active;
    sint32 timeout;
    sint16 scr_width;
    sint32 width, height, top, volume_setting;
    uint32 left;
    bool mode;
    FUNCTION_MARKER(0x8002E814, "SLUS_003.73");
    if (result)
        return result;
    movie &= 255;
    movie_playback.double_buffer = true;
    SelectMovieRec(movie);
    width = movie_playback.width;
    movie_playback.origin_x = (320 - width) / 2;
    mode = movie_playback.rgb24;
    left = movie_playback.origin_x;
    height = movie_playback.height;
    i = movie_playback.sel;
    movie_playback.end_x = left + (mode ? 3 * width / 2 : width);
    movie_playback.at_end = false;
    top = (240 - height) / 2;
    result = movie_sectors[i];
    movie_playback.origin_y = top;
    if (!result)
        return result;
    left = movie_playback.origin_x;
    mode = movie_playback.rgb24;
    movie_playback.slice_y = top;
    movie_playback.slice_height = height;
    movie_playback.slice_x = left;
    scr_width = scene_render.buffers[0].disp.screen.w;
    movie_playback.slice_width = mode ? 24 : 16;
    movie_playback.slice_idx = 0;
    movie_playback.decode_idx = 0;
    movie_playback.frame_ready = false;
    ReleaseStorage();
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
    movie_playback.active = true;
    ClearStartupImageArea();
    SetDefDispEnv(&environment, 0, 256, 320, 240);
    VSync(0);
    PutDispEnv(&environment);
    if (movie == 3)
        volume_setting = 192;
    else
    {
        sint32 setting = game_settings.music_volume;
        volume_setting = setting >= 64 ? 63 * (setting - 64) / 64 + 192 : 3 * setting;
    }
    volume.val0 = volume.val1 = volume_setting;
    volume.val2 = volume.val3 = 0;
    CdMix(&volume);
    InitializeDecoder();
    GamePadInput::Poll();
    game_pads[0].buttons[14].pressed = 0;
    game_pads[0].buttons[3].pressed = 0;
    if (game_session.movie_any_button_skip)
    {
        game_pads[0].buttons[15].pressed = 0;
        for (i = 0; i < 8; ++i)
            if (i != 3)
                game_pads[0].buttons[i].pressed = 0;
    }
    while (!DecodeNextFrame())
    {
    }
    for (;;)
    {
        DecDCTin(movie_buffers.decoded[movie_playback.decode_idx], movie_playback.rgb24);
        DecDCTout(movie_buffers.slices[movie_playback.slice_idx], movie_playback.slice_width * movie_playback.slice_height / 2);
        ControlDataCd(1, 0, status);
        if (status[0] & 16)
        {
            WaitForCdStop();
            break;
        }
        timeout = 0x800000;
        while (!DecodeNextFrame())
            if (timeout-- == 0)
                goto finish;
        if (!timeout)
            break;
        timeout = 0x800000;
        // Native callback progress requires fresh coordinates instead of the original fixed snapshot
        while (movie_playback.slice_x != movie_playback.origin_x)
            if (--timeout == 0)
                goto finish;
        SetDefDispEnv(&environment, 0, movie_playback.slice_y >= 256 ? movie_playback.slice_y - 256 : movie_playback.slice_y + 256, movie_playback.rgb24 ? 480 : 320, 240);
        if (movie_playback.rgb24)
        {
            environment.isrgb24 = 1;
            environment.disp.w = 2 * environment.disp.w / 3;
        }
        VSync(0);
        PutDispEnv(&environment);
        // Native scanout follows the completed movie buffer switch
        gpu_present();
        if (movie_playback.at_end)
            break;
        GamePadInput::Poll();
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
    movie_playback.active = false;
    ClearStartupImageArea();
    environment.isrgb24 = 0;
    environment.disp.w = 512;
    environment.screen.w = scr_width;
    VSync(0);
    PutDispEnv(&environment);
    volume.val0 = volume.val1 = volume.val2 = volume.val3 = 0;
    CdMix(&volume);
    DecDCToutCallback(0);
    StopRead();
    ControlDataCd(9, 0, 0);
cleanup:
    ReleaseStorage();
    return 0;
}

uint32 SelectMovieRec(uint32 movie)
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
        GameDiagnostic(("Invalid movie record %u\n", movie));
        abort();
    }
    const MovieResource *entry = movie_resources + movie;
    movie_playback.width = entry->width;
    movie_playback.sel = movie;
    movie_playback.height = entry->height;
    movie_playback.last_frame = entry->last_frame;
    movie_playback.decode_bytes = entry->decode_bytes;
    movie_playback.ring_sectors = entry->ring_sectors;
    result = entry->rgb24;
    movie_playback.start_sector = movie_sectors[movie];
    movie_playback.rgb24 = result;
    return result;
}

uint32 PrepareMoviePlaybackBuffers()
{
    FUNCTION_MARKER(0x8002EF68, "SLUS_003.73");
    uint32 result = movie_sectors[movie_playback.sel];
    if (!result)
        return result;
    result = movie_playback.active;
    if (result)
        return result;
    movie_playback.slice_x = movie_playback.origin_x;
    movie_playback.slice_y = movie_playback.origin_y;
    movie_playback.slice_width = movie_playback.rgb24 ? 24 : 16;
    movie_playback.paused = false;
    movie_playback.double_buffer = false;
    movie_playback.slice_idx = 0;
    movie_playback.decode_idx = 0;
    movie_playback.frame_ready = false;
    movie_playback.slice_height = movie_playback.height;
    ReleaseStorage();
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
        for (uint32 idx = 0; idx < 2; ++idx)
        {
            if (size >= remaining)
                goto exhausted;
            movie_buffers.decoded[idx] = (uint32 *)cursor;
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
            GameDiagnostic(("Unsupported screen movie slice count %u\n", count));
            abort();
        }
        movie_playback.slice_count = count;
        remaining -= size;
        stride = count ? movie_playback.height << 5 : 0;
        for (uint32 idx = 0; idx < count; ++idx)
        {
            if (stride >= remaining)
                goto exhausted;
            movie_buffers.slices[idx] = (uint32 *)cursor;
            cursor += stride;
            remaining -= stride;
        }
    }
    DecDCTvlcBuild(movie_buffers.vlc);
    movie_playback.active = true;
    movie_playback.at_end = false;
    InitializeDecoder();
    return DecodeNextFrame();
exhausted:
    movie_playback.refresh_pending = true;
    return 1;
}

uint32 ResetMoviePlayback()
{
    FUNCTION_MARKER(0x8002F130, "SLUS_003.73");
    uint32 pending = movie_playback.refresh_pending;
    movie_playback.pos_count = 0;
    if (pending)
        movie_playback.refresh_pending = false;
    uint32 active = movie_playback.active;
    if (!active)
    {
        ReleaseStorage();
        return active;
    }
    CdlATV volume = {0};
    CdMix(&volume);
    movie_playback.active = false;
    ReleaseStorage();
    CdlLOC location;
    CdIntToPos((sint32)movie_playback.start_sector, &location);
    ControlDataCd(22, &location, 0);
    PSX_RECT rect;
    rect.x = movie_playback.origin_x;
    rect.y = movie_playback.origin_y;
    rect.w = movie_playback.width;
    rect.h = movie_playback.height;
    return ClearImage(&rect, 1, 1, 1);
}

uint32 UpdateMoviePlayback()
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
            movie_playback.at_end = false;
            if (!double_buffer && !level_environment.area_kind)
            {
                uint32 sel = RandomRange(4) == 1 ? 31 : RandomRange(11) + 4;
                movie_playback.sel = sel;
                const MovieResource *entry = movie_resources + movie_playback.sel;
                movie_playback.last_frame = entry->last_frame;
                movie_playback.start_sector = movie_sectors[movie_playback.sel];
            }
            StartStream();
        }
        if (movie_playback.frame_ready)
        {
            sint32 words;
            uint32 *output;
            DecDCTin(movie_buffers.decoded[movie_playback.decode_idx], 0);
            words = movie_playback.slice_width * movie_playback.slice_height / 2;
            output = movie_buffers.slices[movie_playback.slice_idx];
            movie_playback.decode_idx = 1 - movie_playback.decode_idx;
            DecDCTout(output, words);
        }
        uint32 decoded = DecodeNextFrame();
        uint32 timer = movie_playback.balance_ticks;
        movie_playback.frame_ready = decoded;
        if (timer)
        {
            uint32 next = timer - 1;
            movie_playback.balance_ticks = next;
            if (!next)
            {
                sint32 idx = 0, sel = 0;
                sint32 best = 0xFFFFFF;
                while (idx < movie_playback.pos_count)
                {
                    sint32 distance = visual_lists.cameras->pos.EuclideanDistance(movie_playback.positions[idx]);
                    if (distance < best)
                    {
                        best = distance;
                        sel = idx;
                    }
                    ++idx;
                }
                UpdateCdVoiceBalance(movie_playback.positions + sel);
                movie_playback.balance_ticks = 4;
            }
        }
    }
    if (scene_frame.paused || scene_frame.camera_paused)
    {
        if (movie_playback.paused)
            return 1;
        movie_playback.paused = true;
        StSetMask(1, 0, 0xFFFFFFFF);
        return PauseRead();
    }
    result = movie_playback.paused;
    if (result)
    {
        movie_playback.paused = false;
        StSetMask(0, 0, 0xFFFFFFFF);
        return ResumeRead(480);
    }
    return result;
}

static uint32 RemapTexPolygons(uint16 tpage, const TextureResource &tex)
{
    FUNCTION_MARKER(0x8002F50C, "SLUS_003.73");
    for (uint32 slot = 0; slot < 2; ++slot)
    {
        const uint32 bank = ModelBank::GetActive(slot);
        if (bank == 0xFFFFFFFF)
            continue;
        ModelGeometrySet &models = ModelBank::Get(bank).geometry;
        for (uint32 idx = 0; idx < models.count; ++idx)
        {
            ModelGeometry &model = models.models[idx];
            for (uint32 p = 0; p < model.poly_count; ++p)
            {
                ModelPolygon &poly = model.polys[p];
                if ((poly.flags & 3) != 3 || poly.tpage != tex.tpage || poly.clut != tex.clut)
                    continue;
                uint32 v;
                for (v = 0; v < 4; ++v)
                    if (poly.uv[v].u < tex.uv[0].u || poly.uv[v].u > tex.uv[1].u || poly.uv[v].v < tex.uv[0].v || poly.uv[v].v > tex.uv[2].v)
                        break;
                if (v != 4)
                    continue;
                poly.tpage = tpage;
                for (v = 0; v < 4; ++v)
                    poly.uv[v].u = (poly.uv[v].u + 1) >> 1;
            }
        }
    }
    return 8;
}

static uint32 WaitForCdStop()
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

static uint32 InitializeDecoder()
{
    FUNCTION_MARKER(0x8002F7C8, "SLUS_003.73");
    DecDCTReset(0);
    DecDCToutCallback(OutputSlice);
    BindRing(movie_buffers.ring, movie_playback.ring_sectors);
    StSetStream(movie_playback.rgb24, 0, 0xFFFFFFFF, 0, 0);
    cd_voice.active = 0;
    cd_voice.available = 1;
    cd_voice.prepared = 0;
    StartStream();
    return FastControlDataCd(12, 0);
}

static void OutputSlice()
{
    FUNCTION_MARKER(0x8002F84C, "SLUS_003.73");
    if (movie_playback.rgb24 && StCdIntrFlag)
    {
        PumpRead();
        StCdIntrFlag = 0;
    }
    uint32 idx = movie_playback.slice_idx;
    uint32 slices = movie_playback.slice_count;
    PSX_RECT rect;
    rect.x = movie_playback.slice_x;
    rect.y = movie_playback.slice_y;
    rect.w = movie_playback.slice_width;
    rect.h = movie_playback.slice_height;
    uint32 next_idx = idx + 1;
    movie_playback.slice_idx = next_idx;
    uint32 *pixels = movie_buffers.slices[idx];
    if (next_idx >= slices)
        movie_playback.slice_idx = 0;
    sint16 x = movie_playback.slice_x;
    sint16 width = movie_playback.slice_width;
    uint16 end_x = movie_playback.end_x;
    x += width;
    movie_playback.slice_x = x;
    if (x < end_x)
    {
        sint32 words = width * movie_playback.slice_height / 2;
        next_idx = movie_playback.slice_idx;
        uint32 *output = movie_buffers.slices[next_idx];
        DecDCTout(output, words);
    }
    else
    {
        movie_playback.slice_x = movie_playback.origin_x;
        if (movie_playback.double_buffer)
        {
            sint16 signed_y = movie_playback.slice_y;
            sint16 y = movie_playback.slice_y;
            movie_playback.slice_y = signed_y < 256 ? y + 256 : y - 256;
        }
    }
    LoadImagePSX(&rect, pixels);
}

static uint32 *NextFrame()
{
    FUNCTION_MARKER(0x8002F990, "SLUS_003.73");
    uint32 *frame, *header;
    if (ReadFrame(&frame, &header))
        return 0;
    sint32 last_frame = movie_playback.last_frame;
    const StHEADER *info = (const StHEADER *)header;
    sint32 number = info->frameCount;
    movie_playback.frame_number = number;
    if (number >= last_frame)
        movie_playback.at_end = true;
    return frame;
}

static uint32 DecodeNextFrame()
{
    FUNCTION_MARKER(0x8002F9E4, "SLUS_003.73");
    uint32 *frame = NextFrame();
    if (!frame)
        return 0;
    uint32 idx = movie_playback.decode_idx;
    DecDCTvlc2(frame, movie_buffers.decoded[idx], movie_buffers.vlc);
    StFreeRing(frame);
    return 1;
}

static uint32 StartStream()
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
        result = StartRead(480);
    } while (!result);
    return result;
}

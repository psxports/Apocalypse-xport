#include <stdlib.h>

#include "psx.h"
#include "hd.h"
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

static uint32 movie_sectors[MovieRecordCount];
static bool movie_located[MovieRecordCount];
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

#if defined(AP_WEB)
static uint8 movie_rgb[96 * 64 * 3];
static uint32 movie_pixels[96 * 64 / 2];

static uint32 ReadWebMovieFrame()
{
    XportFmvFrame frame;
    sint32 result = xport_fmv_next(movie_rgb, sizeof(movie_rgb), &frame);
    if (result < 0)
    {
        GameDiagnostic(("Cannot decode screen movie %s\n", movie_resources[movie_playback.sel].name));
        abort();
    }
    if (result == 2)
        movie_playback.at_end = true;
    if (result != 1)
        return 0;
    if (frame.width != movie_playback.width || frame.height != movie_playback.height)
    {
        GameDiagnostic(("Unexpected screen movie dimensions %u x %u\n", frame.width, frame.height));
        abort();
    }
    for (uint32 pixel = 0; pixel < frame.width * frame.height; pixel += 2)
    {
        uint32 pair = 0;
        for (uint32 half = 0; half < 2; ++half)
        {
            uint32 color = 0;
            for (uint32 channel = 0; channel < 3; ++channel)
            {
                uint32 component = (movie_rgb[(pixel + half) * 3 + channel] + 4) >> 3;
                if (component > 31)
                    component = 31;
                color |= component << (channel * 5);
            }
            pair |= color << (half * 16);
        }
        movie_pixels[pixel / 2] = pair;
    }
    PSX_RECT rect;
    rect.x = movie_playback.origin_x;
    rect.y = movie_playback.origin_y;
    rect.w = frame.width;
    rect.h = frame.height;
    LoadImagePSX(&rect, movie_pixels);
    movie_playback.frame_number = frame.number;
    if (frame.number >= movie_playback.last_frame)
        movie_playback.at_end = true;
    return 1;
}
#endif

static uint32 RemapTexPolygons(uint16 tpage, const TextureResource &tex);
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
#if defined(AP_WEB)
    xport_fmv_close();
#endif
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
    InitializeMovieResourceTable();
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
    for (uint32 idx = 0; idx < MovieRecordCount; ++idx)
    {
        movie_sectors[idx] = movie_resources[idx].initial_sector;
        movie_located[idx] = !movie_resources[idx].initial_sector;
    }
    return 0;
}

static uint32 LocateMovieRec(uint32 movie)
{
    if (!movie_located[movie])
    {
        CdlFILE file;
        movie_sectors[movie] = FindDataCdFile(&file, movie_resources[movie].name) ? CdPosToInt(&file.pos) : 0;
        movie_located[movie] = true;
    }
    return movie_sectors[movie];
}

uint32 PlayMovie(uint32 movie)
{
    FUNCTION_MARKER(0x8002E814, "SLUS_003.73");
    if (movie_playback.active)
        return movie_playback.active;
    SelectMovieRec(movie & 255);
    char path[256];
    if (!ResolveDataPath(path, movie_resources[movie_playback.sel].name))
        abort();
    ReleaseStorage();
    hd_frame_interrupt();
    hd_reset();
    cd_voice.active = 0;
    cd_voice.available = 1;
    cd_voice.prepared = 0;
    ClearStartupImageArea();
    sint32 setting = game_settings.music_volume;
    sint32 volume_setting = movie == 3 ? 192 : setting >= 64 ? 63 * (setting - 64) / 64 + 192 : 3 * setting;
    CdlATV volume;
    volume.val0 = volume.val1 = volume_setting;
    volume.val2 = volume.val3 = 0;
    CdMix(&volume);
    movie_playback.active = true;
    sint32 result = xport_fmv_play(path);
    movie_playback.active = false;
    InvalidateDataCdStream();
    volume.val0 = volume.val1 = volume.val2 = volume.val3 = 0;
    CdMix(&volume);
    GamePadInput::Poll();
    game_pads[0].ClearPresses();
    ClearStartupImageArea();
    DrawSync(0);
    hd_reset();
    if (result < 0)
    {
        GameDiagnostic(("Cannot play movie: %s\n", path));
        abort();
    }
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
    movie_playback.start_sector = 0;
    movie_playback.rgb24 = result;
    return result;
}

uint32 PrepareMoviePlaybackBuffers()
{
    FUNCTION_MARKER(0x8002EF68, "SLUS_003.73");
#if defined(AP_WEB)
    if (movie_playback.active)
        return movie_playback.active;
    if (movie_playback.width != 96 || movie_playback.height != 64 || movie_playback.rgb24)
    {
        GameDiagnostic(("Unsupported screen movie record %u\n", movie_playback.sel));
        abort();
    }
    ReleaseStorage();
    movie_playback.paused = false;
    movie_playback.double_buffer = false;
    movie_playback.at_end = false;
    movie_playback.frame_ready = false;
    movie_playback.frame_number = 0;
    movie_playback.active = true;
    cd_voice.active = 0;
    cd_voice.available = 1;
    cd_voice.prepared = 0;
    return StartStream();
#else
    uint32 result = LocateMovieRec(movie_playback.sel);
    movie_playback.start_sector = result;
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
#endif
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
#if !defined(AP_WEB)
    CdlLOC location;
    CdIntToPos((sint32)movie_playback.start_sector, &location);
    ControlDataCd(22, &location, 0);
#endif
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
#if !defined(AP_WEB)
                movie_playback.start_sector = LocateMovieRec(movie_playback.sel);
#endif
            }
            StartStream();
        }
#if defined(AP_WEB)
        uint32 decoded = ReadWebMovieFrame();
#else
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
#endif
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
#if defined(AP_WEB)
        xport_fmv_pause(1);
        return 1;
#else
        StSetMask(1, 0, 0xFFFFFFFF);
        return PauseRead();
#endif
    }
    result = movie_playback.paused;
    if (result)
    {
        movie_playback.paused = false;
#if defined(AP_WEB)
        xport_fmv_pause(0);
        return 1;
#else
        StSetMask(0, 0, 0xFFFFFFFF);
        return ResumeRead(480);
#endif
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
                poly.texture_region.x = (tex.region.x + 1) >> 1;
                poly.texture_region.y = tex.region.y;
                poly.texture_region.width = (tex.region.width + 1) >> 1;
                poly.texture_region.height = tex.region.height;
                for (v = 0; v < 4; ++v)
                    poly.uv[v].u = (poly.uv[v].u + 1) >> 1;
            }
        }
    }
    return 8;
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
#if defined(AP_WEB)
    char path[256];
    if (!ResolveDataPath(path, movie_resources[movie_playback.sel].name) || xport_fmv_open(path) < 0)
    {
        GameDiagnostic(("Cannot open screen movie %s\n", movie_resources[movie_playback.sel].name));
        abort();
    }
    movie_playback.frame_number = 0;
    return 1;
#else
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
#endif
}

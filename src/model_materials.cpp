#include <string.h>
#include <stdlib.h>

#include "psx.h"
#include "game_diagnostics.h"
#include "model_materials.h"
#include "global.h"

struct ModelTextureTrackFile
{
    uint32 object;
    sint16 scroll_x;
    sint16 scroll_y;
    uint32 phase_speed;
    uint32 quad_count;
};

typedef char ModelTextureTrackFileSize[sizeof(ModelTextureTrackFile) == 16 ? 1 : -1];

static const ModelPalette *cur_model_pal;

void ReleaseModelTexAnimation(ModelTextureAnimation &animation)
{
    for (uint32 i = 0; i < animation.count; ++i)
        free(animation.tracks[i].quads);
    free(animation.tracks);
    animation.tracks = 0;
    animation.count = 0;
}

static bool DecodeModelTexAnimation(ModelTextureAnimation &animation, const uint8 *data, uint32 bytes, uint32 object_count)
{
    uint32 offset = 0, count = 0;
    for (;;)
    {
        if (bytes - offset < 16)
            return false;
        const ModelTextureTrackFile &file = *(const ModelTextureTrackFile *)(data + offset);
        const uint32 object = file.object;
        if (!object)
        {
            for (uint32 i = offset; i < bytes; ++i)
                if (data[i])
                    return false;
            break;
        }
        const uint32 quads = file.quad_count;
        if (object < 12 || (object - 12) % 36 || (object - 12) / 36 >= object_count || quads > (bytes - offset - 16) / 16)
            return false;
        ++count;
        offset += 16 + quads * 16;
    }
    if (!count)
        return true;
    if (count > 0xFFFFFFFF / sizeof(ModelTextureTrack))
        return false;
    animation.tracks = (ModelTextureTrack *)malloc(count * sizeof(ModelTextureTrack));
    if (!animation.tracks)
        return false;
    animation.count = count;
    for (uint32 i = 0; i < count; ++i)
        animation.tracks[i].quads = 0;
    offset = 0;
    for (uint32 i = 0; i < count; ++i)
    {
        ModelTextureTrack &track = animation.tracks[i];
        const ModelTextureTrackFile &file = *(const ModelTextureTrackFile *)(data + offset);
        track.object_idx = (file.object - 12) / 36;
        track.scroll_x = file.scroll_x;
        track.scroll_y = file.scroll_y;
        track.phase_speed = file.phase_speed;
        track.quad_count = file.quad_count;
        if (track.quad_count)
        {
            track.quads = (ModelUvQuad *)malloc(track.quad_count * sizeof(ModelUvQuad));
            if (!track.quads)
                return false;
        }
        for (uint32 quad = 0; quad < track.quad_count; ++quad)
            for (uint32 vertex = 0; vertex < 4; ++vertex)
            {
                const uint8 *src = data + offset + 16 + quad * 16 + vertex * 4;
                ModelUvVertex &target = track.quads[quad].vertices[vertex];
                target.u = src[0];
                target.v = src[1];
                target.wave_x = src[2];
                target.wave_y = src[3];
            }
        offset += 16 + track.quad_count * 16;
    }
    return true;
}

void LoadModelTexAnimation(ModelTextureAnimation &animation, const uint8 *data, uint32 bytes, uint32 object_count)
{
    ModelTextureAnimation decoded = {0, 0};
    if (!DecodeModelTexAnimation(decoded, data, bytes, object_count))
    {
        ReleaseModelTexAnimation(decoded);
        GameDiagnostic(("Invalid model texture animation or allocation failure\n"));
        abort();
    }
    ReleaseModelTexAnimation(animation);
    animation = decoded;
}

void ReleaseModelPal(ModelPalette &pal)
{
    free(pal.words);
    pal.words = 0;
    pal.count = 0;
}

void LoadModelPal(ModelPalette &pal, const uint8 *data, uint32 bytes)
{
    if (!bytes || (bytes & 3))
    {
        GameDiagnostic(("Invalid model palette size\n"));
        abort();
    }
    const uint32 count = bytes / 4;
    uint32 *words = (uint32 *)malloc(count * sizeof(*words));
    if (!words)
    {
        GameDiagnostic(("Cannot allocate model palette\n"));
        abort();
    }
    memcpy(words, data, bytes);
    ReleaseModelPal(pal);
    pal.count = count;
    pal.words = words;
}

uint32 ReadModelPalColor(const ModelPalette *pal, uint32 idx)
{
    if (!pal || !pal->words || idx >= pal->count)
    {
        GameDiagnostic(("Invalid model palette color index\n"));
        abort();
    }
    return pal->words[idx];
}

void SelectModelPal(const ModelPalette &pal)
{
    cur_model_pal = &pal;
}

const ModelPalette *GetCurModelPal()
{
    return cur_model_pal;
}

void ResetModelPalSelection()
{
    cur_model_pal = 0;
}

void ReleaseModelColorAnimation(ModelColorAnimation &animation)
{
    for (uint32 i = 0; i < animation.count; ++i)
        free(animation.tracks[i].keys);
    free(animation.tracks);
    animation.tracks = 0;
    animation.count = 0;
    animation.present = false;
}

static bool DecodeModelColorAnimation(ModelColorAnimation &animation, const uint8 *data, uint32 bytes)
{
    uint32 offset = 0, count = 0;
    while (offset < bytes)
    {
        if (bytes - offset < 4)
            return false;
        uint32 keys = data[offset + 1];
        if (!keys || data[offset + 2] >= keys || keys > (bytes - offset - 4) / 4)
            return false;
        bool duration = false;
        for (uint32 key = 0; key < keys; ++key)
            duration = duration || data[offset + 4 + key * 4 + 3] != 0;
        if (!duration)
            return false;
        ++count;
        offset += 4 + keys * 4;
    }
    if (count > 0xFFFFFFFF / sizeof(ModelColorTrack))
        return false;
    if (count)
    {
        animation.tracks = (ModelColorTrack *)malloc(count * sizeof(ModelColorTrack));
        if (!animation.tracks)
            return false;
    }
    animation.count = count;
    for (uint32 i = 0; i < count; ++i)
        animation.tracks[i].keys = 0;
    offset = 0;
    for (uint32 i = 0; i < count; ++i)
    {
        ModelColorTrack &track = animation.tracks[i];
        track.color_idx = data[offset];
        track.key_count = data[offset + 1];
        track.key = data[offset + 2];
        track.elapsed = data[offset + 3];
        track.keys = (ModelColorKey *)malloc(track.key_count * sizeof(ModelColorKey));
        if (!track.keys)
            return false;
        for (uint32 key = 0; key < track.key_count; ++key)
        {
            const uint8 *src = data + offset + 4 + key * 4;
            for (uint32 axis = 0; axis < 3; ++axis)
                track.keys[key].rgb[axis] = src[axis];
            track.keys[key].duration = src[3];
        }
        offset += 4 + track.key_count * 4;
    }
    animation.present = true;
    return true;
}

void LoadModelColorAnimation(ModelColorAnimation &animation, const uint8 *data, uint32 bytes)
{
    ModelColorAnimation decoded = {0, 0, false};
    if (!DecodeModelColorAnimation(decoded, data, bytes))
    {
        ReleaseModelColorAnimation(decoded);
        GameDiagnostic(("Invalid model color animation\n"));
        abort();
    }
    ReleaseModelColorAnimation(animation);
    animation = decoded;
}

void ValidateModelColorAnimation(const ModelPalette &pal, const ModelColorAnimation &animation)
{
    for (uint32 i = 0; i < animation.count; ++i)
        if (!pal.words || animation.tracks[i].color_idx >= pal.count)
        {
            GameDiagnostic(("Color animation references missing palette color\n"));
            abort();
        }
}

void AdvanceModelColors(ModelPalette &pal, ModelColorAnimation &animation, uint32 step)
{
    for (uint32 i = 0; i < animation.count; ++i)
    {
        ModelColorTrack &track = animation.tracks[i];
        uint32 key = track.key;
        uint32 elapsed = track.elapsed + step;
        uint8 duration = track.keys[key].duration;
        if ((sint32)elapsed >= duration)
            do
            {
                elapsed -= track.keys[key].duration;
                ++key;
                if (key == track.key_count)
                    key = 0;
            } while ((sint32)elapsed >= track.keys[key].duration);
        track.key = key;
        track.elapsed = elapsed;
        const ModelColorKey &active = track.keys[key];
        uint32 next = key + 1;
        if (next == track.key_count)
            next = 0;
        sint32 base[3], delta[3];
        for (uint32 axis = 0; axis < 3; ++axis)
            base[axis] = active.rgb[axis] << 4;
        for (uint32 axis = 0; axis < 3; ++axis)
            delta[axis] = (track.keys[next].rgb[axis] << 4) - base[axis];
        xport_gte_write_data(6, 0);
        xport_gte_write_data(8, DivideSignedWord(elapsed << 12, active.duration));
        for (uint32 axis = 0; axis < 3; ++axis)
            xport_gte_write_data(25 + axis, base[axis]);
        for (uint32 axis = 0; axis < 3; ++axis)
            xport_gte_write_data(9 + axis, delta[axis]);
        xport_gte_execute(0x1A8003E);
        // The palette owner is validated when the complete bank is loaded
        if (track.color_idx >= pal.count)
        {
            GameDiagnostic(("Color animation palette owner changed\n"));
            abort();
        }
        pal.words[track.color_idx] = xport_gte_read_data(22);
    }
}

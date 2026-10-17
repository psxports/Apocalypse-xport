#include "game_sdk_bindings.h"
#include "model_materials.h"
extern "C"
{
#include "draft_first_adapters.h"
}
#include <stdlib.h>

static const ModelPalette *current_model_palette;

void ReleaseModelTextureAnimation(ModelTextureAnimation &animation)
{
    for (uint32 i = 0; i < animation.count; ++i)
        free(animation.tracks[i].quads);
    free(animation.tracks);
    animation.tracks = 0;
    animation.count = 0;
}

static bool DecodeModelTextureAnimation(ModelTextureAnimation &animation, const uint8 *data, uint32 bytes, uint32 object_count)
{
    uint32 offset = 0, count = 0;
    for (;;)
    {
        if (bytes - offset < 16)
            return false;
        const uint32 object = xport_load_le32(data + offset);
        if (!object)
        {
            for (uint32 i = offset; i < bytes; ++i)
                if (data[i])
                    return false;
            break;
        }
        const uint32 quads = xport_load_le32(data + offset + 12);
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
        track.object_index = (xport_load_le32(data + offset) - 12) / 36;
        track.scroll_x = (sint16)((uint32)data[offset + 4] | ((uint32)data[offset + 5] << 8));
        track.scroll_y = (sint16)((uint32)data[offset + 6] | ((uint32)data[offset + 7] << 8));
        track.phase_speed = xport_load_le32(data + offset + 8);
        track.quad_count = xport_load_le32(data + offset + 12);
        if (track.quad_count)
        {
            track.quads = (ModelUvQuad *)malloc(track.quad_count * sizeof(ModelUvQuad));
            if (!track.quads)
                return false;
        }
        for (uint32 quad = 0; quad < track.quad_count; ++quad)
            for (uint32 vertex = 0; vertex < 4; ++vertex)
            {
                const uint8 *source = data + offset + 16 + quad * 16 + vertex * 4;
                ModelUvVertex &target = track.quads[quad].vertices[vertex];
                target.u = source[0];
                target.v = source[1];
                target.wave_x = source[2];
                target.wave_y = source[3];
            }
        offset += 16 + track.quad_count * 16;
    }
    return true;
}

void LoadModelTextureAnimation(ModelTextureAnimation &animation, const uint8 *data, uint32 bytes, uint32 object_count)
{
    ModelTextureAnimation decoded = {0, 0};
    if (!DecodeModelTextureAnimation(decoded, data, bytes, object_count))
    {
        ReleaseModelTextureAnimation(decoded);
        GameDiagnostic("Invalid model texture animation or allocation failure\n");
        abort();
    }
    ReleaseModelTextureAnimation(animation);
    animation = decoded;
}

void ReleaseModelPalette(ModelPalette &palette)
{
    free(palette.words);
    palette.words = 0;
    palette.count = 0;
}

void LoadModelPalette(ModelPalette &palette, const uint8 *data, uint32 bytes)
{
    if (!bytes || (bytes & 3))
    {
        GameDiagnostic("Invalid model palette size\n");
        abort();
    }
    const uint32 count = bytes / 4;
    uint32 *words = (uint32 *)malloc(count * sizeof(uint32));
    if (!words)
    {
        GameDiagnostic("Cannot allocate model palette\n");
        abort();
    }
    for (uint32 i = 0; i < count; ++i)
        words[i] = xport_load_le32(data + i * 4);
    ReleaseModelPalette(palette);
    palette.count = count;
    palette.words = words;
}

uint32 ReadModelPaletteColor(const ModelPalette *palette, uint32 index)
{
    if (!palette || !palette->words || index >= palette->count)
    {
        GameDiagnostic("Invalid model palette color index\n");
        abort();
    }
    return palette->words[index];
}

void SelectModelPalette(const ModelPalette &palette)
{
    current_model_palette = &palette;
}

const ModelPalette *GetCurrentModelPalette()
{
    return current_model_palette;
}

void ResetModelPaletteSelection()
{
    current_model_palette = 0;
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
        track.color_index = data[offset];
        track.key_count = data[offset + 1];
        track.key = data[offset + 2];
        track.elapsed = data[offset + 3];
        track.keys = (ModelColorKey *)malloc(track.key_count * sizeof(ModelColorKey));
        if (!track.keys)
            return false;
        for (uint32 key = 0; key < track.key_count; ++key)
        {
            const uint8 *source = data + offset + 4 + key * 4;
            for (uint32 axis = 0; axis < 3; ++axis)
                track.keys[key].rgb[axis] = source[axis];
            track.keys[key].duration = source[3];
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
        GameDiagnostic("Invalid model color animation\n");
        abort();
    }
    ReleaseModelColorAnimation(animation);
    animation = decoded;
}

void ValidateModelColorAnimation(const ModelPalette &palette, const ModelColorAnimation &animation)
{
    for (uint32 i = 0; i < animation.count; ++i)
        if (!palette.words || animation.tracks[i].color_index >= palette.count)
        {
            GameDiagnostic("Color animation references missing palette color\n");
            abort();
        }
}

static uint32 DivideColorWord(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == 0xFFFFFFFF)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

void AdvanceModelColors(ModelPalette &palette, ModelColorAnimation &animation, uint32 step)
{
    for (uint32 i = 0; i < animation.count; ++i)
    {
        ModelColorTrack &track = animation.tracks[i];
        uint32 key = track.key;
        uint32 elapsed = (uint32)track.elapsed + step;
        uint32 duration = track.keys[key].duration;
        if ((sint32)elapsed >= (sint32)duration)
            do
            {
                elapsed -= track.keys[key].duration;
                ++key;
                if (key == track.key_count)
                    key = 0;
            } while ((sint32)elapsed >= (sint32)track.keys[key].duration);
        track.key = (uint8)key;
        track.elapsed = (uint8)elapsed;
        const ModelColorKey &active = track.keys[key];
        uint32 next = key + 1;
        if (next == track.key_count)
            next = 0;
        uint32 base[3], delta[3];
        for (uint32 axis = 0; axis < 3; ++axis)
            base[axis] = (uint32)active.rgb[axis] << 4;
        for (uint32 axis = 0; axis < 3; ++axis)
            delta[axis] = ((uint32)track.keys[next].rgb[axis] << 4) - base[axis];
        xport_gte_write_data(6, 0);
        xport_gte_write_data(8, DivideColorWord(elapsed << 12, active.duration));
        for (uint32 axis = 0; axis < 3; ++axis)
            xport_gte_write_data(25 + axis, base[axis]);
        for (uint32 axis = 0; axis < 3; ++axis)
            xport_gte_write_data(9 + axis, delta[axis]);
        xport_draft_gte_execute(0x1A8003E);
        // The palette owner is validated when the complete bank is loaded
        if (track.color_index >= palette.count)
        {
            GameDiagnostic("Color animation palette owner changed\n");
            abort();
        }
        palette.words[track.color_index] = xport_gte_read_data(22);
    }
}

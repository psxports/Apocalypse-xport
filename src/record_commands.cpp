#include <stddef.h>
#include <new>
#include <stdlib.h>

#include "game_object_allocation.h"
#include "camera.h"
#include "player.h"
#include "visual_object.h"
#include "visual_registry.h"
#include "audio_voice.h"
#include "cd_voice_event.h"
#include "model_projection.h"
#include "model_render.h"
#include "hud_text.h"
#include "level_session.h"
#include "game_diagnostics.h"
#include "record_commands.h"
#include "record_scripts.h"
#include "record_groups.h"
#include "record_data.h"
#include "enemy_list.h"
#include "player_save_state.h"
#include "model_resources.h"
#include "audio_session.h"
#include "region_actions.h"
#include "game_heap.h"

static void ReadRegionBounds(RecordCursor &cursor, Vector3 &lower, Vector3 &upper)
{
    cursor = cursor.Aligned(4);
    for (uint32 axis = 0; axis < 3; ++axis)
        lower.words[axis] = (cursor + axis * 4).Read32() << 12;
    for (uint32 axis = 0; axis < 3; ++axis)
        upper.words[axis] = (cursor + 12 + axis * 4).Read32() << 12;
    cursor += 24;
}

RecordCursor SkipRecordString(RecordCursor cursor)
{
    FUNCTION_MARKER(0x800650B4, "SLUS_003.73");
    while (cursor.Read8())
        cursor += 1;
    return (cursor + 1).Aligned(2);
}

static RecordCursor SkipRecordCommand(RecordCursor cursor)
{
    uint32 command;
    FUNCTION_MARKER(0x800650EC, "SLUS_003.73");
    command = cursor.Read16();
    cursor += 2;
    switch (command)
    {
        case 2:
            while ((cursor)[0])
                cursor = SkipRecordString(cursor);
            return cursor + 2;
        case 13:
        case 105:
        case 106:
        case 131:
        case 132:
        case 134:
        case 138:
        case 147:
        case 148:
        case 153:
        case 154:
        case 155:
        case 156:
        case 160:
        case 163:
        case 164:
        case 165:
        case 166:
        case 168:
        case 169:
        case 170:
        case 172:
        case 177:
            return cursor + 2;
        case 104:
            return cursor + 6;
        case 115:
        case 119:
        case 126:
        case 127:
        case 128:
        case 140:
        case 142:
        case 176:
            return SkipRecordString(cursor);
        case 130:
        case 135:
        case 139:
        case 143:
        case 144:
        case 145:
        case 146:
        case 167:
        case 174:
            return cursor + 4;
        case 141:
            cursor += 2;
            // Fall through to the aligned six-word region list
        case 133:
            while (cursor.Read16() != 255)
                cursor = cursor.Aligned(4) + 24;
            return cursor + 2;
        case 171:
            return cursor.Aligned(4) + 10;
        case 65535:
            return RecordCursor();
        default:
            // TODO Preserve original skip widths that differ from command execution
            return cursor;
    }
}

struct ScriptWorkspace
{
    GameHeapBlock *reservation;
    uint8 *data;
    uint32 size;
    uint32 references;
};

static ScriptWorkspace *current_workspace;

ScriptWorkspace *AcquireScriptWorkspace()
{
    if (current_workspace)
        ++current_workspace->references;
    return current_workspace;
}

void ReleaseScriptWorkspaceReference(ScriptWorkspace *workspace)
{
    if (workspace && !--workspace->references)
    {
        free(workspace->data);
        free(workspace);
    }
}

uint8 *GetScriptWorkspaceData(ScriptWorkspace *workspace)
{
    return workspace ? workspace->data : 0;
}

uint32 GetScriptWorkspaceSize(const ScriptWorkspace *workspace)
{
    FUNCTION_MARKER(0x8006B71C, "SLUS_003.73");
    return workspace ? workspace->size : 0;
}

void AllocateScriptWorkspace(uint32 size)
{
    FUNCTION_MARKER(0x800653B8, "SLUS_003.73");
    if (current_workspace)
        ReleaseScriptWorkspace();
    GameHeapBlock *reservation = AllocateHeapBlock(size, 1, 1);
    ScriptWorkspace *workspace = (ScriptWorkspace *)malloc(sizeof(*workspace));
    uint8 *data = (uint8 *)malloc(size ? size : 1);
    if (!workspace || !data)
    {
        GameDiagnostic("Cannot allocate script movie workspace\n");
        abort();
    }
    workspace->reservation = reservation;
    workspace->data = data;
    workspace->size = size;
    workspace->references = 1;
    current_workspace = workspace;
    FUNCTION_MARKER(0x8006B70C, "SLUS_003.73");
}

void ReleaseScriptWorkspace()
{
    FUNCTION_MARKER(0x800653F4, "SLUS_003.73");
    ScriptWorkspace *workspace = current_workspace;
    if (!workspace)
        return;
    if (workspace->reservation)
        ReleaseHeapBlock(workspace->reservation);
    workspace->reservation = 0;
    current_workspace = 0;
    ReleaseScriptWorkspaceReference(workspace);
}

uint32 ExecuteRecordCommands(RecordCursor cursor, uint32 index, uint32 synchronous)
{
    GameHeapBlock *reservation;
    ResourceScriptBorrow script_borrow;
    bool has_projection_config;
    sint32 projection_args[3];
    ModelInstance *receiver;
    uint32 command, resource;
    RecordCursor record_payload;
    Camera *camera;
    ResourceScript *script_record;
    sint32 value, previous_start;
    uint32 duration;
    sint32 load_bank_active;
    RecordCursor bank_name;
    RecordCursor visual_data;
    sint16 cycles, region_enabled;
    Vector3 origin, region_end;
    FUNCTION_MARKER(0x8006541C, "SLUS_003.73");
    has_projection_config = false;
    script_record = FindScriptRecord(index);
    while (1)
    {
        command = cursor.Read16();
        (cursor += 2);
        if ((command == 0xFFFF))
            break;
        switch (command)
        {
            case 2:
                checkpoint_names.Clear();
                while (cursor[0])
                {
                    checkpoint_names.Add(cursor.CStr());
                    cursor = SkipRecordString(cursor);
                }

                goto SkipCommandWord;

            case 3:
                if (script_record)
                {
                    if (script_record->spawn_cycles_left)
                    {
                        record_payload = GetRecordPayload(index);
                        SpawnEnemyIndices(record_payload);
                        value = script_record->spawn_cycles_left;
                        if ((value != 0xFFFF))
                            script_record->spawn_cycles_left = value - 1;
                    }
                }
                else
                {
                    record_payload = GetRecordPayload(index);
                    SpawnEnemyIndices(record_payload);
                }
                continue;

            case 4:

            case 5:
                record_payload = GetRecordPayload(index);
                SetObjectRecordGroupMode(record_payload, command);
                continue;

            case 10:
                record_payload = GetRecordPayload(index);
                ActivateEnemyIndices(record_payload);
                continue;

            case 11:
                ApplyRecordGroupAction(index, 0);
                continue;

            case 12:
                ApplyRecordGroupAction(index, 1);
                continue;

            case 13:
                SetRecordGroupEnabled(index, cursor.Read16());
                goto SkipCommandWord;

            case 102:
            case 103:
                // The original flag has no remaining consumers
                continue;

            case 104:
                projection_args[0] = cursor.Read16();
                projection_args[1] = (cursor + 2).Read16();
                projection_args[2] = (cursor + 4).Read16();
                cursor += 6;
                has_projection_config = true;
                continue;

            case 105:
                value = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                PlaySoundEffect(value, 0x2000, 0);
                continue;

            case 106:
                value = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                ::StopSoundVoices(value);
                continue;

            case 119:
                goto SkipCommandString;

            case 126:
                bank_name = cursor;
                load_bank_active = 0;
                goto QueueBankLoad;

            case 127:
                UnloadModelBankByName(cursor.CStr());
                goto SkipCommandString;

            case 128:
                bank_name = cursor;
                load_bank_active = 1;
            QueueBankLoad:
                QueueModelBankLoad(bank_name.CStr(), load_bank_active);

                if (synchronous)
                    WaitForModelBankLoads();
                goto SkipCommandString;

            case 129:
                WaitForModelBankLoads();
                continue;

            case 130:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                value = ((sint16)(cursor.Read16()));
                duration = ((uint16)(((sint16)(((cursor + (1) * 2)).Read16()))));
                cursor += (2) * 2;
                camera->SetYawTransition(value, duration);
                continue;

            case 131:
                value = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                EnableVisualRegistryEntry(value);
                continue;

            case 132:
                value = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                DisableVisualRegistryEntry(value);
                continue;

            case 133:
                while ((((sint16)(cursor.Read16())) != 255))
                {
                    ReadRegionBounds(cursor, origin, region_end);
                    ClearRegion(origin.words, region_end.words);
                }

                goto SkipCommandWord;

            case 134:
                if (!(script_record->spawn_cycles_initialized))
                {
                    cycles = ((sint16)(cursor.Read16()));
                    script_record->spawn_cycles_initialized = 1;
                    script_record->spawn_cycles_left = cycles;
                }
                goto SkipCommandWord;

            case 135:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                value = ((uint16)(((sint16)(cursor.Read16()))));
                duration = ((uint16)(((sint16)(((cursor + (1) * 2)).Read16()))));
                cursor += (2) * 2;
                SetCameraOrbitDistanceTransition(value, duration);
                continue;

            case 136:
                SetCdAudioEnabled(1);
                continue;

            case 137:
                SetCdAudioEnabled(0);
                continue;

            case 138:
                value = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                SeekCdAudioEntry(value);
                continue;

            case 139:
                ::SelectCdVoice(cursor.Read16(), (cursor + 2).Read16());
                goto SkipCommandPair;

            case 140:

            case 176:
                previous_start = GetStartRecordIndex();
                SelectStartRecord(cursor.CStr());
                cursor = SkipRecordString(cursor);
                if (((previous_start != 0xFFFF) && (GetStartRecordIndex() != previous_start)))
                {
                    if ((command != 176))
                    {
                        ClearAnimatedHudText();
                        ::AddAnimatedHudText(HudMessage::checkpoint, hud_pickup_animation);
                    }
                    CaptureCurrentPlayerSave();
                }
                continue;

            case 141:
                region_enabled = ((cursor += 2, cursor - 2)).Read16();
                while ((((sint16)(cursor.Read16())) != 255))
                {
                    ReadRegionBounds(cursor, origin, region_end);
                    SetRegionObjectsEnabled(origin.words, region_end.words, region_enabled != 0);
                }

                goto SkipCommandWord;

            case 142:
                SelectRecordModelBank(cursor.CStr());
                cursor = SkipRecordString(cursor);
                continue;

            case 143:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                value = ((sint16)(cursor.Read16()));
                duration = ((uint16)(((sint16)(((cursor + (1) * 2)).Read16()))));
                cursor += (2) * 2;
                SetCameraOrbitHeightTransition(value, duration);
                continue;

            case 144:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                value = ((sint16)(cursor.Read16()));
                duration = ((uint16)(((sint16)(((cursor + (1) * 2)).Read16()))));
                cursor += (2) * 2;
                SetCameraTargetLateralOffsetTransition(value, duration);
                continue;

            case 145:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                value = ((sint16)(cursor.Read16()));
                duration = ((uint16)(((sint16)(((cursor + (1) * 2)).Read16()))));
                cursor += (2) * 2;
                SetCameraTargetHeightOffsetTransition(value, duration);
                continue;

            case 146:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                value = ((sint16)(cursor.Read16()));
                duration = ((uint16)(((sint16)(((cursor + (1) * 2)).Read16()))));
                cursor += (2) * 2;
                SetCameraTargetForwardOffsetTransition(value, duration);
                continue;

            case 147:
                level_environment.area_kind = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                continue;

            case 148:
                value = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                if ((script_record->spawn_count == value))
                    continue;
                if ((((sint16)(cursor.Read16())) == 149))
                    goto SkipCommandWord;
                do
                    cursor = SkipRecordCommand(cursor);
                while ((((sint16)(cursor.Read16())) != 149));
                (cursor += 2);
                break;

            case 150:
                // The original fade-step word has no consumers
                cursor.Read16();
                cursor += 2;
                continue;

            case 151:
                ReleaseScriptWorkspace();
                value = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                AllocateScriptWorkspace((uint32)value * 1000);
                continue;

            case 152:
                receiver = visual_lists.players;
                if (receiver)
                    static_cast<Player *>(visual_lists.players->owner)->EnterDeathState();
                continue;

            case 153:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandWord;
                value = (uint32)(sint32)(sint16)((cursor += 2, cursor - 2)).Read16();
                camera->SetLateralColDistance(value);
                continue;

            case 154:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandWord;
                value = (uint32)(sint32)(sint16)((cursor += 2, cursor - 2)).Read16();
                camera->SetRearColDistance(value);
                continue;

            case 155:
                value = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                RestoreMusicPlayback();
                continue;

            case 156:
                value = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                StopMusicSequence();
                continue;

            case 157:
                value = ((((cursor += 2, cursor - 2))))[0];
                SetSoundReverbMode(value);
                continue;

            case 158:
                CompleteLevel(1);
                continue;

            case 159:
                LoadAudioSequence(cursor.CStr());
            SkipCommandString:
                cursor = SkipRecordString(cursor);

                continue;

            case 160:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (camera)
                    camera->mode = cursor.Read16();
                goto SkipCommandWord;

            case 161:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandWord;
                value = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                camera->SetModePreset(value);
                break;

            case 163:
            {
                ModelInstance *player = visual_lists.players;
                if (player)
                {
                    value = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                    static_cast<Player *>(player->owner)->disable_timer = value;
                }
                continue;
            }

            case 164:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandWord;
                value = ((cursor += 2, cursor - 2)).Read16();
                camera->SetColYawStrength(value);
                break;

            case 165:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandWord;
                value = ((cursor += 2, cursor - 2)).Read16();
                camera->SetColPitchStrength(value);
                break;

            case 166:
                model_render_state.depth_bias[0] = ((cursor += 2, cursor - 2)).Read16();
                continue;

            case 167:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                value = ((uint16)(((sint16)(cursor.Read16()))));
                duration = ((uint16)(((sint16)(((cursor + (1) * 2)).Read16()))));
                cursor += (2) * 2;
                camera->SetProjDistanceTransition(value, duration);
                break;

            case 168:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (camera)
                    camera->vertical_pitch = cursor.Read16();
                goto SkipCommandWord;

            case 169:
                model_render_state.depth_bias[1] = ((cursor += 2, cursor - 2)).Read16();
                continue;

            case 170:
                visual_activation_distance = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                continue;

            case 171:
                visual_data = cursor.Aligned(4);
                visual_data.Require(10);
                resource = visual_data.Read32();
                cursor = visual_data + 10;
                reservation = AllocateZeroedObject(300);
                if (reservation)
                {
                    new (reservation) VisualRegistryEntry(resource);
                }
                continue;

            case 172:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (camera)
                    camera->vertical_offset_gain = cursor.Read16();
            SkipCommandWord:
                (cursor += 2);

                break;

            case 173:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (camera)
                {
                    camera->aim.target.SetObject(camera->follow.target.GetObject());
                }
                break;

            case 174:
                receiver = visual_lists.players;
                if (receiver)
                {
                    value = cursor.Read16();
                    duration = (cursor + 2).Read16();
                    static_cast<Player *>(visual_lists.players->owner)->fall_damage_threshold = (uint32)value;
                    static_cast<Player *>(visual_lists.players->owner)->fatal_fall_distance = (uint32)duration;
                }
            SkipCommandPair:
                cursor += (2) * 2;

                break;

            case 175:
                receiver = visual_lists.players;
                if (receiver)
                    static_cast<Player *>(visual_lists.players->owner)->fall_damage_threshold = 0;
                break;

            case 177:
                receiver = visual_lists.players;
                if (receiver)
                {
                    value = ((uint16)(((cursor += 2, cursor - 2)).Read16()));
                    static_cast<Player *>(visual_lists.players->owner)->SetWeaponStop(value);
                }
                break;

            default:
                continue;
        }
    }

    if (has_projection_config)
        return ::ConfigureProjectionDepth(projection_args[0], projection_args[1], projection_args[2]);
    return 0xFFFF;
}

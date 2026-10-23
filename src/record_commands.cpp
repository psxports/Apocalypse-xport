#include <stddef.h>
#include <new>
#include <stdlib.h>

#include "game_object_allocation.h"
#include "camera.h"
#include "player.h"
#include "visual_object.h"
#include "visual_registry_entry.h"
#include "audio_voice.h"
#include "cd_voice_event.h"
#include "model_projection.h"
#include "model_render.h"
#include "hud_text.h"
#include "level_session.h"
#include "game_diagnostics.h"
#include "record_commands.h"
#include "resource_script.h"
#include "record_groups.h"
#include "record_data.h"
#include "enemy_list.h"
#include "player_save_state.h"
#include "model_bank.h"
#include "audio_session.h"
#include "region_actions.h"
#include "game_heap.h"

static void ReadRegionBounds(RecordCursor &cursor, Vector3 &lower, Vector3 &upper)
{
    cursor = cursor.Aligned(4);
    for (uint32 axis = 0; axis < 3; ++axis)
        lower.comp[axis] = (cursor + axis * 4).Read32() << 12;
    for (uint32 axis = 0; axis < 3; ++axis)
        upper.comp[axis] = (cursor + 12 + axis * 4).Read32() << 12;
    cursor += 24;
}

RecordCursor SkipRecString(RecordCursor cursor)
{
    FUNCTION_MARKER(0x800650B4, "SLUS_003.73");
    while (cursor.Read8())
        cursor += 1;
    return (cursor + 1).Aligned(2);
}

static RecordCursor SkipRecCommand(RecordCursor cursor)
{
    uint16 command;
    FUNCTION_MARKER(0x800650EC, "SLUS_003.73");
    command = cursor.Read16();
    cursor += 2;
    switch (command)
    {
        case 2:
            while (cursor[0])
                cursor = SkipRecString(cursor);
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
            return SkipRecString(cursor);
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

static ScriptWorkspace *cur_workspace;

ScriptWorkspace *AcquireScriptWorkspace()
{
    if (cur_workspace)
        ++cur_workspace->references;
    return cur_workspace;
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
    if (cur_workspace)
        ReleaseScriptWorkspace();
    GameHeapBlock *reservation = AllocateHeapBlock(size, 1, 1);
    ScriptWorkspace *workspace = (ScriptWorkspace *)malloc(sizeof(*workspace));
    uint8 *data = (uint8 *)malloc(size ? size : 1);
    if (!workspace || !data)
    {
        GameDiagnostic(("Cannot allocate script movie workspace\n"));
        abort();
    }
    workspace->reservation = reservation;
    workspace->data = data;
    workspace->size = size;
    workspace->references = 1;
    cur_workspace = workspace;
    FUNCTION_MARKER(0x8006B70C, "SLUS_003.73");
}

void ReleaseScriptWorkspace()
{
    FUNCTION_MARKER(0x800653F4, "SLUS_003.73");
    ScriptWorkspace *workspace = cur_workspace;
    if (!workspace)
        return;
    if (workspace->reservation)
        ReleaseHeapBlock(workspace->reservation);
    workspace->reservation = 0;
    cur_workspace = 0;
    ReleaseScriptWorkspaceReference(workspace);
}

uint32 ExecuteRecCommands(RecordCursor cursor, uint32 idx, bool synchronous)
{
    GameHeapBlock *reservation;
    ResourceScriptBorrow script_borrow;
    bool has_proj_config;
    sint32 proj_args[3];
    ModelInstance *receiver;
    uint16 command;
    uint32 resource;
    RecordCursor rec_payload;
    Camera *camera;
    ResourceScript *script_rec;
    sint32 val;
    uint16 prev_start;
    uint16 duration;
    bool load_bank_active;
    RecordCursor bank_name;
    RecordCursor visual_data;
    uint16 cycles, region_enabled;
    Vector3 origin, region_end;
    FUNCTION_MARKER(0x8006541C, "SLUS_003.73");
    has_proj_config = false;
    script_rec = FindScriptRec(idx);
    while (1)
    {
        command = cursor.Read16();
        cursor += 2;
        if (command == 0xFFFF)
            break;
        switch (command)
        {
            case 2:
                checkpoint_names.Clear();
                while (cursor[0])
                {
                    checkpoint_names.Add(cursor.CStr());
                    cursor = SkipRecString(cursor);
                }

                goto SkipCommandWord;

            case 3:
                if (script_rec)
                {
                    if (script_rec->spawn_cycles_left)
                    {
                        rec_payload = GetRecPayload(idx);
                        SpawnEnemyIndices(rec_payload);
                        val = script_rec->spawn_cycles_left;
                        if (val != 0xFFFF)
                            script_rec->spawn_cycles_left = val - 1;
                    }
                }
                else
                {
                    rec_payload = GetRecPayload(idx);
                    SpawnEnemyIndices(rec_payload);
                }
                continue;

            case 4:

            case 5:
                rec_payload = GetRecPayload(idx);
                SetObjectRecGrpMode(rec_payload, command);
                continue;

            case 10:
                rec_payload = GetRecPayload(idx);
                ActivateEnemyIndices(rec_payload);
                continue;

            case 11:
                ApplyRecGrpAction(idx, 0);
                continue;

            case 12:
                ApplyRecGrpAction(idx, 1);
                continue;

            case 13:
                SetRecGrpEnabled(idx, cursor.Read16() != 0);
                goto SkipCommandWord;

            case 102:
            case 103:
                // The original flag has no remaining consumers
                continue;

            case 104:
                proj_args[0] = cursor.Read16();
                proj_args[1] = (cursor + 2).Read16();
                proj_args[2] = (cursor + 4).Read16();
                cursor += 6;
                has_proj_config = true;
                continue;

            case 105:
                val = (cursor += 2, cursor - 2).Read16();
                PlaySoundEffect(val, 0x2000, 0);
                continue;

            case 106:
                val = (cursor += 2, cursor - 2).Read16();
                ::StopSoundVoices(val);
                continue;

            case 119:
                goto SkipCommandString;

            case 126:
                bank_name = cursor;
                load_bank_active = false;
                goto QueueBankLoad;

            case 127:
                ModelBank::UnloadByName(cursor.CStr());
                goto SkipCommandString;

            case 128:
                bank_name = cursor;
                load_bank_active = true;
            QueueBankLoad:
                ModelBank::QueueLoad(bank_name.CStr(), load_bank_active);

                if (synchronous)
                    ModelBank::WaitForLoads();
                goto SkipCommandString;

            case 129:
                ModelBank::WaitForLoads();
                continue;

            case 130:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                val = (sint16)cursor.Read16();
                duration = (cursor + 2).Read16();
                cursor += 4;
                camera->SetYawTransition(val, duration);
                continue;

            case 131:
                val = (cursor += 2, cursor - 2).Read16();
                VisualRegistryEntry::Enable(val);
                continue;

            case 132:
                val = (cursor += 2, cursor - 2).Read16();
                VisualRegistryEntry::Disable(val);
                continue;

            case 133:
                while (cursor.Read16() != 255)
                {
                    ReadRegionBounds(cursor, origin, region_end);
                    ClearRegion(&origin, &region_end);
                }

                goto SkipCommandWord;

            case 134:
                if (!script_rec->spawn_cycles_initialized)
                {
                    cycles = cursor.Read16();
                    script_rec->spawn_cycles_initialized = true;
                    script_rec->spawn_cycles_left = cycles;
                }
                goto SkipCommandWord;

            case 135:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                val = cursor.Read16();
                duration = (cursor + 2).Read16();
                cursor += 4;
                camera->SetOrbitDistanceTransition(val, duration);
                continue;

            case 136:
                SetCdAudioEnabled(1);
                continue;

            case 137:
                SetCdAudioEnabled(0);
                continue;

            case 138:
                val = (cursor += 2, cursor - 2).Read16();
                SeekCdAudioEntry(val);
                continue;

            case 139:
                ::SelectCdVoice(cursor.Read16(), (cursor + 2).Read16());
                goto SkipCommandPair;

            case 140:

            case 176:
                prev_start = GetStartRecIdx();
                SelectStartRec(cursor.CStr());
                cursor = SkipRecString(cursor);
                if (prev_start != 0xFFFF && GetStartRecIdx() != prev_start)
                {
                    if (command != 176)
                    {
                        AnimatedHudText::ClearAll();
                        AnimatedHudText::Add(HudMessage::checkpoint, hud_pickup_animation);
                    }
                    CaptureCurPlayerSave();
                }
                continue;

            case 141:
                region_enabled = (cursor += 2, cursor - 2).Read16();
                while (cursor.Read16() != 255)
                {
                    ReadRegionBounds(cursor, origin, region_end);
                    SetRegionObjectsEnabled(&origin, &region_end, region_enabled != 0);
                }

                goto SkipCommandWord;

            case 142:
                SelectRecModelBank(cursor.CStr());
                cursor = SkipRecString(cursor);
                continue;

            case 143:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                val = (sint16)cursor.Read16();
                duration = (cursor + 2).Read16();
                cursor += 4;
                camera->SetOrbitHeightTransition(val, duration);
                continue;

            case 144:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                val = (sint16)cursor.Read16();
                duration = (cursor + 2).Read16();
                cursor += 4;
                camera->SetTargetLateralOffsetTransition(val, duration);
                continue;

            case 145:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                val = (sint16)cursor.Read16();
                duration = (cursor + 2).Read16();
                cursor += 4;
                camera->SetTargetHeightOffsetTransition(val, duration);
                continue;

            case 146:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                val = (sint16)cursor.Read16();
                duration = (cursor + 2).Read16();
                cursor += 4;
                camera->SetTargetForwardOffsetTransition(val, duration);
                continue;

            case 147:
                level_environment.area_kind = (cursor += 2, cursor - 2).Read16();
                continue;

            case 148:
                val = (cursor += 2, cursor - 2).Read16();
                if (script_rec->spawn_count == val)
                    continue;
                if (cursor.Read16() == 149)
                    goto SkipCommandWord;
                do
                    cursor = SkipRecCommand(cursor);
                while (cursor.Read16() != 149);
                cursor += 2;
                break;

            case 150:
                // The original fade-step word has no consumers
                cursor.Read16();
                cursor += 2;
                continue;

            case 151:
                ReleaseScriptWorkspace();
                val = (cursor += 2, cursor - 2).Read16();
                AllocateScriptWorkspace(val * 1000);
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
                val = (sint16)(cursor += 2, cursor - 2).Read16();
                camera->SetLateralColDistance(val);
                continue;

            case 154:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandWord;
                val = (sint16)(cursor += 2, cursor - 2).Read16();
                camera->SetRearColDistance(val);
                continue;

            case 155:
                (cursor += 2, cursor - 2).Read16();
                RestoreMusicPlayback();
                continue;

            case 156:
                (cursor += 2, cursor - 2).Read16();
                StopMusicSequence();
                continue;

            case 157:
                val = (cursor += 2, cursor - 2)[0];
                SetSoundReverbMode(val);
                continue;

            case 158:
                CompleteLevel(1);
                continue;

            case 159:
                LoadAudioSequence(cursor.CStr());
            SkipCommandString:
                cursor = SkipRecString(cursor);

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
                val = (cursor += 2, cursor - 2).Read16();
                camera->SetModePreset(val);
                break;

            case 163:
            {
                ModelInstance *player = visual_lists.players;
                if (player)
                {
                    val = (cursor += 2, cursor - 2).Read16();
                    static_cast<Player *>(player->owner)->disable_timer = val;
                }
                continue;
            }

            case 164:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandWord;
                val = (cursor += 2, cursor - 2).Read16();
                camera->SetColYawStrength(val);
                break;

            case 165:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandWord;
                val = (cursor += 2, cursor - 2).Read16();
                camera->SetColPitchStrength(val);
                break;

            case 166:
                model_render_state.depth_bias[0] = (cursor += 2, cursor - 2).Read16();
                continue;

            case 167:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (!camera)
                    goto SkipCommandPair;
                val = cursor.Read16();
                duration = (cursor + 2).Read16();
                cursor += 4;
                camera->SetProjDistanceTransition(val, duration);
                break;

            case 168:
                camera = visual_lists.cameras ? static_cast<Camera *>(visual_lists.cameras->owner) : 0;
                if (camera)
                    camera->vertical_pitch = cursor.Read16();
                goto SkipCommandWord;

            case 169:
                model_render_state.depth_bias[1] = (cursor += 2, cursor - 2).Read16();
                continue;

            case 170:
                visual_activation_distance = (cursor += 2, cursor - 2).Read16();
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
                cursor += 2;

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
                    val = cursor.Read16();
                    duration = (cursor + 2).Read16();
                    static_cast<Player *>(visual_lists.players->owner)->fall_damage_threshold = val;
                    static_cast<Player *>(visual_lists.players->owner)->fatal_fall_distance = duration;
                }
            SkipCommandPair:
                cursor += 4;

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
                    val = (cursor += 2, cursor - 2).Read16();
                    static_cast<Player *>(visual_lists.players->owner)->SetWeaponStop(val);
                }
                break;

            default:
                continue;
        }
    }

    if (has_proj_config)
        return ::ConfigureProjDepth(proj_args[0], proj_args[1], proj_args[2]);
    return 0xFFFF;
}

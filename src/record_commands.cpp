#include "hud_text.h"
#include "level_session.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
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

static void InitializeScriptVisual(uint32 object, uint32 resource, const uint16 prefix[3])
{
    /* Original 76C10 does not read the private prefix carrier in a2 */
    sub_80076C10(object, resource);
}

uint32 SkipRecordString(uint32 cursor)
{
    FUNCTION_MARKER(0x800650B4, "SLUS_003.73");
    if (r_u8(cursor))
    {
        do
        {
            cursor += 1;
        } while (r_u8(cursor));
    }
    cursor += 1;
    return cursor + (cursor & 1);
}

uint32 SkipRecordCommand(uint32 cursor)
{
    uint32 command;
    FUNCTION_MARKER(0x800650EC, "SLUS_003.73");
    command = r_u16(cursor);
    cursor += 2;
    switch (command)
    {
        case 2:
            while (r_u8(cursor))
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
            while (r_u16(cursor) != 255)
                cursor = ((cursor + 3) & 0xFFFFFFFC) + 24;
            return cursor + 2;
        case 171:
            return ((cursor + 3) & 0xFFFFFFFC) + 10;
        case 65535:
            return 0;
        default:
            // TODO Preserve original skip widths that differ from command execution
            return cursor;
    }
}

uint32 AllocateScriptHeap(uint32 size)
{
    uint32 allocation;
    FUNCTION_MARKER(0x800653B8, "SLUS_003.73");
    allocation = AllocateHeapBlock(size, 1, 1);
    w_u32(0x800FF63C, allocation);
    SetTemporaryHeapBounds(allocation, allocation + size);
    return allocation;
}

void ReleaseScriptHeap()
{
    uint32 allocation;
    FUNCTION_MARKER(0x800653F4, "SLUS_003.73");
    allocation = r_u32(0x800FF63C);
    if (allocation)
        ReleaseHeapBlock(allocation);
    w_u32(0x800FF63C, 0);
}

uint32 ExecuteRecordCommands(uint32 cursor, uint32 index, uint32 synchronous)
{
    sint32 final_first;
    sint32 final_second;
    sint32 has_final_call;
    uint32 receiver;
    uint32 script_record;
    sint32 v11;
    sint32 result;
    unsigned short v13;
    sint32 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    uint32 v18;
    sint32 v19;
    uint32 v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    short v27;
    uint32 v28;
    sint32 v29;
    sint32 v30;
    sint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    uint32 v35;
    sint32 v36;
    sint32 v37;
    sint32 v38;
    uint32 v39;
    sint32 v40;
    sint32 v41;
    sint32 v42;
    sint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    short v50;
    uint32 v51;
    sint32 v52;
    uint32 v53;
    uint32 v54;
    uint32 v55;
    sint32 v56;
    sint32 v57;
    sint32 v58;
    sint32 v59;
    sint32 v60;
    sint32 v61;
    sint32 v62;
    sint32 v63;
    sint32 v64;
    sint32 v65;
    sint32 v66;
    sint32 v67;
    sint32 v68;
    sint32 v69;
    sint32 v70;
    sint32 v71;
    sint32 v72;
    sint32 v73;
    sint32 v74;
    sint32 v75;
    sint32 v76;
    uint32 origin[3];
    uint16 data_prefix[3];
    int v80[4];
    int v81[4];
    sint32 final_third;
    FUNCTION_MARKER(0x8006541C, "SLUS_003.73");
    has_final_call = 0;
    script_record = FindScriptRecord(index);
    while (1)
    {
        v11 = ((unsigned short)(((sint16)(r_u16(cursor)))));
        result = 0xFFFF;
        (cursor += 2);
        if ((v11 == 0xFFFF))
            break;
        v13 = v11;
        switch (v11)
        {
            case 2:
                w_u32(0x800FF618, 0);
                while (1)
                {
                    receiver = ((sint32)(cursor));
                    if (!(r_u8(((uint32)(cursor)))))
                        break;
                    v14 = r_u32(0x800FF618);
                    w_u32(0x800A71FC + (uint32)v14 * 4, cursor);
                    v14 = (sint32)((uint32)v14 + 1);
                    w_u32(0x800FF618, v14);
                    cursor = ((uint32)(SkipRecordString(((uint32)(cursor)))));
                }

                goto LABEL_60;

            case 3:
                if (script_record)
                {
                    if (r_u16(((uint32)((script_record + 8)))))
                    {
                        v51 = ((uint32)(GetRecordPayload(index)));
                        SpawnEnemyIndices((const uint16 *)psx_addr(v51, 2));
                        v52 = r_u16(((uint32)((script_record + 8))));
                        if ((v52 != 0xFFFF))
                            w_u16(((uint32)((script_record + 8))), (v52 - 1));
                    }
                }
                else
                {
                    v53 = ((uint32)(GetRecordPayload(index)));
                    SpawnEnemyIndices((const uint16 *)psx_addr(v53, 2));
                }
                continue;

            case 4:

            case 5:
                v54 = ((uint32)(GetRecordPayload(index)));
                SetObjectRecordGroupMode(v54, v13);
                continue;

            case 10:
                v55 = ((uint32)(GetRecordPayload(index)));
                ActivateEnemyIndices((const uint16 *)psx_addr(v55, 2));
                continue;

            case 11:
                ApplyRecordGroupAction(index, 0);
                continue;

            case 12:
                ApplyRecordGroupAction(index, 1);
                continue;

            case 13:
                SetRecordGroupEnabled(index, r_u16(cursor));
                goto LABEL_60;

            case 102:
                w_u32(0x800FF7BC, 1);
                continue;

            case 103:
                w_u32(0x800FF7BC, 0);
                continue;

            case 104:
                final_first = ((unsigned short)(((sint16)(r_u16(cursor)))));
                v39 = ((uint32)((cursor + (1) * 2)));
                final_second = r_u16((v39 += 2, v39 - 2));
                v40 = r_u16(v39);
                cursor = ((uint32)((v39 + (1) * 2)));
                has_final_call = 1;
                final_third = v40;
                continue;

            case 105:
                v41 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                PlaySoundEffect(v41, 0x2000, 0);
                continue;

            case 106:
                v42 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                sub_8006A294(v42);
                continue;

            case 119:
                goto LABEL_63;

            case 126:
                v18 = cursor;
                v19 = 0;
                goto LABEL_14;

            case 127:
                UnloadModelBankByName(cursor);
                goto LABEL_63;

            case 128:
                v18 = cursor;
                v19 = 1;
            LABEL_14:
                QueueModelBankLoad(v18, v19);

                if (synchronous)
                    WaitForModelBankLoads();
                goto LABEL_63;

            case 129:
                WaitForModelBankLoads();
                continue;

            case 130:
                receiver = r_u32(0x800FF904);
                if (!receiver)
                    goto LABEL_104;
                v58 = ((sint16)(r_u16(cursor)));
                v59 = ((unsigned short)(((sint16)(r_u16((cursor + (1) * 2))))));
                cursor += (2) * 2;
                sub_8007851C(receiver, v58, v59);
                continue;

            case 131:
                v37 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                sub_80076B78(v37);
                continue;

            case 132:
                v38 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                sub_80076BC4(v38);
                continue;

            case 133:
                while ((((sint16)(r_u16(cursor))) != 255))
                {
                    v20 = ((uint32)(((((uint32)(cursor)) + 3) & 0xFFFFFFFC)));
                    v21 = r_u32((v20 += 4, v20 - 4));
                    origin[0] = (uint32)v21 << 12;
                    v22 = r_u32((v20 += 4, v20 - 4));
                    origin[1] = (uint32)v22 << 12;
                    v23 = r_u32((v20 += 4, v20 - 4));
                    origin[2] = (uint32)v23 << 12;
                    v24 = r_u32((v20 += 4, v20 - 4));
                    v80[0] = (uint32)v24 << 12;
                    v25 = r_u32((v20 += 4, v20 - 4));
                    v80[1] = (uint32)v25 << 12;
                    v26 = ((sint32)(r_u32(v20)));
                    cursor = ((uint32)((v20 + (1) * 4)));
                    v80[2] = (uint32)v26 << 12;
                    ClearRegion(origin, (const uint32 *)v80);
                }

                goto LABEL_60;

            case 134:
                if (!(r_u8(((uint32)((script_record + 6))))))
                {
                    v50 = ((sint16)(r_u16(cursor)));
                    w_u8(((uint32)((script_record + 6))), 1);
                    w_u16(((uint32)((script_record + 8))), v50);
                }
                goto LABEL_60;

            case 135:
                receiver = r_u32(0x800FF904);
                if (!receiver)
                    goto LABEL_104;
                v60 = ((unsigned short)(((sint16)(r_u16(cursor)))));
                v61 = ((unsigned short)(((sint16)(r_u16((cursor + (1) * 2))))));
                cursor += (2) * 2;
                sub_800785D8(receiver, v60, v61);
                continue;

            case 136:
                sub_8002FB18(1);
                continue;

            case 137:
                sub_8002FB18(0);
                continue;

            case 138:
                v75 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                sub_8002FBE4(v75);
                continue;

            case 139:
                sub_8002FD2C(r_u16(cursor), r_u16(cursor + 2));
                goto LABEL_104;

            case 140:

            case 176:
                v16 = r_u32(0x800FF620);
                SelectStartRecord(((sint32)(cursor)));
                cursor = ((uint32)(SkipRecordString(((uint32)(cursor)))));
                if (((v16 != 0xFFFF) && (r_u32(0x800FF620) != v16)))
                {
                    if ((v13 != 176))
                    {
                        ClearAnimatedHudText();
                        sub_8001B788(((sint32)(r_u32((0x800A5720 + (0) * 4)))), ((sint32)(0x800A57A8)));
                    }
                    CaptureCurrentPlayerSave();
                }
                continue;

            case 141:
                v27 = r_u16((cursor += 2, cursor - 2));
                while ((((sint16)(r_u16(cursor))) != 255))
                {
                    v28 = ((uint32)(((((uint32)(cursor)) + 3) & 0xFFFFFFFC)));
                    v29 = r_u32((v28 += 4, v28 - 4));
                    origin[0] = (uint32)v29 << 12;
                    v30 = r_u32((v28 += 4, v28 - 4));
                    origin[1] = (uint32)v30 << 12;
                    v31 = r_u32((v28 += 4, v28 - 4));
                    origin[2] = (uint32)v31 << 12;
                    v32 = r_u32((v28 += 4, v28 - 4));
                    v81[0] = (uint32)v32 << 12;
                    v33 = r_u32((v28 += 4, v28 - 4));
                    v81[1] = (uint32)v33 << 12;
                    v34 = ((sint32)(r_u32(v28)));
                    cursor = ((uint32)((v28 + (1) * 4)));
                    v81[2] = (uint32)v34 << 12;
                    SetRegionObjectsEnabled(origin, (const uint32 *)v81, v27 != 0);
                }

                goto LABEL_60;

            case 142:
                w_u32(0x800FF62C, ((sint32)(cursor)));
                cursor = ((uint32)(SkipRecordString(((uint32)(cursor)))));
                w_u8(0x800FF644, FindModelBankByName(r_u32(0x800FF62C)));
                continue;

            case 143:
                receiver = r_u32(0x800FF904);
                if (!receiver)
                    goto LABEL_104;
                v62 = ((sint16)(r_u16(cursor)));
                v63 = ((unsigned short)(((sint16)(r_u16((cursor + (1) * 2))))));
                cursor += (2) * 2;
                sub_80078618(receiver, v62, v63);
                continue;

            case 144:
                receiver = r_u32(0x800FF904);
                if (!receiver)
                    goto LABEL_104;
                v64 = ((sint16)(r_u16(cursor)));
                v65 = ((unsigned short)(((sint16)(r_u16((cursor + (1) * 2))))));
                cursor += (2) * 2;
                sub_8007865C(receiver, v64, v65);
                continue;

            case 145:
                receiver = r_u32(0x800FF904);
                if (!receiver)
                    goto LABEL_104;
                v66 = ((sint16)(r_u16(cursor)));
                v67 = ((unsigned short)(((sint16)(r_u16((cursor + (1) * 2))))));
                cursor += (2) * 2;
                sub_800786A0(receiver, v66, v67);
                continue;

            case 146:
                receiver = r_u32(0x800FF904);
                if (!receiver)
                    goto LABEL_104;
                v68 = ((sint16)(r_u16(cursor)));
                v69 = ((unsigned short)(((sint16)(r_u16((cursor + (1) * 2))))));
                cursor += (2) * 2;
                sub_800786E4(receiver, v68, v69);
                continue;

            case 147:
                w_u32(0x800FF378, ((unsigned short)(r_u16((cursor += 2, cursor - 2)))));
                continue;

            case 148:
                v49 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                if ((r_u8(((uint32)((script_record + 7)))) == v49))
                    continue;
                if ((((sint16)(r_u16(cursor))) == 149))
                    goto LABEL_60;
                do
                    cursor = ((uint32)(SkipRecordCommand(((uint32)(cursor)))));
                while ((((sint16)(r_u16(cursor))) != 149));
                (cursor += 2);
                break;

            case 150:
                v43 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                sub_8006A490(v43);
                continue;

            case 151:
                ReleaseScriptHeap();
                v15 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                AllocateScriptHeap((uint32)v15 * 1000);
                continue;

            case 152:
                receiver = r_u32(0x800FF5A0);
                if (receiver)
                    sub_8005E708(receiver);
                continue;

            case 153:
                receiver = r_u32(0x800FF904);
                if (!receiver)
                    goto LABEL_60;
                v70 = (uint32)(sint32)r_s16((cursor += 2, cursor - 2));
                sub_8007749C(receiver, v70);
                continue;

            case 154:
                receiver = r_u32(0x800FF904);
                if (!receiver)
                    goto LABEL_60;
                v71 = (uint32)(sint32)r_s16((cursor += 2, cursor - 2));
                sub_800774A4(receiver, v71);
                continue;

            case 155:
                v44 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                RestoreMusicPlayback();
                continue;

            case 156:
                v45 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                sub_8006A3C4();
                continue;

            case 157:
                v46 = r_u8(((uint32)((cursor += 2, cursor - 2))));
                SetSoundReverbMode(v46);
                continue;

            case 158:
                CompleteLevel(1);
                continue;

            case 159:
                LoadAudioSequence(cursor);
            LABEL_63:
                cursor = ((uint32)(SkipRecordString(((uint32)(cursor)))));

                continue;

            case 160:
                receiver = r_u32(0x800FF904);
                if (receiver)
                    w_u32(receiver + 564, r_u16(cursor));
                goto LABEL_60;

            case 161:
                receiver = r_u32(0x800FF904);
                if (!receiver)
                    goto LABEL_60;
                v74 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                sub_80077624(receiver, v74);
                break;

            case 163:
                receiver = r_u32(0x800FF5A0);
                if (receiver)
                {
                    v47 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                    w_u32(receiver + 452, v47);
                }
                continue;

            case 164:
                receiver = r_u32(0x800FF904);
                if (!receiver)
                    goto LABEL_60;
                v72 = r_u16((cursor += 2, cursor - 2));
                sub_800774AC(receiver, v72);
                break;

            case 165:
                receiver = r_u32(0x800FF904);
                if (!receiver)
                    goto LABEL_60;
                v73 = r_u16((cursor += 2, cursor - 2));
                sub_800774B4(receiver, v73);
                break;

            case 166:
                w_u16(0x800FFAAC, r_u16((cursor += 2, cursor - 2)));
                continue;

            case 167:
                receiver = r_u32(0x800FF904);
                if (!receiver)
                    goto LABEL_104;
                v56 = ((unsigned short)(((sint16)(r_u16(cursor)))));
                v57 = ((unsigned short)(((sint16)(r_u16((cursor + (1) * 2))))));
                cursor += (2) * 2;
                sub_80078728(receiver, v56, v57);
                break;

            case 168:
                receiver = r_u32(0x800FF904);
                if (receiver)
                    w_u16(receiver + 412, r_u16(cursor));
                goto LABEL_60;

            case 169:
                w_u16(0x800FFAAE, r_u16((cursor += 2, cursor - 2)));
                continue;

            case 170:
                w_u32(0x800FF5EC, ((unsigned short)(r_u16((cursor += 2, cursor - 2)))));
                continue;

            case 171:
                v35 = (cursor + 3) & ~3;
                data_prefix[0] = r_u16(v35 + 4);
                v36 = r_u32(v35);
                data_prefix[1] = r_u16(v35 + 6);
                data_prefix[2] = r_u16(v35 + 8);
                cursor = v35 + 10;
                receiver = sub_800625AC(300);
                if (receiver)
                    InitializeScriptVisual(receiver, v36, data_prefix);
                continue;

            case 172:
                receiver = r_u32(0x800FF904);
                if (receiver)
                    w_u16(receiver + 414, r_u16(cursor));
            LABEL_60:
                (cursor += 2);

                break;

            case 173:
                receiver = r_u32(0x800FF904);
                if (receiver)
                {
                    v76 = r_u32(receiver + 296);
                    w_u32(receiver + 336, v76);
                }
                break;

            case 174:
                receiver = r_u32(0x800FF5A0);
                if (receiver)
                {
                    v75 = r_u16(cursor);
                    v76 = r_u16(cursor + 2);
                    w_u32(receiver + 484, (uint32)v75);
                    w_u32(receiver + 488, (uint32)v76);
                }
            LABEL_104:
                cursor += (2) * 2;

                break;

            case 175:
                receiver = r_u32(0x800FF5A0);
                if (receiver)
                    w_u32(receiver + 484, 0);
                break;

            case 177:
                receiver = r_u32(0x800FF5A0);
                if (receiver)
                {
                    v48 = ((unsigned short)(r_u16((cursor += 2, cursor - 2))));
                    sub_8005F62C(receiver, v48);
                }
                break;

            default:
                continue;
        }
    }

    if (has_final_call)
        return sub_8007D76C(final_first, final_second, final_third);
    return result;
}

extern "C" uint32 sub_8006541C(uint32 cursor, uint32 index, uint32 synchronous)
{
    FUNCTION_MARKER(0x8006541C, "SLUS_003.73");
    return ExecuteRecordCommands(cursor, index, synchronous);
}

extern "C" uint32 sub_800650B4(uint32 cursor)
{
    FUNCTION_MARKER(0x800650B4, "SLUS_003.73");
    return SkipRecordString(cursor);
}
extern "C" uint32 sub_800650EC(uint32 cursor)
{
    FUNCTION_MARKER(0x800650EC, "SLUS_003.73");
    return SkipRecordCommand(cursor);
}

extern "C" uint32 sub_800653B8(uint32 size)
{
    FUNCTION_MARKER(0x800653B8, "SLUS_003.73");
    return AllocateScriptHeap(size);
}
extern "C" void sub_800653F4(void)
{
    FUNCTION_MARKER(0x800653F4, "SLUS_003.73");
    ReleaseScriptHeap();
}

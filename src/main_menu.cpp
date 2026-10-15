#include "main_menu.h"
#include "game_panel.h"
#include "menu_input.h"
#include "credits.h"
#include "card_save.h"
#include "menu_saves.h"
#include "menu_window.h"
#include "menu_lifecycle.h"
#include "scene_lifecycle.h"
#include "hud_text.h"
extern "C"
{
#include "game_menu_helpers.h"
#include "game_sound_native.h"
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// TODO Card BIOS file calls retain explicit missing boundaries
uint32 RunMainMenu()
{
    uint32 v0;
    sint32 v1;
    sint32 v2;
    sint32 v3;
    sint32 v4;
    uint32 v5;
    sint32 main_window;
    sint32 v7;
    uint32 v8;
    uint32 v9;
    sint32 v10;
    sint32 v11;
    uint32 v12;
    sint32 v13;
    sint32 v14;
    uint32 controller_window;
    sint32 v16;
    sint32 preset_window;
    sint32 menu_state;
    sint32 v19;
    uint32 v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    uint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 v28;
    uint32 v29;
    uint32 v30;
    uint32 v31;
    uint32 v32;
    sint32 v33;
    sint32 v34;
    uint32 v35;
    uint32 v36;
    uint32 v37;
    sint32 v38;
    sint32 v39;
    uint32 v40;
    uint32 v41;
    sint32 v42;
    sint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    uint32 v47;
    uint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    sint32 v52;
    sint32 v53;
    sint32 v54;
    sint32 v55;
    uint32 v56;
    uint32 v57;
    sint32 v58;
    sint32 v59;
    uint32 v60;
    uint32 v61;
    sint32 v62;
    sint32 v63;
    sint32 v64;
    sint32 v65;
    uint32 v66;
    uint32 v67;
    sint32 v68;
    sint32 v69;
    sint32 v70;
    sint32 v71;
    uint32 v72;
    uint32 v73;
    sint32 v74;
    sint32 v75;
    sint32 v76;
    sint32 v77;
    sint32 v78;
    uint32 v79;
    sint32 v80;
    sint32 v81;
    uint32 v82;
    sint32 v83;
    sint32 v84;
    uint32 v85;
    uint32 v86;
    uint32 v87;
    uint32 v88;
    sint32 v89;
    sint32 v90;
    sint32 v91;
    sint32 v92;
    sint32 v94;
    sint32 v95;
    sint32 v96;
    uint32 v97;
    char v98[16];
    sint32 v99;
    sint32 v100;
    uint32 v101;
    uint32 v102;
    uint32 v103;
    uint32 v104;
    sint32 result_state;
    sint32 finished;
    uint32 background_menu;
    sint32 level_window;
    sint32 options_window;
    sint32 extras_window;
    uint32 difficulty_window;
    sint32 audio_window;
    uint32 save_window;
    sint32 v114;
    sint32 v115;
    sint32 v116;
    sint32 v117;
    sint32 v118;
    sint32 v119;
    sint32 v120;
    sint32 v121;
    sint32 v122;
    sint32 v123;
    sint32 v124;
    sint32 v125;
    sint32 v126;
    sint32 v127;
    sint32 v128;
    sint32 v129;
    FUNCTION_MARKER(0x80011BA0, "SLUS_003.73");
    result_state = 0;
    finished = 0;
    BeginSceneFrame();
    SubmitSceneFrame();
    sub_8001024C(0, ((sint32)(0x800A0310)), 240);
    w_u32(0x800FF660, ((sint32)(0x800C6310)));
    BeginSceneFrame();
    UpdateScenePacketLimit();
    SetHudTextScale(256);
    SetHudTextAlignment(0);
    SetHudTextColor(128, 128, 128, 0);
    v94 = 256;
    sub_8001AA28(256, 120, r_u32(0x800A54EC), 0, 0, 256);
    w_u8(0x800A72B8, 0);
    w_u8(0x800C6328, 0);
    SubmitSceneFrame();
    DrawSync(0);
    w_u8(0x800A72B8, 1);
    w_u8(0x800C6328, 1);
    PutDispEnv((DISPENV *)psx_addr(0x800A72FC, sizeof(DISPENV)));
    v0 = ((uint32)(0x800A5374));
    v1 = 0;
    if (r_u8(r_u32(0x800A5374)))
    {
        do
        {
            v0 += (8) * 4;
            ++v1;
        } while (r_u8(r_u32(v0)));
    }
    if (((w_u32(0x800FEFF0, (r_u32(0x800FEFF0) + 1)), (sint32)r_u32(0x800FEFF0)) >= ((sint32)(v1))))
        w_u32(0x800FEFF0, 0);
    // Skip the original title-screen input and prompt delay
    w_u32(0x800FF03C, 0);
    v2 = 144;
    sub_80011B34();
    v3 = 0;
    sub_80070748();
    w_u32(0x800FF814, 0);
    v4 = (r_u8(0x800EC1D8) == 0);
    do
    {
        sub_80070748();
        if (r_u32(0x800FF03C))
        {
            (w_u32(0x800FF03C, (r_u32(0x800FF03C) - 1)), r_u32(0x800FF03C));
        }
        else
        {
            if (!r_u8(0x800EC1D8))
                v4 = 1;
            if ((v4 && r_u8(0x800EC1D8)))
            {
                sub_80069DF0(23, 0x2000, 0);
                goto LABEL_22;
            }
            if (v2)
            {
                PutDispEnv((DISPENV *)psx_addr(0x800C636C, sizeof(DISPENV)));
                if (!(--v2))
                    v3 = 24;
            }
            else
            {
                PutDispEnv((DISPENV *)psx_addr(0x800A72FC, sizeof(DISPENV)));
            }
            if (v3)
            {
                if (!(--v3))
                    v2 = 144;
            }
        }
        sub_8006B2A8();
        sub_8006F500();
        nullsub_16();
        sub_800664E4(1);
    } while (((sint32)r_u32(0x800FF814) < 1801));
    result_state = 3;
    finished = 1;
LABEL_22:
    w_u8(0x800EC1D9, 0);

    w_u8(0x800EC129, 0);
    sub_80015EC8();
    sub_8006F7D8();
    sub_8002E148(951706923, 64, 64, 127);
    sub_8007D76C(10, 10240, 0x2000);
    v5 = ((uint32)(sub_8002FED8(28)));
    background_menu = v5;
    if (v5)
        background_menu = sub_80011904(v5, (0x800A537C + (((uint32)(8) * (uint32)(r_u32(0x800FEFF0)))) * 4), r_u32((0x800A538C + (((uint32)(8) * (uint32)(r_u32(0x800FEFF0)))) * 4)), r_u32((0x800A5390 + (((uint32)(8) * (uint32)(r_u32(0x800FEFF0)))) * 4)));
    main_window = sub_8002FED8(444);
    if (main_window)
    {
        v94 = 256;
        v95 = 26;
        main_window = sub_80015228(main_window, 256, 0, 0, 0x140, 0x100, 0x1A);
    }
    sub_80015474(main_window, r_u32((0x800A554C + (0) * 4)));
    if ((!r_u32(0x800A53D4) || sub_80067724(0x800A5460, r_u32(0x800A5258))))
        sub_8001551C(main_window, r_u32((0x800A554C + (0) * 4)));
    sub_80015474(main_window, r_u32((0x800A55D0 + (0) * 4)));
    if (r_u32(0x800FF334))
        sub_80015474(main_window, 0x800A031C);
    sub_80015474(main_window, r_u32((0x800A55D4 + (0) * 4)));
    sub_80015474(main_window, r_u32((0x800A55D8 + (0) * 4)));
    if (r_u32(0x800FF330))
        sub_80015474(main_window, r_u32((0x800A557C + (0) * 4)));
    sub_800152F8(main_window);
    v7 = sub_8002FED8(444);
    level_window = v7;
    if (v7)
    {
        v94 = 192;
        v95 = 26;
        level_window = sub_80015228(v7, 256, 0, 0, 0x100, 0xC0, 0x1A);
    }
    w_u32(((uint32)(((uint32)(level_window) + (uint32)(20)))), 18);
    v8 = ((uint32)(0x800A5254));
    if (r_u8(r_u32(0x800A5254)))
    {
        v9 = 0x800A5254;
        do
        {
            v8 += (6) * 4;
            sub_80015474(level_window, r_u32(v9));
            v9 = v8;
        } while (r_u8(r_u32(v8)));
    }
    sub_800152F8(level_window);
    v10 = sub_8002FED8(444);
    options_window = v10;
    if (v10)
    {
        v94 = 256;
        v95 = 26;
        options_window = sub_80015228(v10, 256, 63, 0, 0x140, 0x100, 0x1A);
    }
    sub_80015474(options_window, r_u32((0x800A55DC + (0) * 4)));
    sub_80015474(options_window, r_u32((0x800A55E0 + (0) * 4)));
    sub_80015474(options_window, r_u32((0x800A55E4 + (0) * 4)));
    sub_80015474(options_window, r_u32((0x800A55E8 + (0) * 4)));
    sub_80015474(options_window, r_u32((0x800A55EC + (0) * 4)));
    v11 = sub_8002FED8(444);
    extras_window = v11;
    if (v11)
    {
        v94 = 256;
        v95 = 26;
        extras_window = sub_80015228(v11, 256, 42, 0, 0x140, 0x100, 0x1A);
    }
    sub_80015474(extras_window, r_u32((0x800A55F0 + (0) * 4)));
    sub_80015474(extras_window, r_u32((0x800A55F4 + (0) * 4)));
    sub_80015474(extras_window, r_u32((0x800A55F8 + (0) * 4)));
    sub_80015474(extras_window, r_u32((0x800A55FC + (0) * 4)));
    sub_80015474(extras_window, r_u32((0x800A5600 + (0) * 4)));
    sub_80015474(extras_window, r_u32((0x800A55A4 + (0) * 4)));
    v12 = ((uint32)(sub_8002FED8(444)));
    difficulty_window = v12;
    if (v12)
    {
        v94 = 256;
        v95 = 26;
        difficulty_window = ((uint32)(sub_80015228(v12, 256, 55, 0, 0x140, 0x100, 0x1A)));
    }
    sub_80015474(difficulty_window, r_u32((0x800A5648 + (0) * 4)));
    sub_80015474(difficulty_window, r_u32((0x800A564C + (0) * 4)));
    sub_80015474(difficulty_window, r_u32((0x800A5650 + (0) * 4)));
    sub_80015474(difficulty_window, r_u32((0x800A55A4 + (0) * 4)));
    w_u8((difficulty_window + (7) * 1), 1);
    v13 = sub_8002FED8(444);
    audio_window = v13;
    if (v13)
    {
        v94 = 256;
        v95 = 26;
        audio_window = sub_80015228(v13, 256, 85, 0, 0x140, 0x100, 0x1A);
    }
    sub_80015474(audio_window, r_u32((0x800A55A8 + (0) * 4)));
    sub_80015474(audio_window, r_u32((0x800A55AC + (0) * 4)));
    sub_80015474(audio_window, r_u32((0x800A55B0 + (0) * 4)));
    sub_80015474(audio_window, r_u32((0x800A55A4 + (0) * 4)));
    v14 = sub_8002FED8(444);
    controller_window = ((uint32)(v14));
    if (v14)
    {
        v94 = 192;
        v95 = 26;
        controller_window = ((uint32)(sub_80015228(v14, 30, 25, 1, 0xC0, 0xC0, 0x1A)));
    }
    sub_80015474(controller_window, r_u32((0x800A5654 + (0) * 4)));
    sub_80015474(controller_window, r_u32((0x800A5658 + (0) * 4)));
    sub_80015474(controller_window, r_u32((0x800A565C + (0) * 4)));
    sub_80015474(controller_window, r_u32((0x800A5730 + (0) * 4)));
    sub_80015474(controller_window, r_u32((0x800A5608 + (0) * 4)));
    sub_80015474(controller_window, r_u32((0x800A5604 + (0) * 4)));
    sub_80015590(controller_window, r_u32((0x800A5608 + (0) * 4)), 10);
    sub_8001551C(controller_window, r_u32((0x800A5608 + (0) * 4)));
    sub_8001551C(controller_window, r_u32((0x800A5604 + (0) * 4)));
    sub_80015474(controller_window, r_u32((0x800A55FC + (0) * 4)));
    sub_80015474(controller_window, r_u32((0x800A55A4 + (0) * 4)));
    w_u32((controller_window + (5) * 4), 22);
    v16 = sub_8002FED8(444);
    preset_window = v16;
    if (v16)
    {
        v94 = 256;
        v95 = 26;
        preset_window = sub_80015228(v16, 256, 77, 0, 0x140, 0x100, 0x1A);
    }
    save_window = 0;
    v114 = 0;
    v118 = 0;
    v119 = 0;
    menu_state = 0;
    sub_80015474(preset_window, r_u32((0x800A560C + (0) * 4)));
    sub_80015474(preset_window, r_u32((0x800A5610 + (0) * 4)));
    sub_80015474(preset_window, r_u32((0x800A5614 + (0) * 4)));
    sub_80015474(preset_window, r_u32((0x800A5618 + (0) * 4)));
    w_u16(((uint32)(((uint32)(preset_window) + (uint32)(118)))), 192);
    w_u16(((uint32)(((uint32)(preset_window) + (uint32)(116)))), 192);
    w_u8(((uint32)(((uint32)(preset_window) + (uint32)(7)))), 1);
    sub_80016014(0x800A54CC);
    w_u32(0x800FF814, 0);
    v128 = r_u32(0x800FF64C);
    while (!finished)
    {
        nullsub_16();
        BeginSceneFrame();
        UpdateScenePacketLimit();
        v129 = r_u32(0x800FF64C);
        if (v118)
        {
            if (!(--v118))
            {
                w_u16(0x800A7304, v116);
                w_u16(0x800C6374, v116);
                w_u16(0x800A7306, v117);
                w_u16(0x800C6376, v117);
            }
        }
        sub_8002E2E8();
        sub_8002E2E8();
        UpdateHudTextGlowCurve();
        sub_800858FC(0x800ED548, 0x800ED550);
        sub_800878AC(0x800ED550, 0x800ED520);
        sub_8007E63C(0x800ED51C, 0x800A67B8, ((uint32)(r_u32(0x800FF660)) + (uint32)(112)));
        sub_8007EBDC(r_u32(0x800FF794));
        sub_8007EBC4();
        SetHudTextScale(256);
        SetHudTextAlignment(0);
        switch (menu_state)
        {
            case 0:
                MenuWindow::Attach(main_window).Draw();
                SetHudTextScale(256);
                if (r_u32(0x800FF348))
                {
                    SetHudTextColor(149, 20, 20, 0);
                    v19 = 20;
                    v20 = 0x800A0328;
                }
                else
                {
                    SetHudTextColor(149, 20, 20, 0);
                    v19 = 28;
                    v20 = r_u32((0x800A54E4 + (0) * 4));
                }
                v94 = 256;
                sub_8001AA28(256, v19, v20, 0, 0, 256);
                sub_80016800();
                break;

            case 1:
                MenuWindow::Attach(level_window).Draw();
                break;

            case 3:
                v21 = options_window;
                goto LABEL_63;

            case 4:
                v21 = extras_window;
            LABEL_63:
                MenuWindow::Attach(v21).Draw();

                sub_800167D8();
                break;

            case 5:
                sub_800151FC();
                sub_8001AA28(256, 80, r_u32((0x800A5620 + (0) * 4)), 0, 0, 256);
                sub_8001AA28(256, 106, r_u32((0x800A5624 + (0) * 4)), 0, 0, 256);
                sub_8001AA28(256, 132, r_u32((0x800A5628 + (0) * 4)), 0, 0, 256);
                SetHudTextAlignment(1);
                sub_80011594();
                sub_8001AA28(104, 210, r_u32(0x800A5588), 0, 0, 256);
                sub_8001AA28(328, 210, r_u32((0x800A5584 + (0) * 4)), 0, 0, 256);
                v22 = sub_8006D028(((uint32)(r_u32(0x800FF01C)) + (uint32)(8)));
                if (v22)
                    sub_8006D0D4(76, 211, v22, ((uint32)(r_u32(0x800FF01C)) + (uint32)(8)));
                v23 = sub_8006D028(r_u32(0x800FF01C));
                if (v23)
                    sub_8006D0D4(300, 211, v23, r_u32(0x800FF01C));
                sub_8006D1C0(0, 0, 512, 5, 0, 0, 255, 0);
                sub_8006D1C0(0, 235, 512, 5, 0, 0, 255, 0);
                sub_8006D1C0(0, 0, 7, 240, 0, 0, 255, 0);
                v94 = 0;
                v95 = 255;
                v96 = 0;
                sub_8006D1C0(505, 0, 7, 240, 0, 0, 255, 0);
                break;

            case 6:
                sub_800167D8();
                MenuWindow::Attach(difficulty_window).Draw();
                break;

            case 7:
                v24 = 0;
                sub_800166AC();
                SetHudTextAlignment(1);
                SetHudTextScale(192);
                if ((sub_800155D4(audio_window, r_u32((0x800A55A8 + (0) * 4))) || sub_800155D4(audio_window, r_u32((0x800A55B0 + (0) * 4)))))
                    v24 = 1;
                if (v24)
                {
                    v25 = r_u32((0x800A55B4 + (0) * 4));
                }
                else if (sub_800155D4(audio_window, r_u32((0x800A55AC + (0) * 4))))
                {
                    v25 = r_u32(0x800A55B8);
                }
                else
                {
                    v25 = r_u32((0x800A5578 + (0) * 4));
                }
                v94 = 256;
                sub_8001AA28(348, 210, v25, 0, 0, 256);
                SetHudTextAlignment(0);
                v26 = sub_8006D028(r_u32(0x800FF01C));
                if (v26)
                    sub_8006D0D4(320, 211, v26, r_u32(0x800FF01C));
                DrawMenuVolumeSlider(MenuWindow::Attach(audio_window));
                MenuWindow::Attach(audio_window).Draw();
                break;

            case 8:
                MenuWindow::Attach(controller_window).Draw();
                v27 = 0;
                if ((((sub_800155D4(controller_window, r_u32((0x800A5654 + (0) * 4))) || sub_800155D4(controller_window, r_u32((0x800A5658 + (0) * 4)))) || sub_800155D4(controller_window, r_u32((0x800A565C + (0) * 4)))) || sub_800155D4(controller_window, r_u32((0x800A5730 + (0) * 4)))))
                {
                    v27 = 1;
                }
                if (v27)
                {
                    SetHudTextAlignment(1);
                    SetHudTextScale(256);
                    sub_80011594();
                    sub_8001AA28(211, 210, r_u32((0x800A5574 + (0) * 4)), 0, 0, 256);
                    v28 = sub_8006D028(((uint32)(r_u32(0x800FF01C)) + (uint32)(8)));
                    if (v28)
                        sub_8006D0D4(183, 211, v28, ((uint32)(r_u32(0x800FF01C)) + (uint32)(8)));
                }
                else
                {
                    sub_800167D8();
                }
                SetHudTextAlignment(1);
                sub_800151D0();
                SetHudTextScale(192);
                if ((r_u32(0x800EC264) == 65))
                {
                    xport_draft_host_sub_80015614_p34(controller_window, r_u32((0x800A5654 + (0) * 4)), &v99, &v100);
                    v29 = sub_800117D8((sint16)r_u16(0x800EC47A));
                    sub_8001AA28(((uint32)(v99) + (uint32)(315)), v100, v29, 0, 0, 256);
                    xport_draft_host_sub_80015614_p34(controller_window, r_u32((0x800A5658 + (0) * 4)), &v99, &v100);
                    v30 = sub_800117D8((sint16)r_u16(0x800EC474));
                    sub_8001AA28(((uint32)(v99) + (uint32)(315)), v100, v30, 0, 0, 256);
                    xport_draft_host_sub_80015614_p34(controller_window, r_u32((0x800A565C + (0) * 4)), &v99, &v100);
                    v31 = sub_800117D8((sint16)r_u16(0x800EC476));
                    sub_8001AA28(((uint32)(v99) + (uint32)(315)), v100, v31, 0, 0, 256);
                    xport_draft_host_sub_80015614_p34(controller_window, r_u32((0x800A5730 + (0) * 4)), &v99, &v100);
                    v32 = ((uint32)(sub_800117D8((sint16)r_u16(0x800EC478))));
                }
                else
                {
                    xport_draft_host_sub_80015614_p34(controller_window, r_u32((0x800A5654 + (0) * 4)), &v99, &v100);
                    v35 = sub_800117D8((sint16)r_u16(0x800EC49A));
                    sub_8001AA28(((uint32)(v99) + (uint32)(315)), v100, v35, 0, 0, 256);
                    xport_draft_host_sub_80015614_p34(controller_window, r_u32((0x800A5658 + (0) * 4)), &v99, &v100);
                    v36 = sub_800117D8((sint16)r_u16(0x800EC494));
                    sub_8001AA28(((uint32)(v99) + (uint32)(315)), v100, v36, 0, 0, 256);
                    xport_draft_host_sub_80015614_p34(controller_window, r_u32((0x800A565C + (0) * 4)), &v99, &v100);
                    v37 = sub_800117D8((sint16)r_u16(0x800EC496));
                    sub_8001AA28(((uint32)(v99) + (uint32)(315)), v100, v37, 0, 0, 256);
                    xport_draft_host_sub_80015614_p34(controller_window, r_u32((0x800A5730 + (0) * 4)), &v99, &v100);
                    v32 = ((uint32)(sub_800117D8((sint16)r_u16(0x800EC498))));
                }
                v33 = v100;
                v94 = 256;
                v34 = ((uint32)(v99) + (uint32)(315));
                goto LABEL_104;

            case 9:
                sub_800151FC();
                sub_8001AA28(256, 80, r_u32((0x800A562C + (0) * 4)), 0, 0, 256);
                sub_8001AA28(256, 106, r_u32((0x800A5630 + (0) * 4)), 0, 0, 256);
                sub_8001AA28(256, 132, r_u32((0x800A5634 + (0) * 4)), 0, 0, 256);
                v34 = 256;
                v32 = r_u32((0x800A5638 + (0) * 4));
                v33 = 158;
                goto LABEL_103;

            case 10:
                sub_800151FC();
                sub_8001AA28(256, 90, r_u32((0x800A563C + (0) * 4)), 0, 0, 256);
                sub_8001AA28(256, 116, r_u32((0x800A5640 + (0) * 4)), 0, 0, 256);
                v34 = 256;
                v32 = r_u32((0x800A5644 + (0) * 4));
                v33 = 142;
                goto LABEL_103;

            case 11:
                sub_80011594();
                v94 = 256;
                sub_8001AA28(256, 34, r_u32((0x800A5608 + (0) * 4)), 0, 0, 256);
                sub_800167D8();
                MenuWindow::Attach(preset_window).Draw();
                break;

            case 12:
                sub_800167D8();
                sub_80011594();
                v94 = 256;
                sub_8001AA28(256, 80, r_u32((0x800A55A0 + (0) * 4)), 0, 0, 256);
                v38 = r_u32(0x800FF024);
                w_u32(((uint32)(((uint32)(r_u32(0x800FF024)) + (uint32)(16)))), 131);
                MenuWindow::Attach(v38).Draw();
                break;

            case 13:

            case 14:
                sub_800151FC();
                sub_8001AA28(256, 90, r_u32((0x800A56B4 + (0) * 4)), 0, 0, 256);
                sub_800151D0();
                sub_8001AA28(256, 140, r_u32((0x800A56AC + (0) * 4)), 0, 0, 256);
                v34 = 256;
                v32 = r_u32((0x800A56B0 + (0) * 4));
                v33 = 166;
                goto LABEL_103;

            case 15:
                sub_800151D0();
                sub_8001AA28(256, 90, r_u32((0x800A5710 + (0) * 4)), 0, 0, 256);
                sub_8001AA28(256, 116, r_u32((0x800A5714 + (0) * 4)), 0, 0, 256);
                sub_8001AA28(256, 142, r_u32((0x800A5718 + (0) * 4)), 0, 0, 256);
                v34 = 256;
                v33 = 168;
                v32 = r_u32((0x800A571C + (0) * 4));
                goto LABEL_103;

            case 16:
                sub_800151D0();
                sub_8001AA28(256, 90, r_u32((0x800A5674 + (0) * 4)), 0, 0, 256);
                sub_8001AA28(256, 116, r_u32((0x800A5678 + (0) * 4)), 0, 0, 256);
                sub_8001AA28(256, 142, r_u32((0x800A567C + (0) * 4)), 0, 0, 256);
                v34 = 256;
                v32 = r_u32(0x800A5680);
                v33 = 168;
                goto LABEL_103;

            case 17:
                sub_800167D8();
                sub_800151D0();
                v94 = 256;
                sub_8001AA28(256, 25, r_u32((0x800A55D4 + (0) * 4)), 0, 0, 256);
                MenuWindow::Attach(save_window).Draw();
                break;

            case 18:
                sub_800151D0();
                sub_8001AA28(256, 90, r_u32((0x800A5664 + (0) * 4)), 0, 0, 256);
                sub_8001AA28(256, 116, r_u32((0x800A5668 + (0) * 4)), 0, 0, 256);
                sub_8001AA28(256, 142, r_u32((0x800A566C + (0) * 4)), 0, 0, 256);
                v34 = 256;
                v32 = r_u32((0x800A5670 + (0) * 4));
                v33 = 168;
            LABEL_103:
                v94 = 256;

            LABEL_104:
                sub_8001AA28(v34, v33, v32, 0, 0, 256);

                break;

            default:
                break;
        }

        if ((((w_u32(0x800FF2F0, (r_u32(0x800FF2F0) + 1)), r_u32(0x800FF2F0)) & 1) != 0))
            UpdateCardStatus(0, 0);
        sub_80070748();
        if (menu_state)
            v128 = r_u32(0x800FF64C);
        ReadMenuInput(&v101, &v102, &v103, &v104);
        v39 = -1;
        if (v101)
            v39 = 23;
        if (v102)
            v39 = 19;
        switch (menu_state)
        {
            case 0:
                MenuWindow::Attach(main_window).Update();
                if (v101)
                {
                    if (sub_800155D4(main_window, r_u32((0x800A554C + (0) * 4))))
                    {
                        v40 = 0x800A5460;
                        xport_draft_host_sub_80067808_p2(0x800A5460, v98);
                        v41 = 0x800A0518;
                        do
                        {
                            v42 = ((sint32)(r_u32((v41 + (1) * 4))));
                            v43 = ((sint32)(r_u32((v41 + (2) * 4))));
                            v44 = ((sint32)(r_u32((v41 + (3) * 4))));
                            w_u32(((uint32)(v40)), ((sint32)(r_u32(v41))));
                            w_u32((((uint32)(v40)) + (1) * 4), v42);
                            w_u32((((uint32)(v40)) + (2) * 4), v43);
                            w_u32((((uint32)(v40)) + (3) * 4), v44);
                            v41 += (4) * 4;
                            v40 += (16) * 1;
                        } while ((v41 != 0x800A0578));
                        v45 = ((sint32)(r_u32((v41 + (1) * 4))));
                        v46 = ((sint32)(r_u32((v41 + (2) * 4))));
                        w_u32(((uint32)(v40)), ((sint32)(r_u32(v41))));
                        w_u32((((uint32)(v40)) + (1) * 4), v45);
                        w_u32((((uint32)(v40)) + (2) * 4), v46);
                        xport_draft_host_sub_80067808_p1(v98, 0x800A5460);
                        finished = 1;
                        w_u8(0x800A5469, 0);
                        w_u32(0x800FF384, r_u32(0x800A54CC));
                    }
                    if (sub_800155D4(main_window, r_u32((0x800A557C + (0) * 4))))
                        menu_state = 1;
                    if (sub_800155D4(main_window, r_u32((0x800A55D0 + (0) * 4))))
                    {
                        v39 = 23;
                        v47 = 0x800A5460;
                        v48 = 0x800A0518;
                        do
                        {
                            v49 = ((sint32)(r_u32((v48 + (1) * 4))));
                            v50 = ((sint32)(r_u32((v48 + (2) * 4))));
                            v51 = ((sint32)(r_u32((v48 + (3) * 4))));
                            w_u32(((uint32)(v47)), ((sint32)(r_u32(v48))));
                            w_u32((((uint32)(v47)) + (1) * 4), v49);
                            w_u32((((uint32)(v47)) + (2) * 4), v50);
                            w_u32((((uint32)(v47)) + (3) * 4), v51);
                            v48 += (4) * 4;
                            v47 += (16) * 1;
                        } while ((v48 != 0x800A0578));
                        v52 = ((sint32)(r_u32((v48 + (1) * 4))));
                        v53 = ((sint32)(r_u32((v48 + (2) * 4))));
                        w_u32(((uint32)(v47)), ((sint32)(r_u32(v48))));
                        w_u32((((uint32)(v47)) + (1) * 4), v52);
                        w_u32((((uint32)(v47)) + (2) * 4), v53);
                        finished = 1;
                    }
                    if (sub_800155D4(main_window, 0x800A031C))
                    {
                        result_state = 2;
                        finished = 1;
                    }
                    if (sub_800155D4(main_window, r_u32((0x800A55D4 + (0) * 4))))
                        menu_state = 13;
                    if (sub_800155D4(main_window, r_u32((0x800A55D8 + (0) * 4))))
                        menu_state = 3;
                }
                if (v102)
                    v39 = -1;
                if (!r_u32(0x800FF814))
                {
                    v128 = r_u32(0x800FF64C);
                    goto LABEL_373;
                }
                v54 = v39;
                if ((((uint32)(((uint32)(r_u32(0x800FF64C)) - (uint32)(v128)))) >= 0x709))
                {
                    result_state = 3;
                    finished = 1;
                }
                goto LABEL_374;

            case 1:
                MenuWindow::Attach(level_window).Update();
                if (!v101)
                    goto LABEL_145;
                v55 = r_u32(((uint32)(((uint32)(((uint32)(level_window) + (uint32)(((uint32)(28) * (uint32)(r_u8(((uint32)(((uint32)(level_window) + (uint32)(6)))))))))) + (uint32)(24)))));
                w_u8(0x800A5460, 0);
                if (!(r_u8(r_u32(0x800A5254))))
                    goto LABEL_143;
                v56 = 0x800A5254;
                do
                {
                    if (sub_80067724(v55, r_u32(v56)))
                    {
                        sub_80067808(r_u32(v56 + 4), 0x800A5460);
                        v39 = 23;
                        goto LABEL_144;
                    }
                    v56 += (6) * 4;
                } while (r_u8(((uint32)(r_u32(v56)))));
            LABEL_143:
                v39 = 23;

            LABEL_144:
                w_u8(0x800A5469, 0);

                finished = 1;
            LABEL_145:
                v54 = v39;

                if (v102)
                    menu_state = 0;
                goto LABEL_374;

            case 3:
                MenuWindow::Attach(options_window).Update();
                if (v101)
                {
                    if (sub_800155D4(options_window, r_u32((0x800A55DC + (0) * 4))))
                    {
                        v57 = difficulty_window;
                        w_u8((difficulty_window + (8) * 1), r_u32(0x800FF384));
                        menu_state = 6;
                        sub_80015374(v57, ((unsigned char)(r_u32(0x800FF384))));
                    }
                    if (sub_800155D4(options_window, r_u32((0x800A55E0 + (0) * 4))))
                    {
                        v39 = -1;
                        sub_80069DF0(23, 0x2000, 0);
                        menu_state = 7;
                        v119 = 0;
                        sub_80015F48(r_u32((0x800A525C + (0) * 4)));
                    }
                    if (sub_800155D4(options_window, r_u32((0x800A55E4 + (0) * 4))))
                        menu_state = 8;
                    if (sub_800155D4(options_window, r_u32((0x800A55E8 + (0) * 4))))
                        menu_state = 4;
                    if (sub_800155D4(options_window, r_u32((0x800A55EC + (0) * 4))))
                    {
                        v39 = 19;
                        menu_state = 0;
                    }
                }
                v54 = v39;
                if (v102)
                    menu_state = 0;
                goto LABEL_374;

            case 4:
                MenuWindow::Attach(extras_window).Update();
                if (v101)
                {
                    if (sub_800155D4(extras_window, r_u32((0x800A55F0 + (0) * 4))))
                    {
                        sub_80069DF0(v39, 0x2000, 0);
                        v39 = -1;
                        RunCredits();
                    }
                    if (sub_800155D4(extras_window, r_u32((0x800A55F4 + (0) * 4))))
                    {
                        sub_80069DF0(v39, 0x2000, 0);
                        v39 = -1;
                        sub_80011B04();
                        sub_8002E814(33);
                        sub_80011B34();
                        sub_8006F7D8();
                    }
                    if (sub_800155D4(extras_window, r_u32((0x800A55F8 + (0) * 4))))
                    {
                        sub_80011B04();
                        sub_8002E814(28);
                        sub_80011B34();
                        sub_8006F7D8();
                    }
                    if (sub_800155D4(extras_window, r_u32((0x800A55FC + (0) * 4))))
                    {
                        menu_state = 12;
                        sub_800153D8(r_u32(0x800FF024));
                    }
                    if (sub_800155D4(extras_window, r_u32((0x800A5600 + (0) * 4))))
                    {
                        menu_state = 5;
                        v116 = (sint16)r_u16(0x800A7304);
                        v117 = (sint16)r_u16(0x800A7306);
                    }
                    if (sub_800155D4(extras_window, r_u32((0x800A55A4 + (0) * 4))))
                    {
                        v39 = 19;
                        menu_state = 3;
                    }
                }
                goto LABEL_296;

            case 5:
                if ((r_u8(0x800EC188) && ((sint16)r_u16(0x800A7304) < 32)))
                {
                    (w_u16(0x800A7304, (r_u16(0x800A7304) + 1)), r_u16(0x800A7304));
                    (w_u16(0x800C6374, (r_u16(0x800C6374) + 1)), r_u16(0x800C6374));
                }
                if ((r_u8(0x800EC178) && ((sint16)r_u16(0x800A7304) > 0)))
                {
                    (w_u16(0x800A7304, (r_u16(0x800A7304) - 1)), r_u16(0x800A7304));
                    (w_u16(0x800C6374, (r_u16(0x800C6374) - 1)), r_u16(0x800C6374));
                }
                if ((r_u8(0x800EC198) && ((sint16)r_u16(0x800A7306) > 0)))
                {
                    (w_u16(0x800A7306, (r_u16(0x800A7306) - 1)), r_u16(0x800A7306));
                    (w_u16(0x800C6376, (r_u16(0x800C6376) - 1)), r_u16(0x800C6376));
                }
                if ((r_u8(0x800EC1A8) && ((sint16)r_u16(0x800A7306) < 32)))
                {
                    (w_u16(0x800A7306, (r_u16(0x800A7306) + 1)), r_u16(0x800A7306));
                    (w_u16(0x800C6376, (r_u16(0x800C6376) + 1)), r_u16(0x800C6376));
                }
                if (v101)
                    menu_state = 4;
                if (v102)
                {
                    v118 = 2;
                    menu_state = 4;
                }
                goto LABEL_373;

            case 6:
                MenuWindow::Attach(difficulty_window).Update();
                if (v101)
                {
                    if (sub_800155D4(difficulty_window, r_u32((0x800A5648 + (0) * 4))))
                    {
                        v39 = 4;
                        w_u32(0x800FF384, 0);
                    }
                    if (sub_800155D4(difficulty_window, r_u32((0x800A564C + (0) * 4))))
                    {
                        v39 = 6;
                        w_u32(0x800FF384, 1);
                    }
                    if (sub_800155D4(difficulty_window, r_u32((0x800A5650 + (0) * 4))))
                    {
                        v39 = 8;
                        w_u32(0x800FF384, 2);
                    }
                    if (sub_800155D4(difficulty_window, r_u32((0x800A55A4 + (0) * 4))))
                    {
                        v39 = 19;
                        menu_state = 3;
                    }
                }
                w_u8((difficulty_window + (8) * 1), r_u32(0x800FF384));
            LABEL_296:
                v54 = v39;

                if (v102)
                    menu_state = 3;
                goto LABEL_374;

            case 7:
                MenuWindow::Attach(audio_window).Update();
                v78 = sub_8001173C();
                if ((sub_800155D4(audio_window, r_u32((0x800A55A8 + (0) * 4))) && (v78 || v101)))
                {
                    sub_80069DF0(16, 0x2000, 0);
                    w_u16(0x800ECC7A, (r_u16(0x800ECC7A) + (((uint32)(((uint16)(v78))) << (uint32)(10)))));
                    if (((r_u16(0x800ECC7A) & 0x8000) != 0))
                        w_u16(0x800ECC7A, 0);
                    if ((r_u16(0x800ECC7A) >= 0x4000))
                        w_u16(0x800ECC7A, 0x3FFF);
                }
                if (sub_800155D4(audio_window, r_u32((0x800A55AC + (0) * 4))))
                {
                    w_u16(0x800ECC78, (r_u16(0x800ECC78) + (((uint32)(8) * (uint32)(v78)))));
                    if (((r_u16(0x800ECC78) & 0x8000) != 0))
                        w_u16(0x800ECC78, 0);
                    if ((r_u16(0x800ECC78) >= 128))
                        w_u16(0x800ECC78, 127);
                    ((void)(0), sub_8006A428());
                    if (v101)
                    {
                        v79 = (((uint32)(0x800A5254)) + (((uint32)(24) * (uint32)(++v119))) * 1);
                        if (!(r_u8(r_u32((((uint32)(v79)) + (2) * 4)))))
                            v119 = 0;
                        sub_80015F48(r_u32(((((uint32)(0x800A5254)) + (((uint32)(6) * (uint32)(v119))) * 4) + (2) * 4)));
                        ((void)(0), sub_8006A428());
                    }
                }
                else
                {
                    sub_8006A3F0();
                }
                if (sub_800155D4(audio_window, r_u32((0x800A55B0 + (0) * 4))))
                {
                    if (((v78 || v101) || !r_u32(0x800FF020)))
                    {
                        v80 = sub_80066570(2);
                        sub_8002FD2C(0, v80);
                        w_u16(0x800ECC7C, (r_u16(0x800ECC7C) + (((uint32)(16) * (uint32)(v78)))));
                        if (((r_u16(0x800ECC7C) & 0x8000) != 0))
                            w_u16(0x800ECC7C, 0);
                        if ((r_u16(0x800ECC7C) >= 256))
                            w_u16(0x800ECC7C, 255);
                        w_u32(0x800FF020, 1);
                    }
                }
                else
                {
                    w_u32(0x800FF020, 0);
                }
                if (((!sub_800155D4(audio_window, r_u32((0x800A55A4 + (0) * 4))) || !v101) && !v102))
                    goto LABEL_329;
                menu_state = 3;
                sub_80069A94();
                w_u32(0x800FF020, 0);
                v54 = 19;
                goto LABEL_374;

            case 8:
                if (((r_u32(0x800EC264) == 65) || (PadGetState(0) != 2)))
                    sub_8001551C(controller_window, r_u32((0x800A5604 + (0) * 4)));
                else
                    sub_800154E0(controller_window, r_u32(0x800A5604));
                if ((PadGetState(0) == 6))
                    sub_800154E0(controller_window, r_u32(0x800A5608));
                else
                    sub_8001551C(controller_window, r_u32((0x800A5608 + (0) * 4)));
                MenuWindow::Attach(controller_window).Update();
                v58 = 0;
                if (v101)
                {
                    if ((((sub_800155D4(controller_window, r_u32((0x800A5654 + (0) * 4))) || sub_800155D4(controller_window, r_u32((0x800A5658 + (0) * 4)))) || sub_800155D4(controller_window, r_u32((0x800A565C + (0) * 4)))) || sub_800155D4(controller_window, r_u32((0x800A5730 + (0) * 4)))))
                    {
                        v58 = 1;
                    }
                    if (v58)
                        v39 = -1;
                }
                v59 = (r_u8(0x800EC148) != 0);
                if (r_u8(0x800EC168))
                    v59 |= 2;
                if (r_u8(0x800EC138))
                    v59 |= 4;
                v60 = ((uint32)(v59) - (uint32)(1));
                if (r_u8(0x800EC158))
                {
                    v59 |= 8;
                    v60 = ((uint32)(v59) - (uint32)(1));
                }
                if ((((v60 >= 2) && (v59 != 4)) && (v59 != 8)))
                    goto LABEL_253;
                if ((r_u32(0x800EC264) == 65))
                {
                    if (sub_800155D4(controller_window, r_u32((0x800A5654 + (0) * 4))))
                        sub_80011860(v59, 0x800EC47A, 0x800EC474, 0x800EC476, ((sint32)(0x800EC478)));
                    if (sub_800155D4(controller_window, r_u32((0x800A5658 + (0) * 4))))
                        sub_80011860(v59, 0x800EC474, 0x800EC47A, 0x800EC476, ((sint32)(0x800EC478)));
                    if (sub_800155D4(controller_window, r_u32((0x800A565C + (0) * 4))))
                        sub_80011860(v59, 0x800EC476, 0x800EC47A, 0x800EC474, ((sint32)(0x800EC478)));
                    if (sub_800155D4(controller_window, r_u32((0x800A5730 + (0) * 4))))
                    {
                        v61 = 0x800EC478;
                    LABEL_252:
                        sub_80011860(v59, v61, (v61 + (1) * 2), (v61 - (2) * 2), ((sint32)((v61 - (1) * 2))));
                    }
                }
                else
                {
                    if (sub_800155D4(controller_window, r_u32((0x800A5654 + (0) * 4))))
                        sub_80011860(v59, 0x800EC49A, 0x800EC494, 0x800EC496, ((sint32)(0x800EC498)));
                    if (sub_800155D4(controller_window, r_u32((0x800A5658 + (0) * 4))))
                        sub_80011860(v59, 0x800EC494, 0x800EC49A, 0x800EC496, ((sint32)(0x800EC498)));
                    if (sub_800155D4(controller_window, r_u32((0x800A565C + (0) * 4))))
                        sub_80011860(v59, 0x800EC496, 0x800EC49A, 0x800EC494, ((sint32)(0x800EC498)));
                    if (sub_800155D4(controller_window, r_u32((0x800A5730 + (0) * 4))))
                    {
                        v61 = 0x800EC498;
                        goto LABEL_252;
                    }
                }
            LABEL_253:
                if (v101)
                {
                    if (sub_800155D4(controller_window, r_u32((0x800A55A4 + (0) * 4))))
                    {
                        v39 = 19;
                        menu_state = 3;
                    }
                    if (sub_800155D4(controller_window, r_u32((0x800A55FC + (0) * 4))))
                    {
                        w_u32(0x800FF2FC, 2);
                        w_u16(0x800EC47A, ((unsigned char)(r_u8(0x800A0594))));
                        w_u16(0x800EC474, ((unsigned char)(r_u8(0x800A0595))));
                        w_u16(0x800EC476, ((unsigned char)(r_u8(0x800A0596))));
                        w_u16(0x800EC478, ((unsigned char)(r_u8(0x800A0597))));
                        w_u16(0x800EC49A, ((unsigned char)(r_u8(0x800A0598))));
                        w_u16(0x800EC494, ((unsigned char)(r_u8(0x800A0599))));
                        w_u16(0x800EC496, ((unsigned char)(r_u8(0x800A059A))));
                        w_u16(0x800EC498, ((unsigned char)(r_u8(0x800A059B))));
                    }
                    if (sub_800155D4(controller_window, r_u32((0x800A5604 + (0) * 4))))
                    {
                        v120 = r_u32(0x800FF7E0);
                        v62 = r_u32(0x800FF7F0);
                        v121 = r_u32(0x800FF7E4);
                        v63 = r_u32(0x800FF7F4);
                        v122 = r_u32(0x800FF7E8);
                        v64 = r_u32(0x800FF7F8);
                        menu_state = 9;
                        v123 = r_u32(0x800FF7EC);
                        v65 = r_u32(0x800FF7FC);
                        w_u32(0x800FF7E0, 255);
                        w_u32(0x800FF7E4, 0);
                        w_u32(0x800FF7E8, 255);
                        w_u32(0x800FF7EC, 0);
                        w_u32(0x800FF7F0, 255);
                        w_u32(0x800FF7F4, 0);
                        w_u32(0x800FF7F8, 255);
                        w_u32(0x800FF7FC, 0);
                        v124 = v62;
                        v125 = v63;
                        v126 = v64;
                        v127 = v65;
                    }
                    if (sub_800155D4(controller_window, r_u32((0x800A5608 + (0) * 4))))
                    {
                        w_u8(((uint32)(((uint32)(preset_window) + (uint32)(8)))), r_u32(0x800FF2FC));
                        menu_state = 11;
                        sub_80015374(preset_window, ((unsigned char)(r_u32(0x800FF2FC))));
                    }
                }

                if (v102)
                    menu_state = 3;
                /* MIPS 80013CD4..80013CF0 passes four signed halfwords */
                sub_8006FDFC(0x800EC0F8, (sint16)r_u16(0x800EC474), (sint16)r_u16(0x800EC476), (sint16)r_u16(0x800EC478), (sint16)r_u16(0x800EC47A));
                v94 = 0x800EC494;
                v95 = 0x800EC496;
                v96 = 0x800EC498;
                v97 = ((uint32)(0x800EC49A));
                /* MIPS 80013D18..80013D2C passes zero then four signed halfwords */
                sub_8006FE14(0x800EC0F8, 3, 2, 1, 0, (sint16)r_u16(0x800EC494), (sint16)r_u16(0x800EC496), (sint16)r_u16(0x800EC498), (sint16)r_u16(0x800EC49A));
                v54 = v39;
                goto LABEL_374;

            case 9:
                if ((((unsigned char)(r_u8(0x800EC25D))) < (sint32)r_u32(0x800FF7E0)))
                    w_u32(0x800FF7E0, ((unsigned char)(r_u8(0x800EC25D))));
                if (((sint32)r_u32(0x800FF7E4) < ((unsigned char)(r_u8(0x800EC25D)))))
                    w_u32(0x800FF7E4, ((unsigned char)(r_u8(0x800EC25D))));
                if ((((unsigned char)(r_u8(0x800EC25C))) < (sint32)r_u32(0x800FF7E8)))
                    w_u32(0x800FF7E8, ((unsigned char)(r_u8(0x800EC25C))));
                if (((sint32)r_u32(0x800FF7EC) < ((unsigned char)(r_u8(0x800EC25C)))))
                    w_u32(0x800FF7EC, ((unsigned char)(r_u8(0x800EC25C))));
                if ((((unsigned char)(r_u8(0x800EC25F))) < (sint32)r_u32(0x800FF7F0)))
                    w_u32(0x800FF7F0, ((unsigned char)(r_u8(0x800EC25F))));
                if (((sint32)r_u32(0x800FF7F4) < ((unsigned char)(r_u8(0x800EC25F)))))
                    w_u32(0x800FF7F4, ((unsigned char)(r_u8(0x800EC25F))));
                if ((((unsigned char)(r_u8(0x800EC25E))) < (sint32)r_u32(0x800FF7F8)))
                    w_u32(0x800FF7F8, ((unsigned char)(r_u8(0x800EC25E))));
                if (((sint32)r_u32(0x800FF7FC) < ((unsigned char)(r_u8(0x800EC25E)))))
                    w_u32(0x800FF7FC, ((unsigned char)(r_u8(0x800EC25E))));
                if (v101)
                    menu_state = 10;
                v54 = v39;
                if (v102)
                {
                    w_u32(0x800FF7E0, v120);
                    w_u32(0x800FF7E4, v121);
                    w_u32(0x800FF7E8, v122);
                    w_u32(0x800FF7EC, v123);
                    w_u32(0x800FF7F0, v124);
                    w_u32(0x800FF7F4, v125);
                    w_u32(0x800FF7F8, v126);
                    w_u32(0x800FF7FC, v127);
                    menu_state = 8;
                }
                goto LABEL_374;

            case 10:
                w_u32(0x800FF800, ((unsigned char)(r_u8(0x800EC25D))));
                w_u32(0x800FF804, ((unsigned char)(r_u8(0x800EC25C))));
                w_u32(0x800FF808, ((unsigned char)(r_u8(0x800EC25F))));
                w_u32(0x800FF80C, ((unsigned char)(r_u8(0x800EC25E))));
                v39 = 23;
                if (v101)
                    menu_state = 8;
                else
                LABEL_329:
                    v39 = -1;

                goto LABEL_373;

            case 11:
                MenuWindow::Attach(preset_window).Update();
                if (v101)
                {
                    if (sub_800155D4(preset_window, r_u32((0x800A560C + (0) * 4))))
                        w_u32(0x800FF2FC, 0);
                    if (sub_800155D4(preset_window, r_u32((0x800A5610 + (0) * 4))))
                        w_u32(0x800FF2FC, 1);
                    if (sub_800155D4(preset_window, r_u32((0x800A5614 + (0) * 4))))
                        w_u32(0x800FF2FC, 2);
                    if (sub_800155D4(preset_window, r_u32((0x800A5618 + (0) * 4))))
                    {
                        menu_state = 8;
                        v39 = 19;
                    }
                }
                w_u8(((uint32)(((uint32)(preset_window) + (uint32)(8)))), r_u32(0x800FF2FC));
                v54 = v39;
                if (v102)
                    menu_state = 8;
                goto LABEL_374;

            case 12:
                sub_80015BA4(r_u32(0x800FF024));
                if (v101)
                {
                    if (sub_800155D4(r_u32(0x800FF024), r_u32((0x800A5594 + (0) * 4))))
                    {
                        v66 = 0x800A545C;
                        v67 = 0x800A0514;
                        do
                        {
                            v68 = ((sint32)(r_u32((v67 + (1) * 4))));
                            v69 = ((sint32)(r_u32((v67 + (2) * 4))));
                            v70 = ((sint32)(r_u32((v67 + (3) * 4))));
                            w_u32(v66, ((sint32)(r_u32(v67))));
                            w_u32((v66 + (1) * 4), v68);
                            w_u32((v66 + (2) * 4), v69);
                            w_u32((v66 + (3) * 4), v70);
                            v67 += (4) * 4;
                            v66 += (4) * 4;
                        } while ((v67 != ((uint32)(0x800A0594))));
                        v71 = ((sint32)(r_u32((v67 + (1) * 4))));
                        w_u32(v66, ((sint32)(r_u32(v67))));
                        w_u32((v66 + (1) * 4), v71);
                        v72 = 0x800A53D4;
                        v73 = 0x800A0514;
                        do
                        {
                            v74 = ((sint32)(r_u32((v73 + (1) * 4))));
                            v75 = ((sint32)(r_u32((v73 + (2) * 4))));
                            v76 = ((sint32)(r_u32((v73 + (3) * 4))));
                            w_u32(v72, ((sint32)(r_u32(v73))));
                            w_u32((v72 + (1) * 4), v74);
                            w_u32((v72 + (2) * 4), v75);
                            w_u32((v72 + (3) * 4), v76);
                            v73 += (4) * 4;
                            v72 += (4) * 4;
                        } while ((v73 != ((uint32)(0x800A0594))));
                        v77 = ((sint32)(r_u32((v73 + (1) * 4))));
                        w_u32(v72, ((sint32)(r_u32(v73))));
                        w_u32((v72 + (1) * 4), v77);
                        sub_80016014(0x800A0584);
                        menu_state = 0;
                        ResetSceneDisplayOffsets();
                        w_u32(0x800FF7E4, 255);
                        w_u32(0x800FF7EC, 255);
                        w_u32(0x800FF7F4, 255);
                        w_u32(0x800FF7FC, 255);
                        w_u32(0x800FF7E0, 0);
                        w_u32(0x800FF7E8, 0);
                        w_u32(0x800FF7F0, 0);
                        w_u32(0x800FF7F8, 0);
                        w_u32(0x800FF800, 128);
                        w_u32(0x800FF804, 128);
                        w_u32(0x800FF808, 128);
                        w_u32(0x800FF80C, 128);
                        sub_8001551C(main_window, r_u32((0x800A554C + (0) * 4)));
                    }
                    else
                    {
                        v39 = 19;
                        menu_state = 4;
                    }
                }
                v54 = v39;
                if (v102)
                    menu_state = 4;
                goto LABEL_374;

            case 13:
                v81 = ((uint32)(v114) - (uint32)(1));
                if (v114)
                {
                    --v114;
                    if (!v81)
                    {
                        v115 = 150;
                        menu_state = 14;
                    }
                }
                else
                {
                    v114 = 10;
                }
                goto LABEL_373;

            case 14:
                if (((sint32)r_u32(0x800FEEC4) == -1))
                {
                    menu_state = 18;
                    goto LABEL_373;
                }
                if (((sint32)r_u32(0x800FEEC4) < 0))
                {
                    if (((sint32)r_u32(0x800FEEC4) == -2))
                    {
                        menu_state = 16;
                    }
                    else
                    {
                    LABEL_346:
                        if (v115)
                            --v115;
                        else
                        LABEL_340:
                            menu_state = 15;
                    }
                LABEL_373:
                    v54 = v39;

                    goto LABEL_374;
                }
                if (((sint32)r_u32(0x800FEEC4) != 1))
                    goto LABEL_346;
                if (QueryCardSaveFile())
                    goto LABEL_340;
                v82 = ((uint32)(sub_8002FED8(444)));
                save_window = v82;
                if (v82)
                {
                    v94 = 192;
                    v95 = 18;
                    save_window = ((uint32)(sub_80015228(v82, 256, 55, 0, 0x100, 0xC0, 0x12)));
                }
                w_u8((save_window + (9) * 1), 1);
                menu_state = 17;
                PopulateSaveSelectionMenu(save_window);
                v54 = v39;
            LABEL_374:
                sub_80069DF0(v54, 0x2000, 0);

                sub_800119C4(((sint32)(background_menu)));
                if ((r_u32(0x800FF64C) == v129))
                    sub_800664E4(1);
                w_u32(0x800FF65C, 0);
                while (1)
                {
                    sub_800664E4(1);
                    if (!DrawSync(1))
                        break;
                    sub_800662A0();
                }

                SubmitSceneFrame();
                if (!r_u32(0x800FF65C))
                {
                    sub_800662A0();
                    w_u32(0x800FF65C, 1);
                }
                break;

            case 15:

            case 16:
                if (((sint32)r_u32(0x800FEEC4) == 2))
                    menu_state = 13;
                goto LABEL_371;

            case 17:
                v83 = 0;
                if (((sint32)r_u32(0x800FEEC4) == -1))
                {
                    menu_state = 18;
                    v83 = 1;
                }
                MenuWindow::Attach(save_window).Update();
                if (v101)
                {
                    v84 = 0;
                    if ((sub_800155D4(save_window, r_u32((0x800A55CC + (0) * 4))) || sub_800155D4(save_window, r_u32((0x800A5660 + (0) * 4)))))
                        v84 = 1;
                    v39 = 28;
                    if (!v84)
                    {
                        v39 = 23;
                        v85 = 0x800A545C;
                        v86 = (0x800A493C + (((uint32)(136) * (uint32)(r_u8((save_window + (6) * 1))))) * 1);
                        v87 = ((uint32)((v86 + (512) * 1)));
                        v88 = (v86 + (640) * 1);
                        do
                        {
                            v89 = ((sint32)(r_u32((v87 + (1) * 4))));
                            v90 = ((sint32)(r_u32((v87 + (2) * 4))));
                            v91 = ((sint32)(r_u32((v87 + (3) * 4))));
                            w_u32(v85, ((sint32)(r_u32(v87))));
                            w_u32((v85 + (1) * 4), v89);
                            w_u32((v85 + (2) * 4), v90);
                            w_u32((v85 + (3) * 4), v91);
                            v87 += (4) * 4;
                            v85 += (4) * 4;
                        } while ((v87 != ((uint32)(v88))));
                        v92 = ((sint32)(r_u32((v87 + (1) * 4))));
                        w_u32(v85, ((sint32)(r_u32(v87))));
                        w_u32((v85 + (1) * 4), v92);
                        sub_80016014(0x800A54CC);
                        v83 = 1;
                        finished = 1;
                    }
                }
                if (v102)
                {
                    v83 = 1;
                    menu_state = 0;
                }
                if (v83)
                {
                    if (save_window)
                        DestroyMenuObject(save_window);
                    save_window = 0;
                }
                goto LABEL_373;

            case 18:
                if ((((sint32)r_u32(0x800FEEC4) == -2) || ((((sint32)(sint32)r_u32(0x800FEEC4) >= -2) && ((sint32)(sint32)r_u32(0x800FEEC4) < 3)) && ((sint32)(sint32)r_u32(0x800FEEC4) > 0))))
                    menu_state = 13;
            LABEL_371:
                v54 = v39;

                if (!v103)
                    goto LABEL_374;
                v39 = 19;
                menu_state = 0;
                goto LABEL_373;

            default:
                goto LABEL_373;
        }
    }

    sub_8002E2B8();
    sub_800664E4(1);
    DrawSync(0);
    SubmitSceneFrame();
    sub_80015EC8();
    if (main_window)
        DestroyMenuObject(main_window);
    if (level_window)
        DestroyMenuObject(level_window);
    if (options_window)
        DestroyMenuObject(options_window);
    if (extras_window)
        DestroyMenuObject(extras_window);
    if (difficulty_window)
        DestroyMenuObject(difficulty_window);
    if (audio_window)
        DestroyMenuObject(audio_window);
    if (controller_window)
        DestroyMenuObject(controller_window);
    if (preset_window)
        DestroyMenuObject(preset_window);
    if (background_menu)
        DestroyMenuObject(background_menu);
    sub_80011B04();
    sub_8007001C();
    sub_80015F7C(0x800A54CC);
    return result_state;
}

extern "C" uint32 sub_80011BA0(void)
{
    FUNCTION_MARKER(0x80011BA0, "SLUS_003.73");
    return RunMainMenu();
}

#include "psx.h"
#include "xport.h"
#include "psx_gpu.h"
#include "psx_spu.h"
#include "game_bios_bootstrap.h"
#include "game_pad_startup.h"
#include "game_gpu_present.h"
#include <string.h>

// XPORT REVISION: 2026-09-29T20:00:00Z
extern void sub_80030D74(void);
extern void apocalypse_bind_callbacks(void);
extern uint32 apocalypse_reset_callbacks(void);

void xport_main(void)
{
    static uint8 image[PSX_DRAM_SIZE];
    size_t size;
    uint32 payload_size;
    const GpuPsyqStateBinding gpu_binding = {0x800FCF18u, 0x800FCEBCu, 0x800FCEACu, 0x800FCEAFu, 0x800FCE4Cu, 0u};
    if (!xport_file_read("../orig/SLUS_003.73", image, sizeof(image), &size) || size < 2048u || memcmp(image, "PS-X EXE", 8u) != 0)
    {
        xport_message_error("Apocalypse", "Cannot load ../orig/SLUS_003.73");
        xport_set_exit_code(1);
        return;
    }
    if (!cd_mount_cue("../iso/Apocalypse (USA).cue"))
    {
        xport_message_error("Apocalypse", "Cannot mount ../iso/Apocalypse (USA).cue");
        xport_set_exit_code(1);
        return;
    }
    payload_size = (uint32)image[28] | ((uint32)image[29] << 8) | ((uint32)image[30] << 16) | ((uint32)image[31] << 24);
    if (payload_size > size - 2048u || payload_size > PSX_DRAM_SIZE - 0x10000u)
    {
        xport_message_error("Apocalypse", "Invalid original executable payload");
        xport_set_exit_code(1);
        return;
    }
    xport_guest_copy(xport_guest_ref(0x80010000u), xport_host_ref(image + 2048u), payload_size);
    xport_guest_fill(0x800FFB50u, 0u, 0x80108740u - 0x800FFB50u);
    w_u32(0x800F7134u, 0x80108740u);
    w_u32(0x800F7138u, r_u32(0x800F7158u) - 8u - r_u32(0x800F7154u) - 0x108740u);
    if (!gpu_bind_psyq_state(&gpu_binding))
    {
        xport_set_exit_code(1);
        return;
    }
    apocalypse_bind_card_bootstrap();
    psx_bios_init_user_heap(0x80108744u, r_u32(0x800F7138u));
    apocalypse_bind_callbacks();
    sub_80030D74();
}

extern uint32 apocalypse_crt_initialize(void);
extern uint32 sub_800713E8(void);
extern uint32 sub_8002E814(uint32);
extern uint32 sub_80071288(uint32);
extern uint32 sub_80014BFC(void);
extern uint32 sub_80011BA0(void);
extern uint32 sub_80018C30(uint32);
extern uint32 sub_8006B04C(uint32);
extern uint32 sub_8006B864(uint32, uint32, uint32);
extern uint32 sub_8006B234(uint32);
extern uint32 sub_8006B44C(void);
extern uint32 sub_80067724(uint32, uint32);
extern uint32 sub_8008BF9C(uint32, uint32);
extern uint32 sub_80067808(uint32, uint32);
extern uint32 xport_draft_host_sub_8006613C_p1(void *position, uint32 index);
extern uint32 sub_80065DE8(void);
extern uint32 sub_800625AC(uint32);
extern uint32 sub_8005C5C0(uint32);
extern uint32 sub_800772B8(uint32, uint32);
extern uint32 sub_80063DD4(uint32);
extern uint32 sub_80063E7C(void);
extern uint32 sub_80030AD4(void);
extern uint32 sub_8006BC20(uint32);
extern uint32 sub_80018B70(uint32);
extern uint32 sub_8006CDC4(void);
extern uint32 sub_80018948(uint32);
extern uint32 sub_8006B5FC(void);
extern uint32 ResetCallbackPSX(void);
extern uint32 sub_80010610(uint32);
extern uint32 sub_8006FE4C(void);

extern void apocalypse_gs_init_graph(uint32, uint32, uint32, uint32, uint32);
extern void apocalypse_gs_def_disp_buff(uint32, uint32, uint32, uint32);
extern uint32 sub_80068084(void);
extern uint32 VSyncCallbackPSX(uint32);
extern sint32 apocalypse_clear_image2(PSX_RECT *, uint8, uint8, uint8);
extern uint32 sub_800771F4(uint32);
extern uint32 sub_8006AEE4(void);
extern uint32 sub_8006DFA0(void);
extern uint32 sub_8001610C(void);
extern uint32 sub_8002E738(void);
extern uint32 sub_8002FBB4(void);
extern uint32 sub_80068404(void);
extern uint32 sub_800698FC(void);
extern uint32 sub_8006A868(void);
extern uint32 sub_80069320(void);
extern void sub_8007D8C0(uint32, uint32, uint32);
extern uint32 sub_8007D76C(uint32, uint32, uint32);
extern void sub_8001024C(uint32, uint32, uint32);
extern void sub_8006AED8(uint32);
extern uint32 sub_8007113C(void);
extern uint32 sub_8006994C(uint32);
extern uint32 sub_8006F29C(uint32, uint32);
extern uint32 sub_8006F7D8(void);
extern uint32 sub_80070748(void);
extern uint32 sub_8006CDA0(void);
extern uint32 sub_8006A834(void);
extern uint32 sub_80032A0C(void);
extern uint32 sub_8001A760(void);
extern uint32 sub_8001A858(void);
extern uint32 sub_8006B4B4(uint32 block, uint32 heap);
extern void xport_bios_exit_critical(void);
extern uint32 sub_8009FA6C(uint32);
extern uint32 sub_8009FAD8(void);
extern uint32 _bu_init(void);
extern uint32 sub_80085E6C(void);
extern sint32 OpenEventGuest(uint32, uint32, uint32, uint32);
extern uint32 sub_80085E7C(uint32);
extern sint32 EnableEvent(uint32);
extern uint32 ChangeClearPAD(uint32);
extern uint32 ReadInitPadFlag(void);
extern uint32 sub_8009FF8C(uint32);
extern uint32 _copy_memcard_patch(void);
extern uint32 _patch_card(void);
extern uint32 _patch_card2(void);
extern sint32 xport_bios_enter_critical(void);
extern uint32 xport_bios_init_card(uint32);
extern uint32 sub_8009FF9C(void);
extern uint32 xport_bios_start_card(void);
extern uint32 sub_8006FDFC(uint32, uint32, uint32, uint32, uint32);
extern uint32 sub_8006FE14(uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32, uint32);
extern uint32 sub_8007001C(void);
extern uint32 PadInitMtap(uint32, uint32);
extern uint32 sub_8009C7CC(void);
extern uint32 sub_800700B0(uint32 controller);

void sub_80030D74(void)
{
    uint32 refresh = 0u;
    uint32 state;
    uint32 source;
    uint32 destination;
    uint32 pointer;
    uint32 word0, word1, word2, word3;
    uint32 index;
    uint32 object;
    uint32 resource;
    uint32 filename;
    uint32 position[3];
    uint32 count;
    uint32 toggle;
    FUNCTION_MARKER(0x80030D74u, "SLUS_003.73");
    apocalypse_crt_initialize();
    sub_800713E8();
    if (r_u8(0x800EC1D8u) == 0u)
        sub_8002E814(0x1Cu);
restart:
    sub_80071288(0u);
    if (refresh != 0u)
    {
        refresh = 0u;
        sub_80014BFC();
    }
    state = sub_80011BA0();
    if (state == 1u)
        goto restart;
    if ((sint32)state < 2)
    {
        if (state == 0u)
        {
            if (sub_80067724(0x800A5460u, r_u32(0x800A5258u)) != 0u)
                sub_8002E814(2u);
            goto load_default;
        }
        goto frame;
    }
    if (state == 2u)
    {
        destination = 0x800A5460u;
        source = 0x800A0518u;
        do
        {
            word0 = r_u32(source);
            word1 = r_u32(source + 4u);
            word2 = r_u32(source + 8u);
            word3 = r_u32(source + 12u);
            w_u32(destination, word0);
            w_u32(destination + 4u, word1);
            w_u32(destination + 8u, word2);
            w_u32(destination + 12u, word3);
            source += 16u;
            destination += 16u;
        } while (source != 0x800A0578u);
        word0 = r_u32(source);
        word1 = r_u32(source + 4u);
        word2 = r_u32(source + 8u);
        w_u32(destination, word0);
        w_u32(destination + 4u, word1);
        w_u32(destination + 8u, word2);
        pointer = 0x800A5254u;
        for (;;)
        {
            if (r_u8(r_u32(pointer + 4u)) == 0u)
                pointer = 0x800A5254u;
            sub_80071288(0u);
            if (pointer == 0x800A5254u)
            {
                sub_8008BF9C(0x800A19D4u, r_u32(0x800FF748u));
                sub_8008BF9C(0x800A19F0u, r_u32(0x800FF74Cu));
            }
            index = 0u;
            sub_80067808(r_u32(pointer + 4u), 0x800A5460u);
            w_u8(0x800A5469u, 0u);
            sub_80018C30(0x800A545Cu);
            while ((sint32)index < (sint32)r_u32(0x800FF628u))
            {
                if ((sint16)r_u16(r_u32(r_u32(0x800FF624u) + index * 4u)) == 8)
                {
                    filename = xport_draft_host_sub_8006613C_p1(position, index) + 6u;
                    sub_80071288(1u);
                    sub_80065DE8();
                    object = sub_800625AC(0x244u);
                    if (object != 0u)
                    {
                        resource = sub_800625AC(0x290u);
                        if (resource != 0u)
                            resource = sub_8005C5C0(resource);
                        object = sub_800772B8(object, resource);
                    }
                    w_u32(object + 0x234u, 3u);
                    sub_80063DD4(filename);
                    w_u32(0x800FF2E0u, filename);
                    sub_80063E7C();
                    w_u32(0x800FF81Cu, 0x800A6558u);
                    w_u32(0x800FF818u, 2u);
                    sub_80030AD4();
                    state = r_u32(0x800FF2ECu);
                    w_u32(0x800FF81Cu, 0u);
                    if (state == 5u)
                    {
                        w_u32(0x800FF818u, 0u);
                        w_u32(0x800FF2E0u, 0u);
                        goto restart;
                    }
                }
                ++index;
            }
            pointer += 0x18u;
        }
    }
    if (state != 3u)
        goto frame;
    if (r_u8(0x800FF314u) == 0u)
    {
        w_u32(0x800FF310u, 1u);
        sub_8002E814(0x1Cu);
        toggle = r_u8(0x800FF314u);
        w_u8(0x800FF314u, toggle ^ 1u);
        goto restart;
    }
    destination = 0x800FFD68u;
    source = 0x800A53D4u;
    state = r_u32(0x800FF2FCu);
    w_u32(0x800FF2FCu, 0u);
    w_u32(0x800FF2E4u, state);
    do
    {
        word0 = r_u32(source);
        word1 = r_u32(source + 4u);
        word2 = r_u32(source + 8u);
        word3 = r_u32(source + 12u);
        w_u32(destination, word0);
        w_u32(destination + 4u, word1);
        w_u32(destination + 8u, word2);
        w_u32(destination + 12u, word3);
        source += 16u;
        destination += 16u;
    } while (source != 0x800A5454u);
    index = r_u8(0x800FF2E9u);
    word0 = r_u32(source);
    word1 = r_u32(source + 4u);
    w_u32(destination, word0);
    w_u32(destination + 4u, word1);
    sub_80018C30(0x800A6578u + index * 0x8Cu);
    index = r_u8(0x800FF2E9u);
    resource = sub_8006B04C(r_u32(0x800A6600u + index * 0x8Cu));
    w_u32(0x800FF394u, resource);
    resource = sub_8006B864(resource, 1u, 1u);
    w_u32(0x800FF81Cu, resource);
    sub_8006B234(resource);
    sub_8006B44C();
    resource = r_u32(0x800FF81Cu);
    toggle = r_u8(0x800FF314u);
    w_u32(0x800FF818u, 2u);
    w_u32(0x800FF820u, resource);
    w_u8(0x800FF314u, toggle ^ 1u);
frame:
    sub_80030AD4();
    resource = r_u32(0x800FF81Cu);
    if (resource != 0u)
    {
        sub_8006BC20(resource);
        w_u32(0x800FF81Cu, 0u);
        w_u32(0x800FF2ECu, 4u);
    }
    if (r_u32(0x800FF818u) == 2u)
    {
        destination = 0x800A53D4u;
        source = 0x800FFD68u;
        state = r_u32(0x800FF2E4u);
        w_u32(0x800FF2FCu, state);
        do
        {
            word0 = r_u32(source);
            word1 = r_u32(source + 4u);
            word2 = r_u32(source + 8u);
            word3 = r_u32(source + 12u);
            w_u32(destination, word0);
            w_u32(destination + 4u, word1);
            w_u32(destination + 8u, word2);
            w_u32(destination + 12u, word3);
            source += 16u;
            destination += 16u;
        } while (source != 0x800FFDE8u);
        word0 = r_u32(source);
        word1 = r_u32(source + 4u);
        w_u32(destination, word0);
        w_u32(destination + 4u, word1);
    }
    state = r_u32(0x800FF2ECu);
    w_u32(0x800FF818u, 0u);
    switch (state)
    {
        case 1u:
            w_u32(0x800FEF00u, 5u);
            goto restart;
        case 2u:
            count = r_u32(0x800FF874u);
            if (count != 0u)
            {
                if (r_u32(0x800FF320u) == 0u)
                {
                    w_u32(0x800FF874u, count - 1u);
                    count = r_u32(0x800FF874u);
                }
                w_u32(0x800A5440u, count);
                if (count != 0u)
                {
                    sub_80071288(1u);
                    sub_80065DE8();
                    object = sub_800625AC(0x244u);
                    if (object != 0u)
                    {
                        resource = sub_800625AC(0x290u);
                        if (resource != 0u)
                            resource = sub_8005C5C0(resource);
                        object = sub_800772B8(object, resource);
                    }
                    w_u32(object + 0x234u, 3u);
                    sub_80018B70(0x800A53D4u);
                    goto restore;
                }
            }
            count = (uint32)(sint32)(sint16)r_u16(0x800EC522u);
            w_u32(0x800FEF00u, 20u);
            w_u32(0x800FF874u, count);
            goto restart;
        case 3u:
            sub_80071288(0u);
            resource = sub_80018948(r_u32(0x800FF83Cu));
            sub_8002E814(r_u8(resource + 0x14u));
            if (r_u32(0x800A53D4u) == 0u)
            {
                refresh = 1u;
                goto restart;
            }
            source = 0x800A53D4u;
            destination = 0x800A545Cu;
            do
            {
                word0 = r_u32(source);
                word1 = r_u32(source + 4u);
                word2 = r_u32(source + 8u);
                word3 = r_u32(source + 12u);
                w_u32(destination, word0);
                w_u32(destination + 4u, word1);
                w_u32(destination + 8u, word2);
                w_u32(destination + 12u, word3);
                source += 16u;
                destination += 16u;
            } while (source != 0x800A5454u);
            word0 = r_u32(source);
            word1 = r_u32(source + 4u);
            w_u32(destination, word0);
            w_u32(destination + 4u, word1);
            goto load_default;
        case 4u:
        case 5u:
            sub_80071288(0u);
            index = r_u8(0x800FF2E9u) + 1u;
            w_u8(0x800FF2E9u, index);
            index &= 0xFFu;
            if (r_u8(r_u32(0x800A6600u + index * 0x8Cu)) == 0u)
                w_u8(0x800FF2E9u, 0u);
            goto restart;
        case 6u:
            sub_80071288(1u);
            sub_80065DE8();
            object = sub_800625AC(0x244u);
            if (object != 0u)
            {
                resource = sub_800625AC(0x290u);
                if (resource != 0u)
                    resource = sub_8005C5C0(resource);
                object = sub_800772B8(object, resource);
            }
            w_u32(object + 0x234u, 3u);
            goto restore;
        case 7u:
            w_u32(0x800FEF00u, 20u);
            goto restart;
        default:
            goto restart;
    }
restore:
    sub_80063E7C();
    sub_8006CDC4();
    goto frame;
load_default:
    sub_80018C30(0x800A545Cu);
    goto frame;
}

uint32 sub_8007A5FC(void)
{
    FUNCTION_MARKER(0x8007A5FCu, "SLUS_003.73");
    w_u32(0x801029B8u, 0x00000000u);
    w_u32(0x801029BCu, 0xFFE70000u);
    w_u32(0x801029C0u, 0x00000000u);
    w_u32(0x801029A8u, 0x00000000u);
    w_u32(0x801029ACu, 0xFFF9C000u);
    w_u32(0x801029B0u, 0x00000000u);
    w_u32(0x80102998u, 0x00000000u);
    w_u32(0x8010299Cu, 0xFFF60000u);
    w_u32(0x801029A0u, 0xFFC4A000u);
    w_u32(0x80102988u, 0x00000000u);
    w_u32(0x8010298Cu, 0xFFF9C000u);
    w_u32(0x80102990u, 0x00000000u);
    w_u32(0x80102978u, 0x00000000u);
    w_u32(0x8010297Cu, 0xFFF6A000u);
    w_u32(0x80102980u, 0x0028A000u);
    w_u32(0x80102968u, 0x00000000u);
    w_u32(0x8010296Cu, 0xFFF9C000u);
    w_u32(0x80102970u, 0x00000000u);
    w_u32(0x80102958u, 0x00000000u);
    w_u32(0x8010295Cu, 0xFFBB4000u);
    w_u32(0x80102960u, 0x00514000u);
    w_u32(0x80102948u, 0x00000000u);
    w_u32(0x8010294Cu, 0xFFF9C000u);
    w_u32(0x80102950u, 0xFFCE0000u);
    w_u32(0x80102938u, 0x00000000u);
    w_u32(0x8010293Cu, 0xFF736000u);
    w_u32(0x80102940u, 0x00190000u);
    w_u32(0x80102928u, 0x00000000u);
    w_u32(0x8010292Cu, 0xFFF9C000u);
    w_u32(0x80102930u, 0xFFED4000u);
    w_u32(0x80102918u, 0x00000000u);
    w_u32(0x8010291Cu, 0xFFF1F000u);
    w_u32(0x80102920u, 0x00320000u);
    w_u32(0x80102908u, 0x00000000u);
    w_u32(0x8010290Cu, 0xFFE7A000u);
    w_u32(0x80102910u, 0x00320000u);
    w_u32(0x801028F8u, 0x00000000u);
    w_u32(0x801028FCu, 0x00064000u);
    w_u32(0x80102900u, 0xF92A0000u);
    w_u32(0x801028E8u, 0x00000000u);
    w_u32(0x801028ECu, 0xFFF9C000u);
    w_u32(0x801028F0u, 0x00000000u);
    w_u16(0x800FFCFCu, 0x0032u);
    w_u16(0x800FFCF4u, 0x001Eu);
    w_u8(0x800FFCE0u, 0x03u);
    w_u8(0x800FFCE2u, 0x03u);
    w_u16(0x800FFCD8u, 0x0258u);
    w_u16(0x800FFCDCu, 0x0258u);
    w_u32(0x801028D8u, 0x00000000u);
    w_u16(0x800FFCF8u, 0x0019u);
    w_u16(0x800FFCFAu, 0x0000u);
    w_u16(0x800FFCF0u, 0x0000u);
    w_u16(0x800FFCF2u, 0x0000u);
    w_u16(0x800FFCE8u, 0x0000u);
    w_u16(0x800FFCEAu, 0x0000u);
    w_u16(0x800FFCECu, 0x0019u);
    w_u8(0x800FFCE1u, 0x00u);
    w_u16(0x800FFCDAu, 0x0000u);
    w_u32(0x801028DCu, 0xFFF92000u);
    w_u32(0x801028E0u, 0x00000000u);
    w_u32(0x800ED510u, 0x00000000u);
    w_u32(0x800ED514u, 0xFFFA6000u);
    w_u32(0x800ED518u, 0x00000000u);
    return 0x800ED510u;
}

uint32 sub_800713E8(void)
{
    uint32 heap_begin;
    uint32 heap_split;
    uint32 first_word;
    uint32 second_word;

    union
    {
        uint32 words[2];
        PSX_RECT value;
    } rectangle;

    uint32 start_tick;
    uint32 resource;
    uint32 first_count;
    uint32 second_count;
    uint32 result;
    FUNCTION_MARKER(0x800713E8u, "SLUS_003.73");
    w_u32(0x800FFAC0u, 0x800EE750u);
    w_u32(0x800FFAC4u, 0x800F0690u);
    heap_begin = r_u32(0x80108740u);
    w_u32(0x800E6C14u, 0x801FE000u);
    w_u32(0x800E6C08u, heap_begin);
    heap_split = heap_begin + 0x43800u;
    w_u32(0x800E6C0Cu, heap_split);
    w_u32(0x800E6C10u, heap_split);
    sub_8006B5FC();
    apocalypse_reset_callbacks();
    sub_80010610(1u);
    apocalypse_set_disp_mask(0u);
    sub_8006FE4C();
    InitGeom();
    apocalypse_reset_graph(0u);
    SetGraphDebug(0u);
    apocalypse_gs_init_graph(0x200u, 0xF0u, 0u, 0u, 0u);
    apocalypse_gs_def_disp_buff(0u, 0u, 0u, 0xF0u);
    sub_80068084();
    VSyncCallbackPSX(0x80066458u);
    first_word = r_u32(0x800FF84Cu);
    second_word = r_u32(0x800FF850u);
    rectangle.words[0] = first_word;
    rectangle.words[1] = second_word;
    apocalypse_clear_image2(&rectangle.value, 0u, 0u, 0u);
    DrawSync(0u);
    apocalypse_set_disp_mask(1u);
    sub_800771F4(0x9F6ADu);
    sub_8006AEE4();
    sub_8006DFA0();
    w_u32(0x800FEF00u, 20u);
    sub_8001610C();
    sub_8002E738();
    sub_8002FBB4();
    sub_80068404();
    sub_800698FC();
    sub_8006A868();
    sub_80069320();
    sub_8007D8C0(0x100u, 0x78u, 0x100u);
    sub_8007D76C(10u, 0x1770u, 0x800u);
    PutDispEnv((DISPENV *)psx_addr(0x800C636Cu, sizeof(DISPENV)));
    sub_8002E814(3u);
    if (r_u8(0x800EC1D8u) == 0u)
        sub_8001024C(1u, 0x800A3B3Cu, 0xF0u);
    start_tick = r_u32(0x800FF64Cu);
    sub_8006AED8(0x7D0u);
    sub_8007113C();
    sub_8006994C(0x800FF854u);
    resource = sub_8006F29C(0x800A3B48u, 0u);
    w_u8(0x800EAF03u + (resource << 6), 1u);
    resource = sub_8006F29C(0x800FF85Cu, 0u);
    w_u8(0x800EAF03u + (resource << 6), 1u);
    resource = sub_8006F29C(0x800FF864u, 0u);
    w_u8(0x800EAF03u + (resource << 6), 1u);
    w_u8(0x800FF645u, resource);
    resource = sub_8006F29C(0x800FF86Cu, 0u);
    w_u8(0x800EAF03u + (resource << 6), 1u);
    resource = sub_8006F29C(0x800A3B54u, 0u);
    w_u8(0x800EAF03u + (resource << 6), 1u);
    sub_8006F7D8();
    sub_8006AED8(4u);
    if (r_u32(0x800FF64Cu) - start_tick < 0x12Cu && (r_u8(0x800EC138u) == 0u || r_u8(0x800EC158u) == 0u))
    {
        do
        {
            if (r_u8(0x800EC1D8u) != 0u)
                break;
            if (r_u8(0x800EC128u) != 0u)
                break;
            sub_80070748();
            if (r_u32(0x800FF64Cu) - start_tick >= 0x12Cu)
                break;
        } while (r_u8(0x800EC138u) == 0u || r_u8(0x800EC158u) == 0u);
    }
    first_count = (uint32)(sint32)(sint16)r_u16(0x800EC522u);
    second_count = (uint32)(sint32)(sint16)r_u16(0x800EC524u);
    w_u32(0x800FF874u, first_count);
    w_u32(0x800FF878u, second_count);
    sub_8006CDA0();
    sub_8006A834();
    sub_80032A0C();
    sub_8001A760();
    result = sub_8001A858();
    w_u32(0x800FF2ECu, 0u);
    return result;
}

uint32 sub_8006B5FC(void)
{
    uint32 heap = 0u;
    uint32 bounds = 0x800E6C08u;
    uint32 destination = 0x800EAC68u;
    uint32 next = 0x800EAD10u;
    sint32 remaining = 98;
    uint32 span;
    uint32 first_end;
    uint32 first_begin;
    uint32 scaled;
    uint32 result;
    FUNCTION_MARKER(0x8006B5FCu, "SLUS_003.73");
    do
    {
        uint32 begin;
        uint32 end;
        uint32 flags;
        w_u32(0x800FF750u + heap * 4u, 0u);
        w_u32(0x800FF748u + heap * 4u, 0u);
        begin = r_u32(bounds);
        end = r_u32(bounds + 4u);
        flags = r_u32(begin + 4u);
        w_u32(begin + 4u, (flags & 15u) | ((end - begin - 8u) << 4));
        sub_8006B4B4(begin, heap);
        ++heap;
        bounds += 8u;
    } while (heap < 2u);
    do
    {
        w_u32(destination, next);
        destination -= 0xA8u;
        --remaining;
        next -= 0xA8u;
    } while (remaining >= 0);
    first_end = r_u32(0x800E6C0Cu);
    first_begin = r_u32(0x800E6C08u);
    span = first_end - first_begin;
    scaled = span * 80u;
    w_u32(0x800EAD10u, 0u);
    w_u32(0x800FF744u, 0x800E6C18u);
    result = (uint32)(((uint64)scaled * 0x51EB851Fu) >> 37);
    w_u32(0x800FF73Cu, result);
    return result;
}

uint32 sub_8006B4B4(uint32 block, uint32 heap)
{
    uint32 current;
    uint32 previous = 0u;
    uint32 result;
    FUNCTION_MARKER(0x8006B4B4u, "SLUS_003.73");
    current = r_u32(0x800FF750u + heap * 4u);
    while (current < block)
    {
        if (current == 0u)
            break;
        previous = current;
        current = r_u32(current);
    }
    if (previous != 0u)
    {
        uint32 previous_flags = r_u32(previous + 4u);
        uint32 previous_span = (previous_flags >> 4) + 8u;
        if (previous + previous_span == block)
        {
            uint32 block_size = r_u32(block + 4u) >> 4;
            result = previous_span + block_size;
            if (block + block_size + 8u == current)
            {
                uint32 next = r_u32(current);
                uint32 total = (previous_flags >> 4) + 16u;
                uint32 reloaded_block_size;
                w_u32(previous, next);
                reloaded_block_size = r_u32(block + 4u) >> 4;
                result = r_u32(current + 4u) >> 4;
                total += reloaded_block_size + result;
                w_u32(previous + 4u, (previous_flags & 15u) | (total << 4));
                return result;
            }
            result <<= 4;
            w_u32(previous + 4u, (previous_flags & 15u) | result);
            return result;
        }
        w_u32(previous, block);
    }
    else
        w_u32(0x800FF750u + heap * 4u, block);
    result = block + (r_u32(block + 4u) >> 4) + 8u;
    if (result == current)
    {
        uint32 flags = r_u32(block + 4u);
        uint32 next = r_u32(current);
        uint32 current_size;
        w_u32(block, next);
        current_size = r_u32(current + 4u) >> 4;
        result = ((flags >> 4) + 8u + current_size) << 4;
        w_u32(block + 4u, (flags & 15u) | result);
    }
    else
        w_u32(block, current);
    return result;
}

uint32 sub_80085E7C(uint32 retained_v0)
{
    FUNCTION_MARKER(0x80085E7Cu, "SLUS_003.73");
    xport_bios_exit_critical();
    return retained_v0;
}

uint32 sub_80010610(uint32 requested_mode)
{
    uint32 result;
    FUNCTION_MARKER(0x80010610u, "SLUS_003.73");
    sub_8009FA6C(requested_mode);
    sub_8009FAD8();
    _bu_init();
    sub_80085E6C();
    result = OpenEventGuest(0xF4000001u, 0x4u, 0x2000u, 0u);
    w_u32(0x800FEED4u, result);
    result = OpenEventGuest(0xF4000001u, 0x8000u, 0x2000u, 0u);
    w_u32(0x800FEED8u, result);
    result = OpenEventGuest(0xF4000001u, 0x100u, 0x2000u, 0u);
    w_u32(0x800FEEDCu, result);
    result = OpenEventGuest(0xF4000001u, 0x2000u, 0x2000u, 0u);
    w_u32(0x800FEEE0u, result);
    result = OpenEventGuest(0xF0000011u, 0x4u, 0x2000u, 0u);
    w_u32(0x800FEEE4u, result);
    result = OpenEventGuest(0xF0000011u, 0x8000u, 0x2000u, 0u);
    w_u32(0x800FEEE8u, result);
    result = OpenEventGuest(0xF0000011u, 0x100u, 0x2000u, 0u);
    w_u32(0x800FEEECu, result);
    result = OpenEventGuest(0xF0000011u, 0x2000u, 0x2000u, 0u);
    w_u32(0x800FEEF0u, result);
    sub_80085E7C(result);
    result = EnableEvent(r_u32(0x800FEED4u));
    result = EnableEvent(r_u32(0x800FEED8u));
    result = EnableEvent(r_u32(0x800FEEDCu));
    result = EnableEvent(r_u32(0x800FEEE0u));
    result = EnableEvent(r_u32(0x800FEEE4u));
    result = EnableEvent(r_u32(0x800FEEE8u));
    result = EnableEvent(r_u32(0x800FEEECu));
    result = EnableEvent(r_u32(0x800FEEF0u));
    w_u32(0x800FEED0u, 0u);
    w_u32(0x800FEEC8u, 0u);
    return result;
}

uint32 sub_8009FA6C(uint32 requested_mode)
{
    uint32 mode = requested_mode;
    uint32 result;
    FUNCTION_MARKER(0x8009FA6Cu, "SLUS_003.73");
    ChangeClearPAD(0u);
    sub_80085E6C();
    if (ReadInitPadFlag() == 0u)
        mode = 0u;
    sub_8009FF8C(mode);
    _copy_memcard_patch();
    _patch_card();
    result = _patch_card2();
    return sub_80085E7C(result);
}

uint32 sub_80085E6C(void)
{
    FUNCTION_MARKER(0x80085E6Cu, "SLUS_003.73");
    return (uint32)xport_bios_enter_critical();
}

uint32 sub_8009FF8C(uint32 mode)
{
    FUNCTION_MARKER(0x8009FF8Cu, "SLUS_003.73");
    return xport_bios_init_card(mode);
}

uint32 sub_8009FAD8(void)
{
    uint32 result;
    FUNCTION_MARKER(0x8009FAD8u, "SLUS_003.73");
    sub_80085E6C();
    sub_8009FF9C();
    result = ChangeClearPAD(0u);
    return sub_80085E7C(result);
}

uint32 sub_8009FF9C(void)
{
    FUNCTION_MARKER(0x8009FF9Cu, "SLUS_003.73");
    return xport_bios_start_card();
}

uint32 sub_8006FE4C(void)
{
    uint32 controller = 0x800EC0F8u;
    uint32 count = 0u;
    uint32 index;
    FUNCTION_MARKER(0x8006FE4Cu, "SLUS_003.73");
    do
    {
        uint32 x = (uint32)(sint32)(sint16)r_u16(0x800EC474u);
        uint32 y = (uint32)(sint32)(sint16)r_u16(0x800EC476u);
        uint32 width = (uint32)(sint32)(sint16)r_u16(0x800EC478u);
        uint32 height = (uint32)(sint32)(sint16)r_u16(0x800EC47Au);
        uint32 destination;
        ++count;
        sub_8006FDFC(controller, x, y, width, height);
        destination = controller;
        x = (uint32)(sint32)(sint16)r_u16(0x800EC494u);
        y = (uint32)(sint32)(sint16)r_u16(0x800EC496u);
        width = (uint32)(sint32)(sint16)r_u16(0x800EC478u);
        height = (uint32)(sint32)(sint16)r_u16(0x800EC47Au);
        controller += 0x17Cu;
        sub_8006FE14(destination, 3u, 2u, 1u, 0u, x, y, width, height);
    } while (count < 2u);
    for (index = 0u; index < 256u; ++index)
    {
        uint32 destination = 0x801027A8u + index;
        w_u8(destination, 0u);
        if ((index & 1u) != 0u)
            w_u8(destination, 1u);
        if ((index & 2u) != 0u)
            w_u8(destination, r_u8(destination) | 4u);
        if ((index & 4u) != 0u)
            w_u8(destination, r_u8(destination) | 0x80u);
        if ((index & 8u) != 0u)
            w_u8(destination, r_u8(destination) | 0x10u);
        if ((index & 0x10u) != 0u)
            w_u8(destination, r_u8(destination) | 8u);
        if ((index & 0x20u) != 0u)
            w_u8(destination, r_u8(destination) | 0x20u);
        if ((index & 0x40u) != 0u)
            w_u8(destination, r_u8(destination) | 0x40u);
        if ((index & 0x80u) != 0u)
            w_u8(destination, r_u8(destination) | 2u);
    }
    sub_8007001C();
    apocalypse_pad_init_mtap(0x800EC3F0u, 0x800EC412u);
    sub_8009C7CC();
    w_u32(0x800FF814u, 0u);
    w_u32(0x800FF82Cu, 1u);
    return 1u;
}

uint32 sub_8006FDFC(uint32 destination, uint32 x, uint32 y, uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x8006FDFCu, "SLUS_003.73");
    w_u32(destination + 0x140u, x);
    w_u32(destination + 0x144u, y);
    w_u32(destination + 0x148u, width);
    w_u32(destination + 0x14Cu, height);
    return height;
}

uint32 sub_8006FE14(uint32 destination, uint32 red, uint32 green, uint32 blue, uint32 mode, uint32 x, uint32 y, uint32 width, uint32 height)
{
    FUNCTION_MARKER(0x8006FE14u, "SLUS_003.73");
    w_u8(destination + 0x160u, red);
    w_u8(destination + 0x161u, green);
    w_u8(destination + 0x162u, blue);
    w_u8(destination + 0x163u, mode);
    w_u32(destination + 0x150u, x);
    w_u32(destination + 0x154u, y);
    w_u32(destination + 0x158u, width);
    w_u32(destination + 0x15Cu, height);
    return mode;
}

uint32 sub_8007001C(void)
{
    uint32 index = 0;
    uint32 controller = 0x800EC0F8u;
    uint32 allocation;
    FUNCTION_MARKER(0x8007001Cu, "SLUS_003.73");
    do
    {
        sub_800700B0(controller);
        w_u32(controller + 0x170u, 0u);
        ++index;
        controller += 0x17Cu;
    } while (index < 2u);
    allocation = r_u32(0x800FF81Cu);
    w_u32(0x800FF830u, 0u);
    w_u32(0x800FF820u, allocation);
    return allocation;
}

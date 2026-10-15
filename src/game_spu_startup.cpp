#include "game_sdk_bindings.h"
extern "C"
{
#include "game_spu_startup.h"
#include "psx_spu.h"
#include "draft_first_adapters.h"
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern "C" uint32 sub_8008655C(void);

static void FailSpuStartup(const char *operation)
{
    GameDiagnostic("TODO SPU startup: %s\n", operation);
    abort();
}

extern "C" void apocalypse_spu_bind_reverb_presets(void)
{
    uint32 work[10], index;
    for (index = 0; index < 10; ++index)
        work[index] = r_u32(0x800FD6AC + index * 4);
    spu_bind_reverb_presets(work, (const uint8 *)psx_addr(0x800FD6DC, 680), r_u32(0x800FD264));
}

static void CompleteNativeSpuDma(void)
{
    uint32 base = r_u32(0x800FD23C);
    w_u16(base + 426, r_u16(base + 426) & 0xFFCF);
    if (r_u32(0x800FD274) != 0)
        FailSpuStartup("Dispatch selected guest transfer callback");
    DeliverEvent(0xF0000009, 0x20);
}

static void BindNativeSpuTransfer(void)
{
    SpuNativeTransferGuestBinding binding = {0x800FD1CC, 0x800FD258, 0x800FD254, 0x800FD270, 0x800FD274, 0x800FD290, 0x800FD294, 3};
    if (!spu_bind_native_transfer_guest(&binding))
        FailSpuStartup("Bind native SPU transfer state");
}

extern "C" uint32 apocalypse_spu_init(uint32 mode)
{
    uint32 base, dma_control, index;
    static const uint32 zero_words[] = {0x800FD1D0, 0x800FD1D4, 0x800FD1E0, 0x800FD1E8, 0x800FD1EC, 0x800FD29C, 0x800FD2A0, 0x800FD2A4, 0x800FD1CC, 0x800FD258, 0x800FD1C8, 0x800FD1F4, 0x800FD1F0, 0x800FD228};
    if (mode != 0)
        FailSpuStartup("Warm _SpuInit mode");
    sub_8008655C();
    /* Initialize the native sequencer before populating SPU storage */
    SsInit();
    SpuInit();
    spu_bind_heap(0);
    base = r_u32(0x800FD23C);
    dma_control = r_u32(0x800FD24C);
    w_u32(dma_control, r_u32(dma_control) | 0xB0000);
    w_u32(0x800FD258, 0);
    w_u32(0x800FD25C, 0);
    w_u16(0x800FD254, 0);
    w_u16(base + 384, 0);
    w_u16(base + 386, 0);
    w_u16(base + 426, 0);
    w_u32(0x800FD260, 2);
    w_u32(0x800FD264, 3);
    w_u32(0x800FD268, 8);
    w_u32(0x800FD26C, 7);
    w_u16(base + 428, 4);
    w_u16(base + 388, 0);
    w_u16(base + 390, 0);
    w_u16(base + 396, 65535);
    w_u16(base + 398, 65535);
    w_u16(base + 408, 0);
    w_u16(base + 410, 0);
    memset(psx_addr(0x801057F0, 20), 0, 20);
    w_u16(0x800FD254, 512);
    for (index = 400; index <= 406; index += 2)
        w_u16(base + index, 0);
    for (index = 432; index <= 438; index += 2)
        w_u16(base + index, 0);
    if (!spu_upload(4096, psx_addr(0x800FD27C, 16), 16))
        FailSpuStartup("Upload initial silent sample");
    for (index = 0; index < 24; ++index)
    {
        uint32 voice = base + 16 * index;
        w_u16(voice, 0);
        w_u16(voice + 2, 0);
        w_u16(voice + 4, 0x3FFF);
        w_u16(voice + 6, 512);
        w_u16(voice + 8, 0);
        w_u16(voice + 10, 0);
    }
    w_u16(base + 392, 65535);
    w_u16(base + 394, 255);
    w_u16(base + 396, 65535);
    w_u16(base + 398, 255);
    w_u32(0x800FD270, 1);
    w_u16(base + 426, 0xC000);
    w_u32(0x800FD274, 0);
    w_u32(0x800FD278, 0);
    for (index = 0; index < 24; ++index)
        w_u16(0x800FD226 - 2 * index, 0xC000);
    if (r_u32(0x800FD22C) == 0)
    {
        uint32 event;
        w_u32(0x800FD22C, 1);
        xport_bios_enter_critical();
        DMACallback(4, CompleteNativeSpuDma);
        event = (uint32)OpenEventGuest(0xF0000009, 0x20, 0x2000, 0);
        w_u32(0x800FD1C4, event);
        EnableEvent(event);
        xport_bios_exit_critical();
    }
    for (index = 0; index < sizeof(zero_words) / sizeof(zero_words[0]); ++index)
        w_u32(zero_words[index], 0);
    w_u16(0x800FD1E4, 0);
    w_u16(0x800FD1E6, 0);
    w_u32(0x800FD1D8, r_u32(0x800FD6AC));
    w_u16(base + 418, r_u32(0x800FD6AC));
    BindNativeSpuTransfer();
    apocalypse_spu_bind_reverb_presets();
    return base + 418;
}

extern "C" uint32 apocalypse_spu_clear_reverb(uint32 mode)
{
    uint32 shift = r_u32(0x800FD264) & 31;
    uint32 remaining, destination, callback, normalized, base;
    sint32 result;
    if (mode >= 10)
        return 0xFFFFFFFF;
    apocalypse_spu_bind_reverb_presets();
    result = SpuClearReverbWorkArea((sint32)mode);
    if (result != 0)
        return (uint32)result;
    destination = (mode != 0 ? r_u32(0x800FD6AC + mode * 4) : 65520) << shift;
    remaining = (mode != 0 ? 65536 - r_u32(0x800FD6AC + mode * 4) : 16) << shift;
    normalized = r_u32(0x800FD258);
    callback = r_u32(0x800FD274);
    if (normalized == 1)
        w_u32(0x800FD258, 0);
    if (callback != 0)
        w_u32(0x800FD274, 0);
    base = r_u32(0x800FD23C);
    do
    {
        uint32 more = remaining > 1024;
        uint32 chunk = more ? 1024 : remaining;
        w_u16(0x800FD254, destination >> shift);
        w_u16(base + 422, destination >> shift);
        w_u32(0x800FD28C, 0);
        w_u16(base + 426, (r_u16(base + 426) & 0xFFCF) | 0x20);
        w_u32(0x800FD290, 0x800FD2AC);
        w_u32(0x800FD294, (chunk >> 6) + ((chunk & 63) != 0));
        CompleteNativeSpuDma();
        TestEvent(r_u32(0x800FD1C4));
        remaining -= 1024;
        destination += 1024;
        if (!more)
            break;
    } while (remaining != 0);
    if (normalized == 1)
        w_u32(0x800FD258, normalized);
    if (callback != 0)
        w_u32(0x800FD274, callback);
    return 0;
}

static void ReplaceVoiceMask(uint32 register_index, uint32 mask, uint32 clear)
{
    uint32 buffered = r_u32(0x800FD228) & 1;
    uint32 base = buffered ? 0x80105668 : r_u32(0x800FD23C);
    uint32 low_address = base + register_index * 2;
    uint32 high_address = low_address + 2;
    uint16 low = (uint16)mask, high = (uint16)((mask >> 16) & 255);
    if (clear != 0)
    {
        low = r_u16(low_address) & ~low;
        high = r_u16(high_address) & ~high;
    }
    w_u16(low_address, low);
    w_u16(high_address, high);
    if (buffered)
        w_u32(0x800FD1F4, r_u32(0x800FD1F4) | (1 << ((register_index - 0xC6) / 2)));
    else if (register_index == 0xCC)
    {
        SpuSetReverbVoice(0, 0xFFFFFF);
        SpuSetReverbVoice(1, low | ((uint32)high << 16));
    }
}

extern "C" void apocalypse_ss_flush(void)
{
    uint32 index = (r_u32(0x801054DC) + 1) & 15;
    uint32 i, inactive = 0xFFFFFFFF, mask, low, high, key_off_low, key_on_low, key_off_high, key_on_high;
    w_u32(0x801054DC, index);
    w_u32(0x801054E0 + index * 4, 0);
    for (i = 0; (sint32)i < (sint32)(sint8)r_u8(0x8010561C); ++i)
    {
#if defined(LOCKSTEP_DEBUG)
        uint32 base = r_u32(0x800FD23C);
        uint16 envelope = r_u16(base + 12 + i * 16);
#else
        SpuVoiceAttr attributes = {0};
        attributes.voice = 1u << (i & 31);
        SpuGetVoiceAttr(&attributes);
        uint16 envelope = (uint16)attributes.envx;
#endif
        w_u16(0x80104E2E + i * 54, envelope);
        if (r_u16(0x80104E2E + i * 54) == 0)
        {
            index = r_u32(0x801054DC);
            mask = r_u32(0x801054E0 + index * 4);
            w_u32(0x801054E0 + index * 4, mask | (1u << (i & 31)));
        }
    }
    if (r_u8(0x80105650) == 0)
    {
        for (i = 0; i < 15; ++i)
            inactive &= r_u32(0x801054E0 + i * 4);
        for (i = 0; (sint32)i < (sint32)(sint8)r_u8(0x8010561C); ++i)
            if ((inactive & (1u << (i & 31))) != 0)
            {
                if (r_u8(0x80104E45 + i * 54) == 2)
                {
                    // Original SRA16 extends the low voice mask through the high byte
                    if ((sint32)i < 16)
                        mask = (uint32)(sint32)(sint16)(1 << i);
                    else
                        mask = ((1u << ((i - 16) & 31)) & 255) << 16;
                    ReplaceVoiceMask(0xCA, mask, 1);
                }
                w_u8(0x80104E45 + i * 54, 0);
            }
    }
    key_off_low = r_u16(0x80105520);
    key_on_low = r_u16(0x80104E18);
    key_off_high = r_u16(0x80105522);
    w_u16(0x80104E18, key_on_low & ~key_off_low);
    key_on_high = r_u16(0x80104E1A);
    w_u16(0x80104E1A, key_on_high & ~key_off_high);
    for (i = 0; i < 24; ++i)
    {
        if (r_u16(0x80104E46 + i * 54) != 0)
            FailSpuStartup("905CC sequencer first guest voice callback");
        if (r_u16(0x80104E52 + i * 54) != 0)
            FailSpuStartup("905CC sequencer second guest voice callback");
    }
    for (i = 0; i < 24; ++i)
    {
        uint32 shadow = 0x80105358 + i * 16;
        SpuVoiceAttr attr;
        memset(&attr, 0, sizeof(attr));
        attr.voice = 1 << i;
        if ((r_u8(0x80105338 + i) & 1) != 0)
        {
            attr.mask |= 3;
            attr.volume.left = (sint16)r_u16(shadow);
            attr.volume.right = (sint16)r_u16(shadow + 2);
        }
        if ((r_u8(0x80105338 + i) & 4) != 0)
        {
            attr.mask |= 0x10;
            attr.pitch = r_u16(shadow + 4);
        }
        if ((r_u8(0x80105338 + i) & 8) != 0)
        {
            attr.mask |= 0x80;
            attr.addr = (uint32)r_u16(shadow + 6) << 3;
        }
        if ((r_u8(0x80105338 + i) & 0x10) != 0)
        {
            attr.mask |= 0x60000;
            attr.adsr1 = r_u16(shadow + 8);
            attr.adsr2 = r_u16(shadow + 10);
        }
        if (attr.mask != 0)
            SpuSetVoiceAttr(&attr);
        w_u8(0x80105338 + i, 0);
    }
    high = r_u8(0x80105522);
    low = r_u16(0x80105520);
    SpuSetKey(0, low | (high << 16));
    high = r_u8(0x80104E1A);
    low = r_u16(0x80104E18);
    SpuSetKey(1, low | (high << 16));
    // Mode 8 replaces the complete original voice masks
    high = r_u8(0x80104E1E);
    low = r_u16(0x80104E1C);
    ReplaceVoiceMask(0xCC, low | (high << 16), 0);
    high = r_u8(0x80104E22);
    low = r_u16(0x80104E20);
    ReplaceVoiceMask(0xCA, low | (high << 16), 0);
    w_u16(0x80105520, 0);
    w_u16(0x80105522, 0);
    w_u16(0x80104E18, 0);
    w_u16(0x80104E1A, 0);
    w_u16(0x80104E20, 0);
    w_u16(0x80104E22, 0);
}

static void InitializeSequencer(uint32 requested_count)
{
    static const uint32 offsets[] = {4, 6, 8, 12, 18, 20, 30, 32, 34, 36, 38, 42, 44, 46, 48, 50};
    SpuMallocGuestBinding malloc_binding = {0x800FD264, 0x800FD26C, 0x800FD29C, 0x800FD2A0, 0x800FD2A4, 0x800FD1D0, 0x800FD1D8};
    uint32 count = (uint32)(sint32)(sint8)requested_count < 24 ? (uint32)(sint32)(sint8)requested_count : 24;
    uint32 i, j;
    w_u32(0x800FD270, 1);
    w_u16(0x801055C0, 0);
    if (!spu_bind_malloc_guest(&malloc_binding))
        FailSpuStartup("9532C malloc guest binding");
    spu_init_malloc_guest(32, 0x801056E8);
    for (i = 0; i < 192; ++i)
        w_u16(0x80105358 + i * 2, 0);
    for (i = 0; i < 24; ++i)
        w_u8(0x80105338 + i, 0);
    w_u16(0x80105698, 0);
    for (i = 0; i < 16; ++i)
        w_u8(0x80105640 + i, 0);
    w_u8(0x8010561C, count);
    for (i = 0; i < count; ++i)
    {
        uint32 record = 0x80104E28 + i * 54;
        uint32 bit = 1 << i;
        SpuVoiceAttr attr;
        w_u16(record, 255);
        w_u16(record + 2, 24);
        for (j = 0; j < sizeof(offsets) / sizeof(offsets[0]); ++j)
            w_u16(record + offsets[j], 0);
        w_u16(record + 16, 0xFFFF);
        w_u16(record + 22, 255);
        w_u8(record + 10, 64);
        w_u8(record + 29, 0);
        memset(&attr, 0, sizeof(attr));
        attr.voice = bit;
        attr.mask = 0x60093;
        attr.pitch = 0x1000;
        attr.addr = 0x1000;
        attr.adsr1 = 0x80FF;
        attr.adsr2 = 0x4000;
        SpuSetVoiceAttr(&attr);
        w_u16(0x80105638, i);
        w_u8(record + 29, 0);
        w_u16(record + 4, 0);
        w_u16(record, 0);
        w_u16(0x80105520, r_u16(0x80105520) | bit);
        w_u16(0x80105522, r_u16(0x80105522) | (bit >> 16));
        w_u16(0x80104E18, r_u16(0x80104E18) & ~bit);
        w_u16(0x80104E1A, r_u16(0x80104E1A) & ~(bit >> 16));
    }
    w_u32(0x80105528, 0);
    w_u16(0x80105530, 0x3FFF);
    w_u16(0x80105532, 0x3FFF);
    w_u32(0x8010552C, 0);
    w_u16(0x80104E18, 0);
    w_u16(0x80104E1A, 0);
    w_u16(0x80105520, 0);
    w_u16(0x80104E1C, 0);
    w_u16(0x80104E1E, 0);
    w_u16(0x80104E20, 0);
    w_u16(0x80104E22, 0);
    w_u8(0x80105650, 0);
    w_u16(0x80105608, 0);
    w_u16(0x8010560A, 0x80);
    apocalypse_ss_flush();
}

extern "C" uint32 apocalypse_ss_init(void)
{
    uint32 voice, field;
    SsInit();
    xport_libsnd_bind_voice_volume(0x80105358, 0x80105338);
    static const SpuVmGuestBinding vm = {0x80105620, 0x80104E28, 0x8010560C, 0x80105614, 0x80105618, 0x80105640, 0x8010560A, 0x80105580, 0x80105540, 0x801055C8, 0x8010561C, 0x800FD01C, 0x801054E0, 0x801055C0, 0x80105608, 0x80104E18, 0x80105520};
    if (!spu_bind_vm_guest(&vm))
        FailSpuStartup("Invalid libsnd VM binding");
    xport_libsnd_bind_sequence_table(0x80104590);
    for (voice = 0; voice < 24; ++voice)
        for (field = 0; field < 8; ++field)
            w_u16(0x1F801C00 + voice * 16 + field * 2, r_u16(0x800FD028 + field * 2));
    for (field = 0; field < 16; ++field)
        w_u16(0x1F801D80 + field * 2, r_u16(0x800FD038 + field * 2));
    InitializeSequencer(24);
    for (field = 0; field < 512; ++field)
        w_u32(0x80104610 + field * 4, 0);
    w_u32(0x8010458C, 60);
    w_u32(0x80104588, 0);
    w_u32(0x80104584, 0);
    return 60;
}

extern "C" uint32 apocalypse_ss_serial_attr(uint32 serial, uint32 attribute, uint32 value)
{
    sint32 channel = (sint8)serial, selector = (sint8)attribute;
    SpuCommonAttr attr;
    memset(&attr, 0, sizeof(attr));
    if ((uint32)channel > 1 || (uint32)selector > 1)
        FailSpuStartup("8E93C unsupported serial or attribute");
    if (channel == 0)
    {
        attr.mask = selector == 0 ? 0x200 : 0x100;
        if (selector == 0)
            attr.cd.mix = (sint8)value;
        else
            attr.cd.reverb = (sint8)value;
    }
    else
    {
        attr.mask = selector == 0 ? 0x2000 : 0x1000;
        if (selector == 0)
            attr.ext.mix = (sint8)value;
        else
            attr.ext.reverb = (sint8)value;
    }
    SpuSetCommonAttr(&attr);
    return (attr.mask & 0x2000) != 0 ? r_u16(0x1F801DAA) : 0;
}

extern "C" uint32 apocalypse_ss_serial_volume(uint32 serial, uint32 left, uint32 right)
{
    sint32 channel = (sint8)serial;
    sint16 l = (sint16)left, r = (sint16)right;
    SpuCommonAttr attr;
    if ((uint32)channel > 1)
        FailSpuStartup("8F05C unsupported serial");
    if (l >= 128)
        l = 127;
    if (r >= 128)
        r = 127;
    memset(&attr, 0, sizeof(attr));
    attr.mask = channel == 0 ? 0xC0 : 0xC00;
    if (channel == 0)
    {
        attr.cd.volume.left = (sint16)((sint32)l * 258);
        attr.cd.volume.right = (sint16)((sint32)r * 258);
    }
    else
    {
        attr.ext.volume.left = (sint16)((sint32)l * 258);
        attr.ext.volume.right = (sint16)((sint32)r * 258);
    }
    SpuSetCommonAttr(&attr);
    if (channel == 0)
        SsSetSerialVol(0, l, r);
    return 0;
}

extern "C" uint32 sub_8008E10C(void)
{
    FUNCTION_MARKER(0x8008E10C, "SLUS_003.73");
    sub_8008655C();
    sub_8009459C();
    sub_800968DC(7);
    return sub_8008E01C();
}

extern "C" uint32 sub_8009459C(void)
{
    FUNCTION_MARKER(0x8009459C, "SLUS_003.73");
    return sub_800945BC(0);
}

extern "C" uint32 sub_80096DEC(uint32 a1)
{
    FUNCTION_MARKER(0x80096DEC, "SLUS_003.73");
    w_u32(0x800FD270, a1 != 1);
    return 1;
}

extern "C" uint32 sub_80096E14(void)
{
    FUNCTION_MARKER(0x80096E14, "SLUS_003.73");
    return r_u32(0x800FD270) != 1;
}

extern "C" uint32 sub_80096C4C(uint32 a1, uint32 a2)
{
    FUNCTION_MARKER(0x80096C4C, "SLUS_003.73");
    uint32 size = a2 > 0x7EFF0 ? 0x7EFF0 : a2;
    sub_80094F40(a1, size);
    if (r_u32(0x800FD274) == 0)
        w_u32(0x800FD270, 0);
    return size;
}

extern "C" uint32 sub_800901EC(void)
{
    FUNCTION_MARKER(0x800901EC, "SLUS_003.73");
    uint32 result = (uint32)SpuSetReverb(1);
    w_u32(0x800FD1D0, result);
    return result;
}

extern "C" uint32 sub_8008E01C(void)
{
    FUNCTION_MARKER(0x8008E01C, "SLUS_003.73");
    return apocalypse_ss_init();
}

extern "C" uint32 sub_8008E93C(uint32 serial, uint32 attribute, uint32 value)
{
    FUNCTION_MARKER(0x8008E93C, "SLUS_003.73");
    return apocalypse_ss_serial_attr(serial, attribute, value);
}

extern "C" uint32 sub_8008F05C(uint32 serial, uint32 left, uint32 right)
{
    FUNCTION_MARKER(0x8008F05C, "SLUS_003.73");
    return apocalypse_ss_serial_volume(serial, left, right);
}

extern "C" uint32 sub_800900AC(uint32 left, uint32 right)
{
    FUNCTION_MARKER(0x800900AC, "SLUS_003.73");
    SpuReverbAttr attribute = {0};
    sint32 left_depth = (32767 * (sint32)(sint16)left) / 127;
    sint32 right_depth = (32767 * (sint32)(sint16)right) / 127;
    w_u32(0x80105528, 6);
    w_u16(0x80105530, (uint16)left_depth);
    w_u16(0x80105532, (uint16)right_depth);
    /* Preserve the depth-only SpuSetReverbModeParam guest state */
    w_u16(r_u32(0x800FD23C) + 388, r_u16(0x80105530));
    w_u16(0x800FD1E4, r_u16(0x80105530));
    w_u16(r_u32(0x800FD23C) + 390, r_u16(0x80105532));
    w_u16(0x800FD1E6, r_u16(0x80105532));
    attribute.mask = 6;
    attribute.depth.left = (sint16)r_u16(0x80105530);
    attribute.depth.right = (sint16)r_u16(0x80105532);
    SpuSetReverbDepth(&attribute);
    return 0;
}

extern "C" uint32 sub_800945BC(uint32 mode)
{
    FUNCTION_MARKER(0x800945BC, "SLUS_003.73");
    return apocalypse_spu_init(mode);
}

extern "C" uint32 sub_800968DC(uint32 mode)
{
    FUNCTION_MARKER(0x800968DC, "SLUS_003.73");
    return apocalypse_spu_clear_reverb(mode);
}

extern "C" uint32 sub_800936AC(uint32 count)
{
    FUNCTION_MARKER(0x800936AC, "SLUS_003.73");
    if (((count - 1) & 255) >= 24)
        return 0xFFFFFFFF;
    w_u8(0x8010561C, count);
    return (uint32)(sint32)(sint8)count;
}

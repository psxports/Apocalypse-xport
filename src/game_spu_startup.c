#include "game_spu_startup.h"
#include "psx_spu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern uint32 sub_8008655C(void);

static void spu_startup_todo(const char *operation)
{
    fprintf(stderr, "TODO SPU startup: %s\n", operation);
    abort();
}

void apocalypse_spu_bind_reverb_presets(void)
{
    uint32 work[10], index;
    for (index = 0u; index < 10u; ++index)
        work[index] = r_u32(0x800FD6ACu + index * 4u);
    spu_bind_reverb_presets(work, (const uint8 *)psx_addr(0x800FD6DCu, 680u), r_u32(0x800FD264u));
}

static void native_spu_dma_complete(uint8 status, uint8 *result)
{
    uint32 base = r_u32(0x800FD23Cu);
    (void)status;
    (void)result;
    w_u16(base + 426u, r_u16(base + 426u) & 0xFFCFu);
    if (r_u32(0x800FD274u) != 0u)
        spu_startup_todo("Dispatch selected guest transfer callback");
    DeliverEvent(0xF0000009u, 0x20u);
}

static void bind_native_transfer(void)
{
    SpuNativeTransferGuestBinding binding = {0x800FD1CCu, 0x800FD258u, 0x800FD254u, 0x800FD270u, 0x800FD274u, 0x800FD290u, 0x800FD294u, 3u};
    if (!spu_bind_native_transfer_guest(&binding))
        spu_startup_todo("Bind native SPU transfer state");
}

uint32 apocalypse_spu_init(uint32 mode)
{
    uint32 base, dma_control, index;
    static const uint32 zero_words[] = {0x800FD1D0u, 0x800FD1D4u, 0x800FD1E0u, 0x800FD1E8u, 0x800FD1ECu, 0x800FD29Cu, 0x800FD2A0u, 0x800FD2A4u, 0x800FD1CCu, 0x800FD258u, 0x800FD1C8u, 0x800FD1F4u, 0x800FD1F0u, 0x800FD228u};
    if (mode != 0u)
        spu_startup_todo("Warm _SpuInit mode");
    sub_8008655C();
    /* Initialize the native sequencer before populating SPU storage */
    SsInit();
    SpuInit();
    spu_bind_heap(NULL);
    base = r_u32(0x800FD23Cu);
    dma_control = r_u32(0x800FD24Cu);
    w_u32(dma_control, r_u32(dma_control) | 0xB0000u);
    w_u32(0x800FD258u, 0u);
    w_u32(0x800FD25Cu, 0u);
    w_u16(0x800FD254u, 0u);
    w_u16(base + 384u, 0u);
    w_u16(base + 386u, 0u);
    w_u16(base + 426u, 0u);
    w_u32(0x800FD260u, 2u);
    w_u32(0x800FD264u, 3u);
    w_u32(0x800FD268u, 8u);
    w_u32(0x800FD26Cu, 7u);
    w_u16(base + 428u, 4u);
    w_u16(base + 388u, 0u);
    w_u16(base + 390u, 0u);
    w_u16(base + 396u, 65535u);
    w_u16(base + 398u, 65535u);
    w_u16(base + 408u, 0u);
    w_u16(base + 410u, 0u);
    memset(psx_addr(0x801057F0u, 20u), 0, 20u);
    w_u16(0x800FD254u, 512u);
    for (index = 400u; index <= 406u; index += 2u)
        w_u16(base + index, 0u);
    for (index = 432u; index <= 438u; index += 2u)
        w_u16(base + index, 0u);
    if (!spu_upload(4096u, psx_addr(0x800FD27Cu, 16u), 16u))
        spu_startup_todo("Upload initial silent sample");
    for (index = 0u; index < 24u; ++index)
    {
        uint32 voice = base + 16u * index;
        w_u16(voice, 0u);
        w_u16(voice + 2u, 0u);
        w_u16(voice + 4u, 0x3FFFu);
        w_u16(voice + 6u, 512u);
        w_u16(voice + 8u, 0u);
        w_u16(voice + 10u, 0u);
    }
    w_u16(base + 392u, 65535u);
    w_u16(base + 394u, 255u);
    w_u16(base + 396u, 65535u);
    w_u16(base + 398u, 255u);
    w_u32(0x800FD270u, 1u);
    w_u16(base + 426u, 0xC000u);
    w_u32(0x800FD274u, 0u);
    w_u32(0x800FD278u, 0u);
    for (index = 0u; index < 24u; ++index)
        w_u16(0x800FD226u - 2u * index, 0xC000u);
    if (r_u32(0x800FD22Cu) == 0u)
    {
        uint32 event;
        w_u32(0x800FD22Cu, 1u);
        xport_bios_enter_critical();
        DMACallback(4u, native_spu_dma_complete);
        event = (uint32)OpenEventGuest(0xF0000009u, 0x20u, 0x2000u, 0u);
        w_u32(0x800FD1C4u, event);
        EnableEvent(event);
        xport_bios_exit_critical();
    }
    for (index = 0u; index < sizeof(zero_words) / sizeof(zero_words[0]); ++index)
        w_u32(zero_words[index], 0u);
    w_u16(0x800FD1E4u, 0u);
    w_u16(0x800FD1E6u, 0u);
    w_u32(0x800FD1D8u, r_u32(0x800FD6ACu));
    w_u16(base + 418u, r_u32(0x800FD6ACu));
    bind_native_transfer();
    apocalypse_spu_bind_reverb_presets();
    return base + 418u;
}

uint32 apocalypse_spu_clear_reverb(uint32 mode)
{
    uint32 shift = r_u32(0x800FD264u) & 31u;
    uint32 remaining, destination, callback, normalized, base;
    sint32 result;
    if (mode >= 10u)
        return 0xFFFFFFFFu;
    apocalypse_spu_bind_reverb_presets();
    result = SpuClearReverbWorkArea((sint32)mode);
    if (result != 0)
        return (uint32)result;
    destination = (mode != 0u ? r_u32(0x800FD6ACu + mode * 4u) : 65520u) << shift;
    remaining = (mode != 0u ? 65536u - r_u32(0x800FD6ACu + mode * 4u) : 16u) << shift;
    normalized = r_u32(0x800FD258u);
    callback = r_u32(0x800FD274u);
    if (normalized == 1u)
        w_u32(0x800FD258u, 0u);
    if (callback != 0u)
        w_u32(0x800FD274u, 0u);
    base = r_u32(0x800FD23Cu);
    do
    {
        uint32 more = remaining > 1024u;
        uint32 chunk = more ? 1024u : remaining;
        w_u16(0x800FD254u, destination >> shift);
        w_u16(base + 422u, destination >> shift);
        w_u32(0x800FD28Cu, 0u);
        w_u16(base + 426u, (r_u16(base + 426u) & 0xFFCFu) | 0x20u);
        w_u32(0x800FD290u, 0x800FD2ACu);
        w_u32(0x800FD294u, (chunk >> 6) + ((chunk & 63u) != 0u));
        native_spu_dma_complete(0u, NULL);
        TestEvent(r_u32(0x800FD1C4u));
        remaining -= 1024u;
        destination += 1024u;
        if (!more)
            break;
    } while (remaining != 0u);
    if (normalized == 1u)
        w_u32(0x800FD258u, normalized);
    if (callback != 0u)
        w_u32(0x800FD274u, callback);
    return 0u;
}

static void replace_voice_mask(uint32 register_index, uint32 mask, uint32 clear)
{
    uint32 buffered = r_u32(0x800FD228u) & 1u;
    uint32 base = buffered ? 0x80105668u : r_u32(0x800FD23Cu);
    uint32 low_address = base + register_index * 2u;
    uint32 high_address = low_address + 2u;
    uint16 low = (uint16)mask, high = (uint16)((mask >> 16) & 255u);
    if (clear != 0u)
    {
        low = r_u16(low_address) & ~low;
        high = r_u16(high_address) & ~high;
    }
    w_u16(low_address, low);
    w_u16(high_address, high);
    if (buffered)
        w_u32(0x800FD1F4u, r_u32(0x800FD1F4u) | (1u << ((register_index - 0xC6u) / 2u)));
    else if (register_index == 0xCCu)
    {
        SpuSetReverbVoice(0, 0xFFFFFFu);
        SpuSetReverbVoice(1, low | ((uint32)high << 16));
    }
}

void apocalypse_ss_flush(void)
{
    uint32 index = (r_u32(0x801054DCu) + 1u) & 15u;
    uint32 count = (uint32)(sint32)(sint8)r_u8(0x8010561Cu), i, inactive = 0xFFFFFFFFu;
    w_u32(0x801054DCu, index);
    w_u32(0x801054E0u + index * 4u, 0u);
    for (i = 0; (sint32)i < (sint32)count; ++i)
    {
        uint16 envelope = r_u16(0x1F801C0Cu + i * 16u);
        w_u16(0x80104E2Eu + i * 54u, envelope);
        if (envelope == 0u)
            w_u32(0x801054E0u + index * 4u, r_u32(0x801054E0u + index * 4u) | (1u << i));
    }
    if (r_u8(0x80105650u) == 0u)
    {
        for (i = 0; i < 15u; ++i)
            inactive &= r_u32(0x801054E0u + i * 4u);
        for (i = 0; (sint32)i < (sint32)count; ++i)
            if ((inactive & (1u << i)) != 0u)
            {
                if (r_u8(0x80104E45u + i * 54u) == 2u)
                    replace_voice_mask(0xCAu, 1u << i, 1u);
                w_u8(0x80104E45u + i * 54u, 0u);
            }
    }
    w_u16(0x80104E18u, r_u16(0x80104E18u) & ~r_u16(0x80105520u));
    w_u16(0x80104E1Au, r_u16(0x80104E1Au) & ~r_u16(0x80105522u));
    for (i = 0; i < 24u; ++i)
    {
        uint32 shadow = 0x80105358u + i * 16u;
        uint8 dirty = r_u8(0x80105338u + i);
        SpuVoiceAttr attr;
        if (r_u16(0x80104E46u + i * 54u) != 0u || r_u16(0x80104E52u + i * 54u) != 0u)
            spu_startup_todo("905CC sequencer guest voice callback");
        memset(&attr, 0, sizeof(attr));
        attr.voice = 1u << i;
        if ((dirty & 1u) != 0u)
        {
            attr.mask |= 3u;
            attr.volume.left = (sint16)r_u16(shadow);
            attr.volume.right = (sint16)r_u16(shadow + 2u);
        }
        if ((dirty & 4u) != 0u)
        {
            attr.mask |= 0x10u;
            attr.pitch = r_u16(shadow + 4u);
        }
        if ((dirty & 8u) != 0u)
        {
            attr.mask |= 0x80u;
            attr.addr = (uint32)r_u16(shadow + 6u) << 3;
        }
        if ((dirty & 0x10u) != 0u)
        {
            attr.mask |= 0x60000u;
            attr.adsr1 = r_u16(shadow + 8u);
            attr.adsr2 = r_u16(shadow + 10u);
        }
        if (attr.mask != 0u)
            SpuSetVoiceAttr(&attr);
        w_u8(0x80105338u + i, 0u);
    }
    SpuSetKey(0, r_u16(0x80105520u) | ((uint32)r_u8(0x80105522u) << 16));
    SpuSetKey(1, r_u16(0x80104E18u) | ((uint32)r_u8(0x80104E1Au) << 16));
    /* Mode 8 replaces the complete original voice masks */
    replace_voice_mask(0xCCu, r_u16(0x80104E1Cu) | ((uint32)r_u8(0x80104E1Eu) << 16), 0u);
    replace_voice_mask(0xCAu, r_u16(0x80104E20u) | ((uint32)r_u8(0x80104E22u) << 16), 0u);
    w_u16(0x80105520u, 0u);
    w_u16(0x80105522u, 0u);
    w_u16(0x80104E18u, 0u);
    w_u16(0x80104E1Au, 0u);
    w_u16(0x80104E20u, 0u);
    w_u16(0x80104E22u, 0u);
}

static void sequencer_init(uint32 requested_count)
{
    static const uint32 offsets[] = {4, 6, 8, 12, 18, 20, 30, 32, 34, 36, 38, 42, 44, 46, 48, 50};
    SpuMallocGuestBinding malloc_binding = {0x800FD264u, 0x800FD26Cu, 0x800FD29Cu, 0x800FD2A0u, 0x800FD2A4u, 0x800FD1D0u, 0x800FD1D8u};
    uint32 count = (uint32)(sint32)(sint8)requested_count < 24u ? (uint32)(sint32)(sint8)requested_count : 24u;
    uint32 i, j;
    w_u32(0x800FD270u, 1u);
    w_u16(0x801055C0u, 0u);
    if (!spu_bind_malloc_guest(&malloc_binding))
        spu_startup_todo("9532C malloc guest binding");
    spu_init_malloc_guest(32u, 0x801056E8u);
    for (i = 0; i < 192u; ++i)
        w_u16(0x80105358u + i * 2u, 0u);
    for (i = 0; i < 24u; ++i)
        w_u8(0x80105338u + i, 0u);
    w_u16(0x80105698u, 0u);
    for (i = 0; i < 16u; ++i)
        w_u8(0x80105640u + i, 0u);
    w_u8(0x8010561Cu, count);
    for (i = 0; i < count; ++i)
    {
        uint32 record = 0x80104E28u + i * 54u;
        uint32 bit = 1u << i;
        SpuVoiceAttr attr;
        w_u16(record, 255u);
        w_u16(record + 2u, 24u);
        for (j = 0; j < sizeof(offsets) / sizeof(offsets[0]); ++j)
            w_u16(record + offsets[j], 0u);
        w_u16(record + 16u, 0xFFFFu);
        w_u16(record + 22u, 255u);
        w_u8(record + 10u, 64u);
        w_u8(record + 29u, 0u);
        memset(&attr, 0, sizeof(attr));
        attr.voice = bit;
        attr.mask = 0x60093u;
        attr.pitch = 0x1000u;
        attr.addr = 0x1000u;
        attr.adsr1 = 0x80FFu;
        attr.adsr2 = 0x4000u;
        SpuSetVoiceAttr(&attr);
        w_u16(0x80105638u, i);
        w_u8(record + 29u, 0u);
        w_u16(record + 4u, 0u);
        w_u16(record, 0u);
        w_u16(0x80105520u, r_u16(0x80105520u) | bit);
        w_u16(0x80105522u, r_u16(0x80105522u) | (bit >> 16));
        w_u16(0x80104E18u, r_u16(0x80104E18u) & ~bit);
        w_u16(0x80104E1Au, r_u16(0x80104E1Au) & ~(bit >> 16));
    }
    w_u32(0x80105528u, 0u);
    w_u16(0x80105530u, 0x3FFFu);
    w_u16(0x80105532u, 0x3FFFu);
    w_u32(0x8010552Cu, 0u);
    w_u16(0x80104E18u, 0u);
    w_u16(0x80104E1Au, 0u);
    w_u16(0x80105520u, 0u);
    w_u16(0x80104E1Cu, 0u);
    w_u16(0x80104E1Eu, 0u);
    w_u16(0x80104E20u, 0u);
    w_u16(0x80104E22u, 0u);
    w_u8(0x80105650u, 0u);
    w_u16(0x80105608u, 0u);
    w_u16(0x8010560Au, 0x80u);
    apocalypse_ss_flush();
}

uint32 apocalypse_ss_init(void)
{
    uint32 voice, field;
    SsInit();
    for (voice = 0; voice < 24u; ++voice)
        for (field = 0; field < 8u; ++field)
            w_u16(0x1F801C00u + voice * 16u + field * 2u, r_u16(0x800FD028u + field * 2u));
    for (field = 0; field < 16u; ++field)
        w_u16(0x1F801D80u + field * 2u, r_u16(0x800FD038u + field * 2u));
    sequencer_init(24u);
    for (field = 0; field < 512u; ++field)
        w_u32(0x80104610u + field * 4u, 0u);
    w_u32(0x8010458Cu, 60u);
    w_u32(0x80104588u, 0u);
    w_u32(0x80104584u, 0u);
    return 60u;
}

uint32 apocalypse_ss_serial_attr(uint32 serial, uint32 attribute, uint32 value)
{
    sint32 channel = (sint8)serial, selector = (sint8)attribute;
    SpuCommonAttr attr;
    memset(&attr, 0, sizeof(attr));
    if ((uint32)channel > 1u || (uint32)selector > 1u)
        spu_startup_todo("8E93C unsupported serial or attribute");
    if (channel == 0)
    {
        attr.mask = selector == 0 ? 0x200u : 0x100u;
        if (selector == 0)
            attr.cd.mix = (sint8)value;
        else
            attr.cd.reverb = (sint8)value;
    }
    else
    {
        attr.mask = selector == 0 ? 0x2000u : 0x1000u;
        if (selector == 0)
            attr.ext.mix = (sint8)value;
        else
            attr.ext.reverb = (sint8)value;
    }
    SpuSetCommonAttr(&attr);
    return (attr.mask & 0x2000u) != 0u ? r_u16(0x1F801DAAu) : 0u;
}

uint32 apocalypse_ss_serial_volume(uint32 serial, uint32 left, uint32 right)
{
    sint32 channel = (sint8)serial;
    sint16 l = (sint16)left, r = (sint16)right;
    SpuCommonAttr attr;
    if ((uint32)channel > 1u)
        spu_startup_todo("8F05C unsupported serial");
    if (l >= 128)
        l = 127;
    if (r >= 128)
        r = 127;
    memset(&attr, 0, sizeof(attr));
    attr.mask = channel == 0 ? 0xC0u : 0xC00u;
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
    return 0u;
}

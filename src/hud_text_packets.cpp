#include "game_random.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "hud_text.h"
#include "native_object_storage.h"

static uint8 ReadStyledTextByte(const char *cursor)
{
    uint32 address = GuestMemoryAddress(cursor);
    return address ? r_u8(address) : (uint8)*cursor;
}

uint32 DrawStyledHudText(uint32 x, uint32 y, const char *text, uint32 filter, uint32 color, uint32 scale)
{
    sint32 v14;
    sint32 v15;
    sint32 v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    unsigned char v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    sint32 v31;
    sint32 v32;
    short v33;
    sint8 v34;
    sint8 v35;
    sint8 v36;
    sint8 v38;
    sint8 v39;
    sint32 v40;
    sint32 v41;
    sint8 v42;
    sint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    short v49;
    short v50;
    sint32 v51;
    sint32 v52;
    sint32 v53;
    sint32 v54;
    sint32 v55;
    sint32 v56;
    sint32 v57;
    short v58;
    sint32 v59;
    sint32 v60;
    short v61;
    sint32 v62;
    short v63;
    short v64;
    short v65;
    short v66;
    short v67;
    short v68;
    short v69;
    short v70;
    short v71;
    short v72;
    sint32 v73;
    sint32 v74;
    uint32 v75;
    sint32 v76;
    sint32 v77;
    uint32 v78;
    uint32 v79;
    sint32 v80;
    sint32 v81;
    sint32 v82;
    sint32 v83;
    sint32 v84;
    sint32 v85;
    short v86;
    short v87;
    short v88;
    short v89;
    short v90;
    short v91;
    short v92;
    sint8 v93;
    sint8 v94;
    short v95;
    sint32 v96;
    sint32 v97;
    sint32 v99;
    sint32 v100;
    const char *v101;
    const char *v102;
    sint32 v103;
    sint32 v104;
    sint32 v105;
    uint32 v106;
    sint32 v107;
    sint32 v109;
    sint32 v110;
    sint32 v111;
    sint32 v112;
    sint32 v114;
    const TextureFrame *glyph_frame = 0;
    const TextureResource *glyph_tex = 0;
    const TextureResource *glow = GetHudGlowTexture();
    uint32 glyph_width, glyph_offset, current_character, packet_end, packet_limit, texture_bottom, texture_top, texture_left, texture_right;
    uint32 coord_x, coord_y, coord_right, coord_bottom, packet_word, head_word, head_address;
    FUNCTION_MARKER(0x8001AA28, "SLUS_003.73");
    // Packet links retain the original ordered header and ordering-table accesses
    if (glow)
        v114 = ((color | (color << 16)) | (color << 8));
    else
        v114 = 0;
    v14 = 0;
    v99 = 0;
    v101 = text;
    v103 = 0;
    while ((current_character = ReadStyledTextByte(v101)) != 0)
    {
        v15 = GetHudGlyphIndex(current_character);
        v16 = (v15 < 0);
        v17 = (8 * v15);
        if (v16)
        {
            v18 = (sint32)((uint32)v103 + 10);
        }
        else
        {
            v14 = (sint32)((uint32)v14 + 1);
            glyph_frame = &GetTextureFrame(GetHudFont(), v15);
            glyph_width = glyph_frame->w;
            glyph_offset = (uint32)(sint32)glyph_frame->ox;
            v18 = (sint32)((uint32)v103 + glyph_width + glyph_offset);
        }
        v103 = v18;
        (v101 += 1);
    }

    v104 = (sint32)((uint32)((sint32)((uint32)v103 * r_u16(0x800FF1B8)) >> 8) + 3 * (uint32)v14);
    current_character = r_u8(0x800FF1B0);
    if (current_character == 0)
    {
        glyph_offset = 0 - (uint32)v104;
        v20 = (sint32)(glyph_offset + (glyph_offset >> 31)) >> 1;
    }
    else if (current_character == 1)
        v20 = 0;
    else if (current_character == 2)
        v20 = (sint32)(0 - (uint32)v104);
    else
    {
        // TODO Original invalid alignment uses an undefined incoming saved register
        v20 = (sint32)xport_draft_unknown_alignment_offset_8001AA28();
    }
    v19 = (sint32)(x + (uint32)v20);
    v105 = v19;

    v106 = y;
    v102 = text;
    v107 = ((9 * ((unsigned short)(r_u16(0x800FF1B8)))) >> 8);
    while (1)
    {
        v21 = ReadStyledTextByte(v102);
        if (!v21)
            return v104;
        (v102 += 1);
        v22 = GetHudGlyphIndex(v21);
        if ((v22 < 0))
        {
            v105 = (sint32)((uint32)v105 + ((5 * (uint32)r_u16(0x800FF1B8)) >> 7));
        }
        else
        {
            v23 = r_u32(0x800FF668);
            v24 = v23 + 40;
            v25 = v23 + 80;
            v26 = v23 + 120;
            glyph_frame = &GetTextureFrame(GetHudFont(), v22);
            packet_end = (uint32)v23 + 212;
            packet_limit = r_u32(0x800FF374);
            glyph_tex = glyph_frame->tex;
            v28 = v23 + 160;
            if (packet_limit < packet_end)
                return v104;
            w_u32(0x800FF668, packet_end);
            v29 = r_u32(0x800FF1B4);
            w_u32(((uint32)(v23)), 150994944);
            w_u32(((uint32)((v23 + 4))), v29);
            v30 = PackTextureUv(*glyph_tex, 0);
            v31 = PackTextureUv(*glyph_tex, 1);
            v32 = PackTextureUv(*glyph_tex, 2);
            v33 = glyph_tex->uv[3].uv;
            w_u32(((uint32)((v23 + 12))), v30);
            w_u32(((uint32)((v23 + 20))), v31);
            w_u32(((uint32)((v23 + 28))), v32);
            w_u16(((uint32)((v23 + 36))), v33);
            if (((((unsigned short)(r_u16(0x800FF1B8))) >= 0x200) || filter))
            {
                texture_left = r_u8((uint32)v23 + 20);
                texture_bottom = r_u8((uint32)v23 + 29);
                w_u8((uint32)v23 + 20, texture_left - 1);
                texture_right = r_u8((uint32)v23 + 36);
                w_u8((uint32)v23 + 29, texture_bottom - 1);
                texture_top = r_u8((uint32)v23 + 37);
                w_u8((uint32)v23 + 36, texture_right - 1);
                w_u8((uint32)v23 + 37, texture_top - 1);
            }
            if (v114)
            {
                w_u32(((uint32)((v26 + 4))), (v114 | 0x2E000000));
                w_u32(((uint32)(v26)), 150994944);
                w_u32(((uint32)((v26 + 12))), PackTextureUv(*glow, 0));
                w_u32(((uint32)((v26 + 20))), PackTextureUv(*glow, 1));
                texture_bottom = glow->uv[2].v;
                texture_top = glow->uv[0].v;
                texture_left = glow->uv[0].u;
                v38 = (sint8)texture_left;
                v39 = (sint8)(texture_bottom - (uint32)(((sint32)texture_bottom - (sint32)texture_top) >> 3));
                w_u8(((uint32)((v26 + 28))), texture_left);
                w_u8(((uint32)((v26 + 29))), v39);
                w_u32(((uint32)((v28 + 4))), (v114 | 0x3E000000));
                w_u32(((uint32)((v28 + 16))), v114);
                w_u32(((uint32)((v28 + 28))), 0);
                w_u32(((uint32)((v28 + 40))), 0);
                w_u32(((uint32)(v28)), 201326592);
                w_u16(((uint32)((v28 + 14))), glow->clut);
                v40 = glow->tpage;
                w_u8(((uint32)((v28 + 12))), v38);
                w_u8(((uint32)((v28 + 13))), v39);
                w_u16(((uint32)((v28 + 26))), v40);
                v42 = glow->uv[1].u;
                w_u8(((uint32)((v26 + 37))), v39);
                w_u8(((uint32)((v26 + 36))), v42);
                w_u8(((uint32)((v28 + 24))), v42);
                w_u8(((uint32)((v28 + 25))), v39);
                v43 = glow->uv[3].u;
                v44 = glow->uv[2].u;
                v45 = ((3 * (v43 - v44)) >> 4);
                v46 = ((v44 + v43) >> 1);
                w_u8(((uint32)((v28 + 36))), (v46 - v45));
                v41 = glow->uv[2].v;
                w_u8(((uint32)((v28 + 48))), (v46 + v45));
                w_u8(((uint32)((v28 + 37))), v41);
                w_u8(((uint32)((v28 + 49))), glow->uv[3].v);
                texture_top = r_u8((uint32)v28 + 37);
                texture_bottom = r_u8((uint32)v28 + 49);
                w_u8((uint32)v28 + 37, texture_top - 1);
                w_u8((uint32)v28 + 49, texture_bottom - 1);
            }
            v47 = r_u16(0x800FF1B8);
            glyph_offset = (uint32)(sint32)glyph_frame->ox;
            w_u16((uint32)v23 + 8, (uint32)v105 + (uint32)((sint32)(glyph_offset * (uint32)v47) >> 8));
            glyph_offset = (uint32)(sint32)glyph_frame->oy;
            w_u16((uint32)v23 + 10, v106 + (uint32)((sint32)(glyph_offset * (uint32)v47) >> 8) - (uint32)v107);
            glyph_width = glyph_frame->w;
            coord_y = r_u16((uint32)v23 + 10);
            coord_x = r_u16((uint32)v23 + 8);
            w_u16((uint32)v23 + 18, coord_y);
            w_u16((uint32)v23 + 24, coord_x);
            w_u16((uint32)v23 + 16, coord_x + ((glyph_width * (uint32)v47) >> 8));
            glyph_width = glyph_frame->h;
            coord_y = r_u16((uint32)v23 + 10);
            w_u16((uint32)v23 + 26, coord_y + ((glyph_width * (uint32)v47) >> 8));
            coord_right = r_u16((uint32)v23 + 16);
            coord_bottom = r_u16((uint32)v23 + 26);
            w_u16((uint32)v23 + 32, coord_right);
            w_u16((uint32)v23 + 34, coord_bottom);
            v50 = (sint16)coord_bottom;
            if (v114)
            {
                v51 = v99;
                v100 = v99 + 1;
                v55 = (sint16)r_u16((uint32)v23 + 8);
                v52 = (sint16)r_u16((uint32)v23 + 10);
                v54 = (sint16)r_u16((uint32)v23 + 18);
                packet_word = r_u32(0x800A593C + (uint32)v51 * 4);
                v56 = (sint16)r_u16((uint32)v23 + 32);
                v52 = (sint32)((uint32)v52 - packet_word);
                v53 = v100 < 41 ? v100 : 0;
                packet_word = r_u32(0x800A593C + (uint32)v53 * 4);
                v54 = (sint32)((uint32)v54 - packet_word);
                v99 = v53 + 1;
                if (v99 >= 41)
                    v99 = 0;
                v57 = (sint32)((uint32)v50 - (uint32)((sint32)(((uint32)v50 - (uint32)v52) * scale) >> 8));
                v54 = (sint32)(((uint32)v50 - (uint32)v54) * scale);
                v55 -= 10;
                v56 += 10;
                w_u16((uint32)v26 + 8, v55);
                w_u16((uint32)v26 + 16, v56);
                w_u16((uint32)v26 + 24, v55);
                w_u16((uint32)v26 + 32, v56);
                w_u16((uint32)v26 + 10, v57);
                w_u16((uint32)v26 + 26, (uint32)v50 - (uint32)((sint32)((uint32)v50 - (uint32)v57) >> 3));
                v57 = (sint32)((uint32)v50 - (uint32)(v54 >> 8));
                w_u16((uint32)v26 + 18, v57);
                w_u16((uint32)v26 + 34, (uint32)v50 - (uint32)((sint32)((uint32)v50 - (uint32)v57) >> 3));
                w_u16((uint32)v28 + 8, v55);
                packet_word = r_u16((uint32)v26 + 26);
                w_u16((uint32)v28 + 10, packet_word);
                packet_word = r_u16((uint32)v26 + 32);
                w_u16((uint32)v28 + 20, packet_word);
                v58 = r_u16((uint32)v26 + 34);
                w_u16((uint32)v28 + 34, (uint32)v50 + 2);
                w_u16((uint32)v28 + 46, (uint32)v50 + 2);
                v59 = (3 * (v56 - v55)) >> 4;
                v60 = (v55 + v56) >> 1;
                w_u16((uint32)v28 + 32, v60 - v59);
                w_u16((uint32)v28 + 44, v60 + v59);
                w_u16((uint32)v28 + 22, v58);
            }
            w_u32((uint32)v24, r_u32((uint32)v23));
            w_u32((uint32)v24 + 4, r_u32((uint32)v23 + 4));
            w_u32((uint32)v24 + 8, r_u32((uint32)v23 + 8));
            packet_word = r_u32((uint32)v23 + 12);
            coord_y = r_u16((uint32)v24 + 10);
            w_u32((uint32)v24 + 12, packet_word);
            w_u32((uint32)v24 + 16, r_u32((uint32)v23 + 16));
            w_u32((uint32)v24 + 20, r_u32((uint32)v23 + 20));
            w_u32((uint32)v24 + 24, r_u32((uint32)v23 + 24));
            w_u32((uint32)v24 + 28, r_u32((uint32)v23 + 28));
            w_u32((uint32)v24 + 32, r_u32((uint32)v23 + 32));
            packet_word = r_u32((uint32)v23 + 36);
            coord_x = r_u16((uint32)v24 + 8);
            w_u16((uint32)v24 + 10, coord_y + 1);
            coord_y = r_u16((uint32)v24 + 18);
            w_u8((uint32)v24 + 6, 0);
            w_u8((uint32)v24 + 5, 0);
            w_u8((uint32)v24 + 4, 0);
            w_u16((uint32)v24 + 8, coord_x + 1);
            coord_x = r_u16((uint32)v24 + 16);
            w_u16((uint32)v24 + 18, coord_y + 1);
            coord_y = r_u16((uint32)v24 + 26);
            w_u16((uint32)v24 + 16, coord_x + 1);
            coord_x = r_u16((uint32)v24 + 24);
            w_u16((uint32)v24 + 24, coord_x + 1);
            w_u32((uint32)v24 + 36, packet_word);
            w_u16((uint32)v24 + 26, coord_y + 1);
            coord_x = r_u16((uint32)v24 + 32);
            coord_y = r_u16((uint32)v24 + 34);
            w_u16((uint32)v24 + 32, coord_x + 1);
            w_u16((uint32)v24 + 34, coord_y + 1);
            w_u32((uint32)v25, r_u32((uint32)v23));
            w_u32((uint32)v25 + 4, r_u32((uint32)v23 + 4));
            w_u32((uint32)v25 + 8, r_u32((uint32)v23 + 8));
            packet_word = r_u32((uint32)v23 + 12);
            coord_y = r_u16((uint32)v25 + 10);
            w_u32((uint32)v25 + 12, packet_word);
            w_u32((uint32)v25 + 16, r_u32((uint32)v23 + 16));
            w_u32((uint32)v25 + 20, r_u32((uint32)v23 + 20));
            w_u32((uint32)v25 + 24, r_u32((uint32)v23 + 24));
            w_u32((uint32)v25 + 28, r_u32((uint32)v23 + 28));
            w_u32((uint32)v25 + 32, r_u32((uint32)v23 + 32));
            w_u32((uint32)v25 + 36, r_u32((uint32)v23 + 36));
            w_u8((uint32)v25 + 4, r_u8(0x800FF1BC));
            w_u8((uint32)v25 + 5, r_u8(0x800FF1C0));
            coord_x = r_u16((uint32)v25 + 8);
            packet_word = r_u8(0x800FF1C4);
            w_u16((uint32)v25 + 8, coord_x - 2);
            w_u8((uint32)v25 + 6, packet_word);
            w_u16((uint32)v25 + 10, coord_y - 1);
            coord_x = r_u16((uint32)v25 + 16);
            coord_right = r_u16((uint32)v25 + 24);
            w_u16((uint32)v25 + 16, coord_x - 2);
            coord_y = r_u16((uint32)v25 + 18);
            w_u16((uint32)v25 + 24, coord_right - 2);
            coord_right = r_u16((uint32)v25 + 32);
            w_u16((uint32)v25 + 18, coord_y - 1);
            coord_bottom = r_u16((uint32)v25 + 26);
            w_u16((uint32)v25 + 32, coord_right - 2);
            w_u16((uint32)v25 + 26, coord_bottom - 1);
            coord_y = r_u16((uint32)v25 + 34);
            v16 = r_u32(0x800FF1CC) != 0;
            w_u16((uint32)v25 + 34, coord_y - 1);
            if (v16)
                goto LABEL_39;
            if (v114)
            {
                packet_word = r_u16((uint32)v26 + 22);
                v73 = r_u16(0x800FF1BA);
                w_u16((uint32)v26 + 22, (packet_word & 0xFF9F) | 0x20);
                packet_word = r_u16((uint32)v28 + 26);
                w_u16((uint32)v28 + 26, (packet_word & 0xFF9F) | 0x20);
                head_address = r_u32(0x800FF660) + (uint32)v73 * 4 + 112;
                packet_word = r_u32((uint32)v26);
                head_word = r_u32(head_address);
                w_u32((uint32)v26, (packet_word & 0xFF000000) | (head_word & 0xFFFFFF));
                head_word = r_u32(head_address);
                head_word = (head_word & 0xFF000000) | ((uint32)v26 & 0xFFFFFF);
                w_u32(head_address, head_word);
                packet_word = r_u32((uint32)v28);
                w_u32((uint32)v28, (packet_word & 0xFF000000) | (head_word & 0xFFFFFF));
                head_word = r_u32(head_address);
                w_u32(head_address, (head_word & 0xFF000000) | ((uint32)v28 & 0xFFFFFF));
            }
            v73 = r_u16(0x800FF1BA);
            head_address = r_u32(0x800FF660) + (uint32)v73 * 4 + 112;
            packet_word = r_u32((uint32)v23);
            v76 = r_u32(0x800FF1CC);
            head_word = r_u32(head_address);
            w_u32((uint32)v23, (packet_word & 0xFF000000) | (head_word & 0xFFFFFF));
            head_word = r_u32(head_address);
            head_word = (head_word & 0xFF000000) | ((uint32)v23 & 0xFFFFFF);
            w_u32(head_address, head_word);
            packet_word = r_u32((uint32)v25);
            w_u32((uint32)v25, (packet_word & 0xFF000000) | (head_word & 0xFFFFFF));
            head_word = r_u32(head_address);
            head_word = (head_word & 0xFF000000) | ((uint32)v25 & 0xFFFFFF);
            w_u32(head_address, head_word);
            packet_word = r_u32((uint32)v24);
            w_u32((uint32)v24, (packet_word & 0xFF000000) | (head_word & 0xFFFFFF));
            head_word = r_u32(head_address);
            w_u32(head_address, (head_word & 0xFF000000) | ((uint32)v24 & 0xFFFFFF));
            if (v76)
            {
            LABEL_39:
                v80 = 0;

                coord_right = (uint32)(sint32)(sint16)r_u16((uint32)v23 + 16);
                coord_x = (uint32)(sint32)(sint16)r_u16((uint32)v23 + 8);
                texture_right = r_u8((uint32)v23 + 20);
                texture_left = r_u8((uint32)v23 + 12);
                v109 = (sint32)(coord_right - coord_x);
                v110 = (sint32)(texture_right - texture_left);
                coord_bottom = (uint32)(sint32)(sint16)r_u16((uint32)v23 + 26);
                coord_y = (uint32)(sint32)(sint16)r_u16((uint32)v23 + 10);
                texture_bottom = r_u8((uint32)v23 + 29);
                texture_top = r_u8((uint32)v23 + 13);
                v111 = (sint32)(coord_bottom - coord_y);
                v112 = (sint32)(texture_bottom - texture_top);
                while ((v80 < 3))
                {
                    v81 = RandomRange(32);
                    v82 = RandomRange(128);
                    v83 = ((v81 + RandomRange((192 - v81))) + 64);
                    v84 = ((v82 + RandomRange((192 - v82))) + 64);
                    v85 = r_u32(0x800FF668);
                    if (r_u32(0x800FF374) < (uint32)v85 + 40)
                        return v104;
                    w_u32(0x800FF668, (uint32)v85 + 40);
                    v87 = (RandomRange(6) - 3);
                    v86 = RandomRange(6);
                    w_u32((uint32)v85, r_u32((uint32)v23));
                    w_u32((uint32)v85 + 4, r_u32((uint32)v23 + 4));
                    v86 -= 3;
                    w_u16((uint32)v85 + 14, r_u16((uint32)v23 + 14));
                    w_u16((uint32)v85 + 22, r_u16((uint32)v23 + 22));
                    coord_x = r_u16((uint32)v23 + 8);
                    w_u16((uint32)v85 + 8, coord_x + (uint32)((sint32)((uint32)v81 * (uint32)v109) >> 8) + (uint32)v87);
                    coord_y = r_u16((uint32)v23 + 10);
                    coord_x = r_u16((uint32)v85 + 8);
                    w_u16((uint32)v85 + 10, coord_y + (uint32)((sint32)((uint32)v82 * (uint32)v111) >> 8) + (uint32)v86);
                    coord_right = r_u16((uint32)v23 + 8);
                    w_u16((uint32)v85 + 24, coord_x);
                    coord_y = r_u16((uint32)v85 + 10);
                    w_u16((uint32)v85 + 16, coord_right + (uint32)((sint32)((uint32)v83 * (uint32)v109) >> 8) + (uint32)v87);
                    w_u16((uint32)v85 + 18, coord_y);
                    coord_bottom = r_u16((uint32)v23 + 10);
                    coord_right = r_u16((uint32)v85 + 16);
                    coord_bottom += (uint32)((sint32)((uint32)v84 * (uint32)v111) >> 8) + (uint32)v86;
                    ++v80;
                    w_u16((uint32)v85 + 26, coord_bottom);
                    w_u16((uint32)v85 + 32, coord_right);
                    w_u16((uint32)v85 + 34, coord_bottom);
                    texture_left = r_u8((uint32)v23 + 12);
                    w_u8((uint32)v85 + 12, texture_left + (uint32)((sint32)((uint32)v81 * (uint32)v110) >> 8));
                    texture_top = r_u8((uint32)v23 + 13);
                    w_u8((uint32)v85 + 13, texture_top + (uint32)((sint32)((uint32)v82 * (uint32)v112) >> 8));
                    texture_left = r_u8((uint32)v23 + 12);
                    texture_right = r_u8((uint32)v85 + 12);
                    texture_top = r_u8((uint32)v85 + 13);
                    w_u8((uint32)v85 + 20, texture_left + (uint32)((sint32)((uint32)v83 * (uint32)v110) >> 8));
                    w_u8((uint32)v85 + 21, texture_top);
                    w_u8((uint32)v85 + 28, texture_right);
                    texture_top = r_u8((uint32)v23 + 13);
                    packet_word = r_u8((uint32)v85 + 7);
                    texture_right = r_u8((uint32)v85 + 20);
                    w_u8((uint32)v85 + 7, packet_word | 2);
                    w_u8((uint32)v85 + 29, texture_top + (uint32)((sint32)((uint32)v84 * (uint32)v112) >> 8));
                    packet_word = r_u16((uint32)v85 + 22);
                    texture_bottom = r_u8((uint32)v85 + 29);
                    w_u8((uint32)v85 + 36, texture_right);
                    w_u8((uint32)v85 + 37, texture_bottom);
                    v96 = r_u16(0x800FF1BA);
                    w_u16((uint32)v85 + 22, packet_word | 0x60);
                    head_address = r_u32(0x800FF660) + (uint32)v96 * 4 + 112;
                    packet_word = r_u32((uint32)v85);
                    head_word = r_u32(head_address);
                    w_u32((uint32)v85, (packet_word & 0xFF000000) | (head_word & 0xFFFFFF));
                    head_word = r_u32(head_address);
                    w_u32(head_address, (head_word & 0xFF000000) | ((uint32)v85 & 0xFFFFFF));
                }
            }
            glyph_width = glyph_frame->w;
            glyph_offset = (uint32)(sint32)glyph_frame->ox;
            current_character = r_u16(0x800FF1B8);
            v105 = (sint32)((uint32)v105 + 3 + (uint32)((sint32)((glyph_width + glyph_offset) * current_character) >> 8));
        }
    }
}

extern "C" uint32 sub_8001AA28(uint32 x, uint32 y, uint32 text, uint32 filter, uint32 color, uint32 scale)
{
    FUNCTION_MARKER(0x8001AA28, "SLUS_003.73");
    return DrawStyledHudText(x, y, (const char *)psx_addr(text, 1), filter, color, scale);
}

extern "C" uint32 apocalypse_draw_text(uint32 x, uint32 y, const char *text, uint32 filter, uint32 color, uint32 scale)
{
    return DrawStyledHudText(x, y, text, filter, color, scale);
}

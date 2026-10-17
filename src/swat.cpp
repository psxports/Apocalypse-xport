#include "model_resources.h"
#include "visual_object.h"
#include "object_damage.h"
#include "object_collision_events.h"
#include "model_animation.h"
#include "game_sdk_bindings.h"
#include "vector_arithmetic.h"
#include "projectile_launch.h"
#include "projectile_guided.h"
#include "flamethrower.h"
#include "effect_lifecycle.h"
#include "model_attachment.h"
#include "audio_session.h"
#include "audio_voice.h"
#include "projectile_oscillating.h"
#include "weapon_nodes.h"
#include "machine_gun.h"
#include "spiral_ray_weapon.h"
#include "vector_distance.h"
#include "angle_math.h"
#include "player.h"
#include "game_random.h"
#include "game_object_allocation.h"
#include "particle_debris.h"
#include "particle_settings.h"
extern "C" {
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "swat.h"
#include "object_parameters.h"
#include "record_data.h"
#include "turret.h"
#include "fireball_effects.h"
#include "panel_particle.h"
#include "native_object_storage.h"
#include <new>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



Swat::Swat(const uint8 *record_data, uint32 index) : Enemy()
{
    const uint32 data = GuestMemoryAddress(record_data);
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004E35C, "SLUS_003.73");
    sint32 decoded;
    if (!data)
    {
        // TODO Support native record data in the original parser adapter
        GameDiagnostic("Swat record data is outside guest memory\n");
        abort();
    }
    w_u32(((uint32)((object + 68))), 0x800A2A68);
    decoded = sub_80062B0C(object, data);
    w_u32(((uint32)((object + 400))), sub_80062B50(object, decoded));
    w_u8(((uint32)((object + 380))), 1);
    this->record_idx = index;
    Init();
    sub_8004BE90(object, r_u32(((uint32)((object + 400)))));
}

Swat::Swat(const uint32 *position_vector, uint32 script) : Enemy()
{
    const uint32 position = GuestMemoryAddress(position_vector);
    const uint32 *native_position = position ? 0 : position_vector;
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004E3E4, "SLUS_003.73");
    uint32 x, y, z, cursor;
    w_u32(object + 68, 0x800A2A68);
    if (native_position)
    {
        x = native_position[0];
        y = native_position[1];
        z = native_position[2];
    }
    else
    {
        x = r_u32(position);
        y = r_u32(position + 4);
        z = r_u32(position + 8);
    }
    pos_history.saved[0] = x;
    pos_history.saved[1] = y;
    pos_history.saved[2] = z;
    x = pos_history.saved[0];
    y = pos_history.saved[1];
    z = pos_history.saved[2];
    this->model.pos[0] = x;
    this->model.pos[1] = y;
    this->model.pos[2] = z;
    w_u32(object + 400, script);
    Init();
    cursor = r_u32(object + 400);
    if (cursor)
    {
        w_u8(object + 380, 1);
        sub_8004BE90(object, cursor);
    }
}


uint32 Swat::Init()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004E48C, "SLUS_003.73");
    sint32 v2;
    sint32 result;
    w_u32(0x800FF308, r_u32(0x800FF308) + 1);
    motion.drag[0] = 31;
    motion.drag[2] = 31;
    motion.drag[1] = 3;
    AddToList(visual_lists.actors);
    v2 = r_u32(0x800FF4DC);
    result = 200;
    this->kind = 200;
    w_u16(((uint32)(((uint32)(object) + (uint32)(472)))), 0);
    w_u16(((uint32)(((uint32)(object) + (uint32)(474)))), 0);
    w_u32(0x800FF4DC, ((uint32)(v2) + (uint32)(1)));
    w_u8(((uint32)(((uint32)(object) + (uint32)(381)))), 0);
    return result;
}

uint32 Swat::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004E508, "SLUS_003.73");
    uint32 child, kind, count;
    w_u32(object + 68, 0x800A2A68);
    RemoveFromList(visual_lists.actors);
    child = r_u32(object + 608);
    if (child)
    {
        uint32 table = r_u32(child + 68);
        uint32 receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        uint32 target = r_u32(table + 12);
        (void)apocalypse_object_cleanup(target, receiver, 3);
    }
    child = r_u32(object + 620);
    if (child)
    {
        uint32 table = r_u32(child);
        uint32 receiver = child + (uint32)(sint32)(sint16)r_u16(table + 8);
        uint32 target = r_u32(table + 12);
        (void)apocalypse_object_cleanup(target, receiver, 3);
    }
    Enemy::FreeBehaviorTable();
    count = r_u32(0x800FF4DC);
    kind = r_u16(object + 390);
    w_u32(0x800FF4DC, count - 1);
    if (kind == 128)
        w_u32(0x800FF4E4, r_u32(0x800FF4E4) - 1);
    if (r_u16(object + 390) == 256)
    {
        child = r_u32(object + 480);
        if (child)
        {
            StopSoundVoices(child);
            w_u32(object + 480, 0);
        }
    }
    Enemy::Destroy(0);
    if (reason & 1)
        return ReleaseObjectStorage(object);
    return reason & 1;
}

uint32 Swat::Update()
{
    SVECTOR motion_cur, motion_target;
    uint32 motion_cur_word, motion_cur_tail, motion_target_word, motion_target_tail;
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004E71C, "SLUS_003.73");
    sint32 v2;
    sint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    short v9;
    sint8 v10;
    sint32 v11;
    sint32 v12;
    short v13;
    sint32 v14;
    sint32 v15;
    const ModelAttachmentTable *attachments;
    uint32 v18;
    unsigned char v19;
    sint32 v20;
    sint32 v21;
    short v22;
    sint32 v23;
    sint32 v24;
    sint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    short v31;
    unsigned short v32;
    short v33;
    unsigned short v34;
    sint32 v35;
    sint32 v36;
    sint32 v37;
    sint32 v38;
    sint32 v39;
    sint32 v40;
    sint8 v41;
    sint32 v43;
    sint32 v44;
    uint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    sint32 v52;
    sint32 v53;
    sint32 v54;
    sint32 v55;
    unsigned short v56;
    sint32 v57;
    short v58;
    sint32 v59;
    sint32 v60;
    short v61;
    unsigned short v62;
    sint32 v63;
    sint32 v64;
    sint32 v65;
    sint32 v66;
    uint32 v67;
    sint32 v68;
    uint32 v69;
    sint32 v70;
    uint32 v71;
    sint32 v72;
    sint32 v73;
    sint32 v74;
    sint32 v75;
    sint32 v76;
    short v77;
    sint32 result;
    sint32 v79;
    sint32 v80;
    sint32 v82;
    short v83;
    short v84;
    sint32 v85;
    uint32 attack_vector[3];
    uint32 attachment_position[3];
    sint16 attachment_descriptor[4];
    unsigned short v86;
    sint32 v87;
    unsigned short v88;
    uint32 first, second, third, fourth, flags, player, table, target;
    sint32 adjustment;
    uint32 v89[4];
    uint32 v90[4];
    first = r_u32(0x800A71CC);
    v2 = r_u32(0x800A71D0);
    v3 = r_u32(0x800A71D4);
    motion.acceleration[0] = first;
    motion.acceleration[1] = v2;
    motion.acceleration[2] = v3;
    v4 = r_u32(((uint32)(((uint32)(object) + (uint32)(620)))));
    v5 = ((uint32)(object) + (uint32)(4));
    if (v4)
    {
        table = r_u32((uint32)v4);
        adjustment = (sint16)r_u16(table + 24);
        target = r_u32(table + 28);
        if (target == 0x800243DC)
            sub_800243DC((uint32)v4 + (uint32)adjustment);
        else if (target == 0x800284A4)
            sub_800284A4((uint32)v4 + (uint32)adjustment);
        else
        {
            /* TODO Resolve any other behavior callback from its original table */
            MissingBehavior(target, (uint32)v4 + (uint32)adjustment, (uint32)v5, 0x800FF5E4, 0);
        }
    }
    if ((r_u16(((uint32)(((uint32)(object) + (uint32)(390))))) == 128))
    {
        v6 = RandomRange(240);
        v7 = 98;
        if (!v6 || (v8 = RandomRange(240), v7 = 99, !v8))
            sub_80069EF4(v7, ((uint32)(object) + (uint32)(4)), 0);
    }
    v9 = this->activation_flags;
    w_u16(((uint32)(((uint32)(object) + (uint32)(388)))), v9);
    if (((v9 & 1) != 0))
    {
        v10 = r_u8(((uint32)(((uint32)(object) + (uint32)(384)))));
        this->activation_flags = (v9 & 0xFFFE);
        w_u8(((uint32)(((uint32)(object) + (uint32)(384)))), ((uint32)(v10) + (uint32)(1)));
    }
    if (((this->activation_flags & 4) != 0))
        ApplyExplosionDamage();
    if (r_u8(((uint32)(((uint32)(object) + (uint32)(380))))))
    {
        v11 = r_u16(((uint32)(((uint32)(object) + (uint32)(476)))));
        v12 = (v11 == 0);
        v13 = ((uint32)(v11) - (uint32)(1));
        if (v12)
            sub_8004BE90(object, r_u32(((uint32)(((uint32)(object) + (uint32)(400))))));
        else
            w_u16(((uint32)(((uint32)(object) + (uint32)(476)))), v13);
    }
    else
    {
        UpdateState();
    }
    flags = r_u32(object + 396);
    v14 = r_u8(object + 381);
    w_u32(object + 396, flags & ~0x400);
    if (!v14)
    {
        v15 = r_u16(object + 390);
        if ((v15 == 8))
        {
            attachments = &SwatMode8Attachments;
        }
        else
        {
            if (((uint32)v15 < 9))
            {
                if ((v15 != 2))
                {
                    if (((uint32)v15 >= 3))
                    {
                        if ((v15 != 4))
                        {
                            w_u8(((uint32)(((uint32)(object) + (uint32)(381)))), 1);
                            goto LABEL_40;
                        }
                        attachments = &SwatMode4Attachments;
                    }
                    else
                    {
                        if ((v15 != 1))
                        {
                            w_u8(((uint32)(((uint32)(object) + (uint32)(381)))), 1);
                            goto LABEL_40;
                        }
                        attachments = &SwatMode1Attachments;
                    }
                    goto LABEL_39;
                }
                goto LABEL_38;
            }
            if ((v15 != 64))
            {
                if (((uint32)v15 < 0x41))
                {
                    if ((v15 != 16))
                    {
                        w_u8(((uint32)(((uint32)(object) + (uint32)(381)))), 1);
                        goto LABEL_40;
                    }
                    attachments = &SwatMode16Attachments;
                    goto LABEL_39;
                }
                if ((v15 != 128))
                {
                    if ((v15 != 256))
                    {
                        w_u8(((uint32)(((uint32)(object) + (uint32)(381)))), 1);
                        goto LABEL_40;
                    }
                    attachments = &SwatMode256Attachments;
                    goto LABEL_39;
                }
            LABEL_38:

                attachments = &SwatMode2Attachments;
                goto LABEL_39;
            }
            attachments = &SwatMode64Attachments;
        }
    LABEL_39:
        SetModelBoneTable(this->model.bank, *attachments);

        w_u8(((uint32)(((uint32)(object) + (uint32)(381)))), 1);
    }
LABEL_40:
    if (r_u8(((uint32)(((uint32)(object) + (uint32)(382))))))
        AddVectors(motion.acceleration, (const uint32 *)psx_addr(0x800A6844, 12));

    sub_8004B9A4(object);
    if (((r_u32(((uint32)(((uint32)(object) + (uint32)(396))))) & 2) != 0))
    {
        first = r_u8(object + 387);
        v18 = r_u8(object + 386);
        v19 = first + 1;
        w_u8(((uint32)(((uint32)(object) + (uint32)(387)))), v19);
        if ((v19 >= v18))
        {
            v20 = ((sint16)(r_u16(((uint32)(((uint32)(object) + (uint32)(456)))))));
            w_u8(((uint32)(((uint32)(object) + (uint32)(387)))), 0);
            v21 = sub_80067A18(((uint32)(object) + (uint32)(4)), v20, ((uint32)(4) * (uint32)(v20)));
            if ((v21 != -1))
            {
                first = (uint32)(sint32)(sint16)r_u16(object + 456);
                v22 = r_u16(object + 216) | 2;
                this->model.pos[1] = (uint32)v21 - (first << 12);
                w_u16(((uint32)(((uint32)(object) + (uint32)(216)))), v22);
            }
        }
    }
    sub_8007FDF0(object);
    first = this->model.pos[0];
    v25 = (uint32)(sint32)(sint16)r_u16(object + 456) << 12;
    v23 = this->model.pos[2];
    v24 = r_u32(object + 396);
    w_u32(object + 184, first);
    v26 = this->model.pos[1];
    w_u32(object + 192, v23);
    w_u32(object + 188, (uint32)v26 + (uint32)v25);
    if (((v24 & 0x80) != 0))
    {
        v27 = r_u32(((uint32)(((uint32)(object) + (uint32)(508)))));
        if ((r_u16(((uint32)(((uint32)(v27) + (uint32)(58))))) == 50))
        {
            v28 = r_u32(((uint32)(((uint32)(v27) + (uint32)(184)))));
            v29 = r_u32(((uint32)(((uint32)(v27) + (uint32)(188)))));
            v30 = r_u32(((uint32)(((uint32)(v27) + (uint32)(192)))));
        }
        else
        {
            v28 = r_u32(((uint32)(((uint32)(v27) + (uint32)(4)))));
            v29 = r_u32(((uint32)(((uint32)(v27) + (uint32)(8)))));
            v30 = r_u32(((uint32)(((uint32)(v27) + (uint32)(12)))));
        }
        w_u32(((uint32)(((uint32)(object) + (uint32)(496)))), v28);
        w_u32(((uint32)(((uint32)(object) + (uint32)(500)))), v29);
        w_u32(((uint32)(((uint32)(object) + (uint32)(504)))), v30);
        first = r_u32(object + 504);
        second = this->model.pos[2];
        third = r_u32(object + 496);
        fourth = this->model.pos[0];
        v31 = ratan2(first - second, third - fourth);
        v82 = 0;
        v82 = ((v82 & 0x0000FFFF) | ((((((uint32)(3072) - (uint32)(v31)) & 0xFFF)) & 0xFFFF) << 16));
        v83 = 0;
        ((uint32)this->model.angles[0] | ((uint32)this->model.angles[1] << 16));
        v85 = ((uint32)this->model.angles[0] | ((uint32)this->model.angles[1] << 16));
        v32 = this->model.angles[2];
        v86 = v32;
        motion_cur_word = (((unsigned short)(v85)) | ((uint32)(((v85 >> 16) & 65535)) << (uint32)(16)));
        motion_cur_tail = v32;
        motion_target_word = v82;
        motion_target_tail = v83;
        motion_cur.vx = (sint16)motion_cur_word;
        motion_cur.vy = (sint16)(motion_cur_word >> 16);
        motion_cur.vz = (sint16)motion_cur_tail;
        motion_cur.pad = (sint16)(motion_cur_tail >> 16);
        motion_target.vx = (sint16)motion_target_word;
        motion_target.vy = (sint16)(motion_target_word >> 16);
        motion_target.vz = (sint16)motion_target_tail;
        motion_target.pad = (sint16)(motion_target_tail >> 16);
        UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, 32);
    }
    if (((r_u32(((uint32)(((uint32)(object) + (uint32)(396))))) & 1) != 0))
    {
        first = r_u32(object + 492);
        second = this->model.pos[2];
        third = r_u32(object + 484);
        fourth = this->model.pos[0];
        v33 = ratan2(first - second, third - fourth);
        v82 = 0;
        v82 = ((v82 & 0x0000FFFF) | ((((((uint32)(3072) - (uint32)(v33)) & 0xFFF)) & 0xFFFF) << 16));
        v83 = 0;
        ((uint32)this->model.angles[0] | ((uint32)this->model.angles[1] << 16));
        v87 = ((uint32)this->model.angles[0] | ((uint32)this->model.angles[1] << 16));
        v34 = this->model.angles[2];
        v88 = v34;
        motion_cur_word = (((unsigned short)(v87)) | ((uint32)(((v87 >> 16) & 65535)) << (uint32)(16)));
        motion_cur_tail = v34;
        motion_target_word = v82;
        motion_target_tail = v83;
        motion_cur.vx = (sint16)motion_cur_word;
        motion_cur.vy = (sint16)(motion_cur_word >> 16);
        motion_cur.vz = (sint16)motion_cur_tail;
        motion_cur.pad = (sint16)(motion_cur_tail >> 16);
        motion_target.vx = (sint16)motion_target_word;
        motion_target.vy = (sint16)(motion_target_word >> 16);
        motion_target.vz = (sint16)motion_target_tail;
        motion_target.pad = (sint16)(motion_target_tail >> 16);
        UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, 32);
        v35 = r_u32(((uint32)(((uint32)(object) + (uint32)(396)))));
        if (((v35 & 8) != 0))
        {
            if ((((sint32)(((sint8)(this->model.frame)))) >= 3))
            {
                if (r_u8(((uint32)(((uint32)(object) + (uint32)(612))))))
                    w_u8(((uint32)(((uint32)(object) + (uint32)(382)))), 1);
            }
            else
            {
                w_u32(((uint32)(((uint32)(object) + (uint32)(304)))), 0x10000);
                w_u8(((uint32)(((uint32)(object) + (uint32)(382)))), 0);
                motion.velocity[2] = 0;
                motion.velocity[1] = 0;
                motion.velocity[0] = 0;
                motion.acceleration[2] = 0;
                motion.acceleration[1] = 0;
                motion.acceleration[0] = 0;
            }
            if (((((sint32)(((sint8)(this->model.frame)))) < 3) || ((r_u32(((uint32)(((uint32)(object) + (uint32)(396))))) & 0x20) != 0)))
            {
                if ((((sint32)(((sint8)(this->model.frame)))) >= 22))
                {
                    if (!(r_u8(((uint32)(((uint32)(object) + (uint32)(612)))))))
                    {
                    LABEL_68:
                        if (!(r_u8(((uint32)(((uint32)(object) + (uint32)(303)))))))
                            goto LABEL_83;

                        v45 = (r_u32(((uint32)(((uint32)(object) + (uint32)(396))))) & 0xFFFFFFD6);
                        goto LABEL_82;
                    }
                    w_u32(((uint32)(((uint32)(object) + (uint32)(304)))), 0);
                }
            }
            else
            {
                first = r_u32(object + 484);
                second = this->model.pos[0];
                third = r_u8(object + 612);
                v36 = Divide((sint32)(first - second), third);
                first = r_u32(object + 492);
                second = this->model.pos[2];
                third = r_u8(object + 612);
                v37 = Divide((sint32)(first - second), third);
                first = r_u32(object + 488);
                second = this->model.pos[1];
                v39 = first - second;
                v38 = r_u8(object + 612);
                motion.drag[0] = 31;
                w_u32(((uint32)(((uint32)(object) + (uint32)(396)))), (r_u32(((uint32)(((uint32)(object) + (uint32)(396))))) | (0x20)));
                motion.velocity[0] = v36;
                motion.velocity[2] = v37;
                motion.velocity[1] = (uint32)Divide(v39, (uint32)v38) - ((uint32)v38 << 14);
                motion.drag[1] = 31;
                motion.drag[2] = 31;
            }
            v40 = r_u8(((uint32)(((uint32)(object) + (uint32)(612)))));
            v12 = (v40 == 0);
            v41 = ((uint32)(v40) - (uint32)(1));
            if (!v12)
            {
                w_u8(((uint32)(((uint32)(object) + (uint32)(612)))), v41);
                v12 = (v41 == 0);
                if (!v12 && (sint32)motion.velocity[1] > 0)
                {
                    first = this->model.pos[1];
                    second = r_u32(object + 488);
                    v12 = (sint32)second < (sint32)first;
                }
                if (v12)
                {
                    w_u8(((uint32)(((uint32)(object) + (uint32)(612)))), 0);
                    first = r_u32(object + 484);
                    v43 = r_u32(((uint32)(((uint32)(object) + (uint32)(488)))));
                    v44 = r_u32(((uint32)(((uint32)(object) + (uint32)(492)))));
                    this->model.pos[0] = first;
                    this->model.pos[1] = v43;
                    this->model.pos[2] = v44;
                    motion.velocity[2] = 0;
                    motion.velocity[1] = 0;
                    motion.velocity[0] = 0;
                    motion.acceleration[2] = 0;
                    motion.acceleration[1] = 0;
                    w_u32(((uint32)(((uint32)(object) + (uint32)(304)))), 0x10000);
                    motion.acceleration[0] = 0;
                    w_u8(((uint32)(((uint32)(object) + (uint32)(382)))), 0);
                    motion.drag[0] = 31;
                    motion.drag[1] = 3;
                    motion.drag[2] = 31;
                }
            }
            goto LABEL_68;
        }
        if (((v35 & 0x10) != 0))
        {
            first = this->model.pos[1];
            v46 = r_u32(object + 488);
            if (v46 < (sint32)first)
            {
                this->model.pos[1] = v46;
                w_u8(((uint32)(((uint32)(object) + (uint32)(382)))), 0);
                motion.velocity[2] = 0;
                motion.velocity[1] = 0;
                v47 = ((sint8)(this->model.frame));
                motion.velocity[0] = 0;
                SetAnimationRange(4, v47, -1);
            }
            if (r_u8(((uint32)(((uint32)(object) + (uint32)(303))))))
            {
                v45 = (r_u32(((uint32)(((uint32)(object) + (uint32)(396))))) & 0xFFFFFFEE);
            LABEL_82:
                w_u32(((uint32)(((uint32)(object) + (uint32)(396)))), v45);
            }
        }
        else
        {
            first = r_u32(object + 484);
            second = this->model.pos[0];
            v89[0] = (sint32)(first - second) >> 12;
            first = r_u32(object + 488);
            second = this->model.pos[1];
            v89[1] = (sint32)(first - second) >> 12;
            first = r_u32(object + 492);
            second = this->model.pos[2];
            v89[2] = (sint32)(first - second) >> 12;
            NormalizeShortVector(v89, v90);
            v48 = (sint16)motion.angular_velocity[1];
            if (v48 < 0)
                v48 = -v48;
            if ((((sint32)(v48)) >= 65))
                v48 = 64;
            v49 = (((uint32)(r_u16(((uint32)(((uint32)(object) + (uint32)(436)))))) * (uint32)(((uint32)(64) - (uint32)(v48)))) / 64);
            v50 = ((sint32)(((uint32)(v90[1]) * (uint32)(v49))));
            v51 = ((sint32)(((uint32)(v90[2]) * (uint32)(v49))));
            motion.velocity[0] = ((sint32)(((uint32)(v90[0]) * (uint32)(v49))));
            motion.velocity[1] = v50;
            motion.velocity[2] = v51;
            v52 = ((uint32)(object) + (uint32)(4));
            if (((r_u32(((uint32)(((uint32)(object) + (uint32)(396))))) & 2) != 0))
                v53 = (((uint32)(sub_80066918(v52, ((uint32)(object) + (uint32)(484))))) < 0x40);
            else
                v53 = (((uint32)(sub_8006689C(v52, ((uint32)(object) + (uint32)(484))))) < 0x40);
            if (v53)
            {
                v45 = (r_u32(((uint32)(((uint32)(object) + (uint32)(396))))) & 0xFFFFFFFE);
                goto LABEL_82;
            }
        }
    }
LABEL_83:
    v54 = this->model.animation;

    if ((v54 == 3))
    {
        v55 = DidAnimationCrossFrame(3, r_u8(((uint32)(((uint32)(object) + (uint32)(613))))));
        v12 = 0;
        if (sub_80062D24(object, v55, 1))
        {
            player = visual_lists.players;
            if (player && sub_80066918(object + 4, player + 4) < 256)
            {
                first = (uint32)(sint32)(sint16)r_u16(object + 456) << 12;
                second = this->model.pos[1];
                player = visual_lists.players;
                third = (uint32)r_u8(player + 582) << 12;
                fourth = r_u32(player + 8);
                v12 = Abs32((sint32)(fourth + third - second - first)) <= 0x7FFFF;
            }
        }
        if (v12)
        {
            v56 = sub_8004BDFC(object, 0, 1);
            v57 = sub_8004B914(object, v56);
            xport_draft_host_sub_8006C3AC_p1(attack_vector, ((uint32)(visual_lists.players) + (uint32)(4)), ((uint32)(object) + (uint32)(4)));
            {
                player = visual_lists.players;
                ApplyObjectDamage(player, (uint32)v57, (const sint32 *)attack_vector, 0);
            }
        }
        v54 = this->model.animation;
    }
    if ((v54 == 1))
    {
        v58 = r_u16(((uint32)(((uint32)(object) + (uint32)(390)))));
        if (((v58 & 0x10) != 0))
        {
            v59 = DidAnimationCrossFrame(1, 10);
            if ((sub_80062D24(object, v59, 2) && ((sint32)sub_80066918(((uint32)(object) + (uint32)(4)), ((uint32)(visual_lists.players) + (uint32)(4))) < 4500)))
                goto LABEL_101;
        }
        else if (((v58 & 8) != 0))
        {
            v60 = DidAnimationCrossFrame(1, 12);
            if (sub_80062D24(object, v60, 2))
            LABEL_101:
                Fire();
        }
        else if (((v58 & 0x40) != 0))
        {
            player = visual_lists.players;
            second = this->model.pos[2];
            fourth = this->model.pos[0];
            first = r_u32(player + 12);
            third = r_u32(player + 4);
            v61 = ratan2(first - second, third - fourth);
            v82 = 0;
            v82 = ((v82 & 0x0000FFFF) | ((((((uint32)(3072) - (uint32)(v61)) & 0xFFF)) & 0xFFFF) << 16));
            v83 = 0;
            ((uint32)this->model.angles[0] | ((uint32)this->model.angles[1] << 16));
            v85 = ((uint32)this->model.angles[0] | ((uint32)this->model.angles[1] << 16));
            v62 = this->model.angles[2];
            v86 = v62;
            motion_cur_word = (((unsigned short)(v85)) | ((uint32)(((v85 >> 16) & 65535)) << (uint32)(16)));
            motion_cur_tail = v62;
            motion_target_word = v82;
            motion_target_tail = v83;
            motion_cur.vx = (sint16)motion_cur_word;
            motion_cur.vy = (sint16)(motion_cur_word >> 16);
            motion_cur.vz = (sint16)motion_cur_tail;
            motion_cur.pad = (sint16)(motion_cur_tail >> 16);
            motion_target.vx = (sint16)motion_target_word;
            motion_target.vy = (sint16)(motion_target_word >> 16);
            motion_target.vz = (sint16)motion_target_tail;
            motion_target.pad = (sint16)(motion_target_tail >> 16);
            UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, 32);
            v63 = this->model.frame;
            if (((((uint32)(((uint32)(v63) - (uint32)(21)))) < 0x13) && ((v63 & 3) == 0)))
                goto LABEL_101;
        }
    }
    if ((r_u16(((uint32)(((uint32)(object) + (uint32)(390))))) == 128))
    {
        v64 = visual_lists.players;
        w_u32(((uint32)(((uint32)(object) + (uint32)(176)))), 100687952);
        w_u16(((uint32)(((uint32)(object) + (uint32)(180)))), 10240);
        w_u16(((uint32)(((uint32)(object) + (uint32)(182)))), 10);
        if ((((((uint32)(sub_80066918(((uint32)(object) + (uint32)(4)), ((uint32)(v64) + (uint32)(4))))) < 0xA0) && (r_u16(((uint32)(((uint32)(object) + (uint32)(472))))) != 16)) && visual_lists.players))
        {
            v65 = sub_8004B914(object, 1);
            {
                player = visual_lists.players;
                ApplyObjectDamage(player, (uint32)v65, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
            }
        }
    }
    if ((((r_u32(((uint32)(((uint32)(object) + (uint32)(396))))) & 0x100) != 0) && !RandomRange(4)))
    {
        w_u32(0x800FF3AC, 0);
        v66 = RandomRange(3);
        v67 = ((uint32)(v66) + (uint32)(2));
        if ((v66 != -2))
        {
            do
            {
                v68 = sub_80032DC0(116);
                if (v68)
                {
                    void *dram_addr = AllocateNativeObjectStorage((uint32)v68, sizeof(PanelParticle));
                    new (dram_addr) PanelParticle((const uint32 *)this->model.pos);
                }
                --v67;
            } while (v67);
        }
        w_u32(0x800FF3AC, 1);
    }
    if (((r_u32(((uint32)(((uint32)(object) + (uint32)(396))))) & 4) != 0))
    {
        if (!(r_u32(((uint32)(((uint32)(object) + (uint32)(608)))))))
        {
            v69 = ((uint32)(sub_80032DC0(176)));
            if (v69)
                v69 = sub_80034C0C(v69, ((uint32)(((uint32)(object) + (uint32)(4)))), 64, 32, 64, 112, 96, 0, 48, 0x25, 0);
            w_u32(((uint32)(((uint32)(object) + (uint32)(608)))), v69);
            w_u8((((uint32)(v69)) + (66) * 1), 1);
            sub_80032EE4(r_u32(((uint32)(((uint32)(object) + (uint32)(608))))), ((uint32)(((uint32)(object) + (uint32)(4)))));
            w_u32(((uint32)(((uint32)(object) + (uint32)(396)))), (r_u32(((uint32)(((uint32)(object) + (uint32)(396))))) | (0x800)));
        }
        if (((r_u32(0x800FF2F0) & 1) != 0))
        {
            v70 = r_u16(((uint32)(((uint32)(object) + (uint32)(390)))));
            if ((v70 == 4))
            {
                v71 = 0x800FF500;
                v84 = ((unsigned char)(r_u8(0x800FF501)));
            }
            else if (((uint32)v70 >= 5))
            {
                if ((v70 == 16))
                {
                    v71 = 0x800FF518;
                    v84 = ((unsigned char)(r_u8(0x800FF519)));
                }
                else if (((uint32)v70 >= 0x11))
                {
                    if ((v70 != 256))
                    {
                        v84 = 0;
                        goto LABEL_137;
                    }
                    v71 = 0x800FF520;
                    v84 = ((unsigned char)(r_u8(0x800FF521)));
                }
                else
                {
                    if ((v70 != 8))
                    {
                        v84 = 0;
                        goto LABEL_137;
                    }
                    v71 = 0x800FF510;
                    v84 = ((unsigned char)(r_u8(0x800FF511)));
                }
            }
            else if ((v70 == 1))
            {
                v71 = 0x800FF4F8;
                v84 = ((unsigned char)(r_u8(0x800FF4F9)));
            }
            else
            {
                if ((v70 != 2))
                {
                    v84 = 0;
                LABEL_137:
                    v71 = 0;

                    goto LABEL_138;
                }
                v71 = 0x800FF508;
                v84 = ((unsigned char)(r_u8(0x800FF509)));
            }
        LABEL_138:
            if (v71)
            {
                v72 = RandomRange(((unsigned char)(((sint8)(r_u8(v71))))));
                w_u32(0x800FF3AC, 0);
                v73 = v72;
                v74 = sub_80032DC0(132);
                if (v74)
                {
                    void *dram_addr = AllocateNativeObjectStorage((uint32)v74, sizeof(Fireball));
                    new (dram_addr) Fireball(object, (sint16)r_u8(v71 + (uint32)v73 + 1));
                }
                w_u32(0x800FF3AC, 1);
            }

            if (r_u32(((uint32)(((uint32)(object) + (uint32)(608))))))
            {
                attachment_descriptor[0] = attachment_descriptor[1] = attachment_descriptor[2] = 0;
                attachment_descriptor[3] = v84;
                xport_draft_host_sub_8007CD74_p13(attachment_position, object, attachment_descriptor);
                xport_draft_host_sub_80032EE4_p2(r_u32(object + 608), attachment_position);
            }
        }
    }
    v75 = r_u16(((uint32)(((uint32)(object) + (uint32)(390)))));
    if ((v75 == 8))
    {
        v76 = this->model.animation;
        if ((v76 == 8))
        {
            v77 = (this->model.flags & 0xFFFE);
        }
        else
        {
            if ((!(r_u8(((uint32)(((uint32)(object) + (uint32)(303)))))) || (v76 != 7)))
                goto LABEL_151;
            v77 = (this->model.flags | 1);
        }
        this->model.flags = v77;
    LABEL_151:
        v75 = r_u16(((uint32)(((uint32)(object) + (uint32)(390)))));
    }
    result = 2;
    if (((v75 == 256) && (this->model.animation == 2)))
    {
        v79 = DidAnimationCrossFrame(2, 3);
        if (sub_80062D24(object, v79, 1))
            return sub_80069EF4(121, ((uint32)(object) + (uint32)(4)), 3);
        v80 = DidAnimationCrossFrame(2, 17);
        result = sub_80062D24(object, v80, 2);
        if (result)
            return sub_80069EF4(121, ((uint32)(object) + (uint32)(4)), 3);
    }
    return result;
}

// Native helper preserves original signed DIV corner results
static uint32 DivideExplosionDamage(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

uint32 Swat::ApplyExplosionDamage()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 index = 0, count, result, radius, distance, damage, table, target, receiver;
    FUNCTION_MARKER(0x8004E624, "SLUS_003.73");
    while (1)
    {
        count = r_u32(0x800FF648);
        result = (uint32)((sint32)index < (sint32)count);
        if (!result)
            return result;
        distance = sub_8006696C(object + 4, 0x800A724C + index * 12);
        radius = r_u32(0x800A727C + index * 4);
        if ((sint32)distance < (sint32)radius)
        {
            damage = r_u32(0x800A7288 + index * 4);
            damage = DivideExplosionDamage(damage * (radius - distance), radius);
            receiver = object;
            ApplyObjectDamage(receiver, damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
            result = (uint32)(sint32)(sint16)r_u16(object + 218);
            if ((sint32)result <= 0)
                return result;
        }
        ++index;
    }
}



uint32 Swat::Damage(uint32 damage, const sint32 *input_vector, uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004F5C8, "SLUS_003.73");
    uint32 count, flags, remaining, hero;
    sint32 health;
    if (r_u16(object + 390) == 64 && reason == 5)
        return 0;
    count = r_u8(object + 383);
    flags = r_u32(object + 396);
    w_u8(object + 383, count + 1);
    health = (sint16)r_u16(object + 218);
    w_u32(object + 396, flags | 0x400);
    remaining = r_u16(object + 218) - damage;
    if (health <= 0)
        return 1;
    w_u16(object + 218, remaining);
    if ((sint32)(remaining << 16) > 0)
    {
        SetColor(3, 255, 255, 255);
        SpawnZombieFragment();
    }
    else
    {
        hero = visual_lists.players;
        if (hero)
            TryPlayPlayerKillQuote((const uint32 *)psx_addr(hero + 4, 12));
        flags = this->lifecycle_flags;
        motion.angular_acceleration[1] = 0;
        motion.angular_velocity[1] = 0;
        this->lifecycle_flags = flags & 0xFFEF;
        DetachTurret();
        DeathReaction(input_vector, reason);
    }
    return 1;
}

uint32 Swat::SpawnZombieFragment()
{
    uint32 trig_angle;
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 angle, random, velocity[3], child, kind, height, name, flags;
    FUNCTION_MARKER(0x8004F6C4, "SLUS_003.73");
    if (r_u16(object + 390) != 128)
        return 128;
    angle = RandomRange(4096);
    random = RandomRange(16);
    trig_angle = angle & 0xFFF;
    velocity[0] = (uint32)rsin(trig_angle) * (random + 24);
    random = RandomRange(16);
    velocity[1] = (0 - 32 - random) << 12;
    random = RandomRange(16);
    velocity[2] = (uint32)rcos(trig_angle) * (random + 24);
    child = AllocateZeroedObject(320);
    if (child)
    {
        random = RandomRange(r_u8(object + 615));
        kind = r_u8(object + 614);
        height = (uint32)(sint32)(sint16)r_u16(object + 456) << 12;
        height += this->model.pos[1];
        name = r_u32(0x800FF62C);
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(Debris));
        new (dram_addr) Debris((const uint32 *)this->model.pos, velocity, (kind + random) & 0xFFFF, height, 9, 24, psx_addr(name, 1));
    }
    if (!RandomRange(2))
        return 0x10000;
    flags = r_u32(object + 372);
    if (!(flags & 0x10000))
        flags |= 0x100C0;
    else if (!(flags & 0x1000))
        flags |= 0x1300;
    else
    {
        uint32 result = flags | 7;
        if (!(flags & 1))
            w_u32(object + 372, result);
        return result;
    }
    w_u32(object + 372, flags);
    return flags;
}

void Swat::DeathReaction(const sint32 *vector, uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004F840, "SLUS_003.73");
    uint32 x, y, z, nonzero, flags, value, divisor, factor;
    sint32 direction[3];
    sint16 angles[3];
    x = ReadVector(vector, 0);
    y = ReadVector(vector, 1);
    z = ReadVector(vector, 2);
    nonzero = x | y | z;
    if (r_u16(object + 390) == 128)
    {
        flags = r_u32(object + 372);
        SetDeathState(16);
        w_u8(object + 380, 0);
        w_u32(object + 372, flags | 0x113FF);
        return;
    }
    if ((r_u32(object + 396) & 0x200) || this->model.animation == 4)
    {
        FragmentAndFinish(0);
        return;
    }
    motion.angular_velocity[2] = 0;
    motion.angular_velocity[1] = 0;
    motion.angular_velocity[0] = 0;
    motion.angular_acceleration[2] = 0;
    motion.angular_acceleration[1] = 0;
    motion.angular_acceleration[0] = 0;
    if (reason == 5)
    {
        if (!RandomRange(5))
            FragmentAndFinish(5);
        else
        {
            flags = r_u32(object + 396);
            w_u8(object + 380, 0);
            SetDeathState(11);
            w_u32(object + 396, flags | 4);
        }
        return;
    }
    if ((sint32)reason < 6 && reason == 2)
    {
        FragmentAndFinish(0);
        return;
    }
    if (reason == 9 || reason == 10)
    {
        if ((r_u16(object + 390) & 31) && !RandomRange(reason == 9 ? 3 : 4))
        {
            EnterDeathState14();
            return;
        }
        value = RandomRange(reason == 9 ? 3 : 2);
        if (reason == 9 && !value)
        {
            w_u8(object + 380, 0);
            SetDeathState(8);
        }
        else if (reason == 10 && value)
            EnterDeathState13();
        else
            FragmentAndFinish(0);
        return;
    }
    if (RandomRange(2))
    {
        FragmentAndFinish(0);
        return;
    }
    if ((r_u16(object + 390) & 31) && !RandomRange(5))
    {
        EnterDeathState14();
        return;
    }
    value = r_u16(object + 390);
    w_u8(object + 380, 0);
    if (value != 4)
    {
        SetDeathState(8);
        return;
    }
    if (nonzero)
    {
        x = ReadVector(vector, 0);
        z = ReadVector(vector, 2);
        direction[0] = (sint32)x;
        direction[1] = 0;
        direction[2] = (sint32)z;
        divisor = MeasureVectorLength((const uint32 *)direction) + 1;
        if ((sint32)divisor < 128)
        {
            factor = (uint32)Divide(128, divisor);
            direction[0] = (sint32)((uint32)direction[0] * factor);
            direction[2] = (sint32)((uint32)direction[2] * factor);
        }
        CalculateDirectionAngles((uint16 *)angles, (const uint32 *)direction, (const uint32 *)psx_addr(0x800A71CC, 12));
        this->model.angles[1] = (uint16)angles[1];
    }
    SetDeathState(10);
}

void Swat::FragmentAndFinish(uint32 mode)
{
    uint32 trig_angle;
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004FB00, "SLUS_003.73");
    uint32 angle = RandomRange(1024);
    uint32 index;
    for (index = 0; index < 6; ++index)
    {
        uint32 velocity[3];
        uint32 child;
        if (r_u32(object + 220) >= 2048 || !visual_lists.cameras || RandomRange(4))
        {
            uint32 random = RandomRange(16);
            uint32 trig_angle = angle & 4095;
            velocity[0] = (uint32)rsin(trig_angle) * (random + 24);
            velocity[1] = (0 - 32 - RandomRange(16)) << 12;
            random = RandomRange(16);
            velocity[2] = (uint32)rcos(trig_angle) * (random + 24);
        }
        else
        {
            uint32 target = visual_lists.cameras;
            uint32 position[3];
            uint32 divisor;
            sint32 numerator;
            position[0] = r_u32(target + 4);
            position[1] = r_u32(target + 8);
            position[2] = r_u32(target + 12);
            position[1] += 0x80000;
            divisor = MeasurePositionDistance((const uint32 *)this->model.pos, position) >> 7;
            // MIPS DIV defines LO even for zero
            numerator = (sint32)(position[0] - this->model.pos[0]);
            velocity[0] = (uint32)Divide(numerator, divisor);
            numerator = (sint32)(position[2] - this->model.pos[2]);
            velocity[2] = (uint32)Divide(numerator, divisor);
            numerator = (sint32)(position[1] - this->model.pos[1]);
            velocity[1] = ((uint32)Divide(numerator, divisor)) - (divisor << 13);
        }
        uint32 fragment_type = mode == 5 ? 5 : (r_u16(object + 390) & 0x180) ? 9 : 3;
        child = AllocateZeroedObject(320);
        if (child)
        {
            uint32 variation = RandomRange(r_u8(object + 615));
            uint32 kind = (r_u8(object + 614) + variation) & 65535;
            uint32 height = (uint32)(sint32)(sint16)r_u16(object + 456) << 12;
            height += this->model.pos[1];
            uint32 name = r_u32(0x800FF62C);
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(Debris));
            new (dram_addr) Debris((const uint32 *)this->model.pos, velocity, kind, height, fragment_type, 24, psx_addr(name, 1));
            if (mode == 5)
                SetColor(0, 0, 0, 0);
        }
        angle += 682;
    }
    uint32 table = r_u32(object + 68);
    uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
    uint32 target = r_u32(table + 20);
    apocalypse_object_virtual20(target, receiver);
}

uint32 Swat::EnterDeathState13()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004FE20, "SLUS_003.73");
    uint32 kind = r_u16(object + 390);
    uint32 mask, result;
    w_u16(object + 472, 13);
    w_u8(object + 380, 0);
    w_u16(object + 474, 0);
    switch (kind)
    {
        case 1: mask = 0x1F80; break;
        case 2: mask = 0xFF80; break;
        case 4: mask = 0x7D; break;
        case 8: mask = 0x1FF80; break;
        default: return kind < 3 ? 1 : 8;
    }
    result = r_u32(object + 372) | mask;
    w_u32(object + 372, result);
    return result;
}

uint32 Swat::EnterDeathState14()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004FEC4, "SLUS_003.73");
    uint32 kind = r_u16(object + 390);
    uint32 index = kind - 1;
    uint32 mask, result;
    w_u8(object + 380, 0);
    w_u16(object + 472, 14);
    w_u16(object + 474, 0);
    if (index >= 16)
        return 0;
    result = r_u32(0x800A2E08 + index * 4);
    switch (kind)
    {
        case 1: mask = 0x1000; break;
        case 2: mask = 0x8000; break;
        case 4: mask = 4; break;
        case 8: mask = 0x10000; break;
        case 16: mask = 0x2000; break;
        default: return result;
    }
    result = r_u32(object + 372) | mask;
    w_u32(object + 372, result);
    return result;
}

void Swat::UpdateState()
{
    SVECTOR motion_cur, motion_target;
    uint32 motion_cur_word, motion_cur_tail, motion_target_word, motion_target_tail;
    uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8004FF6C, "SLUS_003.73");
    CollisionRay raycast;

    uint32 position_output[3];
    uint32 effect_position0[3], effect_position1[3];
    uint32 distance_position[3];
    sint16 desired_angles[3];
    sint16 effect_angles[3];
    sint32 v1;
    sint32 result;
    uint32 v3;
    sint32 v4;
    sint32 v5;
    sint32 v6;
    sint32 v7;
    sint32 v8;
    short v9;
    sint32 v10;
    sint32 v11;
    sint32 v12;
    sint32 v13;
    sint32 v14;
    sint32 v15;
    short v16;
    sint32 v17;
    sint32 v18;
    sint32 v19;
    sint32 v20;
    unsigned short v21;
    sint32 v22;
    sint32 v23;
    uint32 v24;
    sint32 v25;
    sint32 v26;
    sint32 v27;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    sint32 v31;
    sint32 v32;
    unsigned short v33;
    sint32 v34;
    sint32 v35;
    short v36;
    unsigned short v37;
    sint32 v38;
    sint32 v39;
    sint32 v40;
    sint32 v41;
    short v42;
    short v43;
    uint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    sint32 v50;
    sint32 v51;
    sint32 v52;
    sint32 v53;
    uint32 v54;
    sint32 v55;
    sint32 v56;
    sint32 v57;
    sint32 v59;
    sint32 v60;
    sint32 v61;
    sint32 v62;
    sint32 v63;
    uint32 v64;
    short v65;
    sint32 v66;
    sint32 v67;
    sint32 v68;
    sint32 v69;
    uint32 v70;
    sint32 v71;
    sint32 v72;
    sint32 v73;
    sint32 v74;
    sint32 v75;
    uint32 v76;
    short v77;
    short v78;
    sint32 v79;
    sint32 v80;
    sint32 v81;
    sint32 v82;
    sint32 v83;
    sint32 v84;
    sint32 v85;
    sint32 v86;
    short v87;
    short v88;
    short v89;
    sint32 v90;
    short v91;
    sint32 v92;
    short v93;
    sint32 v94;
    sint32 v95;
    sint32 v96;
    uint32 v97;
    short v98;
    sint32 v99;
    sint32 v100;
    sint32 v101;
    sint32 v102;
    short v103;
    sint32 v104;
    sint32 v105;
    short v106;
    sint32 v107;
    sint32 v108;
    sint32 v109;
    sint32 v110;
    sint32 v111;
    sint32 v112;
    uint32 v113;
    sint32 v114;
    sint32 v115;
    sint32 v116;
    sint32 v117;
    int v127[4];
    sint32 v128;
    sint32 v129;
    (v1 = object);
    (result = -2146828288);
    switch (r_u16(((uint32)((object + 472)))))
    {
        case 0:
            (v3 = r_u16(((uint32)((object + 474)))));
            (result = (v3 < 2));
            if ((v3 == 1))
            {
                if (((r_u32(((uint32)((object + 396)))) & 2) != 0))
                {
                    (result = (r_u16(((uint32)((object + 216)))) & 2));
                    if (!result)
                        return;
                    (v10 = ((sint8)(this->model.frame)));
                    w_u8(((uint32)((object + 382))), 0);
                }
                else
                {
                    (v11 = (sint32)((uint32)(sint16)r_u16(object + 460) << 14));
                    (v12 = (sint32)((uint32)(sint16)r_u16(object + 456) << 12));
                    (result = (v11 < (sint32)(this->model.pos[1] + (uint32)v12)));
                    if ((v11 >= (sint32)(this->model.pos[1] + (uint32)v12)))
                        return;
                    this->model.pos[1] = ((uint32)v11 - (uint32)v12);
                    w_u8(((uint32)((object + 382))), 0);
                    motion.velocity[2] = 0;
                    motion.velocity[1] = 0;
                    (v10 = ((sint8)(this->model.frame)));
                    motion.velocity[0] = 0;
                }
                SetAnimationRange(4, v10, -1);
                (v8 = r_u16(((uint32)((v1 + 390)))));
                w_u16(((uint32)((v1 + 474))), 2);
                (result = 8);
                goto LABEL_20;
            }
            if ((r_u16(((uint32)((object + 474)))) < 2))
            {
                if (r_u16(((uint32)((object + 474)))))
                    return;
                (v4 = visual_lists.players);
                (v5 = (visual_lists.players == 0));
                w_u32(((uint32)((object + 396))), (r_u32(((uint32)((object + 396)))) & (~1)));
                if (!v5)
                    this->model.angles[1] = (3072 - ratan2((sint32)(r_u32(((uint32)((v4 + 12)))) - this->model.pos[2]), (sint32)(r_u32(((uint32)((v4 + 4)))) - this->model.pos[0])));
                (v6 = sub_80067A18((v1 + 4), 0, 4096));
                if ((v6 == -1))
                {
                    (void)(Finish());
                    return;
                }
                (v7 = ((sint16)(r_u16(((uint32)((v1 + 456)))))));
                if (((sint32)((uint32)v6 - this->model.pos[1]) >> 12) >= 2 * v7)
                {
                    w_u16(((uint32)((v1 + 460))), (v6 >> 14));
                    w_u16(((uint32)((v1 + 474))), 1);
                    sub_80063038(v1, 4, 21, 22);
                    (v9 = r_u16(((uint32)((v1 + 78)))));
                    w_u8(((uint32)((v1 + 382))), 1);
                    w_u16(((uint32)((v1 + 78))), (v9 & 0xFFEF));
                    {
                        (void)(sub_80062D84(v1));
                        return;
                    }
                }
                this->model.pos[1] = ((uint32)v6 - ((uint32)v7 << 12));
                w_u16(((uint32)((v1 + 472))), 1);
                w_u8(((uint32)((v1 + 382))), 0);
                motion.velocity[2] = 0;
                motion.velocity[1] = 0;
                (v8 = r_u16(((uint32)((v1 + 390)))));
                (result = 8);
                motion.velocity[0] = 0;
            LABEL_20:
                if ((v8 != 8))
                {
                    (void)(sub_80062D70(v1));
                    return;
                }

                return;
            }
            (result = 2);
            if ((v3 == 2))
            {
                (result = r_u8(((uint32)((object + 303)))));
                if (r_u8(((uint32)((object + 303)))))
                {
                    (v13 = this->lifecycle_flags);
                    w_u32(((uint32)((object + 472))), 1);
                    (result = (v13 | 0x10));
                    this->lifecycle_flags = result;
                }
            }
            return;

        case 1:
            (result = r_u16(((uint32)((object + 474)))));
            if (!(r_u16(((uint32)((object + 474))))))
            {
                (v14 = r_u32(((uint32)((object + 396)))));
                w_u8(((uint32)((object + 382))), 0);
                w_u32(((uint32)((object + 396))), (v14 & 0xFFFFFFFE));
                motion.velocity[2] = 0;
                motion.velocity[1] = 0;
                motion.velocity[0] = 0;
                motion.angular_velocity[2] = 0;
                motion.angular_velocity[1] = 0;
                motion.angular_velocity[0] = 0;
                motion.angular_acceleration[2] = 0;
                motion.angular_acceleration[1] = 0;
                (v15 = r_u8(((uint32)((object + 303)))));
                motion.angular_acceleration[0] = 0;
                if (v15)
                    SetAnimationRange(0, 0, -1);
                (v16 = ChooseBehavior());
                (v17 = r_u16(((uint32)((v1 + 390)))));
                w_u16(((uint32)((v1 + 472))), (v16 + 2));
                (result = 4);
                w_u16(((uint32)((v1 + 474))), 0);
                if ((v17 == 4))
                {
                    (v18 = RandomRange(16));
                    (v19 = 43);
                    if (!v18)
                    {
                        (void)(sub_80069DF0(v19, 0x2000, 0));
                        return;
                    }
                    (result = RandomRange(16));
                    (v19 = 44);
                    if (!result)
                    {
                        (void)(sub_80069DF0(v19, 0x2000, 0));
                        return;
                    }
                }
            }
            return;

        case 2:
            (v20 = r_u16(((uint32)((object + 474)))));
            (result = 1);
            if (!(r_u16(((uint32)((object + 474))))))
            {
                motion.velocity[2] = 0;
                motion.velocity[1] = 0;
                motion.velocity[0] = 0;
                SetAnimationRange(3, 0, -1);
                (result = (r_u16(((uint32)((v1 + 474)))) + 1));
                goto LABEL_210;
            }
            if ((v20 == 1))
            {
                if (visual_lists.players)
                {
                    xport_draft_host_sub_80066B8C_p1(desired_angles, v1 + 4, visual_lists.players + 4);
                    desired_angles[0] = 0;
                    (v21 = this->model.angles[2]);
                    (v114 = ((uint32)this->model.angles[0] | ((uint32)this->model.angles[1] << 16)));

                    motion_cur_word = (((unsigned short)(v114)) | (((uint32)(v114 >> 16) & 65535) << 16));
                    motion_cur_tail = v21;
                    motion_target_word = (uint16)desired_angles[0] | ((uint32)(uint16)desired_angles[1] << 16);
                    motion_target_tail = (uint16)desired_angles[2];
                    motion_cur.vx = (sint16)motion_cur_word;
                    motion_cur.vy = (sint16)(motion_cur_word >> 16);
                    motion_cur.vz = (sint16)motion_cur_tail;
                    motion_cur.pad = (sint16)(motion_cur_tail >> 16);
                    motion_target.vx = (sint16)motion_target_word;
                    motion_target.vy = (sint16)(motion_target_word >> 16);
                    motion_target.vz = (sint16)motion_target_tail;
                    motion_target.pad = (sint16)(motion_target_tail >> 16);
                    UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, 32);
                }
                (result = r_u8(((uint32)((v1 + 303)))));
                if (r_u8(((uint32)((v1 + 303)))))
                {
                    motion.angular_acceleration[1] = 0;
                    motion.angular_velocity[1] = 0;
                    goto LABEL_86;
                }
            }
            return;

        case 3:
            (v22 = r_u16(((uint32)((object + 474)))));
            (result = 1);
            if (!(r_u16(((uint32)((object + 474))))))
            {
                (v23 = (object + 484));
                (v24 = ((uint32)((object + 484))));
                (v25 = 128);
                (v107 = 192);
                (v26 = 1024);
                goto LABEL_43;
            }
            if ((v22 != 1))
                return;
            (v27 = r_u32(((uint32)((object + 396)))));
            if (((v27 & 0x400) != 0))
            {
                (result = (v27 & 0xFFFFFFFE));
                w_u32(((uint32)((v1 + 396))), (v27 & 0xFFFFFFFE));
            LABEL_47:
                w_u32(((uint32)((v1 + 472))), ((unsigned short)(v22)));

                return;
            }
            (result = (v27 & 1));
            if (((v27 & 1) == 0))
                goto LABEL_47;
            (result = visual_lists.players);
            if (visual_lists.players)
            {
                (result = -2);
                if ((r_u32(((uint32)((v1 + 220)))) < r_u32(((uint32)((v1 + 512))))))
                {
                    w_u32(((uint32)((v1 + 396))), (v27 & 0xFFFFFFFE));
                    (result = 2);
                    goto LABEL_166;
                }
            }
            return;

        case 4:
            (v28 = r_u16(((uint32)((object + 474)))));
            (result = 1);
            if (!(r_u16(((uint32)((object + 474))))))
            {
                motion.velocity[2] = 0;
                motion.velocity[1] = 0;
                (v29 = this->model.pos[0]);
                (v30 = visual_lists.players);
                motion.velocity[0] = 0;
                distance_position[0] = v29;
                distance_position[1] = this->model.pos[1] - ((uint32)(sint16)r_u16(object + 458) << 12);
                distance_position[2] = this->model.pos[2];
                v5 = xport_draft_host_sub_800679A4_p1(distance_position, v30 + 4) == 0;
                (result = 1);
                if (v5)
                    goto LABEL_166;
                BeginAttack();
                (v31 = r_u16(((uint32)((v1 + 474)))));
                w_u16(((uint32)((v1 + 460))), 0);
                goto LABEL_209;
            }
            if ((v28 == 1))
            {
                (v32 = ((sint16)(r_u16(((uint32)((object + 460)))))));
                (v5 = (v32 < ((unsigned short)(sub_8004BDFC(object, 1, 3)))));
                (result = (v32 + 1));
                if (v5)
                    w_u16(((uint32)((v1 + 460))), result);
                else
                    w_u32(((uint32)((v1 + 472))), ((unsigned short)(v28)));
                if (visual_lists.players)
                {
                    xport_draft_host_sub_80066B8C_p1(desired_angles, v1 + 4, visual_lists.players + 4);
                    desired_angles[0] = 0;
                    (v33 = this->model.angles[2]);
                    (v116 = ((uint32)this->model.angles[0] | ((uint32)this->model.angles[1] << 16)));

                    {
                        motion_cur_word = (((unsigned short)(v116)) | (((uint32)(v116 >> 16) & 65535) << 16));
                        motion_cur_tail = v33;
                        motion_target_word = (uint16)desired_angles[0] | ((uint32)(uint16)desired_angles[1] << 16);
                        motion_target_tail = (uint16)desired_angles[2];
                        motion_cur.vx = (sint16)motion_cur_word;
                        motion_cur.vy = (sint16)(motion_cur_word >> 16);
                        motion_cur.vz = (sint16)motion_cur_tail;
                        motion_cur.pad = (sint16)(motion_cur_tail >> 16);
                        motion_target.vx = (sint16)motion_target_word;
                        motion_target.vy = (sint16)(motion_target_word >> 16);
                        motion_target.vz = (sint16)motion_target_tail;
                        motion_target.pad = (sint16)(motion_target_tail >> 16);
                        (void)(UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, 32));
                        return;
                    }
                }
            }
            return;

        case 5:
            (v34 = r_u16(((uint32)((object + 474)))));
            (v23 = (v1 + 484));
            if (!(r_u16(((uint32)((v1 + 474))))))
            {
                (object = v1);
                (v24 = ((uint32)((v1 + 484))));
                (v25 = 1024);
                (v107 = 2048);
                (v26 = 512);
            LABEL_43:
                v20 = 1;

                (result = sub_8004BA64(object, v24, ((uint32)((visual_lists.players + 4))), v25, v107, r_u16(((uint32)((v1 + 392)))), v26, 1, 1));
                goto LABEL_85;
            }
            (result = 1);
            if ((v34 == 1))
            {
                (v35 = r_u32(((uint32)((v1 + 396)))));
                if (((v35 & 0x400) == 0))
                    goto LABEL_89;
                (result = (v35 & 0xFFFFFFFE));
                w_u32(((uint32)((v1 + 396))), (v35 & 0xFFFFFFFE));
                goto LABEL_90;
            }
            return;

        case 6:
            if (r_u16(((uint32)((object + 474)))))
            {
                (result = 128);
                if ((r_u16(((uint32)((object + 474)))) == 1))
                {
                    if (((r_u16(((uint32)((object + 390)))) == 128) && visual_lists.players))
                    {
                        (v36 = ratan2((sint32)(r_u32(((uint32)((visual_lists.players + 12)))) - this->model.pos[2]), (sint32)(r_u32(((uint32)((visual_lists.players + 4)))) - this->model.pos[0])));
                        desired_angles[0] = 0;
                        desired_angles[1] = (3072 - v36) & 0xFFF;
                        desired_angles[2] = 0;
                        (v37 = this->model.angles[2]);
                        (v114 = ((uint32)this->model.angles[0] | ((uint32)this->model.angles[1] << 16)));

                        motion_cur_word = (((unsigned short)(v114)) | (((uint32)(v114 >> 16) & 65535) << 16));
                        motion_cur_tail = v37;
                        motion_target_word = (uint16)desired_angles[0] | ((uint32)(uint16)desired_angles[1] << 16);
                        motion_target_tail = (uint16)desired_angles[2];
                        motion_cur.vx = (sint16)motion_cur_word;
                        motion_cur.vy = (sint16)(motion_cur_word >> 16);
                        motion_cur.vz = (sint16)motion_cur_tail;
                        motion_cur.pad = (sint16)(motion_cur_tail >> 16);
                        motion_target.vx = (sint16)motion_target_word;
                        motion_target.vy = (sint16)(motion_target_word >> 16);
                        motion_target.vz = (sint16)motion_target_tail;
                        motion_target.pad = (sint16)(motion_target_tail >> 16);
                        UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, 32);
                    }
                    (result = 1);
                    if (((r_u32(((uint32)((v1 + 396)))) & 0x400) != 0))
                        goto LABEL_166;
                    (result = 1);
                    if (r_u8(((uint32)((v1 + 303)))))
                        goto LABEL_166;
                    (result = r_u32(((uint32)((v1 + 548)))));
                    if (result)
                    {
                        (result = 1);
                        if ((r_u32(((uint32)((v1 + 220)))) < r_u32(((uint32)((v1 + 552))))))
                        {
                            w_u16(((uint32)((v1 + 472))), 1);
                            goto LABEL_167;
                        }
                    }
                }
            }
            else
            {
                if (this->model.animation)
                    SetAnimationRange(0, 0, -1);
                (result = 1);
                w_u16(((uint32)((v1 + 474))), 1);
            }
            return;

        case 7:
            (v34 = r_u16(((uint32)((object + 474)))));
            (result = 1);
            if (r_u16(((uint32)((v1 + 474)))))
            {
                if ((v34 != 1))
                    return;
                (v35 = r_u32(((uint32)((v1 + 396)))));
                if (((v35 & 0x400) != 0))
                {
                    (result = (v35 & 0xFFFFFFFE));
                    w_u32(((uint32)((v1 + 396))), (v35 & 0xFFFFFFFE));
                }
                else
                {
                LABEL_89:
                    (result = (v35 & 1));

                    if (((v35 & 1) != 0))
                        return;
                }
            LABEL_90:
                w_u32(((uint32)((v1 + 472))), ((unsigned short)(v34)));
            }
            else
            {
                (v23 = (v1 + 484));
                v20 = 1;
                (result = sub_8004BA64(v1, ((uint32)((v1 + 484))), ((uint32)((v1 + 4))), 1024, 2048, r_u16(((uint32)((v1 + 392)))), 4096, 1, 1));
            LABEL_85:
                (v38 = v1);

                if (result)
                    goto LABEL_157;
            LABEL_86:
                w_u16(((uint32)((v1 + 472))), v20);

                w_u16(((uint32)((v1 + 474))), 0);
            }
            return;

        case 8:
            (result = 1);
            if (r_u16(((uint32)((object + 474)))))
            {
                if ((r_u16(((uint32)((object + 474)))) == 1))
                {
                    (result = r_u8(((uint32)((object + 303)))));
                    if (r_u8(((uint32)((object + 303)))))
                    {
                        (v41 = ((sint16)(r_u16(((uint32)((object + 460)))))));
                        (v5 = (v41 == 0));
                        (v42 = (v41 - 1));
                        if (v5)
                            Finish();
                        else
                            w_u16(((uint32)((object + 460))), v42);
                        (result = ((sint16)(r_u16(((uint32)((v1 + 462)))))));
                        if (!(r_u16(((uint32)((v1 + 462))))))
                        {
                            sub_800626F8(v1, (uint32)-32, 0, 0, 0);
                            (v43 = this->model.flags);
                            this->model.render_mode = 128;
                            this->model.flags = (v43 | 0x800);
                            (result = 1);
                            w_u16(((uint32)((v1 + 462))), 1);
                        }
                    }
                }
            }
            else
            {
                motion.velocity[2] = 0;
                motion.velocity[1] = 0;
                motion.velocity[0] = 0;
                motion.angular_velocity[2] = 0;
                motion.angular_velocity[1] = 0;
                motion.angular_velocity[0] = 0;
                motion.angular_acceleration[2] = 0;
                motion.angular_acceleration[1] = 0;
                (v39 = r_u32(((uint32)((object + 396)))));
                motion.angular_acceleration[0] = 0;
                w_u16(((uint32)((object + 212))), 0);
                w_u32(((uint32)((object + 396))), (v39 & 0xFFFFFFFE));
                sub_8004B948(object);
                sub_80063038(v1, 5, 0, -1);
                sub_80062D84(v1);
                (v40 = r_u16(((uint32)((v1 + 390)))));
                w_u16(((uint32)((v1 + 474))), 1);
                w_u16(((uint32)((v1 + 460))), 32);
                (result = 256);
                w_u16(((uint32)((v1 + 462))), 0);
                if ((v40 == 256))
                {
                    (void)(sub_80069EF4(120, (v1 + 4), 3));
                    return;
                }
            }
            return;

        case 0xA:
            (v44 = r_u16(((uint32)((object + 474)))));
            (result = (v44 < 2));
            if ((v44 == 1))
            {
                if (r_u16(((uint32)((object + 460)))))
                {
                    sub_80034F9C((object + 16));
                    ::SetParticleSize(2);
                    (v59 = 0);
                    sub_800350FC(r_u8(0x800ECC94), r_u8(0x800ECC96), r_u8(0x800ECC98));
                    sub_80035110(((((unsigned short)(r_u16(0x800ECC94))) >> 5) & 255), ((((unsigned short)(r_u16(0x800ECC96))) >> 5) & 255), ((((unsigned short)(r_u16(0x800ECC98))) >> 5) & 255));
                    w_u32(0x800FF3AC, 0);
                    do
                    {
                        (v60 = sub_80032DC0(88));
                        if (v60)
                            sub_80035124(v60, ((uint32)((v1 + 4))), 16, 0x2000, 16);
                        ++v59;
                    } while ((v59 < 16));
                    (v61 = ((sint16)(r_u16(((uint32)((v1 + 460)))))));
                    w_u32(0x800FF3AC, 1);
                    (result = (v61 - 1));
                    w_u16(((uint32)((v1 + 460))), result);
                    return;
                }
            LABEL_208:
                motion.velocity[2] = 0;

                motion.velocity[1] = 0;
                (v31 = r_u16(((uint32)((object + 474)))));
                motion.velocity[0] = 0;
            LABEL_209:
                (result = (v31 + 1));

                goto LABEL_210;
            }
            if ((r_u16(((uint32)((object + 474)))) < 2))
            {
                if (r_u16(((uint32)((object + 474)))))
                    return;
                this->model.angles[0] = 0;
                motion.angular_velocity[2] = 0;
                motion.angular_velocity[1] = 0;
                motion.angular_velocity[0] = 0;
                motion.angular_acceleration[2] = 0;
                motion.angular_acceleration[1] = 0;
                (v45 = r_u32(((uint32)((object + 396)))));
                motion.angular_acceleration[0] = 0;
                w_u16(((uint32)((object + 212))), 0);
                w_u32(((uint32)((object + 396))), (v45 & 0xFFFFFFFE));
                sub_80062D84(object);
                sub_800667CC((v1 + 484), 1024, (v1 + 16));
                xport_draft_host_sub_8006C3AC_p1(position_output, (v1 + 4), (v1 + 484));
                v113 = position_output[0];
                v114 = position_output[2];
                (v46 = (sint32)position_output[1]);
                (v47 = v114);
                w_u32(((uint32)((v1 + 484))), v113);
                w_u32(((uint32)((v1 + 488))), v46);
                w_u32(((uint32)((v1 + 492))), v47);
                (raycast.pos[0][0] = this->model.pos[0]);
                (raycast.pos[0][1] = this->model.pos[1]);
                (raycast.pos[0][2] = this->model.pos[2]);
                (raycast.pos[1][0] = r_u32(((uint32)((v1 + 484)))));
                (raycast.pos[1][1] = r_u32(((uint32)((v1 + 488)))));
                (raycast.pos[1][2] = r_u32(((uint32)((v1 + 492)))));
                InitializeCollisionRay(raycast);
                (raycast.triggers = 0);
                QueryCollisionRay(raycast, 1);
                if (raycast.hit.GetObject())
                {
                    (v48 = ((sint32)((uint32)raycast.distance - 256)));
                    if ((((sint32)((uint32)raycast.distance - 256)) >= 64))
                    {
                        (v49 = ((sint32)((uint32)raycast.distance - 256)));
                        if ((v48 < 0))
                            (v49 = ((sint32)((uint32)raycast.distance - 193)));
                        w_u16(((uint32)((v1 + 460))), (v49 >> 6));
                        sub_800667CC((v1 + 484), v48, (v1 + 16));
                        xport_draft_host_sub_8006C3AC_p1(position_output, (v1 + 4), (v1 + 484));
                        v113 = position_output[0];
                        v114 = position_output[2];
                        (v50 = (sint32)position_output[1]);
                        (v51 = v114);
                        w_u32(((uint32)((v1 + 484))), v113);
                        w_u32(((uint32)((v1 + 488))), v50);
                        w_u32(((uint32)((v1 + 492))), v51);
                        xport_draft_host_sub_8006C3AC_p1(v127, v1 + 484, v1 + 4);
                        (v128 = ((sint16)(r_u16(((uint32)((v1 + 460)))))));
                        xport_draft_host_sub_8006C4EC_p123(position_output, v127, &v128);
                        v113 = position_output[0];
                        v114 = position_output[2];
                        (v52 = (sint32)position_output[1]);
                        (v53 = v114);
                        motion.velocity[0] = v113;
                        motion.velocity[1] = v52;
                        motion.velocity[2] = v53;
                        (v54 = ((uint32)((v1 + 4))));
                        goto LABEL_117;
                    }
                    motion.velocity[2] = 0;
                    motion.velocity[1] = 0;
                    motion.velocity[0] = 0;
                    w_u16(((uint32)((v1 + 460))), 1);
                }
                else
                {
                    w_u16(((uint32)((v1 + 460))), 16);
                    xport_draft_host_sub_8006C3AC_p1(position_output, (v1 + 484), (v1 + 4));
                    v113 = position_output[0];
                    v114 = position_output[2];
                    (v129 = 16);
                    xport_draft_host_sub_8006C4EC_p123(v127, position_output, &v129);
                    (v55 = v127[1]);
                    (v56 = v127[2]);
                    motion.velocity[0] = v127[0];
                    motion.velocity[1] = v55;
                    motion.velocity[2] = v56;
                    (v57 = sub_80067A18((v1 + 484), ((sint16)(r_u16(((uint32)((v1 + 456)))))), (4 * ((sint16)(r_u16(((uint32)((v1 + 456)))))))));
                    (v54 = ((uint32)((v1 + 4))));
                    if ((v57 != -1))
                    {
                    LABEL_117:
                        sub_8001D320(v54, 128, 0x80, 0x40, 0, 5, 0, 100);

                        sub_80063038(v1, 5, 0, -1);
                        sub_80069DF0(45, 0x2000, 0);
                        w_u16(((uint32)((v1 + 212))), 0);
                        sub_8004B948(v1);
                        (result = (r_u16(((uint32)((v1 + 474)))) + 1));
                        goto LABEL_210;
                    }
                    w_u8(((uint32)((v1 + 382))), 1);
                }
                (v54 = ((uint32)((v1 + 4))));
                goto LABEL_117;
            }
            (result = 2);
            if ((v44 == 2))
            {
                if (r_u8(((uint32)((object + 382)))))
                {
                LABEL_188:
                    (result = r_u8(((uint32)((v1 + 303)))));

                    if (r_u8(((uint32)((v1 + 303)))))
                    {
                        (void)(Finish());
                        return;
                    }
                    return;
                }
                (result = r_u8(((uint32)((object + 303)))));
                (v62 = (object + 4));
                if (!(r_u8(((uint32)((v1 + 303))))))
                    return;
                (v63 = sub_80067A18(v62, 0, 256));
                (object = v1);
                if ((v63 == -1))
                {
                    (void)(FragmentAndFinish(0));
                    return;
                }
                (result = (r_u16(((uint32)((v1 + 474)))) + 1));
            LABEL_210:
                w_u16(((uint32)((v1 + 474))), result);
            }
            return;

        case 0xB:
            (v64 = r_u16(((uint32)((object + 474)))));
            (result = (v64 < 2));
            if ((v64 == 1))
                goto LABEL_138;
            if ((r_u16(((uint32)((object + 474)))) >= 2))
            {
                (result = 256);
                if ((v64 != 2))
                    return;
                if ((r_u16(((uint32)((object + 390)))) == 256))
                {
                    ::StopSoundVoices(r_u32(((uint32)((object + 480)))));
                    w_u32(((uint32)((v1 + 480))), 0);
                }
                goto LABEL_188;
            }
            if (r_u16(((uint32)((object + 474)))))
                return;
            sub_800626F8(object, 0, 255, 0, 0);
            sub_80062D84(v1);
            sub_8004B948(v1);
            w_u32(((uint32)((v1 + 176))), 37765184);
            w_u16(((uint32)((v1 + 180))), 10240);
            w_u16(((uint32)((v1 + 212))), 0);
            w_u16(((uint32)((v1 + 182))), 10);
            sub_80063038(v1, 6, 0, -1);
            if ((r_u16(((uint32)((v1 + 390)))) == 256))
                w_u32(((uint32)((v1 + 480))), sub_80069EF4(18, (v1 + 4), 3));
            else
                sub_80069EF4(25, (v1 + 4), 3);
            w_u32(((uint32)((v1 + 396))), (r_u32(((uint32)((v1 + 396)))) & (~1)));
            motion.velocity[2] = 0;
            motion.velocity[1] = 0;
            (v65 = r_u16(((uint32)((v1 + 474)))));
            motion.velocity[0] = 0;
            w_u16(((uint32)((v1 + 474))), (v65 + 1));
        LABEL_138:
            (result = 256);

            if (!(r_u8(((uint32)((v1 + 303))))))
                return;
            (v66 = r_u16(((uint32)((v1 + 390)))));
            if ((v66 != 256))
            {
                if (!RandomRange(4))
                    sub_80069EF4(25, (v1 + 4), 3);
                (v66 = r_u16(((uint32)((v1 + 390)))));
            }
            (v67 = 4);
            if ((v66 == 256))
                (v67 = 2);
            (v68 = RandomRange(v67));
            (v69 = v1);
            if ((!v68 || r_u32(0x800FF738)))
            {
                sub_80063038(v1, 9, 0, -1);
                (result = (r_u16(((uint32)((v1 + 474)))) + 1));
                goto LABEL_210;
            }
            {
                (void)(sub_80063038(v69, 6, 0, -1));
                return;
            }

        case 0xD:
            (v70 = r_u16(((uint32)((object + 474)))));
            (result = (v70 < 2));
            if ((v70 == 1))
            {
                effect_angles[0] = -1024;
                effect_angles[1] = 0;
                effect_angles[2] = 0;
                SetEffectDirection(effect_angles);
                effect_angles[0] = 512;
                effect_angles[1] = 4096;
                effect_angles[2] = 0;
                SetEffectAngles(effect_angles);
                ::SetParticleSize(2);
                (v72 = 0);
                sub_800350FC(r_u8(0x800ECC94), r_u8(0x800ECC96), r_u8(0x800ECC98));
                (v73 = 24);
                sub_80035110(((((unsigned short)(r_u16(0x800ECC94))) >> 5) & 255), ((((unsigned short)(r_u16(0x800ECC96))) >> 5) & 255), ((((unsigned short)(r_u16(0x800ECC98))) >> 5) & 255));
                w_u32(0x800FF3AC, 0);
                do
                {
                    (v74 = sub_80032DC0(88));
                    if (v74)
                        sub_80035124(v74, ((uint32)((v1 + 4))), v73, 0x2000, 16);
                    ++v72;
                    (v73 += 4);
                } while ((v72 < 4));
                (v75 = r_u32(((uint32)((v1 + 396)))));
                w_u32(0x800FF3AC, 1);
                (result = (v75 & 1));
                if (result)
                    return;
                (v5 = (RandomRange(2) == 0));
                (result = 8);
                if (!v5)
                {
                    (result = (r_u16(((uint32)((v1 + 474)))) - 1));
                    w_u16(((uint32)((v1 + 474))), result);
                    return;
                }
            }
            else
            {
                if ((r_u16(((uint32)((object + 474)))) >= 2))
                {
                    (result = 2);
                    if ((v70 == 2))
                        goto LABEL_188;
                    return;
                }
                if (r_u16(((uint32)((object + 474)))))
                    return;
                (v23 = (object + 484));
                (v71 = sub_8004BA64(object, ((uint32)((object + 484))), ((uint32)((object + 4))), 256, 512, r_u16(((uint32)((object + 392)))), 4096, 1, 1));
                (v38 = v1);
                if (v71)
                {
                LABEL_157:
                    (result = SetMoveTarget((const uint32 *)psx_addr((uint32)v23, 12)));

                    w_u16(((uint32)((v1 + 474))), 1);
                    return;
                }
                (result = 8);
            }
        LABEL_166:
            w_u16(((uint32)((v1 + 472))), result);

        LABEL_167:
            w_u16(((uint32)((v1 + 474))), 0);

            return;

        case 0xE:
            (v76 = r_u16(((uint32)((object + 474)))));
            (result = (v76 < 2));
            if ((v76 == 1))
                goto LABEL_175;
            if ((r_u16(((uint32)((object + 474)))) >= 2))
            {
                (result = 2);
                if ((v76 != 2))
                    return;
                effect_angles[0] = 512;
                effect_angles[1] = 4096;
                effect_angles[2] = 0;
                SetEffectAngles(effect_angles);
                ::SetParticleSize(2);
                (v83 = 0);
                sub_800350FC(r_u8(0x800ECC94), r_u8(0x800ECC96), r_u8(0x800ECC98));
                (v84 = 32);
                sub_80035110(((((unsigned short)(r_u16(0x800ECC94))) >> 5) & 255), ((((unsigned short)(r_u16(0x800ECC96))) >> 5) & 255), ((((unsigned short)(r_u16(0x800ECC98))) >> 5) & 255));
                xport_draft_host_sub_8007CC10_p1(effect_position0, v1, 1);
                effect_angles[0] = -1024;
                effect_angles[1] = 0;
                effect_angles[2] = 0;
                SetEffectDirection(effect_angles);
                w_u32(0x800FF3AC, 0);
                effect_position0[1] += 0x20000;
                do
                {
                    (v85 = sub_80032DC0(88));
                    if (v85)
                        xport_draft_host_sub_80035124_p2(v85, effect_position0, v84, 0x2000, 16);
                    ++v83;
                    (v84 += 4);
                } while ((v83 < 4));
                w_u32(0x800FF3AC, 1);
                goto LABEL_188;
            }
            if (r_u16(((uint32)((object + 474)))))
                return;
            sub_80062D84(object);
            (v77 = r_u16(((uint32)((v1 + 78)))));
            w_u16(((uint32)((v1 + 212))), 0);
            w_u16(((uint32)((v1 + 78))), (v77 & 0xFFEF));
            sub_8004B948(v1);
            sub_80063038(v1, 6, 0, -1);
            w_u32(((uint32)((v1 + 396))), ((r_u32(((uint32)((v1 + 396)))) & 0xFFFFEFFE) | 0x1000));
            motion.velocity[2] = 0;
            motion.velocity[1] = 0;
            (v78 = r_u16(((uint32)((v1 + 474)))));
            motion.velocity[0] = 0;
            w_u16(((uint32)((v1 + 474))), (v78 + 1));
        LABEL_175:
            effect_angles[0] = -1024;
            effect_angles[1] = 0;
            effect_angles[2] = 0;
            SetEffectDirection(effect_angles);
            effect_angles[0] = 512;
            effect_angles[1] = 4096;
            effect_angles[2] = 0;
            SetEffectAngles(effect_angles);
            ::SetParticleSize(2);
            (v79 = 0);
            sub_800350FC(r_u8(0x800ECC94), r_u8(0x800ECC96), r_u8(0x800ECC98));
            (v80 = 32);
            sub_80035110(((((unsigned short)(r_u16(0x800ECC94))) >> 5) & 255), ((((unsigned short)(r_u16(0x800ECC96))) >> 5) & 255), ((((unsigned short)(r_u16(0x800ECC98))) >> 5) & 255));
            xport_draft_host_sub_8007CC10_p1(effect_position1, v1, 1);
            w_u32(0x800FF3AC, 0);
            effect_position1[1] += 0x20000;
            do
            {
                (v81 = sub_80032DC0(88));
                if (v81)
                    xport_draft_host_sub_80035124_p2(v81, effect_position1, v80, 0x2000, 16);
                ++v79;
                (v80 += 4);
            } while ((v79 < 4));
            w_u32(0x800FF3AC, 1);
            (result = r_u8(((uint32)((v1 + 303)))));
            if (!(r_u8(((uint32)((v1 + 303))))))
                return;
            (v82 = RandomRange(2));
            (v69 = v1);
            if (v82)
            {
                (void)(sub_80063038(v69, 6, 0, -1));
                return;
            }
            sub_80063038(v1, 9, 0, -1);
            (result = (r_u16(((uint32)((v1 + 474)))) + 1));
            goto LABEL_210;

        case 0x10:
            (result = 1);
            if (!(r_u16(((uint32)((object + 474))))))
            {
                (v86 = RandomRange(64));
                motion.velocity[0] = 0;
                motion.velocity[1] = (0 - ((uint32)v86 + 96)) << 12;
                motion.velocity[2] = 0;
                (v88 = (RandomRange(256) - 128));
                (v87 = RandomRange(256));
                motion.angular_velocity[0] = v88;
                motion.angular_velocity[1] = (v87 - 128);
                motion.angular_velocity[2] = 0;
                motion.angular_drag[0] = 16;
                motion.angular_drag[1] = 16;
                motion.angular_drag[2] = 16;
                w_u8(((uint32)((v1 + 382))), 1);
                w_u32(((uint32)((v1 + 396))), (r_u32(((uint32)((v1 + 396)))) & (~1)));
                (v89 = RandomRange(32));
                (v90 = this->model.pos[1]);
                w_u16(((uint32)((v1 + 460))), (v89 + 16));
                (result = (r_u16(((uint32)((v1 + 456)))) + ((sint32)v90 >> 12)));
                (v91 = (r_u16(((uint32)((v1 + 474)))) + 1));
                w_u16(((uint32)((v1 + 462))), result);
                w_u16(((uint32)((v1 + 474))), v91);
                return;
            }
            if ((r_u16(((uint32)((object + 474)))) != 1))
                return;
            (v92 = ((sint16)(r_u16(((uint32)((object + 460)))))));
            (v5 = (v92 == 0));
            (v93 = (v92 - 1));
            if (v5)
            {
                (void)(FragmentAndFinish(0));
                return;
            }
            w_u16(((uint32)((object + 460))), v93);
            (v94 = ((sint16)(r_u16(((uint32)((object + 462)))))));
            (result = (v94 < (((sint32)(this->model.pos[1])) >> 12)));
            (v5 = (v94 >= (((sint32)(this->model.pos[1])) >> 12)));
            (v95 = (sint32)((uint32)v94 << 12));
            if (!v5)
            {
                (v96 = motion.velocity[1]);
                this->model.pos[1] = v95;
                (result = (sint32)(0 - (uint32)v96));
                motion.velocity[1] = result;
            }
            return;

        case 0x20:
            (v97 = r_u16(((uint32)((object + 474)))));
            (result = (v97 < 2));
            if ((v97 == 1))
            {
                if (((r_u32(((uint32)((object + 396)))) & 1) == 0))
                    goto LABEL_208;
                (result = visual_lists.players);
                if (visual_lists.players)
                {
                    (result = 2);
                    if ((r_u32(((uint32)((object + 220)))) < r_u32(((uint32)((object + 512))))))
                        w_u16(((uint32)((object + 474))), 2);
                }
            }
            else if ((r_u16(((uint32)((object + 474)))) >= 2))
            {
                (result = 2);
                if ((v97 == 2))
                {
                    w_u32(((uint32)((object + 396))), (r_u32(((uint32)((object + 396)))) & (~1)));
                    SetAnimationRange(8, 0, -1);
                    (v98 = r_u16(((uint32)((v1 + 78)))));
                    w_u16(((uint32)((v1 + 212))), 128);
                    (result = 8);
                    w_u16(((uint32)((v1 + 476))), 8);
                    w_u32(((uint32)((v1 + 472))), 1);
                    w_u16(((uint32)((v1 + 78))), (v98 | 0x10));
                }
            }
            else if (!(r_u16(((uint32)((object + 474))))))
            {
                if (sub_8004BA64(object, ((uint32)((object + 484))), ((uint32)((visual_lists.players + 4))), 128, 192, r_u16(((uint32)((object + 392)))), 1024, 1, 1))
                {
                    (result = SetMoveTarget((const uint32 *)psx_addr((uint32)v1 + 484, 12)));
                    w_u16(((uint32)((v1 + 474))), 1);
                }
                else
                {
                    (result = 2);
                    w_u16(((uint32)((v1 + 474))), 2);
                }
            }
            return;

        case 0x30:
            (v99 = r_u16(((uint32)((object + 474)))));
            (result = 1);
            if (!(r_u16(((uint32)((object + 474))))))
            {
                this->lifecycle_flags = (this->lifecycle_flags & (~0x10));
                (v101 = this->model.pos[1]);
                goto LABEL_225;
            }
            (v100 = (object + 2));
            if ((v99 != 1))
                return;
            (v102 = ((sint16)(r_u16(((uint32)((object + 462)))))));
            if ((v102 != 60))
                goto LABEL_228;
            w_u16(((uint32)((v1 + 78))), (r_u16(((uint32)((v1 + 78)))) | (0x10)));
            sub_80063038(v1, 0, 0, -1);
            (v103 = this->model.flags);
            w_u32(((uint32)((v1 + 472))), ((unsigned short)(v99)));
            (result = (v103 & 0xFFF7));
            this->model.flags = result;
            return;

        case 0x31:
            (v104 = r_u16(((uint32)((object + 474)))));
            (result = 1);
            if (r_u16(((uint32)((object + 474)))))
            {
                (v100 = (object + 2));
                if ((v104 == 1))
                {
                    (v102 = ((sint16)(r_u16(((uint32)((object + 462)))))));
                    if ((v102 == 60))
                    {
                        this->model.flags = (this->model.flags & (~8));
                        sub_80063038(v1, 0, 0, -1);
                        (result = 1);
                        w_u32(((uint32)((v1 + 472))), ((unsigned short)(v104)));
                        w_u8(((uint32)((v1 + 380))), 1);
                    }
                    else
                    {
                    LABEL_228:
                        w_u16(((uint32)((v100 + 460))), (v102 + 1));

                        (result = (this->model.pos[1] - 34952));
                        this->model.pos[1] = result;
                    }
                }
            }
            else
            {
                motion.velocity[2] = 0;
                motion.velocity[1] = 0;
                (v101 = this->model.pos[1]);
                motion.velocity[0] = 0;
            LABEL_225:
                this->model.pos[1] = ((uint32)v101 + 0x200000);

                StartAnimation(0, 1);
                (v105 = r_u16(((uint32)((v1 + 474)))));
                (v106 = this->model.flags);
                w_u16(((uint32)((v1 + 462))), 0);
                (result = (v105 + 1));
                w_u16(((uint32)((v1 + 474))), result);
                this->model.flags = (v106 | 8);
            }
            return;

        default:
            return;
    }
}

uint32 Swat::SetBehaviorRange(uint32 index, uint32 first, uint32 second, uint32 third, uint32 fourth)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8005171C, "SLUS_003.73");
    uint32 base = object + 16 * index;
    w_u32(base + 512, first);
    w_u32(base + 520, second);
    w_u32(base + 516, third);
    w_u32(base + 524, fourth);
    return third;
}

uint32 Swat::SetMode(uint32 mode)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80051740, "SLUS_003.73");
    sint32 v4;
    short v5;
    sint32 v6;
    short v7;
    uint32 v8;
    unsigned short v9;
    short v10;
    sint32 v11;
    short v12;
    sint32 v13;
    sint8 v14;
    sint32 v15;
    short v16;
    sint32 v17;
    short v18;
    sint32 v19;
    sint32 v20;
    short v21;
    sint32 v22;
    short v23;
    sint32 v24;
    sint32 v25;
    sint32 result;
    uint32 own_y;
    mode = (uint16)mode;
    switch (mode)
    {
        case 0:
            sub_800626C8(object, 0x800FF524);
            w_u16(((uint32)((object + 390))), 1);
            w_u16(((uint32)((object + 218))), 20);
            w_u16(((uint32)((object + 456))), 130);
            w_u16(((uint32)((object + 458))), 64);
            w_u8(((uint32)((object + 613))), 10);
            sub_8004BD04(object, 2, 5);
            w_u16(((uint32)((object + 436))), 32);
            w_u16(((uint32)((object + 438))), 32);
            sub_8004BDCC(object, 0, 1, 20);
            sub_8004BDCC(object, 1, 1, 10);
            sub_8004BDCC(object, 1, 2, 64);
            sub_8004BDCC(object, 1, 3, 16);
            sub_8004BDCC(object, 1, 4, 45);
            v4 = object;
            goto LABEL_7;

        case 1:
            sub_800626C8(object, 0x800FF524);
            w_u16(((uint32)((object + 390))), 1);
            v5 = this->model.flags;
            this->model.scale[2] = 6144;
            this->model.scale[1] = 6144;
            this->model.scale[0] = 6144;
            this->model.flags = (v5 | 0x200);
            sub_800626F8(object, 0, 208, 96, 96);
            w_u16(((uint32)((object + 218))), 60);
            w_u16(((uint32)((object + 456))), 200);
            w_u16(((uint32)((object + 458))), 96);
            w_u8(((uint32)((object + 613))), 10);
            sub_8004BD04(object, 2, 5);
            w_u16(((uint32)((object + 436))), 32);
            w_u16(((uint32)((object + 438))), 32);
            sub_8004BDCC(object, 0, 1, 20);
            sub_8004BDCC(object, 1, 1, 10);
            sub_8004BDCC(object, 1, 2, 64);
            sub_8004BDCC(object, 1, 3, 32);
            sub_8004BDCC(object, 1, 4, 90);
            sub_8005171C(object, 2, 0, 4096, 256, 128);
            sub_8005171C(object, 4, 4064, 0x4000, 128, 128);
            sub_80062D70(object);
            v6 = ((unsigned char)(r_u8(0x800FF644)));
            v7 = r_u16(((uint32)((object + 456))));
            w_u16(((uint32)((object + 204))), 1024);
            w_u16(((uint32)((object + 200))), 96);
            goto LABEL_8;

        case 2:
            sub_800626C8(object, 0x800FF524);
            w_u16(((uint32)((object + 390))), 1);
            w_u16(((uint32)((object + 218))), 20);
            w_u16(((uint32)((object + 456))), 130);
            w_u16(((uint32)((object + 458))), 64);
            w_u8(((uint32)((object + 613))), 10);
            sub_8004BD04(object, 2, 5);
            w_u16(((uint32)((object + 436))), 32);
            w_u16(((uint32)((object + 438))), 32);
            sub_8004BDCC(object, 0, 1, 20);
            sub_8004BDCC(object, 1, 1, 10);
            sub_8004BDCC(object, 1, 2, 64);
            sub_8004BDCC(object, 1, 3, 16);
            sub_8004BDCC(object, 1, 4, 45);
            v8 = ((uint32)(sub_8002FED8(124)));
            if (v8)
                v8 = sub_80023B2C(v8, 0x800FF5A0, 0);
            w_u32(((uint32)((object + 620))), v8);
            v9 = sub_8004BDFC(object, 1, 1);
            v4 = object;
            w_u32(((uint32)((r_u32(((uint32)((object + 620)))) + 40))), v9);
        LABEL_7:
            sub_8005171C(v4, 0, 128, 192, 256, 0);

            sub_8005171C(object, 1, 0, 512, 128, 16);
            sub_8005171C(object, 2, 512, 4096, 256, 0);
            sub_8005171C(object, 3, 1024, 12288, 128, 96);
            sub_8005171C(object, 4, 2048, 12288, 128, 256);
            sub_8005171C(object, 5, 1024, 12288, 64, 256);
            sub_80062D70(object);
            v6 = ((unsigned char)(r_u8(0x800FF644)));
            v7 = r_u16(((uint32)((object + 456))));
            w_u16(((uint32)((object + 204))), 1024);
            w_u16(((uint32)((object + 200))), 64);
        LABEL_8:
            w_u16(((uint32)((object + 202))), v7);

            w_u8(((uint32)((object + 614))), ::FindModelResourceIndex(-617804721, v6));
            w_u8(((uint32)((object + 615))), 10);
            goto LABEL_36;

        case 16:
            sub_800626C8(object, 0x800A2754);
            w_u16(((uint32)((object + 390))), 2);
            w_u16(((uint32)((object + 218))), 1);
            w_u16(((uint32)((object + 456))), 112);
            this->model.scale[0] = 7000;
            this->model.scale[1] = 5000;
            this->model.scale[2] = 0x2000;
            v10 = this->model.flags;
            w_u16(((uint32)((object + 458))), 32);
            w_u8(((uint32)((object + 613))), 15);
            this->model.flags = (v10 | 0x200);
            sub_8004BD04(object, 2, 5);
            w_u16(((uint32)((object + 436))), 16);
            w_u16(((uint32)((object + 438))), 32);
            sub_8004BDCC(object, 0, 1, 5);
            sub_8005171C(object, 0, 128, 192, 256, 0);
            sub_8005171C(object, 1, 0, 1024, 128, 64);
            goto LABEL_28;

        case 32:

        case 33:

        case 34:

        case 35:
            sub_800626C8(object, 0x800FF52C);
            w_u16(((uint32)((object + 390))), 4);
            w_u16(((uint32)((object + 456))), 128);
            w_u16(((uint32)((object + 458))), 128);
            w_u8(((uint32)((object + 613))), 15);
            sub_8004BD04(object, 2, 5);
            sub_80062D70(object);
            v11 = ((unsigned char)(r_u8(0x800FF644)));
            v12 = r_u16(((uint32)((object + 456))));
            w_u16(((uint32)((object + 204))), 1024);
            w_u16(((uint32)((object + 200))), 96);
            w_u16(((uint32)((object + 202))), v12);
            w_u8(((uint32)((object + 614))), ::FindModelResourceIndex(-317030919, v11));
            w_u8(((uint32)((object + 615))), 10);
            switch (mode)
            {
                case ' ':
                    w_u16(((uint32)((object + 218))), 30);
                    w_u16(((uint32)((object + 436))), 32);
                    w_u16(((uint32)((object + 438))), 32);
                    sub_8004BDCC(object, 0, 1, 10);
                    sub_8004BDCC(object, 1, 1, 5);
                    sub_8004BDCC(object, 1, 2, 48);
                    sub_8004BDCC(object, 1, 3, 16);
                    sub_8004BDCC(object, 1, 4, 45);
                    sub_8005171C(object, 0, 128, 192, 256, 0);
                    sub_8005171C(object, 1, 0, 1024, 128, 64);
                    sub_8005171C(object, 2, 512, 4096, 256, 64);
                    sub_8005171C(object, 3, 1024, 0x2000, 128, 96);
                    sub_8005171C(object, 4, 2048, 0x2000, 64, 128);
                    sub_8005171C(object, 5, 1024, 0x2000, 64, 256);
                    v13 = (object + 4);
                    goto LABEL_37;

                case '!':
                    w_u16(((uint32)((object + 218))), 30);
                    w_u16(((uint32)((object + 436))), 32);
                    w_u16(((uint32)((object + 438))), 32);
                    sub_8004BDCC(object, 0, 1, 10);
                    sub_8004BDCC(object, 1, 1, 5);
                    sub_8004BDCC(object, 1, 2, 48);
                    sub_8004BDCC(object, 1, 3, 16);
                    sub_8004BDCC(object, 1, 4, 45);
                    sub_8005171C(object, 0, 128, 256, 256, 0);
                    sub_8005171C(object, 2, 232, 4096, 256, 0);
                    sub_8005171C(object, 4, 2048, 12288, 128, 256);
                    break;

                case '"':
                    w_u16(((uint32)((object + 218))), 30);
                    w_u16(((uint32)((object + 436))), 32);
                    w_u16(((uint32)((object + 438))), 32);
                    sub_8004BDCC(object, 0, 1, 10);
                    sub_8004BDCC(object, 1, 1, 5);
                    sub_8004BDCC(object, 1, 2, 48);
                    sub_8004BDCC(object, 1, 3, 16);
                    sub_8004BDCC(object, 1, 4, 45);
                    sub_8005171C(object, 0, 128, 192, 256, 0);
                    sub_8005171C(object, 1, 0, 4096, 0, 128);
                    sub_8005171C(object, 4, 3072, 12288, 32, 256);
                    break;

                default:
                    v13 = (object + 4);
                    if ((mode != 35))
                        goto LABEL_37;
                    w_u16(((uint32)((object + 218))), 30);
                    sub_8004BDCC(object, 1, 1, 5);
                    sub_8004BDCC(object, 1, 2, 48);
                    sub_8004BDCC(object, 1, 3, 32);
                    sub_8004BDCC(object, 1, 4, 45);
                    sub_8005171C(object, 2, 0, 4096, 256, 0);
                    sub_8005171C(object, 4, 4096, 0x4000, 256, 256);
                    break;
            }

            v13 = (object + 4);
        LABEL_37:
            v24 = sub_80067A18(v13, 64, 512);

            if ((v24 == -1))
                return sub_80062D84(object);
            own_y = this->model.pos[1];
            v25 = (sint16)r_u16(object + 456);
            result = (sint32)((uint32)v25 << 12);
            if (((sint32)((uint32)v24 - own_y) >> 12) < (sint32)((uint32)v25 << 1))
            {
                result = (sint32)((uint32)v24 - (uint32)result);
                this->model.pos[1] = result;
            }
            return result;

        case 48:

        case 49:

        case 50:
            sub_800626C8(object, 0x800FF534);
            w_u16(((uint32)((object + 390))), 8);
            w_u16(((uint32)((object + 218))), 10);
            w_u16(((uint32)((object + 456))), 140);
            w_u16(((uint32)((object + 458))), 32);
            w_u8(((uint32)((object + 613))), 15);
            sub_8004BD04(object, 2, 5);
            w_u16(((uint32)((object + 436))), 16);
            w_u16(((uint32)((object + 438))), 32);
            sub_8004BDCC(object, 0, 1, 5);
            sub_8004BDCC(object, 1, 1, 10);
            sub_8004BDCC(object, 1, 3, 32);
            switch (mode)
            {
                case '0':
                    sub_8005171C(object, 0, 128, 192, 256, 0);
                    sub_8005171C(object, 1, 0, 1024, 128, 64);
                    sub_8005171C(object, 2, 512, 4096, 256, 0);
                    sub_8005171C(object, 3, 1024, 12288, 128, 96);
                    sub_8005171C(object, 4, 2048, 12288, 128, 256);
                    sub_8005171C(object, 5, 1024, 12288, 64, 256);
                    break;

                case '1':
                    sub_8005171C(object, 0, 128, 192, 256, 0);
                    sub_8005171C(object, 2, 512, 4096, 256, 0);
                    sub_8005171C(object, 4, 2048, 12288, 128, 256);
                    break;

                case '2':
                    sub_8005171C(object, 0, 128, 192, 256, 0);
                    sub_8005171C(object, 1, 0, 0x2000, 128, 64);
                    sub_8005171C(object, 4, 4096, 12288, 128, 256);
                    sub_8005171C(object, 5, 2048, 12288, 64, 256);
                    break;
            }

            w_u8(((uint32)((object + 614))), ::FindModelResourceIndex(808338210, ((unsigned char)(r_u8(0x800FF644)))));
            v14 = 15;
            goto LABEL_35;

        case 64:
            sub_800626C8(object, 0x800FF53C);
            w_u16(((uint32)((object + 218))), 30);
            w_u16(((uint32)((object + 456))), 120);
            w_u16(((uint32)((object + 390))), 16);
            w_u16(((uint32)((object + 458))), 32);
            w_u8(((uint32)((object + 613))), 13);
            sub_8004BD04(object, 2, 5);
            w_u16(((uint32)((object + 436))), 29);
            w_u16(((uint32)((object + 438))), 32);
            sub_8004BDCC(object, 0, 1, 5);
            sub_8004BDCC(object, 1, 1, 17);
            sub_8004BDCC(object, 1, 3, 32);
            sub_8005171C(object, 0, 128, 192, 256, 0);
            sub_8005171C(object, 1, 0, 1024, 128, 64);
            sub_8005171C(object, 2, 512, 4096, 256, 0);
            sub_8005171C(object, 3, 1024, 12288, 128, 96);
        LABEL_28:
            sub_8005171C(object, 4, 2048, 12288, 128, 256);

            sub_8005171C(object, 5, 1024, 12288, 64, 256);
            sub_80062D70(object);
            v15 = ((unsigned char)(r_u8(0x800FF644)));
            v16 = r_u16(((uint32)((object + 456))));
            w_u16(((uint32)((object + 204))), 1024);
            w_u16(((uint32)((object + 200))), 64);
            w_u16(((uint32)((object + 202))), v16);
            w_u8(((uint32)((object + 614))), ::FindModelResourceIndex(1020334405, v15));
            w_u8(((uint32)((object + 615))), 16);
            goto LABEL_36;

        case 96:
            sub_800626C8(object, 0x800FF544);
            w_u16(((uint32)((object + 218))), 100);
            w_u16(((uint32)((object + 456))), 116);
            w_u16(((uint32)((object + 390))), 64);
            w_u16(((uint32)((object + 458))), 64);
            w_u8(((uint32)((object + 613))), 10);
            sub_8004BD04(object, 2, 5);
            w_u16(((uint32)((object + 436))), 48);
            w_u16(((uint32)((object + 438))), 32);
            sub_8004BDCC(object, 0, 1, 20);
            sub_8004BDCC(object, 1, 1, 10);
            sub_8004BDCC(object, 1, 2, 64);
            sub_8004BDCC(object, 1, 3, 64);
            sub_8004BDCC(object, 1, 4, 45);
            sub_8005171C(object, 0, 128, 192, 256, 0);
            sub_8005171C(object, 1, 0, 512, 128, 16);
            sub_8005171C(object, 2, 512, 4096, 256, 0);
            sub_8005171C(object, 3, 1024, 12288, 128, 96);
            sub_8005171C(object, 4, 3072, 12288, 128, 256);
            sub_8005171C(object, 5, 2048, 12288, 64, 256);
            sub_80062D70(object);
            v17 = ((unsigned char)(r_u8(0x800FF644)));
            v18 = r_u16(((uint32)((object + 456))));
            w_u16(((uint32)((object + 204))), 1024);
            w_u16(((uint32)((object + 200))), 64);
            w_u16(((uint32)((object + 202))), v18);
            w_u8(((uint32)((object + 614))), ::FindModelResourceIndex(-87017481, v17));
            v14 = 14;
            goto LABEL_35;

        case 112:
            sub_800626C8(object, 0x800FF54C);
            v19 = r_u32(0x800FF4E4);
            w_u16(((uint32)((object + 390))), 128);
            w_u16(((uint32)((object + 218))), 40);
            w_u16(((uint32)((object + 456))), 116);
            w_u16(((uint32)((object + 458))), 40);
            w_u32(0x800FF4E4, (uint32)v19 + 1);
            sub_8004BD04(object, 2, 5);
            w_u16(((uint32)((object + 436))), 24);
            w_u16(((uint32)((object + 438))), 32);
            sub_8004BDCC(object, 0, 1, 5);
            sub_8005171C(object, 1, 64, 4096, 128, 64);
            sub_8005171C(object, 4, 0, 64, 256, 8);
            sub_8005171C(object, 5, 256, 12288, 64, 256);
            sub_80062D70(object);
            v20 = ((unsigned char)(r_u8(0x800FF644)));
            v21 = r_u16(((uint32)((object + 456))));
            w_u16(((uint32)((object + 204))), 1024);
            w_u16(((uint32)((object + 200))), 64);
            w_u16(((uint32)((object + 202))), v21);
            w_u8(((uint32)((object + 614))), ::FindModelResourceIndex(1020334405, v20));
            v14 = 16;
            goto LABEL_35;

        case 128:

        case 129:
            sub_800626C8(object, 0x800FF554);
            w_u16(((uint32)((object + 390))), 256);
            w_u16(((uint32)((object + 456))), 180);
            w_u16(((uint32)((object + 458))), 64);
            w_u8(((uint32)((object + 613))), 19);
            sub_8004BD04(object, 2, 5);
            w_u16(((uint32)((object + 218))), 30);
            w_u16(((uint32)((object + 436))), 32);
            w_u16(((uint32)((object + 438))), 32);
            sub_8004BDCC(object, 0, 1, 10);
            sub_8004BDCC(object, 1, 1, 10);
            sub_8004BDCC(object, 1, 2, 96);
            sub_8004BDCC(object, 1, 3, 16);
            sub_8004BDCC(object, 1, 4, 45);
            sub_8005171C(object, 0, 128, 192, 256, 0);
            if ((mode == 128))
            {
                sub_8005171C(object, 1, 0, 1024, 128, 64);
                sub_8005171C(object, 2, 512, 4096, 256, 64);
                sub_8005171C(object, 3, 1024, 0x2000, 128, 96);
                sub_8005171C(object, 4, 2048, 0x2000, 64, 128);
                sub_8005171C(object, 5, 1024, 0x2000, 64, 256);
            }
            else
            {
                sub_8005171C(object, 2, 512, 4096, 256, 64);
                sub_8005171C(object, 4, 2048, 0x2000, 64, 128);
            }
            sub_80062D70(object);
            v22 = ((unsigned char)(r_u8(0x800FF644)));
            v23 = r_u16(((uint32)((object + 456))));
            w_u16(((uint32)((object + 204))), 1024);
            w_u16(((uint32)((object + 200))), 64);
            w_u16(((uint32)((object + 202))), v23);
            w_u8(((uint32)((object + 614))), ::FindModelResourceIndex(873792324, v22));
            v14 = 12;
        LABEL_35:
            w_u8(((uint32)((object + 615))), v14);

            goto LABEL_36;

        default:
        LABEL_36:
            v13 = (object + 4);

            goto LABEL_37;
    }
}

uint32 Swat::ExecuteCommand(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x800529AC, "SLUS_003.73");
    uint32 position[3], shift = 12, target;
    command = (uint16)command;
    switch (command)
    {
        case 16902:
            SetMode(ReadCommand());
            return 1;
        case 16928:
        case 16931:
        case 16930:
        case 16933:
        {
            uint32 cursor = (r_u32(object + 400) + 3) & ~3;
            for (uint32 i = 0; i < 3; ++i)
                position[i] = r_u32(cursor + i * 4);
            ShiftVectorLeft(position, &shift);
            if (command == 16930 || command == 16933)
                AddVectors(position, (const uint32 *)this->model.pos);
            w_u32(object + 400, cursor + 12);
            target = command == 16928 || command == 16930 ? 0x80053784 : 0x80053834;
            ApplyCommandPosition(target, position);
            return 1;
        }
        case 16929:
        case 16932:
        case 16936:
        {
            uint32 value = ResolveCommandValue(ReadCommand());
            ReadRecordPosition(position, value);
            sint32 height = QueryGround(position, 0, 2048);
            if (height != -1)
                position[1] = (uint32)height - ((uint32)(sint16)r_u16(object + 456) << 12);
            target = command == 16929 ? 0x80053784 : command == 16932 ? 0x80053834 : 0x8005389C;
            ApplyCommandPosition(target, position);
            return 1;
        }
        case 16938:
        case 16939:
            SetAnimationRange(command == 16938 ? 7 : 8, 0, 0xFFFFFFFF);
            w_u16(object + 212, command == 16938 ? 32 : 128);
            {
                uint32 flags = r_u32(object + 396);
                uint32 state = this->lifecycle_flags;
                w_u32(object + 396, flags & ~1);
                this->lifecycle_flags = command == 16938 ? state & ~0x10 : state | 0x10;
            }
            motion.velocity[2] = 0;
            motion.velocity[1] = 0;
            motion.velocity[0] = 0;
            w_u16(object + 476, command == 16938 ? 12 : 14);
            return 0;
        case 16937:
        {
            uint32 value = ResolveCommandValue(ReadCommand());
            for (uint32 node = visual_lists.solids; node; node = r_u32(node + 28))
                if (r_u16(node + 214) == value)
                {
                    AttachTurret(node);
                    break;
                }
            return 1;
        }
        case 16994:
            if (ReadCommand() == 1)
                BeginAttack();
            return 1;
        case 17415:
        {
            uint32 value = ReadCommand();
            if (value)
                SetTargetRecord(ResolveCommandValue(value));
            else
                ClearTarget();
            return 1;
        }
        case 18176:
            if (ReadCommand())
                w_u32(object + 396, r_u32(object + 396) | 0x100);
            else
                w_u32(object + 396, r_u32(object + 396) & ~0x100);
            return 1;
        case 18177:
            w_u32(object + 396, r_u32(object + 396) | 0x200);
            return 1;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 Swat::DetachTurret()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80052EA8, "SLUS_003.73");
    uint32 receiver = r_u32(object + 616);
    return receiver && r_u16(receiver + 58) == 206 ? Turret::Attach(receiver).DetachAndDestroy() : 206;
}

uint32 Swat::AttachTurret(uint32 linked)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80052EE8, "SLUS_003.73");
    w_u32(object + 616, linked);
    if (r_u16(linked + 58) == 206)
    {
        Turret::Attach(linked).AttachOperator(object);
        w_u16(object + 472, 12);
        w_u16(object + 474, 0);
        return 12;
    }
    return 206;
}

uint32 Swat::BeginAttack()
{
    FUNCTION_MARKER(0x80052F34, "SLUS_003.73");
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 kind = r_u16(object + 390);
    switch (kind)
    {
        case 1:
        case 4:
        case 256:
            return FireProjectile();
        case 8:
        case 16:
        case 64:
            return SetAnimationRange(1, 0, 0xFFFFFFFF);
        default:
            return kind < 9 ? 4 : kind < 65 ? 16 : 256;
    }
}

uint32 Swat::Fire()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 mode, muzzle[3], velocity[3], predicted[3], aim[3], temporary[3];
    uint32 child, random, speed, damage, index, player, scale, distance;
    FUNCTION_MARKER(0x80052FCC, "SLUS_003.73");
    mode = r_u16(object + 390);
    if (mode == 16)
    {
        GetModelAttachment(muzzle, object, 0);
        child = AllocateZeroedObject(400);
        if (child)
        {
            random = RandomRange(8);
            speed = r_u32(object + 220);
            speed = DivideUnsigned(speed, random + 24);
            index = GetBehaviorValue( 1, 1) & 65535;
            damage = ScaleDamageForDifficulty(index);
            player = visual_lists.players;
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(LaunchedProjectile));
            new (dram_addr) LaunchedProjectile(muzzle, (const uint32 *)psx_addr(player + 4, 12), speed, damage, 512, &visual_lists.players, 0, 1, 0);
        }
        return LaunchedProjectile::Attach(child).SetGlowColor( 128, 128, 0);
    }
    if (mode == 64)
    {
        index = (this->model.frame & 4) ? 1 : 0;
        GetModelAttachment(muzzle, object, index);
        child = AllocateZeroedObject(428);
        if (child)
        {
            index = GetBehaviorValue( 1, 1) & 65535;
            damage = ScaleDamageForDifficulty(index);
            void *dram_addr = AllocateNativeObjectStorage(child, sizeof(GuidedProjectile));
            new (dram_addr) GuidedProjectile(muzzle, damage, &visual_lists.players, 0);
        }
        GuidedProjectile::Attach(child).SetAngles((const uint16 *)this->model.angles);
        player = visual_lists.players;
        GuidedProjectile::Attach(child).Launch(0, (const uint32 *)psx_addr(player + 184, 12));
        return GuidedProjectile::Attach(child).SetSize(2048);
    }
    if (mode != 8)
        return 8;
    random = RandomRange(16);
    player = visual_lists.players;
    scale = random + 24;
    if (!player)
        return scale;
    GetModelAttachment(muzzle, object, 0);
    random = RandomRange(3);
    if (random)
    {
        player = visual_lists.players;
        velocity[0] = VisualObject::Attach(player).motion.velocity[0];
        velocity[1] = VisualObject::Attach(player).motion.velocity[1];
        velocity[2] = VisualObject::Attach(player).motion.velocity[2];
        DampVector(velocity, (const uint8 *)VisualObject::Attach(player).motion.drag);
        MultiplyVector(predicted, velocity, &scale);
        player = visual_lists.players;
        AddVectors(temporary, (const uint32 *)psx_addr(player + 4, 12), predicted);
        player = visual_lists.players;
        aim[0] = temporary[0];
        aim[1] = temporary[1];
        aim[2] = temporary[2];
        aim[1] = r_u32(player + 8);
        distance = MeasureEuclideanDistance(muzzle, aim);
        random = RandomRange(12);
        speed = DivideUnsigned(distance, random + 16);
    }
    else
    {
        player = visual_lists.players;
        aim[0] = r_u32(player + 4);
        aim[1] = r_u32(player + 8);
        aim[2] = r_u32(player + 12);
        random = RandomRange(8);
        speed = r_u32(object + 220);
        speed = DivideUnsigned(speed, random + 24);
    }
    child = AllocateZeroedObject(400);
    if (child)
    {
        index = GetBehaviorValue( 1, 1) & 65535;
        damage = ScaleDamageForDifficulty(index);
        void *dram_addr = AllocateNativeObjectStorage(child, sizeof(LaunchedProjectile));
        new (dram_addr) LaunchedProjectile(muzzle, aim, speed, damage, 512, &visual_lists.players, 0, 1, 0);
    }
    LaunchedProjectile::Attach(child).SetGlowColor( 32, 128, 32);
    return PlayPositionedSound(68, (const uint32 *)this->model.pos, 0);
}

uint32 Swat::FireProjectile()
{
    FUNCTION_MARKER(0x80053358, "SLUS_003.73");
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 player, target_x, own_x, target_z, own_z, mode, projectile;
    uint32 damage, parameter, speed, behavior, table, receiver, target;
    sint32 position[3], aim[3];
    sint16 angles[3];
    if (r_u32(object + 396) & 64)
    {
        target_z = r_u32(object + 504);
        own_z = this->model.pos[2];
        target_x = r_u32(object + 496);
        own_x = this->model.pos[0];
        this->model.angles[1] = 3072 - ratan2(target_z - own_z, target_x - own_x);
        SetAnimationRange(1, 0, 0xFFFFFFFF);
        GetModelAttachment((uint32 *)position, object, 0);
        CalculateDirectionAngles((uint16 *)angles, (const uint32 *)position, (const uint32 *)psx_addr(object + 496, 12));
    }
    else
    {
        player = visual_lists.players;
        if (!player)
            return 0;
        target_z = r_u32(player + 12);
        own_z = this->model.pos[2];
        target_x = r_u32(player + 4);
        own_x = this->model.pos[0];
        this->model.angles[1] = 3072 - ratan2(target_z - own_z, target_x - own_x);
        SetAnimationRange(1, 0, 0xFFFFFFFF);
        GetModelAttachment((uint32 *)position, object, 0);
        player = visual_lists.players;
        GetCharacterAttachment((uint32 *)aim, player, 9);
        aim[1] = (sint32)((uint32)aim[1] + 0x10000);
        CalculateDirectionAngles((uint16 *)angles, (const uint32 *)position, (const uint32 *)aim);
    }
    mode = r_u16(object + 390);
    if (mode == 4 || mode == 256)
    {
        projectile = AllocateEffectStorage(108);
        if (projectile)
        {
            parameter = GetBehaviorValue( 1, 1) & 65535;
            damage = ScaleDamageForDifficulty(parameter);
            parameter = GetBehaviorValue( 1, 4) & 65535;
            speed = GetBehaviorValue( 1, 2) & 65535;
            if (mode == 4)
                {
                void *dram_addr = AllocateNativeObjectStorage(projectile, sizeof(OscillatingProjectile));
                new (dram_addr) OscillatingProjectile((const uint32 *)position, angles, damage, parameter, speed, 16, 240, 240, 240, 32, 128, 64);
            }
            else
                {
                void *dram_addr = AllocateNativeObjectStorage(projectile, sizeof(OscillatingProjectile));
                new (dram_addr) OscillatingProjectile((const uint32 *)position, angles, damage, parameter, speed, 16, 240, 240, 240, 128, 32, 32);
            }
        }
        if (mode == 256)
        {
            GetModelAttachment((uint32 *)position, object, 1);
            projectile = AllocateEffectStorage(108);
            if (projectile)
            {
                parameter = GetBehaviorValue( 1, 1) & 65535;
                damage = ScaleDamageForDifficulty(parameter);
                parameter = GetBehaviorValue( 1, 4) & 65535;
                speed = GetBehaviorValue( 1, 2) & 65535;
                {
                void *dram_addr = AllocateNativeObjectStorage(projectile, sizeof(OscillatingProjectile));
                new (dram_addr) OscillatingProjectile((const uint32 *)position, angles, damage, parameter, speed, 16, 240, 240, 240, 128, 32, 32);
            }
            }
        }
    }
    else
    {
        behavior = r_u32(object + 620);
        if (behavior)
        {
            table = r_u32(behavior);
            receiver = behavior + (uint32)(sint32)(sint16)r_u16(table + 16);
            target = r_u32(table + 20);
            switch (target)
            {
                case 0x8002BCA8: WeaponNode::Attach(receiver).WeaponNode::Fire((const uint32 *)position, angles); break;
                case 0x80022C90: MachineGun::Attach(receiver).MachineGun::Fire((const uint32 *)position, angles); break;
                case 0x80023C14: SpiralRayWeapon::Attach(receiver).SpiralRayWeapon::Fire((const uint32 *)position, angles); break;
                case 0x80024598: Flamethrower::Attach(receiver).Flamethrower::Fire((const uint32 *)position, angles); break;
                case 0x8002510C: HomingMissiles::Attach(receiver).HomingMissiles::Fire((const uint32 *)position, angles); break;
                case 0x80025380: PulseLaser::Attach(receiver).PulseLaser::Fire((const uint32 *)position, angles); break;
                case 0x80027978: ParticleBeam::Attach(receiver).ParticleBeam::Fire((const uint32 *)position, angles); break;
                case 0x800286F4: RipLaser::Attach(receiver).RipLaser::Fire((const uint32 *)position, angles); break;
                case 0x8002A65C: Grenades::Attach(receiver).Grenades::Fire((const uint32 *)position, angles); break;
                case 0x8002B588: Rockets::Attach(receiver).Rockets::Fire((const uint32 *)position, angles); break;
                default:
                    // TODO Recover any additional original weapon Fire target
                    GameDiagnostic("Missing list-node attack 80053358 target %08X receiver %08X\n", target, receiver);
                    abort();
            }
        }
        else
        {
            projectile = AllocateEffectStorage(108);
            if (projectile)
            {
                parameter = GetBehaviorValue( 1, 1) & 65535;
                damage = ScaleDamageForDifficulty(parameter);
                parameter = GetBehaviorValue( 1, 4) & 65535;
                speed = GetBehaviorValue( 1, 2) & 65535;
                {
                void *dram_addr = AllocateNativeObjectStorage(projectile, sizeof(OscillatingProjectile));
                new (dram_addr) OscillatingProjectile((const uint32 *)position, angles, damage, parameter, speed, 16, 240, 240, 240, 64, 32, 128);
            }
            }
        }
    }
    return PlayPositionedSound(20, (const uint32 *)this->model.pos, 2);
}

Swat &Swat::Attach(uint32 object)
{
    return *static_cast<Swat *>(FindNativeObjectStorage(object));
}

// Native input bridge for the original vector loads
uint32 Swat::SetMoveTarget(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_position = GuestMemoryAddress(position);
    FUNCTION_MARKER(0x80053784, "SLUS_003.73");
    uint32 values[3], result;
    if (this->model.animation != 2)
        sub_80063118(object, 2, 1);
    if (r_u16(object + 390) == 128 && (r_u32(object + 372) & 9) && this->model.animation != 1)
        sub_80063118(object, 1, 1);
    if (guest_position)
    {
        values[0] = r_u32(guest_position);
        values[1] = r_u32(guest_position + 4);
        values[2] = r_u32(guest_position + 8);
    }
    else
        memcpy(values, position, sizeof(values));
    w_u32(object + 484, values[0]);
    w_u32(object + 488, values[1]);
    w_u32(object + 492, values[2]);
    result = (r_u32(object + 396) | 1) & 0xFFFFFFC7;
    w_u32(object + 396, result);
    return result;
}

uint32 Swat::SetJumpTarget(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_position = GuestMemoryAddress(position);
    FUNCTION_MARKER(0x80053834, "SLUS_003.73");
    uint32 values[3], result, state;
    sub_80063038(object, 4, 0, 0xFFFFFFFF);
    if (guest_position)
    {
        values[0] = r_u32(guest_position);
        values[1] = r_u32(guest_position + 4);
        values[2] = r_u32(guest_position + 8);
    }
    else
        memcpy(values, position, sizeof(values));
    w_u32(object + 484, values[0]);
    w_u32(object + 488, values[1]);
    w_u32(object + 492, values[2]);
    result = r_u32(object + 396) | 9;
    state = r_u8(object + 438);
    w_u8(object + 612, state);
    w_u32(object + 396, result);
    return result;
}

uint32 Swat::SetSpecialTarget(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 guest_position = GuestMemoryAddress(position);
    FUNCTION_MARKER(0x8005389C, "SLUS_003.73");
    uint32 values[3], result;
    sub_80063038(object, 4, 0, 0xFFFFFFFF);
    if (guest_position)
    {
        values[0] = r_u32(guest_position);
        values[1] = r_u32(guest_position + 4);
        values[2] = r_u32(guest_position + 8);
    }
    else
        memcpy(values, position, sizeof(values));
    w_u32(object + 484, values[0]);
    w_u32(object + 488, values[1]);
    w_u32(object + 492, values[2]);
    result = r_u32(object + 396);
    w_u8(object + 382, 1);
    result |= 0x11;
    w_u32(object + 396, result);
    return result;
}

uint32 Swat::SetTargetRecord(uint32 index)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x80053950, "SLUS_003.73");
    uint32 result;
    sub_8006613C((object + 496), (uint16)index);
    result = (r_u32(((uint32)((object + 396)))) | 0x40);
    w_u32(((uint32)((object + 396))), result);
    return result;
}

uint32 Swat::ClearTarget()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8005398C, "SLUS_003.73");
    uint32 result;
    result = (r_u32(((uint32)((object + 396)))) & 0xFFFFFF3F);
    w_u32(((uint32)((object + 396))), result);
    return result;
}

uint32 Swat::ChooseBehavior()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cumulative[6];
    uint32 distance, total = 0, index, record, minimum, maximum;
    uint32 start_weight, end_weight, weight, product, denominator, random;
    FUNCTION_MARKER(0x800539A8, "SLUS_003.73");
    distance = sub_8006689C(object + 4, visual_lists.players + 4);
    for (index = 0; index < 6; ++index)
    {
        record = object + 16 * index;
        minimum = r_u32(record + 512);
        weight = 0;
        if (minimum < distance)
        {
            maximum = r_u32(record + 520);
            if (distance >= maximum)
                weight = r_u32(record + 524);
            else
            {
                end_weight = r_u32(record + 524);
                start_weight = r_u32(record + 516);
                product = (end_weight - start_weight) * (distance - minimum);
                denominator = maximum - minimum;
                weight = (uint32)Divide((sint32)product, denominator) + start_weight;
            }
        }
        total += weight;
        cumulative[index] = total;
    }
    random = RandomRange(total);
    for (index = 0; index < 6; ++index)
        if ((sint32)random < (sint32)cumulative[index])
            return index;
    return 4;
}

uint32 Swat::ReadVector(const sint32 *vector, uint32 index)
{
    uint32 guest = GuestMemoryAddress(vector), value;
    if (guest || !vector)
        return r_u32(guest + 4 * index);
    memcpy(&value, (const unsigned char *)vector + 4 * index, 4);
    return value;
}

void Swat::SetDeathState(uint32 state)
{
    const uint32 object = NativeObjectGuestAddress(this);
    w_u16(object + 472, state);
    w_u16(object + 474, 0);
}

sint32 Swat::Divide(sint32 numerator, uint32 divisor)
{
    if (!divisor)
        return numerator < 0 ? 1 : -1;
    if ((uint32)numerator == 0x80000000 && divisor == 0xFFFFFFFF)
        return numerator;
    return numerator / (sint32)divisor;
}

uint32 Swat::DivideUnsigned(uint32 numerator, uint32 denominator)
{
    return denominator ? numerator / denominator : 0xFFFFFFFF;
}

sint32 Swat::Abs32(sint32 value)
{
    return value < 0 ? (sint32)(0 - (uint32)value) : value;
}

void Swat::MissingBehavior(uint32 service, uint32 receiver, uint32 arg2, uint32 arg3, uint32 arg4)
{
    /* TODO Implement the named excluded service or native callback boundary */
    GameDiagnostic("Missing behavior boundary %08X receiver %08X args %08X %08X %08X\n", service, receiver, arg2, arg3, arg4);
    abort();
}

void Swat::SetEffectDirection(const sint16 angles[3])
{
    (void)SetParticleDirection(angles);
}

void Swat::SetEffectAngles(const sint16 angles[3])
{
    (void)apocalypse_effect_angles_native(angles);
}

uint32 Swat::ReadCommand()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor = r_u32(object + 400);
    uint32 value = r_u16(cursor);
    w_u32(object + 400, cursor + 2);
    return value;
}

uint32 Swat::ResolveCommandValue(uint32 value)
{
    const uint32 object = NativeObjectGuestAddress(this);
    if (value & 0x2000)
    {
        uint32 table = r_u32(object + 68);
        uint32 receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80);
        uint32 target = r_u32(table + 84);
        value = DispatchObjectParameterGetter(target, receiver, value);
    }
    return (uint16)value;
}

sint32 Swat::QueryGround(const uint32 position[3], uint32 below, uint32 above)
{
    return (sint32)apocalypse_ground_native(position, below, above);
}

void Swat::ApplyCommandPosition(uint32 target, const uint32 position[3])
{
    const uint32 object = NativeObjectGuestAddress(this);
    switch (target)
    {
        case 0x80053784:
            SetMoveTarget(position);
            return;
        case 0x80053834:
            SetJumpTarget(position);
            return;
        case 0x8005389C:
            SetSpecialTarget(position);
            return;
        default:
            abort();
    }
}

uint32 sub_8004E35C(uint32 object, uint32 data, uint32 index)
{
    FUNCTION_MARKER(0x8004E35C, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Swat));
    new (native_instance_storage) Swat((const uint8 *)psx_addr(data, 1), index);
    return object;
}

uint32 sub_8004E3E4(uint32 object, uint32 position, uint32 script)
{
    FUNCTION_MARKER(0x8004E3E4, "SLUS_003.73");
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Swat));
    new (native_instance_storage) Swat((const uint32 *)psx_addr(position, 12), script);
    return object;
}

extern "C" uint32 apocalypse_swat_spawn_native(uint32 object, const uint32 position[3], uint32 script)
{
    void *native_instance_storage = AllocateNativeObjectStorage(object, sizeof(Swat));
    new (native_instance_storage) Swat(position, script);
    return object;
}

uint32 sub_8004E48C(uint32 object)
{
    FUNCTION_MARKER(0x8004E48C, "SLUS_003.73");
    return Swat::Attach(object).Init();
}

uint32 sub_8004E508(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8004E508, "SLUS_003.73");
    return Swat::Attach(object).Swat::Destroy(reason);
}

uint32 sub_8004E71C(uint32 object)
{
    FUNCTION_MARKER(0x8004E71C, "SLUS_003.73");
    return Swat::Attach(object).Swat::Update();
}

uint32 apocalypse_enemy_damage_4F5C8_native(uint32 object, uint32 damage, const sint32 vector[3], uint32 reason)
{
    return Swat::Attach(object).Swat::Damage(damage, vector, reason);
}

uint32 sub_8004F5C8(uint32 object, uint32 damage, uint32 vector, uint32 reason)
{
    FUNCTION_MARKER(0x8004F5C8, "SLUS_003.73");
    return Swat::Attach(object).Swat::Damage(damage, vector ? (const sint32 *)psx_addr(vector, 12) : 0, reason);
}

uint32 sub_8004F6C4(uint32 object)
{
    FUNCTION_MARKER(0x8004F6C4, "SLUS_003.73");
    return Swat::Attach(object).SpawnZombieFragment();
}

void sub_8004F840(uint32 object, uint32 vector, uint32 reason)
{
    FUNCTION_MARKER(0x8004F840, "SLUS_003.73");
    Swat::Attach(object).DeathReaction(vector ? (const sint32 *)psx_addr(vector, 12) : 0, reason);
}

void sub_8004FB00(uint32 object, uint32 mode)
{
    FUNCTION_MARKER(0x8004FB00, "SLUS_003.73");
    Swat::Attach(object).FragmentAndFinish(mode);
}

uint32 sub_8004FE20(uint32 object)
{
    FUNCTION_MARKER(0x8004FE20, "SLUS_003.73");
    return Swat::Attach(object).EnterDeathState13();
}

uint32 sub_8004FEC4(uint32 object)
{
    FUNCTION_MARKER(0x8004FEC4, "SLUS_003.73");
    return Swat::Attach(object).EnterDeathState14();
}

void sub_8004FF6C(uint32 object)
{
    FUNCTION_MARKER(0x8004FF6C, "SLUS_003.73");
    Swat::Attach(object).UpdateState();
}

void sub_8005171C(uint32 object, uint32 index, uint32 first, uint32 second, uint32 third, uint32 fourth)
{
    FUNCTION_MARKER(0x8005171C, "SLUS_003.73");
    Swat::Attach(object).SetBehaviorRange(index, first, second, third, fourth);
}

uint32 sub_80051740(uint32 object, uint32 mode)
{
    FUNCTION_MARKER(0x80051740, "SLUS_003.73");
    return Swat::Attach(object).SetMode(mode);
}

uint32 sub_800529AC(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x800529AC, "SLUS_003.73");
    return Swat::Attach(object).Swat::ExecuteCommand(command);
}

uint32 sub_80052EA8(uint32 object)
{
    FUNCTION_MARKER(0x80052EA8, "SLUS_003.73");
    return Swat::Attach(object).DetachTurret();
}

uint32 sub_80052EE8(uint32 object, uint32 linked)
{
    FUNCTION_MARKER(0x80052EE8, "SLUS_003.73");
    return Swat::Attach(object).AttachTurret(linked);
}

uint32 sub_80052F34(uint32 object)
{
    FUNCTION_MARKER(0x80052F34, "SLUS_003.73");
    return Swat::Attach(object).BeginAttack();
}

uint32 sub_80052FCC(uint32 object)
{
    FUNCTION_MARKER(0x80052FCC, "SLUS_003.73");
    return Swat::Attach(object).Fire();
}

uint32 sub_80053358(uint32 object)
{
    FUNCTION_MARKER(0x80053358, "SLUS_003.73");
    return Swat::Attach(object).FireProjectile();
}

uint32 apocalypse_move_target_native(uint32 object, const void *position) { return Swat::Attach(object).SetMoveTarget((const uint32 *)position); }

uint32 sub_80053784(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80053784, "SLUS_003.73");
    return Swat::Attach(object).SetMoveTarget((const uint32 *)psx_addr(position, 12));
}

uint32 apocalypse_jump_target_native(uint32 object, const void *position) { return Swat::Attach(object).SetJumpTarget((const uint32 *)position); }

uint32 sub_80053834(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x80053834, "SLUS_003.73");
    return Swat::Attach(object).SetJumpTarget((const uint32 *)psx_addr(position, 12));
}

uint32 apocalypse_special_target_native(uint32 object, const void *position) { return Swat::Attach(object).SetSpecialTarget((const uint32 *)position); }

uint32 sub_8005389C(uint32 object, uint32 position)
{
    FUNCTION_MARKER(0x8005389C, "SLUS_003.73");
    return Swat::Attach(object).SetSpecialTarget((const uint32 *)psx_addr(position, 12));
}

uint32 sub_80053950(uint32 object, uint32 index)
{
    FUNCTION_MARKER(0x80053950, "SLUS_003.73");
    return Swat::Attach(object).SetTargetRecord(index);
}

uint32 sub_8005398C(uint32 object)
{
    FUNCTION_MARKER(0x8005398C, "SLUS_003.73");
    return Swat::Attach(object).ClearTarget();
}

uint32 sub_800539A8(uint32 object)
{
    FUNCTION_MARKER(0x800539A8, "SLUS_003.73");
    return Swat::Attach(object).ChooseBehavior();
}

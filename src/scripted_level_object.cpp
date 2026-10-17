#include "visual_object.h"
#include "object_damage.h"
#include "game_sdk_bindings.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "scripted_level_object.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include "object_fragments.h"
#include "audio_session.h"
#include "game_random.h"
#include "native_object_storage.h"
#include <new>
#include <stdio.h>
#include <stdlib.h>

// Native DIV helpers preserve MIPS zero and signed-overflow cases

// Native bridge retains both aligned reads of the original LWL/LWR pair

// Native extraction of the original adjusted player damage callback

#include "model_resources.h"
#include "vector_arithmetic.h"
#include "object_parameters.h"
#include "record_data.h"
#include <string.h>

// Native extraction of repeated raw command halfword reads

// Native bridge for the original target-word loads

// Native callback adapter discards the original parameter-reader result

#include "object_collision_events.h"
#include "vector_distance.h"
#include "angle_math.h"
#include "model_render.h"
#include "record_scripts.h"
#include "model_projection.h"
#include "player.h"

static uint32 DivideScriptedLevelObjectMotion(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return (sint32)numerator < 0 ? 1 : (uint32)-1;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return numerator;
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static uint32 RemainderScriptedLevelObjectMotion(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        return numerator;
    if (numerator == 0x80000000 && denominator == (uint32)-1)
        return 0;
    return (uint32)((sint32)numerator % (sint32)denominator);
}

static uint32 ReadScriptedLevelObjectAngleWord(uint32 address)
{
    uint32 upper = r_u32((address + 3) & ~3);
    uint32 lower = r_u32(address & ~3);
    uint32 shift = (address & 3) * 8;
    return shift ? (upper << (32 - shift)) | (lower >> shift) : lower;
}

static void DamageScriptedLevelObjectPlayer(uint32 player)
{
    ApplyObjectDamage(player, 2, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
}

static uint32 ReadScriptedLevelObjectStreamHalf(uint32 object)
{
    uint32 cursor = r_u32(object + 400);
    uint32 value = r_u16(cursor);
    w_u32(object + 400, cursor + 2);
    return value;
}

static uint32 ReadScriptedLevelObjectTargetWord(const uint32 *position, uint32 axis)
{
    uint32 address = GuestMemoryAddress(position), value;
    if (address)
        return r_u32(address + axis * 4);
    memcpy(&value, (const uint8 *)position + axis * 4, 4);
    return value;
}

ScriptedLevelObject &ScriptedLevelObject::Attach(uint32 object)
{
    return *static_cast<ScriptedLevelObject *>(FindNativeObjectStorage(object));
}

void ScriptedLevelObject::SetParameters(uint32 command)
{
    SetScriptParameters(command);
}

ScriptedLevelObject::ScriptedLevelObject(const uint8 *data, uint32 index) : Enemy()
{
    const uint32 object = NativeObjectGuestAddress(this);
    const uint32 address = GuestMemoryAddress(data);
    uint32 bank, flags, cursor;
    if (!address)
    {
        // TODO Bind native record data to the existing record parser
        GameDiagnostic("TODO Record402 native record data\n");
        abort();
    }
    FUNCTION_MARKER(0x8003B964, "SLUS_003.73");
    bank = r_u32(0x800FF62C);
    w_u32(object + 68, 0x800A1DF0);
    sub_800626C8(object, bank);
    AddToList(visual_lists.solids);
    flags = this->model.flags;
    this->kind = 402;
    this->model.flags = (flags | 273) & 65533;
    cursor = sub_80062B0C(object, address);
    cursor = sub_80062B50(object, cursor);
    w_u32(object + 400, cursor);
    w_u32(object + 524, (uint32)-1);
    w_u16(object + 436, 32);
    this->record_idx = index;
    w_u8(object + 380, 1);
}

uint32 ScriptedLevelObject::Destroy(uint32 reason)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 sound;
    FUNCTION_MARKER(0x8003BAE0, "SLUS_003.73");
    w_u32(object + 68, 0x800A1DF0);
    RemoveFromList(visual_lists.solids);
    sound = r_u32(object + 520);
    if (sound)
    {
        StopSoundVoices(sound);
        w_u32(object + 520, 0);
    }
    Enemy::Destroy(0);
    return (reason & 1) ? ReleaseObjectStorage(object) : 0;
}

void ScriptedLevelObject::ApplyPlayerDamage()
{
    const uint32 object = NativeObjectGuestAddress(this);
    sint32 damage;
    uint32 player, receiver;
    FUNCTION_MARKER(0x8003BB60, "SLUS_003.73");
    damage = (sint16)r_u16(object + 508);
    if (!damage)
        return;
    player = visual_lists.players;
    if (damage < 0)
    {
        Player::Attach(player).RestoreHealth((uint32)(0 - damage));
        return;
    }
    receiver = player;
    ApplyObjectDamage(receiver, (uint32)damage, (const sint32 *)psx_addr(0x800A71CC, 12), 0);
    // TODO Classify callers that consume V0 on the original no-damage branch
}

uint32 ScriptedLevelObject::Update()
{
    SVECTOR motion_cur, motion_target;
    uint32 motion_cur_word, motion_cur_tail, motion_target_word, motion_target_tail;
    uint32 trig_angle;
    const uint32 a1 = NativeObjectGuestAddress(this);
    uint32 motion_distance;
    FUNCTION_MARKER(0x8003BBD8, "SLUS_003.73");
    sint32 v2;
    sint32 v3;
    short v4;
    short v5;
    short v6;
    sint32 v7;
    short v8;
    short v9;
    short v10;
    sint32 v11;
    short v12;
    short v13;
    short v14;
    sint32 v15;
    sint32 v16;
    short v17;
    sint8 v18;
    sint32 v19;
    short v20;
    sint32 v21;
    sint32 v22;
    sint32 v23;
    sint32 v24;
    unsigned short v27;
    sint32 v28;
    sint32 v29;
    sint32 v30;
    sint32 v31;
    sint32 v32;
    sint32 v33;
    sint32 v34;
    sint32 v35;
    sint32 v36;
    sint32 v37;
    sint32 v38;
    sint32 v39;
    unsigned short v42;
    sint32 v43;
    sint32 v44;
    sint32 v45;
    sint32 v46;
    sint32 v47;
    sint32 v48;
    sint32 v49;
    short v50;
    sint32 v51;
        sint32 v53;
    sint32 v54;
    sint32 v55;
    sint32 v56;
    sint32 result;
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
    sint32 direction[3];
    uint32 angles[2];

    union
    {
        uint32 words[4];
        sint16 halves[8];
    } temporary;

    uint32 v79[4];
    v2 = r_u16(((uint32)((a1 + 496))));
    v3 = (v2 == 0);
    v4 = (v2 - 1);
    if (!v3)
    {
        v5 = this->model.scale[0];
        v6 = r_u16(((uint32)((a1 + 502))));
        w_u16(((uint32)((a1 + 496))), v4);
        this->model.scale[0] = (v5 + v6);
    }
    v7 = r_u16(((uint32)((a1 + 498))));
    v3 = (v7 == 0);
    v8 = (v7 - 1);
    if (!v3)
    {
        v9 = this->model.scale[1];
        v10 = r_u16(((uint32)((a1 + 504))));
        w_u16(((uint32)((a1 + 498))), v8);
        this->model.scale[1] = (v9 + v10);
    }
    v11 = r_u16(((uint32)((a1 + 500))));
    v3 = (v11 == 0);
    v12 = (v11 - 1);
    if (!v3)
    {
        v13 = this->model.scale[2];
        v14 = r_u16(((uint32)((a1 + 506))));
        w_u16(((uint32)((a1 + 500))), v12);
        this->model.scale[2] = (v13 + v14);
    }
    motion_distance = r_u32(0x800A71CC);
    v15 = r_u32(0x800A71D0);
    v16 = r_u32(0x800A71D4);
    motion.acceleration[0] = motion_distance;
    motion.acceleration[1] = v15;
    motion.acceleration[2] = v16;
    if (!(r_u32(((uint32)((a1 + 512))))))
        w_u32(a1 + 516, r_u32(a1 + 516) + 1);
    v17 = this->activation_flags;
    w_u16(((uint32)((a1 + 388))), v17);
    if (((v17 & 1) != 0))
    {
        v18 = r_u8(((uint32)((a1 + 384))));
        this->activation_flags = (v17 & 0xFFFE);
        w_u8(((uint32)((a1 + 384))), (v18 + 1));
    }
    if (r_u8(((uint32)((a1 + 380)))))
    {
        v19 = r_u16(((uint32)((a1 + 476))));
        v3 = (v19 == 0);
        v20 = (v19 - 1);
        if (v3)
            sub_8004BE90(a1, r_u32(((uint32)((a1 + 400)))));
        else
            w_u16(((uint32)((a1 + 476))), v20);
    }
    else
    {
        /* Original nullsub_20 has no side effects */
    }
    if (((r_u32(((uint32)((a1 + 396)))) & 1) == 0))
    {
        if ((r_u16(((uint32)((a1 + 440)))) != 1))
            goto LABEL_64;
        v44 = (sint32)(0 - this->model.angles[2]);
        if ((sint16)v44 < -2048)
            v44 += 4096;
        v45 = (sint32)((uint32)v44 << 16);
        if ((((short)(v44)) >= 2049))
            v45 = (sint32)(((uint32)v44 - 4096) << 16);
        v46 = (v45 >> 16);
        v3 = (v46 != 0);
        v47 = (v46 >> 3);
        if (v3)
        {
            motion.angular_acceleration[2] = v47;
            goto LABEL_64;
        }
    LABEL_62:
        motion.angular_velocity[2] = 0;

        goto LABEL_64;
    }
    xport_draft_host_sub_8006C3AC_p1(direction, a1 + 4, a1 + 484);
    motion_distance = xport_draft_host_sub_8006BF04_p1(direction);
    if ((sint32)r_u16(a1 + 436) < (sint32)motion_distance)
    {
        v23 = r_u16(((uint32)((a1 + 440))));
        if (!v23)
        {
            xport_draft_host_sub_80066B8C_p1(angles, a1 + 4, a1 + 484);
            BuildDirectionVector(motion.velocity, r_u16(a1 + 436), (const uint16 *)angles);
            goto LABEL_64;
        }
        if ((v23 == 1))
        {
            xport_draft_host_sub_80066B8C_p1(angles, a1 + 4, a1 + 484);
            xport_draft_host_sub_8006C3AC_p1(temporary.words, a1 + 484, a1 + 4);
            v24 = (sint32)DivideScriptedLevelObjectMotion(8160, xport_draft_host_sub_8006BF04_p1(temporary.words));
            if ((((v24 >= -256) && (v24 < 257)) && (((uint32)((v24 - 1))) >= 3)))
            {
                if ((v24 >= 0))
                {
                LABEL_30:

                LABEL_31:
                    temporary.words[0] = ReadScriptedLevelObjectAngleWord(a1 + 16);
                    v27 = this->model.angles[2];
                    temporary.halves[2] = v27;
                    motion_cur_word = (uint32)temporary.words[0];
                    motion_cur_tail = v27;
                    motion_target_word = angles[0];
                    motion_target_tail = angles[1] & 65535;
                    motion_cur.vx = (sint16)motion_cur_word;
                    motion_cur.vy = (sint16)(motion_cur_word >> 16);
                    motion_cur.vz = (sint16)motion_cur_tail;
                    motion_cur.pad = (sint16)(motion_cur_tail >> 16);
                    motion_target.vx = (sint16)motion_target_word;
                    motion_target.vy = (sint16)(motion_target_word >> 16);
                    motion_target.vz = (sint16)motion_target_tail;
                    motion_target.pad = (sint16)(motion_target_tail >> 16);
                    UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, (uint32)(v24 < -256 ? -256 : v24 > 256 ? 256 : (uint32)(v24 - 1) < 3 ? 4 : v24 < 0 && v24 >= -3 ? -4 : v24));
                    v28 = r_u16(((uint32)((a1 + 436))));
                    this->model.angles[0] = 0;
                    BuildDirectionVector(motion.acceleration, v28, (const uint16 *)angles);
                    if (((r_u32(((uint32)((a1 + 396)))) & 0x20) != 0))
                        v29 = ((((angles[0] >> 16) & 65535) - this->model.angles[1]) & 0xFFF);
                    else
                        v29 = 0;
                    v30 = (sint32)((uint32)v29 << 16);
                    if ((((short)(v29)) >= 2049))
                    {
                        v29 -= 4096;
                        v30 = (sint32)((uint32)v29 << 16);
                    }
                    v3 = ((v30 >> 16) < 1025);
                    v31 = ((v30 >> 16) < -1024);
                    if (v3)
                    {
                        v3 = !v31;
                        v33 = (sint32)((uint32)v29 << 16);
                        if (v3)
                            goto LABEL_41;
                        v32 = -1024;
                    }
                    else
                    {
                        v32 = 1024;
                    }
                    v33 = (sint32)((uint32)v32 << 16);
                LABEL_41:
                    v34 = (((-((v33 >> 16)) >> 1) & 0xFFF) - this->model.angles[2]);

                    v35 = v34;
                    if ((((short)(v34)) < -2048))
                        v35 = (v34 + 4096);
                    v36 = (sint32)((uint32)v35 << 16);
                    if ((((short)(v35)) >= 2049))
                        v36 = (sint32)(((uint32)v35 - 4096) << 16);
                    v37 = (v36 >> 16);
                    v3 = (v37 == 0);
                    v38 = (v37 >> 3);
                    if (!v3)
                    {
                        motion.angular_acceleration[2] = v38;
                        goto LABEL_64;
                    }
                    goto LABEL_62;
                }
                if ((v24 < -3))
                    goto LABEL_31;
            }
            goto LABEL_30;
        }
        if ((v23 != 2))
            goto LABEL_64;
        xport_draft_host_sub_80066B8C_p1(angles, a1 + 4, a1 + 484);
        xport_draft_host_sub_8006C3AC_p1(v79, a1 + 484, a1 + 4);
        v39 = (sint32)DivideScriptedLevelObjectMotion(8160, xport_draft_host_sub_8006BF04_p1(v79));
        if ((((v39 >= -256) && (v39 < 257)) && (((uint32)((v39 - 1))) >= 3)))
        {
            if ((v39 >= 0))
            {
            LABEL_54:

                goto LABEL_55;
            }
            if ((v39 < -3))
            {
            LABEL_55:
                temporary.words[0] = ReadScriptedLevelObjectAngleWord(a1 + 16);
                v42 = this->model.angles[2];
                temporary.halves[2] = v42;
                motion_cur_word = (uint32)temporary.words[0];
                motion_cur_tail = v42;
                motion_target_word = angles[0];
                motion_target_tail = angles[1] & 65535;
                motion_cur.vx = (sint16)motion_cur_word;
                motion_cur.vy = (sint16)(motion_cur_word >> 16);
                motion_cur.vz = (sint16)motion_cur_tail;
                motion_cur.pad = (sint16)(motion_cur_tail >> 16);
                motion_target.vx = (sint16)motion_target_word;
                motion_target.vy = (sint16)(motion_target_word >> 16);
                motion_target.vz = (sint16)motion_target_tail;
                motion_target.pad = (sint16)(motion_target_tail >> 16);
                UpdateAngleAcceleration(motion_cur, motion.angular_velocity, motion.angular_acceleration, motion_target, (uint32)(v39 < -256 ? -256 : v39 > 256 ? 256 : (uint32)(v39 - 1) < 3 ? 4 : v39 < 0 && v39 >= -3 ? -4 : v39));
                v43 = r_u16(((uint32)((a1 + 436))));
                this->model.angles[0] = 0;
                BuildDirectionVector(motion.acceleration, v43, (const uint16 *)angles);
                goto LABEL_64;
            }
        }
        goto LABEL_54;
    }
    motion_distance = r_u32(a1 + 484);
    v21 = r_u32(a1 + 488);
    v22 = r_u32(a1 + 492);
    this->model.pos[0] = motion_distance;
    this->model.pos[1] = v21;
    this->model.pos[2] = v22;
    if ((r_u16(((uint32)((a1 + 440)))) < 2))
    {
        motion.velocity[2] = 0;
        motion.velocity[1] = 0;
        motion.velocity[0] = 0;
    }
    w_u32(((uint32)((a1 + 396))), (r_u32(((uint32)((a1 + 396)))) & (~1)));
LABEL_64:
    if (r_u8(((uint32)((a1 + 382)))))
        AddVectors(motion.acceleration, (const uint32 *)psx_addr(0x800A6844, 12));

    IntegrateMotion();
    if (((r_u16(((uint32)((a1 + 440)))) == 2) && visual_lists.players))
    {
        if (((sint32)sub_80066918((a1 + 4), (visual_lists.players + 4)) >= r_u16(((uint32)((a1 + 442))))))
        {
            v55 = r_u32(((uint32)((a1 + 520))));
            w_u32(((uint32)((a1 + 396))), (r_u32(((uint32)((a1 + 396)))) & (~8)));
            if (v55)
            {
                ::StopSoundVoices(v55);
                w_u32(((uint32)((a1 + 520))), 0);
            }
        }
        else
        {
            if (((r_u32(((uint32)((a1 + 396)))) & 8) == 0))
                sub_80064A08((r_u32(((uint32)(((4 * this->record_idx) + r_u32(0x800FF624))))) + 6));
            motion_distance = r_u32(a1 + 396);
            v48 = r_u32(a1 + 520);
            w_u32(a1 + 396, motion_distance | 8);
            if (!v48)
                w_u32(((uint32)((a1 + 520))), sub_80069DF0(11, 0x2000, 0));
            v49 = 0;
            DamageScriptedLevelObjectPlayer(visual_lists.players);
            while (v49 < (sint32)RandomRange(4))
            {
                v50 = RandomRange(4096);
                v51 = RandomRange((r_u16(((uint32)((a1 + 442)))) >> 1));
                trig_angle = v50 & 0xFFF;
                direction[0] = (sint32)((uint32)rsin(trig_angle) * (uint32)v51);
                v53 = (sint32)((uint32)rcos(trig_angle) * (uint32)v51);
                direction[1] = 0;
                direction[2] = v53;
                xport_draft_host_sub_8006C0B8_p1(direction, visual_lists.players + 4);
                direction[1] = this->model.pos[1];
                v54 = sub_80032DC0(120);
                if (v54)
                    v54 = xport_draft_host_sub_80035478_p2(v54, &direction[0], 0, 512, 1, 1, 0xFFFFFFFF);
                w_u16(((uint32)((v54 + 64))), 64);
                ++v49;
            }
        }
    }
    v56 = r_u32(((uint32)((a1 + 524))));
    if (((v56 != -1) && !RemainderScriptedLevelObjectMotion(r_u32(0x800FF2F0), (uint32)v56)))
        sub_80069FAC(r_u32(a1 + 520), a1 + 4, 0);
    result = r_u32(a1 + 396) & 4;
    w_u32(a1 + 512, 0);
    if (result)
    {
        uint32 player = visual_lists.players;
        result = player;
        if (player)
        {
            uint32 px = r_u32(player + 4), py = r_u32(player + 8), pz = r_u32(player + 12);
            uint32 x = this->model.pos[0], rx = r_u32(a1 + 528), z, rz, y, ry;
            result = x - rx;
            if (!((sint32)px < (sint32)(x + rx)))
                return result;
            result = (sint32)(x - rx) < (sint32)px;
            if (!result)
                return result;
            z = this->model.pos[2];
            rz = r_u32(a1 + 536);
            result = z - rz;
            if (!((sint32)pz < (sint32)(z + rz)))
                return result;
            result = (sint32)(z - rz) < (sint32)pz;
            if (!result)
                return result;
            y = this->model.pos[1];
            ry = r_u32(a1 + 532);
            result = y - ry;
            if (!((sint32)py < (sint32)(y + ry)))
                return result;
            result = (sint32)(y - ry) < (sint32)py;
            if (result)
            {
                uint32 vx = motion.velocity[0], vz = motion.velocity[2];
                uint32 ax = (sint32)vx < 0 ? 0 - vx : vx;
                uint32 az = (sint32)vz < 0 ? 0 - vz : vz;
                result = az;
                if ((sint32)az < (sint32)ax)
                {
                    result = (sint32)vx > 0 ? x + rx + 0x40000 : x - rx - 0x40000;
                    w_u32(visual_lists.players + 4, result);
                }
                else if ((sint32)az > 0)
                {
                    result = (sint32)vz > 0 ? z + rz + 0x40000 : z - rz - 0x40000;
                    w_u32(visual_lists.players + 12, result);
                }
            }
        }
    }
    return result;
}

uint32 ScriptedLevelObject::Damage(uint32 damage, const sint32 *, uint32)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 count, enabled, remaining, table, receiver, target, sound;
    sint32 health;
    FUNCTION_MARKER(0x8003C490, "SLUS_003.73");
    count = r_u8(object + 383);
    enabled = r_u16(object + 438);
    w_u8(object + 383, count + 1);
    if (enabled)
    {
        health = (sint16)r_u16(object + 218);
        remaining = r_u16(object + 218) - damage;
        if (health > 0)
        {
            w_u16(object + 218, remaining);
            if ((sint32)(remaining << 16) > 0)
                SetColor(3, 255, 255, 255);
            else
            {
                table = r_u32(object + 68);
                receiver = object + (uint32)(sint32)(sint16)r_u16(table + 16);
                target = r_u32(table + 20);
                apocalypse_object_virtual20(target, receiver);
                sound = RandomRange(2) + 1;
                PlayPositionedSound((sint32)sound, (const uint32 *)this->model.pos, 0);
                FragmentLevelObject(&model, 0, 1);
                this->model.flags = this->model.flags | 1;
            }
        }
    }
    return 1;
}

uint32 ScriptedLevelObject::IntegrateMotion()
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 packed, tail, mode, result;
    FUNCTION_MARKER(0x8003C558, "SLUS_003.73");
    packed = ReadScriptedLevelObjectAngleWord(0x800FF5E4);
    tail = r_u16(0x800FF5E8);
    w_u32(object + 164, packed);
    w_u32(object + 164, packed);
    w_u16(object + 168, tail);
    mode = r_u16(object + 440);
    if (mode)
    {
        result = ((mode - 1) & 65535) < 2;
        if (!result)
            return result;
    }
    AddVectors(motion.velocity, motion.acceleration);
    if (mode)
        DampVector(motion.velocity, motion.drag);
    SnapSmallVector(motion.velocity);
    AddVectors((uint32 *)this->model.pos, motion.velocity);
    AddAngleVector((uint16 *)this->model.angles, motion.angular_velocity);
    sub_8006C624(object + 16);
    AddAngleVector(motion.angular_velocity, motion.angular_acceleration);
    if (mode)
        DampAngles(motion.angular_velocity, motion.angular_drag);
    return SnapSmallAngles(motion.angular_velocity);
}

uint32 ScriptedLevelObject::SetDestination(const uint32 *position)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 result = 0, axis, first, second, coordinates[3];
    FUNCTION_MARKER(0x8003C684, "SLUS_003.73");
    for (axis = 0; axis < 3; ++axis)
    {
        first = r_u32(object + 484 + axis * 4);
        second = ReadScriptedLevelObjectTargetWord(position, axis);
        if (first != second)
        {
            result = 1;
            break;
        }
    }
    if (result)
    {
        coordinates[0] = ReadScriptedLevelObjectTargetWord(position, 0);
        coordinates[1] = ReadScriptedLevelObjectTargetWord(position, 1);
        coordinates[2] = ReadScriptedLevelObjectTargetWord(position, 2);
        w_u32(object + 484, coordinates[0]);
        w_u32(object + 488, coordinates[1]);
        w_u32(object + 492, coordinates[2]);
        result = r_u32(object + 396) | 1;
        w_u32(object + 396, result);
    }
    return result;
}

uint32 ScriptedLevelObject::ActivateContact()
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8003C79C, "SLUS_003.73");
    w_u32(object + 512, 1);
    w_u32(object + 516, 0);
    return 1;
}

uint32 ScriptedLevelObject::ExecuteCommand(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8003C7B4, "SLUS_003.73");
    uint32 value, duration, axis, step, cursor, position[3], shift = 12;
    uint32 sound, table, player, receiver, target, x, y, z, flags;
    command = (uint16)command;
    switch (command)
    {
        case 0x4303:
            w_u16(object + 508, ReadScriptedLevelObjectStreamHalf(object));
            return 1;
        case 0x4305:
            value = ReadScriptedLevelObjectStreamHalf(object);
            this->model.flags = value ? this->model.flags | 8 : this->model.flags & 0xFFF7;
            return 1;
        case 0x4304:
        case 0x4306:
        case 0x4307:
        case 0x4308:
            flags = this->model.flags;
            if (!(flags & 0x200))
            {
                this->model.flags = flags | 0x200;
                this->model.scale[2] = 4096;
                this->model.scale[1] = 4096;
                this->model.scale[0] = 4096;
            }
            value = ReadScriptedLevelObjectStreamHalf(object);
            duration = ReadScriptedLevelObjectStreamHalf(object);
            axis = command == 0x4304 ? 0 : command - 0x4306;
            w_u16(object + 496 + axis * 2, duration);
            if (command == 0x4304)
            {
                w_u16(object + 500, duration);
                w_u16(object + 498, duration);
            }
            step = (uint32)(((sint32)(value << 16) >> 4) / (sint32)(duration ? 100 * r_u16(object + 496 + axis * 2) : 100));
            w_u16(object + 502 + axis * 2, step);
            if (command == 0x4304)
            {
                w_u16(object + 506, step);
                w_u16(object + 504, step);
                x = this->model.scale[0];
                step = r_u16(object + 502);
                y = r_u16(object + 504);
                z = r_u16(object + 506);
                this->model.scale[0] = x + step;
                x = this->model.scale[1];
                value = this->model.scale[2];
                this->model.scale[1] = x + y;
                this->model.scale[2] = value + z;
            }
            else
                this->model.scale[axis] = this->model.scale[axis] + r_u16(object + 502 + axis * 2);
            return 1;
        case 0x4507:
            sound = r_u32(object + 520);
            if (sound)
                StopSoundVoices(sound);
            value = ReadScriptedLevelObjectStreamHalf(object);
            w_u32(object + 520, PlaySoundEffect(value, 8192, 0));
            w_u32(object + 524, 0xFFFFFFFF);
            return 1;
        case 0x4508:
            sound = r_u32(object + 520);
            if (sound)
                StopSoundVoices(sound);
            value = ReadScriptedLevelObjectStreamHalf(object);
            duration = ReadScriptedLevelObjectStreamHalf(object);
            w_u32(object + 524, duration);
            w_u32(object + 520, PlayPositionedSound(value, (const uint32 *)this->model.pos, 0));
            return 1;
        case 0x4509:
            sound = r_u32(object + 520);
            if (sound)
                StopSoundVoices(sound);
            w_u32(object + 520, 0);
            return 1;
        case 0x4300:
            if (r_u32(object + 512))
                return 1;
            w_u32(object + 400, r_u32(object + 400) - 2);
            return 0;
        case 0x4301:
            if (static_cast<Player *>(visual_lists.players->owner)->ground.GetObject() == &model)
                static_cast<Player *>(visual_lists.players->owner)->ground.Reset();
            value = this->model.flags;
            table = r_u32(object + 68);
            this->model.flags = value | 1;
            apocalypse_object_virtual20(r_u32(table + 20), object + (uint32)(sint32)(sint16)r_u16(table + 16));
            FragmentLevelObject(&model, 0, 0);
            return 0;
        case 0x4302:
            cursor = r_u32(object + 400);
            x = r_u16(cursor);
            w_u32(object + 400, cursor + 2);
            y = r_u16(cursor + 2);
            w_u32(object + 400, cursor + 4);
            z = r_u16(cursor + 4);
            flags = r_u32(object + 396);
            w_u32(object + 400, cursor + 6);
            w_u32(object + 396, flags | 4);
            w_u32(object + 528, x << 11);
            w_u32(object + 532, y << 11);
            w_u32(object + 536, z << 11);
            return 1;
        case 0x4220:
        case 0x4222:
            cursor = (r_u32(object + 400) + 3) & 0xFFFFFFFC;
            position[0] = r_u32(cursor);
            position[1] = r_u32(cursor + 4);
            position[2] = r_u32(cursor + 8);
            ShiftVectorLeft(position, &shift);
            if (command == 0x4222)
                AddVectors(position, (const uint32 *)this->model.pos);
            w_u32(object + 400, cursor + 12);
            SetDestination(position);
            return 1;
        case 0x4221:
            value = ReadScriptedLevelObjectStreamHalf(object);
            if (value & 0x2000)
            {
                table = r_u32(object + 68);
                receiver = object + (uint32)(sint32)(sint16)r_u16(table + 80);
                target = r_u32(table + 84);
                value = DispatchObjectParameterGetter(target, receiver, value);
            }
            ReadRecordPosition(position, (uint16)value);
            SetDestination(position);
            return 1;
        case 0x4205:
            table = r_u32(object + 68);
            apocalypse_object_virtual20(r_u32(table + 20), object + (uint32)(sint32)(sint16)r_u16(table + 16));
            return 0;
        default:
            return Enemy::ExecuteCommand(command);
    }
}

uint32 ScriptedLevelObject::SetScriptParameters(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    uint32 cursor, value, bank, flags, first, second, result, destination;
    FUNCTION_MARKER(0x8003CF3C, "SLUS_003.73");
    command &= 65535;
    switch (command)
    {
        case 0x2123:
            cursor = r_u32(object + 400);
            value = r_u8(cursor);
            result = cursor + 2;
            w_u32(object + 400, result);
            w_u8(object + 382, value);
            return result;
        case 0x2124:
            cursor = r_u32(object + 400);
            value = r_u16(cursor);
            result = cursor + 2;
            w_u32(object + 400, result);
            this->model.model_idx = value;
            return result;
        case 0x212F:
            cursor = r_u32(object + 400);
            bank = this->model.bank;
            cursor = (cursor + 3) & 0xFFFFFFFC;
            value = r_u32(cursor);
            value = FindModelResourceIndex(value, bank);
            this->model.model_idx = value;
            flags = this->model.flags;
            w_u32(object + 400, cursor + 4);
            result = flags & 65534;
            this->model.flags = result;
            return result;
        case 0x2134:
            first = ReadScriptValue();
            second = ReadScriptValue();
            result = ReadScriptValue();
            first = (uint32)((sint32)(first << 16) >> 4);
            second = (uint32)((sint32)(second << 16) >> 4);
            result = (uint32)((sint32)(result << 16) >> 4);
            motion.velocity[0] = first;
            motion.velocity[1] = second;
            motion.velocity[2] = result;
            return result;
        case 0x2127:
        case 0x2128:
        case 0x2137:
            first = ReadScriptValue();
            second = ReadScriptValue();
            result = ReadScriptValue();
            destination = object + (command == 0x2127 ? 132 : command == 0x2128 ? 138 : 16);
            w_u16(destination, first);
            w_u16(destination + 2, second);
            w_u16(destination + 4, result);
            return result;
        default:
            return Enemy::ReadParameters(command);
    }
}

uint32 ScriptedLevelObject::ResolveParameter(uint32 command)
{
    const uint32 object = NativeObjectGuestAddress(this);
    FUNCTION_MARKER(0x8003D0F0, "SLUS_003.73");
    command &= 65535;
    if (command == 0x2200)
        return r_u32(object + 516);
    return (uint32)(sint32)(sint16)Enemy::ResolveParameter(command);
}



extern "C" uint32 sub_8003BAE0(uint32 object, uint32 reason)
{
    FUNCTION_MARKER(0x8003BAE0, "SLUS_003.73");
    return ScriptedLevelObject::Attach(object).ScriptedLevelObject::Destroy(reason);
}

extern "C" uint32 sub_8003C490(uint32 object, uint32 damage)
{
    FUNCTION_MARKER(0x8003C490, "SLUS_003.73");
    return ScriptedLevelObject::Attach(object).ScriptedLevelObject::Damage(damage, 0, 0);
}

extern "C" uint32 sub_8003BBD8(uint32 object)
{
    FUNCTION_MARKER(0x8003BBD8, "SLUS_003.73");
    return ScriptedLevelObject::Attach(object).ScriptedLevelObject::Update();
}





extern "C" uint32 apocalypse_object_target_native(uint32 object, const void *position)
{
    return ScriptedLevelObject::Attach(object).ScriptedLevelObject::SetDestination((const uint32 *)position);
}

extern "C" uint32 sub_8003C7B4(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x8003C7B4, "SLUS_003.73");
    return ScriptedLevelObject::Attach(object).ScriptedLevelObject::ExecuteCommand(command);
}

extern "C" uint32 sub_8003CF3C(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x8003CF3C, "SLUS_003.73");
    return ScriptedLevelObject::Attach(object).ScriptedLevelObject::SetScriptParameters(command);
}

extern "C" uint32 sub_8003D0F0(uint32 object, uint32 command)
{
    FUNCTION_MARKER(0x8003D0F0, "SLUS_003.73");
    return ScriptedLevelObject::Attach(object).ScriptedLevelObject::ResolveParameter(command);
}

extern "C" void sub_8003BB60(uint32 object)
{
    FUNCTION_MARKER(0x8003BB60, "SLUS_003.73");
    ScriptedLevelObject::Attach(object).ScriptedLevelObject::ApplyPlayerDamage();
}

extern "C" uint32 sub_8003C79C(uint32 object)
{
    FUNCTION_MARKER(0x8003C79C, "SLUS_003.73");
    return ScriptedLevelObject::Attach(object).ScriptedLevelObject::ActivateContact();
}

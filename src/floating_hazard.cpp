#include <new>

#include "collision_ray.h"
#include "game_heap.h"
#include "level_session.h"
#include "resource_script.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "fire_cycle.h"
#include "audio_session.h"
#include "global.h"
#include "effect_lifecycle.h"
#include "effect_splash.h"
#include "model_bank.h"
#include "collision.h"
#include "object_damage.h"
#include "ring_burst_effect.h"
#include "game_object_allocation.h"
#include "debris.h"
#include "floating_hazard.h"
#include "textured_quad.h"
#include "record_data.h"
#include "enemy.h"
#include "effect_object.h"

FloatingHazard::FloatingHazard(const RecordCursor &rec_data, uint16 idx) : Enemy(), bob_phase(0), bob_offset(0)
{
    RecordDataBorrow rec_borrow;

    FUNCTION_MARKER(0x8005A630, "SLUS_003.73");
    SetModelBank(GetRecModelBankName());
    model.model_idx = ModelBank::FindResourceIdx(0xB9E8D489, GetRecModelBankSlot());
    model.flags = (model.flags & 0xFFFD) | 8;
    AddToList(visual_lists.actors);
    col_radius = 64;
    health = 20;
    kind = 9;
    RecordCursor angles(ReadPos(rec_data));
    RecordCursor script(ReadAngles(angles));
    script_cursor = script;
    rec_idx = idx;
    motion.drag[0] = 2;
    motion.drag[1] = 2;
    motion.drag[2] = 2;
    motion.angular_drag[0] = 16;
    motion.angular_drag[1] = 16;
    motion.angular_drag[2] = 16;
    ai.parameters[0] = 12;
    ai.parameters[1] = 256;
    ai.parameters[2] = 20;
    control.script_active = 1;
    ExecuteScript(script_cursor);
}

uint32 FloatingHazard::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8005A74C, "SLUS_003.73");
    RemoveFromList(visual_lists.actors);
    result = Enemy::Destroy(0);
    if (reason & 1)
        result = ReleaseStorage();
    return result;
}

uint32 FloatingHazard::Damage(uint32 damage, const Vector3 *, uint32)
{
    FUNCTION_MARKER(0x8005A7B0, "SLUS_003.73");
    if (health > 0)
    {
        health -= damage;
        if (health <= 0)
        {
            Explode();
            Finish();
        }
    }
    return 1;
}

uint32 FloatingHazard::Update()
{
    RecordDataBorrow rec_borrow;
    uint32 val, damage, distance;
    uint16 flags;
    ShortVector3 angles;
    FUNCTION_MARKER(0x8005A860, "SLUS_003.73");
    if (!(uint32)motion.velocity.y)
        model.pos.y = (uint32)model.pos.y - bob_offset;
    flags = activation_flags;
    control.prev_activation_flags = flags;
    if (flags & 1)
    {
        val = control.activation_count;
        activation_flags = flags & 0xFFFE;
        control.activation_count = val + 1;
    }
    if (control.script_active)
    {
        val = control.script_delay;
        if (val)
            control.script_delay = val - 1;
        else
            ExecuteScript(script_cursor);
    }
    else
        UpdateWaterHeight();
    if (visual_lists.players)
    {
        val = ai.parameters[1];
        distance = player_distance;
        if (distance < val)
        {
            Explode();
            damage = ScaleDamageForDifficulty(ai.parameters[2]);
            VisualObject *damage_target = static_cast<VisualObject *>(visual_lists.players->owner);
            damage_target->Damage(damage, &zero_vector, 0);
            Finish();
        }
    }
    if (control.flags & 1)
    {
        sint32 dst_distance = model.pos.Distance(dst);
        if (dst_distance < 129)
            control.flags = control.flags & ~1;
        else
        {
            angles.SetDir(model.pos, dst);
            motion.acceleration.SetDir(ai.parameters[0], angles);
        }
    }
    if (!RandomRange(64))
    {
        val = RandomRange(256) - 128;
        motion.angular_velocity.x = 0;
        motion.angular_velocity.y = val;
        motion.angular_velocity.z = 0;
    }
    if (!(uint32)motion.velocity.y)
    {
        bob_phase = (bob_phase + RandomRange(512)) & 4095;
        bob_offset = rsin(bob_phase) * 32;
        model.pos.y = (uint32)model.pos.y + bob_offset;
    }
    return IntegrateMotion();
}

void FloatingHazard::UpdateWaterHeight()
{
    FUNCTION_MARKER(0x8005AADC, "SLUS_003.73");
    if (ai.state || ai.phase > 1)
        return;
    if (!ai.phase)
    {
        motion.acceleration.y = 24576;
        ++ai.phase;
    }
    if (model.pos.y > level_environment.water_y)
    {
        motion.acceleration.y = 0;
        motion.velocity.y = 0;
        model.pos.y = level_environment.water_y;
        ++ai.phase;
    }
}

uint32 FloatingHazard::ExecuteCommand(uint16 command)
{
    RecordDataBorrow rec_borrow;
    RecordCursor cursor;
    FUNCTION_MARKER(0x8005AB58, "SLUS_003.73");
    if (command != 0x4221)
        return Enemy::ExecuteCommand(command);
    cursor = script_cursor;
    uint16 rec = cursor.Read16();
    script_cursor = cursor + 2;
    if (rec & 0x2000)
        rec = ResolveParameter(rec);
    ReadRecPos(&dst, rec);
    control.flags |= 1;
    return 1;
}

void FloatingHazard::Explode()
{
    RingBurstEffect::SpawnExplosion(&model.pos, 96, 96, 128, 0, 4, 1, 100);
    PlayPositionedSound(RandomRange(2) + 1, &model.pos, 0);
}

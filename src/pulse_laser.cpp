#include <new>
#include <stdlib.h>

#include "collision_ray.h"
#include "visual_object.h"
#include "game_tuning.h"
#include "game_tick.h"
#include "object_initializers.h"
#include "effect_object.h"
#include "game_diagnostics.h"
#include "weapon_type.h"
#include "weapon_node.h"
#include "pulse_laser.h"
#include "angle_math.h"
#include "game_object_allocation.h"
#include "tracking_projectile.h"
#include "projectile_line.h"
#include "line_projectile.h"
#include "launched_projectile.h"
#include "ballistic_projectile.h"
#include "beam_segments.h"
#include "ring_burst_effect.h"
#include "global.h"
#include "collision.h"
#include "object_damage.h"
#include "effect_lifecycle.h"
#include "game_heap.h"
#include "audio_voice.h"
#include "audio_session.h"
#include "spiral_ray_weapon.h"
#include "machine_gun.h"
#include "flamethrower.h"

PulseLaser::PulseLaser(ModelInstance **list) : WeaponNode(list, 0), fire_delay(0), fire_timer(0)
{
    FUNCTION_MARKER(0x800252CC, "SLUS_003.73");
    type = WPN_PULSE_LASER;
    fire_delay = 3;
}

uint32 PulseLaser::Destroy(uint32 reason)
{
    FUNCTION_MARKER(0x80025314, "SLUS_003.73");
    if (sound)
        StopSoundVoices(sound);
    sound = 0;
    WeaponNode::Destroy(0);
    return reason & 1 ? ReleaseStorage() : 0;
}

uint32 PulseLaser::Fire(const Vector3 &input_pos, const ShortVector3 &input_angles)
{
    FUNCTION_MARKER(0x80025380, "SLUS_003.73");
    if (!sound)
        sound = PlaySoundEffect(30, 8192, 0);
    if (fire_timer)
    {
        --fire_timer;
        return 0;
    }
    GameHeapBlock *projectile = EffectObject::AllocateStorage(268);
    if (projectile)
        new (projectile) LineProjectile(&input_pos, &input_angles, 500, damage, owner_list, 0, 130, 0, 1);
    GameHeapBlock *particle = EffectObject::AllocateStorage(124);
    if (particle)
        new (particle) RingBurstEffect(&input_pos, 5, 255, 255, 255, 2, 0, 100, 0, 20, 20, 0, 0, 14, 0, 10, 20, 0, 1);
    ConsumeAmmo(20);
    if (ammo < 20)
        ammo = 0;
    fire_timer = fire_delay;
    return 1;
}

uint32 PulseLaser::Update(const Vector3 *)
{
    FUNCTION_MARKER(0x800254E8, "SLUS_003.73");
    if (fire_timer)
        --fire_timer;
    if (sound)
        StopSoundVoices(sound);
    sound = 0;
    return 0;
}

uint32 PulseLaser::ReadLight(uint8 *r, uint8 *g, uint8 *b)
{
    FUNCTION_MARKER(0x80025538, "SLUS_003.73");
    if (!fire_timer)
        return 0;
    sint32 elapsed = fire_delay - fire_timer;
    if (elapsed >= 3)
        return 0;
    *r = 0;
    *g = 255;
    *b = 0;
    return 1;
}

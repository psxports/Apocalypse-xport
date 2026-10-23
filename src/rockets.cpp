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
#include "rockets.h"
#include "angle_math.h"
#include "game_object_allocation.h"
#include "tracking_projectile.h"
#include "projectile_line.h"
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

Rockets::Rockets(ModelInstance **list) : WeaponNode(list, 0), fire_delay(0), fire_timer(0)
{
    FUNCTION_MARKER(0x8002B4EC, "SLUS_003.73");
    type = WPN_ROCKETS;
    fire_delay = 15;
}

uint32 Rockets::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8002B534, "SLUS_003.73");
    result = WeaponNode::Destroy(0);
    if (reason & 1)
        result = ReleaseStorage();
    return result;
}

uint32 Rockets::Fire(const Vector3 &input_pos, const ShortVector3 &input_angles)
{
    FUNCTION_MARKER(0x8002B588, "SLUS_003.73");
    if (fire_timer)
    {
        --fire_timer;
        return 0;
    }
    GameHeapBlock *projectile = AllocateZeroedObject(380);
    if (projectile)
        new (projectile) BallisticProjectile(&input_pos, &input_angles, damage, 256, &visual_lists.actors);
    ConsumeAmmo(50);
    if (ammo < 50)
        ammo = 0;
    fire_timer = fire_delay;
    return 1;
}

uint32 Rockets::Update(const Vector3 *)
{
    FUNCTION_MARKER(0x8002B640, "SLUS_003.73");
    if (fire_timer)
        return --fire_timer;
    return 0xFFFFFFFF;
}

uint32 Rockets::ReadLight(uint8 *r, uint8 *g, uint8 *b)
{
    FUNCTION_MARKER(0x8002B65C, "SLUS_003.73");
    if (!fire_timer)
        return 0;
    sint32 elapsed = fire_delay - fire_timer;
    if (elapsed >= 5)
        return 0;
    *r = 255;
    *g = 255;
    *b = 0;
    return 1;
}

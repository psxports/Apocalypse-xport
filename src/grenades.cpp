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
#include "grenades.h"
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

Grenades::Grenades(ModelInstance **list) : WeaponNode(list, 0), fire_delay(0), fire_timer(0)
{
    FUNCTION_MARKER(0x8002A5C0, "SLUS_003.73");
    type = WPN_GRENADES;
    fire_delay = 15;
}

uint32 Grenades::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x8002A608, "SLUS_003.73");
    result = WeaponNode::Destroy(0);
    if (reason & 1)
        result = ReleaseStorage();
    return result;
}

uint32 Grenades::Fire(const Vector3 &input_pos, const ShortVector3 &input_angles)
{
    FUNCTION_MARKER(0x8002A65C, "SLUS_003.73");
    if (fire_timer)
    {
        --fire_timer;
        return 0;
    }
    PlaySoundEffect(23, 8192, 0);
    Vector3 pos = input_pos;
    OffsetByAngles(pos, input_angles);
    ShortVector3 angles = input_angles;
    angles.x = (uint16)angles.x - (420);
    GameHeapBlock *projectile = AllocateZeroedObject(400);
    if (projectile)
        new (projectile) LaunchedProjectile(pos, angles, 100, damage, GetGameTuning().grenade_radius, &visual_lists.actors, 0, 0);
    ConsumeAmmo(66);
    if (ammo < 66)
        ammo = 0;
    fire_timer = fire_delay;
    return 1;
}

uint32 Grenades::Update(const Vector3 *)
{
    FUNCTION_MARKER(0x8002A798, "SLUS_003.73");
    if (fire_timer)
        return --fire_timer;
    return 0xFFFFFFFF;
}

uint32 Grenades::ReadLight(uint8 *r, uint8 *g, uint8 *b)
{
    FUNCTION_MARKER(0x8002A7B4, "SLUS_003.73");
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

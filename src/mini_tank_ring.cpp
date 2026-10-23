#include <new>
#include <stdlib.h>
#include <string.h>

#include "collision_ray.h"
#include "game_heap.h"
#include "level_session.h"
#include "game_tick.h"
#include "record_data.h"
#include "collision.h"
#include "world_gravity.h"
#include "object_initializers.h"
#include "visual_object.h"
#include "object_damage.h"
#include "model_animation.h"
#include "model_attachment_data.h"
#include "game_diagnostics.h"
#include "player.h"
#include "explosion_effects.h"
#include "ring_burst_effect.h"
#include "mini_tank.h"
#include "mini_tank_ring.h"
#include "game_object_allocation.h"
#include "audio_voice.h"
#include "effect_lifecycle.h"
#include "global.h"
#include "model_attachment.h"
#include "angle_math.h"
#include "oscillating_projectile.h"
#include "sprite_trail.h"
#include "audio_session.h"
#include "enemy.h"
#include "ring_effect.h"
#include "mesh_ring_effect.h"
#include "effect_object.h"

void MiniTankRing::Update()
{
    FUNCTION_MARKER(0x80075E7C, "SLUS_003.73");
    ++age;
    if (lifetime < age)
    {
        Finish();
        return;
    }
    pos.Add(velocity);
    SetGeometry(&pos, radius + radius_step, expansion + width_step);
    sint32 color = DivideSignedWord((lifetime - age) * 255, lifetime);
    SetColors(color, color, color >> 1);
}

uint32 MiniTankRing::Destroy(uint32 reason)
{
    uint32 result;
    FUNCTION_MARKER(0x80075F38, "SLUS_003.73");
    result = MeshRingEffect::Destroy(0);
    if (reason & 1)
        return ReleaseStorage();
    return result;
}

MiniTankRing::MiniTankRing(const Vector3 &velocity) : MeshRingEffect(12, 0xCCE0187C), age(0), lifetime(0), radius_step(0), width_step(0)
{
    // Original constructor is inlined in MiniTank state and damage handlers
    this->velocity = velocity;
}

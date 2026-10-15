#include "game_sdk_bindings.h"
#include "items.h"
#include "scripted_level_object.h"
#include "level_hazards.h"
extern "C"
{
#include "draft_first_signatures.h"
#include "draft_first_adapters.h"
}
#include "plague_area.h"
#include "death_effects.h"
#include "projectile_line.h"
#include "glow_effect.h"
#include "waving_beam.h"
#include "expanding_ring.h"
#include "effect_splash.h"
#include "effect_launch_explosion.h"
#include "ring_burst_effect.h"
#include "ring_attack.h"
#include "fireball_effects.h"
#include "smoke_emitter.h"
#include "effect_sprite.h"
#include "beam_endpoint.h"
#include "bouncing_point_effect.h"
#include "oscillating_ring.h"
#include "opaque_fragment.h"
#include "fading_particle.h"
#include "sprite_trail.h"
#include "beast_effects.h"
#include "projectile_trail.h"
#include "plague_ground_effects.h"
#include "mini_tank.h"
#include "projectile_base.h"
#include "path_particle_emitter.h"
#include "proximity_ring_emitter.h"
#include "pickup.h"
#include "projectile_oscillating.h"
#include "exp_act_burst.h"
#include "war_effects.h"
#include "panel_particle.h"
#include "textured_fragment.h"
#include "fire_cycle.h"
#include "plague.h"
#include "the_beast.h"
#include "war.h"
#include "death.h"
#include "player.h"
#include <stdio.h>
#include <stdlib.h>

// Native adapters retain adjusted receivers and original callback carriers
static void FailObjectCallback(const char *operation)
{
    GameDiagnostic("TODO Object callback: %s\n", operation);
    abort();
}

uint32 DispatchObjectFinish(uint32 target, uint32 receiver)
{
    switch (target)
    {
        case 0x80062F0C:
            return VisualObject::Attach(receiver).VisualObject::Finish();
        case 0x80020264:
            return Pickup::Attach(receiver).Pickup::Finish();
        case 0x8004B8C8:
            return Enemy::Attach(receiver).Enemy::Finish();
        case 0x8005DD40:
            return Player::Attach(receiver).Player::Finish();
        default:
            GameDiagnostic("Unimplemented object finish %08X receiver %08X\n", target, receiver);
            abort();
    }
}

static void DispatchObjectUpdate(uint32 target, uint32 receiver)
{
    switch (target)
    {
        case 0x8002A138:
            RingAttack::Attach(receiver).RingAttack::Update();
            break;
        case 0x80049D54:
            DeathPersistentRing::Attach(receiver).DeathPersistentRing::Update();
            break;
        case 0x80049E38:
            DeathFadingRing::Attach(receiver).DeathFadingRing::Update();
            break;
        case 0x80047844:
            PlagueArea::Attach(receiver).PlagueArea::Update();
            break;
        case 0x80075E7C:
            MiniTankRing::Attach(receiver).MiniTankRing::Update();
            break;
        case 0x8002E008:
            ExplosionRing::Attach(receiver).ExplosionRing::Update();
            break;
        case 0x8005C47C:
            ImpactRing::Attach(receiver).ImpactRing::Update();
            break;
        case 0x8001FA0C:
            PathParticleEmitter::Attach(receiver).PathParticleEmitter::Update();
            break;
        case 0x80034CF8:
            OscillatingRing::Attach(receiver).OscillatingRing::Update();
            break;
        case 0x8001CA68:
            ProximityRingEmitter::Attach(receiver).ProximityRingEmitter::Update();
            break;
        case 0x8001EA60:
            GlowEffect::Attach(receiver).GlowEffect::Update();
            break;
        case 0x8001ECAC:
            ExpandingGlow::Attach(receiver).ExpandingGlow::Update();
            break;
        case 0x80036FFC:
            OpaqueFragment::Attach(receiver).OpaqueFragment::Update();
            break;
        case 0x800212C8:
            TexturedFragment::Attach(receiver).TexturedFragment::Update();
            break;
        case 0x80036614:
            LoopingEndpointSprite::Attach(receiver).LoopingEndpointSprite::Update();
            break;
        case 0x80036CB0:
            RibbonTrail::Attach(receiver).RibbonTrail::Update();
            break;
        case 0x8001DE24:
            FireCycle::Attach(receiver).FireCycle::Update();
            break;
        case 0x8001116C:
            WavingBeam::Attach(receiver).WavingBeam::Update();
            break;
        case 0x80044BFC:
            PlagueImpactEffect::Attach(receiver).PlagueImpactEffect::Update();
            break;
        case 0x8004B2B8:
            TrailProjectile::Attach(receiver).TrailProjectile::Update();
            break;
        case 0x80044DA8:
            PlagueGroundEffect::Attach(receiver).PlagueGroundEffect::Update();
            break;
        case 0x80040734:
            BeastGroundEffect::Attach(receiver).BeastGroundEffect::Update();
            break;
        case 0x8003D2A8:
            WarImpact::Attach(receiver).WarImpact::Update();
            break;
        case 0x80035C24:
            LaunchExplosionEffect::Attach(receiver).LaunchExplosionEffect::Update();
            break;
        case 0x80036200:
            EndpointBeam::Attach(receiver).EndpointBeam::Update();
            break;
        case 0x80040460:
            BeastOrbitEffect::Attach(receiver).BeastOrbitEffect::Update();
            break;
        case 0x8001EE9C:
            ExpandingRing::Attach(receiver).ExpandingRing::Update();
            break;
        case 0x80025E2C:
            LineProjectile::Attach(receiver).LineProjectile::Update();
            break;
        case 0x8001C598:
            OscillatingProjectile::Attach(receiver).OscillatingProjectile::Update();
            break;
        case 0x8003A118:
        case 0x8003A144:
        case 0x80032004:
            return;
        case 0x800352B0:
            FadingParticle::Attach(receiver).FadingParticle::Update();
            break;
        case 0x80062F0C:
        case 0x80020264:
        case 0x8004B8C8:
        case 0x8005DD40:
            DispatchObjectFinish(target, receiver);
            break;
        case 0x8001D9D0:
            ExpActBurst::Attach(receiver).ExpActBurst::Update();
            break;
        case 0x8001D240:
            RingBurstEffect::Attach(receiver).RingBurstEffect::Update();
            break;
        case 0x8002396C:
            BouncingPointEffect::Attach(receiver).BouncingPointEffect::Update();
            break;
        case 0x800236D4:
            ProjectileTracer::Attach(receiver).ProjectileTracer::Update();
            break;
        case 0x80035A54:
            ParticleSprite::Attach(receiver).ParticleSprite::Update();
            break;
        case 0x8004B570:
            Fireball::Attach(receiver).Fireball::Update();
            break;
        case 0x8004B798:
            Smoke::Attach(receiver).Smoke::Update();
            break;
        case 0x8005C2B8:
            PanelParticle::Attach(receiver).PanelParticle::Update();
            break;
        case 0x800355A0:
            AnimatedSprite::Attach(receiver).AnimatedSprite::Update();
            break;
        case 0x800326E8:
            SmokeEmitter::Attach(receiver).SmokeEmitter::Update();
            break;
        case 0x80035758:
            MovingSprite::Attach(receiver).MovingSprite::Update();
            break;
        default:
            FailObjectCallback("Object virtual slot20 target");
    }
}

static void DispatchGuestObjectDamage(uint32 target, uint32 receiver, uint32 argument, uint32 vector, uint32 reason)
{
    switch (target)
    {
        case 0x800736AC:
        case 0x8004A25C:
        case 0x800747A8:
        case 0x80075930:
        case 0x80058A14:
        case 0x80056C50:
        case 0x80054714:
        case 0x800593CC:
            apocalypse_enemy_damage_cpp(target, receiver, argument, vector, 0, reason);
            break;
        case 0x8004F5C8:
            apocalypse_enemy_damage_cpp(target, receiver, argument, vector, 0, reason);
            break;
        case 0x8005B864:
            Items::Attach(receiver).Items::Damage(argument);
            break;
        case 0x80045B14:
            Plague::Attach(receiver).Plague::Damage(argument, 0, reason);
            break;
        case 0x800422C0:
            TheBeast::Attach(receiver).TheBeast::Damage(argument, 0, reason);
            break;
        case 0x8003E354:
            War::Attach(receiver).War::Damage(argument, 0, reason);
            break;
        case 0x80048428:
            Death::Attach(receiver).Death::Damage(argument, 0, reason);
            break;
        case 0x8005DDF0:
            Player::Attach(receiver).Player::Damage(argument, vector ? (const sint32 *)psx_addr(vector, 12) : 0, reason);
            break;
        case 0x8003C490:
            ScriptedLevelObject::Attach(receiver).ScriptedLevelObject::Damage(argument, 0, 0);
            break;
        case 0x8005A7B0:
            FloatingHazard::Attach(receiver).FloatingHazard::Damage(argument, 0, 0);
            break;
        case 0x8005AF98:
            BallisticHazard::Attach(receiver).BallisticHazard::Damage(argument, 0, 0);
            break;
        default:
            FailObjectCallback("Object virtual slot52 target");
    }
}

extern "C" void apocalypse_object_virtual20(uint32 target, uint32 receiver)
{
    switch (target)
    {
        case 0x80062F0C:
        case 0x80020264:
        case 0x8004B8C8:
        case 0x8005DD40:
            (void)DispatchObjectFinish(target, receiver);
            return;
        default:
            DispatchObjectUpdate(target, receiver);
    }
}

extern "C" void apocalypse_object_virtual52(uint32 target, uint32 receiver, uint32 argument, uint32 vector, uint32 reason)
{
    DispatchGuestObjectDamage(target, receiver, argument, vector, reason);
}

extern "C" void nullsub_22(void)
{
    FUNCTION_MARKER(0x800639A4, "SLUS_003.73");
}

extern "C" void nullsub_20(void)
{
    FUNCTION_MARKER(0x8003C7AC, "SLUS_003.73");
}

extern "C" void nullsub_31(void)
{
}

extern "C" void nullsub_28(void)
{
    FUNCTION_MARKER(0x80032004, "SLUS_003.73");
}

extern "C" void nullsub_29(void)
{
    FUNCTION_MARKER(0x8003200C, "SLUS_003.73");
}

extern "C" void nullsub_27(void)
{
    FUNCTION_MARKER(0x800639AC, "SLUS_003.73");
}

extern "C" void nullsub_19(void)
{
    FUNCTION_MARKER(0x80077194, "SLUS_003.73");
}

extern "C" void nullsub_16(void)
{
    FUNCTION_MARKER(0x8002FF80, "SLUS_003.73");
    /* Original empty routine */
}

extern "C" void nullsub_34(void)
{
    /* Original empty routine */
}

extern "C" void nullsub_24(void)
{
    FUNCTION_MARKER(0x80063994, "SLUS_003.73");
    /* Original empty routine */
}

extern "C" void nullsub_25(void)
{
    ;
}

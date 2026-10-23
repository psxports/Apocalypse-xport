#ifndef WEAPON_TYPE_H
#define WEAPON_TYPE_H

#include "vector3.h"
#include "object.h"
#include "model_instance.h"

extern "C"
{
#include "psx.h"
}

// Original weapon kinds stored at object offset 4
enum WeaponType
{
    WPN_FLAMETHROWER = 1,
    WPN_PULSE_LASER = 2,
    WPN_PARTICLE_BEAM = 3,
    WPN_RIP_LASER = 4,
    WPN_GRENADES = 5,
    WPN_HOMING_MISSILES = 6,
    WPN_ROCKETS = 7,
    WPN_MACHINE_GUN = 8,
    WPN_SPIRAL_RAY = 9
};

class EffectObject;
class RingEffect;
class BeamSegments;

#endif

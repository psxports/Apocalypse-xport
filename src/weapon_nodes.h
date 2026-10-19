#ifndef WEAPON_NODES_H
#define WEAPON_NODES_H

#include "vector3.h"

extern "C"
{
#include "psx.h"
}

// Native service boundary retains the authorized BIOS fail-fast contract
sint32 ReadWeaponLightRandom();
class Object;
uint32 ReleaseWeaponStorage(Object *object);

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

// TODO Recover the original base class name
#include "object.h"
#include "model_instance.h"

class EffectObject;
class RingEffect;
class BeamSegments;

class WeaponNodeBase : public Object
{
public:
    virtual ~WeaponNodeBase()
    {
    }

    virtual uint32 Destroy(uint32 reason);

protected:
    // Native base binding adds no original memory writes
    WeaponNodeBase()
    {
    }
};

class WeaponNode : public WeaponNodeBase
{
public:
    WeaponNode(ModelInstance **list, ModelInstance **secondary_list);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Fire(const uint32 *position, const sint16 *angles);
    // Native cleanup returns zero while timer callbacks retain their carriers
    virtual uint32 Update(const uint32 *position, const sint16 *angles);
    void ClearChild();
    uint32 ConsumeAmmo(uint32 amount);

    uint32 type;
    uint32 fire_active;
    uint32 last_fire_tick;
    uint32 start_tick;
    uint32 child_tick;
    ObjectReference<EffectObject> child;
    uint32 damage;
    ModelInstance **owner_list;
    ModelInstance **secondary_list;
    uint32 sound;
    uint32 ammo;

protected:
    Vector3 muzzle_pos;
};

class HomingMissiles : public WeaponNode
{
public:
    HomingMissiles(ModelInstance **list);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const uint32 *position, const sint16 *angles);

private:
    uint32 fire_delay;
    uint32 fire_timer;
};

class PulseLaser : public WeaponNode
{
public:
    PulseLaser(ModelInstance **list);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const uint32 *position, const sint16 *angles);

private:
    uint32 fire_delay;
    uint32 fire_timer;
};

class ParticleBeam : public WeaponNode
{
public:
    ParticleBeam(ModelInstance **list, uint32 range, uint32 mode, uint32 effect_scale, uint32 sound);
    virtual ~ParticleBeam();
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const uint32 *position, const sint16 *angles);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    uint32 SetColors(uint32 red1, uint32 green1, uint32 blue1, uint32 red2, uint32 green2, uint32 blue2, uint32 red3, uint32 green3, uint32 blue3, uint32 red4, uint32 green4, uint32 blue4);
    void ClearChildren();

private:
    friend class Plague;
    uint32 impact_count;
    uint32 segment_len;
    uint32 trail_mode;
    ObjectReference<BeamSegments> beam[2];
    CVECTOR colors[4];
    uint32 ring_scale;
    uint32 sound_id;
    ObjectReference<RingEffect> ring;

    // Native helper retains original inline damage dispatch
    void DamageTarget(ModelInstance *hit, const Vector3 &direction);
};

class RipLaser : public WeaponNode
{
public:
    RipLaser(ModelInstance **list);
    virtual ~RipLaser();
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const uint32 *position, const sint16 *angles);
    uint32 SetColors(uint32 first_red, uint32 first_green, uint32 first_blue, uint32 second_red, uint32 second_green, uint32 second_blue);
    void ClearChildren();

private:
    uint32 retarget_timer;
    uint32 impact_active;
    uint32 impact_count;
    ObjectReference<BeamSegments> beam[2];
    Vector3 impact_pos;
    CVECTOR colors[2];

    // Native helpers retain original inline call setup
    void DamageTarget(ModelInstance *hit);
};

class Grenades : public WeaponNode
{
public:
    Grenades(ModelInstance **list);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const uint32 *position, const sint16 *angles);

private:
    uint32 fire_delay;
    uint32 fire_timer;
};

class Rockets : public WeaponNode
{
public:
    Rockets(ModelInstance **list);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const uint32 *position, const sint16 *angles);

private:
    uint32 fire_delay;
    uint32 fire_timer;
};

#endif

#ifndef WEAPON_NODES_H
#define WEAPON_NODES_H

extern "C"
{
#include "psx.h"
}

// Native service boundary retains the authorized BIOS fail-fast contract
sint32 ReadWeaponLightRandom();
uint32 ReleaseWeaponStorage(uint32 allocation);
uint32 FireInactiveWeapon();

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
class WeaponNodeBase
{
public:
    virtual ~WeaponNodeBase() {}
    static WeaponNodeBase &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);

protected:
    // Native base binding adds no original memory writes
    WeaponNodeBase() {}
};

class WeaponNode : public WeaponNodeBase
{
public:
    WeaponNode(uint32 list, uint32 mode);
    static WeaponNode &Attach(uint32 object);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Fire(const uint32 *position, const sint16 *angles);
    // Native cleanup returns zero while timer callbacks retain their carriers
    virtual uint32 Update(const sint32 *position, const sint16 *angles);
    void ClearChild();
    uint32 ConsumeAmmo(uint32 amount);

protected:
    // Native helper retains the original paired LWL/LWR angle load
    static uint32 ReadAnglesWord(uint32 address);
};

class HomingMissiles : public WeaponNode
{
public:
    HomingMissiles(uint32 list);
    static HomingMissiles &Attach(uint32 object);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const sint32 *position, const sint16 *angles);

private:
    };

class PulseLaser : public WeaponNode
{
public:
    PulseLaser(uint32 list);
    static PulseLaser &Attach(uint32 object);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const sint32 *position, const sint16 *angles);

private:
    };

class ParticleBeam : public WeaponNode
{
public:
    ParticleBeam(uint32 list, uint32 range, uint32 mode, uint32 effect_scale, uint32 sound);
    static ParticleBeam &Attach(uint32 object);
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const sint32 *position, const sint16 *angles);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    uint32 SetColors(uint32 red1, uint32 green1, uint32 blue1, uint32 red2, uint32 green2, uint32 blue2, uint32 red3, uint32 green3, uint32 blue3, uint32 red4, uint32 green4, uint32 blue4);
    void ClearChildren();

private:
    // Native helper retains original inline damage dispatch
    void DamageTarget(uint32 hit, const uint32 direction[3]);
};

class RipLaser : public WeaponNode
{
public:
    RipLaser(uint32 list);
    static RipLaser &Attach(uint32 object);
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const sint32 *position, const sint16 *angles);
    uint32 SetColors(uint32 first_red, uint32 first_green, uint32 first_blue, uint32 second_red, uint32 second_green, uint32 second_blue);
    void ClearChildren();

private:
    // Native helpers retain original inline call setup
    void DamageTarget(uint32 hit);
};

class Grenades : public WeaponNode
{
public:
    Grenades(uint32 list);
    static Grenades &Attach(uint32 object);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const sint32 *position, const sint16 *angles);

private:
    };

class Rockets : public WeaponNode
{
public:
    Rockets(uint32 list);
    static Rockets &Attach(uint32 object);
    virtual uint32 ReadLight(uint8 *red, uint8 *green, uint8 *blue);
    virtual uint32 Fire(const uint32 *input_position, const sint16 *input_angles);
    virtual uint32 Destroy(uint32 reason);
    virtual uint32 Update(const sint32 *position, const sint16 *angles);

private:
    };

#endif
